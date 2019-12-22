/* hdbmk_clear.c
** hdb file writer/generator
** wcm, 2010.05.27 - 2010.12.14
** ===
*/

/* libc: */
#include <stdlib.h>

/* libhdb: */
#include "hdbmk.h"


/* hdbmk_clear()
**   deallocate/reset hdbmk_t object
*/

void
hdbmk_clear(struct hdbmk *M)
{
  struct hdbmk_block  *block, *next;
  int                  i;

  if(M == NULL)
      return;

  /* free input blocks: */
  for(i = 0; i < HDB_NTABS; ++i){
      block = M->block_list[i];
      for(;;){
          if(block == NULL) break;
          next = block->next;
          free(block);
          block = next;
      }
      M->block_list[i] = NULL;
  }
    
  return;
}

/* eof (hdbmk_clear.c) */
