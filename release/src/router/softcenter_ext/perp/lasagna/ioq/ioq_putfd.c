/* ioq_putfd.c
** wcm, 2010.01.07 - 2012.07.24
** ===
*/
/* stdlib: */
#include <errno.h>
/* unix: */
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>

/* lasagna: */
#include "buf.h"
#include "cstr.h"
#include "uchar.h"

/* ioq module: */
#include "ioq.h"


/* ioq_putfd()
**   append contents of open file descriptor fd to internal buffer
**   calls ioq_flush() as needed to handle all bytes of file
**   return
**     0 : success, no error
**    -1 : mmap(), or write() error, errno set
**
**   notes:
**     caller supplies size of open/readable fd from stat() call
**     uses mmap()
**     does not perform final ioq_flush()
**     does not close() fd
*/


/* XXX:
**   if mmap() fails, fallback to buffered read() operations
*/

int
ioq_putfd(ioq_t *ioq, int fd, size_t len)
{
    void         *map = NULL;
    int           terrno;

    if(len == 0){
        return 0;
    }

    map = (uchar_t *)mmap(0, len, PROT_READ, MAP_SHARED, fd, 0);
    if(map == MAP_FAILED){
        goto fail;
    }

    if(ioq_put(ioq, (const uchar_t *)map, len) == -1){
        goto fail;
    }

    /* success: */
    munmap(map, len);
    return 0; 

fail:
    terrno = errno;
    if(map) munmap(map, len);
    errno = terrno;
    return -1;
}


/* eof: ioq_putffd.c */
