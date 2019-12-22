/* hdbmk__update.c
** hdb file writer/generator
** wcm, 2010.05.27 - 2010.12.14
** ===
*/

/* libc: */
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

/* unix: */
#include <sys/types.h>
#include <unistd.h>

/* libaxative: */

/* libhdb: */
#include "hdb.h"
#include "hdbmk.h"

/* hdbmk__update()
** internal subroutine common to hdbmk_add*() functions
*/

/* hdbmk__update()
**   update block list for hdbmk_add*()
**   update M->fp, M->nrecs, M->subtabs[].n counters
**
**   return:
**     0 : no error
**    -1 : error (malloc() failure for new block)
*/
int
hdbmk__update(struct hdbmk *M, uint32_t hash, uint32_t rpos, uint32_t fp_up)
{
  struct hdbmk_block  *block;
  int                  ntab, nlist;

  /* subtable for this hash value: */
  ntab = hdb_NTAB(hash);

  /* input list: */
  nlist = ntab;

  /* store/update block for selected subtable: */
  block = M->block_list[nlist];
  if((block == NULL) || (block->n >= HDBMK_NRECS)){
      block = (struct hdbmk_block *)malloc(sizeof (struct hdbmk_block));
      if(block == NULL){
          return -1;
      }
      block->n = 0;
      block->next = M->block_list[nlist];
      M->block_list[nlist] = block;
  }

  block->record[block->n].value  = hash;
  block->record[block->n].offset = rpos;
  ++block->n;

  /* update file position offset: */
  M->fp = (off_t)fp_up;

  /* update counters: */
  ++M->subtab_count[ntab];

  return 0;
}

/* eof (hdbmk__update.c) */
