/* catuid.c
** display current uid, pid, etc
** wcm, 2009.09.21 - 2011.02.01
** ===
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
/* getsid() not posix: */
extern pid_t getsid(pid_t pid);

#include "nextopt.h"
#include "nfmt.h"

#include "runtools_common.h"

static const char *progname = NULL;
static const char prog_usage[] = "[-hv]";

#define report(k,v) \
  eputs((k), ":\t", (v))

int
main(int argc, char *argv[])
{
  nextopt_t  nopt = nextopt_INIT(argc, argv, "hV");
  char       opt;
  char       nfmt[NFMT_SIZE];

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

  argc -= optind;
  argv += optind;

  report("UID", nfmt_uint32(nfmt, (uint32_t)getuid()));
  report("EUID", nfmt_uint32(nfmt, (uint32_t)geteuid()));
  report("GID", nfmt_uint32(nfmt, (uint32_t)getgid()));
  report("EGID", nfmt_uint32(nfmt, (uint32_t)getegid()));
  report("PID", nfmt_uint32(nfmt, (uint32_t)getpid()));
  report("PPID", nfmt_uint32(nfmt, (uint32_t)getppid()));
  report("PGID", nfmt_uint32(nfmt, (uint32_t)getpgrp()));
  report("SID", nfmt_uint32(nfmt, (uint32_t)getsid(0)));
  report("umask", nfmt_uint32o_pad0(nfmt, (uint32_t)umask(0), 4));

  return 0;
}


/* eof: catuid.c */
