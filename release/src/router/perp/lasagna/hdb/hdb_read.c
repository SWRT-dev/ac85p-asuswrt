/* hdb_read.c
** wcm, 2010.05.29 - 2010.12.14
** ===
*/

/* libc: */
#include <stddef.h>
#include <stdint.h>

/* unix: */
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

/* lasagna: */
#include "buf.h"
#include "uchar.h"

/* libhdb: */
#include "hdb.h"


int
hdb_read(struct hdb *H, uchar_t *buf, size_t len, uint32_t offset)
{
  int      fd = H->fd;
  ssize_t  r;

  /* if mmap'ed, "read" from mmap: */
  if(H->map != 0){
      if((offset >= H->map_size) || ((H->map_size - offset) < len)){
          errno = EPROTO;
          return -1;
      }
      buf_copy(buf, H->map + offset, len);
      return 0;
  }
      
  /* else read() from fd ... */

  if(lseek(fd, (off_t)offset, SEEK_SET) == -1){
      return -1;
  }

  while(len){
      do{
          r = read(fd, buf, len);
      }while((r == -1) && (errno == EINTR));

      switch(r){
      case -1: /* io error */
          return -1; break;
      case  0: /* unexpected eof */
          errno = EPROTO;
          return -1; break;
      default:
          buf += r;
          len -= r;
      }
  }

  return 0;    
}

/* eof (hdb_read.c) */
