/* hdb_cc.c
** wcm, 2010.11.04 - 2010.12.14
** ===
*/

/* lasagna: */
#include "buf.h"

/* libhdb: */
#include "hdb.h"


struct hdb *
hdb_cc(struct hdb *dest, const struct hdb *src)
{
    return (struct hdb *)buf_copy(dest, src, sizeof (struct hdb));
}

/* eof (hdb_cc.c) */
