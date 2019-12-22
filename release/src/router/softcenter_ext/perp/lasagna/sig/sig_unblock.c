/* sig_unblock.c
** signal operations
** wcm, 2008.01.04 - 2008.01.28
** ===
*/

#include <stdlib.h>
#include <signal.h>

#include "sig.h"


void sig_unblock(int sig)
{
  sigset_t ss;

  sigemptyset(&ss);
  sigaddset(&ss, sig);
  sigprocmask(SIG_UNBLOCK, &ss, (sigset_t *) 0);

  return;
}


/* EOF (sig_unblock.c) */
