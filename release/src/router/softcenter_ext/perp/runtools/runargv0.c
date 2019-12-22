/* runargv0.c
** exec prog with argv[0] setup
** (djb argv0 facility [ucspi-tcp])
** wcm, 2009.09.08 - 2011.01.31
** ===
*/
#include <unistd.h>

#include "execvx.h"
#include "nextopt.h"

#include "runtools_common.h"

static const char *progname = NULL;
static const char prog_usage[] = "[-hV] realprog alias [args ...]";


int
main(int argc, char *argv[], char *envp[])
{
  nextopt_t    nopt = nextopt_INIT(argc, argv, "hV");
  char         opt;
  const char  *prog = NULL;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
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

  if(argc < 2){
      fatal_usage("missing required argument(s)");
  }

  /* our real executable: */
  prog = argv[0];

  /* ratchet alias into argv[0]: */
  ++argv;

  /* execvx() provides path search for prog */
  execvx(prog, argv, envp, NULL);

  /* uh oh: */
  fatal_syserr("unable to run ", prog);

  /* not reached: */
  return 0;
}


/* eof: runargv0.c */
