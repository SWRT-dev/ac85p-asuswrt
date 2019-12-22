/* sysstr_signal.c
** correlate signal number with stringified signal number
** wcm, 2008.01.21 - 2009.09.18
** ===
*/

#include <stdlib.h>

#include "sysstr.h"
#include "sysstr_private.h"

/* sorted signo table produced by mksysstr: */
#include "sysstr_signo.c.in"

/* sysno_lookup() definition: */
#include "sysno_lookup.c.in"


/* variables in local scope: */
static const int signo_items = (sizeof signo_defs / sizeof(struct sysno_def));


const char *
sysstr_signal(int signo_num)
{
  const struct sysno_def  *signo_def;

  signo_def = sysno_lookup(signo_defs, signo_items, signo_num);
  if((signo_def != NULL) && (signo_def->name != NULL))
      return signo_def->name;

  return NULL;
}


const char *
sysstr_signal_mesg(int signo_num)
{
  const struct sysno_def  *signo_def;

  signo_def = sysno_lookup(signo_defs, signo_items, signo_num);
  if((signo_def != NULL) && (signo_def->mesg != NULL))
      return signo_def->mesg;

  return NULL;
}


/* eof (sysstr_signal.c) */
