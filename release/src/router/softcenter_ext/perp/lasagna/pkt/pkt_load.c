/* pkt_load.c
** pkt: tiny packet protocol
** wcm, 2009.07.29 - 2011.01.21
** ===
*/

#include <stddef.h>
#include <stdint.h>
#include <errno.h>

#include "uchar.h"
#include "buf.h"
#include "pkt.h"


int
pkt_load(pkt_t K, uchar_t P, uchar_t T, uchar_t *buf, size_t N)
{
  /* fail if N too large for pkt_t: */
  if(N > (sizeof(pkt_t) - 3)){
      errno = ERANGE;
      return -1;
  }

  K[0] = P;
  K[1] = T;
  K[2] = (uint8_t)N;
  buf_copy(&K[3], buf, N);

  return 0;
}


/* eof: pkt_load.c */
