/* runchoom.c
** run a program after writing "-17\n" to /proc/<pid>/oom_adj 
** (abatement for "dreaded linux oom killer")
** wcm, 2011.03.18 - 2011.03.22
** ===
*/

/* libc: */
#include <stdlib.h>

/* unix: */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

/* lasagna: */
#include "cstr.h"
#include "fd.h"
#include "execvx.h"
#include "nextopt.h"
#include "nfmt.h"
#include "nuscan.h"
#include "sysstr.h"

/* runtools: */
#include "runtools_common.h"


/* default definitions: */
#ifndef CHOOM_BASE_DEFAULT
#define CHOOM_BASE_DEFAULT  "/proc"
#endif

#ifndef CHOOM_KEY_DEFAULT
#define CHOOM_KEY_DEFAULT  "oom_adj"
#endif

/* see proc(5) manual (linux-specific): */
#ifndef CHOOM_SET_DEFAULT
#define CHOOM_SET_DEFAULT  "-17"
#endif

/*
** variables in scope:
*/
static const char *progname = NULL;
static const char prog_usage[] =
  "[-hV] [-ev] [-b base] [-k key] [-p pid] [-s str] program [args ...]";

/* options/optargs: */
static int   opt_e = 0;  /* fail on error */
static int   opt_v = 0;  /* verbose */
static char *arg_base = NULL;
static char *arg_key = NULL;
static char *arg_set = NULL;

static char  pathbuf[256];
static char  setbuf[256];
static char  pidfmt[NFMT_SIZE];

/* functions in scope: */
static int write_all(int fd, void *buf, size_t to_write);
static void do_choom(void);

/* syserr_warn() macro: */
#define syserr_warn(...) \
  {\
    eputs(progname, ": ", __VA_ARGS__, ": ", \
          sysstr_errno_mesg(errno), " (", \
          sysstr_errno(errno), ")"); \
  }


/*
** definitions:
*/

static
int
write_all(int fd, void *buf, size_t to_write)
{
  ssize_t  w = 0;

  while(to_write){
      do{
          w =  write(fd, buf, to_write);
      }while((w == -1) && (errno == EINTR));

      if(w == -1)  return -1;  /* error! */
      if(w == 0){;}            /* XXX no bytes written */

      buf += w;
      to_write -= w;
  }

  /* return:
  **    = -1, error (above)
  **    =  0, success (here)
  */

  return 0;
} 


/* do_choom()
**   if opt_e: abort on fail
**   no chdir(), no effect to cwd
*/
static
void
do_choom(void)
{
  int          fd;

  /* comment:
  ** normally we would first create/write a tmpfile
  ** then atomically rename() tmpfile into procfile
  ** but open(...,O_CREAT,...) of a new tmpfile on /proc fails (EEXIST)
  ** leaving no choice but to write into the procfile directly
  */

  if((fd = open(pathbuf, O_WRONLY | O_TRUNC, 0)) == -1){
      if(opt_e){
          fatal_syserr("failure on open() for path: ", pathbuf);
      }
      /* else: */
      if(opt_v){
          syserr_warn("ignoring failure on open() for path: ", pathbuf);
      }
      return;
  }
  fd_cloexec(fd);

  if(write_all(fd, setbuf, cstr_len(setbuf)) == -1){
      if(opt_e){
          fatal_syserr("failure on write() to path: ", pathbuf);
      }
      /* else: */
      if(opt_v){
          syserr_warn("ignoring failure on write() to path: ", pathbuf);
      }
      return;
  }

  /* comment:
  ** fsync() on /proc fails (EINVAL)
  ** so we simply ignore any following errors
  */
  fsync(fd);
  close(fd);

  /* success: */
  if(opt_v){
      eputs(progname, ": successfully configured ", pathbuf);
  } 

  return;
}


int
main(int argc, char *argv[], char *envp[])
{
  nextopt_t    nopt = nextopt_INIT(argc, argv, ":hVb:ek:p:s:v");
  char         opt;
  const char  *prog = NULL;
  uint32_t     pid = 0;
  const char  *z;
  size_t       n;
 
  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'V': version(); die(0); break;
      case 'h': usage(); die(0); break;
      case 'b': arg_base = nopt.opt_arg; break;
      case 'e': ++opt_e; break;
      case 'k': arg_key = nopt.opt_arg; break;
      case 'p':
          z = nuscan_uint32(&pid, nopt.opt_arg);
          if(*z != '\0'){
              fatal_usage("bad numeric argument for -", optc, ": ", nopt.opt_arg);
          }
          break;
      case 's': arg_set = nopt.opt_arg; break;
      case 'v': ++opt_v; break;
      case ':':
          fatal_usage("missing argument for option -", optc);
          break;
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

  if(argc > 0) prog = argv[0];
  if(prog == NULL){
      fatal_usage("missing required program argument");
  }

  if(arg_base == NULL)
      arg_base = getenv("CHOOM_BASE");
  if(arg_base == NULL)
      arg_base = CHOOM_BASE_DEFAULT;

  if(arg_base[0] != '/'){
      fatal_usage("path base is not absolute (must begin with `/'): ", arg_base);
  }

  if(arg_key == NULL)
      arg_key = getenv("CHOOM_KEY");
  if(arg_key == NULL)
      arg_key = CHOOM_KEY_DEFAULT;

  if(arg_set == NULL)
      arg_set = getenv("CHOOM_SET");
  if(arg_set == NULL)
      arg_set = CHOOM_SET_DEFAULT;

  nfmt_uint32(pidfmt, (pid == 0) ? (uint32_t)getpid() : pid); 

  n = cstr_vlen(arg_base, "/", pidfmt, "/", arg_key);
  if((sizeof pathbuf) - n < 5){
      errno = ENAMETOOLONG;
      fatal_syserr("path too long: ", arg_base, "/", pidfmt, "/", arg_key);
  }
  cstr_vcopy(pathbuf, arg_base, "/", pidfmt, "/", arg_key);

  n = cstr_vlen(arg_set, "\n");
  if((sizeof setbuf) - n < 1){
      fatal_usage("setting too long: ", arg_set);
  }
  cstr_vcopy(setbuf, arg_set, "\n");

  do_choom();

  execvx(prog, argv, envp, NULL);
  fatal_syserr("unable to run ", prog); 

  /* not reached: */
  return 0;
}

/* eof: runchoom.c */
