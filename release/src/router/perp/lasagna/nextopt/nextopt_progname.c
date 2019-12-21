/* nextopt_progname.c
** command-line option parser
** get progname from initialized nextopt_t object
** wcm, 2009.11.11 - 2009.11.12
** ===
*/
#include <stddef.h>

#include "cstr.h"

#include "nextopt.h"

#if 0
struct nextopt {
  char          opt_got;  /* actual option letter found */
  char         *opt_arg;  /* option argument found */
  const char   *opt_list; /* valid option specification */
  size_t        arg_ndx;  /* current argv index */
  /*
  ** private!
  */
  int           _argc;    /* arg count */
  char * const *_argv;    /* argument list */
  size_t        _ondx;    /* option cursor position */
}
typedef struct nextopt  nextopt_t;
#endif


const char *
nextopt_progname(struct nextopt *nopt)
{
  char *progname = nopt->_argv[0];
  char *s;

  for(s = progname; *s != '\0'; ++s){
      if(*s == '/') progname = s + 1;
  }

  return (const char *)progname;
}


/* eof (nextopt_progname.c) */
