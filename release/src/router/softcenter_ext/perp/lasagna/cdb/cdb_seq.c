/* cdb_seq.c
** wcm, 2010.05.29 - 2010.10.26
** ===
*/

/* lasagna: */
#include "uchar.h"
#include "upak.h"

/* libcdb: */
#include "cdb.h"

/* subroutine in local scope: */
static int cdb_seq_(struct cdb *C, uint32_t rpos);

static
int
cdb_seq_(struct cdb *C, uint32_t rpos)
{
    uint32_t  eod = C->hbase;
    uchar_t   nbuf[8];

    if(! (rpos < eod))
        return 0;

    if(cdb_read(C, nbuf, 8, rpos) == -1)
        return -1;

    C->rpos = rpos;
    C->klen = upak32_unpack(nbuf);
    C->dlen = upak32_unpack(nbuf + 4);

    return 1;
}


int
cdb_seqinit(struct cdb *C)
{
    uint32_t  rpos = (256 * 8);

    /* start from first record offset: */
    return cdb_seq_(C, rpos);
}

int
cdb_seqnext(struct cdb *C)
{
    uint32_t  rpos = C->rpos;

    /* increment from last record offset: */
    rpos += C->klen + C->dlen + 8;
    return cdb_seq_(C, rpos);
}

/* eof (cdb_seq.c) */
