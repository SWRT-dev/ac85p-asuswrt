/* cdbmk_addrec.c
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

/* libcdb: */
#include "cdb.h"
#include "cdbmk.h"


/* cdbmk_addrec()
**   add record from current cursor in C to M
**   return:
**     0 : success, no error
**    -1 : failure, errno set
*/
int
cdbmk_addrec(struct cdbmk *M, struct cdb *C)
{
  uint32_t    fp = M->fp;
  uchar_t    *map = cdb_map(C);
  uint32_t    klen, dlen, rpos;
  uint32_t    fp_up;
  uint32_t    hash;
  uchar_t     buf[512];
  uint32_t    bufsize = (uint32_t) sizeof buf;
  uint32_t    r;

  if(map != NULL){
      /* easy if cdb is mmap()'d: */
      return cdbmk_add(M,
                       map + cdb_kpos(C), cdb_klen(C),
                       map + cdb_dpos(C), cdb_dlen(C));
  }

  /* here if cdb *not* mmap()'d
  ** process key/data incrementally through intermediate buf[]
  */

  klen = cdb_klen(C);
  dlen = cdb_dlen(C);
  rpos = cdb_rpos(C);

  /* check for overflow: */
  fp_up = fp;
  if(ufunc_u32add(&fp_up, 8) == -1) return -1;
  if(ufunc_u32add(&fp_up, klen) == -1) return -1;
  if(ufunc_u32add(&fp_up, dlen) == -1) return -1;

  /* key: */
  hash = CDB_HASHINIT;
  while(klen){
      r = (klen < bufsize) ? klen : bufsize;
      if(cdb_read(C, buf, r, rpos) == -1){
          return -1;
      }
      hash = cdb_hashpart(hash, buf, r);
      if(ioq_put(&M->ioq, buf, r) == -1){
          return -1;
      }
      rpos += r;
      klen -= r;
  }

  /* data: */
  while(dlen){
      r = (dlen < bufsize) ? dlen : bufsize;
      if(cdb_read(C, buf, r, rpos) == -1){
          return -1;
      }
      hash = cdb_hashpart(hash, buf, r);
      if(ioq_put(&M->ioq, buf, r) == -1){
          return -1;
      }
      rpos += r;
      dlen -= r;
  }

  /* store/update block: */
  return cdbmk__update(M, hash, fp, fp_up);
}

/* eof (cdbmk_addrec.c) */
