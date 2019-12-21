/* cdb_cc.c
** wcm, 2010.11.04 - 2010.11.04
** ===
*/

/* lasagna: */
#include "buf.h"

/* libcdb: */
#include "cdb.h"


struct cdb *
cdb_cc(struct cdb *dest, const struct cdb *src)
{
    return (struct cdb *)buf_copy(dest, src, sizeof (struct cdb));
}

/* eof (cdb_cc.c) */
