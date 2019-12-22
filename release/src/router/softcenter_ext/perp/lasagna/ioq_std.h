/* ioq_std.h
** ioq_std: ioq's for stdin, stdout, stderr
** wcm, 2004.04.21 - 2009.08.12
** ===
*/
#ifndef IOQ_STD_H
#define IOQ_STD_H

#include "ioq.h"


/* "standard" ioq buffers: */

/* stdin: ioq0
**
** note:
**   the read() operation installed in ioq0 follows the djb implementation,
**   first calls ioq_flush() on ioq1 (stdout) prior to read() on stdin
*/
extern ioq_t  *ioq0;

/* stdout: ioq1 */
extern ioq_t  *ioq1;

/* stderr: ioq2 */
extern ioq_t  *ioq2;

#endif /* IOQ_STD_H */
/* eof: ioq_std.h */
