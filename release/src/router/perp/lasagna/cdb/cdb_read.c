/* cdb_read.c
** wcm, 2010.05.29 - 2010.11.24
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

/* libcdb: */
#include "cdb.h"


int
cdb_read(struct cdb *C, uchar_t *buf, size_t len, uint32_t offset)
{
  int      fd = C->fd;
  ssize_t  r;

  /* if mmap'ed, "read" from mmap: */
  if(C->map != 0){
      if((offset >= C->map_size) || ((C->map_size - offset) < len)){
          errno = EPROTO;
          return -1;
      }
      buf_copy(buf, C->map + offset, len);
      return 0;
  }
      
  /* else read() from fd ... */

  if(lseek(fd, (off_t)offset, SEEK_SET) == -1)
      return -1;

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

/* eof (cdb_read.c) */
