/* runlimit.c
** exec prog with resource limits 
** (update/replace djb softlimit facility)
** wcm, 2009.09.08 - 2013.01.07
** ===
*/

/* libc: */
#include <stdlib.h> /* getenv() */

/* unix: */
#include <unistd.h>
#include <fcntl.h>
#include <sys/resource.h>

/* lasagna: */
#include "cstr.h"
#include "execvx.h"
#include "ioq.h"
#include "nextopt.h"
#include "nfmt.h"
#include "nuscan.h"
#include "rlimit.h"

/* runtools: */
#include "runtools_common.h"

/* additional stderr: */
#define warn(...) \
  {\
    if(verbose) eputs(progname, " warning: ", __VA_ARGS__); \
  }

#define barf(...) \
  fatal(99, "barf on programming error: ", __VA_ARGS__)


/* variables in scope: */
static const char *progname = NULL;
static const char prog_usage[] =
  "[-hV]"
  " [-E] [-F file] "
  " [-a availmem] [-c coresize] [-d databytes] [-f filesize]"
  " [-l lockbytes] [-m membytes] [-o openfiles] [-p processes]"
  " [-r rssbytes] [-s stackbytes] [-t cpusecs]"
  " program [args ...]";

/* rlimits in environment: */
static int    opt_E = 0;
/* rlimits in file: */
static int    opt_F = 0;
static char  *arg_F = NULL;
/* stderr on warnings (option -v): */
static int verbose = 0;


struct runlimit {
  char   *name;
  char   *env;  /* XXX, unused */
  int     id;
  int     is_set;
  rlim_t  rl_soft;
  rlim_t  rl_hard;
};

struct runlimit runlimits[] = {
  /* name (define)     environment        id   is_set  soft   hard */
  /*---------------   ----------------    ---  ------  -----  -----*/
  { "RLIMIT_AS",      "RUNLIMIT_AS",      -1,   0,      0,    0 },
  { "RLIMIT_VMEM",    "RUNLIMIT_VMEM",    -1,   0,      0,    0 },
  { "RLIMIT_CORE",    "RUNLIMIT_CORE",    -1,   0,      0,    0 },
  { "RLIMIT_DATA",    "RUNLIMIT_DATA",    -1,   0,      0,    0 },
  { "RLIMIT_FSIZE",   "RUNLIMIT_FSIZE",   -1,   0,      0,    0 },
  { "RLIMIT_MEMLOCK", "RUNLIMIT_MEMLOCK", -1,   0,      0,    0 },
  { "RLIMIT_NOFILE",  "RUNLIMIT_NOFILE",  -1,   0,      0,    0 },
  { "RLIMIT_NPROC",   "RUNLIMIT_NPROC",   -1,   0,      0,    0 },
  { "RLIMIT_RSS",     "RUNLIMIT_RSS",     -1,   0,      0,    0 },
  { "RLIMIT_STACK",   "RUNLIMIT_STACK",   -1,   0,      0,    0 },
  { "RLIMIT_CPU",     "RUNLIMIT_CPU",     -1,   0,      0,    0 },
  /* list terminal: */
  { NULL, NULL, -1, 0, 0, 0}
};

/* methods in scope: */
static int runlimit_ndx(const char *name);
static void runlimit_setup_opt(const char *name, const char *arg, char opt);
static void runlimit_setup_ndx(int ndx, const char *arg, const char *source);
static void do_file(const char *rlimfile);
static void do_environment(void);


/* runlimit_ndx()
**   lookup name in runlimits[]
**   return:
**    >= 0: index to match in runlimits[]
**      -1: not found
*/
static
int
runlimit_ndx(const char *name)
{
  int  i;

  for(i = 0; runlimits[i].name != NULL; ++i){
      if(cstr_cmp(name, runlimits[i].name) == 0){
          return i;
      }
  }

  /* not found: */
  return -1;
}


/* runlimit_setup_opt()
**   set runlimit for name with arg, as given with option character opt
**   abort on fail
*/
static
void
runlimit_setup_opt(const char *name, const char *arg, char opt)
{
  char           option[2] = {opt, '\0'};
  int            ndx, id;
  struct rlimit  rlim;
  char           nbuf[NFMT_SIZE];
  uint32_t       u;
  const char    *z;

  ndx = runlimit_ndx(name);
  if(ndx == -1){
      /* shouldn't happen from command line! */
      barf(name, " is blaargh!");
  }

  id = rlimit_lookup(name);
  if(id == -1){
      warn("option -", option, ": resource ", name, " not supported on this system");
      return;
  }

  if(getrlimit(id, &rlim) == -1){
      fatal_syserr("option -", option, ": failure getrlimit() for resource ", name);
  }

  runlimits[ndx].rl_hard = rlim.rlim_max;
  if((arg[0] == '_') || (arg[0] == '^') || (arg[0] == '=')){
      runlimits[ndx].rl_soft = rlim.rlim_max;
  } else {
      z = nuscan_uint32(&u, arg);
      if(*z != '\0'){
          fatal_usage("option -", option,
                      ": non-numeric argument for resource ", name,
                      ": ", arg);
      }
      if(u < rlim.rlim_max){
          runlimits[ndx].rl_soft = (rlim_t)u;
      } else {
          warn("option -", option,
               ": truncating runlimit request for ", name,
               " to hard limit: ", nfmt_uint32(nbuf, (uint32_t)rlim.rlim_max));
          runlimits[ndx].rl_soft = rlim.rlim_max;
      }
  }
  runlimits[ndx].id = id;
  ++runlimits[ndx].is_set;

  return;
}


/* runlimit_setup_ndx()
**   setup runlimits[ndx] with runlimit specified in arg obtained from source
**   ndx previously obtained from runlimit_ndx()
**   skips any setup on any previous setting found
**   abort on failure
*/
static
void
runlimit_setup_ndx(int ndx, const char *arg, const char *source)
{
  const char     *name = runlimits[ndx].name;
  int             id;
  struct rlimit   rlim;
  uint32_t        u;
  const char     *z;
  char            nbuf[NFMT_SIZE];

  if(runlimits[ndx].is_set){
     /* skipping on previous setup: */
     return;
  }
 
  if((arg == NULL) || (arg[0] == '\0')){
      fatal_usage("NULL or empty argument for resource ", name,
                  " found in ", source);
  }

  id = rlimit_lookup(name);
  if(id == -1){
      warn("resource ", name, " from ", source, "not supported on this system");
      return;
  }

  if(getrlimit(id, &rlim) == -1){
      fatal_syserr("failure getrlimit() for resource ", name, " from ", source);
  }

  runlimits[ndx].rl_hard = rlim.rlim_max;
  if((arg[0] == '_') || (arg[0] == '^') || (arg[0] == '=')){
      runlimits[ndx].rl_soft = rlim.rlim_max;
  } else {
      z = nuscan_uint32(&u, arg);
      if(*z != '\0'){
          fatal_usage("non-numeric argument for resource ", name,
                      " from ", source, ": ", arg);
      }
      if(u < rlim.rlim_max){
          runlimits[ndx].rl_soft = (rlim_t)u;
      } else {
          warn("truncating runlimit request for ", name, " from ", source,
               " to hard limit: ", nfmt_uint32(nbuf, (uint32_t)rlim.rlim_max));
          runlimits[ndx].rl_soft = rlim.rlim_max;
      }
  }
  ++runlimits[ndx].is_set;
  runlimits[ndx].id = id;

  return;
}


/* do_rlimfile()
**   read RLIMIT_* values from rlimfile
**   abort on failure
*/
static
void
do_file(const char *rlimfile)
{
  int         fd;
  const char *source = rlimfile;
  int         eof = 0;
  ioq_t       q;
  uchar_t     qbuf[IOQ_BUFSIZE];
  dynstr_t    L = dynstr_INIT();
  char       *line = NULL;
  char       *key, *val;
  size_t      split;
  size_t      lineno = 0;
  char        nbuf[NFMT_SIZE];
  int         ndx;
  int         r;

  if(cstr_cmp(rlimfile, "-") == 0){
      fd = 0;
      source = "<stdin>";
  } else {
      fd = open(rlimfile, O_RDONLY | O_NONBLOCK);
      if(fd == -1){
          fatal_syserr("unable to open ", rlimfile);
      }
  }

  ioq_init(&q, fd, qbuf, sizeof qbuf, &read); 

  while(!eof){ 
      /* recycle any allocated dynstr: */
      dynstr_CLEAR(&L);

      /* fetch next line: */
      r = ioq_getln(&q, &L);
      if(r == -1){
          fatal_syserr("error reading ", source);
      }
      ++lineno;
      if(r == 0){
          /* set terminal condition: */
          ++eof;
          if((dynstr_STR(&L) == NULL) || (dynstr_LEN(&L) == 0)){
              /* all done: */
              break;
          }
          /* else:
          ** eof was encountered after partial line read
          ** (last line not terminated with '\n')
          ** proceed through the end of this loop 
          */
      }

      /* work directly on string buffer: */
      line = dynstr_STR(&L);
      cstr_trim(line);

      /* skip empty lines and comments: */
      if((line[0] == '\0') || (line[0] == '#')){
          continue;
      }

      /* parse line into key, value: */
      key = line;
      split = cstr_pos(key, '=');
      if(key[split] == '='){
          val = &line[split + 1];
          key[split] = '\0';
          /* trim whitespace around '=': */
          cstr_rtrim(key);
          cstr_ltrim(val);
      } else {
          errno = EPROTO;
          fatal_syserr("empty value or format error found in ", source,
                       ", line ", nfmt_uint32(nbuf, (uint32_t)lineno));
      }

      /* skip empty key: */
      if(key[0] == '\0'){
          continue;
      }

      /* setup resource: */
      if((ndx = runlimit_ndx(key)) == -1){
          errno = EPROTO;
          /* fail on bogus resource name: */
          fatal_syserr("unknown resource found in ", source,
                       ", line ", nfmt_uint32(nbuf, (uint32_t)lineno),
                       ": ", key);
      }
      /* else: */
      runlimit_setup_ndx(ndx, val, source);
  }
 
  /* success: */
  if(fd) close(fd);
  return;
}


/* do_environment()
**   scan runlimits[] for matching keys in environment
**   on found keys, set runlimit with runlimit_setup_ndx()
*/
static
void
do_environment(void)
{
  char  *val;
  int    i;
 
  for(i = 0; runlimits[i].name != NULL; ++i){
      val = getenv(runlimits[i].name);
      if(val != NULL){
          runlimit_setup_ndx(i, val, "environment");
      }
  } 

  return;
}


/* do_runlimits()
**   scan through runlimits[]
**   call setrlimit() for any entries that have been setup
**   abort on failure
*/
static
void
do_runlimits(void)
{
  struct  rlimit  rlim;
  int             i;

  for(i = 0; runlimits[i].name != NULL; ++i){
      if(runlimits[i].is_set){
          rlim.rlim_cur = runlimits[i].rl_soft;
          rlim.rlim_max = runlimits[i].rl_hard;
          if(setrlimit(runlimits[i].id, &rlim) == -1){
              fatal_syserr("failure setrlimit() for resource ", runlimits[i].name);
          }
      }
      /* else skip... */
  }

  return;
}


int
main(int argc, char *argv[], char *envp[])
{
  nextopt_t  nopt = nextopt_INIT(argc, argv, ":hVvEF:a:c:d:f:l:m:o:p:r:s:t:");
  char       opt;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'V': version(); die(0); break;
      case 'h': usage(); die(0); break;
      case 'v': ++verbose; break;
      case 'E': ++opt_E; break;
      case 'F': ++opt_F; arg_F = nopt.opt_arg; break;
      case 'a': runlimit_setup_opt("RLIMIT_AS", nopt.opt_arg, opt); break;
      case 'c': runlimit_setup_opt("RLIMIT_CORE", nopt.opt_arg, opt); break;
      case 'd': runlimit_setup_opt("RLIMIT_DATA", nopt.opt_arg, opt); break;
      case 'f': runlimit_setup_opt("RLIMIT_FSIZE", nopt.opt_arg, opt); break;
      case 'l': runlimit_setup_opt("RLIMIT_MEMLOCK", nopt.opt_arg, opt); break;
      case 'm':
          /* all of _AS/_VMEM _DATA _STACK _MEMLOCK */
          runlimit_setup_opt("RLIMIT_AS", nopt.opt_arg, opt);
          runlimit_setup_opt("RLIMIT_DATA", nopt.opt_arg, opt);
          runlimit_setup_opt("RLIMIT_MEMLOCK", nopt.opt_arg, opt);
          runlimit_setup_opt("RLIMIT_STACK", nopt.opt_arg, opt);
          break;
      case 'o': runlimit_setup_opt("RLIMIT_NOFILE", nopt.opt_arg, opt); break;
      case 'p': runlimit_setup_opt("RLIMIT_NPROC", nopt.opt_arg, opt); break;
      case 'r': runlimit_setup_opt("RLIMIT_RSS", nopt.opt_arg, opt); break;
      case 's': runlimit_setup_opt("RLIMIT_STACK", nopt.opt_arg, opt); break;
      case 't': runlimit_setup_opt("RLIMIT_CPU", nopt.opt_arg, opt); break;
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

  if(argv[0] == NULL){
      fatal_usage("missing program argument");
  }

  /* precedence:
  **   cmdline overrides all
  **   rlimit file overrides environment
  **   environment if not otherwise set
  */
  if(opt_F){
      /* setup rlimits from file: */
      do_file(arg_F);
  }

  if(opt_E){
      /* setup rlimits from environment */
      do_environment();
  }

  /* process rlimits: */
  do_runlimits();

  /* execvx() provides path search for prog */
  execvx(argv[0], argv, envp, NULL);

  /* uh oh: */
  fatal_syserr("unable to run ", argv[0]);

  /* not reached: */
  return 0;
}


/* eof: runlimit.c */
