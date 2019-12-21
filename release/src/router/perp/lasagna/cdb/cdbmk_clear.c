/* cdbmk_clear.c
** cdb file writer/generator
** wcm, 2010.05.27 - 2010.11.16
** ===
*/

/* libc: */
#include <stdlib.h>

/* libcdb: */
#include "cdbmk.h"


/* cdbmk_clear()
**   deallocate/reset cdbmk_t object
*/

void
cdbmk_clear(struct cdbmk *M)
{
  struct input_block  *block, *next;
  int                  i;

  if(M == NULL)
      return;

  /* free input blocks: */
  for(i = 0; i < CDBMK_NLISTS; ++i){
      block = M->input_list[i];
      for(;;){
          if(block == NULL) break;
          next = block->next;
          free(block);
          block = next;
      }
      M->input_list[i] = NULL;
  }
    
  return;
}

/* eof (cdbmk_clear.c) */
