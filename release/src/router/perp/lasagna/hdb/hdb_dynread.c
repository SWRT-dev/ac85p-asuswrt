/* hdb_dynread.c
** wcm, 2010.05.29 - 2012.07.25
** ===
*/

/* libc: */
#include <stddef.h>
#include <stdint.h>

/* lasagna: */
#include "dynbuf.h"
#include "uchar.h"

/* libhdb: */
#include "hdb.h"


/* XXX, hacks dynbuf interface */

int
hdb_dynread(struct hdb *H, struct dynbuf *B, size_t len, uint32_t offset)
{
  if(dynbuf_grow(B, len) == -1)
      return -1;

  if(hdb_read(H, dynbuf_BUF(B) + dynbuf_LEN(B), len, offset) == -1)
      return -1;

  dynbuf_LEN(B) += len;

  return 0;
}

/* eof (hdb_dynread.c) */
