/* nextopt.h
** command-line option parser
** wcm, 2009.11.11 - 2010.12.01
** ===
*/
#ifndef NEXTOPT_H
#define NEXTOPT_H 1

#include <stddef.h>    /* size_t */

/* nextopt summary: 
**   not quite getopt()
**   * no error reporting (that is, no stdio/printf() inside!)
**   * does not export any global variables
**   * reentrant
**   * always implies ':' beginning optlist
**   * intended to support all conventions of common syntax for
**     simple single-letter command-line options
**
**   * eg:
**     options w/o arguments may be concatenated after single '-'
**     option arguments may be concatenated to the option letter or separate
**     options and option arguments must preceed other command-line arguments
**     the argument '--' may be used to flag end of option processing
*/

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
};
typedef struct nextopt  nextopt_t;


/* nextopt_start()
**   initialize nextopt parser
**   caution: see nextopt_init() for the conventional interface
**
**   this function provides for explicit initialization of the start index
**   into the argv argument vector
**
**   in conventional nextopt() usage this will be set to 1,
**   as the argv[0] index is normally the name of the program
*/
extern
void
nextopt_start(
  struct nextopt *nopt,
  int             argc,
  char * const    argv[],
  const char     *optlist,
  size_t          start
);


/* nextopt_init()
**   initialize nextopt parser:
**   nopt : pointer to nextopt_t object
**   argc : number of arguments in argv
**   argv : NULL-terminated argument vector
**   optlist:
**     string containing set of valid option letters
**     option letters followed by ':' flag option requires argument
**
**   notes:
**     assumes argv[0] is program name, parsing begins with argv[1]
**     avoid using ':' and '-' as option characters
**     unlike getopt(3), nextopt ignores ':' at beginning of optlist
**     (actually, nextopt assumes it)
*/
#define nextopt_init(nopt, argc, argv, optlist) \
    nextopt_start((nopt), (argc), (argv), (optlist), 1)

/* nextopt_INIT()
**   static nextopt_t object initializer macro
**   usage:
**
**   struct nextopt  nopt = nextopt_INIT(argc, argv, optlist);
**
**   note: C99 initialization would be preferable here:
**   { \
**     .opt_got = '\0', .opt_arg = NULL, .opt_list = (optlist), .arg_ndx = 1, \
**     ._argc = (argc), ._argv = (argv), ._ondx = 1 \
**   }
**
*/
#define nextopt_INIT(argc, argv, optlist)  {'\0', NULL, (optlist), 1, (size_t)(argc), (argv), 1}

/* nextopt()
**   get next valid option
**   return:
**     '\0' : end of option arguments (additional arguments may follow)
**     '?'  : option character found is invalid (not defined in optlist)
**     ':'  : option requires argument but argument is missing
**     otherwise, the valid option character is returned
**
**   the following items may be read from the nextopt_t object on return:
**
**   nopt->opt_got: the actual option character found
**       for usage when nextopt() itself returns '?' or ':'
**
**   nopt->opt_arg: pointer to option argument, if option requires argument 
**       set to NULL if option does not require argument
**
**   nopt->arg_ndx: current argv index
**       for usage to reindex argc/argv after option parsing complete
**
**   notes:
**     nextopt() does not perform any error reporting output
**     nextopt() returns char, not int
**     nextopt() terminal is '\0', not -1
**     results of calls to nextopt() after returning '\0', '?', ':' are undefined
*/
extern char nextopt(struct nextopt *nopt);

/* nextopt_progname()
**   return pointer to progname from argv[0] of initialized nextopt_t object
**   progname is equivalent to basename of argv[0]
**     ie: stripped of any leading directory names
**
**   return value is a pointer into original argv[0] of initializing argv
**     ie: not a strdup, not a static buffer
*/
extern const char *nextopt_progname(struct nextopt *nopt);

#if 0
/*
** example usage:
*/
#include <stdio.h>
#include "nextopt.h"

int
main(int argc, char *argv[])
{
    char       opt;
    nextopt_t  nopt;

    nextopt_init(&nopt, argc, argv, "b:vf:");
    while((opt = nextopt(&nopt)) != '\0'){
        switch(opt){
        case 'b':
            printf("option %c found with arg [%s]\n", opt, nopt.opt_arg);
            break;
        case 'v':
            printf("setting verbosity on\n");
            break;
        case 'f':
            printf("got option 'f' with arg [%s]\n", nopt.opt_arg);
            break;
        case '?':
            printf("oops, unknown option -%c\n", nopt.opt_got);
            return -1;
            break;
        case ':':
            printf("oops, missing argument for option -%c\n", nopt.opt_got);
            return -1;
            break;
        default:
            printf("oops, shouldn't be in default case???\n");
            return -1;
            break;
        }
    }

    printf("done with option processing...\n");

    argc -= nopt.arg_ndx;
    argv += nopt.arg_ndx;

    printf("remaining argument(s):\n");
    while(*argv != NULL){
        printf("-->%s\n", *argv);
        ++argv;
    }

    printf("all done!\n");
    return 0;    
}
#endif /* example */


#endif /* NEXTOPT_H */
/* eof (nextopt.h) */
