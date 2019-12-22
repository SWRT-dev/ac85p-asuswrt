/* outvec_put.c
** output using writev()
** wcm, 2010.11.27 - 2010.12.01
** ===
*/

#include "outvec.h"

int
outvec_put(struct outvec *vec, const void *buf, size_t len)
{
    if(!(vec->n < vec->max)){
        if(outvec_flush(vec) == -1){
            return -1;
        }
    }

    vec->vec[vec->n].iov_base = (void *)buf;
    vec->vec[vec->n].iov_len = len;
    ++vec->n;

    return 0;
}


/* eof: outvec_put.c */
