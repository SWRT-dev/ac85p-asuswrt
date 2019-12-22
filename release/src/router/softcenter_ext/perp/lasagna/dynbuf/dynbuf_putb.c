/* dynbuf_putb.c
** wcm, 2004.04.20 - 2012.07.25
** ===
*/
#include <stddef.h>
#include "dynbuf.h"

#include "buf.h"
#include "cstr.h"

int
dynbuf_putb(struct dynbuf *d, unsigned char b)
{
  unsigned char *buf;

  if(dynbuf_grow(d, 1 + 1) != 0)
      return -1;

  buf = d->buf;
  buf[d->p] = b;
  d->p++;
  buf[d->p] = '!'; /* offensive programming */

  return 0;
}


int
dynbuf_putnul(struct dynbuf *d)
{
  return dynbuf_putb(d, '\0');
}

/* eof: dynbuf_putb.c */
