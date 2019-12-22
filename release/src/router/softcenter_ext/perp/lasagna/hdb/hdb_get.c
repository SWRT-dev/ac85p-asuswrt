/* hdb_get.c
** wcm, 2010.05.29 - 2010.12.14
** ===
*/

/* libc: */
#include <stddef.h>  /* size_t */
#include <stdint.h>  /* uint32_t */

/* lasagna: */
#include "uchar.h"

/* libhdb: */
#include "hdb.h"


/* hdb_get()
**   copy len bytes of record data at hdb_dpos(H) in buf
**   assumed to follow successful hdb_find()
**   ie, cursor positioned at desired record
*/
int
hdb_get(struct hdb *H, uchar_t *buf, size_t len)
{
    /* truncate len to size of data: */
    if(len > H->dlen)
        len = H->dlen;

    return (hdb_read(H, buf, len, hdb_dpos(H))); 
}

/* eof (hdb_get.c) */
