/* hdb_init.c
** wcm, 2010.05.29 - 2010.12.16
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

/* lasagna: */
#include "buf.h"
#include "cstr.h"
#include "uchar.h"
#include "upak.h"

/* libhdb: */
#include "hdb.h"


int
hdb_init(struct hdb *H, int fd)
{
  struct stat  sb;
  uchar_t     *map;
  uchar_t      header[HDB_CBASE];
  uchar_t     *buf = header;
  int          i = 0;

  buf_zero(H, sizeof (struct hdb));

  /* try to mmap() fd: */
  if((fstat(fd, &sb) == 0) && (sb.st_size <= (off_t)0xffffffff)){
         map = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
         if(map != MAP_FAILED){
             H->map = map;
             H->map_size = sb.st_size;
         }
  }
  H->fd = fd;

  /* suck in file header: */
  if(hdb_read(H, buf, sizeof header, 0) == -1){
      return -1;
  }

  /* validate ident: */
  if(buf_cmp(buf, HDB32_IDENT, HDB_IDLEN) != 0){
      errno = EPROTO;
      return -1;
  }

  /* section M metadata: */
  buf += HDB_IDLEN;
  H->nrecs = upak32_UNPACK(buf);
  buf += 4;
  H->rbase = upak32_UNPACK(buf);

  /* section P hash table index: */
  for(i = 0; i < HDB_NTABS; ++i){
      buf += 4;
      H->tndx[i].value = upak32_UNPACK(buf);
      buf += 4;
      H->tndx[i].offset = upak32_UNPACK(buf);
  }      

  /* H->cbase: offset to base of comment area: */
  H->cbase = HDB_CBASE;

  /* H->hbase: offset to base of hash table (also eod): */
  H->hbase = H->tndx[0].offset;

  return 0;
}


/* eof (hdb_init.c) */
