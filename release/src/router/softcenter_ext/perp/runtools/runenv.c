/* runenv.c
** exec prog with modified environment
** (extended version of djb envdir facility, somewhat)
** wcm, 2009.09.08 - 2011.01.31
** ===
*/
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>

#include "cstr.h"
#include "dynstr.h"
#include "ioq.h"
#include "newenv.h"
#include "nextopt.h"
#include "nfmt.h"

#include "runtools_common.h"

static const char *progname = NULL;
static const char prog_usage[] =
  "[-hV] [-i] [-U account] newenv program [args ...]";


static char *val_unescape(char *s);
static int do_envfile(const char *envfile);
static int do_envdir(const char *envdir);
static int do_envuid(const char *account);


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
int
do_envfile(const char *envfile)
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

  if(cstr_cmp(envfile, "-") == 0){
      fd = 0;
      envfile = "<stdin>";
  } else {
      fd = open(envfile, O_RDONLY | O_NONBLOCK);
      if(fd == -1){
          fatal_syserr("unable to open ", envfile);
      }
  }

  ioq_init(&q, fd, qbuf, sizeof qbuf, &read); 

  while(!eof){ 
      /* recycle any allocated dynstr: */
      dynstr_CLEAR(&L);

      /* fetch next line: */
      r = ioq_getln(&q, &L);
      if(r == -1){
          fatal_syserr("error reading ", envfile);
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
  return 0;
}


static
int
do_envdir(const char *envdir)
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
      fatal_syserr("unable to open current directory");
  }
  err = chdir(envdir);
  if(err == -1){
      fatal_syserr("unable to chdir to ", envdir);
  }

  dir = opendir(".");
  if(!dir){
      fatal_syserr("unable to open directory ", envdir);
  }

  for(;;){
      int   fd;
      char *line = NULL;

      errno = 0;
      d = readdir(dir);
      if(!d){
          if(errno)
              fatal_syserr("unable to read directory ", envdir);
          /* else all done: */
          break;
      }

      /* skip any dot files: */
      if(d->d_name[0] == '.')
          continue;

      fd = open(d->d_name, O_RDONLY | O_NONBLOCK);
      if(fd == -1){
          fatal_syserr("unable to open ", envdir, "/", d->d_name);
      }

      /* prepare ioq buffer and recycle line buffer: */
      ioq_init(&q, fd, qbuf, sizeof qbuf, &read);
      dynstr_CLEAR(&L);

      /* one line read: */
      err = ioq_getln(&q, &L);
      if(err == -1){
          fatal_syserr("unable to read ", envdir, "/", d->d_name);
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
      fatal_syserr("failure changing to original directory");
  }
  close(fd_orig);
  return 0;
}


static
int
do_envuid(const char *account)
{
  struct passwd  *pw = NULL;
  char            nfmt[NFMT_SIZE];
  int             err; 

  pw = getpwnam(account);
  if(pw == NULL){
      fatal(111, "no account for user ", account);
  }

  err = newenv_set("GID", nfmt_uint32(nfmt, (uint32_t)pw->pw_gid));
  if(err){
      fatal_syserr("failure allocating new environment");
  }    
  err = newenv_set("UID", nfmt_uint32(nfmt, (uint32_t)pw->pw_uid));
  if(err){
      fatal_syserr("failure allocating new environment");
  }    
  
  return 0;
}


int
main(int argc, char *argv[], char *envp[])
{
  nextopt_t    nopt = nextopt_INIT(argc, argv, ":hViU:");
  char         opt;
  const char  *newenv = NULL;
  const char  *arg_envuid = NULL;
  int          opt_merge = 1;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
      case 'i': opt_merge = 0; break;
      case 'U': arg_envuid = nopt.opt_arg; break;
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

  if(argc < 2){
      fatal_usage("missing required argument(s)");
  }
  newenv = argv[0];
  ++argv;

  /* newenv may be file or directory: */
  if(cstr_cmp(newenv, "-") == 0){
      do_envfile(newenv);
  } else {
      struct stat  sb;
      if(stat(newenv, &sb) == -1){
          fatal_syserr("unable to stat ", newenv);
      }
      if(S_ISREG(sb.st_mode)){
          do_envfile(newenv);
      } else if(S_ISDIR(sb.st_mode)){
          do_envdir(newenv);
      } else {
          fatal_usage("argument is not a file or directory: ", newenv);
      }
  }

  if(arg_envuid){
      do_envuid(arg_envuid);
  }

  /* execvx() provides shell-like path search for argv[0] */
  newenv_run(argv, (opt_merge ? envp : NULL));

  /* uh oh: */
  fatal_syserr("unable to run ", argv[0]);

  /* not reached: */
  return 0;
}


/* eof: runenv.c */
