/* cdbmk_init.c
** cdb file writer/generator
** wcm, 2010.05.27 - 2010.11.16
** ===
*/

/* libc: */

/* unix: */
#include <unistd.h>
#include <sys/types.h>

/* libaxative: */
#include "ioq.h"

/* libcdb: */
#include "cdb.h"
#include "cdbmk.h"



/* cdbmk_init()
**   initialize cdbmk generator
**   return:
**     0: success, ioq/fd positioned for writing record data
**    -1: failure, errno set (lseek())
*/

int
cdbmk_init(struct cdbmk *M, int fd)
{
  int              i;

  /* setup ioq writer: */
  M->fd = fd;
  ioq_init(&M->ioq, fd, M->buf, sizeof M->buf, &write);

  /* set fp to begin writing record section R (offset 2048 bytes): */
  M->fp = (off_t)(sizeof(uint32_t) * 2 * CDB_NTABS);

  /* initialize subtab counters: */
  for(i = 0; i < CDB_NTABS; ++i){
      M->subtab_count[i] = 0;
  }

  /* initialize input batch lists: */
  for(i = 0; i < CDBMK_NLISTS; ++i){
      /* block allocations as records added: */
      M->input_list[i] = NULL;
  }

  /* position for writing records: */
  return lseek(fd, M->fp, SEEK_SET);
}



/* eof (cdbmk_init.c) */
