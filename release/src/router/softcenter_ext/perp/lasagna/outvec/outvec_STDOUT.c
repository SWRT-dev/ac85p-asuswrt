/* outvec_STDOUT.c
** output using writev()
** wcm, 2010.11.27 - 2010.12.01
** ===
*/

#include "outvec.h"

/* platform-specific define for _VEC_STDOUT_SIZE: */
#include "outvec_STDOUT.h"


/*
** predefine outvec objects for stdout:
*/

/* slightly buffered stdout (flushme not set): */
static struct iovec _VEC_STDOUT[_VEC_STDOUT_SIZE];
struct outvec OUTVEC_STDOUT = outvec_INIT(1, _VEC_STDOUT, _VEC_STDOUT_SIZE, 0);


/* eof: outvec_STDOUT.c */
