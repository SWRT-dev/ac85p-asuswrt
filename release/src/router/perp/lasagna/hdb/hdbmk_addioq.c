/* hdbmk_addioq.c
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

/* hdbmk_addioq()
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
**     "wirespeed" interface for hdbmk
**     no double-copying of data
*/
int
hdbmk_addioq(struct hdbmk *M,
  ioq_t *ioq_in,
  size_t klen, size_t dlen,
  uchar_t *sep, size_t seplen)
{
  uint32_t               rpos = M->fp;
  uchar_t                buf[6];
  uchar_t               *inbuf;
  uchar_t                ch;
  uint32_t               hash;
  uint32_t               fp_up;
  ssize_t                r;

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

  /* start record: */
  if(ioq_put(&M->ioq, buf, sizeof buf) == -1){
      return -1;
  }

  /* key: */
  hash = HDB_HASHINIT;
  while(klen){
      r = ioq_feed(ioq_in);
      switch(r){
      case -1: return -1; break;
      case  0: errno = EPROTO; return -1; break;
      }
      inbuf = ioq_peek(ioq_in);
      if((uint32_t)r > klen) r = klen;
      hash = hdb_hashpart(hash, inbuf, r);
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
  return hdbmk__update(M, hash, rpos, fp_up);
}

/* eof (hdbmk_addioq.c) */
