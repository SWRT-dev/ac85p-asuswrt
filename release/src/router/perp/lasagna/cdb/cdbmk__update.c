/* cdbmk__update.c
** cdb file writer/generator
** wcm, 2010.05.27 - 2010.11.16
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

/* libcdb: */
#include "cdb.h"
#include "cdbmk.h"

/* cdbmk__update()
** internal subroutine common to cdbmk_add*() functions
*/

/* cdbmk__update()
**   update block list for cdbmk_add*()
**   update M->fp, M->nrecs, M->subtabs[].n counters
**
**   return:
**     0 : no error
**    -1 : error (malloc() failure for new block)
*/
int
cdbmk__update(struct cdbmk *M, uint32_t hash, uint32_t rpos, uint32_t fp_up)
{
  struct input_block  *block;
  int                  ntab, nlist;

  /* subtable for this hash value: */
  ntab = cdb_NTAB(hash);

  /* input list: */
  nlist = cdbmk_NLIST(ntab);

  /* store/update block for selected subtable: */
  block = M->input_list[nlist];
  if((block == NULL) || (block->n >= CDBMK_NRECS)){
      block = (struct input_block *)malloc(sizeof (struct input_block));
      if(block == NULL){
          return -1;
      }
      block->n = 0;
      block->next = M->input_list[nlist];
      M->input_list[nlist] = block;
  }

  block->record[block->n].hash = hash;
  block->record[block->n].rpos = rpos;
  ++block->n;

  /* update file position offset: */
  M->fp = (off_t)fp_up;

  /* update counters: */
  ++M->subtab_count[ntab];

  return 0;
}

/* eof (cdbmk__update.c) */
