#include "touch.h"
extern time_t last_operation_time; 
extern pthread_mutex_t mutex; 

void touch_update_last_operation_time() 
{
    pthread_mutex_lock(&mutex);
    last_operation_time = time(NULL);
    pthread_mutex_unlock(&mutex);
}
int get_xy(int *x, int *y)
{
    //1.打开触摸屏设备文件  /dev/input/event0
    int fd = open("/dev/input/event0", O_RDWR);
    if(-1 == fd)
    {
        printf("open touch err!\n");
        return 0;
    }
    
    //2.按照特定的格式不停从设备文件中读取数据
    struct input_event ts;
    while(1)
    {
        //没有点击触摸屏时，设备文件中没有数据，read函数会阻塞等待
        read(fd, &ts, sizeof(ts));
        
        //3.分析读取的数据
        if(ts.type == EV_ABS)        //触摸屏
        {
            if(ts.code == ABS_X)    //横坐标
            {
                //printf("x:%d\n", ts.value);
                *x = ts.value;
            }
            
            if(ts.code == ABS_Y)    //纵坐标
            {
                //printf("y:%d\n", ts.value);
                *y = ts.value;
            }
            if(ts.code == ABS_PRESSURE)        //    压力值版本一
            {
                //printf("(1)PRESSURE:%d\n", ts.value);
                if(ts.value == 1)    //按下时跳出
                {
                    update_last_operation_time();
                    break;
                }
            }
        }
        if(ts.type == EV_KEY && ts.code == BTN_TOUCH)    //    压力值版本二
        {
            //printf("(2)PRESSURE:%d\n", ts.value);
            if(ts.value == 1)    //按下时跳出
            {
                update_last_operation_time();
                break;
            }
        }
    }
    
    //4.关闭触摸屏设备文件
    close(fd);
    
    return 0;
}

