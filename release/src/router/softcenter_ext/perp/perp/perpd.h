/* perpd.h
** perp: perpsistent process supervision
** perpd 2.0: single process scanner/supervisor/controller
** perpd.h: header file for perpd
** wcm, 2011.01.12 - 2013.01.07
** ===
*/
#ifndef PERPD_H
#define PERPD_H  1

#include <stddef.h>
#include <stdint.h>

/* unix: */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>

/* lasanga: */
#include "outvec.h"
#include "pkt.h"
#include "sysstr.h"
#include "tain.h"
#include "uchar.h"

/* perp: */
#include "perp_common.h"


/* map to source:
** 
** the perpd application is partitioned into 3 source files:
**
**   [] perpd.c:
**      main() entry, option processing, initialization, signal handling,
**      main event loop, definition directory scanning, and capturing
**      terminated child processes
** 
**   [] perpd_svdef.c:
**      service activation, service initialization, service start/reset exec(),
**      service deactivation ("cull")
** 
**   [] perpd_conn.c:
**      client connection routines, packet/protocol processing
*/ 


/*
** configurable defines (perpd-specific):
*/

/* maximum number of services per perpd instance: */
#ifndef PERP_MAX
#define PERP_MAX  200
#endif

/* maximum number of conncurrent perpd client connections: */
#ifndef PERPD_CONNMAX
#define PERPD_CONNMAX  20
#endif

/* timeout for perpd client connection (in seconds): */
#ifndef PERPD_CONNSECS
#define PERPD_CONNSECS  8
#endif

/*
** access environment:
*/
extern char **environ;

/*
** other variables in scope (definitions in perpd.c):
*/

/* sigset blocking: */
extern sigset_t  poll_sigset;

/* stderr/logging variables: */
extern const char  *progname;
extern const char   prog_usage[];
extern const char  *my_pidstr;

/* base directory (default will be "/etc/perp"): */
extern const char  *basedir;

/* status variables: */
extern pid_t   my_pid;
extern tain_t  my_when;

/* perpd_trigger*()s
**   using selfpipe_ping() to trigger perpd_mainloop() processing
**   defined in perpd.c:
*/
extern void perpd_trigger_fail(void);
extern void perpd_trigger_scan(void);

/* perpd_lookup()
**   find a svdef by its dev/ino
**   defined in perpd.c:
*/
extern struct svdef * perpd_lookup(dev_t dev, ino_t ino);


/*
** perpd_svdef declarations:
*/

/* perpd_svdef macros: */
/* "which" subservice: */
#define SUBSV_MAIN  0
#define SUBSV_LOG   1
/* runscript "target": */
#define SVRUN_START 0
#define SVRUN_RESET 1

/* subsv object (one of a service pair): */
struct subsv {
  /* process id of current active main/log: */
  pid_t    pid;
  /* process id of previously active main/log: */
  pid_t    pid_prev;
  /* bitset flags (definitions in perp_common.h as described below): */
  uchar_t  bitflags;
/*
** set if this is the logger for the service pair:
**   #define SUBSV_FLAG_ISLOG     0x01
** runtime state flags:
**   #define SUBSV_FLAG_ISRESET   0x02
**   #define SUBSV_FLAG_ISONCE    0x04
**   #define SUBSV_FLAG_ISPAUSED  0x08
**   #define SUBSV_FLAG_WANTDOWN  0x10
** set if fork() fails in perpd_svrun():
**   #define SUBSV_FLAG_FAILING   0x20
*/
  /* timestamps: */
  tain_t  when;
  tain_t  when_ok;
  /* waitpid() status at termination: */
  int     wstat;
};

/* svdef object, perp service definition: */
struct svdef {
  /* device/inode of the service definition directory: */
  dev_t    dev;
  ino_t    ino;
  /* using fchdir() into svdir: */
  int      fd_dir;
  /* name (to 31 characters) of service: */
  /* notes:
  **   - basename of service directory, nul-terminated
  **   - the actual name may be truncated to fit this buffer
  **   - name is informational only, used for message/logging
  **   - the unique identifier for the service is dev/ino pair
  */
  char     name[31 + 1];
  /* timestamp at activation: */
  tain_t   when;
  /* bitset flags (definitions in perp_common.h as described below): */
  uchar_t  bitflags;
/*
** set on service activation in perpd_scan():
**   #define SVDEF_FLAG_ACTIVE  0x01
** set if logger defined for service:
**   #define SVDEF_FLAG_HASLOG  0x02
** set for service deactivation in perpd_scan():
**   #define SVDEF_FLAG_CULL    0x04
** set if reactivated before deactivation complete:
**   #define SVDEF_FLAG_CYCLE   0x08
** set for flag files found at startup:
**   #define SVDEF_FLAG_DOWN    0x10
**   #define SVDEF_FLAG_ONCE    0x20
*/
  /* pipe() between MAIN --> LOG: */
  int      logpipe[2];
  /* main/log service pair: */
  struct subsv  svpair[2];
};

/* perpd_svdef subroutines (defined in perpd_svdef.c): */
extern void perpd_svdef_clear(struct svdef *svdef);
extern void perpd_svdef_close(struct svdef *svdef);
extern int perpd_svdef_activate(struct svdef *svdef, const char *svdir, const struct stat *st);
extern void perpd_svdef_keep(struct svdef *svdef, const char *svdir);
extern void perpd_svdef_checkfail(struct svdef *svdef);
extern int perpd_svdef_wantcull(struct svdef *svdef);
extern int perpd_svdef_cullok(struct svdef *svdef);
extern int perpd_svdef_run(struct svdef *svdef, int which, int what);


/*
** perpd_conn declarations:
*/

/* perpd_conn object: ipc client connection: */
struct perpd_conn {
  int      connfd;  /* read/write socket for this client */
  tain_t   stamp;   /* tainstamp at connect */
  uchar_t  state;   /* current state of connection, defined below: */ 
#define PERPD_CONN_CLOSED   0
#define PERPD_CONN_READING  1
#define PERPD_CONN_WRITING  2
  pkt_t    pkt;
  size_t   n;       /* current bytes read into pkt[] */
  size_t   w;       /* current bytes written out of pkt[] */
}; 

/* perpd_conn subroutines (defined in perpd_conn.c): */
extern void perpd_conn_reset(struct perpd_conn *client);
extern void perpd_conn_close(struct perpd_conn *client);
extern void perpd_conn_start(struct perpd_conn *client, int connfd);
extern void perpd_conn_checkstale(struct perpd_conn *client, struct tain *now);
extern void perpd_conn_read(struct perpd_conn *client);
extern void perpd_conn_write(struct perpd_conn *client);


/*
** stderr/logging macros:
*/

/*
** eputs using outvec:
*/
#define eputs(...) \
  outvec_vputs(&OUTVEC_STDERR, __VA_ARGS__, "\n")

/* version(), usage(), etc: */
#define version() \
  eputs("version: ", progname, " ", PERP_VERSION)

#define usage() \
  eputs("usage: ", progname, " ", prog_usage)

#define die_usage() \
  {usage(); die(100);}


/*
** LOGGIT_FMT for perpd:
*/
#define LOGGIT_FMT \
  progname, "[", my_pidstr, "]: "

/*
** logging macros:
*/
#define loggit(...) \
  eputs(LOGGIT_FMT, __VA_ARGS__)

/* logging to stderr, prefix with priority "pri": */
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


#endif /* PERPD_H */
/* eof: perpd.h */
