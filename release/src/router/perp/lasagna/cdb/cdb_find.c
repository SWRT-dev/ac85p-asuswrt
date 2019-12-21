/* cdb_find.c
** wcm, 2010.05.29 - 2010.12.06
** ===
*/

/* lasagna: */
#include "buf.h"
#include "uchar.h"
#include "upak.h"

/* libcdb: */
#include "cdb.h"

/* subroutines in scope: */
static int cdb_key_match(struct cdb *C, const uchar_t *key, uint32_t klen, uint32_t kpos);
static int cdb_hash_probe(struct cdb *C, uint32_t target);


/* cdb_key_match()
**   compare record key of klen at kpos
**   to search key initialized in cdb_find()
*/   
static
int
cdb_key_match(struct cdb *C, const uchar_t *key, uint32_t klen, uint32_t kpos)
{
  uchar_t   kbuf[80];  /* compare upto 80 bytes of key at a time */
  uint32_t  n;

  while(klen > 0){
      n = sizeof kbuf;
      if(n > klen) n = klen;
      if(cdb_read(C, kbuf, n, kpos) == -1){
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


/* cdb_hash_probe()
**   after cdb_find() initialization,
**   probe for match in hash table beginning at target
**   return:
**      1: key match found, cursors setup accordingly
**      0: key match not found
**     -1: error
*/
static
int
cdb_hash_probe(struct cdb *C, uint32_t target)
{
  /* initialized by cdb_find(): */
  uint32_t   hash = C->h;
  uint32_t   klen = C->klen;
  uint32_t   tbase = C->tbase;
  uint32_t   tslots = C->tslots;
  /* etc: */
  uint32_t   fp;
  uchar_t    nbuf[8];
  uint32_t   rpos;
  uint32_t   slot_hash;
  uint32_t   u;

  /* probe for matching key starting at target: */
  while(target < tslots){
      fp = tbase + (target * 8);
      if(cdb_read(C, nbuf, 8, fp) == -1){
          return -1;
      }
      rpos = upak32_unpack(nbuf + 4);
      if(rpos == 0){
          /* slot empty, key not found: */
          return 0;
      }
      /* else, check hash: */
      slot_hash = upak32_unpack(nbuf);
      if(slot_hash == hash){
          /* hash match; test record key match: */
          if(cdb_read(C, nbuf, 8, rpos) == -1){
              return -1;
          }
          u = upak32_unpack(nbuf);
          if(u == klen){
              switch(cdb_key_match(C, C->key, klen, rpos + 8)){
              case -1:
                  /* io error: */
                  return -1; break;
              case 1:
                  /* match! */
                  C->sN = target;
                  C->rpos = rpos;
                  C->dlen = upak32_unpack(nbuf + 4);
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
      if(target == C->s0){
          /* back at original target; key not found: */
          break;
      }
  }

  /* not found: */
  return 0;
}


int
cdb_find(struct cdb *C, const uchar_t *key, uint32_t klen)
{
  uint32_t  fp;
  uchar_t   nbuf[8];
  uint32_t  hash, tslots, target;
  
  /* search initialization: */
  C->key = (uchar_t *)key;
  C->klen = klen;
  C->h = hash = cdb_hash(key, klen);

  /* lookup subtable for this hash value: */
  fp = cdb_NTAB(hash) * 8;
  if(cdb_read(C, nbuf, 8, fp) == -1){
      return -1;
  }
  /* subtable offet: */
  C->tbase = upak32_unpack(nbuf);
  /* slots in subtable: */
  C->tslots = tslots = upak32_unpack(nbuf + 4);

  if(tslots == 0){
      /* no slots in this table; key not found: */
      return 0;
  }

  /* target slot for this key: */
  target = cdb_SLOT(hash, tslots);
  C->s0 = target;

  /* probe: */
  return cdb_hash_probe(C, target);
}


int
cdb_findnext(struct cdb *C)
{
    uint32_t  target = C->sN;

    /* advance target from last succesful find: */
    ++target; 
    if(target == C->tslots) target = 0;
    if(target == C->s0){
        /* back at original target; key not found: */
        return 0;
    }

    /* probe: */
    return cdb_hash_probe(C, target);
}


/* eof (cdb_find.c) */
