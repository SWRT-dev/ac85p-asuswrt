/* perpok.c
** perp: persistent process supervision
** perp 2.0: single process scanner/supervisor/controller
** perpok: query and return service "ok"
** (ipc client query to perpd server)
** wcm, 2009.11.09 - 2011.01.27
** ===
*/

/* libc: */
#include <stdlib.h>

/* unix: */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>

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
#include "sysstr.h"
#include "tain.h"
#include "upak.h"
#include "uchar.h"

#include "perp_common.h"
#include "perp_stderr.h"


static const char *progname = NULL;
static const char prog_usage[] = "[-hV] [-b basedir] [-u secs] [-v] sv";


#define  report_fail(...) \
  {\
      if(verbose){\
          eputs(progname, ": ", svdir, ": fail: ", __VA_ARGS__);\
      }\
      die(1);\
  }

#define  report_ok(...) \
  {\
      if(verbose){\
          eputs(progname, ": ", svdir, ": ok: ",  __VA_ARGS__);\
      }\
      die(0);\
  }


int
main(int argc, char *argv[])
{
  nextopt_t       nopt = nextopt_INIT(argc, argv, ":hVb:u:v");
  char            opt;
  int             verbose = 0;
  const char     *basedir = NULL;
  uint32_t        arg_upsecs = 0;
  const char     *z;
  const char     *svdir = NULL;
  struct stat     st;
  char            pathbuf[256];
  size_t          n;
  int             fd_conn;
  pkt_t           pkt = pkt_INIT(2, 'Q', 16);
  uchar_t        *status = pkt_data(pkt);
  pid_t           pid_main;
  tain_t          when_main;
  uint64_t        uptime_main;
  uchar_t         flags;
  tain_t          now;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
      case 'b': basedir = nopt.opt_arg; break;
      case 'u':
          z = nuscan_uint32(&arg_upsecs, nopt.opt_arg);
          if(*z != '\0'){
              fatal_usage("non-numeric argument found for option -", optc, " ", nopt.opt_arg);
          }
          break;
      case 'v': ++verbose; break;
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

  svdir = argv[0];
  if(svdir == NULL){
      fatal_usage("missing argument");
  }

  if(!basedir)
      basedir = getenv("PERP_BASE");
  if(!basedir)
      basedir = ".";


  if(chdir(basedir) != 0){
      fatal_syserr("failure chdir() to ", basedir);
  }

  if(stat(svdir, &st) == -1){
      fatal_syserr("failure stat() on service directory ", svdir);
  }

  if(! S_ISDIR(st.st_mode)){
      fatal_usage("argument not a directory: ", svdir);
  }

  if(!(S_ISVTX & st.st_mode)){
      report_fail("service directory ", svdir, " not activated");
  }

  /* connect to control socket: */
  n = cstr_vlen(basedir, "/", PERP_CONTROL, "/", PERPD_SOCKET);
  if(!(n < sizeof pathbuf)){
      errno = ENAMETOOLONG;
      fatal_syserr("failure locating perpd control socket ",
                   basedir, "/", PERP_CONTROL, "/", PERPD_SOCKET);
  }
  cstr_vcopy(pathbuf, basedir, "/", PERP_CONTROL, "/", PERPD_SOCKET);
  fd_conn = domsock_connect(pathbuf);
  if(fd_conn == -1){
      if(errno == ECONNREFUSED){
          fatal_syserr("perpd not running on control socket ", pathbuf);
      } else {
          fatal_syserr("failure connecting to perpd control socket ", pathbuf);
      }
  }

  /* uptime compared to now: */
  tain_now(&now);

  /* status query packet: */ 
  upak_pack(pkt_data(pkt), "LL", (uint64_t)st.st_dev, (uint64_t)st.st_ino);

  if(pkt_write(fd_conn, pkt, 0) == -1){
      fatal_syserr("failure pkt_write() to perpd control socket");
  }

  if(pkt_read(fd_conn, pkt, 0) == -1){
      fatal_syserr("failure pkt_read() from perpd control socket");
  }

  /* done with connection: */
  close(fd_conn);

  if(pkt[0] != 2){
      fatal(111, "unknown protocol found in reply from perpd control socket");
  }
  if(pkt[1] != 'S'){
      if(pkt[1] == 'E'){
          errno = (int)upak32_unpack(&pkt[3]);
          fatal_syserr("error reported in reply from perpd control socket");
      } else {
          fatal(111, "unknown packet reply type from perpd control socket");
      }
  }

  if(arg_upsecs == 0){
  /* basic test complete: */
      report_ok("service is activated/ok");
  }

  /* else continue extended testing... */

  pid_main = upak32_unpack(&status[30]);
  if(!(pid_main > 0)){
      report_fail("service not running (pid is 0)");
  }

  flags = status[46];
  if(flags & SUBSV_FLAG_ISRESET){
      report_fail("service is resetting");
  }
  if(flags & SUBSV_FLAG_WANTDOWN){
      report_fail("service wants down");
  }

  tain_unpack(&when_main, &status[34]);
  uptime_main = tain_uptime(&now, &when_main);
  if(uptime_main < (uint64_t)arg_upsecs){
      report_fail("service uptime not met");
  }

  /* okie dokie! */
  report_ok("service uptime ok");

  /* not reached: */
  die(0);
}


/* eof: perpok.c */
