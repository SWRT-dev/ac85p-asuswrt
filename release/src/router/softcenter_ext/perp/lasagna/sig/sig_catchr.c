/* sig_catchr.c
** signal operations
** wcm, 2008.01.04 - 2009.01.06
** ===
*/

#include <stdlib.h>
#include <signal.h>

#include "sig.h"


/*
** APUE/2e, fig 10.18, p. 328
** plus:
**   SA_NOCLDSTOP
*/

sig_handler_t sig_catchr(int sig, sig_handler_t handler)
{
  struct sigaction new_sa, old_sa;

  new_sa.sa_handler = handler;
  sigemptyset(&new_sa.sa_mask);
  new_sa.sa_flags = 0;
  if(sig == SIGALRM){
#ifdef SA_INTERRUPT
      new_sa.sa_flags |= SA_INTERRUPT;
#endif
  } else {
#ifdef SA_RESTART
      new_sa.sa_flags |= SA_RESTART;
#endif
  }
  /* suppress SIGCHLD on child STOPs (job control): */
  new_sa.sa_flags |= SA_NOCLDSTOP;
  if(sigaction(sig, &new_sa, &old_sa) != 0){
      /* sigaction() failure: */
      return SIG_ERR;
  }

  /* success: */
  return old_sa.sa_handler;
}


/* eof: sig_catchr.c */
