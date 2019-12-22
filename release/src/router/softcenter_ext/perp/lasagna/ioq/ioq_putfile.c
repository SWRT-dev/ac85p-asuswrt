/* ioq_putfile.c
** wcm, 2010.01.07 - 2012.07.24
** ===
*/
/* stdlib: */
#include <errno.h>
/* unix: */
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/* lasagna: */
#include "buf.h"
#include "cstr.h"
#include "uchar.h"

/* ioq module: */
#include "ioq.h"


/* ioq_putfile()
**   append contents of filename to internal buffer
**   calls ioq_flush() as needed to handle all bytes of file
**   return
**     0 : success, no error
**    -1 : open(), stat(), mmap(), or write() error, errno set
**
**   notes:
**     uses mmap()
**     does not perform final ioq_flush()
*/

int
ioq_putfile(ioq_t *ioq, const char *filename)
{
    int           fd = 0;
    struct  stat  st;
    size_t        len = 0;
    int           terrno;

    if((fd = open(filename, O_RDONLY)) == -1){
        goto fail;
    }

    if(fstat(fd, &st) == -1){
        goto fail;
    }

    len = st.st_size;
    if(len == 0){
        close(fd);
        return 0;
    }

    if(ioq_putfd(ioq, fd, len) == -1){
        goto fail;
    }

    /* success: */
    close(fd);
    return 0; 

fail:
    terrno = errno;
    if(fd) close(fd);
    errno = terrno;
    return -1;
}


/* eof: ioq_putfile.c */
