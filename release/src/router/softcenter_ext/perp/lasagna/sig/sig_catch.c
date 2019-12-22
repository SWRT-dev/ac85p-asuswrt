/* sig_catch.c
** signal operations
** wcm, 2008.01.04 - 2009.10.06
** ===
*/

#include <stdlib.h>
#include <signal.h>

#include "sig.h"


/*
** APUE/2e, fig 10.19, p. 329
**
** this is the "default" signal handler utility in libasagna
** it does NOT set the SA_RESTART sigaction flag:
** blocking calls such as read() ARE interrupted by the signal,
** rather than quietly restarted
**
** note: compatible with "djb" sig_catch()
**
** see also sig_catchr() for the SA_RESTART version
*/

sig_handler_t sig_catch(int sig, sig_handler_t handler)
{
  struct sigaction new_sa, old_sa;

  new_sa.sa_handler = handler;
  sigemptyset(&new_sa.sa_mask);
  new_sa.sa_flags = 0;
#ifdef SA_INTERRUPT
      new_sa.sa_flags |= SA_INTERRUPT;
#endif
  /* suppress SIGCHLD on child STOPs (job control): */
  new_sa.sa_flags |= SA_NOCLDSTOP;
  if(sigaction(sig, &new_sa, &old_sa) != 0){
      /* sigaction() failure: */
      return SIG_ERR;
  }

  /* success: */
  return old_sa.sa_handler;
}


/* eof: sig_catch.c */
