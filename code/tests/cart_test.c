#include "cart.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    Item original[ITEM_COUNT];
    FILE *fp;
    int i;
    const char *file="cart-test-stock.txt";
    memcpy(original,items,sizeof items);
    assert(cart_change(-1,1)==-1);
    assert(cart_change(0,-1)==-1);
    assert(cart_change(0,2)==-1);
    for(i=0;i<10;++i) assert(cart_change(0,1)==0);
    assert(cart_change(0,1)==-1);
    assert(items[0].stock==10 && cart_total()==2000);
    cart_reset();
    assert(items[0].stock==10 && cart_total()==0);
    assert(cart_checkout(file)==-1);
    assert(cart_change(0,1)==0);
    assert(cart_change(0,-1)==0);
    assert(cart_change(0,1)==0);
    assert(items[0].count==1 && cart_total()==200);
    /* Persistence failure must preserve both stock and the pending cart. */
    assert(cart_checkout("missing-test-directory/stock.txt")==-1);
    assert(items[0].stock==10 && items[0].count==1);
    assert(cart_checkout(file)==0);
    assert(items[0].stock==9 && items[0].count==0);
    memcpy(items,original,sizeof items);
    assert(stock_load(file)==0 && items[0].stock==9);
    /* POSIX replacement of an existing file; Windows rename cannot replace it. */
#ifndef _WIN32
    cart_change(0,1);
    assert(cart_checkout(file)==0 && items[0].stock==8);
#endif
    fp=fopen(file,"w"); assert(fp);
    fprintf(fp,"%s 1\ninvalid\n",items[0].name); fclose(fp);
    i=items[0].stock;
    assert(stock_load(file)==-1 && items[0].stock==i);
    fp=fopen(file,"w"); assert(fp);
    fprintf(fp,"%s -1\n",items[0].name); fclose(fp);
    assert(stock_load(file)==-1 && items[0].stock==i);
    fp=fopen(file,"w"); assert(fp); fclose(fp);
    assert(stock_load(file)==0 && items[0].stock==i);
    remove(file);
    puts("cart tests passed");
    return 0;
}
