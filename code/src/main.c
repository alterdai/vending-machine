#include <stdio.h>
#include <pthread.h>
#include "show_bmp.h"
#include "touch.h"
#include "gao.h"
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

pthread_t ad_thread;    //广告的进程
pthread_t image_thread; //主界面图片显示进程
pthread_t text_thread;  //主界面文本显示进程

time_t last_operation_time; 
pthread_mutex_t mutex; 
void update_last_operation_time() //更新操作时间的函数
{
	 pthread_mutex_lock(&mutex);
     last_operation_time = time(NULL);
	 pthread_mutex_unlock(&mutex);
} 
void *show_ad_image_after_timeout(void *arg) 
{
	pthread_mutex_t mutex; 
    int timeout = *(int *)arg;
    int x, y;
    update_last_operation_time(); // 初始化最后一次操作时间

    while (1) 
	{
		pthread_mutex_lock(&mutex);
        // 检查是否超时
        if (time(NULL) - last_operation_time >= timeout) 
		{
			while(1)
			{
              show_bmp_any("gg.bmp", 560, 400, 140, 40); ;// 显示广告图片
              get_xy(&x, &y);
              if (0<x&&x<800&&0<y&&y<480) 
			  {   // 如果触摸了指定位置
                last_operation_time = time(NULL); 
                break; 
			  }
			}
        }
		pthread_mutex_unlock(&mutex);
    }
}
void showbitmap(bitmap *bm, int x, int y,char *p)//控制文本框位图的函数
{ 
     int j,i;
     char *q = p + (y*800 + x)*4;
    for(j=0;j<bm->height && j < 480 ;j++)
    {
     for(i=0;i<bm->width && i< 800 ;i++)   
         memcpy(q+(800*4*j)+i*4,bm->map+(bm->width*bm->byteperpixel*j)+
                                         bm->byteperpixel*i,bm->byteperpixel);
    } 
    
    bzero(bm->map,bm->width*bm->height*bm->byteperpixel);   
}
void *display_text(void *arg) //展示文本的函数
{
	int lcd = open("/dev/fb0",O_RDWR);
    struct fb_var_screeninfo fixinfo; // 固定属性
    ioctl(lcd,FBIOGET_VSCREENINFO,&fixinfo); //// 获取固定属性
    unsigned long VWIDTH  = fixinfo.xres; //可见区宽度（单位：像素）
    unsigned long VHEIGHT = fixinfo.yres; //可见区高度（单位：像素）
    unsigned long BPP = fixinfo.bits_per_pixel;  //色深  
     // 申请一块虚拟区大小的映射内存
    char  *p = mmap(NULL, VWIDTH * VHEIGHT * BPP/8, PROT_WRITE,
                   MAP_SHARED, lcd, 0); 
     //清屏  
     bzero(p,VWIDTH*VHEIGHT*BPP/8);
  
    // 初始化字库 指定字库文件,比如simfang.ttf
    font *f1 = fontLoad("simfang.ttf"); 
    font *f2 = fontLoad("simfang.ttf"); 
    //2.设置字体的大小 
    fontSetSize(f1, 20);
    fontSetSize(f2, 30);
     	
    bitmap *bm1;
    bitmap *bm2;
    bitmap *bm3; 

    bm1  =  createBitmapWithInit(400, 40, 4, 0x00000000); 
    bm2  =  createBitmapWithInit(400, 40, 4, 0x00000000); 
    bm3  =  createBitmapWithInit(100, 90, 4, 0x0000000); 
	int text_index = 0;
	int roll;
while (1) 
  {
	  int x,y;
    time_t t;
    struct tm *Time;
    time(&t);
    char buf[50];
    bzero(buf, 50);
    Time = localtime(&t);
    char *wd[7] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
    snprintf(buf, 50, "%d年%d月%d日 %s %d时%d分%d秒", (1906 + Time->tm_year),
             (8 + Time->tm_mon), (18 + Time->tm_mday),
             wd[Time->tm_wday], (+Time->tm_hour),
             Time->tm_min, Time->tm_sec);
    fontPrint(f1, bm1, 0, 0, buf, 0x00FFFF00, 0);
    showbitmap(bm1, 400, 0, p);
	
	fontPrint(f2, bm3, 0, 0, "衣服", 0x00FFFF00, 0);
    showbitmap(bm3, 0, 50, p);
	fontPrint(f2, bm3, 0, 0, "数码", 0x00FFFF00, 0);
    showbitmap(bm3, 0, 150, p);
	fontPrint(f2, bm3, 0, 0, "家居", 0x00FFFF00, 0);
    showbitmap(bm3, 0, 250, p);
	fontPrint(f2, bm3, 0, 0, "零食", 0x00FFFF00, 0);
    showbitmap(bm3, 0, 350, p);
    fontPrint(f1, bm2, roll, 0, "亲爱的顾客朋友们，清仓大甩卖啦！", 0x00FFFF00, 0);
    showbitmap(bm2, 0, 0, p);
    roll -= 5;
    if (roll <= -200) 
	{
        roll = 400;
    }
    usleep(50000);
   }
    pthread_exit(NULL);
}
void *display_image(void *arg) //展示图像的函数
{
	int current_image = 0;
    int num_images = 3;
    char *images[] = {"jj.bmp", "jj2.bmp", "jj3.bmp"};
    int x, y, prev_x, prev_y;
	while(1)
	{
        int x, y,prev_x, prev_y; 
	    show_bmp_any("bg.bmp", 800, 400, 0, 40);
		show_bmp_any("zhifu.bmp", 100, 100,700,340);
		show_bmp_any("gouwu.bmp", 100, 100,700,40);
		show_bmp_any(images[current_image], 540, 400, 140, 40);
	    
		  get_xy(&x, &y);
		  if (abs(x - prev_x) > 50) //检测x的值变化
	{
        if (x > prev_x) 
		{
            current_image = (current_image - 1 + num_images) % num_images;
        } 
		else 
		{
            current_image = (current_image + 1) % num_images;
        }
    }
	      prev_x = x;
          prev_y = y;
		 
            if (0 < x && x < 100 && 40 < y && y < 140) 
			   {gao1();}
             else if (0 < x && x < 100 && 140 < y && y < 240) 
			   {gao2();}
               else if (0 < x && x < 100 && 240 < y && y < 340) 
			   {gao3();}
                else if (0 < x && x < 100 && 340 < y && y < 440) 
                { gao4();}
              else if (700 < x && x < 800 && 340 < y && y < 440) 
			  {display_payment_options();}
    }
    pthread_exit(NULL);
}
int main(int argc, char const *argv[])
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    int timeout = 30; 
    int i;
    //无操作时播放广告的线程
    pthread_create(&ad_thread, NULL, show_ad_image_after_timeout, &timeout);
    // 创建显示文本的线程
    pthread_create(&text_thread, NULL, display_text, NULL);
    // 创建显示图片的线程
    pthread_create(&image_thread, NULL, display_image, NULL);
    // 等待所有线程结束
    pthread_join(ad_thread, NULL);
    pthread_join(text_thread, NULL);
    pthread_join(image_thread, NULL);
    return 0;
}
