/* hdbmk_addrec.c
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

/* libhdb: */
#include "hdb.h"
#include "hdbmk.h"


/* hdbmk_addrec()
**   add record from current cursor in H to M
**   return:
**     0 : success, no error
**    -1 : failure, errno set
*/
int
hdbmk_addrec(struct hdbmk *M, struct hdb *H)
{
  uint32_t    fp = M->fp;
  uchar_t    *map = hdb_map(H);
  uint32_t    klen, dlen, rpos;
  uint32_t    fp_up;
  uint32_t    hash;
  uchar_t     buf[512];
  uint32_t    bufsize = (uint32_t) sizeof buf;
  uint32_t    r;

  if(map != NULL){
      /* easy if hdb is mmap()'d: */
      return hdbmk_add(M,
                       map + hdb_kpos(H), hdb_klen(H),
                       map + hdb_dpos(H), hdb_dlen(H));
  }

  /* here if hdb *not* mmap()'d
  ** process key/data incrementally through intermediate buf[]
  */

  klen = hdb_klen(H);
  dlen = hdb_dlen(H);
  rpos = hdb_rpos(H);

  /* check for overflow: */
  fp_up = fp;
  if(ufunc_u32add(&fp_up, 8) == -1) return -1;
  if(ufunc_u32add(&fp_up, klen) == -1) return -1;
  if(ufunc_u32add(&fp_up, dlen) == -1) return -1;

  /* key: */
  hash = HDB_HASHINIT;
  while(klen){
      r = (klen < bufsize) ? klen : bufsize;
      if(hdb_read(H, buf, r, rpos) == -1){
          return -1;
      }
      hash = hdb_hashpart(hash, buf, r);
      if(ioq_put(&M->ioq, buf, r) == -1){
          return -1;
      }
      rpos += r;
      klen -= r;
  }

  /* data: */
  while(dlen){
      r = (dlen < bufsize) ? dlen : bufsize;
      if(hdb_read(H, buf, r, rpos) == -1){
          return -1;
      }
      hash = hdb_hashpart(hash, buf, r);
      if(ioq_put(&M->ioq, buf, r) == -1){
          return -1;
      }
      rpos += r;
      dlen -= r;
  }

  /* store/update block: */
  return hdbmk__update(M, hash, fp, fp_up);
}

/* eof (hdbmk_addrec.c) */
