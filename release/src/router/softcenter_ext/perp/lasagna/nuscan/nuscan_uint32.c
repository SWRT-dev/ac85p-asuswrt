/* nuscan_uint32.c
** scan numeric string into numeric value
** wcm, 2008.10.09 - 2010.02.17
** ===
*/
#include <stddef.h>
#include <inttypes.h>

#include "nuscan.h"


const char *
nuscan_uint32(uint32_t *result, const char *s)
{
   uint32_t  n = 0;
   uint32_t  r = 0;

   while((n = *s - '0') < 10){
       n = r * 10 + n;
       /* test overflow: */
       if(n < r) break;
       /* else: */
       r = n;
       ++s;
   }

   *result = r;
   return s;
} 

/* EOF (nuscan_uint32.c) */
