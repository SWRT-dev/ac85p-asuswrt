/* hdb_seq.c
** wcm, 2010.05.29 - 2010.12.14
** ===
*/

/* lasagna: */
#include "uchar.h"
#include "upak.h"

/* libhdb: */
#include "hdb.h"

/* subroutine in local scope: */
static int hdb_seq_(struct hdb *H, uint32_t rpos);


static
int
hdb_seq_(struct hdb *H, uint32_t rpos)
{
  uint32_t  eod = H->hbase;
  uchar_t   nbuf[6];

  if(! (rpos < eod)){
      return 0;
  }

  if(hdb_read(H, nbuf, sizeof nbuf, rpos) == -1){
      return -1;
  }

  H->rpos = rpos;
  H->klen = upak24_UNPACK(nbuf);
  H->dlen = upak24_UNPACK(nbuf + 3);

  return 1;
}


int
hdb_seqinit(struct hdb *H)
{
  /* initialize sequential access from first record offset: */
  return hdb_seq_(H, H->rbase);
}


int
hdb_seqnext(struct hdb *H)
{
  uint32_t  rpos = H->rpos;

  /* increment from last record offset: */
  rpos += H->klen + H->dlen + 6;
  return hdb_seq_(H, rpos);
}

/* eof (hdb_seq.c) */
