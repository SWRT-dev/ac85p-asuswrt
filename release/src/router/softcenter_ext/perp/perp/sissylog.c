/* sissylog.c
** log stdin to syslog
** wcm, 2009.09.28 - 2011.01.31
** ===
*/
#include <stddef.h>

#include <unistd.h>
#include <syslog.h>

#include "cstr.h"
#include "ioq.h"
#include "ioq_std.h"
#include "nextopt.h"

#include "sissylog.h"

static const char *progname = NULL;
static const char prog_usage[] = "[-hV] [ ident [ facility ]]";

/* ioq for stdin: */
#define INBUF_SIZE  1024
uchar_t  inbuf[INBUF_SIZE];
static ssize_t  read_op(int fd, void *buf, size_t len);
ioq_t in = ioq_INIT(0, inbuf, sizeof inbuf, &read_op);

/* buffer for sissylog's own messages: */
static char attention[200];

/* other declarations in scope: */
static void logline_post(const char *logline, int flag_continue);
static int do_log(void); 


static
ssize_t
read_op(int fd, void *buf, size_t len)
{
  ssize_t  r = 0;

  do{
      r = read(fd, buf, len);
  }while((r == -1) && (errno == EINTR));

  return r;
}


static 
void
logline_post(const char *logline, int flag_continue)
{
  static int  priority;

  if(flag_continue){
      syslog(priority, "+%s", logline);
  } else {
      priority = LOG_INFO;
      if(cstr_contains(logline, "alert:")){
          priority = LOG_ALERT;
      } else if(cstr_contains(logline, "error:")){
          priority = LOG_ERR;
      } else if(cstr_contains(logline, "warning:")){
          priority = LOG_WARNING;
      } else if(cstr_contains(logline, "notice:")){
          priority = LOG_NOTICE;
      } else if(cstr_contains(logline, "debug:")){
          priority = LOG_DEBUG;
      }
      syslog(priority, "%s", logline);
  }

  return;
}


static
int
do_log(void)
{
  char     logline[LOGLINE_MAX + 1];
  size_t   len = 0;
  ssize_t  r = 0;
  char     c;
  int      flag_continue = 0;

  for(;;){
      r = ioq_GETC(&in, &c);
      if(r < 1){
          /* eof or io error: */
          if(len > 0){
              /* post partial line: */
              logline[len] = '\0';
              logline_post(logline, flag_continue);
              flag_continue = 0;
              len = 0;
          }
          break;
      }
      if(c == '\n'){
          logline[len] = '\0';
          if(len > 0){
              logline_post(logline, flag_continue);
          }
          flag_continue = 0;
          len = 0;
          continue;
      }
      /* filter control characters other than \t: */
      if((c < 32) && (c != '\t')) c = '?';
      logline[len++] = c;
      if(len == LOGLINE_MAX){
          logline[len] = '\0';
          logline_post(logline, flag_continue);
          flag_continue = 1;
          len = 0;
          continue;
      }
  }

  return r;    
}


int
main(int argc, char *argv[])
{
  nextopt_t    nopt = nextopt_INIT(argc, argv, ":hV");
  char         opt;
  const char  *arg_ident;
  const char  *arg_facility = "LOG_DAEMON";
  int          id_facility;
  int          e;

  progname = nextopt_progname(&nopt);
  arg_ident = progname;
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
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

  if(argv[0]){
      arg_ident = argv[0];
      if(argv[1]){
          arg_facility = argv[1];
      }
  }

  if(cstr_cmp(arg_facility, "LOG_AUTH") == 0){
      id_facility = LOG_AUTH;
  } else if(cstr_cmp(arg_facility, "LOG_CRON") == 0){
      id_facility = LOG_CRON;
  } else if(cstr_cmp(arg_facility, "LOG_DAEMON") == 0){
      id_facility = LOG_DAEMON;
  } else if(cstr_cmp(arg_facility, "LOG_FTP") == 0){
      id_facility = LOG_FTP;
  } else if(cstr_cmp(arg_facility, "LOG_KERN") == 0){
      id_facility = LOG_KERN;
  } else if(cstr_cmp(arg_facility, "LOG_LPR") == 0){
      id_facility = LOG_LPR;
  } else if(cstr_cmp(arg_facility, "LOG_MAIL") == 0){
      id_facility = LOG_MAIL;
  } else if(cstr_cmp(arg_facility, "LOG_NEWS") == 0){
      id_facility = LOG_NEWS;
  } else if(cstr_cmp(arg_facility, "LOG_SYSLOG") == 0){
      id_facility = LOG_SYSLOG;
  } else if(cstr_cmp(arg_facility, "LOG_USER") == 0){
      id_facility = LOG_USER;
  } else if(cstr_cmp(arg_facility, "LOG_UUCP") == 0){
      id_facility = LOG_UUCP;
  } else {
      id_facility = LOG_DAEMON;
  }

  openlog(arg_ident, 0, id_facility);
  cstr_vcopy(attention, progname, ": logging from stdin ...");
  logline_post(attention, 0);

  /* loop reading stdin: */
  e = do_log();

  /* done: eof or i/o error: */
  if(e){
      cstr_vcopy(attention, "notice: ", progname, ": terminating on i/o error reading stdin");
  } else {
      cstr_vcopy(attention, progname, ": terminating normally on eof reading stdin");
  }
  logline_post(attention, 0);
  closelog();

  return ((e != 0) ? 111 : 0);
}


/* eof: sissylog.c */
