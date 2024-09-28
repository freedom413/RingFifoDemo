---
tags:
- FIFO
- 数据结构  
- STM32 
title: 理解环形队列
date: 2024-09-14
updated: 
categories:
- 记录 
keywords:
- STM32  
- modbus  
description: 
top_img: 
comments: 
cover: 环形.png
toc: 
toc_number: 
toc_style_simple: 
copyright: 
copyright_author: 
copyright_author_href: 
copyright_url: 
copyright_info: 
mathjax: 
katex: 
aplayer: 
highlight_shrink: 
aside: 
swiper_index: 1
top_group_index: 1
background: "#fff"
---

# 解读环形队列-C语言实现  
环形队列（Circular Queue）是一种特殊的队列数据结构。与普通的线性队列不同，环形队列在物理存储上表现为一个首尾相连的环状结构，在逻辑上则仍然遵循先进先出（FIFO, First In First Out）的原则。  
## 特点  
1. **循环利用空间**：当队列中的元素被删除后，原来的空间可以被重新使用，从而避免了普通队列中即使有空闲空间也无法使用的现象。  
2. **队头队尾指针**：通常环形队列会使用两个指针来标识队列的头部和尾部，分别称为head（队头指针）和end（队尾指针）。  
3. **队满与队空判断**：由于是环形结构，判断队列是否已满或为空需要特殊处理。一种常见的方法是在数组大小为N的情况下，预留一个位置不使用，这样当end + 1 == head时，表示队列已满；而当end == head时，则表示队列为空或满（取决于具体实现方式）。  
4. **动态调整**：在一些实现中，可以通过动态调整数组大小来适应不断变化的数据量，但这不是环形队列特有的性质，也可以应用于其他数据结构。  
## 应用场景  
环形队列因其循环利用空间的特点，在某些应用场景中非常有用，例如：  
- 缓冲区管理：在网络编程或者硬件驱动程序中，环形队列经常用于管理固定大小的缓冲区。
- 生产者消费者模型：在多线程环境中，环形队列可以用作生产者与消费者之间共享的数据结构。
- 滑动窗口算法：在算法设计中，环形队列可以帮助实现滑动窗口等高效的数据处理策略。
  
具体到stm32中,常用于串口接收,将数据存入环形fifo可以使得数据可以异步的使用，并且可以在收发频繁的使用场景中(如串口连接wifi或者4g模块，进行网络通信，需要保持网络一直传输有效数据)，可以有效的利用单片机的ram空间.并且提高单片机的执行效率。

## 图解
### 非循环队列  
非循环队列 也是由一个头指针*head*和一个尾指针*end*构成,并且遵循两个规则
>1. 头指针指向数据头
>2. 尾指针指向数据尾
- **首先来看怎么入队**
为了保证第一个入队的数据存储在下标为0的数组，并且还要保证每次入队后头指针要加一后移，且要满足**规则2**尾指针指向数据尾，所以尾指针*end*的初始值关系为end+1=0 所以end等于 **-1** ，
入下图所示
![1](1.png "end指向-1")
*end* = *end*+1 = 0
然后将数据放入end指向的地址空间就完成了一次入队如下图所示
![1](2.png "end指向0")
- **怎么出队**
做法就是**先将***head*指针指向的数据抛出，**然后***head*加一指向数据头**这个先后顺序比较关键是跟循环队列有区别的**，在第一次出队的时候必定会将下标为0的数据抛出，然后指向下标为1的数据。所以head+1 = 1 所以head的初始值为**0**
假如现在队列中有两个数据入下图所示
![1](3.png "end指向1 head指向0")
出一个数据就是先将*head*指向空间的数据抛出去供使用，然后*head*加一移到下一个待抛出的数据下标位置入下图所示
![1](4.png "end指向1 head指向1")
- **那么怎么判断队空呢**
  首先有一个条件是我们已经知道的就是当*end为-1*时表示还没有数据入队那队列肯定为空，然后我们接着上面再出一次队是不是数据也出玩了，让我们来看看效果吧。
![1](5.png "end指向1 head指向2")
此时我们发现*head*指针是不是比*end*指针还大一呀，所以队空的条件有两条
>1. ***end为 -1*** 表示队空
>2. ***end* + 1 = *head*** 表示队空

这个**队空规则2**同样示用于当队列里只有一个数据时的情况如下图所示
![1](6.png "end指向0 head指向0")
此时队列里有一个数据，然后出队一个数据满足**队空规则2**如下图所示
![1](7.png "end指向0 head指向1")
- **那么怎么判断队满呢**
在非循环队列中规则很简单就一条
>1. ***end* + 1 = MAX_SIZE** //其中*MAX_SIZE*是队列的最大存储个数  

下图表示队满
![1](8.png "end指向0 head指向6")
但是我们会发现下图也表示队满
![1](9.png "end指向2 head指向6")
其中 0 1 2 这些位置空着也无法使用，这就是非循环队列最大的缺点不能充分利用空间。
### 循环队列  
循环队列 同样也是由一个头指针*head*和一个尾指针*end*构成,但是遵循两个规则中头指针并不指向数据头而是指向上一次被删除的数据地址。
>1. 头指针指向上一次被删除的数据地址
>2. 尾指针指向数据尾
- **怎么实现下标在设定范围移动**
假如一个队列的可存储数据个数**MAX_SIZE**=7那么可以使用下面这个公式使得让指针一直加一，但是可以让指针保持在**0~6**这个区间
>***end = (end+1)%MAX_SIZE***
>***haed = (head+1)%MAX_SIZE***
- **循环队列入队**
其操作逻辑和非循环队列一样 但是起始下标为**0**
>1. end 加一
>2. 把数据存入end指向的数据空间

入队一个数据如下图所示 第一个数据存在了1号下标
![1](10.png "end指向1 head指向0")
   
- **循环队列出队**
这一点顺序和非循环队了相反 但起始下标也为**0**

>1. head 加一
>2. 把head指向的数据空间的数据抛出

在队列有2个数据时出队一个数据，可以发现此时head指向的数据空间并的数据已经被抛出过。
![1](11.png "end指向2 head指向1")
- **循环队列判断队满**
在下图这种情况下，也就是一直入队到**MAX_SIZE -1**的下标并且没有出队，虽然0号下标是空的，但此时我们也认为队满了。
![1](12.png "end指向6 head指向0")
这种队满的规则可以总结为
>**head = (end+1)%MAX_SIZE**

那这种情况适用于其它情况吗，比如head end 产生循环不在头尾的时候
![1](13.png "end指向2 head指向3")
可以发现这个规则同样适用
- **循环队列判断队空**
首先在没有任何出队入队的情况下
>1. head = 0 ;
>2. end = 0 ;

所以队空的规则是 

> **head == end**

在有了队列操作过后这个规则还满足吗？比如在入队4个数据后出队四个数据
先入队4个数据入下图
![1](14.png "end指向4 head指向0")
然后出队4个数据
![1](15.png "end指向4 head指向4")
我们发现这个规则同样适用，那我们在继续入队看看产生循环后还满足吗
首先入队5个数据
![1](16.png "end指向2 head指向4")
然后出队5个数据
![1](17.png "end指向2 head指向2")
发现在产生循环后这个规则一样适用

当然你也可以尝试将**循环队列出队**的规则改为**非循环队列出队**的规则来试试看，你会发现判断队空的规则和判断队满的规则就变得一样了都是**head = (end+1)%MAX_SIZE**，所以这就是为什么循环队列需要留出一个空位置不用的原因了，主要就是为了使队空和队满的规则变得不同，从而在程序中才好进行判断

## 实现  

当然是使用c语言来实现那，毕竟是要用在单片机上跑的呀了💘  

### 定义环形Fifo数据结构
- **Max_Size** ：队列的空间个数 这里使用宏定义以后方便更改
  
---

- **head** ：头指针指向队头
- **end**  ：尾指针指向队尾
- **data**[Max_Size] ：定义的数据buff

C语言结构体定义如下
```c
    //队列可用容量为最大长度减一 
    #define Max_Size (10)
    typedef struct
    {
        uint32_t head;
        uint32_t end;
        uint8_t data[Max_Size];
    } Fifo_t;
```
### 出队入队及满空判断
在写程序前我们先将每一个小的步骤梳理清楚，那写整体程序时就可以集齐龙珠来召唤神龙了😎
- 为了理解方便加入输入和输出变量。
```c
in  //入队的数据
out //出队的数据
```
- 队列初始化：head =0 ；end =0；
```c
head =0 ；
end =0；
```
- 入队方式:先end指针加1，后把in的数据赋值给end指向的空间；为了保证范围在0~Max_Size 采用取余的方式。
```c
end = (end+1)%Max_Size;
data[end] = in;
```
- 出队方式:先head指针加1，后把head指向的空间数据赋值给out；为了保证head范围在0~Max_Size 采用取余的方式。
```c
head = (head+1)%Max_Size;
out = data[head];
```
- 队满判断: end+1 == head  ；为了保证end范围在0~Max_Size 采用取余的方式。
```c
(end+1)%Max_Size == head
```
- 队空判断: end == head。
```c
end == head;
```
至此所有龙珠已经收集到位🌞
### 初始化函数
这个直接抄
```c
void FifoInit(Fifo_t *f)
{
    f->end = 0;
    f->head = 0;
}
```
### 判断队列空函数
抄
```c
uint8_t isEmpty(Fifo_t *f)
{
    return (f->end == f->head);
}
```
### 判断队列满函数
抄,不就是抄公式吗...😂
```c
uint8_t isFull(Fifo_t *f)
{
    return ((f->end + 1) % Max_Size == f->head);
}
```

### 入队函数
这个终于有点发挥空间了
```c
/**
 * @brief 入队函数
 * @param Fifo_t *f 队列结构体指针
 * @param uint8_t *in 入队数据指针
 * @param uint8_t *in 入队数据长度
 * @retval 0 表示队列满了 其它数字表示成功入队的数据个数
 */
uint32_t FifoIn(Fifo_t *f, uint8_t *in, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++)
    {
        if (isFull(f))
        {
            printf(" in: %d 入队失败\n", in[i]);
            return 0;
        }
        f->end = (f->end + 1) % Max_Size;
        f->data[f->end] = in[i];

        printf("in: %d\n", in[i]);
    }

    return i;
}
```
### 出队函数
继续
```c
/**
 * @brief 出队函数
 * @param Fifo_t *f 队列结构体指针
 * @param uint8_t *out 出队数据指针
 * @param uint32_t len 出队数据长度
 * @retval 0 表示队列空了没数据量出队失败 其它数字表示成功出队的数据个数
 */
uint8_t FifoOut(Fifo_t *f, uint8_t *out, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++)
    {
        if (isEmpty(f))
        {
            printf("出队失败\n");
            return 0;
        }
        f->head = (f->head + 1) % Max_Size;
        out[i] = f->data[f->head];
        printf("out: %d\n", out[i]);
    }
    return i;
}
```
### 程序验证
写了程序要验证，得有始有终😊。验证结果非常完美ok了
```c
static Fifo_t g_fifo;

int main(void)
{
    FifoInit(&g_fifo);

    uint8_t in[10] = {1,2,3,4,5,6,7,8,9,10};
    uint8_t out[10] = {0};
    
    //入队 3个数据 1 2 3
    FifoIn(&g_fifo,in,3);

    //出队 2个数据 1 2
    FifoOut(&g_fifo,out,2);

    //入队 4 5 6 7 8 此时队列里右6个数据
    FifoIn(&g_fifo,&in[3],5);

    //出队队2个数据 应该是 3 4 此时队列里有4 个数据
    FifoOut(&g_fifo,out,2);

    //入队6 个数据 1 2 3 4 5 6   此时队列里有 10 个数据 
    //但是最后一个数据会 入队失败一次  因为队列的最大容量为Max_Size -1 == 9。 
    FifoIn(&g_fifo,&in[0],6);


    //出队8 个数据应该是  5 6 7 8 1 2 3 4 此时队列里有 1 个数据 
    FifoOut(&g_fifo,out,8);

    //出队2 个数据应该是 5 然后队列空 出队失败一次
    FifoOut(&g_fifo,out,8);

    return 0;
}
```
程序运行结果和我写验证程序时的注释一致，如图所示
![队列验证](队列验证.png "验证一致")

下面附上完整源码了🙌
```c

/* 环形队列 规则说明
head 头指针指向队头
end  尾指针指向队尾
Max_Size 队列的空间个数
data[Max_Size] 定义的数据buff


in  入队的数据
out 出队的数据

队列初始化：head =0 ；end =0；

入队方式:先end指针加1，后把in的数据赋值给end指向的空间；为了保证范围在0~Max_Size 采用取余的方式

    end = (end+1)%Max_Size;
    data[end] = in;

出队方式:先head指针加1，后把head指向的空间数据赋值给out；为了保证head范围在0~Max_Size 采用取余的方式

    head = (head+1)%Max_Size;
    out = data[head];

队满判断: end+1 == head  ；为了保证end范围在0~Max_Size 采用取余的方式

    (end+1)%Max_Size == head

队空判断: end == head ；

     end == head；

*/
#include "stdio.h"
#include "stdint.h"

//队列可用容量为最大长度减一 
#define Max_Size (10)

typedef struct
{
    uint32_t head;
    uint32_t end;
    uint8_t data[Max_Size];
} Fifo_t;

void FifoInit(Fifo_t *f)
{
    f->end = 0;
    f->head = 0;
}

uint8_t isEmpty(Fifo_t *f)
{
    return (f->end == f->head);
}

uint8_t isFull(Fifo_t *f)
{
    return ((f->end + 1) % Max_Size == f->head);
}

/**
 * @brief 入队函数
 * @param Fifo_t *f 队列结构体指针
 * @param uint8_t *in 入队数据指针
 * @param uint32_t len 入队数据长度
 * @retval 0 表示队列满了存不下了入队失败 其它数字表示成功入队的数据个数
 */
uint32_t FifoIn(Fifo_t *f, uint8_t *in, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++)
    {
        if (isFull(f))
        {
            printf(" in: %d 入队失败\n", in[i]);
            return 0;
        }
        f->end = (f->end + 1) % Max_Size;
        f->data[f->end] = in[i];

        printf("in: %d\n", in[i]);
    }

    return i;
}


/**
 * @brief 出队函数
 * @param Fifo_t *f 队列结构体指针
 * @param uint8_t *out 出队数据指针
 * @param uint32_t len 出队数据长度
 * @retval 0 表示队列空了没数据量出队失败 其它数字表示成功出队的数据个数
 */
uint8_t FifoOut(Fifo_t *f, uint8_t *out, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++)
    {
        if (isEmpty(f))
        {
            printf("出队失败\n");
            return 0;
        }
        f->head = (f->head + 1) % Max_Size;
        out[i] = f->data[f->head];
        printf("out: %d\n", out[i]);
    }
    return i;
}


static Fifo_t g_fifo;

int main(void)
{
    FifoInit(&g_fifo);

    uint8_t in[10] = {1,2,3,4,5,6,7,8,9,10};
    uint8_t out[10] = {0};
    
    //入队 3个数据 1 2 3
    FifoIn(&g_fifo,in,3);

    //出队 2个数据 1 2
    FifoOut(&g_fifo,out,2);

    //入队 4 5 6 7 8 此时队列里右6个数据
    FifoIn(&g_fifo,&in[3],5);

    //出队队2个数据 应该是 3 4 此时队列里有4 个数据
    FifoOut(&g_fifo,out,2);

    //入队6 个数据 1 2 3 4 5 6   此时队列里有 10 个数据 
    //但是最后一个数据会 入队失败一次  因为队列的最大容量为Max_Size -1 == 9。 
    FifoIn(&g_fifo,&in[0],6);


    //出队8 个数据应该是  5 6 7 8 1 2 3 4 此时队列里有 1 个数据 
    FifoOut(&g_fifo,out,8);

    //出队2 个数据应该是 5 然后队列空 出队失败一次
    FifoOut(&g_fifo,out,8);

    return 0;
}
```
