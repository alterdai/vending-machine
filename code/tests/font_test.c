#include "font.h"
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv) {
    font *f;
    bitmap *b;
    assert(fontLoad("missing-test-font.ttf")==NULL);
    assert(createBitmap(0,10,4)==NULL);
    assert(createBitmap(801,10,4)==NULL);
    assert(argc==2);
    f=fontLoad(argv[1]); assert(f);
    b=createBitmapWithInit(100,40,4,0); assert(b);
    fontSetSize(f,16);
    fontPrint(f,b,-20,-10,"中文 text\nline 2",0xffffff,80);
    fontPrint(f,b,99,39,"edge",0xffffff,0);
    fontPrint(f,b,0,0,"\xf0\x9f\x98\x80\xc2\x41\xe0\x80\x80",0xffffff,100);
    fontPrint(f,b,0,0,"\xf0",0xffffff,100);
    destroyBitmap(b); fontUnload(f);
    puts("font tests passed");
    return 0;
}
