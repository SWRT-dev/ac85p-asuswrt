/* rundetach.c
** exec prog after detaching from controlling terminal
** (ie, background execution in new session)
** wcm, 2009.09.23 - 2011.01.31
** ===
*/
#include <unistd.h>
#include <sys/types.h>

#include "execvx.h"
#include "nextopt.h"

#include "runtools_common.h"


static const char *progname = NULL;
static const char prog_usage[] = "[-hV] program [args ...]";


int
main(int argc, char *argv[], char *envp[])
{
  nextopt_t  nopt = nextopt_INIT(argc, argv, "hV");
  char       opt;
  pid_t      pid;

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

  if(argc < 1){
      fatal_usage("missing required program argument");
  }

  if((pid = fork()) == -1){
      fatal_syserr("failure to detach");
  }
  if(pid != 0){
      /* parent exits: */
      _exit(0);
  }
  setsid(); 

  /* execvx() provides path search for prog */
  execvx(argv[0], argv, envp, NULL);

  /* uh oh: */
  fatal_syserr("unable to run ", argv[0]);

  /* not reached: */
  return 0;
}


/* eof: rundetach.c */
