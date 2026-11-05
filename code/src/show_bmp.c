#include "show_bmp.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
static uint32_t le32(const unsigned char *p) {
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
int show_bmp_any(char *file,int w,int h,int rx,int ry) {
    unsigned char header[54], *row=NULL;
    uint32_t *screen=NULL, offset, height;
    int fd=-1,result=-1,y,x,top_down;
    size_t stride;
    FILE *fp;
    if(w<=0 || h<=0 || rx<0 || ry<0 || w>800-rx || h>480-ry) return -1;
    fp=fopen(file,"rb");
    if(!fp) { perror(file); return -1; }
    if(fread(header,1,sizeof header,fp)!=sizeof header || header[0]!='B' || header[1]!='M' ||
       le32(header+14)<40 || le32(header+18)!=(uint32_t)w || header[26]!=1 || header[27]!=0 ||
       header[28]!=24 || header[29]!=0 || le32(header+30)!=0) goto done;
    height=le32(header+22); top_down=(height&0x80000000U)!=0;
    if((top_down ? (~height+1U) : height)!=(uint32_t)h) goto done;
    offset=le32(header+10);
    if(offset<54 || offset>0x7fffffffU || fseek(fp,(long)offset,SEEK_SET)) goto done;
    stride=((size_t)w*3+3)&~(size_t)3;
    row=malloc(stride);
    if(!row) goto done;
    fd=open("/dev/fb0",O_RDWR);
    if(fd<0) goto done;
    screen=mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(screen==MAP_FAILED) { screen=NULL; goto done; }
    for(y=0;y<h;++y) {
        int dy=ry+(top_down ? y : h-1-y);
        if(fread(row,1,stride,fp)!=stride) goto done;
        for(x=0;x<w;++x) screen[dy*800+rx+x]=(uint32_t)row[x*3]|(uint32_t)row[x*3+1]<<8|(uint32_t)row[x*3+2]<<16;
    }
    result=0;
done:
    if(screen) munmap(screen,800*480*4);
    if(fd>=0) close(fd);
    free(row); fclose(fp); return result;
}
int show_bmp(char *file) { return show_bmp_any(file,800,480,0,0); }
