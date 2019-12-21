/* cstr_lcpy.c
** wcm, 2004.04.19 - 2009.08.03
** ===
*/
#include "cstr.h"

#if 0
  copy src to dest
  src must be NUL-terminated
  buffer dest is of length dest_len, includes terminating NUL
  max of (dest_len - 1) copied from src, to leave room for NUL

  returns length of src:

    if (cstr_lcpy(dest, src, dest_len) >=  dest_len){
        // copy of src was truncated!
    }
#endif

size_t
cstr_lcpy(char *dest, const char *src, size_t dest_len)
{
  char        *to = dest;
  const char  *from = src;
  size_t       n = dest_len;

  if((n) && (--n)){
      for(;;) {
          if (!(*to++ = *from++)) break; if(!(--n)) break;
          if (!(*to++ = *from++)) break; if(!(--n)) break;
          if (!(*to++ = *from++)) break; if(!(--n)) break;
          if (!(*to++ = *from++)) break; if(!(--n)) break;
      }
  }

  /* dest full; append NUL and find strlen(src): */
  if(n == 0) {
      if(dest_len > 0)  *to = '\0';
      while(*from++)
          ;
  }

  /* strlen(src): */
  return (from - src - 1);
}

/* eof: cstr_lcpy.c */
