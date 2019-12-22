/* runfile.c
** exec argv read from file
** wcm, 2009.09.28 - 2011.01.31
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
static const char prog_usage[] =
  "[-hV] argvfile [args ...]";

#define fatal_alloc() \
  fatal(111, "allocation failure");


/* arglist in scope: */
static dynstuf_t arglist = dynstuf_INIT();

static void arglist_add(const char *arg);
static void do_argvfile(const char *argvfile);


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
do_argvfile(const char *argvfile)
{
  int        fd;
  int        eof = 0;
  ioq_t      q;
  uchar_t    qbuf[IOQ_BUFSIZE];
  dynstr_t   L = dynstr_INIT();
  char      *line = NULL;
  int        r;

  if(cstr_cmp(argvfile, "<stdin>") == 0){
      fd = 0;
  } else {
      fd = open(argvfile, O_RDONLY | O_NONBLOCK);
      if(fd == -1){
          fatal_syserr("unable to open ", argvfile);
      }
  }

  ioq_init(&q, fd, qbuf, sizeof qbuf, &read); 

  while(!eof){ 
      /* recycle any allocated dynstr: */
      dynstr_CLEAR(&L);

      /* fetch next line: */
      r = ioq_getln(&q, &L);
      if(r == -1){
          fatal_syserr("error reading ", argvfile);
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
  if(fd != 0) close(fd);
  return;
}


int
main(int argc, char * argv[], char * envp[])
{
  nextopt_t    nopt = nextopt_INIT(argc, argv, "hV");
  char         opt;
  const char  *argvfile = NULL;
  const char  *prog = NULL;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
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
      fatal_usage("missing required filename");
  }
  argvfile = argv[0];
  ++argv;

  if(cstr_cmp(argvfile, "-") == 0){
      argvfile = "<stdin>";
  }

  /* process argvfile: */
  do_argvfile(argvfile);

  /* require argv[0] in argvfile: */
  prog = (const char *)dynstuf_get(&arglist, 0);
  if(prog == NULL){
      fatal_usage("empty argvfile: no argv[0] element found in ", argvfile);
  }

  /* append any additional command line arguments: */
  while(*argv != NULL){
      arglist_add(*argv);
      ++argv;
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


/* eof: runfile.c */
