/* nextopt.c
** command-line option parser
** wcm, 2009.11.11 - 2010.12.01
** ===
*/
#include <stddef.h>

#include "cstr.h"

#include "nextopt.h"

/*
** adapted from original public domain reference implementation of getopt(3)
*/

#if 0
struct nextopt {
  char          opt_got;  /* actual option letter found */
  char         *opt_arg;  /* option argument found */
  const char   *opt_list; /* valid option specification */
  size_t        arg_ndx;  /* current argv index */
  /*
  ** private!
  */
  size_t        _argc;    /* arg count */
  char * const *_argv;    /* argument list */
  size_t        _ondx;    /* option cursor position */
}
typedef struct nextopt  nextopt_t;
#endif


void
nextopt_start(
  struct nextopt *nopt,
  int             argc,
  char * const    argv[],
  const char     *optlist,
  size_t          start
){
  nopt->opt_got  = '\0';
  nopt->opt_arg  = NULL;
  nopt->opt_list = optlist;
  nopt->arg_ndx  = start;
  nopt->_argc    = (size_t)argc;
  nopt->_argv    = argv;
  nopt->_ondx    = 1;

  return;
}


char
nextopt(struct nextopt *nopt)
{
    char         opt_got;
    const char  *opt_lookup;
    const char  *optlist = nopt->opt_list;
#   define       ARG_NDX  nopt->arg_ndx
#   define       OPT_NDX  nopt->_ondx

    nopt->opt_got = '\0';
    nopt->opt_arg = NULL;

    if(OPT_NDX == 1){
        /* starting new option argument: */
        /* check terminal conditions: */
        if((ARG_NDX >= nopt->_argc)
           || (nopt->_argv[ARG_NDX][0] != '-')
           || (nopt->_argv[ARG_NDX][1] == '\0')){
            /* no more args, not an option arg, or '-' only: */
            return '\0';
        } else if(cstr_cmp(nopt->_argv[ARG_NDX], "--") == 0){
            /* skip arg '--' flagging end of options: */
            ++ARG_NDX;
            return '\0';
        }
    } 

    /* read current option letter: */
    opt_got = nopt->opt_got = nopt->_argv[ARG_NDX][OPT_NDX];
    /* advance option cursor: */
    ++OPT_NDX;
    /* end of this arg? */
    if(nopt->_argv[ARG_NDX][OPT_NDX] == '\0'){
        /* advance argv index, reset option cursor: */
        ++ARG_NDX;
        OPT_NDX = 1;
    }

    /* opt_got valid? */
    opt_lookup = &optlist[cstr_pos(optlist, opt_got)];
    if((opt_got == ':') || (*opt_lookup == '\0')){
        /* oops, opt_got is invalid, either ':' or not found in optlist: */
        return '?';
    }

    /* does option take an argument? */
    if(opt_lookup[1] == ':'){
        if(OPT_NDX != 1){
            /* extract opt_arg from current position in current arg: */
            nopt->opt_arg = &nopt->_argv[ARG_NDX][OPT_NDX];
            /* advance argv/opt cursors: */
            ++ARG_NDX;
            OPT_NDX = 1;
        } else if(ARG_NDX >= nopt->_argc){
            /* oops, no more args available for opt_arg: */
            return ':';
        } else {
            /* OPT_NDX == 1, indicating new arg for opt_arg: */
            nopt->opt_arg = nopt->_argv[ARG_NDX];
            /* advance argv cursor: */
            ++ARG_NDX;
        }
    }

    /* here on valid opt_got: */
    return opt_got;
}


/* eof (nextopt.c) */
