/* pidlock_set.c
** wcm, 2008.10.09 - 2011.02.01
** ===
*/

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "cstr.h"
#include "nfmt.h"
#include "padlock.h"

#include "pidlock.h"


static
int
do_pid(int fd, pid_t pid)
{
    char    nfmt[NFMT_SIZE];
    size_t  n;
    int     e = 0;

    do{
        e = ftruncate(fd, 0);
    }while((e == -1) && (errno == EINTR));
    if(e == -1){
        return -1;
    }

    nfmt_uint32(nfmt, (uint32_t) pid);
    /* append newline: */
    n = cstr_len(nfmt);
    nfmt[n] = '\n';
    ++n;
    nfmt[n] = '\0';
    
    do{
        e = write(fd, nfmt, n);
    }while((e == -1) && (errno == EINTR));
    if((e < 0) || ((size_t)e != n)){
        return -1;
    }

    /* success: */
    return 0;
}


int
pidlock_set(const char *lockfile, pid_t pid, enum pidlock_wait lockwait)
{
    int    fd;
    int    in_use = 0;
    int    e, terrno;

    fd = open(lockfile, (O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK), 0600);
    if(fd == -1){
        return -1;
    }

    e = padlock_exlock(fd, lockwait);
    if(e == -1){
        if((errno == EACCES) || (errno == EAGAIN)){
            /* lock held by another process, don't unlink it: */
            in_use = 1;
        }
        goto fail;
    }

    if(pid){
        if(do_pid(fd, pid) == -1)
            goto fail;
    }

    if(fchmod(fd, 0644) == -1){
        goto fail;
    }

    /* success: */
    return fd;


fail:
    terrno = errno;
    {
        do{
            e = close(fd);
        }while((e == -1) && (errno == EINTR));
        if(!in_use){
            padlock_unlock(fd, PADLOCK_NOW);
            unlink(lockfile);
        }
    }
    errno = terrno;

    return -1;
}


/* eof: pidlock_set.c */
