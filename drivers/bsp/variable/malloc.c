/**
 ****************************************************************************************************
 * @file        malloc.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2023-05-01
 * @brief       �ڴ���� ����(����GCC,�������ڲ�SRAM�ڴ��)
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 */

#include "variable.h"

SemaphoreHandle_t xMallocMutex = NULL;

/* �ڴ��(32�ֽڶ���) */
static __attribute__((aligned(32))) uint8_t mem1base[MEM1_MAX_SIZE];

/* �ڴ����� */
static MT_TYPE mem1mapbase[MEM1_ALLOC_TABLE_SIZE];

/* �ڴ������� */
const uint32_t memtblsize = MEM1_ALLOC_TABLE_SIZE; /* �ڴ���С */
const uint32_t memblksize = MEM1_BLOCK_SIZE;       /* �ڴ�ֿ��С */
const uint32_t memsize = MEM1_MAX_SIZE;            /* �ڴ��ܴ�С */

/* �ڴ��������� */
struct _m_mallco_dev mallco_dev =
    {
        my_mem_init,    /* �ڴ��ʼ�� */
        my_mem_perused, /* �ڴ�ʹ���� */
        mem1base,       /* �ڴ�� */
        mem1mapbase,    /* �ڴ����״̬�� */
        0,              /* �ڴ����δ���� */
};

/**
 * @brief       �����ڴ�
 * @param       *des : Ŀ�ĵ�ַ
 * @param       *src : Դ��ַ
 * @param       n    : ��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
 * @retval      ��
 */
void my_mem_copy(void *des, void *src, uint32_t n)
{
    uint8_t *xdes = des;
    uint8_t *xsrc = src;
    while (n--)
        *xdes++ = *xsrc++;
}

/**
 * @brief       �����ڴ�ֵ
 * @param       *s    : �ڴ��׵�ַ
 * @param       c     : Ҫ���õ�ֵ
 * @param       count : ��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
 * @retval      ��
 */
void my_mem_set(void *s, uint8_t c, uint32_t count)
{
    uint8_t *xs = s;
    while (count--)
        *xs++ = c;
}

/**
 * @brief       �ڴ�����ʼ��
 * @param       ��
 * @retval      ��
 */
void my_mem_init(void)
{
    if (xMallocMutex == NULL)
    {
        xMallocMutex = xSemaphoreCreateMutex();
    }

    my_mem_set(mallco_dev.memmap, 0, memtblsize * 2); /* �ڴ�״̬���������� */
    mallco_dev.membase = (uint8_t *)SRAM_BASE;        /* �����ڴ����ʼ��ַ */
    mallco_dev.memrdy = 1;                            /* �ڴ�����ʼ��OK */
}

/**
 * @brief       ��ȡ�ڴ�ʹ����
 * @param       ��
 * @retval      ʹ����(0~100)
 */
uint8_t my_mem_perused(void)
{
    uint32_t used = 0;
    uint32_t i;
    for (i = 0; i < memtblsize; i++)
    {
        if (mallco_dev.memmap[i])
            used++;
    }
    return (used * 100) / (memtblsize);
}

/**
 * @brief       �ڴ����(�ڲ�����)
 * @param       size : Ҫ������ڴ��С(�ֽ�)
 * @retval      �ڴ�ƫ�Ƶ�ַ
 */
static uint32_t my_mem_malloc(uint32_t size)
{
    signed long offset = 0;
    uint32_t nmemb;     /* ��Ҫ���ڴ���� */
    uint32_t cmemb = 0; /* �������ڴ���� */
    uint32_t i;
    if (!mallco_dev.memrdy)
    {
        mallco_dev.init(); /* δ��ʼ��,��ִ�г�ʼ�� */
    }
    if (size == 0)
        return 0xFFFFFFFF;     /* ����Ҫ���� */
    nmemb = size / memblksize; /* ��ȡ��Ҫ����������ڴ���� */
    if (size % memblksize)
        nmemb++;
    for (offset = memtblsize - 1; offset >= 0; offset--)
    {
        if (!mallco_dev.memmap[offset])
            cmemb++; /* �������ڴ�������� */
        else
            cmemb = 0;      /* �����ڴ������ */
        if (cmemb == nmemb) /* �ҵ�������nmemb�����ڴ�� */
        {
            for (i = 0; i < nmemb; i++) /* ��ע�ڴ��ǿ� */
            {
                mallco_dev.memmap[offset + i] = nmemb;
            }
            return (offset * memblksize); /* ����ƫ�Ƶ�ַ */
        }
    }
    return 0xFFFFFFFF; /* δ�ҵ����Ϸ����������ڴ�� */
}

/**
 * @brief       �ͷ��ڴ�(�ڲ�����)
 * @param       offset : �ڴ��ַƫ��
 * @retval      0, �ͷųɹ�; 1, �ͷ�ʧ��;
 */
static uint8_t my_mem_free(uint32_t offset)
{
    int i;
    if (!mallco_dev.memrdy)
    {
        mallco_dev.init();
        return 1; /* δ��ʼ�� */
    }
    if (offset < memsize) /* ƫ�����ڴ���� */
    {
        int index = offset / memblksize;      /* ƫ�������ڴ����� */
        int nmemb = mallco_dev.memmap[index]; /* �ڴ������ */
        for (i = 0; i < nmemb; i++)           /* �ڴ������ */
        {
            mallco_dev.memmap[index + i] = 0;
        }
        return 0;
    }
    else
        return 2; /* ƫ�Ƴ����� */
}

/**
 * @brief       �ͷ��ڴ�(�ⲿ����)
 * @param       ptr : �ڴ��׵�ַ
 * @retval      ��
 * @note        �˺���ͨ�����ϵͳ״̬�������Ƿ�ʹ�û��������Ա���Ǳ�ڵ��������⣺
 *              1. ����ϵͳ��ʼ���׶Σ���FreeRTOS������δ����ʱ������������ʹ�á�
 *              2. �����ж������ģ����ж��в�ʹ�ÿ��ܵ��������л���FreeRTOS API��
 *              3. ����ݹ���������ϵͳ��ʼ�����ж�����������������ֹǱ�ڵĵݹ�������
 *              4. ��������ʱ��������ϵͳ��������ʱʹ�û����������ڴ������ȷ���̰߳�ȫ��
 */
void myfree(void *ptr)
{
    if (ptr == NULL)
        return;

    // �������Ĺؼ����֣�
    // ���������Ƿ���������ǰ�Ƿ����ж���
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED || xPortIsInsideInterrupt())
    {
        // ���������δ���������ж��У�ֱ���ͷ��ڴ棬��ʹ�û�����
        // ���������ϵͳδ��ȫ��ʼ�����жϴ���ʱ��������
        uint32_t offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase;
        my_mem_free(offset);
    }
    else
    {
        // ϵͳ��������ʱ��ʹ�û����������ڴ��ͷ�
        if (xSemaphoreTake(xMallocMutex, portMAX_DELAY) == pdTRUE)
        {
            uint32_t offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase;
            my_mem_free(offset);
            xSemaphoreGive(xMallocMutex);
        }
    }
}

/**
 * @brief       �����ڴ�(�ⲿ����)
 * @param       size : Ҫ������ڴ��С(�ֽ�)
 * @retval      ���䵽���ڴ��׵�ַ
 * @note        �˺���ͨ�����ϵͳ״̬�������Ƿ�ʹ�û��������Ա���Ǳ�ڵ��������⣺
 *              1. ����ϵͳ��ʼ���׶Σ���FreeRTOS������δ����ʱ������������ʹ�á�
 *              2. �����ж������ģ����ж��в�ʹ�ÿ��ܵ��������л���FreeRTOS API��
 *              3. ����ݹ���������ϵͳ��ʼ�����ж�����������������ֹǱ�ڵĵݹ�������
 *              4. ��������ʱ��������ϵͳ��������ʱʹ�û����������ڴ������ȷ���̰߳�ȫ��
 */
void *mymalloc(uint32_t size)
{
    void *ptr = NULL;
    // �������Ĺؼ����֣�
    // ���������Ƿ���������ǰ�Ƿ����ж���
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED || xPortIsInsideInterrupt())
    {
        // ���������δ���������ж��У�ֱ�ӷ����ڴ棬��ʹ�û�����
        // ���������ϵͳδ��ȫ��ʼ�����жϴ���ʱ��������
        uint32_t offset = my_mem_malloc(size);
        if (offset != 0xFFFFFFFF)
        {
            ptr = (void *)((uint32_t)mallco_dev.membase + offset);
        }
    }
    else
    {
        // ϵͳ��������ʱ��ʹ�û����������ڴ����
        if (xSemaphoreTake(xMallocMutex, portMAX_DELAY) == pdTRUE)
        {
            uint32_t offset = my_mem_malloc(size);
            if (offset != 0xFFFFFFFF)
            {
                ptr = (void *)((uint32_t)mallco_dev.membase + offset);
            }
            xSemaphoreGive(xMallocMutex);
        }
    }
    return ptr;
}

/**
 * @brief       ���·����ڴ�(�ⲿ����)
 * @param       *ptr : ���ڴ��׵�ַ
 * @param       size : Ҫ������ڴ��С(�ֽ�)
 * @retval      �·��䵽���ڴ��׵�ַ.
 */
void *myrealloc(void *ptr, uint32_t size)
{
    void *new_ptr = NULL;
    if (xSemaphoreTake(xMallocMutex, portMAX_DELAY) == pdTRUE)
    {
        uint32_t offset = my_mem_malloc(size);
        if (offset != 0xFFFFFFFF)
        {
            new_ptr = (void *)((uint32_t)mallco_dev.membase + offset);
            my_mem_copy(new_ptr, ptr, size);
            uint32_t old_offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase;
            my_mem_free(old_offset);
        }
        xSemaphoreGive(xMallocMutex);
    }
    return new_ptr;
}