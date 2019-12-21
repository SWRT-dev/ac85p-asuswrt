/* packet_read.c
** packet i/o module
** wcm, 2008.01.04 - 2010.12.01
** ===
*/

#include <unistd.h>
#include <errno.h>

#include "buf.h"
#include "packet.h"


int
new_packet_read(int fd, struct packet *k)
{
    ssize_t  r = 0;
    char     buf[260];
    size_t   len;

    do{
        r = read(fd, buf, sizeof buf);
    }while((r == -1) && (errno == EINTR));

    if(r == -1){
        /* error during read(), errno set */
        return -1;
    }

    if(r < 3){
        /* protocol error: */ 
        errno = EPROTO;
        return -1;
    }

    len = (size_t)buf[2];
    if(r != (ssize_t)(len + 3)){
        /* length mismatch (protocol error or short read): */ 
        errno = EPROTO;
        return -1;
    }

    k->proto_id = buf[0];
    k->type_id  = buf[1];
    k->len      = (uint8_t)len;
    if(len > 0)
        buf_copy(k->data, &buf[3], len);

    return 0;
}


ssize_t
packet_read(int fd, void *packet, size_t n)
{
   ssize_t  r = 0;

   do{
       r = read(fd, packet, n);
   }while((r == -1) && (errno == EINTR));

   return r;   
}


/* EOF (packet_read.c) */
