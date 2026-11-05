#ifndef TOUCH_H
#define TOUCH_H
/* 0: touch release, 1: timeout, -1: device error */
int get_xy(int *x, int *y);
int touch_open(void);
void touch_close(void);
#endif
