#include "beep.h"

void beep(int on, int time)
{
    int fd;
    int ret;
    fd = open("/dev/beep", O_RDWR);
    if (fd < 0)
    {
        perror("open:");
        return;
    }

    if (on)
    {
        printf("Buzzer On\n");
        ret = ioctl(fd, 1, 1);
        if (ret < 0)
        {
            perror("ioctl:");
            return;
        }
    }
    else
    {
        printf("Buzzer Off\n");
        ret = ioctl(fd, 0, 1);
        if (ret < 0)
        {
            perror("ioctl:");
            return;
        }
    }

    sleep(time);
    close(fd);
}
