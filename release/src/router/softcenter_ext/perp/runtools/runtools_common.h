/* runtools_common.h
** common defines for runtools
** wcm, 2008.01.23 - 2013.01.11
** ===
*/
#ifndef RUNTOOLS_COMMON_H
#define RUNTOOLS_COMMON_H 1

#include <errno.h>

#include "ioq.h"
#include "ioq_std.h"
#include "sysstr.h"

/* release version string: */
#ifndef RUNTOOLS_VERSION
#define RUNTOOLS_VERSION "2.07"
#endif


/* stderr: */
#define eputs(...) \
  {\
    ioq_vputs(ioq2, __VA_ARGS__, "\n"); \
    ioq_flush(ioq2); \
  }


#define die(e) \
  _exit((e))

#define usage() \
  eputs("usage: ", progname, " ", prog_usage)

#define version() \
  eputs(progname, ": version: ", RUNTOOLS_VERSION)

#define die_usage() \
  {usage(); die(100);}


#define fatal(e, ...) \
  {\
    eputs(progname, ": fatal: ", __VA_ARGS__); \
    die((e)); \
  }

#define fatal_syserr(...) \
    fatal(111, __VA_ARGS__, ": ", \
        sysstr_errno_mesg(errno), " (", sysstr_errno(errno), ")" )

#define fatal_usage(...) \
  {\
    eputs(progname, ": usage error: ", __VA_ARGS__);\
    die_usage();\
  }


#endif /* RUNTOOLS_COMMON_H */
/* eof (runtools_common.h) */
