#ifndef __MALLOC_H
#define __MALLOC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"

#define MEM1_BLOCK_SIZE         32                              /* �ڴ���СΪ32�ֽ� */
#define MEM1_MAX_SIZE           100 * 1024                      /* �������ڴ� 100K������һЩ�ռ����ջ */
#define MEM1_ALLOC_TABLE_SIZE   MEM1_MAX_SIZE/MEM1_BLOCK_SIZE   /* �ڴ���С */

/* �����ڴ��������� */
#define MT_TYPE     uint16_t

/* �ڴ��������� */
struct _m_mallco_dev
{
    void (*init)(void);                 /* ��ʼ�� */
    uint8_t (*perused)(void);           /* �ڴ�ʹ���� */
    uint8_t *membase;                   /* �ڴ�� */
    MT_TYPE *memmap;                    /* �ڴ����״̬�� */
    uint8_t memrdy;                     /* �ڴ�����Ƿ���� */
};

extern struct _m_mallco_dev mallco_dev; /* ��mallco.c���涨�� */
extern SemaphoreHandle_t xMallocMutex;  /* �����ź��������ڱ���mymalloc��myfree�Ļ������ */

void my_mem_init(void);                             /* �ڴ�����ʼ������ */
uint8_t my_mem_perused(void);                       /* ����ڴ�ʹ���� */
void my_mem_set(void *s, uint8_t c, uint32_t count);/* �ڴ����ú��� */
void my_mem_copy(void *des, void *src, uint32_t n); /* �ڴ濽������ */
void myfree(void *ptr);                             /* �ڴ��ͷ�(�ⲿ����) */
void *mymalloc(uint32_t size);                      /* �ڴ����(�ⲿ����) */
void *myrealloc(void *ptr, uint32_t size);          /* ���·����ڴ�(�ⲿ����) */

#ifdef __cplusplus
}
#endif

#endif