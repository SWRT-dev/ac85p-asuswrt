/* perp_stderr.h
** common stderr i/o macros for perp client apps
** wcm, 2011.01.24 - 2011.01.24
** ===
*/
#ifndef PERP_STDERR_H
#define PERP_STDERR_H  1

/* lasagna: */
#include "sysstr.h"

/* stderr output macros require access to strings/variables:
**   PERP_VERSION:  version identifer
**   progname:      basename of executable
**   prog_usage:    brief help summary
*/

/* default eputs(): */
#ifndef eputs
/* eputs using outvec: */
#include "outvec.h"
#define eputs(...) \
  outvec_vputs(&OUTVEC_STDERR, __VA_ARGS__, "\n")
#endif

/* default die(): */
#ifndef die
#include <unistd.h>
#define die(e) _exit((e))
#endif


#define version() \
  eputs("version: ", progname, " ", PERP_VERSION)

#define usage() \
  eputs("usage: ", progname, " ", prog_usage)

#define die_usage() \
  {usage(); die(100);}

#define fatal(e, ...) \
  {eputs(progname, " fatal: ", __VA_ARGS__); die((e));}

#define fatal_usage(...) \
  {eputs(progname, " usage error: ", __VA_ARGS__); die_usage();}

#define fatal_syserr(...) \
  {\
      eputs(progname, " fatal: ", __VA_ARGS__, ": ", \
                sysstr_errno_mesg(errno), " (", \
                sysstr_errno(errno), ")"); \
      die(111);\
  }

#define eputs_syserr(...) \
      eputs(__VA_ARGS__, ": ", \
                sysstr_errno_mesg(errno), " (", \
                sysstr_errno(errno), ")"); \


/* debug output: */
#ifndef NDEBUG
#define debug(...) \
  eputs(progname, " debug: ", __VA_ARGS__)
#else
#define debug(...)
#endif


#endif /* PERP_STDERR_H */
/* eof: perp_stderr.h */
