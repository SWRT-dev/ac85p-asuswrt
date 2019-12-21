/* catlimits.c
** display current resource limits
** wcm, 2009.09.08 - 2011.01.31
** ===
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/resource.h>

#include "nextopt.h"
#include "nfmt.h"
#include "rlimit.h"

#include "runtools_common.h"


static const char *progname = NULL;
static const char  prog_usage[] = "[-hV]" ;

static const char *resources[] = {
  "RLIMIT_AS",
  "RLIMIT_CORE",
  "RLIMIT_CPU",
  "RLIMIT_DATA",
  "RLIMIT_FSIZE",
  "RLIMIT_LOCKS",
  "RLIMIT_MEMLOCK",
  "RLIMIT_NOFILE",
  "RLIMIT_NPROC",
  "RLIMIT_RSS",
  "RLIMIT_SBSIZE",
  "RLIMIT_STACK",
  NULL
};


int
main(int argc, char *argv[])
{
  nextopt_t  nopt = nextopt_INIT(argc, argv, "hV");
  char       opt;
  int        i;

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

  for(i = 0; resources[i] != NULL; ++i){
      const char  *resource = resources[i];
      int  r = rlimit_lookup(resource);
      char  nfmt[NFMT_SIZE];
      struct rlimit  rlim;
      if(r == -1){
          ioq_vputs(ioq1, resource, "\t[not provided on this platform]\n");
          continue;
      }
      getrlimit(r, &rlim);
      ioq_vputs(ioq1, resource, "\t", rlimit_mesg(r), ": "); 
      if(rlim.rlim_cur == RLIM_INFINITY){
          ioq_vputs(ioq1, "unlimited\n");
      } else {
          ioq_vputs(ioq1, nfmt_uint32(nfmt, rlim.rlim_cur), "\n");
      }
  }
  ioq_flush(ioq1);

  return 0;
}


/* eof: catlimits.c */
