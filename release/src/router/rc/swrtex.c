#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "rc.h"
#include <shared.h>
#include <shutils.h>

void patch_Factory()
{
	int i, mtdfd;
	mtdfd = open("/dev/mtdblock2", O_RDWR|O_SYNC);
	if (mtdfd >= 0)
	{
		unsigned char buffer[99];
		int offset[]={0xff80, 0xff90, 0xffa0, 0xffb0, 0xffc0, 0xffd0, 0xffe0, 0xfff0, 0xff30, 0xff70, 0xff78, 0xff7a, 0xff7e};
		memset(buffer, 0, sizeof(buffer));
		lseek(mtdfd, offset[1], SEEK_SET);
		read(mtdfd, buffer, 3);
		if(buffer[0]==0xFF || buffer[0]==0x00 ){
			lseek(mtdfd, offset[1], SEEK_SET);
			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, "%s","CN/01");
			write(mtdfd, buffer, strlen(buffer));
			lseek(mtdfd, offset[3], SEEK_SET);
			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, "%s","RM-AC2100");
			write(mtdfd, buffer, strlen(buffer));
			lseek(mtdfd, offset[9], SEEK_SET);
			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, "%s","14725836");
			write(mtdfd, buffer, strlen(buffer));
			lseek(mtdfd, offset[10], SEEK_SET);
			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, "%s","CN");
			write(mtdfd, buffer, strlen(buffer));
			lseek(mtdfd, offset[11], SEEK_SET);
			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, "%s","1000");
			write(mtdfd, buffer, strlen(buffer));
		}
		close(mtdfd);
		printf("patch ok\n");
	} else{
		printf("can't open flash, patch failed\n");
	}
}
