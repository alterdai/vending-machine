#include "show_bmp.h"

int show_bmp(char *pathname)
{
	//打开图片文件
	int bmpfd = open(pathname, O_RDWR);
	if(-1 == bmpfd)
	{
		printf("open bmp err!\n");
		return -1;
	}
	
	//打开显示屏的设备文件  open
	int lcdfd = open("/dev/fb0", O_RDWR);
	if(-1 == lcdfd)
	{
		printf("open lcd err!\n");
		return -1;
	}
	
	//4.跳过图片文件前54字节的文件信息头
	lseek(bmpfd, 54, SEEK_SET);
	
	//5.读取800*480*3 （1152000）字节的颜色数据
	char BGR[800*480*3];
	read(bmpfd, BGR, 800*480*3);
	
	//6.需要将BGR数据转换为屏幕要求的ARGB格式
	int ARGB[800*480];
	int i;
	for(i=0; i<800*480; i++)
		ARGB[i] = BGR[3*i] | BGR[3*i+1]<<8 | BGR[3*i+2]<<16;
	//				B         G            R
	
	//7.由于BMP图片是由下往上存，所有需要进行按行翻转
	int data[800*480];
	int x, y;
	for(y=0; y<480; y++)
		for(x=0; x<800; x++)
			data[(479-y)*800+x] = ARGB[y*800+x];
		
	//8.将处理好的数据写入显示屏设备文件
	//write(lcdfd, data, 800*480*4);
	
	//内存映射，提高数据写入效率
	int *p = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, 
				lcdfd, 0);
				
	for(y=0; y<480; y++)
		for(x=0; x<800; x++)
			*(p+y*800+x) = data[y*800+x];
		
	munmap(p, 800*480*4);
	
	//9.关闭显示屏设备文件和图片文件
	close(lcdfd);
	close(bmpfd);
	
}

//显示非全屏图片
int show_bmp_any(char *pathname,int w, int h, int rx, int ry)
{
	if((w+rx>800) || (h+ry>480) || (w*3%4!=0))
	{
		printf("bmp err!\n");
		return -1;
	}
	
	//打开图片文件
	int bmpfd = open(pathname, O_RDWR);
	if(-1 == bmpfd)
	{
		printf("open bmp err!\n");
		return -1;
	}
	
	//打开显示屏的设备文件  open
	int lcdfd = open("/dev/fb0", O_RDWR);
	if(-1 == lcdfd)
	{
		printf("open lcd err!\n");
		return -1;
	}
	
	//4.跳过图片文件前54字节的文件信息头
	lseek(bmpfd, 54, SEEK_SET);
	
	//5.读取w*h*3 字节的颜色数据
	char BGR[w*h*3];
	read(bmpfd, BGR, w*h*3);
	
	//6.需要将BGR数据转换为屏幕要求的ARGB格式
	int ARGB[w*h];
	int i;
	for(i=0; i<w*h; i++)
		ARGB[i] = BGR[3*i] | BGR[3*i+1]<<8 | BGR[3*i+2]<<16;
	//				B             G              R
	
	//7.由于BMP图片是由下往上存，所有需要进行按行翻转
	int data[w*h];
	int x, y;
	for(y=0; y<h; y++)
		for(x=0; x<w; x++)
			data[(h-1-y)*w+x] = ARGB[y*w+x];
		
	//8.将处理好的数据写入显示屏设备文件
	//write(lcdfd, data, 800*480*4);
	
	//内存映射，提高数据写入效率
	int *p = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, 
				lcdfd, 0);
				
	for(y=0; y<h; y++)
		for(x=0; x<w; x++)
			*(p+(ry+y)*800+x+rx) = data[y*w+x];
		//	*(p + ry*800 + rx)	= data[0];
			
	munmap(p, 800*480*4);
	
	//9.关闭显示屏设备文件和图片文件
	close(lcdfd);
	close(bmpfd);
	
}
