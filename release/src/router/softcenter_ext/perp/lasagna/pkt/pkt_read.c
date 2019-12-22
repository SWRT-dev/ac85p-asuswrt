/* pkt_read.c
** pkt: tiny packet protocol
** wcm, 2009.07.29 - 2011.01.24
** ===
*/

#include <unistd.h>
#include <errno.h>

#include "uchar.h"
#include "buf.h"
#include "pkt.h"

/* pkt_read():
**   from file descriptor fd, read() into pkt K[offset]
**   perform successive read() as necessary for K[2]+3 bytes
**   assumes reading an incoming pkt
**   checks size field (K[2]) of incoming pkt correlates with bytes read
**
**   return
**    >0 : no error, number bytes read()
**    -1 : error, errno set (some bytes may have been read)
**         read() error
**         EPROTO if read() size mismatch with K[2]
**
**   notes:
**     behavior depends on blocking/non-blocking of fd
**     if fd is blocking:
**         any short read() will return error
**     if fd is non-blocking:
**         short read() due to EAGAIN will return number of bytes read
**         in this call; caller should call again with adjusted offset
**         if the pkt_len(pkt) is incomplete
**
*/ 
ssize_t
pkt_read(int fd, pkt_t pkt, size_t offset) 
{
    size_t    n = 0;
    size_t    to_read = 0;
    size_t    max = sizeof(pkt_t);
    ssize_t   r;

    if(offset > max){
       errno = EINVAL;
       return -1;
    }
    /* else: */ 
    max -= offset;
    n += offset;

    to_read = (n < PKT_HEADER) ? PKT_HEADER : pkt_len(pkt);

    while(n < to_read){

        do{
            r = read(fd, &pkt[n], max);
        }while((r == -1) && (errno == EINTR));

        if(r == -1){
            if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                /* short read on non-blocking fd: */
                return n - offset;
            } else {
                /* error during read(), errno set */
                return -1;
            }
        }

        if(r == 0){
            /* eof: */
            return 0;
        }

        n += r;
        max -= r;

        to_read = (n < PKT_HEADER) ? PKT_HEADER : pkt_len(pkt);
    }

    /*
    ** here on:
    **   eof: if short read (n < to_read), EPROTO
    **   n > to_read: EPROTO
    **   n == to_read: ok
    **
    ** note: short read (n < to_read) on non-blocking fd filtered in loop
    */

    if(n != to_read){
        errno = EPROTO;
        return -1;
    }

    /* success: */
    return n - offset;
}


/* eof: pkt_read.c */
