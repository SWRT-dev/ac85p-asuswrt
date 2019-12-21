/* perpd_conn.c
** perp: persistent process supervision
** perpd 2.0:  single process scanner/supervisor/controller
** perpd_conn:  ipc routines for perpd
** wcm, 2010.12.28 - 2011.03.18
** ===
*/

#include <stddef.h>
#include <stdint.h>

/* unix: */
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>

/* lasanga: */
#include "buf.h"
#include "cstr.h"
#include "domsock.h"
#include "fd.h"
#include "nfmt.h"
#include "pkt.h"
#include "sysstr.h"
#include "tain.h"
#include "uchar.h"
#include "upak.h"

/* perp: */
#include "perp_common.h"
#include "perpd.h"


static int perpd_pkt_check(uchar_t *pkt, size_t n);

static void do_kill(struct svdef *svdef, int which, int sig, int is_killpg);
static int do_control(struct svdef *svdef, int which, uchar_t cmd, int is_killpg);

static void perpd_conn_exec(struct perpd_conn *client);
static void perpd_conn_exec_control(struct perpd_conn *client);
static void perpd_conn_exec_query(struct perpd_conn *client);
static void perpd_conn_exec_pidyank(struct perpd_conn *client);
static void perpd_conn_exec_reply(struct perpd_conn *client, int err);



/* perpd_pkt_check()
**   inspect first n bytes of pkt read from a client
**   return:
**     1: read complete/ok
**     0: read incomplete
*/
static
int
perpd_pkt_check(pkt_t pkt, size_t n)
{
  /* need at least PKT_HEADER bytes to check pkt_len() bytes: */
  if((n < PKT_HEADER) || (n < pkt_len(pkt)))
      return 0;

  /* else: assume read complete: */
  return 1;
}


/* do_kill()
**   deliver signal sig to pid of svdef->which
**   filter appropriately
*/
static
void
do_kill(struct svdef *svdef, int which, int sig, int is_killpg)
{
  struct subsv  *subsv = &svdef->svpair[which];
  pid_t          pid = subsv->pid;

  /* screen signal from non-existent log: */
  if((which == SUBSV_LOG) && !(svdef->bitflags & SVDEF_FLAG_HASLOG)){
      return;
  }

  /* screen signal if not running: */
  if(!(pid > 0)) return;

  /* direct kill() to process group: */
  if(is_killpg) pid = 0 - pid;

  /* deliver signal if not running reset: */
  if(!(subsv->bitflags & SUBSV_FLAG_ISRESET)){
      if(kill(pid, sig) == -1){
          warn_syserr("failure kill() on ", sysstr_signal(sig),
                      " to service ", svdef->name, " (",
                      (which == SUBSV_MAIN) ? "main)" : "log)");
      }
      return;
  }

  /* filter if running reset: */
  switch(sig){
  case SIGCONT:
  case SIGKILL:
      log_warning("sending signal ", sysstr_signal(sig),
                 " to resetting service ", svdef->name, " (",
                 (which == SUBSV_MAIN) ? "main)" : "log)");
      if(kill(pid, sig) == -1){
          warn_syserr("failure kill() on ", sysstr_signal(sig),
                      " to service ", svdef->name, " (",
                      (which == SUBSV_MAIN) ? "main)" : "log)");
      }
      break;
  default:
      log_warning("dropping signal ", sysstr_signal(sig),
                 " to resetting service ", svdef->name, " (",
                 (which == SUBSV_MAIN) ? "main)" : "log)");
  }

  return;
}


/* do_control()
**   dispatch "cmd" to svdef->which
**   return:
**     0: success, no error
**    -1: unrecognized "cmd" (protocol error)
*/
static
int
do_control(struct svdef *svdef, int which, uchar_t cmd, int is_killpg)
{
  struct subsv  *subsv = &svdef->svpair[which];

  /* handling states FLAG_CULL, FLAG_FAILING:
  **   FLAG_CULL has been screened out prior to entry
  **   FLAG_FAILING means !(pid > 0), so no kill() performed
  */

  switch(cmd){
  case 'D': /* meta "down": both main and log */
      /* note: meta commands ignored on SUBSV_LOG */
      if(which == SUBSV_MAIN){
          do_control(svdef, SUBSV_MAIN, 'd', is_killpg);
          do_control(svdef, SUBSV_LOG, 'd', is_killpg);
      }
      break;
  case 'U': /* meta "up": both main and log */
      /* note: meta commands ignored on SUBSV_LOG */
      if(which == SUBSV_MAIN){
          do_control(svdef, SUBSV_LOG, 'u', is_killpg);
          do_control(svdef, SUBSV_MAIN, 'u', is_killpg);
      }
      break;
  case 'a': /* alarm */
      do_kill(svdef, which, SIGALRM, is_killpg);
      break;
  case 'c': /* continue */
      subsv->bitflags &= ~SUBSV_FLAG_ISPAUSED;
      do_kill(svdef, which, SIGCONT, is_killpg);
      break;
  case 'd': /* faux "down" */
      subsv->bitflags |= SUBSV_FLAG_WANTDOWN;
      if(subsv->pid > 0){
          do_control(svdef, which, 't', is_killpg);
          do_control(svdef, which, 'c', is_killpg);
      }
      break;
  case 'h': /* hup */
      do_kill(svdef, which, SIGHUP, is_killpg);
      break;
  case 'i': /* interrupt */
      do_kill(svdef, which, SIGINT, is_killpg);
      break;
  case 'k': /* kill */
      do_kill(svdef, which, SIGKILL, is_killpg);
      break;
  case 'o': /* faux "once" */
      subsv->bitflags |= SUBSV_FLAG_ISONCE;
      subsv->bitflags &= ~SUBSV_FLAG_WANTDOWN;
      /* bring it up if it is down: */
      if(subsv->pid == 0){
          perpd_svdef_run(svdef, which, SVRUN_START);
      }
      break;
  case 'p': /* pause */
      /* filter pause on resetting service: */
      if((subsv->pid > 0) && !(subsv->bitflags & SUBSV_FLAG_ISRESET)){
          subsv->bitflags |= SUBSV_FLAG_ISPAUSED;
          do_kill(svdef, which, SIGSTOP, is_killpg);
      }
      break;
  case 'q': /* quit */
      do_kill(svdef, which, SIGQUIT, is_killpg);
      break;
  case 't': /* term */
      do_kill(svdef, which, SIGTERM, is_killpg);
      break;
  case 'u': /* faux "up" */
      subsv->bitflags &= ~SUBSV_FLAG_ISONCE;
      subsv->bitflags &= ~SUBSV_FLAG_WANTDOWN;
      /* bring it up if it is down: */
      if(subsv->pid == 0){
          perpd_svdef_run(svdef, which, SVRUN_START);
      }
      break;
  case 'w': /* SIGWINCH */
#ifdef SIGWINCH
      do_kill(svdef, which, SIGWINCH, is_killpg);
#endif
      break;
  case '1': /* usr1 */
      do_kill(svdef, which, SIGUSR1, is_killpg);
      break;
  case '2': /* usr2 */
      do_kill(svdef, which, SIGUSR2, is_killpg);
      break;
  default: /* not recognized */
      return -1;
      break;
  }

  return 0;
}


/* perpd_conn_exec()
**   pkt read is complete:
**     process control packet and execute
**     put reply packet in client buffer
**     set client state to PERPD_CONN_WRITING
*/
static
void
perpd_conn_exec(struct perpd_conn *client)
{
  size_t    n = client->n;

  if((n < PKT_HEADER) ||
     (pkt_proto(client->pkt) != 2) ||
     (pkt_len(client->pkt) != n)){
      perpd_conn_exec_reply(client, EPROTO);
  }

  /* dispatch to exec handler: */
  switch(pkt_type(client->pkt)){
  case 'C': /* control request */
      log_debug("got control request");
      if(n != (size_t)(18 + 3)){
          perpd_conn_exec_reply(client, EPROTO);
      } else {
          perpd_conn_exec_control(client);
      }
      break;
  case 'Q': /* query status */
      if(n != (size_t)(16 + 3)){
          log_debug("status query has bad size!");
          perpd_conn_exec_reply(client, EPROTO);
      } else {
          perpd_conn_exec_query(client);
      }
      break;
  case 'Y': /* pidyank */
      perpd_conn_exec_pidyank(client);
      break;
  default:  /* unknown */
      perpd_conn_exec_reply(client, EPROTO);
  }

  return;
}


/* perpd_conn_exec_control()
**   process 'C' pkt
*/
static
void
perpd_conn_exec_control(struct perpd_conn *client)
{
  uchar_t       *payload = pkt_data(client->pkt);
  dev_t          dev;
  ino_t          ino;
  uchar_t        cmd;
  uchar_t        flags;
  int            is_killpg = 0;
  struct svdef  *svdef;
  int            which = SUBSV_MAIN;
  int            err;

  dev = (dev_t)upak64_unpack(&payload[0]);
  ino = (dev_t)upak64_unpack(&payload[8]);
  cmd = payload[16];
  flags = payload[17];

  svdef = perpd_lookup(dev, ino);
  if(svdef == NULL){
      perpd_conn_exec_reply(client, ENOENT);
      return;
  }

  /* drop control on deactivating service: */
  if(svdef->bitflags & SVDEF_FLAG_CULL){
      /* XXX, select other return value? */
      perpd_conn_exec_reply(client, EBUSY);
      return;
  }

  /* command destined for log control: */
  if(flags & SVCMD_FLAG_LOG){
      which = SUBSV_LOG;
  }

  /* command destined for killpg(): */
  if(flags & SVCMD_FLAG_KILLPG){
      ++is_killpg;
  }
  err = do_control(svdef, which, cmd, is_killpg);
  perpd_conn_exec_reply(client, err ? EPROTO : 0);

  return;
}


static
void
perpd_conn_exec_query(struct perpd_conn *client)
{
  uchar_t       *input = pkt_data(client->pkt);
  dev_t          dev;
  ino_t          ino;
  struct svdef  *svdef;
  uchar_t        buf[PKT_PAYLOAD];

  dev = (dev_t)upak64_unpack(&input[0]);
  ino = (ino_t)upak64_unpack(&input[8]);

  svdef = perpd_lookup(dev, ino);
  if(svdef == NULL){
      perpd_conn_exec_reply(client, ENOTDIR);
      return;
  }

  /* status payload is 66 bytes: */
  buf_WIPE(buf, 66);

  /* perpd: */
  upak32_pack(&buf[0], (uint32_t)my_pid);
  tain_pack(&buf[4], &my_when);

  /* svdef: */
  tain_pack(&buf[16], &svdef->when);
  buf[28] = svdef->bitflags;
  buf[29] = 0;

  /* main: */
  upak32_pack(&buf[30], (uint32_t)svdef->svpair[SUBSV_MAIN].pid);
  tain_pack(&buf[34], &svdef->svpair[SUBSV_MAIN].when);
  buf[46] = svdef->svpair[SUBSV_MAIN].bitflags;
  buf[47] = 0;

  /* log: */
  if(svdef->bitflags & SVDEF_FLAG_HASLOG){
      upak32_pack(&buf[48], (uint32_t)svdef->svpair[SUBSV_LOG].pid);
      tain_pack(&buf[52], &svdef->svpair[SUBSV_LOG].when);
      buf[64] = svdef->svpair[SUBSV_LOG].bitflags;
      buf[65] = 0;
  }

  pkt_load(client->pkt, 2, 'S', buf, 66); 
  client->n = pkt_len(client->pkt);
  client->w = 0;
  client->state = PERPD_CONN_WRITING;  

  return;
}


static
void
perpd_conn_exec_pidyank(struct perpd_conn *client)
{
  /* currently not supported: */
  perpd_conn_exec_reply(client, ENOTSUP);
  return;
}


static
void
perpd_conn_exec_reply(struct perpd_conn *client, int err)
{
  uchar_t   buf[PKT_PAYLOAD];

  upak32_pack(buf, (uint32_t)err);
  pkt_load(client->pkt, 2, 'E', buf, sizeof(uint32_t));
  client->n = pkt_len(client->pkt);
  client->w = 0;
  client->state = PERPD_CONN_WRITING;  

  return;
}


/*
** perpd scope:
*/

void
perpd_conn_reset(struct perpd_conn *client)
{
  client->connfd = -1;
  client->state = PERPD_CONN_CLOSED;
  client->n = 0;
  client->w = 0;
  return;
}


void
perpd_conn_close(struct perpd_conn *client)
{
  if(client->connfd != -1) close(client->connfd);
  perpd_conn_reset(client);
  return;
}


void
perpd_conn_start(struct perpd_conn *client, int connfd)
{
  client->connfd = connfd;
  tain_now(&client->stamp);
  client->state = PERPD_CONN_READING;
  client->n = 0;
  client->w = 0;
  return;
}


void
perpd_conn_checkstale(struct perpd_conn *client, struct tain *now)
{
  tain_t  diff, max_diff = tain_INIT(PERPD_CONNSECS, 0);

  /* assumes clock is strictly monotonic! */
  if(tain_less(&client->stamp, now)){
      tain_minus(&diff, now, &client->stamp);
      if(tain_less(&max_diff, &diff)){
          /* close stale connection: */
          log_warning("closing stale client connection");
          perpd_conn_close(client);
      }
  }

  return;
}


void
perpd_conn_read(struct perpd_conn *client)
{
  size_t    n = client->n;
  ssize_t   r;

  /* pkt_read() from socket, socket is non-blocking: */
  r = pkt_read(client->connfd, client->pkt, n);

  /* handle errors: */
  if(r == -1){
      /* error, close socket, reset client */
      warn_syserr("error reading client");
      perpd_conn_close(client);
      return;
  }else if(r == 0){
      /* eof, close socket, reset client */
      log_debug("client closed connection");
      perpd_conn_close(client);
      return;
  }

  /* else: */
  client->n += r;
  if(perpd_pkt_check(client->pkt, client->n) == 1){
      log_debug("perpd_conn_read() complete");
      /* pkt read is complete:
      **   process control packet and execute
      **   put reply packet in client buffer
      **   set client state to PERPD_CONN_WRITING
      */
      perpd_conn_exec(client);
  } else {
      /* pkt read not yet complete, leave to next poll(): */
      log_debug("perpd_conn_read() to be continued...");
  }

  return;
}


void
perpd_conn_write(struct perpd_conn *client)
{
  int       connfd = client->connfd;
  size_t    n = client->n;  /* total bytes in pkt */
  size_t    w = client->w;  /* bytes previously written */
  ssize_t   r;

  /* pkt_write() to socket, socket is non-blocking: */
  r = pkt_write(client->connfd, client->pkt, w);

  if(r == -1){
      /* error, close socket, reset client */
      warn_syserr("error writing to client");
      perpd_conn_close(client);
      return;
  }

  /* else: */
  w += r;
  if(w == n){
      /* packet write complete, resume in read state: */
      log_debug("perpd_conn_write() complete");
      perpd_conn_start(client, connfd);
  } else {
      /* continue writing on next poll()... */
      log_debug("perpd_conn_write() to be continued...");
      client->w = w;
  }

  return;
}


/* eof: perpd_conn.c */
