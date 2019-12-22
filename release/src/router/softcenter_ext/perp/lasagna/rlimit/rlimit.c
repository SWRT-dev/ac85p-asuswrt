/* rlimit.c
** wcm, 2009.09.18 - 2009.09.18
** ===
*/
#include <stddef.h>

#include "cstr.h"

#include "rlimit.h"
#include "rlimit_private.h"

#include "rlimit_defs.c.in"


static const size_t rlimit_items = (sizeof rlimit_defs / sizeof (struct rlimit_def));


int
rlimit_lookup(const char *name)
{
   size_t  i;

   /* sequential search: */
   for(i = 0; i < rlimit_items; ++i){
       if(cstr_cmp(rlimit_defs[i].name, name) == 0){
           return rlimit_defs[i].r;
       }
  }

  /* not found: */
  return -1;
}


const
char *
rlimit_mesg(int r)
{
    size_t  i;

   /* sequential search: */
    for(i = 0; i < rlimit_items; ++i){
        if(rlimit_defs[i].r == r) return rlimit_defs[i].mesg;
    }

    /* not found: */
    return NULL;
}

const
char *
rlimit_name(int r)
{
    size_t  i;

   /* sequential search: */
    for(i = 0; i < rlimit_items; ++i){
        if(rlimit_defs[i].r == r) return rlimit_defs[i].name;
    }

    /* not found: */
    return NULL;
}
/* eof: rlimit.c */
