/* hdbmk_add.c
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
#include <errno.h>

/* libaxative: */
#include "buf.h"
#include "ioq.h"
#include "ufunc.h"
#include "upak.h"

/* libhdb: */
#include "hdb.h"
#include "hdbmk.h"


/*
** public:
*/

/* hdbmk_add()
**   add a record to hdb
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
hdbmk_add(struct hdbmk *M,
  const uchar_t *key, uint32_t klen,
  const uchar_t *data, uint32_t dlen)
{
  uint32_t               rpos = M->fp;
  uchar_t                buf[6];
  uint32_t               hash;
  uint32_t               fp_up;

  /* constrain input to 24-bit max: */
  if((klen > HDB_U24MAX) || (dlen > HDB_U24MAX)){
      errno = ERANGE;
      return -1;
  }

  /* check for overflow: */
  fp_up = rpos;
  if(ufunc_u32add(&fp_up, sizeof buf) == -1) return -1;
  if(ufunc_u32add(&fp_up, klen) == -1) return -1;
  if(ufunc_u32add(&fp_up, dlen) == -1) return -1;

  upak24_pack(buf, klen);
  upak24_pack(buf + 3, dlen);

  /* write record: */
  if((ioq_put(&M->ioq, buf, sizeof buf) == -1) ||
     (ioq_put(&M->ioq, key, klen) == -1) ||
     (ioq_put(&M->ioq, data, dlen) == -1))
  {
      return -1;
  }

  /* hash calc: */
  hash = hdb_hash(key, klen);

  /* store/update block: */
  return hdbmk__update(M, hash, rpos, fp_up);
}

/* eof (hdbmk_add.c) */
