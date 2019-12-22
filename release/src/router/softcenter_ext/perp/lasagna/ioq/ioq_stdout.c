/* ioq_stdout.c
** wcm, 2004.04.21 - 2009.08.12
** ===
*/
#include <unistd.h>

#include "ioq.h"
#include "ioq_std.h"

/* stdout: */
uchar_t ioq_fd1_buf[IOQ_BUFSIZE];
static ioq_t fd1_  = ioq_INIT(1, ioq_fd1_buf, sizeof ioq_fd1_buf, &write);
ioq_t *ioq1 = &fd1_;

/* eof: ioq_stdout.c */
