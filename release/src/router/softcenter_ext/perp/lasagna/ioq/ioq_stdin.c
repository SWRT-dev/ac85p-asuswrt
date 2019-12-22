/* ioq_stdin.c
** wcm, 2004.04.21 - 2010.11.24
** ===
*/
#include <unistd.h>

#include "ioq.h"
#include "ioq_std.h"

/* reader for stdin: */
ssize_t ioq_fd0_read(int fd, void *buf, size_t len)
{
  /* note: standard djb, flushes stdout before read() on stdin */
  if(ioq_flush(ioq1) == -1)
      return -1;

  return read(fd, buf, len);
}

/* stdin: */
uchar_t ioq_fd0_buf[IOQ_BUFSIZE];
static ioq_t fd0_ = ioq_INIT(0, ioq_fd0_buf, sizeof ioq_fd0_buf, &ioq_fd0_read);
ioq_t *ioq0 = &fd0_;

/* eof: ioq_stdin.c */
