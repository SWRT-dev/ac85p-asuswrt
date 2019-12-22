/* cdb_get.c
** wcm, 2010.05.29 - 2010.11.24
** ===
*/

/* libc: */
#include <stddef.h>  /* size_t */
#include <stdint.h>  /* uint32_t */

/* lasagna: */
#include "uchar.h"

/* libcdb: */
#include "cdb.h"


/* cdb_get()
**   copy len bytes of record data at cdb_dpos(C) in buf
**   assumed to follow successful cdb_find()
**   ie, cursor positioned at desired record
*/
int
cdb_get(struct cdb *C, uchar_t *buf, size_t len)
{
    /* truncate len to size of data: */
    if(len > C->dlen)
        len = C->dlen;

    return (cdb_read(C, buf, len, cdb_dpos(C))); 
}

/* eof (cdb_get.c) */
