/* dynstr.h
** dynamic (growable) character strings (nul-terminated)
** wcm, 2004.04.21 - 2010.06.15
** ===
*/
#ifndef DYNSTR_H
#define DYNSTR_H

#include <stddef.h>  /* size_t */

#define DYNSTR_INITSIZE  1

struct dynstr {
  char    *s;
  size_t   n;   /* current position (aka "length") */
  size_t   k;   /* current size allocated for s  */
}; 
typedef struct dynstr  dynstr_t;

/* dynstr_INIT()
** dynstr static initialization
** usage:
**   dynstr S = dynstr_INIT();
**
** should be preferred to:  dynstr S = {0,0,0};
*/
#define dynstr_INIT() {NULL, 0, 0}

/* dynstr_set()
**   initialize a dynstr with an existing cstr
*/
extern void dynstr_set(dynstr_t *S, char *s);

/* constructor for malloc'ed dynstr: */
extern dynstr_t * dynstr_new(void); 

/* destructor for malloc'ed dynstr: */
extern void dynstr_free(dynstr_t *);

/* release internal string only: */
extern void dynstr_freestr(dynstr_t *);

/* no memory release, just set S->n = 0 */
extern void dynstr_clear(dynstr_t *S);
#define dynstr_CLEAR(S) {(S)->n = 0; if((S)->k) (S)->s[0] = '\0';}

/* accessors: */
extern char * dynstr_str(dynstr_t *S);
extern size_t dynstr_len(dynstr_t *S);
/* accessor macros: */
#define dynstr_STR(S) ((S)->s)
#define dynstr_LEN(S) ((S)->n)

/* dynstr_need()
**   resize internal string buffer as necessary to accomodate need
**   need may be considered as for strlen, not inclusive of terminating-nul
**   (resize will ensure space for terminating-nul in addition to need)
**   (need is for total need, rather than additional need)
**
**   return
**     0 : no error
**    -1 : allocation failure
*/
extern int dynstr_need(dynstr_t *, size_t need);

/* dynstr_grow()
**   resize internal string buffer as necessary to accomodate additional grow
**   grow may be considered as for strlen, not inclusive of terminating-nul
**   (grow is for additional need, as for appending)
**
**   return
**     0 : no error
**    -1 : allocation failure
*/
extern int dynstr_grow(dynstr_t *, size_t grow);

/* dynstr "put" operations:
**   "put" means _append_ to dynstr
**
**   return -1 on error, 0 no error
*/
extern int dynstr_put(dynstr_t *to, dynstr_t *from);
extern int dynstr_puts(dynstr_t *S, const char *s);

/* dynstr_putn()
**   append n characters from s to dynstr
**   (appends lesser of n or strlen(s) characters from s)
*/
extern int dynstr_putn(dynstr_t *S, const char *s, size_t n);

/* dynstr_putc()
** append single char
*/
extern int dynstr_putc(dynstr_t *S, char c);

/* dynstr_vputs_()
** append one or more argument strings into a dynstr
**
** notes:
**   caller must supply NULL as final argument
**   use macro version instead
*/
extern int dynstr_vputs_(dynstr_t *, const char *s, ...);
#define dynstr_vputs(S,...) dynstr_vputs_((S), __VA_ARGS__, NULL)

/* dynstr "copy" operations (overwrite previous contents): */
extern int dynstr_copy(dynstr_t *to, dynstr_t *from);
extern int dynstr_copys(dynstr_t *S, const char *s);


/*
** string manipulation
*/

/* dynstr_chop()
**   remove trailing '\n' if present from end of dynstr
*/
extern void dynstr_chop(dynstr_t *S);


#endif /* DYNSTR_H */
/* eof: dynstr.h */
