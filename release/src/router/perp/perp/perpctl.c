/* perpctl.c
** perp: persistent process supervision
** perp 2.0: single process scanner/supervisor/controller
** perpctl: perpd administrative control interface
** (ipc client control to perpd server)
** wcm, 2008.01.23 - 2011.01.28
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
#include "nextopt.h"
#include "nfmt.h"
#include "pidlock.h"
#include "pkt.h"
#include "sysstr.h"
#include "upak.h"
#include "uchar.h"

#include "perp_common.h"
#include "perp_stderr.h"

/* additional stderr macros: */
#define report(...) \
 {if(!opt_q) eputs(__VA_ARGS__);}


/* logging variables in scope: */
static const char  *progname = NULL;
static const char   prog_usage[] =
  "[-hV] [-b basedir] [-g] [-L] [-q] [ADUXduopcahikqtw12] sv [sv ...]";

/* other variables in scope: */
/* "quiet" option: */
static int opt_q = 0;
/* perp base directory: */
static const char  *basedir = NULL;
/* error counter: */
static int  errs = 0;

/* functions in scope: */
static void do_sticky(int flag_sticky, char *argv[]);
static void do_control(uchar_t cmd[], char *argv[]);


/* do_sticky()
**  - set sticky bit according to flag_sticky to list
**    of directories given in argv
**  -trigger rescan
**
**  on entry, flag_sticky is one of two values:
**    +1: set sticky bit
**    -1: unset sticky bit
*/
void
do_sticky(int flag_sticky, char *argv[])
{
  char     pathbuf[256];
  size_t   n;
  size_t   count = 0;
  pid_t    lockpid;
  char    *what = (flag_sticky == 1) ? "activation" : "deactivation";
  char     nbuf[NFMT_SIZE];

  /* loop through directory arguments and chmod() for sticky: */
  for(; *argv != NULL; ++argv){
      struct stat  st;
      mode_t       mode;

      if(stat(*argv, &st) == -1){
          ++errs;
          eputs("error: ", *argv, ": service directory not found");
          continue;
      }

      if(! S_ISDIR(st.st_mode)){
          ++errs;
          eputs("error: ", *argv, ": not a directory");
          continue;
      }

      mode = (flag_sticky == 1) ?
             (st.st_mode | S_ISVTX) : (st.st_mode & ~S_ISVTX);

      if(chmod(*argv, mode) == -1){
          ++errs;
          eputs("error: ", *argv, "");
          continue;
      }

      /* success: */
      ++count;
      report(*argv, ": set for ", what);
  }

  /* get pid from active perpd pidlock: */
  n = cstr_vlen(basedir, "/", PERP_CONTROL, "/", PERPD_PIDLOCK);
  if(!(n < sizeof pathbuf)){
      errno = ENAMETOOLONG;
      fatal_syserr("failure locating perpd lockfile ",
                   basedir, "/", PERP_CONTROL, "/", PERPD_SOCKET);
  }
  cstr_vcopy(pathbuf, basedir, "/", PERP_CONTROL, "/", PERPD_PIDLOCK);

  lockpid = pidlock_check(pathbuf);

  if(lockpid == -1){
      fatal_syserr("failure checking lock on ", pathbuf);
  }

  if(lockpid == 0){
      fatal(111, "perpd not running on ", basedir, ": no lock active on ", pathbuf);
  }
 
  if(kill(lockpid, SIGHUP) == -1){
      fatal_syserr("failure kill() on SIGHUP to perpd pid ",
                   nfmt_uint32(nbuf, (uint32_t)lockpid),
                   "running on ", basedir);
  }

  /* success: */
  report("rescan triggered for ", what, " set on ",
        nfmt_uint32(nbuf, count),
        (count == 1) ? " service" : " services");

  return;
}


/* do_control()
**   send cmd to list of services given in argv
*/
void
do_control(uchar_t cmd[], char *argv[]){
  char     pathbuf[256];
  size_t   n;
  int      fd_conn;
  int      e;

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

  /* loop through service directory arguments and send control packet: */
  for(; *argv != NULL; ++argv){
      pkt_t        pkt = pkt_INIT(2, 'C', 18);
      struct stat  st;

      if(stat(*argv, &st) == -1){
          ++errs;
          eputs("error: ", *argv, ": service directory not found");
          continue;
      }

      if(! S_ISDIR(st.st_mode)){
          ++errs;
          eputs("error: ", *argv, ": not a directory");
          continue;
      }

      if(!(S_ISVTX & st.st_mode)){
          ++errs;
          eputs("error: ", *argv, ": service directory not activated");
          continue;
      }

      /* control packet for this directory: */
      upak_pack(pkt_data(pkt), "LLbb",
                (uint64_t)st.st_dev, (uint64_t)st.st_ino, cmd[0], cmd[1]);

      if(pkt_write(fd_conn, pkt, 0) == -1){
          ++errs;
          eputs_syserr("error: ", *argv, ": error writing request");
          continue;
      }

      if(pkt_read(fd_conn, pkt, 0) == -1){
          ++errs;
          eputs_syserr("error: ", *argv, ": error reading response");
          continue;
      }

      if(pkt[0] != 2){
          ++errs;
          eputs("error: ", *argv, ": unknown packet protocol in reply");
          continue;
      }
      if(pkt[1] != 'E'){
          ++errs;
          eputs("error: ", *argv, ": unknown packet type in reply");
          continue;
      }

      e = (int)upak32_unpack(&pkt[3]);
      if(e != 0){
          ++errs;
          errno = e;
          eputs_syserr("error: ", *argv, ": error reported in reply");
          continue;
      }

      /* success: */
      report(*argv, ": ok");
  }

  return; 
}


int
main(int argc, char *argv[])
{
  nextopt_t       nopt = nextopt_INIT(argc, argv, ":hVb:gLq");
  char            opt;
  int             opt_g = 0;
  int             opt_L = 0;
  int             flag_sticky = 0;
  uchar_t         cmd[2] = {'\0', '\0'};

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
      case 'b': basedir = nopt.opt_arg; break;
      case 'g': ++opt_g; break;
      case 'L': ++opt_L; break;
      case 'q': ++opt_q; break;
      case ':':
          fatal_usage("missing argument for option -", optc);
          break;
      case '?':
          if(nopt.opt_got != '?'){
              fatal_usage("invalid option: -", optc);
          }
          /* else fallthrough: */
      default :
          die_usage(); break;
      }
  }

  argc -= nopt.arg_ndx;
  argv += nopt.arg_ndx;
  if(!*argv){
      eputs(progname, ": usage error: missing arguments");
      die_usage();
  }

  /* next argv is the control command (can be taken from first letter): */
  switch(cmd[0] = *argv[0]){
  case 'A': flag_sticky = +1; break;
  case 'X': flag_sticky = -1; break;
  case 'd':
  case 'u':
  case 'o':
  case 'p':
  case 'c':
  case 'a':
  case 'h':
  case 'i':
  case 'k':
  case 'q':
  case 't':
  case 'w':
  case '1':
  case '2':
      if(opt_L){
         /* control command for log service: */
         cmd[1] |= SVCMD_FLAG_LOG;
      }
      break;
  case 'D':
  case 'U':
      if(opt_L){
          fatal_usage("meta-command '", *argv,
                      "' may not be used with option -L");
      }
      break;
  default:
      fatal_usage("unknown control command '", *argv, "'");
      break;
  }

  --argc;
  ++argv;
  if(!*argv){
      fatal_usage("missing argument: no service(s) specified");
  }

  if(basedir == NULL)
      basedir = getenv("PERP_BASE");
  if((basedir == NULL) || (basedir[0] == '\0'))
      basedir = ".";

  if(chdir(basedir) != 0){
      fatal_syserr("unable to chdir() to ", basedir);
  }

  /*
  ** service activation/deactivation command (A or X):
  */
  if(flag_sticky != 0){
      do_sticky(flag_sticky, argv);
      die((errs ? 111 : 0));
  }

  /*
  ** else: service control command
  */

  /* killpg() flag: */
  if(opt_g){
      cmd[1] |= SVCMD_FLAG_KILLPG;
  }

  do_control(cmd, argv);

  die((errs ? 111 : 0));
}

/* eof: perpctl.c */
