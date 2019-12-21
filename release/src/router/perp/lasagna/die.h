/* die.h
** die: immediate termination
** wcm, 2010.06.08 - 2010.06.08
** ===
*/
#ifndef DIE_H
#define DIE_H 1

#include <unistd.h>

#ifndef DIE_DEFINED
#define DIE_DEFINED  1
/* die()
** immediate termination with exit code status e
** void return
**
** notes:
** die() is used by lasagna apps that do not use/require stdio/streams
** see the _exit(2) manpage for consequences and comparison with exit(3)
*/
#define  die(e)  _exit((int)(e))
#endif

#endif /* DIE_H */
/* eof: die.h */
