/* cdbmk_addioq.c
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

/* cdbmk_addioq()
**   add next record from an ioq reader
**     ioq_in: ioq of input reader positioned for next key/data item
**     klen:   size of key
**     dlen:   size of data
**     sep:    explicit byte/character sequence between key/data
**     seplen: size of sep
**
**   return:
**     0 : success, no error
**    -1 : failure, errno set
**
**   notes:
**     "wirespeed" interface for cdbmk
**     no double-copying of data
*/
int
cdbmk_addioq(struct cdbmk *M,
  ioq_t *ioq_in,
  size_t klen, size_t dlen,
  uchar_t *sep, size_t seplen)
{
  uint32_t               rpos = M->fp;
  uchar_t                buf[8];
  uchar_t               *inbuf;
  uchar_t                ch;
  uint32_t               hash;
  uint32_t               fp_up;
  ssize_t                r;

  /* check for overflow: */
  fp_up = rpos;
  if(ufunc_u32add(&fp_up, 8) == -1) return -1;
  if(ufunc_u32add(&fp_up, klen) == -1) return -1;
  if(ufunc_u32add(&fp_up, dlen) == -1) return -1;

  upak_pack(buf, "dd", klen, dlen);
  if(ioq_put(&M->ioq, buf, sizeof buf) == -1){
      return -1;
  }

  /* key: */
  hash = CDB_HASHINIT;
  while(klen){
      r = ioq_feed(ioq_in);
      switch(r){
      case -1: return -1; break;
      case  0: errno = EPROTO; return -1; break;
      }
      inbuf = ioq_peek(ioq_in);
      if((uint32_t)r > klen) r = klen;
      hash = cdb_hashpart(hash, inbuf, r);
      if(ioq_put(&M->ioq, inbuf, r) == -1){
          return -1;
      }
      ioq_seek(ioq_in, r);
      klen -= r;
  } 

  /* drain separator from input: */
  while(seplen){
      switch(ioq_GETC(ioq_in, &ch)){
      case -1: return -1; break;
      case  0: errno = EPROTO; return -1; break;
      }
      if(ch != *sep){
          errno = EPROTO;
          return -1;
      }
      ++sep; --seplen;
  }

  /* data: */
  while(dlen){
      r = ioq_feed(ioq_in);
      switch(r){
      case -1: return -1; break;
      case  0: errno = EPROTO; return -1; break;
      }
      inbuf = ioq_peek(ioq_in);
      if((uint32_t)r > dlen) r = dlen;
      if(ioq_put(&M->ioq, inbuf, r) == -1){
          return -1;
      }
      ioq_seek(ioq_in, r);
      dlen -= r;
  } 

  /* store/update block: */
  return cdbmk__update(M, hash, rpos, fp_up);
}

/* eof (cdbmk_addioq.c) */
