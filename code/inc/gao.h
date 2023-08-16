#ifndef _GAO_
#define _GAO_
#include"touch.h"
#include "show_bmp.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include<sys/stat.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <time.h>
#include "font.h"
#include "beep.h"

void gao1();
void gao2();
void gao3();
void gao4();
void update_cart(char *name, int delta);
void display_payment_options(void);
#endif