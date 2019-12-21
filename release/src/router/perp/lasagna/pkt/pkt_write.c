/* pkt_write.c
** pkt: tiny packet protocol
** wcm, 2009.07.29 - 2011.01.24
** ===
*/

#include <unistd.h>
#include <errno.h>

#include "uchar.h"
#include "buf.h"
#include "pkt.h"

/* pkt_write():
**   write() pkt K[offset] to file descriptor fd
**   perform successive write() as necessary to complete K[2]+3 bytes
**
**   return
**    >0 : no error
**    -1 : write() error, errno set (some bytes may have been written)
**
**   notes:
**     behavior depends on blocking/non-blocking of fd
**     if fd is blocking:
**         any short write() error will return error
**     if fd is non-blocking:
**         short write() due to EAGAIN will return number of bytes written
**         in this call; caller should call again with adjusted offset
**         if the pkt_len(pkt) is incomplete
**
*/
ssize_t
pkt_write(int fd, const pkt_t pkt, size_t offset)
{
    uchar_t  *b = (uchar_t *)pkt;
    ssize_t   w;
    size_t    to_write = pkt_len(pkt);

    if(offset > to_write){
        errno = EINVAL;
        return -1;
    }

    b += offset;
    to_write -= offset;

    while(to_write > 0){

        do{
            w = write(fd, b, to_write);
        }while((w == -1) && (errno == EINTR));

        if(w == -1){
            if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                break;
            } else {
                return -1;
            }
        }

        if(w == 0) continue;

        b += w;
        to_write -= w;
    }

    return (b - offset - pkt); 
}

/* eof: pkt_write.c */
