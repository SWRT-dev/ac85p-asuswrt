/* catargs.c
** output argv in runargs format
** wcm, 2009.09.08 - 2011.01.31
** ===
*/
#include <stdlib.h>
#include <unistd.h>

#include "ioq.h"
#include "ioq_std.h"

int
main(int argc, char *argv[])
{
  (void)argc;

  while(*argv){
      ioq_vputs(ioq1, *argv, "\n");
      ++argv;
  }
  ioq_flush(ioq1);

  return 0;
}


/* eof: catargs.c */
