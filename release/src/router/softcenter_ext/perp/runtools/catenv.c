/* catenv.c
** display current environ
** wcm, 2009.09.08 - 2011.01.31
** ===
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "nextopt.h"

#include "runtools_common.h"

static const char *progname = NULL;
static const char  prog_usage[] = "[-hV]";


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
      case '?':
          if(nopt.opt_got != '?'){
              eputs(progname, ": usage error: invalid option: -", optc);
          }
          /* fallthrough: */
      default :
          die_usage(); break; 
      }
  }

  argc -= nopt.arg_ndx;
  argv += nopt.arg_ndx;

  while(*envp){
      ioq_vputs(ioq1, *envp, "\n");
      ++envp;
  }
  ioq_flush(ioq1);

  return 0;
}


/* eof: catenv.c */
