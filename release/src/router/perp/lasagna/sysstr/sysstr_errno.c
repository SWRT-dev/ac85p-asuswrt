/* sysstr_errno.c
** correlate errno with stringified errno
** wcm, 2008.01.21 - 2009.09.18
** ===
*/

#include <stdlib.h>

#include "sysstr.h"
#include "sysstr_private.h"

/* sorted errno table produced by mksysstr: */
#include "sysstr_errno.c.in"

/* sysno_lookup() definition: */
#include "sysno_lookup.c.in"


/* variables in local scope: */
static const int errno_items = (sizeof errno_defs / sizeof(struct sysno_def));


const char *
sysstr_errno(int errno_num)
{
  const struct sysno_def  *errno_def;

  errno_def = sysno_lookup(errno_defs, errno_items, errno_num);
  if((errno_def != NULL) && (errno_def->name != NULL))
      return errno_def->name;

  return NULL;
}


const char *
sysstr_errno_mesg(int errno_num)
{
  const struct sysno_def  *errno_def;

  errno_def = sysno_lookup(errno_defs, errno_items, errno_num);
  if((errno_def != NULL) && (errno_def->mesg != NULL))
      return errno_def->mesg;

  return NULL;
}


/* eof (sysstr_errno.c) */
