/* cdb_dynget.c
** wcm, 2010.05.29 - 2012.07.25
** ===
*/

/* lasagna: */
#include "dynbuf.h"
#include "uchar.h"

/* libcdb: */
#include "cdb.h"


/* cdb_dynget()
**   assumed to follow successful cdb_find()
**
**   note:
**     operation on dynbuf is "append"
*/
int
cdb_dynget(struct cdb *C, struct dynbuf *B)
{
    return cdb_dynread(C, B, cdb_dlen(C), cdb_dpos(C));
}

/* eof (cdb_dynget.c) */
