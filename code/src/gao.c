#define _POSIX_C_SOURCE 200809L
#include "gao.h"
#include "cart.h"
#include "font.h"
#include "show_bmp.h"
#include "touch.h"
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>

static int lcd=-1;
static unsigned char *screen;
static font *face;
static volatile sig_atomic_t running=1;
static void stop(int sig) { (void)sig; running=0; }
static long seconds_now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC,&t);
    return t.tv_sec;
}
static void text(int x,int y,int w,int h,const char *s) {
    bitmap *b=createBitmapWithInit(w,h,4,0x00202020);
    int row;
    if(!b) return;
    fontPrint(face,b,0,0,(char *)s,0x00ffffff,w);
    for(row=0;row<h && y+row<480;++row)
        memcpy(screen+((y+row)*800+x)*4,b->map+row*w*4,w*4);
    destroyBitmap(b);
}
static void picture(const char *file,int w,int h,int x,int y) {
    show_bmp_any((char *)file,w,h,x,y);
}
static void clock_draw(void) {
    char buf[64];
    time_t now=time(NULL);
    struct tm *t=localtime(&now);
    if(t && strftime(buf,sizeof buf,"%Y-%m-%d %H:%M:%S",t)) text(0,0,800,40,buf);
}
static void draw(int category,int page,int paying,const char *message) {
    const char *names[]={"衣服","数码","家居","零食"};
    char buf[256], image[24];
    int i,id,x,y,count=0;
    memset(screen,0,800*480*4);
    clock_draw();
    for(i=0;i<4;++i) text(0,50+i*100,100,60,names[i]);
    if(paying) {
        picture("zhifubao.bmp",280,400,140,40);
        picture("weixin.bmp",280,400,420,40);
        text(100,40,40,50,"返回");
        text(100,100,40,50,"成功");
        text(100,160,40,50,"失败");
    } else {
        text(100,40,40,50,"翻页");
        for(i=0;i<4;++i) {
            id=category==0 && page==1 ? 16+i : category*4+i;
            x=145+(i%2)*270; y=45+(i/2)*200;
            snprintf(image,sizeof image,"%d.bmp",id+1);
            picture(image,160,160,x,y);
            snprintf(buf,sizeof buf,"%s\n已选:%d\n库存:%d\n价格:%d元",items[id].name,items[id].count,items[id].stock,items[id].price);
            text(x+160,y+40,90,90,buf);
            picture("jia.bmp",40,40,x+160,y+135);
            picture("jian.bmp",40,40,x+210,y+135);
        }
    }
    text(700,40,100,40,"购物车");
    y=90;
    for(i=0;i<ITEM_COUNT;++i) if(items[i].count) {
        ++count;
        if(y<290) {
            snprintf(buf,sizeof buf,"%s x%d",items[i].name,items[i].count);
            text(700,y,100,40,buf); y+=40;
        }
    }
    if(count>5) text(700,290,100,40,"更多商品已计入");
    text(700,340,100,80,"支付\n(模拟)");
    snprintf(buf,sizeof buf,"合计:%d元  %s",cart_total(),message);
    text(0,440,800,40,buf);
}
int shop_run(void) {
    struct fb_var_screeninfo v;
    struct fb_fix_screeninfo f;
    int x=0,y=0,ret,result=1,category=0,page=0,paying=0,advert=0,i,id,dirty=1;
    long last;
    const char *message="支付为演示，不会实际扣款";
    if(stock_load("stock.txt")<0) { fprintf(stderr,"Invalid stock.txt; refusing to overwrite it\n"); return 1; }
    lcd=open("/dev/fb0",O_RDWR);
    if(lcd<0 || ioctl(lcd,FBIOGET_VSCREENINFO,&v)<0 || ioctl(lcd,FBIOGET_FSCREENINFO,&f)<0) goto done;
    if(v.xres!=800 || v.yres!=480 || v.bits_per_pixel!=32 || f.line_length!=3200 || v.xoffset || v.yoffset ||
       v.red.offset!=16 || v.green.offset!=8 || v.blue.offset!=0) {
        fprintf(stderr,"Requires 800x480 packed BGRX32 framebuffer\n"); goto done;
    }
    screen=mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd,0);
    if(screen==MAP_FAILED) { screen=NULL; goto done; }
    face=fontLoad("simfang.ttf");
    if(!face) { fprintf(stderr,"Cannot load simfang.ttf\n"); goto done; }
    fontSetSize(face,16);
    if(touch_open()<0) goto done;
    signal(SIGINT,stop); signal(SIGTERM,stop);
    last=seconds_now();
    while(running) {
        if(seconds_now()-last>=30) {
            if(!advert) {
                advert=1; paying=0; cart_reset();
                memset(screen,0,800*480*4);
                picture("gg.bmp",560,400,140,40);
            }
        } else if(dirty) { draw(category,page,paying,message); dirty=0; }
        else clock_draw();
        ret=get_xy(&x,&y);
        if(ret<0) { perror("touch"); goto done; }
        if(ret>0) continue;
        dirty=1;
        last=seconds_now();
        if(advert) { advert=0; continue; }
        if(paying) {
            if(x>=100 && x<140) {
                if(y>=40 && y<90) { paying=0; cart_reset(); message="已取消，库存不变"; }
                else if(y>=100 && y<150) {
                    if(cart_checkout("stock.txt")==0) { paying=0; message="模拟支付成功，请取货"; }
                    else message="空购物车或库存保存失败，未完成交易";
                } else if(y>=160 && y<210) message="模拟支付失败，可重试或返回取消";
            }
            continue;
        }
        if(x>=0 && x<100 && y>=40 && y<440) { category=(y-40)/100; page=0; }
        else if(x>=100 && x<140 && y>=40 && y<90 && category==0) page=1-page;
        else if(x>=700 && x<800 && y>=340 && y<440) {
            if(cart_total()) paying=1;
            else message="请先选择商品";
        } else for(i=0;i<4;++i) {
            int bx=305+(i%2)*270, by=180+(i/2)*200;
            id=category==0 && page==1 ? 16+i : category*4+i;
            if(y>=by && y<by+40) {
                if(x>=bx && x<bx+40) cart_change(id,1);
                else if(x>=bx+50 && x<bx+90) cart_change(id,-1);
            }
        }
    }
    result=0;
done:
    touch_close();
    if(face) fontUnload(face);
    if(screen) munmap(screen,800*480*4);
    if(lcd>=0) close(lcd);
    return result;
}
