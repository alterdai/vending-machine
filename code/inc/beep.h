#ifndef _BEEP_
#define _BEEP_

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

void beep(int on, int time);

#endif