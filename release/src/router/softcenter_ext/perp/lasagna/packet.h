/* packet.h
** packet i/o module
** wcm, 2008.01.04 - 2009.07.26
** ===
*/
#ifndef PACKET_H
#define PACKET_H 1

#include <stddef.h>
#include <stdint.h>

struct packet {
    char      proto_id; /* protocol identifier */
    char      type_id;  /* packet type identifier */
    uint8_t   len;      /* length of payload */
    char     *data;     /* payload */
};
typedef struct packet  packet_t;

#define packet_load(p, proto, type, len, data) \
    p = {proto, type, len, data}

/* packet_read():
**   read() at most to_read bytes from fd into packet
**   perform one read()
**   returns:
**     0 : eof
**    >0 : bytes read
**    -1 : io error
*/
extern
ssize_t
packet_read(int fd, void *packet, size_t to_read);


/* packet_write():
**   write() to_write bytes from packet to fd
**   perform successive write() as necessary
**   returns:
**     >0 : bytes written
**     -1 : io error (some bytes may have been written)
*/ 
extern
ssize_t
packet_write(int fd, const void *packet, size_t to_write);

#endif /* PACKET_H */
/* EOF (packet.h) */
