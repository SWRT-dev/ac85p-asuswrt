/* hdbmk_finish.c
** hdb file writer/generator
** wcm, 2010.05.27 - 2012.07.25
** ===
*/

/* libc: */
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

/* unix: */
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

/* libaxative: */
#include "buf.h"
#include "ioq.h"
#include "ufunc.h"
#include "upak.h"

/* libhdb: */
#include "hdb.h"
#include "hdbmk.h"


/* subroutines in local scope: */
static void list_reverse(struct hdbmk_block **list);
static int tbuf_load(uchar_t *tbuf, uint32_t tslots, uint32_t target, uint32_t hash, uint32_t rpos);


/* list_reverse()
**   reverse an hdbmk_block list
**   (restores original entry order for stable hash key insertion)
*/
static
void
list_reverse(struct hdbmk_block **list)
{
  struct hdbmk_block  *k, *n, *t;

  k = *list;
  n = NULL;
  while(k != NULL){
      t = k->next;
      k->next = n;
      n = k;
      k = t;
  }

  *list = n;
  return;
}


/* tbuf_load()
**   in hash subtable tbuf with tslots slots,
**   upak/load record pointer rpos into slot nearest to target slot
**
**   return:
**     0 : success
**    -1 : internal error (shouldn't happen!)
**
**   notes:
**     target is used as an integral index into the table:
**     ie, 0 <= target < tslots
**     each slot is 8 bytes:
**       4 bytes hash value
**       4 bytes rpos (0 if slot empty)
*/ 
static
int
tbuf_load(uchar_t *tbuf, uint32_t tslots, uint32_t target, uint32_t hash, uint32_t rpos)
{
  uchar_t   *slotbuf;

  if(! (target < tslots)){
      errno = ERANGE;
      return -1;
  }

  for(;;){
      /* index to target slot (target * 8 bytes): */
      slotbuf = tbuf + (target << 3);
      /* if rpos in slot is 0 (empty), load hash/rpos: */
      if(upak32_UNPACK(slotbuf + 4) == 0){
          upak_pack(slotbuf, "dd", hash, rpos);
          break;
      }
      /* else increment target for retry: */
      ++target;
      if(target == tslots) target = 0;
#if 0
      /* fail if target == start (shouldn't happen!): */
      if(target == start){
          errno = ERANGE;
          return -1;
      }
#endif
  } 

  return 0;
}


/*
** public:
*/

/*
** notes:
**   hdbmk_finish internals are much simpler than ezcdb because
**   input block lists are already partitioned according to subtable
**
**        blocklists   subtables
**        ----------   ---------
**   cdb     4            256
**   hdb     8              8
*/

int
hdbmk_finish(struct hdbmk *M)
{
  /* hbase initialized from current file offset: */
  uint32_t        hbase = (uint32_t)M->fp;
  uint32_t        fp_eof;
  uint32_t        slots, sum_slots;
  uint32_t        toff;
  uint32_t        nrecs;
  uint32_t        subtab_slots[HDB_NTABS];
  uint32_t        subtab_offset[HDB_NTABS];
  uint32_t        slot_max;
  uchar_t        *tbuf = NULL;
  uchar_t         nbuf[16];
  ssize_t         w;
  int             i, k;
  struct hdbmk_block *block = NULL;

  /* compute nrecs, slots, offsets and list counts for hash subtables: */
  for(i = 0, nrecs = 0, sum_slots = 0, toff = 0; i < HDB_NTABS; ++i){
     uint32_t  n = M->subtab_count[i];
     /* metadata record counter (for hdb header): */
     nrecs += n;
     /* hash table loading factor (0.5): */
     slots = n << 1;
     subtab_slots[i] = slots;
     sum_slots += slots;
     subtab_offset[i] = toff;
     /* update toff with overflow check: */
     if(ufunc_u32add(&toff, (slots * 8)) == -1) return -1;
  } 

  /* overflow check: */
  fp_eof = hbase;
  if(ufunc_u32add(&fp_eof, (sum_slots * 8)) == -1) return -1;

  /* get size of largest input list: */
  for(i = 0, slot_max = 0; i < HDB_NTABS; ++i){
      if(slot_max < subtab_slots[i])
          slot_max = subtab_slots[i];
  }

  /* note:
  **
  ** the scheme of building separate record lists on input is to
  ** provide a smaller allocation requirement for tbuf here than
  ** would otherwise be the case, and is designed specifically to
  ** support improved hdbmk operation on memory constrained systems
  **
  */

  /* allocate/initialize tbuf accomodating largest input list: */
  tbuf = (uchar_t *)malloc(slot_max * 8);
  if(tbuf == NULL){
      return -1;
  }

  /*
  ** section H: write hash tables
  */

  /* scan all input_list[]: */
  for(i = 0; i < HDB_NTABS; ++i){
      /* slots in this subtable: */
      slots = subtab_slots[i];
      /* reset tbuf: */
      buf_zero(tbuf, slots * 8);
      /* restore entry order for stable hash key insertion: */
      list_reverse(&M->block_list[i]);
      /* scan blocks in this input_list and load tbuf: */
      for(block = M->block_list[i]; block != NULL; block = block->next){
          uint32_t  hash, rpos, target;
          /* process records in this block: */
          for(k = 0; k < block->n; ++k){
              hash = block->record[k].value;   /* hash for this record */
              rpos = block->record[k].offset;  /* offset to the record data */
              target = hdb_SLOT(hash, slots);  /* target slot for this entry */
              /* load/pack hash entry into slot nearest target: */
              if(tbuf_load(tbuf, slots, target, hash, rpos) == -1){
                  free(tbuf);
                  return -1;
              }
          }
      }
      /* write this tbuf: */
      if(ioq_put(&M->ioq, tbuf, subtab_slots[i] * 8) == -1){
          free(tbuf);
          return -1;
      }
  }

  /* release tbuf: */
  free(tbuf);

  /* flush output for section H: */
  if(ioq_flush(&M->ioq) == -1){
      return -1;
  }

  /* reset fd to offset 0: */
  if(lseek(M->fd, 0, SEEK_SET) == -1){
      return -1;
  }

  /* write section I identifier: */
  if(ioq_put(&M->ioq, (uchar_t *)HDB32_IDENT, HDB_IDLEN) == -1){
      return -1;
  }

  /* write section M metadata: */
  w = upak_pack(nbuf, "dd", nrecs, M->rbase);
  if(ioq_put(&M->ioq, nbuf, w) == -1){
      return -1;
  }

  /* write section P subtable pointers: */
  for(i = 0; i < HDB_NTABS; ++i){
      w = upak_pack(nbuf, "dd", subtab_slots[i], hbase + subtab_offset[i]);
      if(ioq_put(&M->ioq, nbuf, w) == -1){
          return -1;
      }
  }

  /* flush output for headers: */
  if(ioq_flush(&M->ioq) == -1){
      return -1;
  }

  /* note: caller should close/fsync fd, hdbmk_clear() */
  return 0;
}


/* eof (hdbmk_finish.c) */
