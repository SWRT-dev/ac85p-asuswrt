/* pidlock_check.c
** wcm, 2008.10.09 - 2009.09.29
** ===
*/

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "padlock.h"

#include "pidlock.h"


pid_t
pidlock_check(const char *lockfile)
{
    int          fd;
    pid_t        pid;
    int          terrno;
    int          e;

    fd = open(lockfile, (O_RDONLY | O_NONBLOCK), 0);
    if(fd == -1){
        if(errno == ENOENT)
            return 0;
        else
            return -1;
    }

    /* test for acquiring shared lock: */
    pid = padlock_shtest(fd);

    terrno = errno;
    do{
        e = close(fd);
    }while((e == -1) && (errno == EINTR));
    errno = terrno;

    return pid; 
}


/* eof: pidlock_check.c */
