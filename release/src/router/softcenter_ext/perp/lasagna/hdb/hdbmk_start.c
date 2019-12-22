/* hdbmk_start.c
** hdb file writer/generator
** wcm, 2010.05.27 - 2010.12.14
** ===
*/

/* libc: */

/* unix: */
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

/* libaxative: */
#include "ioq.h"
#include "uchar.h"
#include "ufunc.h"

/* libhdb: */
#include "hdb.h"
#include "hdbmk.h"

/* hdbmk_start()
**   initialize hdbmk generator
**   return:
**     0: success, ioq/fd positioned for writing record data
**    -1: failure, errno set (lseek())
*/

int
hdbmk_start(struct hdbmk *M, int fd, const uchar_t *comment, uint32_t clen)
{
  uint32_t  fp;
  int       i;

  /* setup ioq writer: */
  M->fd = fd;
  ioq_init(&M->ioq, fd, M->buf, sizeof M->buf, &write);

  /* initialize subtab counters: */
  for(i = 0; i < HDB_NTABS; ++i){
      M->subtab_count[i] = 0;
  }

  /* initialize input batch lists: */
  for(i = 0; i < HDB_NTABS; ++i){
      /* block allocations as records added: */
      M->block_list[i] = NULL;
  }

  /* position fp for comment: */
  fp = (off_t)(HDB_CBASE);
  if(lseek(fd, fp, SEEK_SET) == -1){
      return -1;
  }

  if(clen){
      if(ufunc_u32add(&fp, clen) == -1){
          errno = ERANGE;
          return -1;
      }
      if(ioq_put(&M->ioq, comment, clen) == -1){
          return -1;
      }
  }

  /* fp positioned at start of record block: */
  M->rbase = M->fp = fp;

  return 0;
}


/* eof (hdbmk_start.c) */
