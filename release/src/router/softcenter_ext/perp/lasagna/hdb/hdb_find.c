/* hdb_find.c
** wcm, 2010.05.29 - 2010.12.14
** ===
*/

#include <errno.h>

/* lasagna: */
#include "buf.h"
#include "uchar.h"
#include "upak.h"

/* libhdb: */
#include "hdb.h"

/* subroutines in scope: */
static int hdb_key_match(struct hdb *H, const uchar_t *key, uint32_t klen, uint32_t kpos);
static int hdb_hash_probe(struct hdb *H, uint32_t target);


/* hdb_key_match()
**   compare record key of klen at kpos
**   to search key initialized in hdb_find()
*/   
static
int
hdb_key_match(struct hdb *H, const uchar_t *key, uint32_t klen, uint32_t kpos)
{
  uchar_t   kbuf[80];  /* compare upto 80 bytes of key at a time */
  uint32_t  n;

  while(klen > 0){
      n = sizeof kbuf;
      if(n > klen) n = klen;
      if(hdb_read(H, kbuf, n, kpos) == -1){
          /* io error: */
          return -1;
      }
      if(buf_cmp(key, kbuf, n) != 0){
          /* no match: */
          return 0;
      }
      kpos += n;
      klen -= n;
  }

  /* match! */
  return 1;
}


/* hdb_hash_probe()
**   after hdb_find() initialization,
**   probe for match in hash table beginning at target
**   return:
**      1: key match found, cursors setup accordingly
**      0: key match not found
**     -1: error
*/
static
int
hdb_hash_probe(struct hdb *H, uint32_t target)
{
  /* initialized by hdb_find(): */
  uint32_t   hash = H->h;
  uint32_t   klen = H->klen;
  uint32_t   tbase = H->tbase;
  uint32_t   tslots = H->tslots;
  /* etc: */
  uint32_t   fp;
  uchar_t    nbuf[8];
  uint32_t   rpos;
  uint32_t   slot_hash;
  uint32_t   u;

  /* probe for matching key starting at target: */
  while(target < tslots){
      fp = tbase + (target * 8);
      if(hdb_read(H, nbuf, sizeof nbuf, fp) == -1){
          return -1;
      }
      rpos = upak32_UNPACK(nbuf + 4);
      if(rpos == 0){
          /* slot empty, key not found: */
          return 0;
      }
      /* else, check hash: */
      slot_hash = upak32_UNPACK(nbuf);
      if(slot_hash == hash){
          /* hash match; test record key match: */
          if(hdb_read(H, nbuf, 6, rpos) == -1){
              return -1;
          }
          u = upak24_UNPACK(nbuf);
          if(u == klen){
              switch(hdb_key_match(H, H->key, klen, rpos + 6)){
              case -1:
                  /* io error: */
                  return -1; break;
              case 1:
                  /* match! */
                  H->sN = target;
                  H->rpos = rpos;
                  H->dlen = upak24_UNPACK(nbuf + 3);
                  return 1;
                  break;
              default:
                  /* no match; continue probe... */
                  break;
              }
          }
      }

      /* advance target: */
      ++target;
      if(target == tslots) target = 0;
      if(target == H->s0){
          /* back at original target; key not found: */
          break;
      }
  }

  /* not found: */
  return 0;
}


int
hdb_find(struct hdb *H, const uchar_t *key, uint32_t klen)
{
  uint32_t  hash, ntab, tslots, target;
 
  /* constrain klen to 24-bit length: */
  if(klen > HDB_U24MAX){
      errno = ERANGE;
      return -1;
  }
 
  /* search initialization: */
  H->key = (uchar_t *)key;
  H->klen = klen;
  H->h = hash = hdb_hash(key, klen);

  /* subtable: */
  ntab = hdb_NTAB(hash);

  /* subtable offet: */
  H->tbase = H->tndx[ntab].offset;
  /* slots in subtable: */
  H->tslots = tslots = H->tndx[ntab].value;

  if(tslots == 0){
      /* no slots in this table; key not found: */
      return 0;
  }

  /* target slot for this key: */
  H->s0 = target = hdb_SLOT(hash, tslots);

  /* probe: */
  return hdb_hash_probe(H, target);
}


int
hdb_findnext(struct hdb *H)
{
    uint32_t  target = H->sN;

    /* advance target from last succesful find: */
    ++target; 
    if(target == H->tslots) target = 0;
    if(target == H->s0){
        /* back at original target; key not found: */
        return 0;
    }

    /* probe: */
    return hdb_hash_probe(H, target);
}


/* eof (hdb_find.c) */
