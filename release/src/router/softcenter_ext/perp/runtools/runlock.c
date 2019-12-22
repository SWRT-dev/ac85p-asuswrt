/* runlock.c
** exec prog with a lock file
** (djb setlock facility)
** wcm, 2009.09.08 - 2011.01.31
** ===
*/

/* unix: */
#include <unistd.h>
#include <fcntl.h>

/* lasagna: */
#include "cstr.h"
#include "execvx.h"
#include "nextopt.h"
#include "nfmt.h"
#include "pidlock.h"

#include "runtools_common.h"

static const char *progname = NULL;
static const char prog_usage[] =
  "[-hV] [-c [-q] lockfile | [-bpx] lockfile program [args ...]]";


static void do_check(const char *lockfile, int quiet);


static
void
do_check(const char *lockfile, int quiet)
{
  pid_t  lockpid;
  char   nfmt[NFMT_SIZE];

  lockpid = pidlock_check(lockfile);

  if(lockpid == -1){
      if(quiet) die(111);
      /* else: */
      fatal_syserr("failure checking lock on ", lockfile);
  } else if(lockpid == 0){
      if(quiet) die(0);
      /* else: */
      eputs(progname, ": no lock held on ", lockfile);
      die(0);
  } else {
      if(quiet) die(1);
      /* else: */
      nfmt_uint32(nfmt, (uint32_t)lockpid);
      eputs(progname, ": lock held on ", lockfile, " by pid ", nfmt);
      die(1);
  }

  /* not reached: */
  return;
}


int
main(int argc, char *argv[], char *envp[])
{
  nextopt_t    nopt = nextopt_INIT(argc, argv, "hVbcpqx");
  char         opt;
  const char  *lockfile = NULL;
  const char  *prog = NULL;
  int          opt_block = 0;
  int          opt_err = 1;
  int          opt_pid = 0;
  int          opt_check = 0;
  int          opt_checkquiet = 0;
  int          fd;
 
  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'V': version(); die(0); break;
      case 'h': usage(); die(0); break;
      case 'b': opt_block = 1; break;
      case 'c': opt_check = 1; break;
      case 'p': opt_pid = 1; break;
      case 'q': opt_checkquiet = 1; break;
      case 'x': opt_err = 0; break;
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

  if(argc > 0) lockfile = argv[0];
  if(argc > 1) prog = argv[1];

  if(opt_check){
      if(lockfile == NULL){
          fatal_usage("missing lockfile argument");
      }
      /* no return: */
      do_check(lockfile, opt_checkquiet);
  }

  if(prog == NULL){
      fatal_usage("missing required argument(s)");
  }
  ++argv; 

  fd = pidlock_set(lockfile,
                   (opt_pid ? getpid() : 0),
                   (opt_block ? PIDLOCK_WAIT : PIDLOCK_NOW));

  if(fd == -1){
      if(!opt_err) die(0);
      /* else: */
      fatal_syserr("unable to acquire lock on ", lockfile);
  }

  execvx(prog, argv, envp, NULL);
  fatal_syserr("unable to run ", prog); 

  /* not reached: */
  return 0;
}


/* eof: runlock.c */
