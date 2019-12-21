/* cdb_dynread.c
** wcm, 2010.05.29 - 2012.07.25
** ===
*/

/* libc: */
#include <stddef.h>
#include <stdint.h>

/* lasagna: */
#include "dynbuf.h"
#include "uchar.h"

/* libcdb: */
#include "cdb.h"


/* XXX, hacks dynbuf interface */

int
cdb_dynread(struct cdb *C, struct dynbuf *B, size_t len, uint32_t offset)
{
  if(dynbuf_grow(B, len) == -1)
      return -1;

  if(cdb_read(C, dynbuf_BUF(B) + dynbuf_LEN(B), len, offset) == -1)
      return -1;

  dynbuf_LEN(B) += len;

  return 0;
}

/* eof (cdb_dynread.c) */
