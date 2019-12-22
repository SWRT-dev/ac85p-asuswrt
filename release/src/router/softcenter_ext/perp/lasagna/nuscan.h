/* nuscan.h
** scan numeric string into numeric value
** wcm, 2010.02.04 - 2010.02.17
** ===
*/
#ifndef NUSCAN_H
#define NUSCAN_H 1

#include <stddef.h>    /* sizet_t */
#include <stdint.h>

/* nuscan_uint32()
**   scan nul-terminated numeric decimal string s into destination result
**   return pointer in s where scan stopped:
**     '\0' :       success, scan reached end of s
**     digit 0-9 :  overflow, s too large for result
**     other:       illegal (non-numeric) value in s
**
**   handles all non-empty s representing values [0 .. (2^32 - 1)]
**   CAVEAT: input string s must be non-empty
**
**   snippet:
**
**   const char *z;
**   uint32_t    u = 0;
**
**   z = nuscan_uint32(&u, input);
**   if(*z != '\0'){
**       if(isdigit(*z))
**           barf("overflow on input!");
**       else
**           barf("bad input: ", input);
**   }
*/
extern const char *nuscan_uint32(uint32_t *result, const char *s);

/* nuscan_uint32o()
**   scan nul-terminated numeric octal string s into destination result
**   return pointer in s where scan stopped:
**     '\0':       success, scan reached end of s
**     digit 0-7:  overflow, s too large for result
**     other:      illegal (non-octal) value in s
**
**   handles all non-empty s representing values [0 .. (2^32 - 1)]
**   CAVEAT: input string s must be non-empty
*/
extern const char *nuscan_uint32o(uint32_t *result, const char *s);


#endif /* NUSCAN_H */
/* EOF (nuscan.h) */
