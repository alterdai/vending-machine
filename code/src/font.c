#define STB_TRUETYPE_IMPLEMENTATION
#include "font.h"
#include <stdint.h>
#include <string.h>
#include <limits.h>

bitmap *createBitmap(u32 w,u32 h,u32 bpp) {
    bitmap *b;
    if(!w || !h || bpp!=4 || w>800 || h>480) return NULL;
    b=calloc(1,sizeof *b);
    if(!b) return NULL;
    b->map=calloc((size_t)w*h,bpp);
    if(!b->map) { free(b); return NULL; }
    b->width=w; b->height=h; b->byteperpixel=bpp;
    return b;
}
bitmap *createBitmapWithInit(u32 w,u32 h,u32 bpp,color c) {
    bitmap *b=createBitmap(w,h,bpp);
    size_t i;
    if(b) for(i=0;i<(size_t)w*h;++i) ((u32 *)b->map)[i]=c;
    return b;
}
void destroyBitmap(bitmap *b) { if(b) { free(b->map); free(b); } }
font *fontLoad(char *file) {
    FILE *fp=fopen(file,"rb");
    long len;
    font *f=NULL;
    int offset;
    if(!fp) return NULL;
    if(fseek(fp,0,SEEK_END) || (len=ftell(fp))<12 || len>32*1024*1024 || fseek(fp,0,SEEK_SET)) goto done;
    f=calloc(1,sizeof *f);
    if(!f) goto done;
    f->buffer=malloc((size_t)len); f->info=calloc(1,sizeof *f->info);
    if(!f->buffer || !f->info || fread(f->buffer,1,(size_t)len,fp)!=(size_t)len) goto fail;
    offset=stbtt_GetFontOffsetForIndex(f->buffer,0);
    if(offset<0 || !stbtt_InitFont(f->info,f->buffer,offset)) goto fail;
    f->scale=stbtt_ScaleForPixelHeight(f->info,16);
    goto done;
fail:
    fontUnload(f); f=NULL;
done:
    fclose(fp); return f;
}
void fontUnload(font *f) { if(f) { free(f->info); free(f->buffer); free(f); } }
void fontSetSize(font *f,s32 pixels) { if(f && pixels>0) f->scale=stbtt_ScaleForPixelHeight(f->info,(float)pixels); }
static int next_codepoint(const unsigned char **p) {
    const unsigned char *s=*p;
    unsigned int c=*s++;
    int n=0,i;
    unsigned int min=0;
    if(c<128) { *p=s; return (int)c; }
    if(c>=0xc2 && c<=0xdf) { c&=31; n=1; min=128; }
    else if(c>=0xe0 && c<=0xef) { c&=15; n=2; min=2048; }
    else if(c>=0xf0 && c<=0xf4) { c&=7; n=3; min=65536; }
    else { *p=s; return 0xfffd; }
    for(i=0;i<n;++i) {
        if((*s&0xc0)!=0x80) { *p=s; return 0xfffd; }
        c=(c<<6)|(*s++&63);
    }
    *p=s;
    return c<min || c>0x10ffff || (c>=0xd800 && c<=0xdfff) ? 0xfffd : (int)c;
}
void fontPrint(font *f,bitmap *b,s32 x,s32 y,char *text,color c,s32 maxWidth) {
    const unsigned char *p=(const unsigned char *)text;
    int ascent,descent,gap,step,sx=0,sy=0,cp;
    if(!f || !b || !text) return;
    stbtt_GetFontVMetrics(f->info,&ascent,&descent,&gap);
    step=(int)((ascent-descent+gap)*f->scale);
    if(step<1) step=1;
    while(*p) {
        int w,h,ox,oy,advance,left,ix,iy;
        unsigned char *raster;
        cp=next_codepoint(&p);
        if(cp=='\n') { sx=0; sy+=step; continue; }
        stbtt_GetCodepointHMetrics(f->info,cp,&advance,&left);
        advance=(int)(advance*f->scale);
        if(maxWidth>0 && sx>0 && sx+advance>maxWidth) { sx=0; sy+=step; }
        if(y+sy>=(int)b->height) break;
        raster=stbtt_GetCodepointBitmap(f->info,0,f->scale,cp,&w,&h,&ox,&oy);
        if(raster) for(iy=0;iy<h;++iy) for(ix=0;ix<w;++ix) {
            int dx=x+sx+ox+ix, dy=y+sy+(int)(ascent*f->scale)+oy+iy;
            if(dx>=0 && dy>=0 && dx<(int)b->width && dy<(int)b->height) {
                unsigned int a=raster[iy*w+ix], bg=((u32 *)b->map)[dy*b->width+dx], out=0;
                int shift;
                for(shift=0;shift<24;shift+=8)
                    out|=((((c>>shift)&255)*a+((bg>>shift)&255)*(255-a))/255)<<shift;
                ((u32 *)b->map)[dy*b->width+dx]=out;
            }
        }
        stbtt_FreeBitmap(raster,NULL);
        sx+=advance;
    }
}
