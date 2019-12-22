/* sigset.h
** sigset_t operations
** wcm, 2008.01.04 - 2008.02.12
** ===
*/
#ifndef SIGSET_H
#define SIGSET_H 1

#include <signal.h>

/*
** macro definitions for sigset operations
**
** notes:
**   'sigset' argument is of type: sigset_t *
**   'sig'    argument is of type: int
**
*/

#define sigset_empty(sigset)     sigemptyset((sigset))
#define sigset_fill(sigset)      sigfillset((sigset))
#define sigset_add(sigset, sig)  sigaddset((sigset), (sig))
#define sigset_del(sigset, sig)  sigdelset((sigset), (sig))
#define sigset_block(sigset)     sigprocmask(SIG_BLOCK, (sigset), (sigset_t *)0)
#define sigset_unblock(sigset)   sigprocmask(SIG_UNBLOCK, (sigset), (sigset_t *)0)


#endif /* SIGSET_H */
/* EOF (sigset.h) */
