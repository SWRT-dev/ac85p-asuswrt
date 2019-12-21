/* tinylog.h
** tinylog configuration header
** wcm, 2008.10.09 - 2013.01.11
** ===
*/
#ifndef TINYLOG_H
#define TINYLOG_H 1

#ifndef TINYLOG_VERSION
#define TINYLOG_VERSION  "2.07"
#endif

/* what to name single concurrency lock file: */
#ifndef TINYLOG_PIDLOCK
#define TINYLOG_PIDLOCK  "tinylog.pid"
#endif

/* where to find gzip executable:
** (may also be set with TINYLOG_ZIP in the environment)
*/
#ifndef TINYLOG_ZIP
#define TINYLOG_ZIP  "/usr/bin/gzip"
#endif

/* extension for zipped files: */
#ifndef ZIP_EXT
#define ZIP_EXT  ".Z"
#endif

/* mortality: */
#include <unistd.h>
#define die(e) \
  _exit((e))

/* ioq-based io: */
#include <errno.h>
#include "ioq.h"
#include "ioq_std.h"
#include "sysstr.h"

/* stderr: */
#define eputs(...) \
  {ioq_vputs(ioq2, __VA_ARGS__, "\n"); ioq_flush(ioq2);}

#define usage() \
  eputs("usage: ", progname, " ", prog_usage)

#define version() \
  eputs(progname, ": version: ", TINYLOG_VERSION)

#define die_usage() \
  {usage(); die(100);}

/* configure loggit macros: */
#define LOGGIT_FMT \
  progname, "[", my_pidstr, "]: "

/* logging to stderr, prefix with priority "pri": */
#define loggit(...) \
  eputs(LOGGIT_FMT, __VA_ARGS__)

#define loggit_pri(pri, ...) \
  eputs((pri), ": ", LOGGIT_FMT, __VA_ARGS__)

#define log_alert(...) \
  loggit_pri("alert", __VA_ARGS__)

#define log_error(...) \
  loggit_pri("error", __VA_ARGS__)

#define log_warning(...) \
  loggit_pri("warning", __VA_ARGS__)

#define log_notice(...) \
  loggit_pri("notice", __VA_ARGS__)

#define log_info(...) \
  loggit(__VA_ARGS__)

#ifndef NDEBUG
#  define log_debug(...) \
       loggit_pri("debug", __VA_ARGS__)
#  define log_trace(...) \
       loggit_pri("debug", "trace: ", __func__, "(): ", __VA_ARGS__)
#else
#  define log_trace(...)
#  define log_debug(...)
#endif /* NDEBUG */

/*
** fatal*(), warn*() -- using loggit:
*/

#define fatal(e, ...) \
  {log_error("fatal: ", __VA_ARGS__); die(e);}

#define fatal_usage(...) \
  {log_error("usage error: ", __VA_ARGS__); die_usage();}

#define fatal_syserr(...) \
  {\
      log_error("fatal: ", __VA_ARGS__, ": ", \
                sysstr_errno_mesg(errno), " (", \
                sysstr_errno(errno), ")"); \
      die(111);\
  }

#define warn_syserr(...) \
  log_warning(__VA_ARGS__, ": ", \
               sysstr_errno_mesg(errno), " (", \
               sysstr_errno(errno), ")")


#endif /* TINYLOG_H */
/* eof: tinylog.h */
