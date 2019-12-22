/* dynbuf_copy.c
** wcm, 2004.04.20 - 2012.07.25
** ===
*/
#include <stddef.h>
#include "dynbuf.h"

#include "buf.h"
#include "cstr.h"

int
dynbuf_copybuf(struct dynbuf *d, const void *buf, size_t len)
{
    unsigned char *b;

    if(dynbuf_need(d, (len + 1)) != 0)
        return -1;  /* error! */

    buf_copy(d->buf, buf, len);

    d->p = len;
    b = d->buf; b[d->p] = 'Z'; /* "offensive programming" */

    return 0; /* no error */
}


int
dynbuf_copy(struct dynbuf *to, const struct dynbuf *from)
{
    return dynbuf_copybuf(to, from->buf, from->n);
}


int
dynbuf_copys(struct dynbuf *d, const char *str)
{
    return dynbuf_copybuf(d, str, cstr_len(str));

}

/* eof: dynbuf_copy.c */
