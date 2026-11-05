#include "touch.h"
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
static int fd=-1, px, py, have_x, have_y;
int touch_open(void) { fd=open("/dev/input/event0",O_RDONLY); return fd<0 ? -1 : 0; }
void touch_close(void) { if(fd>=0) close(fd); fd=-1; }
int get_xy(int *x,int *y) {
    struct pollfd p={fd,POLLIN,0};
    struct input_event e;
    int ret;
    for (;;) {
        ret=poll(&p,1,1000);
        if(ret<0 && errno==EINTR) return 1;
        if(ret==0) return 1;
        if(ret<0 || !(p.revents&POLLIN)) return -1;
        if(read(fd,&e,sizeof e)!=(ssize_t)sizeof e) return -1;
        if(e.type==EV_ABS && e.code==ABS_X) { px=e.value; have_x=1; }
        if(e.type==EV_ABS && e.code==ABS_Y) { py=e.value; have_y=1; }
        /* Release follows coordinate updates on the original pixel-coordinate panel. */
        if(((e.type==EV_KEY && e.code==BTN_TOUCH) ||
            (e.type==EV_ABS && e.code==ABS_PRESSURE)) && e.value==0 && have_x && have_y) {
            *x=px; *y=py; return 0;
        }
    }
}
