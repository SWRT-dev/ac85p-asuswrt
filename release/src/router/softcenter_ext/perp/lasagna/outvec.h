/* outvec.h
** output using writev()
** wcm, 2008.01.04 - 2010.12.01
** ===
*/
#ifndef OUTVEC_H
#define OUTVEC_H 1

#include <stddef.h>
#include <sys/uio.h>

/*
** outvec object:
*/
struct outvec {
  /* file descriptor open for write() operations: */
  int            fd;
  /* array of struct iovec elements for writev(): */
  struct iovec  *vec;
  /* internal index cursor for next position in vec[]: */
  size_t         n;
  /* number of struct iovec elements in vec[]: */
  size_t         max;
  /* boolean flag for autoflush on outvec_vput(): */
  int            flushme;
};
typedef struct outvec outvec_t;

/* comment:
**
**   The outvec module may be used for lightweight output requirements,
**   eg: writing brief error messages to stderr.
**
**   In the functions described below, just the string pointer arguments
**   are entered into the internal iovec array of the outvec object;
**   the contents of the string buffers referenced by their pointers
**   are *not* copied.
**
**   This makes outvec small and fast and convenient, but the caller does
**   need to refrain from modifying the contents of any string buffers
**   entered into an outvec until the outvec has been flushed.
*/

/*
** outvec methods:
*/

/* outvec_INIT()
**   djb idiom for initialization of static outvec_t object
**   example:
**
**   struct iovec  myvec[10];
**   outvec_t      V = outvec_INIT(fd, myvec, (sizeof myvec / sizeof(struct iovec)), 1);
**
*/
#define outvec_INIT(fd, vec, max, flushme)  {(fd), (vec), 0, (max), (flushme)}

/* outvec_put()
**   enter pointer to buffer buf into outvec object
**   if vec->n iovec buffers have reached vec->max, first flush to vec->fd
**   (otherwise, this function does not autoflush)
**
**   returns:
**     0 on success
**    -1 on writev() failure (errno set)
*/
extern int outvec_put(struct outvec *vec, const void *buf, size_t len);


/* outvec_puts()
**   enter pointer to string s into outvec object
**   if vec->n iovec buffers have reached vec->max, first flush to vec->fd
**   (otherwise, this function does not autoflush)
**
**   returns:
**     0 on success
**    -1 on writev() failure (errno set)
*/
extern int outvec_puts(struct outvec *vec, const char *s);


/* outvec_vputs_()
**   enter variable number of string pointer args into outvec object
**
**   as vec->n iovec buffers reach vec->max, flush to vec->fd
**   if vec->flushme != 0, flush to vec->fd
**   the last item in the variable argument list must be NULL!
**
**   returns:
**     0 on success
**    -1 on writev() failure (errno set)
**
**   note:
**     recommended usage of this function is within a macro definition
**     eg: outvec_vput(), to apply the final NULL argument
*/
extern int outvec_vputs_(struct outvec *vec, const char *s, ...);


/* outvec_vputs()
**   convenience macro for outvec_vput_()
**   appends final NULL argument
*/
#define outvec_vputs(v, ...) \
    outvec_vputs_((v), __VA_ARGS__, NULL)


/* example usage with macro definition:
**
#define eputs(...) \
    outvec_vputs(&OUTVEC_STDERR, progname, ": ", __VA_ARGS__, "\n")
*/


/*
** outvec_flush():  flush vec to vec->fd
**
**    flush existing vec->n iovec buffers to vec->fd
**
**    returns:
**         0 on success
**        -1 on writev() failure (errno set)
**
**    note: ensures complete writes of all iovec buffers
**    (supports use with non-blocking i/o)
*/
extern int outvec_flush(struct outvec *vec);


/*
** predefined standard outvec objects provided by this module
** (with static internal iovec[] buffers):
*/

/* stdout ("slightly buffered", flushme = 0): */
extern struct outvec OUTVEC_STDOUT;

/* stderr (unbuffered, flushme = 1): */
extern struct outvec OUTVEC_STDERR;


#endif /* OUTVEC_H */
/* eof: outvec.h */
