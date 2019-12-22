/* ioq_stderr.c
** wcm, 2004.04.21 - 2009.08.12
** ===
*/
#include <unistd.h>

#include "ioq.h"
#include "ioq_std.h"


/* stderr: */
uchar_t ioq_fd2_buf[256];
static ioq_t fd2_ = ioq_INIT(2, ioq_fd2_buf, sizeof ioq_fd2_buf, &write);
ioq_t *ioq2 = &fd2_;


/* eof: ioq_stderr.c */
