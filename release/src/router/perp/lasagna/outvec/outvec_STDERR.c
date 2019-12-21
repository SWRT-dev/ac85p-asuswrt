/* outvec_STDERR.c
** output using writev()
** wcm, 2010.11.27 - 2010.12.01
** ===
*/

#include "outvec.h"

/*
** predefine outvec objects for stderr:
*/

/* unbuffered stderr (flushme set): */ 
#define _VEC_STDERR_SIZE  16
static struct iovec _VEC_STDERR[_VEC_STDERR_SIZE];
struct outvec OUTVEC_STDERR = outvec_INIT(2, _VEC_STDERR, _VEC_STDERR_SIZE, 1);

/* eof: outvec_STDERR.c */
