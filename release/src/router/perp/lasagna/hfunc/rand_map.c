/* rand_map.c
** generator of 256-byte random number table
** wcm, 2010.06.04 - 2010.06.05
** ===
*/

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "uchar.h"
#include "upak.h"

int
main(int argc, char *argv[])
{
  uchar_t  rand_buf[1024];
  size_t   buflen = sizeof rand_buf;
  uchar_t *b = rand_buf;
  ssize_t  r = 0;
  int      fd;
  int      row = 0, col = 0;
  int      i = 0;

  /* note:
  **   why not use random(3)?
  **   range of values returned by random() is [0 .. (2^31 - 1)]
  **   ie, 1-bit short of a full 32-bit value range
  **
  **   instead:
  **     draw 1028 bytes from the /dev/random entropy pool
  **     generate values in range [0 .. (2^32 -1)]
  */

  /* netbsd entropy pool: */
  if((fd = open("/dev/random", O_RDONLY)) == -1){
      fprintf(stderr, "failure open() on /dev/random\n");
      exit(111);
  }

  /* note: this can take some time! */
  while(buflen){
      do{
          r = read(fd, b, buflen);
      }while((r == -1) && ((errno == EINTR) || (errno == EAGAIN)));

      if(r == -1){
          fprintf(stderr, "failure read() on /dev/random: %s\n", strerror(errno));
          exit(111);
      }
      b += r;
      buflen -= r;
  }

  b = rand_buf;
  printf("static const uint32_t rand_map[] = {\n");
  for(row = 0; row < 32; ++row){
      printf("  ");
      for(col = 0; col < 8; ++col){
          printf("  0x%x%s", upak32_unpack(b), i < 255 ? "," : ""); 
          b += 4;
          ++i;
      }
      printf("\n");
  }
  printf("};\n");

  return 0;
}

/* eof: rand_map.c */
