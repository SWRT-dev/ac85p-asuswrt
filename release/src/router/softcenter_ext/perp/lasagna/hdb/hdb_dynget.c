/* hdb_dynget.c
** wcm, 2010.05.29 - 2012.07.25
** ===
*/

/* lasagna: */
#include "dynbuf.h"
#include "uchar.h"

/* libhdb: */
#include "hdb.h"


/* hdb_dynget()
**   assumed to follow successful hdb_find()
**
**   note:
**     operation on dynbuf is "append"
*/
int
hdb_dynget(struct hdb *H, struct dynbuf *B)
{
    return hdb_dynread(H, B, hdb_dlen(H), hdb_dpos(H));
}

/* eof (hdb_dynget.c) */
