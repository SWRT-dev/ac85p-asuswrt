/* perpd.c
** perp: persistent process supervision
** perpd 2.0: single process scanner/supervisor/controller
** wcm, 2010.12.28 - 2013.01.07
** ===
*/

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* unix: */
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* lasanga: */
#include "buf.h"
#include "cstr.h"
#include "domsock.h"
#include "fd.h"
#include "nextopt.h"
#include "nfmt.h"
#include "nuscan.h"
#include "pidlock.h"
#include "pkt.h"
#include "pollio.h"
#include "sig.h"
#include "sigset.h"
#include "sysstr.h"
#include "tain.h"
#include "uchar.h"

/* perp: */
#include "perp_common.h"
#include "perpd.h"

/* brief pause on exceptional error (billionths of second): */
#define EPAUSE  555444321UL

/* logging variables in perpd scope: */
const char  *progname = NULL;
const char   prog_usage[] = "[-hV] [-a secs] [-g gid] [basedir]";
const char  *my_pidstr = NULL;

/* other variables available in perpd scope: */
/* base directory: */
const char  *basedir = NULL;
/* sigset blocking: */
sigset_t  poll_sigset;
/* status variables for perpd: */
pid_t     my_pid;
tain_t    my_when;

/*
** variables in file scope:
*/
/* options/args: */
static uint32_t  arg_autoscan = 0;
static gid_t     arg_gid = (gid_t)-1;
/* signal flags: */
static int  flag_chld = 0;
static int  flag_hup = 0;
static int  flag_term = 0;
/* exceptional failure on fork(): */
static int  flag_failing = 0;
/* perpd termination in progress: */
static int  flag_terminating = 0;
/* file descriptors: selfpipe, pidlock, listening socket: */
static int  selfpipe[2];
static int  fd_pidlock = -1;
static int  fd_listen = -1;
/* service definitions vector: */
static struct svdef  svdefs[PERP_MAX];
/* services currently active (in svdefs[]): */
static int  nservices = 0;

/*
** declarations in file scope:
*/

/* selfpipe: */
static void selfpipe_ping(void);
/* signal handler: */
static void sig_trap(int sig);

/* startup/initialize control directory: */
static void perpd_control_init(void);

/* scanner on basedir ("/etc/perp"): */
static void perpd_scan(void);
/* scanner helper function: */
static const struct stat * perpd_svdir_stat(const char *dirname);

/* cull deactivated services: */
static void perpd_cull(void);

/* waitpid() and process terminated children: */
static void perpd_waitup(void);

/* poll()-based event loop: */
static void perpd_mainloop(void);


/* selfpipe_ping():
**   triggered by sig_trap() signal handler
*/
static
void
selfpipe_ping(void)
{
  int  terrno = errno;
  int  w;

  do{
      w = write(selfpipe[1], "!", 1);
  }while((w == -1) && (errno == EINTR));

  errno = terrno;
  return;
}

/* signal handler: */
static
void
sig_trap(int sig)
{
    switch(sig){
    case SIGCHLD: ++flag_chld; break;
    case SIGHUP:  ++flag_hup;  break;
    case SIGINT:  /* fallthrough: */
    case SIGTERM: ++flag_term; break;
    default:      break;
    }

    /* unset signal flags if termination in progress: */
    if(flag_terminating){
        flag_term = 0;
        flag_hup = 0;
    }

    selfpipe_ping();
    return;
}


/* perpd_trigger_scan()
**   uses selfpipe_ping() to trigger rescan from mainloop
**   think: "fake_hup"
**   in perpd scope
*/
void
perpd_trigger_scan(void)
{
  if(!flag_terminating){
      ++flag_hup;
      selfpipe_ping();
  }

  return;
}


/* perpd_trigger_fail()
**   fork() fails: uses selfpipe_ping() to trigger mainloop
**   in perpd scope
*/
void
perpd_trigger_fail(void)
{
  if(!flag_terminating){
      ++flag_failing;
      selfpipe_ping();
  }

  return;
}


/* perpd_lookup():
**   scan svdefs[] for dev, ino
**   return:
**     NULL: not found
**     non-null: found, pointer to svdef
*/
struct svdef *
perpd_lookup(dev_t dev, ino_t ino)
{
  int  i;
  for(i = 0; i < nservices; ++i){
      if((svdefs[i].ino == ino) && (svdefs[i].dev == dev)){
          /* found: */
          return &svdefs[i];
      }
  }

  /* not found: */
  return NULL;
}


/* perpd_control_init()
**   setup/initialize perp control directory
**   abort on fail
**
**   notes:
**     cwd is basedir on entry/exit
*/
static
void
perpd_control_init(void)
{
  int  fdbase;  
  int  fd = -1;

  /* setup for return to base directory: */
  if((fdbase = open(".", O_RDONLY)) == -1){
      fatal_syserr("failure open() on base directory");
  }

  /* setup umask for intentional mode on file creation: */
  umask(0);

  /* initialize .control directory: */
  if(mkdir(PERP_CONTROL, 0700) == -1){
      char pathbuf[256];
      int  n;
      /* permit configuration using dangling symlink: */
      if((n = readlink(PERP_CONTROL, pathbuf, sizeof pathbuf)) != -1){
          if(n < (int)(sizeof pathbuf)){
              pathbuf[n] = '\0';
              mkdir(pathbuf, 0700); /* ignore failure */
          } else {
              errno = ENAMETOOLONG;
              fatal_syserr("failure readlink() on base control directory");
          }
      }
      /* ignoring other errors */
  }

  if(chdir(PERP_CONTROL) != 0){
      fatal_syserr("failure chdir() to ", PERP_CONTROL);
  }

  /* initialize pidlock (for single server instance): */
  fd = pidlock_set(PERPD_PIDLOCK, my_pid, PIDLOCK_NOW);
  if(fd == -1){
      fatal_syserr("failure on lock file ", PERPD_PIDLOCK);
  }
  fd_cloexec(fd);
  fd_pidlock = fd;

  /* create listening socket and bind: */
  fd = domsock_create(PERPD_SOCKET, 0700);
  if(fd == -1){
      fatal_syserr("failure bind() on socket ", PERPD_SOCKET);
  }
  if(arg_gid != (gid_t)-1){
      /* open socket to members of group: */
      if(chown(PERPD_SOCKET, (uid_t)-1, arg_gid) == -1){
          fatal_syserr("failure chown() on socket ", PERPD_SOCKET);
      }
      if(chmod(PERPD_SOCKET, 0770) == -1){
          fatal_syserr("failure chmod() on socket ", PERPD_SOCKET);
      }
  }
  if(domsock_listen(fd, PERPD_CONNMAX) == -1){
      fatal_syserr("failure listen() on socket ", PERPD_SOCKET); 
  }
  if(fd_nonblock(fd) == -1){
      fatal_syserr("failure fcntl() non-blocking on socket ", PERPD_SOCKET);
  }
  fd_cloexec(fd);
  fd_listen = fd;

  /* restore cwd: */
  if(fchdir(fdbase) == -1){
      fatal_syserr("failure fchdir() to base directory");
  }
  close(fdbase);

  return;
}


/* perpd_cull()
**   scan svdefs[] and cull any deactivated services
*/
static
void
perpd_cull(void)
{
  int  i = 0;

  while(i < nservices){
      if(perpd_svdef_cullok(&svdefs[i])){
          log_info("deactivating service ", svdefs[i].name);
          perpd_svdef_close(&svdefs[i]);
          --nservices;
          svdefs[i] = svdefs[nservices];
          continue;
      }
      ++i;
  }

  return;
}


/* perpd_waitup()
**   waitpid on sigchld
**   called by perpd_mainloop()
**   if service is set for cull and reaches cull state:
**     - harvest services at cull state
**
**   side effects:
**     any cull harvest will decrement nservices, reorder svdefs[]
*/
static
void
perpd_waitup(void)
{
  struct subsv  *subsv;
  pid_t          pid;
  int            wstat;
  int            which;
  int            got_cull = 0;
  int            got_cycle = 0;
  int            i;

  while((pid = waitpid(-1, &wstat, WNOHANG)) > 0){
      /* find dead child: */
      which = -1;
      for(i = 0; i < nservices; ++i){
          if(pid == svdefs[i].svpair[SUBSV_MAIN].pid){
              which = SUBSV_MAIN;
              break;
          }else if(pid == svdefs[i].svpair[SUBSV_LOG].pid){
              which = SUBSV_LOG;
              break;
          }
      }
      if(which == -1){
          log_debug("not my child");
          continue;
      }

      subsv = &svdefs[i].svpair[which];
      subsv->pid_prev = pid;
      subsv->pid = 0;
      subsv->wstat = wstat;
      /* if terminating from running "once", set want down: */
      if(subsv->bitflags & SUBSV_FLAG_ISONCE){
          subsv->bitflags |= SUBSV_FLAG_WANTDOWN;
      }

      /* check for reset/restart: */
      if(!(subsv->bitflags & SUBSV_FLAG_ISRESET)){
          /* subsv exited from start, always run reset: */
          log_debug("service ", svdefs[i].name, " (",
                    (which == SUBSV_MAIN) ? "main)" : "log)",
                    " terminated");
          perpd_svdef_run(&svdefs[i], which, SVRUN_RESET);
          continue;
      }
      /* else subsv exited from reset; restart if not wantdown: */
      if(!(subsv->bitflags & SUBSV_FLAG_WANTDOWN)){
          log_debug("restarting service ", svdefs[i].name, " (",
                    (which == SUBSV_MAIN) ? "main)" : "log)");
          perpd_svdef_run(&svdefs[i], which, SVRUN_START);
          continue;
      }
      /* else subsv wantsdown; initiate log termination too? */
      if((which == SUBSV_MAIN) && (svdefs[i].bitflags & SVDEF_FLAG_HASLOG)){
          subsv = &svdefs[i].svpair[SUBSV_LOG];
          if((subsv->bitflags & SUBSV_FLAG_WANTDOWN) && (subsv->pid > 0)){
              if(!(subsv->bitflags & SUBSV_FLAG_ISRESET)){
                  /* shutdown logger if not already running reset: */
                  close(svdefs[i].logpipe[1]);
                  close(svdefs[i].logpipe[0]);
                  kill(subsv->pid, SIGTERM);
              }
              /* make sure not paused (even if running reset): */
              kill(subsv->pid, SIGCONT);
              subsv->bitflags &= ~SUBSV_FLAG_ISPAUSED;
              continue;
          }
      }

      /*
      **  else:
      **    - subsv exited from reset
      **    - wants down
      **    - and svdef is all done
      **
      **  check if set for culling:
      */
      if(perpd_svdef_cullok(&svdefs[i])){
          ++got_cull;
      }
      /* check if service wants reactivation: */ 
      if(svdefs[i].bitflags & SVDEF_FLAG_CYCLE){
          log_debug("setting up reactivation following deactivation for ", svdefs[i].name);
          ++got_cycle;
      }
  }

  /* cull any deactivated services: */
  if(got_cull){
      perpd_cull();
  }

  if(got_cycle){
      /* trigger a perpd_scan() to reactivate this service: */
      log_debug("triggering a perpd_scan() for service reactivation");
      perpd_trigger_scan();
  }

  return;
}



/* perpd_svdir_stat()
**   stat dirname for valid service directory: name, isdir, and sticky
**   called by perpd_scan()
**
**   returns:
**     non-NULL: success
**       dirname is a valid perp service name
**       returns pointer to the static struct stat object for dirname
**
**     NULL: failure
**       dirname is not a valid perp service name, or
**       stat() failure
**
**   note:
**     errno is *not* left set on stat() failure
*/
static
const struct stat *
perpd_svdir_stat(const char *dirname)
{
  /* note: maintaining persistent stat object! */
  static struct stat  st;

  /* ignore if leading '.': */
  if(dirname[0] == '.'){
      return NULL;
  }

  /* get the stat: */
  if(stat(dirname, &st) == -1){
      warn_syserr("failure stat() on ", dirname);
      /* clear errno: */
      errno = 0;
      return NULL;
  }

  /* check stat for directory and sticky bit: */
  if(! ((S_ISDIR(st.st_mode)) && (st.st_mode & S_ISVTX))){
    return NULL;
  }

  /* okay: */
  return (const struct stat *)&st;
}


/* perpd_scan()
**   scan the perp base directory
**   activate new definitions
**   deactivate ("cull") deleted definitions
**
**   side effects:
**     activated services: svdefs[] and nservices
**     harvested for cull: likewise
**     got_fail activation failure:
**       - unexpected pipe()/open() failures in perpd_svdef_activate()
**       - pause and setup rescan with perpd_trigger_scan()
**     flag_failing:
**       - set in perpd_svdef_run() called by perpd_svdef_activate()
*/
static
void
perpd_scan(void)
{
  DIR                *dir;
  struct dirent      *d;
  const char         *svdir;
  const struct stat  *st;
  struct svdef       *svdef;
  tain_t              epause = tain_INIT(0, EPAUSE);
  int                 got_fail = 0;
  int                 got_cull = 0;
  int                 i, terrno;

  if((dir = opendir(".")) == NULL){
      warn_syserr("failure opendir() for service scan");
      return;
  }

  /*
  ** unflag as active all existing services
  ** (active services will be reflagged during scan)
  */
  for(i = 0; i < nservices; ++i){
      svdefs[i].bitflags &= ~SVDEF_FLAG_ACTIVE;
  }

  /* reset errno before scanning: */
  errno = 0;
  /* scan: */
  for(;;){

      /* loop terminal (end of directory): */
      if((d = readdir(dir)) == NULL){
          break;
      }

      svdir = d->d_name;
      if((st = perpd_svdir_stat(svdir)) == NULL){
          /* ignore this dirent: */
          continue;
      }

      /* otherwise, scan existing services for this dev/ino: */
      svdef = perpd_lookup(st->st_dev, st->st_ino);
      if(svdef != NULL){
          /* keeper, reflag service as active: */
          perpd_svdef_keep(svdef, svdir);
          if(svdef->bitflags & SVDEF_FLAG_CULL){
              /* deactivation in progress but not yet complete:
              **   - because something is still running
              **   - set FLAG_CYCLE to reactivate
              **   - perpd_waitup() will trigger rescan when ready to reactivate
              */
              log_debug("setting reactivation flag for ", svdir);
              svdef->bitflags |= SVDEF_FLAG_CYCLE;
          }
          continue;
      }
      /* else, activate new service: */
      if(!(nservices < PERP_MAX)){
          log_warning("unable to activate new service ", svdir, ": too many services");
          ++got_fail;
          continue;
      }
      /* activate and first start: */
      /* explicitly shield errno from perpd_svdef_activate(): */
      terrno = errno;
      if(perpd_svdef_activate(&svdefs[nservices], svdir, st) == -1){
          log_warning("unable to activate new service ", svdir);
          ++got_fail;
      } else {
          /* service activation successful: */
          svdefs[nservices].bitflags |= SVDEF_FLAG_ACTIVE;
          ++nservices;
          log_info("activated new service: ", svdir);
      }
      errno = terrno;
  }

  terrno = errno;
  closedir(dir);
  errno = terrno;

  /* note:
  **   want to assess errno only from readdir()
  **     -- which shouldn't happen!
  **   meaning: errno shielded from perpd_svdef_activate()
  */

  if(errno){
      warn_syserr("failure readdir() while scanning base directory");
      log_warning("pausing on base directory scanning failure...");
      tain_pause(&epause, NULL);
      /* trigger rescan: */
      perpd_trigger_scan();
      /* if readdir() failed prematurely, cull state is unstable: */
      return;
  }

  /* initiate/check cull on any services not flagged active: */
  for(i = 0; i < nservices; ++i){
      if(!(svdefs[i].bitflags & SVDEF_FLAG_ACTIVE)){
          if(!(svdefs[i].bitflags & SVDEF_FLAG_CULL)){
              /* initiate cull of service: */
              if(perpd_svdef_wantcull(&svdefs[i]) == 1){
                  /* this service is already in cull state: */
                  ++got_cull;
              }
          }else{
              /* unset any FLAG_CYCLE: */
              svdefs[i].bitflags &= ~SVDEF_FLAG_CYCLE;
              /* check if cullable: */
              if(perpd_svdef_cullok(&svdefs[i])){
                  /* this service is in cull state: */
                  ++got_cull;
              }
          }
      }
  }

  /* harvest any services ready for cull (from perpd_svdef_wantcull() above): */
  if(got_cull){
      perpd_cull();
  }

  /* too many services or activation failures in perpd_svdef_activate(): */
  if(got_fail){
      log_warning("pausing on service activation failure...");
      tain_pause(&epause, NULL);
      /* trigger rescan: */
      perpd_trigger_scan();
  }

  return;
}


/* perpd_mainloop()
**
** The perpd event loop is conceptually simple -- though long-winded
** in its full elaboration.
** 
** It is based on using poll() to multiplex a set of (non-blocking)
** i/o events.
** 
** The events are of three types:
** 
**   * signal interrupts (arriving via selfpipe)
**   * read/write events for connected clients
**   * new client connections on control socket
** 
** The connections are set non-blocking to be sure the perpd server
** can never be "hung" by a non-responsive or malicious client.  This
** requirement means it must accomodate the possibility of partial
** read/write from/to client connections.  Each client connection is
** set with a small static buffer sufficient to hold a single complete
** control packet, along with cursors to indicate the current read/write
** position within the buffer.  The control packet protocol is defined
** in such a way to allow a completeness check after each read/write.
** 
** Additionally, timestamps are applied to each client connection, so that
** stale connections may be closed and culled prior to each new poll().
**
** This version of the function initializes the pollv[] vector to consider
** only currently active connections on each poll().  The alternative is
** to poll() on a pollv[] vector that includes all possible PERPD_CONNMAX
** clients, whether active or inactive.
*/
static
void
perpd_mainloop(void)
{
  struct perpd_conn  clients[PERPD_CONNMAX];
  struct pollfd      pollv[PERPD_CONNMAX + 2];
  tain_t             now;
  tain_t             epause = tain_INIT(0, EPAUSE);
  int                poll_max;
  int                poll_interval, poll_remain;
  int                connfd;
  int                nconns = 0;
  int                last_nconns = -1;
  int                last_nservices = -1;
  int                nready;
  char               c, nbuf[NFMT_SIZE];
  int                i;

  /* fill pollv[] poll vector: */
  /* selfpipe: */
  pollv[0].fd = selfpipe[0];
  pollv[0].events = POLLIN;
  /* listening socket: */
  pollv[1].fd = fd_listen;
  pollv[1].events = POLLIN;

  /* initialize clients[]: */
  for(i = 0; i < PERPD_CONNMAX; ++i){
      perpd_conn_reset(&clients[i]);
  }

  poll_max = ((int)arg_autoscan) ? ((int)arg_autoscan * 1000) : -1;
  poll_interval = poll_max;
  poll_remain = 0;

  /* setup initial scan: */
  perpd_trigger_scan();

  /* main poll loop: */
  for(;;){

      /* loop terminal: */
      if(flag_terminating && (nservices == 0)){
          log_info("termination complete");
          break;
      }

      /* info logging: */
      if(last_nservices != nservices){
          log_info("supervising ",
                   nfmt_uint32(nbuf, (uint32_t)nservices), " active ",
                   (nservices == 1) ? "service" : "services");
          last_nservices = nservices;
      }
      if(last_nconns != nconns){
          log_info("monitoring ",
                   nfmt_uint32(nbuf, (uint32_t)nconns), " client ",
                   (nconns == 1) ? "connection" : "connections");
          last_nconns = nconns;
      }

      /*
      ** initializations for each poll:
      */

      /* close stale connections: */
      tain_now(&now);
      for(i = 0; i < nconns; ++i){
          if(clients[i].connfd != -1){
              perpd_conn_checkstale(&clients[i], &now);
          }
      }

      /* cull closed connections: */
      i = 0;
      while(i < nconns){
          if(clients[i].connfd == -1){
              --nconns;
              clients[i] = clients[nconns];
              continue;
          }
          ++i;
      }

      /* setup pollv[]: */       
      for(i = 0; i < nconns; ++i){
          pollv[2 + i].fd = clients[i].connfd;
          switch(clients[i].state){
          case PERPD_CONN_READING: pollv[2 + i].events = POLLIN; break;
          case PERPD_CONN_WRITING: pollv[2 + i].events = POLLOUT; break;
          default: pollv[2 + i].events = 0;
          }
      }

      /* set poll_interval for autoscan: */
      if(arg_autoscan > 0){
          poll_interval = (poll_remain < 100) ? poll_max : poll_remain;
      }

      /*
      ** poll:
      */

      /* poll() while signals unblocked: */
      sigset_unblock(&poll_sigset);
      {
          /* listening socket is closed during shutdown: */
          nfds_t  nfds = flag_terminating ? 1 : nconns + 2;
          do{
              nready = pollio(pollv, nfds, poll_interval, &poll_remain);
          }while((nready == -1) && (errno == EINTR));
      }
      sigset_block(&poll_sigset);

      /*
      ** process poll events:
      */

      /* poll error? */
      if(nready == -1){
          warn_syserr("failure poll() in main loop");
          continue;
      }

      /* check selfpipe: */
      if(pollv[0].revents & POLLIN){
          --nready;
          while(read(selfpipe[0], &c, 1) == 1){/*empty*/;}
      }

      /* term: */
      if(flag_term){
          log_info("initiating termination...");
          int got_cull = 0;
          /* setup global flags for termination in progress
          ** (disables any further setting of flag_term, flag_hup): */
          flag_terminating = 1;
          flag_term = 0;
          flag_hup = 0;
          /* disable further scanning: */
          arg_autoscan = 0;
          poll_max = -1;
          poll_interval = poll_max;
          /* close listening socket: */
          close(fd_listen);
          /* dump pending client connections: */
          for(i = 0; i < nconns; ++i){
              close(clients[i].connfd);
          }
          nconns = 0;
          /* initiate shutdown on all services: */
          for(i = 0; i < nservices; ++i){
              if(perpd_svdef_wantcull(&svdefs[i]) == 1){
                  ++got_cull;
              }
          }
          if(got_cull){
              perpd_cull();
          }
      }

      /* tend to dead children! */
      if(flag_chld){
          flag_chld = 0;
          perpd_waitup();
      }

      /* scan: */
      if(flag_hup || ((arg_autoscan > 0) && (poll_remain < 100))){
          flag_hup = 0;
          perpd_scan();
          poll_interval = poll_max;
      }

      /* exceptional failure in progress:
      **   fork() failure during a perp_svdef_run() call
      **   seek and retry failing svdefs with perpd_svdef_checkfail():
      */
      if(flag_failing){
          if(flag_terminating){
              /* no mucking about while failing: */
              log_warning("exceptional failure in progress at termination");
              log_warning("aborting normal shutdown sequence");
              die(111);
          }
          /* else: */
          flag_failing = 0;
          for(i = 0; i < nservices; ++i){
              perpd_svdef_checkfail(&svdefs[i]);
          }
          if(flag_failing){
              /* still failing! */
              /* note: implies perpd_trigger_fail() will trigger another loop */
              log_warning("pausing on persistent fork() failure...");
              tain_pause(&epause, NULL);
              /* XXX, ignore clients while failing: */
              continue;
          }
      }

      /* short-circuit if only selfpipe: */
      if(nready == 0) continue;

      /*
      ** check client sockets:
      */

      for(i = 0; i < nconns; ++i){
          if((connfd = clients[i].connfd) == -1){
              continue;
          }
          if(nready == 0) break;
          if(pollv[2 + i].revents & (POLLERR | POLLHUP | POLLNVAL)){
              --nready;
              if(pollv[2 + i].revents & (POLLERR | POLLNVAL)){
                  log_warning("error on client socket");
              }
              perpd_conn_close(&clients[i]);
              continue;
          }else if(pollv[2 + i].revents & POLLIN){
              --nready;
              perpd_conn_read(&clients[i]);
              continue;
          }else if(pollv[2 + i].revents & POLLOUT){
              --nready;
              perpd_conn_write(&clients[i]);
              continue;
          }else{
              /* XXX, what else? */
          }
      }/*end check clients*/

      /* short circuit if no more poll events: */
      if(nready == 0) continue;

      /* check new client connection: */
      if(pollv[1].revents & POLLIN){
          --nready;
          connfd = domsock_accept(fd_listen);
          if(connfd == -1){
              warn_syserr("failure accept() on new client connection");
          }else if(fd_nonblock(connfd) == -1){
              warn_syserr("failure fd_nonblock() on new client connection");
              close(connfd);
          }else if(nconns == PERPD_CONNMAX){
              log_warning("too many client connections");
              close(connfd);
          }else{
              log_debug("starting new client connection...");
              perpd_conn_start(&clients[nconns], connfd);
              ++nconns;
          }
      }/*end check connections*/

      if(nready != 0){
          /* shouldn't happen: */
          log_debug("uh oh, unhandled poll() events!");
      }

  }/* end for(;;) main poll loop */

  return;
}


int
main(int argc, char *argv[])
{
  nextopt_t      nopt = nextopt_INIT(argc, argv, ":hVa:g:");
  char           opt;
  uint32_t       u;
  static char    pidbuf[NFMT_SIZE];
  const char    *z;
  struct group  *grent = NULL;
  int            fd;

  /* obtain stringified pid (for logging): */
  my_pid = getpid();
  my_pidstr = nfmt_uint32(pidbuf, my_pid);

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
     char optc[2] = {nopt.opt_got, '\0'};
     switch(opt){
     case 'h': usage(); die(0); break;
     case 'V': version(); die(0); break;
     case 'a':
         z = nuscan_uint32(&u, nopt.opt_arg);
         if(*z != '\0'){
             fatal_usage("non-numeric argument found for option -", optc, ": ", nopt.opt_arg);
         }
         arg_autoscan = u;
         break;
     case 'g':
         if((nopt.opt_arg[0] > '0') && (nopt.opt_arg[0] < '9')){
         /* gid numeric: */
             z = nuscan_uint32(&u, nopt.opt_arg);
             if(*z != '\0'){
                 fatal_usage("bad format for group id argument found for option -", optc, ": ", nopt.opt_arg);
             }
             errno = 0;
             grent = getgrgid((gid_t)u);
             if(grent == NULL){
                 if((errno == 0) || (errno == ENOENT)){
                     fatal_usage("no group id found for option -", optc, " ", nopt.opt_arg);
                 }else{
                     fatal_syserr("failure getgrgid() for option -", optc, " ", nopt.opt_arg);
                 }
             }
         } else {
         /* gid group name: */
             errno = 0;
             grent = getgrnam(nopt.opt_arg);
             if(grent == NULL){
                 if((errno == 0) || (errno == ENOENT)){
                     fatal_usage("no group id found for option -", optc, " ", nopt.opt_arg);
                 }else{
                     fatal_syserr("failure getgrnam() for option -", optc, " ", nopt.opt_arg);
                 }
             }
         }
         arg_gid = grent->gr_gid;
         break;
     case ':':
         fatal_usage("missing argument for option -", optc);
         break; 
     case '?':
         if(nopt.opt_got != '?'){
             fatal_usage("invalid option -", optc);
         }
         /* else fallthrough: */
     default : die_usage(); break;
     }
  }

  argc -= nopt.arg_ndx;
  argv += nopt.arg_ndx;

  /* global base directory ("/etc/perp"): */
  basedir = argv[0];
  if(!basedir)
      basedir = getenv("PERP_BASE");
  if(!basedir || (basedir[0] == '\0'))
      basedir = PERP_BASE_DEFAULT;
  if(basedir[0] != '/'){
      fatal_usage("base directory not defined as absolute path: ", basedir);
  }
  if(chdir(basedir) != 0){
      fatal_syserr("failure chdir() to base directory ", basedir);
  }

  /* block signals: */
  sigset_fill(&poll_sigset);
  sigset_block(&poll_sigset);

  /* setup signal handlers: */ 
  sig_catch(SIGCHLD, &sig_trap);
  sig_catch(SIGHUP,  &sig_trap);
  sig_catch(SIGINT,  &sig_trap);
  sig_catch(SIGTERM, &sig_trap);
  sig_catch(SIGPIPE, &sig_trap);

  /* redirect stdin: */
  if((fd = open("/dev/null", O_RDWR)) == -1){
      fatal_syserr("failure on open() for /dev/null");
  }
  fd_move(0, fd);

  /* initialize selfpipe: */
  if(pipe(selfpipe) == -1){
      fatal_syserr("failure pipe() for selfpipe");
  }
  fd_cloexec(selfpipe[0]); fd_nonblock(selfpipe[0]);
  fd_cloexec(selfpipe[1]); fd_nonblock(selfpipe[1]);

  /* initialize control directory (pidlock, socket, etc): */
  perpd_control_init();

  /*
  ** no fatals beyond this point!
  */

  /* timestamp startup: */
  tain_now(&my_when);

  log_info("starting on ", basedir, " ...");
  perpd_mainloop();

  die(0);
}


/* eof: perpd.c */
