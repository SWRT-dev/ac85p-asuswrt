/* runsession.c
** exec prog in a new session group
** (djb pgrphack facility)
** wcm, 2009.09.08 - 2011.01.31
** ===
*/
#include <unistd.h>

#include "execvx.h"
#include "nextopt.h"

#include "runtools_common.h"

static const char *progname = NULL;
static const char prog_usage[] =
  "[-hV] program [args ...]";


int
main(int argc, char *argv[], char *envp[])
{
  nextopt_t  nopt = nextopt_INIT(argc, argv, "hV");
  char       opt;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'V': version(); die(0); break;
      case 'h': usage(); die(0); break;
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

  if(argc < 1){
      fatal_usage("missing required program argument");
      die_usage();
  }

  /* start new session group (ignore failure): */
  setsid();

  /* execvx() provides path search for prog */
  execvx(argv[0], argv, envp, NULL);

  /* uh oh: */
  fatal_syserr("unable to run ", argv[0]);

  /* not reached: */
  return 0;
}


/* eof: runsession.c */
