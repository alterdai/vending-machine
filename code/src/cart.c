#include "cart.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

Item items[ITEM_COUNT] = {
    {"地狱火战衣",200,10,0}, {"凤求凰战衣",300,10,0},
    {"至尊宝战衣",400,10,0}, {"电玩小子战衣",400,10,0},
    {"GG鼠标",20,10,0}, {"4090猛禽显卡",12000,10,0},
    {"三星爆炸手机",8000,10,0}, {"烂苹果14pro",10000,10,0},
    {"真皮沙发",5000,10,0}, {"精美茶几",4000,10,0},
    {"象皮床",8000,10,0}, {"小盆摘",50,10,0},
    {"虎皮凤爪",50,100,0}, {"梅干菜扣肉",30,100,0},
    {"劲爽鱿鱼",5,100,0}, {"一根葱",1,100,0},
    {"衣服1",200,10,0}, {"衣服2",200,10,0},
    {"衣服3",200,10,0}, {"衣服4",200,10,0}
};

void cart_reset(void) {
    int i;
    for (i=0; i<ITEM_COUNT; ++i) items[i].count=0;
}
int cart_change(int id, int delta) {
    if (id<0 || id>=ITEM_COUNT || (delta!=1 && delta!=-1)) return -1;
    if ((delta>0 && items[id].count>=items[id].stock) ||
        (delta<0 && items[id].count==0)) return -1;
    items[id].count+=delta;
    return 0;
}
int cart_total(void) {
    int i, total=0;
    for (i=0; i<ITEM_COUNT; ++i) total+=items[i].price*items[i].count;
    return total;
}
/* Load into a temporary snapshot; a malformed file never partially changes stock.
 * Keep the original name/stock format for existing installations. */
int stock_load(const char *file) {
    FILE *fp=fopen(file,"r");
    int stock[ITEM_COUNT], seen[ITEM_COUNT]={0}, rows=0, i, value;
    char line[128], name[32], extra;
    if (!fp) return errno==ENOENT ? 0 : -1;
    while (fgets(line,sizeof line,fp)) {
        if (sscanf(line," %31s %d %c",name,&value,&extra)!=2 || value<0 || value>10000) goto fail;
        for (i=0; i<ITEM_COUNT && strcmp(name,items[i].name); ++i) {}
        if (i==ITEM_COUNT || seen[i]) goto fail;
        stock[i]=value; seen[i]=1; ++rows;
    }
    if (ferror(fp) || (rows!=0 && rows!=ITEM_COUNT)) goto fail;
    if (fclose(fp)!=0) return -1;
    if (rows) for(i=0;i<ITEM_COUNT;++i) items[i].stock=stock[i];
    return 0;
fail:
    fclose(fp);
    return -1;
}
/* Persist the candidate before changing memory. Rename is atomic on the target
 * Linux filesystem; this does not promise durability across sudden power loss. */
int cart_checkout(const char *file) {
    char tmp[512];
    int i, failed=0;
    FILE *fp;
    if (cart_total()==0 || snprintf(tmp,sizeof tmp,"%s.tmp",file)>=(int)sizeof tmp) return -1;
    fp=fopen(tmp,"w");
    if (!fp) return -1;
    for(i=0;i<ITEM_COUNT;++i) {
        if (items[i].count<0 || items[i].count>items[i].stock ||
            fprintf(fp,"%s %d\n",items[i].name,items[i].stock-items[i].count)<0) failed=1;
    }
    if (fclose(fp)!=0) failed=1;
    if (failed || rename(tmp,file)!=0) { remove(tmp); return -1; }
    for(i=0;i<ITEM_COUNT;++i) items[i].stock-=items[i].count;
    cart_reset();
    return 0;
}
