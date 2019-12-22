/* devout.c
** simple unbuffered variable argument output
** intended for sparing use, such as for stderr diagnostics
** as many write() syscalls as arguments!
** wcm, 2009.11.25 - 2009.11.25
** ===
*/

#include <stdarg.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>

#include "cstr.h"
#include "devout.h"


ssize_t
devout_(int fd, const char *s0, ...)
{
    va_list      args;
    const char  *s = s0;
    ssize_t      w, total = 0;

    va_start(args, s0);
    while(s != NULL){
        size_t  n = cstr_len(s);
        while(n > 0){
            do{
                w = write(fd, s, n);
            }while((w == -1) && (errno == EINTR));
            if(w == -1){
                return -1;
            }
            s += w;
            n -= w;
            total += w;
        }
        s = va_arg(args, const char *);
    }
    va_end(args);

    return total;
}


/* eof: devout.c */
