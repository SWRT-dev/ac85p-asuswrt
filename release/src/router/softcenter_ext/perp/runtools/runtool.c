/* runtool.c
** multipurpose runtool: run program in configured process environment
** wcm, 2009.09.08 - 2011.01.31
** ===
*/
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>

#include "buf.h"
#include "cstr.h"
#include "dynstr.h"
#include "dynstuf.h"
#include "fd.h"
#include "nextopt.h"
#include "newenv.h"
#include "nfmt.h"
#include "nuscan.h"
#include "rlimit.h"
#include "pidlock.h"
#include "sig.h"

#include "runtools_common.h"

static const char *progname = NULL;
static const char prog_usage[] =
  "[-hV]"
  " [-0 argv0]"
  " [<-a | -A> argfile]"
  " [-c chdir]"
  " [-C chroot]"
  " [-d]"
  " [<-e | -E> envfile]"
  " [-F fdset]"
  " [-L [:]lockfile | -P [:]pidlock]"
  " [-m umask]"
  " [-R <a|c|d|f|m|o|p|r|s|t>=<num|!>[:...]]"
  " [-s]"
  " [-S altpath]"
  " [-u user]"
  " [-U user]"
  " [-z secs]"
  " [-W]"
  " program [args ...]";

static int  warn = 0;

#define fatal_alloc(...) \
  fatal(111, "allocation failure: ", __VA_ARGS__)

#define warning(...) \
  {\
      if(warn){\
          eputs(progname, ": warning: ", __VA_ARGS__);\
      }\
  }


/* runlock: */
static void do_setlock(const char *arg, pid_t pid);
/* runargs: */
static struct dynstuf arglist = dynstuf_INIT();
static void arglist_add(const char *arg);
static void do_argfile(const char *optc, const char *argfile);
/* runenv: */
static char * val_unescape(char  *s);
static void do_envfile(const char *optc, const char *arg);
static void do_envdir(const char *optc, const char *arg);
static void do_env(const char *optc, const char *arg_env);
/* runenv + envuid: */
static void do_envuid(const char *user);
/* runlimit: */
static void runlimit_set(const char *rdef, const char *arg);
static void do_runlimit(const char *arg_rlimit);
/* runuid: */
static void do_setuid(const char *user);
/* fdset: */
static void fdset_redirect(int fd, char op, const char *target);
static void fdset_duplicate(int fd, char target);
static void do_fdset(const char *arg_fdset);
/* chdir: */
static void do_chdir(const char *arg);
/* chroot: */
static void do_chroot(const char *arg);
/* umask: */
static void do_umask(const char *arg);
/* sleep: */
static void sig_trap(int sig);
static void do_sleep(const char *arg);


/*
**  runlock
*/

static
void
do_setlock(const char *arg, pid_t pid)
{
  const char  *lockfile = arg;
  int          fd;
  int          block = 0;

  if(arg[0] == ':'){
      if(arg[1] == '\0'){
          fatal_usage("empty lockfile argument with option ", (pid ? "-P" : "-L"));
      }
      block = 1;
      lockfile = &arg[1];
  }

  fd = pidlock_set(lockfile, pid, (block ? PIDLOCK_WAIT : PIDLOCK_NOW));
 
  if(fd == -1){
      fatal_syserr("unable to acquire lock on", lockfile);
  }

  return;
}



/*
**  runargs
*/

static
void
arglist_add(const char *arg)
{
  char *newarg;

  /* NULL arg acceptable to terminate arglist: */
  if(arg == NULL){
      newarg = NULL;
  } else {
      newarg = cstr_dup(arg);
      if(newarg == NULL){
          fatal_syserr("failure duplicating new argv");
      }
  }

  if(dynstuf_push(&arglist, newarg) == -1){
      fatal_syserr("failure allocating new argv");
  }

  return;
}


static
void
do_argfile(const char *optc, const char *argfile)
{
  int        fd;
  int        eof = 0;
  ioq_t      q;
  uchar_t    qbuf[IOQ_BUFSIZE];
  dynstr_t   L = dynstr_INIT();
  char      *line = NULL;
  int        r;

  fd = open(argfile, O_RDONLY | O_NONBLOCK);
  if(fd == -1){
      fatal_syserr("unable to open file for option -", optc, " ", argfile);
  }

  ioq_init(&q, fd, qbuf, sizeof qbuf, &read); 

  while(!eof){ 
      /* recycle any allocated dynstr: */
      dynstr_CLEAR(&L);

      /* fetch next line: */
      r = ioq_getln(&q, &L);
      if(r == -1){
          fatal_syserr("error reading argument file for option -", optc, " ", argfile);
      }
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

      /* add this argument: */
      arglist_add(line);
  }

  /* success: */
  if(fd) close(fd);
  return;
}



/*
**  runenv
*/

static
char *
val_unescape(char  *s)
{
  char    *new = s;
  char    *c;

  for(c = s; *c != '\0'; ++c){
      if(*c == '\\'){
          ++c;
          switch(*c){
          /* recognize a few escape sequences in value string: */
          case '\\': *new++ = '\\'; break;
          case 'n' : *new++ = '\n'; break;
          case 't' : *new++ = '\t'; break;
          /* "protected" space (use to preserve from cstr_trim()): */
          case '_' : *new++ = ' ' ; break;
          /* if string ends with single backslash: */
          case '\0':
              *new++ = '\\'; continue; break;
          default:
              /* escape sequence not defined, leave verbatim: */
              *new++ = '\\'; *new++ = *c; break;
          }
      } else {
          *new++ = *c;
      }
  }

  *new = '\0';
  return s;
}


static
void
do_envfile(const char *optc, const char *arg)
{
  int        fd;
  int        eof = 0;
  ioq_t      q;
  uchar_t    qbuf[IOQ_BUFSIZE];
  dynstr_t   L = dynstr_INIT();
  char      *line = NULL;
  char      *key, *val;
  size_t     split;
  int        r;

  fd = open(arg, O_RDONLY | O_NONBLOCK);
  if(fd == -1){
      fatal_syserr("failure opening file for option -", optc, ": ", arg);
  }

  ioq_init(&q, fd, qbuf, sizeof qbuf, &read); 

  while(!eof){ 
      /* recycle any allocated dynstr: */
      dynstr_CLEAR(&L);

      /* fetch next line: */
      r = ioq_getln(&q, &L);
      if(r == -1){
          fatal_syserr("error reading file for option -", optc, ": ", arg);
      }
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
          /* process escape sequences in val: */
          val_unescape(val);
      } else {
          /* no value sets up delete of existing variable: */
          val = NULL;
      }

      /* skip empty key: */
      if(key[0] == '\0'){
          continue;
      }

      /* add new environment variable
      ** (null value sets up delete of existing variable)
      */
      if(newenv_set(key, val) == -1){
          fatal_syserr("failure allocating new environment");
      }

  }
 
  /* success: */
  if(fd) close(fd);
  return;
}


static
void
do_envdir(const char *optc, const char *arg)
{
  int             fd_orig;
  DIR            *dir;
  struct dirent  *d;
  ioq_t           q;
  uchar_t         qbuf[IOQ_BUFSIZE];
  dynstr_t        L = dynstr_INIT();
  int             err;

  fd_orig = open(".", O_RDONLY | O_NONBLOCK);
  if(fd_orig == -1){
      fatal_syserr("unable to open current directory (option -", optc, " ", arg, ")");
  }
  err = chdir(arg);
  if(err == -1){
      fatal_syserr("unable to chdir (option -", optc, " ", arg, ")");
  }

  dir = opendir(".");
  if(!dir){
      fatal_syserr("unable to open directory (option -", optc, " ", arg, ")");
  }

  for(;;){
      int   fd;
      char *line = NULL;

      errno = 0;
      d = readdir(dir);
      if(!d){
          if(errno)
              fatal_syserr("unable to read directory (option -", optc, " ", arg, ")");
          /* else all done: */
          break;
      }

      /* skip any dot files: */
      if(d->d_name[0] == '.')
          continue;

      fd = open(d->d_name, O_RDONLY | O_NONBLOCK);
      if(fd == -1){
          fatal_syserr("unable to open ", arg, "/", d->d_name, " (option -", optc, " ", arg, ")");
      }

      /* prepare ioq buffer and recycle line buffer: */
      ioq_init(&q, fd, qbuf, sizeof qbuf, &read);
      dynstr_CLEAR(&L);

      /* one line read: */
      err = ioq_getln(&q, &L);
      if(err == -1){
          fatal_syserr("unable to read ", arg, "/", d->d_name, " (option -", optc, " ", arg, ")");
      }
      close(fd);

      /* work directly on line buffer: */
      line = dynstr_STR(&L);
      if(line){
          cstr_trim(line);
          val_unescape(line);
      }

      /* add new environmental variable
      ** (null value sets up delete of existing variable)
      */
      err = newenv_set(d->d_name, ((line && line[0]) ? line : NULL));
      if(err == -1){
          fatal_syserr("failure allocating new environment");
      }
  }

  closedir(dir);
  if(fchdir(fd_orig) == -1){
      fatal_syserr("failure changing to original directory (option -", optc, " ", arg, ")");
  }
  close(fd_orig);
  return;
}


static
void
do_env(const char *optc, const char *arg)
{
  struct stat  sb;

  if(stat(arg, &sb) == -1){
      fatal_syserr("failure stat() on path argument for option -", optc, ": ", arg);
  }

  if(S_ISREG(sb.st_mode)){
      do_envfile(optc, arg);
  } else if (S_ISDIR(sb.st_mode)){
      do_envdir(optc, arg);
  } else {
      fatal_usage("argument is neither file nor directory for option -", optc, ": ", arg);
  }

  /* not reached: */
  return;
}



/*
**  runenv + envuid
*/

static
void
do_envuid(const char *user)
{
  struct passwd  *pw = NULL;
  char            nfmt[NFMT_SIZE];
  int             err;

  pw = getpwnam(user);
  if(pw == NULL){
      fatal_usage("no user account found for envuid option -U: ", user);
  }

  err = newenv_set("GID", nfmt_uint32(nfmt, (uint32_t)pw->pw_gid));
  if(err){
      fatal_syserr("failure allocating new environment");
  }    
  err = newenv_set("UID", nfmt_uint32(nfmt, (uint32_t)pw->pw_uid));
  if(err){
      fatal_syserr("failure allocating new environment");
  }    

  return; 
}



/*
**  runlimit
*/

static
void
runlimit_set(const char *rdef, const char *arg)
{
  int            r;
  struct rlimit  rlim;
  uint32_t       u;
  const char    *z;
  
  r = rlimit_lookup(rdef);
  if(r == -1){
      warning("resource limit ", rdef, " not supported on this system");
      return;
  }

  if(getrlimit(r, &rlim) == -1){
      fatal_syserr("failure getrlimit() for resource ", rdef);
  }

  if(arg[0] == '!'){
      rlim.rlim_cur = rlim.rlim_max;
  } else {
      z = nuscan_uint32(&u, arg);
      if(*z != '\0'){
          fatal_usage("bad numeric argument for runlimit option -R :", arg);
      }
      rlim.rlim_cur = (u < rlim.rlim_max ? u : rlim.rlim_max);
  }

  if(setrlimit(r, &rlim) == -1){
      fatal_syserr("failure setrlimit() for resource ", rdef);
  }

  return;
}


static
void
do_runlimit(const char *arg_rlimit)
{
  const char  *arg = arg_rlimit;
  char         r;
  char         nbuf[40];
  size_t       tok;

  while(*arg != '\0'){
      r = *arg;
      ++arg;
      if(*arg != '='){
          fatal_usage("bad format in runlimit argument ", arg_rlimit);
      }
      ++arg;
      tok = cstr_pos(arg, ':');
      if(tok > 0){
          if(tok > sizeof nbuf){
              fatal_usage("bad format in runlimit argument ", arg_rlimit);
          }
          buf_copy(nbuf, arg, tok);
          arg += tok;
      }
      nbuf[tok] = '\0';
      if(nbuf[0] != '\0'){
          switch(r){
          case 'a' : runlimit_set("RLIMIT_AS", nbuf); break;
          case 'c' : runlimit_set("RLIMIT_CORE", nbuf); break;
          case 'd' : runlimit_set("RLIMIT_DATA", nbuf); break;
          case 'f' : runlimit_set("RLIMIT_FSIZE", nbuf); break;
          case 'm' :
              runlimit_set("RLIMIT_AS", nbuf);
              runlimit_set("RLIMIT_DATA", nbuf);
              runlimit_set("RLIMIT_STACK", nbuf);
              runlimit_set("RLIMIT_MEMLOCK", nbuf);
              break;
          case 'o' : runlimit_set("RLIMIT_NOFILE", nbuf); break;
          case 'p' : runlimit_set("RLIMIT_NPROC", nbuf); break;
          case 'r' : runlimit_set("RLIMIT_RSS", nbuf); break;
          case 's' : runlimit_set("RLIMIT_STACK", nbuf); break;
          case 't' : runlimit_set("RLIMIT_CPU", nbuf); break;
          default  :
              fatal_usage("bad format in runlimit argument ", arg_rlimit);
              break;
          }
      }
      if(*arg == ':') ++arg;
  }
          
  return;
}




/*
**  runuid
*/

static
void
do_setuid(const char *user)
{
  struct passwd  *pw = NULL;
  uid_t           uid;
  gid_t           gid;

  if(getuid() != 0){
      fatal_usage("must have root privilege to setuid");
  }

  pw = getpwnam(user);
  if(pw == NULL){
      fatal_usage("no user account found for setuid option -u: ", user);
  }

  uid = pw->pw_uid;
  gid = pw->pw_gid;

  if(setgroups(1, &gid) == -1){
      fatal_syserr("failure setgroups() for setuid option -u: ", user);
  }

  if(setgid(gid) == -1){
      fatal_syserr("failure setgid() for setuid option -u: ", user);
  }

  if(setuid(uid) == -1){
      fatal_syserr("failure setuid() for setuid option -u: ", user);
  } 

  return; 
}



/*
**  fdset
*/
static
void
fdset_redirect(int fd, char op, const char *target)
{
  int          fdx = -1;
  char         t = target[0];

  if(! ((t == '/') || (t == '.') || (t == '%'))){
      fatal_usage("bad target specification in fdset argument ", target);
  }
  if(t == '%'){
      if(target[1] != '\0'){
          fatal_usage("bad target specification in fdset argument ", target);
      }
      /* else: */
      target = "/dev/null";
  }

  switch(op){
  case '<' : /* input redirection */
      if((fdx = open(target, O_RDONLY)) == -1){
          fatal_syserr("failure opening ", target, " for input redirection");
      }
      break;
  case '>' : /* output redirection, clobber */
      if((fdx = open(target, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0600)) == -1){
          fatal_syserr("failure opening ", target, " for output redirection");
      }
      break;
  case '+' : /* output redirection, append */
      if((fdx = open(target, O_WRONLY | O_CREAT | O_APPEND, 0600)) == -1){
          fatal_syserr("failure opening ", target, " for output redirection");
      }
      break;
  }

  if(fd_move(fd, fdx) == -1){
      fatal_syserr("failure duplicating file descriptor for ", target);
  }

  return;
}

static
void
fdset_duplicate(int fd, char target)
{
  int fdx;

  if(target == '!'){
      if(close(fd) == -1){
         warning("failure on closing file descriptor");
      }
      return;
  }

  /* else: */
  fdx = (int)(target - '0');
  if((fdx < 0) || (fdx > 9)){
      fatal_usage("bad fd target for fdset duplication");
  }

  if(fd_dupe(fd, fdx) == -1){
      fatal_syserr("failure duplicating file descriptor");
  }

  return;
}

static
void
do_fdset(const char *arg_fdset)
{
  const char  *arg = arg_fdset;
  int          fd;
  char         op;
  char         target[400];
  size_t       tok;

#if 0
  parse instructions from argument string in the format:
      <fd> <op> <target> [:...]
  where:
      <fd>: single ascii numeral, 0..9
      <op>: '<' | '>' | '+' | '='
      <target> : depends on <op>:
        '<' | '>' | '+' :
            redirection operator, target is:
                <pathname>, (must begin with '.' or '/'); or
                "%", a special string interpreted as "/dev/null"
        '=' :
            duplication operator, target is:
                single ascii numeral, 0..9; or
                "!", instruction to close <fd>

   concatenation:
       multiple instructions may be concatenated with ':'

   examples:
       "0</dev/null"      redirect stdin reading from /dev/null
       "2+./error.log"    redirect stderr writing to ./error.log in append mode
       "2=1"              duplicate stderr to stdin
       "0=!"              close stdin
       "0<%:1>%:2>%"      redirect stdin, stdout, stderr to /dev/null
#endif

  while(*arg != '\0'){
      fd = (int)(*arg - '0');
      if((fd < 0) || (fd > 9)){
          fatal_usage("bad fd numeral in fdset argument ", arg_fdset);
      }
      ++arg;
      op = *arg;
      if(!((op == '<') || (op == '>') || (op == '+') || (op == '='))){
          fatal_usage("bad op format in fdset argument ", arg_fdset);
      }
      ++arg;
      tok = cstr_pos(arg, ':');
      if(tok == 0){
          fatal_usage("empty target in fdset argument ", arg_fdset);
      }
      /* (else tok > 0) */
      if(tok > sizeof target){
          fatal_usage("target too long in fdset argument ", arg_fdset);
      }
      buf_copy(target, arg, tok);
      target[tok] = '\0';
      /* perform op on fd with target: */
      if(op == '='){
          /* duplication: target must be a single character: */
          if(target[1] != '\0'){
              fatal_usage("bad duplication target in fdset argument ", arg_fdset);
          }
          fdset_duplicate(fd, *target);
      } else {
          /* '<' '>' '+' redirection: */
          fdset_redirect(fd, op, target);
      }

      /* setup for next while loop: */
      arg += tok;
      if(*arg == ':') ++arg;
  }
          
  return;
}



/*
**  chdir
*/
static
void
do_chdir(const char *arg)
{
  if(chdir(arg) == -1){
      fatal_syserr("failure on chdir to ", arg);
  }

  return;
}



/*
**  chroot
*/

static
void
do_chroot(const char *arg)
{
  if(getuid() != 0){
      fatal_usage("must have root privilege to chroot");
  }
  if(chroot(arg) == -1){
      fatal_syserr("failure on chroot to ", arg);
  }

  return;
}



/*
**  umask
*/
static
void
do_umask(const char *arg)
{
  const char   *z;
  uint32_t      mode;

  z = nuscan_uint32o(&mode, arg);
  if(*z != '\0'){
      fatal_usage("bad octal numeric argument for option -m: ", arg);
  }
  umask((mode_t)mode);

  return;
}


/*
**  sleep
*/
static
void
sig_trap(int sig)
{
  /* catch signal, do nothing: */
  (void)sig;
  return;
}

static
void
do_sleep(const char *arg)
{
  uint32_t     secs;
  const char  *z;

  z = nuscan_uint32(&secs, arg);
  if(*z != '\0'){
      fatal_usage("bad numeric argument for sleep option -z: ", arg);
  }

  /* catch SIGALRM on pause()/sleep() without termination: */
  sig_catch(SIGALRM, sig_trap);

  if(secs == 0)
      pause();
  else
      sleep(secs);

  sig_uncatch(SIGALRM);
  errno = 0; 

  return;
}


int
main(int argc, char *argv[], char *envp[])
{
  nextopt_t  nopt = nextopt_INIT(argc, argv,
                    ":hV0:a:A:c:C:de:E:F:L:m:P:R:sS:u:U:Wz:");
  char       opt;

  int        opt_arginsert = 0;
  int        opt_detach = 0;
  int        opt_envmerge = 1;
  int        opt_setsid = 0;
  char      *prog = NULL;
  char      *arg_alias = NULL;
  char      *arg_argfile = NULL;
  char      *arg_chdir = NULL;
  char      *arg_chroot = NULL;
  char      *arg_env = NULL;
  char      *arg_envuid = NULL;
  char      *arg_fdset = NULL;
  char      *arg_pidlock = NULL;
  char      *arg_rlimit = NULL;
  char      *arg_setlock = NULL;
  char      *arg_setuid = NULL;
  char      *arg_search = NULL;
  char      *arg_sleep = NULL;
  char      *arg_umask = NULL;
  pid_t      pid;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
      case '0': arg_alias = nopt.opt_arg; break;
      case 'a': opt_arginsert = 0; arg_argfile = nopt.opt_arg; break;
      case 'A': opt_arginsert = 1; arg_argfile = nopt.opt_arg; break;
      case 'c': arg_chdir = nopt.opt_arg; break;
      case 'C': arg_chroot = nopt.opt_arg; break;
      case 'd': opt_detach = 1; opt_setsid = 1; break;
      case 'e': opt_envmerge = 1; arg_env = nopt.opt_arg; break;
      case 'E': opt_envmerge = 0; arg_env = nopt.opt_arg; break;
      case 'F': arg_fdset = nopt.opt_arg; break;
      case 'L': arg_setlock = nopt.opt_arg; break;
      case 'm': arg_umask = nopt.opt_arg; break;
      case 'P': arg_pidlock = nopt.opt_arg; break;
      case 'R': arg_rlimit = nopt.opt_arg; break;
      case 's': opt_setsid = 1; break;
      case 'S': arg_search = nopt.opt_arg; break;
      case 'u': arg_setuid = nopt.opt_arg; break;
      case 'U': arg_envuid = nopt.opt_arg; break;
      case 'W': warn = 1; break;
      case 'z': arg_sleep = nopt.opt_arg; break;
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

  if(argc < 1){
      fatal_usage("missing required program argument");
  }

  if(arg_setlock && arg_pidlock){
      fatal_usage("options -L (setlock) and -P (pidlock) are mutually exclusive");
  }

  prog = argv[0];
  if(arg_alias) argv[0] = arg_alias;

  if(arg_argfile){
      /* set argv[0]: */
      arglist_add((arg_alias ? arg_alias : prog));
      ++argv;
 
      if(opt_arginsert){
          while(*argv != NULL){
              arglist_add(*argv);
              ++argv;
          }
      }

      do_argfile((opt_arginsert ? "A" : "a"), arg_argfile);

      if(!opt_arginsert){
          while(*argv != NULL){
              arglist_add(*argv);
              ++argv;
          }
      }

      /* append NULL to arglist: */
      arglist_add(NULL);
  }

  if(arg_env){
      do_env((opt_envmerge ? "e" : "E"), arg_env);
  }

  if(arg_envuid){
      do_envuid(arg_envuid);
  }

  if(opt_detach){
      if((pid = fork()) == -1){
          fatal_syserr("failure to detach");
      }
      if(pid != 0){
          /* parent exits: */
          _exit(0);
      }
  }

  if(opt_setsid){
      /* start new session group (warn failure): */
      if(setsid() == -1){
          warning("failure setsid() for option -s: ",
                  sysstr_errno_mesg(errno),
                  " (", sysstr_errno(errno), ")");
      }
  }

  /* setlock after detach so lock held by new pid: */
  if(arg_setlock){
      do_setlock(arg_setlock, 0);
  } else if(arg_pidlock){
      do_setlock(arg_pidlock, getpid());
  }

  if(arg_umask){
      do_umask(arg_umask);
  }

  if(arg_fdset){
      do_fdset(arg_fdset);
  }

  if(arg_rlimit){
      do_runlimit(arg_rlimit);
  }

  if(arg_chdir){
      do_chdir(arg_chdir);
  }

  if(arg_chroot){
      do_chroot(arg_chroot);
  }

  if(arg_setuid){
      do_setuid(arg_setuid);
  }

  if(arg_sleep){
      do_sleep(arg_sleep);
  }

  newenv_exec(
     prog,
     (arg_argfile ? (char **)dynstuf_STUF(&arglist) : argv),
     arg_search,
     (opt_envmerge ? envp : NULL));
     
  /* uh oh: */
  fatal_syserr("unable to run ", argv[0]);

  /* not reached: */
  return 0;
}


/* eof: runtool.c */
