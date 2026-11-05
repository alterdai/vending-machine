#ifndef CART_H
#define CART_H
#define ITEM_COUNT 20
typedef struct {
    char name[32];
    int price, stock, count;
} Item;
extern Item items[ITEM_COUNT];
void cart_reset(void);
int cart_change(int id, int delta);
int cart_total(void);
int stock_load(const char *file);
int cart_checkout(const char *file);
#endif
