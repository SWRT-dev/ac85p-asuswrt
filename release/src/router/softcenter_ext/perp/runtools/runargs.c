/* runargs.c
** exec prog with args read from file
** wcm, 2009.09.08 - 2011.01.31
** ===
*/

#include <unistd.h>
#include <fcntl.h>

#include "cstr.h"
#include "dynstr.h"
#include "dynstuf.h"
#include "execvx.h"
#include "ioq.h"
#include "nextopt.h"

#include "runtools_common.h"


static const char *progname = NULL;
static const char prog_usage[] = "[-hV] [-i] argfile program [args ...]";

#define fatal_alloc() \
  fatal(111, "allocation failure");


/* arglist in scope: */
static dynstuf_t arglist = dynstuf_INIT();

static void arglist_add(const char *arg);
static void do_argfile(const char *argfile);


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
          fatal_alloc();
      }
  }

  if(dynstuf_push(&arglist, newarg) == -1){
      fatal_alloc();
  }

  return;
}


static
void
do_argfile(const char *argfile)
{
  int        fd;
  int        eof = 0;
  ioq_t      q;
  uchar_t    qbuf[IOQ_BUFSIZE];
  dynstr_t   L = dynstr_INIT();
  char      *line = NULL;
  int        r;

  if(cstr_cmp(argfile, "-") == 0){
      fd = 0;
      argfile = "<stdin>";
  } else {
      fd = open(argfile, O_RDONLY | O_NONBLOCK);
      if(fd == -1){
          fatal_syserr("unable to open ", argfile);
      }
  }

  ioq_init(&q, fd, qbuf, sizeof qbuf, &read); 

  while(!eof){ 
      /* recycle any allocated dynstr: */
      dynstr_CLEAR(&L);

      /* fetch next line: */
      r = ioq_getln(&q, &L);
      if(r == -1){
          fatal_syserr("error reading ", argfile);
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


int
main(int argc, char * argv[], char * envp[])
{
  nextopt_t    nopt = nextopt_INIT(argc, argv, "hVi");
  char         opt;
  const char  *argfile = NULL;
  const char  *prog = NULL;
  int          opt_insert = 0;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
      case 'i': opt_insert = 1; break;
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
  argfile = argv[0];
  ++argv;
  prog = argv[0];
  ++argv;

  /* set argv[0] of new arglist to prog: */
  arglist_add(prog);

  /*
  ** "opt_insert"  :  prog args before argfile args
  ** "!opt_insert" :  argfile args before prog args
  **
  ** "!opt_insert" (opt_insert == 0) is the default
  */
  if(opt_insert){
      while(*argv != NULL){
          arglist_add(*argv);
          ++argv;
      }
  }

  /* process argfile: */
  do_argfile(argfile);

  if(!opt_insert){
      while(*argv != NULL){
          arglist_add(*argv);
          ++argv;
      }
  }

  /* append NULL terminal to arglist: */
  arglist_add(NULL);

  /* execvx() provides shell-like path search for prog */
  execvx(prog, (char **)dynstuf_STUF(&arglist), envp, NULL);

  /* uh oh: */
  fatal_syserr("unable to run ", prog);

  /* not reached: */
  return 0;
}


/* eof: runargs.c */
