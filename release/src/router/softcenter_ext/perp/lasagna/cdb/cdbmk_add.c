/* cdbmk_add.c
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
#include <errno.h>

/* libaxative: */
#include "buf.h"
#include "ioq.h"
#include "ufunc.h"
#include "upak.h"

/* libcdb: */
#include "cdb.h"
#include "cdbmk.h"


/*
** public:
*/

/* cdbmk_add()
**   add a record to cdb
**     klen: length of key
**     key:  key
**     dlen: length of data
**     data: data
**
**    return:
**      0 : success, no error
**     -1 : failure, errno set
*/
int
cdbmk_add(struct cdbmk *M,
  const uchar_t *key, uint32_t klen,
  const uchar_t *data, uint32_t dlen)
{
  uint32_t               rpos = M->fp;
  uchar_t                buf[8];
  uint32_t               hash;
  uint32_t               fp_up;

  /* check for overflow: */
  fp_up = rpos;
  if(ufunc_u32add(&fp_up, 8) == -1) return -1;
  if(ufunc_u32add(&fp_up, klen) == -1) return -1;
  if(ufunc_u32add(&fp_up, dlen) == -1) return -1;

  upak_pack(buf, "dd", klen, dlen);

  /* write record: */
  if((ioq_put(&M->ioq, buf, sizeof buf) == -1) ||
     (ioq_put(&M->ioq, key, klen) == -1) ||
     (ioq_put(&M->ioq, data, dlen) == -1))
  {
      return -1;
  }

  /* hash calc: */
  hash = cdb_hash(key, klen);

  /* store/update block: */
  return cdbmk__update(M, hash, rpos, fp_up);
}

/* eof (cdbmk_add.c) */
