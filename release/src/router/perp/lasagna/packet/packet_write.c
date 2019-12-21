/* packet_write.c
** packet i/o module
** wcm, 2008.01.04 - 2009.07.26
** ===
*/

#include <unistd.h>
#include <errno.h>

#include "buf.h"
#include "packet.h"

int
new_packet_write(int fd, struct packet *k)
{
    ssize_t  w = 0;
    size_t   len = (size_t)(k->len);
    char     buf[260];
    char    *b = buf;

    buf[0] = k->proto_id;
    buf[1] = k->type_id;
    buf[2] = (char)len;
    if(len > 0)
        buf_copy(&buf[3], k->data, len);

    len += 3;
    while(len > 0){

        do{
            w = write(fd, b, len);
        }while((w == -1) && (errno == EINTR));

        if(w == -1)
            return -1;

        if(w == 0)
            continue;

        b += w;
        len -= w;
    }

    return 0;
}


ssize_t
packet_write(int fd, const void *packet, size_t n)
{
   ssize_t  w = 0;
   size_t   to_write = n;

   while(to_write > 0){

       do{
           w = write(fd, packet, to_write);
       }while((w == -1) && (errno == EINTR));

       if(w == -1)
           return -1;

       if(w == 0)
           continue;

       packet += w;
       to_write -= w;
   }

   return n;
}


/* EOF (packet_write.c) */
