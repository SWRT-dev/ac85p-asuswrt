/* perphup.c
** perp: persistent process supervision
** perp 2.0: single process scanner/supervisor/controller
** perphup: trigger rescan in perpd
** wcm, 2009.11.11 - 2011.03.18
** ===
*/

/* libc: */
#include <stdlib.h>

/* unix: */
#include <unistd.h>
#include <errno.h>
#include <signal.h>

/* lasagna: */
#include "cstr.h"
#include "nextopt.h"
#include "nfmt.h"
#include "outvec.h"
#include "pidlock.h"
#include "sysstr.h"

/* perp: */
#include "perp_common.h"
#include "perp_stderr.h"


/* logging variables in scope: */
static const char *progname = NULL;
static const char  prog_usage[] = "[-hV] [-q] [-t] [basedir]";

/* option variables in scope: */
/* "quiet", !opq_q == verbose, default is verbose: */
static int opt_q = 0;
/* send sigterm instead of sighup: */
static int opt_t = 0;

int
main(int argc, char *argv[])
{
  nextopt_t    nopt = nextopt_INIT(argc, argv, ":hVqt");
  char         opt;
  const char  *basedir = NULL;
  char         pathbuf[256];
  size_t       n;
  pid_t        lockpid;
  char         nbuf[NFMT_SIZE];

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'}; 
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
      case 'q': ++opt_q; break;
      case 't': ++opt_t; break;
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

  basedir = *argv;

  if(!basedir)
      basedir = getenv("PERP_BASE");
  if(!basedir || (basedir[0] == '\0'))
      basedir = PERP_BASE_DEFAULT;

  if(chdir(basedir) != 0){
      fatal_syserr("failure chdir() to ", basedir);
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
 
  if(kill(lockpid, opt_t ? SIGTERM : SIGHUP) == -1){
      fatal_syserr("failure kill() on ", opt_t ? "SIGTERM" : "SIGHUP" , " to perpd pid ",
                   nfmt_uint32(nbuf, (uint32_t)lockpid),
                   "running on ", basedir);
  }

  /* success: */
  if(!opt_q){
      if(!opt_t){
          eputs(progname, ": perpd rescan triggered on ", basedir);
      }else{
          eputs(progname, ": SIGTERM for perpd on ", basedir);
      }
  }

  die(0);
}


/* eof: perphup.c */
