/* cdb_init.c
** wcm, 2010.05.29 - 2010.10.25
** ===
*/

#include <stddef.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

/* lasagna: */
#include "buf.h"
#include "uchar.h"
#include "upak.h"

/* libcdb: */
#include "cdb.h"


/* init_buf size for first entry of P in file header: */
#define INITBUF_SIZE  4


int
cdb_init(struct cdb *C, int fd)
{
  struct stat  sb;
  uchar_t     *map;
  uchar_t      buf[INITBUF_SIZE];

  buf_zero(C, sizeof (struct cdb));

  /* try to mmap() fd: */
  if((fstat(fd, &sb) == 0) && (sb.st_size <= (off_t)0xffffffff)){
         map = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
         if(map != MAP_FAILED){
             C->map = map;
             C->map_size = sb.st_size;
         }
  }
  C->fd = fd;

  /* suck in file header: */
  if(cdb_read(C, buf, sizeof buf, 0) == -1){
      return -1;
  }

  /* C->hbase: offset to base of hash table (also eod): */
  C->hbase = upak32_unpack(buf);

  return 0;
}


/* eof (cdb_init.c) */
