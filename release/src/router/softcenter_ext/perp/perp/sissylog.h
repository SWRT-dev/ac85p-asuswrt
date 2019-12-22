/* sissylog.h
** wcm, 2009.09.29 - 2013.01.13
** ===
*/
#ifndef SISSYLOG_H
#define SISSYLOG_H 1

#include <errno.h>

/* ioq-based stderr: */
#include "ioq.h"
#include "ioq_std.h"
#include "sysstr.h"

/* release version string: */
#ifndef SISSYLOG_VERSION
#define SISSYLOG_VERSION "2.07"
#endif

#ifndef LOGLINE_MAX
#define LOGLINE_MAX  800
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
  {\
    eputs("usage: ", progname, " ", prog_usage); \
  }

#define version() \
  {\
    eputs(progname, ": version: ", SISSYLOG_VERSION); \
  }

#define die_usage() \
  { usage(); die(100); }


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


#endif /* SISSYLOG_H */
/* eof (sissylog.h) */
