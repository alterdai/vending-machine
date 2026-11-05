# 自动售货机：6818 触屏购物练习

用 C 编写的嵌入式 Linux 购物演示，保留原项目的 20 件商品与 BMP 图片。
直接使用 framebuffer 和 Linux input 设备，不依赖桌面窗口系统。

**支付是模拟操作**：支付宝/微信图片仅用于界面展示，点击“成功”模拟交易完成。
没有真实收款、支付回调、账号认证或出货电机控制，不能直接作为商用售货机使用。

## 代码结构

- `code/src/main.c`：入口。
- `gao.c`：单线程事件循环，商品分类、两页衣服、购物车、模拟支付和空闲广告。
- `cart.c`：与硬件无关的购物车与库存规则，可在电脑上测试。
- `touch.c`：持有一个触摸设备描述符，用 poll 等待输入，每秒返回一次以刷新时钟。
- `show_bmp.c`：校验并显示 24 位未压缩 BMP，支持行填充，释放文件及映射。
- `font.c`：字体加载、UTF-8 解码、裁剪渲染；字体实现来自 vendored stb_truetype。
- `beep.c`：独立蜂鸣器接口，当前购物流程未调用。

所有界面绘制和触摸事件在同一线程处理，避免多个线程争抢输入或覆盖画面。
加购物车只增加待购买数量，不扣库存；返回取消或空闲 30 秒会清空购物车。
模拟支付失败保留购物车供重试。模拟成功时先将候选库存写入临时文件并 rename，
保存成功后才扣除内存库存、清空购物车。失败会保留原库存和购物车。

启动会读取 `stock.txt`，兼容原来的“商品名 库存”格式；文件不存在或为空使用初始库存。
非空文件必须包含全部 20 件商品且无重复，库存须为 0–10000，否则拒绝启动以避免覆盖坏数据。
文件替换针对 Linux 单实例运行，不支持多进程并发交易，也不保证突然断电后的持久性。

## 编译

开发板使用原来的 ARM Linux 交叉工具链（工具链名称可能因环境不同而异）：

```sh
make -C code CC=arm-linux-gcc
# 或
cd code
arm-linux-gcc -std=c99 -O2 -Wall -Wextra -Iinc src/*.c -lm -o bin/test
```

`code/vendor/stb_truetype.h` 已包含实现和上游许可，无须额外链接字体库。
字体文件未分发：请自行准备有使用权限的中文 TrueType 字体，命名为 `simfang.ttf`。
字体解析仅用于本地可信字体。

## 部署与运行

```sh
mkdir -p runtime
cp picture/*.bmp runtime/
cp code/bin/test runtime/
# 将自己的中文字体复制为 runtime/simfang.ttf
cd runtime
./test
```

程序相对于**当前工作目录**读取图片、字体和 `stock.txt`，此目录需要可写。
升级时保留已有库存文件，不要复制空的 `other/stock.txt` 覆盖它。
`other/gec6818_beep.ko` 仅在需要独立蜂鸣器接口时安装，购物演示无需它。
Ctrl+C 或 SIGTERM 退出时释放设备、字体和屏幕映射。

硬件要求：800×480、32 位 BGRX framebuffer，行宽 3200 字节、无偏移，设备为 `/dev/fb0`；
触摸设备为 `/dev/input/event0`，ABS_X/ABS_Y 已按屏幕像素标定，并报告 BTN_TOUCH 或 ABS_PRESSURE。
触摸释放时触发操作；更换面板后需要修改设备路径和坐标转换。
不符合 framebuffer 配置时会拒绝启动。

## 验证

电脑上的 Linux 环境可编译整个程序，但运行界面仍需要开发板：

```sh
make -C code CC=gcc
make -C code CC=gcc test
```

购物车测试覆盖数量上下限、取消不扣库存、重复加减、保存失败不改状态、成功扣库存、
重启读取、坏文件拒绝和空文件兼容。GitHub Actions 另运行 AddressSanitizer / UBSan
检查购物车和字体的边界渲染。

开发板验收：逐类加减商品；切换两页衣服；取消支付后核对库存；模拟失败重试；
成功后重启核对库存；等待 30 秒后唤醒广告；反复购物观察资源占用；Ctrl+C 检查退出。
这些硬件行为必须在实际板上验证。
