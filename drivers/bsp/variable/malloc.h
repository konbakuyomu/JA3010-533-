#ifndef __MALLOC_H
#define __MALLOC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"

#define MEM1_BLOCK_SIZE         32                              /* 内存块大小为32字节 */
#define MEM1_MAX_SIZE           100 * 1024                      /* 最大管理内存 100K，留出一些空间给堆栈 */
#define MEM1_ALLOC_TABLE_SIZE   MEM1_MAX_SIZE/MEM1_BLOCK_SIZE   /* 内存表大小 */

/* 定义内存管理表类型 */
#define MT_TYPE     uint16_t

/* 内存管理控制器 */
struct _m_mallco_dev
{
    void (*init)(void);                 /* 初始化 */
    uint8_t (*perused)(void);           /* 内存使用率 */
    uint8_t *membase;                   /* 内存池 */
    MT_TYPE *memmap;                    /* 内存管理状态表 */
    uint8_t memrdy;                     /* 内存管理是否就绪 */
};

extern struct _m_mallco_dev mallco_dev; /* 在mallco.c里面定义 */
extern SemaphoreHandle_t xMallocMutex;  /* 互斥信号量，用于保护mymalloc和myfree的互斥访问 */

void my_mem_init(void);                             /* 内存管理初始化函数 */
uint8_t my_mem_perused(void);                       /* 获得内存使用率 */
void my_mem_set(void *s, uint8_t c, uint32_t count);/* 内存设置函数 */
void my_mem_copy(void *des, void *src, uint32_t n); /* 内存拷贝函数 */
void myfree(void *ptr);                             /* 内存释放(外部调用) */
void *mymalloc(uint32_t size);                      /* 内存分配(外部调用) */
void *myrealloc(void *ptr, uint32_t size);          /* 重新分配内存(外部调用) */

#ifdef __cplusplus
}
#endif

#endif