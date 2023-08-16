#include"gao.h"
#define MAX_ITEMS 20

typedef struct 
{
    char name[20]; //商品名
    int price; //价格
    int stock; //库存
    int count; //已选份数
} item;
typedef struct node //链表的结构体
{
    char name[20];
    struct node *next;
} node;
node *selected_items = NULL; // 链表头指针

item items[] = 
{
    {"地狱火战衣", 200, 10, 0},//衣服界面1
    {"凤求凰战衣", 300, 10, 0},//衣服界面1
    {"至尊宝战衣", 400, 10, 0},//衣服界面1
    {"电玩小子战衣", 400, 10, 0},//衣服界面1
    {"GG鼠标", 20, 10, 0},
    {"4090猛禽显卡", 12000, 10, 0},
    {"三星爆炸手机", 8000, 10, 0},
    {"烂苹果14pro", 10000, 10, 0},
    {"真皮沙发", 5000, 10, 0},
    {"精美茶几", 4000, 10, 0},
    {"象皮床", 8000, 10, 0},
    {"小盆摘", 50, 10, 0},
    {"虎皮凤爪", 50, 100, 0},
    {"梅干菜扣肉", 30, 100, 0},
    {"劲爽鱿鱼", 5, 100, 0},
    {"一根葱", 1, 100, 0},
	{"衣服1", 200, 10, 0},//衣服界面2
	{"衣服2", 200, 10, 0},//衣服界面2
	{"衣服3", 200, 10, 0},//衣服界面2
	{"衣服4", 200, 10, 0},//衣服界面2
};
void add_selected_item(char *name) //链表指针函数
{
    // 创建新节点
    node *new_node = malloc(sizeof(node));
    strcpy(new_node->name, name);
    new_node->next = NULL;

    // 将新节点添加到链表末尾
    if(selected_items == NULL) 
	{
        selected_items = new_node;
    } else 
	{
        node *current = selected_items;
        while(current->next != NULL) 
		{
            current = current->next;
        }
        current->next = new_node;
    }
}
char buf[1024];
int total_count = 0;//已选商品的数量
int total_price = 0;//已选商品的总价
void update_cart(char *name, int delta) //实现购物车功能的函数
{
    int lcd = open("/dev/fb0",O_RDWR);
    struct fb_var_screeninfo fixinfo; 
    ioctl(lcd,FBIOGET_VSCREENINFO,&fixinfo); 
    unsigned long VWIDTH  = fixinfo.xres; 
    unsigned long VHEIGHT = fixinfo.yres; 
    unsigned long BPP = fixinfo.bits_per_pixel;
    char  *p = mmap(NULL, VWIDTH * VHEIGHT * BPP/8, PROT_WRITE,MAP_SHARED, lcd, 0);  
    font *f1 = fontLoad("simfang.ttf"); 
    fontSetSize(f1, 13);
    bitmap *bm1;
    bitmap *bm2;
    bm1  =  createBitmapWithInit(100, 180, 4, 0x0000000); //购物车
    bm2  =  createBitmapWithInit(800, 40, 4, 0x0000000);  //数量和总价
    char buf1[100];
    char buf2[100];
     int i;
    for(i = 0; i < MAX_ITEMS; i++) 
    {
        if(strcmp(items[i].name, name) == 0 && items[i].count + delta >= 0 && items[i].stock - delta >= 0) 
        {
            // 找到匹配商品,更新数量
            items[i].count += delta; 
            items[i].stock -= delta;
            total_count += delta;                     // 更新已选商品的数量
            total_price += delta * items[i].price;    // 更新商品总价

            // 如果是新增加的商品，将其添加到链表中
            if(delta > 0 && items[i].count == delta) 
			{
                add_selected_item(name);
            }
        }
    }
    // 使用链表来确定购物车中商品的显示顺序
    int y_offset = 0;
    node *current = selected_items;
    while(current != NULL) 
	{
        for(i = 0; i < MAX_ITEMS; i++) 
		{
            if(strcmp(items[i].name, current->name) == 0 && items[i].count > 0) 
			{
                snprintf(buf1, 100, "%s*%d\n", items[i].name, items[i].count);
                fontPrint(f1,bm1,0,y_offset,buf1,0xffffffff,0);
                y_offset += 20;
            }
        }
        current = current->next;
    }
    showbitmap(bm1, 700, 160, p);
    snprintf(buf2, 100, "已选商品数量：%d\n总价：%d元\n", total_count, total_price);
    fontPrint(f1,bm2,0,0,buf2,0xffffffff,0);
    showbitmap(bm2, 0, 440, p);
}
void reset_cart() //重置购物车内容的函数
{
	int i;
    total_count = 0;
    total_price = 0;
    for(i = 0; i < MAX_ITEMS; i++) 
	{
        if(items[i].count > 0) 
		{
            items[i].count = 0;
        }
    }
}
void display_payment_options(void) //实现支付功能的函数
{
    int x, y;
	while(1)
	{
	     if(700<x&&x<800&&340<y&&y<440)
         {
           show_bmp_any("zhifubao.bmp", 280, 400, 140, 40);
           show_bmp_any("weixin.bmp", 280, 400, 420, 40);
           show_bmp_any("back.bmp", 40, 40, 100, 40);
		   show_bmp_any("cheng.bmp", 40, 40, 100, 100);
		   show_bmp_any("shi.bmp", 40, 40, 100, 160);
         }
         get_xy(&x, &y);
		 if(100<x&&x<140&&40<y&&y<80)
         {
			reset_cart();
			break;
		 }
         else if(100<x&&x<140&&100<y&&y<140)
         {
		   int i;
		   for(i = 0; i < MAX_ITEMS; i++)
           {
              if(items[i].count > 0) // 如果商品已被选中
              {
                 reset_cart(); //清空购物车
              }
         }
		 show_bmp_any("cg.bmp", 560, 400, 140, 40);
		 sleep(1);
		 show_bmp_any("zhifubao.bmp", 280, 400, 140, 40);
         show_bmp_any("weixin.bmp", 280, 400, 420, 40);
         show_bmp_any("back.bmp", 40, 40, 100, 40);
		 show_bmp_any("cheng.bmp", 40, 40, 100, 100);
		 show_bmp_any("shi.bmp", 40, 40, 100, 160);
		 }
         else if(100<x&&x<140&&160<y&&y<200)
         {
		   show_bmp_any("sb.bmp", 560, 400, 140, 40);
		   sleep(1);
		   show_bmp_any("zhifubao.bmp", 280, 400, 140, 40);
           show_bmp_any("weixin.bmp", 280, 400, 420, 40);
           show_bmp_any("back.bmp", 40, 40, 100, 40);
		   show_bmp_any("cheng.bmp", 40, 40, 100, 100);
		   show_bmp_any("shi.bmp", 40, 40, 100, 160);
		 }	  		 		 
	}
	int i;
	FILE *fp = fopen("stock.txt", "w");
    if(fp == NULL)
    {
    printf("打开文件错误！\n");
    exit(1);
    }
    total_count = 0;
    total_price = 0;
    for(i = 0; i < MAX_ITEMS; i++)
    {
		 if(items[i].count > 0)
      {
        // 设置 count 为 0
        items[i].count = 0;
      }
    fprintf(fp, "%s %d\n", items[i].name, items[i].stock);
    }

    fclose(fp);
}
void gao1() //管理商品”衣服“的函数
{
	int page=0;//第一页商品
	int lcd = open("/dev/fb0",O_RDWR);
    struct fb_var_screeninfo fixinfo; // 固定属性
    ioctl(lcd,FBIOGET_VSCREENINFO,&fixinfo); //// 获取固定属性
    unsigned long VWIDTH  = fixinfo.xres; //可见区宽度（单位：像素）
    unsigned long VHEIGHT = fixinfo.yres; //可见区高度（单位：像素）
    unsigned long BPP = fixinfo.bits_per_pixel;  //色深  
    char  *p = mmap(NULL, VWIDTH * VHEIGHT * BPP/8, PROT_WRITE,MAP_SHARED, lcd, 0);  
    bzero(p,VWIDTH*VHEIGHT*BPP/8);
  
    font *f1 = fontLoad("simfang.ttf"); 
	font *f2 = fontLoad("simfang.ttf"); 
    fontSetSize(f1, 20);
    fontSetSize(f2, 12);
    bitmap *bm1;
    bitmap *bm2;
    bitmap *bm3;
    bitmap *bm4; 
    bm1  =  createBitmapWithInit(400, 40, 4, 0xFFFFFFFF); 
    bm2  =  createBitmapWithInit(400, 40, 4, 0xFFFFFFFF); 
    bm3  =  createBitmapWithInit(75, 60,  4, 0xFFFFFFF); 
	int x,y;
	while(1)
	{
		if(page == 1) 
      {
       show_bmp_any("17.bmp", 160, 160, 145, 45);
       show_bmp_any("18.bmp", 160, 160, 415, 45);
       show_bmp_any("19.bmp", 160, 160, 145, 245);
       show_bmp_any("20.bmp", 160, 160, 415, 245);
	   
	        snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[16].name, items[16].count, items[16].stock,items[16].price);
            fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
            showbitmap(bm3, 320, 100, p);
	        snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[17].name, items[17].count, items[17].stock,items[17].price);
		    fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
            showbitmap(bm3, 580, 100, p);
            snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[18].name, items[18].count, items[18].stock,items[18].price);
		    fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
            showbitmap(bm3, 320, 310, p);
            snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[19].name, items[19].count, items[19].stock,items[19].price);
		    fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
            showbitmap(bm3, 580, 310, p);
      } 
        else if(page == 0) 
      {
       show_bmp_any("1.bmp", 160, 160, 145, 45);
       show_bmp_any("2.bmp", 160, 160, 415, 45);
       show_bmp_any("3.bmp", 160, 160, 145, 245);
       show_bmp_any("4.bmp", 160, 160, 415, 245);
	        snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[0].name, items[0].count, items[0].stock,items[0].price);
            fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
            showbitmap(bm3, 320, 100, p);
	        snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[1].name, items[1].count, items[1].stock,items[1].price);
		    fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
            showbitmap(bm3, 580, 100, p);
            snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[2].name, items[2].count, items[2].stock,items[2].price);
		    fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
            showbitmap(bm3, 320, 310, p);
            snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[3].name, items[3].count, items[3].stock,items[3].price);
		    fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
            showbitmap(bm3, 580, 310, p);
      }
		show_bmp_any("zhifu.bmp", 100, 100,700,340);
		show_bmp_any("gouwu.bmp", 100, 100,700,40);
		show_bmp_any("jia.bmp", 40, 40,305,180);   //1
		show_bmp_any("jian.bmp", 40, 40,355,180); //1
		show_bmp_any("jia.bmp", 40, 40,575,180);   //2
		show_bmp_any("jian.bmp", 40, 40,625,180); //2
		show_bmp_any("jia.bmp", 40, 40,305,375);   //3
		show_bmp_any("jian.bmp", 40, 40,355,375); //3
		show_bmp_any("jia.bmp", 40, 40,575,375);   //4
		show_bmp_any("jian.bmp", 40, 40,625,375); //4
		show_bmp_any("fanye.bmp", 40, 40,100,40);
		show_bmp_any("back.bmp", 40, 40, 100, 100);
		
		get_xy(&x, &y);
		if(100<x&&x<140&&40<y&&y<80)//下一页
        {
			page++;
			
		}
		if(100 < x && x < 140 && 100 < y && y < 140)//返回上一页
        {
			if(page>0)
			{
            page--;
			}
        }
		if(305 < x && x < 345 && 180 < y && y < 220)
		{
			if(page==0)
			{update_cart("地狱火战衣", 1);}
		if(page==1)
		{update_cart("衣服1", 1);}
		}
		if(355 < x && x < 395 && 180 < y && y < 220)
		{
			if(page==0)
			{update_cart("地狱火战衣", -1);}
		    if(page==1)
			{update_cart("衣服1", -1);}
		}
		if(575 < x && x < 615 && 180 < y && y < 220)
		{
			if(page==0)
			{update_cart("凤求凰战衣", 1);}
		    if(page==1)
		    {update_cart("衣服2", 1);}
		}
		if(625 < x && x < 665 && 180 < y && y < 220)
		{
			if(page==0)
			{update_cart("凤求凰战衣", -1);}
		if(page==1)
		{update_cart("衣服2", -1);}
		}
		if(305 < x && x < 345 && 375 < y && y < 415)
		{
			if(page==0)
			{update_cart("至尊宝战衣", 1);}
		    if(page==1)
			{update_cart("衣服3", 1);}
		}
		if(355 < x && x < 395 && 375 < y && y < 415)
		{
			if(page==0)
			{update_cart("至尊宝战衣", -1);}
		    if(page==1)
			{update_cart("衣服3", -1);}
		}
		if(575 < x && x < 615 && 375 < y && y < 415)
		{
			if(page==0)
			{update_cart("电玩小子战衣", 1);}
		    if(page==1)
			{update_cart("衣服4", 1);}
		}
		if(625 < x && x < 665 && 375 < y && y < 415)
		{
			if(page==0)
			{update_cart("电玩小子战衣", -1);}
		    if(page==1)
			{update_cart("衣服4", -1);}
		}
		else if((700 < x && x < 800 && 340 < y && y < 440)||(0 < x && x < 100 && 140 < y && y < 240)||(0 < x && x < 100 && 240 < y && y < 340)||(0 < x && x < 100 && 340 < y && y < 440))
		{
			break;
		}
	}
	int i;
	FILE *fp = fopen("stock.txt", "w");
    if(fp == NULL)
    {
    printf("打开文件错误！\n");
    exit(1);
    }
    for(i = 0; i < MAX_ITEMS; i++)
    {
      fprintf(fp, "%s %d\n", items[i].name, items[i].stock);
    }
    fclose(fp);
}
void gao2()//管理商品”数码“的函数
{
	int lcd = open("/dev/fb0",O_RDWR);
    struct fb_var_screeninfo fixinfo; // 固定属性
    ioctl(lcd,FBIOGET_VSCREENINFO,&fixinfo); //// 获取固定属性
    unsigned long VWIDTH  = fixinfo.xres; //可见区宽度（单位：像素）
    unsigned long VHEIGHT = fixinfo.yres; //可见区高度（单位：像素）
    unsigned long BPP = fixinfo.bits_per_pixel;  //色深  
    char  *p = mmap(NULL, VWIDTH * VHEIGHT * BPP/8, PROT_WRITE,MAP_SHARED, lcd, 0); 
     bzero(p,VWIDTH*VHEIGHT*BPP/8);
    font *f1 = fontLoad("simfang.ttf"); 
    font *f2 = fontLoad("simfang.ttf"); 
    fontSetSize(f1, 20);
    fontSetSize(f2, 12);
    bitmap *bm1;
    bitmap *bm2;
    bitmap *bm3;
    bitmap *bm4;    
    bm1  =  createBitmapWithInit(400, 40, 4, 0xFFFFFFFF); 
    bm2  =  createBitmapWithInit(400, 40, 4, 0xFFFFFFFF); 
    bm3  =  createBitmapWithInit(75, 60,  4, 0x0000000); 
	int x,y;
	while(1)
	{
		show_bmp_any("5.bmp", 160, 160, 145, 45);
		show_bmp_any("6.bmp", 160, 160, 415, 45);
		show_bmp_any("7.bmp", 160, 160, 145, 245);
		show_bmp_any("8.bmp", 160, 160, 415, 245);
		show_bmp_any("zhifu.bmp", 100, 100,700,340);
		show_bmp_any("gouwu.bmp", 100, 100,700,40);
		show_bmp_any("jia.bmp", 40, 40,305,180);   //1
		show_bmp_any("jian.bmp", 40, 40,355,180); //1
		show_bmp_any("jia.bmp", 40, 40,575,180);   //2
		show_bmp_any("jian.bmp", 40, 40,625,180); //2
		show_bmp_any("jia.bmp", 40, 40,305,375);   //3
		show_bmp_any("jian.bmp", 40, 40,355,375); //3
		show_bmp_any("jia.bmp", 40, 40,575,375);   //4
		show_bmp_any("jian.bmp", 40, 40,625,375); //4
		
		snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[4].name, items[4].count, items[4].stock,items[4].price);
        fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 320, 100, p);
         snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[5].name, items[5].count, items[5].stock,items[5].price);
		fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 580, 100, p);
         snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[6].name, items[6].count, items[6].stock,items[6].price);
		fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 320, 310, p);
         snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[7].name, items[7].count, items[7].stock,items[7].price);
		fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 580, 310, p);
		get_xy(&x, &y);
		if(305 < x && x < 345 && 180 < y && y < 220)
		{
			 update_cart("GG鼠标", 1);
		}
		if(355 < x && x < 395 && 180 < y && y < 220)
		{
			 update_cart("GG鼠标", -1);
		}
		if(575 < x && x < 615 && 180 < y && y < 220)
		{
			update_cart("4090猛禽显卡",1);
		}
		if(625 < x && x < 665 && 180 < y && y < 220)
		{
			update_cart("4090猛禽显卡", -1);
		}
		if(305 < x && x < 345 && 375 < y && y < 415)
		{
			update_cart("三星爆炸手机", 1);
		}
		if(355 < x && x < 395 && 375 < y && y < 415)
		{
			 update_cart("三星爆炸手机", -1);
		}
		if(575 < x && x < 615 && 375 < y && y < 415)
		{
			 update_cart("烂苹果14pro", 1);
		}
		if(625 < x && x < 665 && 375 < y && y < 415)
		{
			 update_cart("烂苹果14pro", -1);
		}
		if((700 < x && x < 800 && 340 < y && y < 440)||(0 < x && x < 100 && 40 < y && y < 140)||(0 < x && x < 100 && 240 < y && y < 340)||(0 < x && x < 100 && 340 < y && y < 440))
		{break;}
	}
	int i;
	FILE *fp = fopen("stock.txt", "w");
    if(fp == NULL)
    {
    printf("打开文件错误！\n");
    exit(1);
    }
    for(i = 0; i < MAX_ITEMS; i++)
    {
      fprintf(fp, "%s %d\n", items[i].name, items[i].stock);
    }

    fclose(fp);
}
void gao3()//管理商品”家居“的函数
{
	int lcd = open("/dev/fb0",O_RDWR);
    struct fb_var_screeninfo fixinfo; // 固定属性
    ioctl(lcd,FBIOGET_VSCREENINFO,&fixinfo); //// 获取固定属性
    unsigned long VWIDTH  = fixinfo.xres; //可见区宽度（单位：像素）
    unsigned long VHEIGHT = fixinfo.yres; //可见区高度（单位：像素）
    unsigned long BPP = fixinfo.bits_per_pixel;  //色深  
     // 申请一块虚拟区大小的映射内存
    char  *p = mmap(NULL, VWIDTH * VHEIGHT * BPP/8, PROT_WRITE,MAP_SHARED, lcd, 0); 
     bzero(p,VWIDTH*VHEIGHT*BPP/8);
    font *f1 = fontLoad("simfang.ttf"); 
    font *f2 = fontLoad("simfang.ttf"); 
    fontSetSize(f1, 20);
	fontSetSize(f2, 12);
    bitmap *bm1;
    bitmap *bm2;
    bitmap *bm3;
    bitmap *bm4; 
    bm1  =  createBitmapWithInit(400, 40, 4, 0xFFFFFFFF); 
    bm2  =  createBitmapWithInit(400, 40, 4, 0xFFFFFFFF);  
    bm3  =  createBitmapWithInit(75, 60,  4, 0x0000000); 
	
		int x,y;
	while(1)
	{
		show_bmp_any("9.bmp", 160, 160, 145, 45);
		show_bmp_any("10.bmp", 160, 160, 415, 45);
		show_bmp_any("11.bmp", 160, 160, 145, 245);
		show_bmp_any("12.bmp", 160, 160, 415, 245);
		show_bmp_any("zhifu.bmp", 100, 100,700,340);
		show_bmp_any("gouwu.bmp", 100, 100,700,40);
		show_bmp_any("jia.bmp", 40, 40,305,180);   //1
		show_bmp_any("jian.bmp", 40, 40,355,180); //1
		show_bmp_any("jia.bmp", 40, 40,575,180);   //2
		show_bmp_any("jian.bmp", 40, 40,625,180); //2
		show_bmp_any("jia.bmp", 40, 40,305,375);   //3
		show_bmp_any("jian.bmp", 40, 40,355,375); //3
		show_bmp_any("jia.bmp", 40, 40,575,375);   //4
		show_bmp_any("jian.bmp", 40, 40,625,375); //4
		
		 snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[8].name, items[8].count, items[8].stock,items[8].price);
        fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 320, 100, p);
         snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[9].name, items[9].count, items[9].stock,items[9].price);
		fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 580, 100, p);
        snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[10].name, items[10].count, items[10].stock,items[10].price);
		fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 320, 310, p);
         snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[11].name, items[11].count, items[11].stock,items[11].price);
		fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 580, 310, p);
		get_xy(&x, &y);
		if(305 < x && x < 345 && 180 < y && y < 220)
		{
			 update_cart("真皮沙发", 1);
		}
		if(355 < x && x < 395 && 180 < y && y < 220)
		{
			 update_cart("真皮沙发", -1);
		}
		if(575 < x && x < 615 && 180 < y && y < 220)
		{
			 update_cart("精美茶几", 1);
		}
		if(625 < x && x < 665 && 180 < y && y < 220)
		{
			update_cart("精美茶几", -1);
		}
		if(305 < x && x < 345 && 375 < y && y < 415)
		{
			 update_cart("象皮床", 1);
		}
		if(355 < x && x < 395 && 375 < y && y < 415)
		{
			 update_cart("象皮床", -1);
		}
		if(575 < x && x < 615 && 375 < y && y < 415)
		{
			 update_cart("小盆摘", 1);
		}
		if(625 < x && x < 665 && 375 < y && y < 415)
		{
			 update_cart("小盆摘", -1);
		}
		if((700 < x && x < 800 && 340 < y && y < 440)||(0 < x && x < 100 && 140 < y && y < 240)||(0 < x && x < 100 && 0 < y && y < 140)||(0 < x && x < 100 && 340 < y && y < 440))
		{break;}
	}
	int i;
	FILE *fp = fopen("stock.txt", "w");
    if(fp == NULL)
    {
    printf("打开文件错误！\n");
    exit(1);
    }
    for(i = 0; i < MAX_ITEMS; i++)
    {
      fprintf(fp, "%s %d\n", items[i].name, items[i].stock);
    }

    fclose(fp);
}
void gao4()//管理商品”零食“的函数
{
    int lcd = open("/dev/fb0",O_RDWR);
    struct fb_var_screeninfo fixinfo; // 固定属性
    ioctl(lcd,FBIOGET_VSCREENINFO,&fixinfo); //// 获取固定属性
    unsigned long VWIDTH  = fixinfo.xres; //可见区宽度（单位：像素）
    unsigned long VHEIGHT = fixinfo.yres; //可见区高度（单位：像素）
    unsigned long BPP = fixinfo.bits_per_pixel;  //色深  
    char  *p = mmap(NULL, VWIDTH * VHEIGHT * BPP/8, PROT_WRITE, MAP_SHARED, lcd, 0); 
     bzero(p,VWIDTH*VHEIGHT*BPP/8);
    font *f1 = fontLoad("simfang.ttf"); 
    font *f2 = fontLoad("simfang.ttf"); 
    fontSetSize(f1, 20);
    fontSetSize(f2, 12);
    bitmap *bm1;
    bitmap *bm2;
    bitmap *bm3;
    bitmap *bm4; 
    bm1  =  createBitmapWithInit(400, 40, 4, 0xFFFFFFFF); 
    bm2  =  createBitmapWithInit(400, 40, 4, 0xFFFFFFFF); 
    bm3  =  createBitmapWithInit(75, 60,  4, 0x0000000); 
	int x,y;
	while(1)
	{
		show_bmp_any("13.bmp", 160, 160, 145, 45);
		show_bmp_any("14.bmp", 160, 160, 415, 45);
		show_bmp_any("15.bmp", 160, 160, 145, 245);
		show_bmp_any("16.bmp", 160, 160, 415, 245);
		show_bmp_any("zhifu.bmp", 100, 100,700,340);
		show_bmp_any("gouwu.bmp", 100, 100,700,40);
		show_bmp_any("jia.bmp", 40, 40,305,180);   //1
		show_bmp_any("jian.bmp", 40, 40,355,180); //1
		show_bmp_any("jia.bmp", 40, 40,575,180);   //2
		show_bmp_any("jian.bmp", 40, 40,625,180); //2
		show_bmp_any("jia.bmp", 40, 40,305,375);   //3
		show_bmp_any("jian.bmp", 40, 40,355,375); //3
		show_bmp_any("jia.bmp", 40, 40,575,375);   //4
		show_bmp_any("jian.bmp", 40, 40,625,375); //4
		
		 snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[12].name, items[12].count, items[12].stock,items[12].price);
        fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 320, 100, p);
         snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[13].name, items[13].count, items[13].stock,items[13].price);
		fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 580, 100, p);
        snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[14].name, items[14].count, items[14].stock,items[14].price);
		fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 320, 310, p);
         snprintf(buf, 100, "%s\n数量：%d\n库存：%d\n单价：%d元\n", items[15].name, items[15].count, items[15].stock,items[15].price);
		fontPrint(f2,bm3,0,0,buf,0xffffffff,0);
        showbitmap(bm3, 580, 310, p);
		get_xy(&x, &y);
		if(305 < x && x < 345 && 180 < y && y < 220)
		{
			 update_cart("虎皮凤爪", 1);
		}
		if(355 < x && x < 395 && 180 < y && y < 220)
		{
			 update_cart("虎皮凤爪", -1);
		}
		if(575 < x && x < 615 && 180 < y && y < 220)
		{
			update_cart("梅干菜扣肉", 1);
		}
		if(625 < x && x < 665 && 180 < y && y < 220)
		{
			update_cart("梅干菜扣肉", -1);
		}
		if(305 < x && x < 345 && 375 < y && y < 415)
		{
			 update_cart("劲爽鱿鱼", 1);
		}
		if(355 < x && x < 395 && 375 < y && y < 415)
		{
			 update_cart("劲爽鱿鱼", -1);
		}
		if(575 < x && x < 615 && 375 < y && y < 415)
		{
			update_cart("一根葱",1);
		}
		if(625 < x && x < 665 && 375 < y && y < 415)
		{
			 update_cart("一根葱", -1);
		}
		if((700 < x && x < 800 && 340 < y && y < 440)||(0 < x && x < 100 && 140 < y && y < 240)||(0 < x && x < 100 && 240 < y && y < 340)||(0 < x && x < 100 && 0 < y && y < 140))
		{break;}
	}
	int i;
	FILE *fp = fopen("stock.txt", "w");
    if(fp == NULL)
    {
    printf("打开文件错误！\n");
    exit(1);
    }
    for(i = 0; i < MAX_ITEMS; i++)
    {
      fprintf(fp, "%s %d\n", items[i].name, items[i].stock);
    }
    fclose(fp);
}