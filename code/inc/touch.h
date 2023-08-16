#ifndef _TOUCH_H_
#define _TOUCH_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//struct input_event
#include <linux/input.h>

int get_xy(int *x, int *y);

#endif