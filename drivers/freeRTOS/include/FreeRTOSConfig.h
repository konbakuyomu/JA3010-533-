#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* 确保定义仅被编译器使用,而不是汇编器 */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
  #include "hc32_ll.h"
  extern uint32_t SystemCoreClock;
#endif

#define configENABLE_FPU                          0 /* 是否启用FPU支持 */
#define configENABLE_MPU                          0 /* 是否启用MPU支持 */
#define configUSE_PREEMPTION                      1 /* 1: 使用抢占式调度器, 0: 使用协作式调度器 */
#define configSUPPORT_STATIC_ALLOCATION           0 /* 1: 支持静态内存分配, 0: 不支持 */
#define configSUPPORT_DYNAMIC_ALLOCATION          1 /* 1: 支持动态内存分配, 0: 不支持 */
#define configUSE_IDLE_HOOK                       0 /* 1: 使用空闲任务钩子, 0: 不使用 */
#define configUSE_TICK_HOOK                       0 /* 1: 使用系统时钟中断钩子函数, 0: 不使用 */
#define configCPU_CLOCK_HZ                        ( SystemCoreClock ) /* 系统时钟频率(Hz) */
#define configTICK_RATE_HZ                        ((TickType_t)1000) /* 系统节拍频率(Hz) */
#define configMAX_PRIORITIES                      ( 7 ) /* 可使用的最大优先级数量 */
#define configMINIMAL_STACK_SIZE                  ((uint16_t)128) /* 空闲任务使用的堆栈大小(单位:字) */
#define configTOTAL_HEAP_SIZE                     ((size_t)15360) /* 系统所有可用的RAM大小(单位:字节) */
#define configMAX_TASK_NAME_LEN                   ( 16 ) /* 任务名最大长度 */
#define configUSE_16_BIT_TICKS                    0  /* 系统节拍计数器变量数据类型, 1:16位, 0:32位 */
#define configUSE_MUTEXES                         1  /* 1: 使用互斥信号量, 0: 不使用 */
#define configQUEUE_REGISTRY_SIZE                 8  /* 可以注册的信号量和消息队列个数 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION   1 /* 1: 使用硬件计算下一个要执行的任务, 0: 使用软件算法 */
#define configMESSAGE_BUFFER_LENGTH_TYPE          size_t /* 消息缓冲区中消息长度的数据类型 */
#define configUSE_CO_ROUTINES                     0  /* 1: 启用协程, 0: 不启用 */
#define configMAX_CO_ROUTINE_PRIORITIES           ( 2 )  /* 协程的最大优先级数 */

/* 以下是一些任务相关的API函数控制宏 */
#define INCLUDE_vTaskPrioritySet                  1  /* 设置任务优先级 */
#define INCLUDE_uxTaskPriorityGet                 1  /* 获取任务优先级 */
#define INCLUDE_vTaskDelete                       1  /* 删除任务 */
#define INCLUDE_vTaskCleanUpResources             0  /* 清除任务资源 */
#define INCLUDE_vTaskSuspend                      1  /* 挂起任务 */
#define INCLUDE_vTaskDelayUntil                   1  /* 任务绝对延时 */
#define INCLUDE_vTaskDelay                        1  /* 任务延时 */
#define INCLUDE_xTaskGetSchedulerState            1  /* 获取调度程序状态 */
#define INCLUDE_xTimerPendFunctionCall            1  /* 用于在定时器服务任务中执行函数调用 */

#define configUSE_COUNTING_SEMAPHORES             1  /* 启用计数信号量 */

/* 软件定时器相关定义 */
#define configUSE_TIMERS                          1  /* 1: 启用软件定时器, 0: 不启用 */
#define configTIMER_TASK_PRIORITY                 ( configMAX_PRIORITIES - 1 )    /* 定时器服务任务的优先级 */
#define configTIMER_QUEUE_LENGTH                  5  /* 定时器命令队列的长度 */
#define configTIMER_TASK_STACK_DEPTH              ( configMINIMAL_STACK_SIZE * 2) /* 定时器服务任务的堆栈大小 */

/* Cortex-M特定定义 */
#ifdef __NVIC_PRIO_BITS
 #define configPRIO_BITS                          __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS                          4  /* 中断优先级位数 */
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY       15  /* 中断最低优先级 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  5   /* FreeRTOS可管理的最高中断优先级 */
#define configKERNEL_INTERRUPT_PRIORITY 		          ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )  /* 内核中断的优先级 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	        ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) ) /* FreeRTOS可管理的最高中断优先级 */

/* 用户代码开始 */
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );} /* 断言 */
/* 用户代码结束 */

#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* 用户代码结束 */
#endif /* FREERTOS_CONFIG_H */
