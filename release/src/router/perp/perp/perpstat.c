/* perpstat.c
** perp: persistent process supervision
** perp 2.0: single process scanner/supervisor/controller
** perpstat: query and report current service status
** (ipc client query to perpd server)
** wcm, 2008.01.23 - 2012.01.04
** ===
*/

/* libc: */
#include <stdlib.h>

/* unix: */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>

/* lasanga: */
#include "buf.h"
#include "cstr.h"
#include "domsock.h"
#include "fd.h"
#include "nextopt.h"
#include "nfmt.h"
#include "nuscan.h"
#include "pidlock.h"
#include "pkt.h"
#include "sysstr.h"
#include "tain.h"
#include "upak.h"
#include "uchar.h"

#include "perp_common.h"
#include "perp_stderr.h"

/* redefine eputs() for ioq: */
#ifdef eputs
#undef eputs
#endif

/* ioq-based i/o: */
#include "ioq.h"
#include "ioq_std.h"

#define eputs(...) \
  {ioq_flush(ioq1); ioq_vputs(ioq2, __VA_ARGS__, "\n"); ioq_flush(ioq2); }

/* stdout using ioq: */
#define vputs(...) \
  ioq_vputs(ioq1, __VA_ARGS__)

#define vputs_flush() \
  ioq_flush(ioq1)


/* logging variables in scope: */
static const char  *progname = NULL;
static const char   prog_usage[] = "[-hV] [-b basedir] sv [sv ...]";


static
void
report(const char *name, const uchar_t *status, const tain_t *now)
{
  pid_t    pid;
  tain_t   when;
  uint64_t uptime;
  uchar_t  flags;
  int      haslog;
  char     nbuf[NFMT_SIZE];

  /* svdef: */
  tain_unpack(&when, &status[16]);
  flags = status[28];
  haslog = (flags & SVDEF_FLAG_HASLOG) ? 1 : 0;

  /* uptime since activation: */
  uptime = tain_uptime(now, &when);
  vputs(name, ": activated ", nfmt_uint64(nbuf, uptime), " seconds");
  if(flags & SVDEF_FLAG_CULL){
      vputs(", deactivation in progress");
  }
  if(flags & SVDEF_FLAG_CYCLE){
      vputs(", flagged for reactivation");
  }
  vputs("\n");

  /* main: */
  pid = upak32_unpack(&status[30]); 
  tain_unpack(&when, &status[34]);
  flags = status[46];
  uptime = tain_uptime(now, &when);
  vputs("  main: ");
  if(pid == 0){
      vputs("down ", nfmt_uint64(nbuf, uptime), " seconds");
      if(!(flags & SUBSV_FLAG_WANTDOWN)) vputs(", want up!");
      if(flags & SUBSV_FLAG_ISONCE) vputs(", flagged once");
  }else if((uptime < 1) && !(flags & SUBSV_FLAG_ISRESET)){
      /* munge uptime < 1 second to want up: */
      vputs("down, want up!");
      if(flags & SUBSV_FLAG_ISONCE) vputs(", flagged once");
  }else{
      vputs((flags & SUBSV_FLAG_ISRESET) ? "resetting " : "up ");
      vputs(nfmt_uint64(nbuf, uptime), " seconds");
      vputs(" (pid ", nfmt_uint32(nbuf, (uint32_t)pid), ")");
      if(flags & SUBSV_FLAG_ISPAUSED) vputs(", paused");
      if(flags & SUBSV_FLAG_ISONCE) vputs(", flagged once");
  }

  /* log: */
  vputs("\n   log: ");
  if(!haslog){
      vputs("(no log)\n");
      return;
  }

  /* else: */
  pid = upak32_unpack(&status[48]);
  tain_unpack(&when, &status[52]);
  flags = status[64];
  uptime = tain_uptime(now, &when);
  if(pid == 0){
      vputs("down ", nfmt_uint64(nbuf, uptime), " seconds");
      if(!(flags & SUBSV_FLAG_WANTDOWN)) vputs(", want up!");
      if(flags & SUBSV_FLAG_ISONCE) vputs(", flagged once");
  }else if((uptime < 1) && !(flags & SUBSV_FLAG_ISRESET)){
      /* munge uptime < 1 second to want up: */
      vputs("down, want up!");
      if(flags & SUBSV_FLAG_ISONCE) vputs(", flagged once");
  }else{
      vputs((flags & SUBSV_FLAG_ISRESET) ? "resetting " : "up ");
      vputs(nfmt_uint64(nbuf, uptime), " seconds");
      vputs(" (pid ", nfmt_uint32(nbuf, (uint32_t)pid), ")");
      if(flags & SUBSV_FLAG_ISPAUSED) vputs(", paused");
      if(flags & SUBSV_FLAG_ISONCE) vputs(", flagged once");
  }
  vputs("\n");

  return;
}


int
main(int argc, char *argv[])
{
  nextopt_t    nopt = nextopt_INIT(argc, argv, ":hVb:");
  char         opt;
  const char  *basedir = NULL;
  char         pathbuf[256];
  tain_t       now;
  size_t       n;
  int          fd_conn;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char  optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
      case 'b': basedir = nopt.opt_arg; break;
      case ':':
          fatal_usage("missing argument for option -", optc);
          break;
      case '?':
          if(nopt.opt_got != '?'){
              fatal_usage("invalid option -", optc);
          }
          /* else fallthrough: */
      default : die_usage(); break;
      }
  }

  argc -= nopt.arg_ndx;
  argv += nopt.arg_ndx;

  if(!*argv){
      fatal_usage("missing argument");
  }

  if(!basedir)
      basedir = getenv("PERP_BASE");
  if(!basedir)
      basedir = ".";

  if(chdir(basedir) != 0){
      fatal_syserr("fail chdir() to ", basedir);
  }

  /* connect to control socket: */
  n = cstr_vlen(basedir, "/", PERP_CONTROL, "/", PERPD_SOCKET);
  if(!(n < sizeof pathbuf)){
      errno = ENAMETOOLONG;
      fatal_syserr("failure locating perpd control socket ",
                   basedir, "/", PERP_CONTROL, "/", PERPD_SOCKET);
  }
  cstr_vcopy(pathbuf, basedir, "/", PERP_CONTROL, "/", PERPD_SOCKET);
  fd_conn = domsock_connect(pathbuf);
  if(fd_conn == -1){
      if(errno == ECONNREFUSED){
          fatal_syserr("perpd not running on control socket ", pathbuf);
      }else{
          fatal_syserr("failure connecting to perpd control socket ", pathbuf);
      }
  }
 
  /* uptimes compared to now: */
  tain_now(&now);

  /* loop through service directory arguments and display report: */
  for(; *argv != NULL; ++argv){
      pkt_t        pkt = pkt_INIT(2, 'Q', 16);
      struct stat  st;

      if(stat(*argv, &st) == -1){
          eputs(*argv, ": error: service directory not found");
          continue;
      }

      if(! S_ISDIR(st.st_mode)){
          eputs(*argv, ": error: not a directory");
          continue;
      }

      if(!(S_ISVTX & st.st_mode)){
          vputs(*argv, ": not activated\n");
          continue;
      }

      upak_pack(pkt_data(pkt), "LL", (uint64_t)st.st_dev, (uint64_t)st.st_ino);

      if(pkt_write(fd_conn, pkt, 0) == -1){
          eputs_syserr("error: ", *argv, ": error writing query");
          continue;
      }

      if(pkt_read(fd_conn, pkt, 0) == -1){
          eputs_syserr("error: ", *argv, ": error reading response");
          continue;
      }

      if(pkt[0] != 2){
          eputs("error: ", *argv, ": unknown packet protocol in reply");
          continue;
      }
      if(pkt[1] != 'S'){
          if(pkt[1] == 'E'){
              errno = (int)upak32_unpack(&pkt[3]);
              eputs_syserr("error: ", *argv, ": error reported in reply");
          } else {
              eputs("error: ", *argv, ": unknown packet type in reply");
          }
          continue;
      }

      report(*argv, pkt_data(pkt), &now);
  }

  vputs_flush();
  die(0);
}

/* eof: perpstat.c */
