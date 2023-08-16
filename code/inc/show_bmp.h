#ifndef _SHOW_BMP_
#define _SHOW_BMP_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


int show_bmp(char *pathname);
int show_bmp_any(char *pathname,int w, int h, int rx, int ry);

#endif