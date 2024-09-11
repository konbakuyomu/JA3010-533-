#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* 确保定义仅被编译器使用,而不是汇编器 */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
  #include "hc32_ll.h"
  extern uint32_t SystemCoreClock;
#endif

/* 断言 */
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );} 

/************************************************************************
                    FreeRTOS基础配置配置选项 
************************************************************************/
#define configENABLE_FPU                                0  /* 是否启用FPU支持 */
#define configENABLE_MPU                                0  /* 是否启用MPU支持 */
/** 
  * 注：在多任务管理机制上，操作系统可以分为抢占式和协作式两种。
  * 协作式调度器是任务执行完主动释放CPU后，再切换到下一个任务，任务切换的时机完全取决于正在运行的任务
  * 抢占式调度器是任务要么被高优先级的抢占，要么就是执行遇到阻塞式API来释放CPU使用权，让低优先级的任务执行
  */

#define configUSE_PREEMPTION                            1  /* 1: 使用抢占式调度器, 0: 使用协作式调度器 */
/**
  *如何找到最高优先级的就绪任务的TCB？
  *某些硬件运行FreeRTOS有两种方法选择下一个要执行的任务：通用方法和特定于硬件的方法（以下简称“特殊方法”）。
  *查找最高优先级的就绪任务
  * 
  * 通用方法：
  *      1.configUSE_PORT_OPTIMISED_TASK_SELECTION 为 0 或者硬件不支持这种特殊方法。
  *      2.可以用于所有FreeRTOS支持的硬件
  *      3.完全用C实现，效率略低于特殊方法。
  *      4.不强制要求限制最大可用优先级数目
  * 特殊方法：
  *      1.必须将configUSE_PORT_OPTIMISED_TASK_SELECTION设置为1。
  *      2.依赖一个或多个特定架构的汇编指令（一般是类似计算前导零[CLZ]指令）。
  *      3.比通用方法更高效
  *      4.一般强制限定最大可用优先级数目为32
  * 一般是硬件计算前导零指令，如果所使用的MCU没有这些硬件指令的话此宏应该设置为0！
  */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION         1  /* 0：通用方法  1：特殊方法 (根据处理器优化的方法) */
/** 
  * 低功耗模式的使能，假设开启低功耗的话可能会导致下载出现问题，因为程序在睡眠中,可用以下办法解决
  * 
  * 下载方法：
  *      1.将开发版正常连接好
  *      2.按住复位按键，点击下载瞬间松开复位按键
  *     
  *      1.通过跳线帽将 BOOT 0 接高电平(3.3V)
  *      2.重新上电，下载
  *    
  *         1.使用FlyMcu擦除一下芯片，然后进行下载
  *        STMISP -> 清除芯片(z)
  */
#define configUSE_TICKLESS_IDLE                         0  /* 1：使能低功耗tickless模式   0：保持系统节拍（tick）中断一直运行 */
/**
  * 写入实际的CPU内核时钟频率，也就是CPU指令执行频率，通常称为Fclk
  * Fclk为供给CPU内核的时钟信号，我们所说的cpu主频为 XX MHz，
  * 就是指的这个时钟信号，相应的，1/Fclk即为cpu时钟周期；
  */
#define configCPU_CLOCK_HZ                              ( SystemCoreClock )  /* 系统时钟频率(Hz) */
#define configTICK_RATE_HZ                              ((TickType_t)1000)   /* 1000表示1ms中断1次，RTOS系统节拍中断的频率，即系统时钟周期。即一秒中断的次数，每次中断RTOS都会进行任务调度 */
#define configMAX_PRIORITIES                            ( 7 )  /* 可使用的最大优先级数量 */
#define configMINIMAL_STACK_SIZE                        ((uint16_t)128)  /* 空闲任务使用的堆栈大小(单位:字) */
#define configMAX_TASK_NAME_LEN                         ( 16 )  /* 任务名最大长度 */
#define configUSE_16_BIT_TICKS                          0  /* 系统节拍计数器变量数据类型, 1:16位, 0:32位 */
#define configIDLE_SHOULD_YIELD                         1  /* 1:  相同优先级的任务将进行时间片轮转, 0: 空闲任务将不进行时间片轮转 */
#define configUSE_TASK_NOTIFICATIONS                    1  /* 1: 使用任务通知, 0: 不使用 */
#define configTASK_NOTIFICATION_ARRAY_ENTRIES           1  /* 任务通知数组大小 */
#define configUSE_MUTEXES                               1  /* 1: 使用互斥信号量, 0: 不使用 */
#define configUSE_RECURSIVE_MUTEXES                     0  /* 1: 使用递归互斥信号量, 0: 不使用 */
#define configUSE_COUNTING_SEMAPHORES                   1  /* 启用计数信号量 */
#define configQUEUE_REGISTRY_SIZE                       8  /* 可以注册的信号量和消息队列个数 */
#define configUSE_QUEUE_SETS                            0  /* 1: 使用队列集, 0: 不使用 */
#define configUSE_TIME_SLICING                          1  /* 1: 采用带时间片划分的优先级抢占式调度, 0: 不使用 */
#define configUSE_NEWLIB_REENTRANT                      0  /* 1: 使用Newlib库, 0: 不使用 */
#define configENABLE_BACKWARD_COMPATIBILITY             0  /* 1: 启用向后兼容性, 0: 不启用 */
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS         0  /* 配置 NUM_线程本地存储指针数量 */
#define configSTACK_DEPTH_TYPE                          uint16_t  /* 堆栈深度类型 */
#define configMESSAGE_BUFFER_LENGTH_TYPE                size_t  /* 消息缓冲区中消息长度的数据类型 */

/*****************************************************************
              FreeRTOS与内存申请有关配置选项                                               
*****************************************************************/
#define configSUPPORT_STATIC_ALLOCATION                 0  /* 1: 支持静态内存分配, 0: 不支持 */
#define configSUPPORT_DYNAMIC_ALLOCATION                1  /* 1: 支持动态内存分配, 0: 不支持 */
#define configTOTAL_HEAP_SIZE                           ((size_t)15360)  /* 系统所有堆的大小，即总共有多少堆(单位:字节) */
#define configAPPLICATION_ALLOCATED_HEAP                0  /* 1: 允许应用程序分配堆, 0: 不允许 */

/***************************************************************
              FreeRTOS与钩子函数有关的配置选项                                            
**************************************************************/
/** 
  * 
  * 空闲任务钩子是一个函数，这个函数由用户来实现，
  * FreeRTOS规定了函数的名字和参数：void vApplicationIdleHook(void )，这个函数在每个空闲任务周期都会被调用。
  * 对于已经删除的RTOS任务，空闲任务可以释放分配给它们的堆栈内存。
  * 因此必须保证空闲任务可以被CPU执行
  * 使用空闲钩子函数设置CPU进入省电模式是很常见的
  * 会引起空闲任务阻塞的API函数不能调用
  */
#define configUSE_IDLE_HOOK                             0  /* 1: 使用空闲任务钩子, 0: 不使用 */
/** 
  * 时间片钩子是一个函数，这个函数由用户来实现，
  * FreeRTOS规定了函数的名字和参数：void vApplicationTickHook(void )
  * 时间片中断可以周期性的调用
  * 函数必须非常短小，不能大量使用堆栈，
  * 不能调用以”FromISR" 或 "FROM_ISR”结尾的API函数
  */
/*xTaskIncrementTick函数是在xPortSysTickHandler中断函数中被调用的。因此，vApplicationTickHook()函数执行的时间必须很短才行*/
#define configUSE_TICK_HOOK                             0  /* 1: 使用系统时钟中断钩子函数, 0: 不使用 */
/**
  * 大于0时启用堆栈溢出检测功能，如果使用此功能，用户必须提供一个栈溢出钩子函数，如果使用的话此值可以为1或者2，因为有两种栈溢出检测方法 
  */
#define configCHECK_FOR_STACK_OVERFLOW                  0  /* 配置检查堆栈溢出 */
/**
  * 内存分配失败钩子是一个函数，这个函数由用户来实现，
  * FreeRTOS规定了函数的名字和参数：void vApplicationMallocFailedHook(void )
  * 当内存分配失败时，会调用这个函数
  */
#define configUSE_MALLOC_FAILED_HOOK                    0  /* 1: 使用内存分配失败钩子, 0: 不使用 */
#define configUSE_DAEMON_TASK_STARTUP_HOOK              0  /* 1: 使用守护进程任务启动钩子, 0: 不使用 */

/********************************************************************
          FreeRTOS与运行时间和任务状态收集有关的配置选项   
**********************************************************************/
#define configGENERATE_RUN_TIME_STATS                   0  /* 1: 生成运行时统计信息, 0: 不生成 */
#define configUSE_TRACE_FACILITY                        0  /* 1: 启用跟踪功能, 0: 不启用 */
/** 与宏configUSE_TRACE_FACILITY同时为1时会编译下面3个函数
  * prvWriteNameToBuffer()
  * vTaskList(),
  * vTaskGetRunTimeStats()
  */
#define configUSE_STATS_FORMATTING_FUNCTIONS            0  /* 1: 使用统计格式化函数, 0: 不使用 */

/********************************************************************
                FreeRTOS与协程有关的配置选项                                                
*********************************************************************/
#define configUSE_CO_ROUTINES                           0  /* 1: 启用协程, 0: 不启用 */
#define configMAX_CO_ROUTINE_PRIORITIES                 ( 2 )  /* 协程的最大优先级数 */

/***********************************************************************
                FreeRTOS与软件定时器有关的配置选项      
**********************************************************************/
#define configUSE_TIMERS                                1  /* 1: 启用软件定时器, 0: 不启用 */
#define configTIMER_TASK_PRIORITY                       ( configMAX_PRIORITIES - 1 )  /* 定时器服务任务的优先级 */
#define configTIMER_QUEUE_LENGTH                        5  /* 定时器命令队列的长度 */
#define configTIMER_TASK_STACK_DEPTH                    ( configMINIMAL_STACK_SIZE * 2)  /* 定时器服务任务的堆栈大小 */

/************************************************************
            FreeRTOS可选函数配置选项                                                     
************************************************************/
#define INCLUDE_vTaskPrioritySet                        1  /* 设置任务优先级 */
#define INCLUDE_uxTaskPriorityGet                       1  /* 获取任务优先级 */
#define INCLUDE_vTaskDelete                             1  /* 删除任务 */
#define INCLUDE_vTaskCleanUpResources                   0  /* 清除任务资源 */
#define INCLUDE_vTaskSuspend                            1  /* 挂起任务 */
#define INCLUDE_vTaskDelayUntil                         1  /* 任务绝对延时 */
#define INCLUDE_vTaskDelay                              1  /* 任务延时 */
#define INCLUDE_xTaskGetSchedulerState                  1  /* 获取调度程序状态 */
#define INCLUDE_xTimerPendFunctionCall                  1  /* 用于在定时器服务任务中执行函数调用 */
#define INCLUDE_xQueueGetMutexHolder                    0  /* 获取互斥量持有者 */
#define INCLUDE_uxTaskGetStackHighWaterMark             0  /* 获取任务堆栈历史最大使用量 */
#define INCLUDE_xTaskGetCurrentTaskHandle               1  /* 获取当前任务句柄 */
#define INCLUDE_eTaskGetState                           0  /* 获取任务状态 */
#define INCLUDE_xTimerPendFunctionCall                  1  /* 用于在定时器服务任务中执行函数调用 */
#define INCLUDE_xTaskAbortDelay                         0  /* 中止任务延时 */
#define INCLUDE_xTaskGetHandle                          0  /* 获取任务句柄 */
#define INCLUDE_xTaskResumeFromISR                      1  /* 从中断中恢复任务 */

/******************************************************************
            FreeRTOS与中断有关的配置选项                                                 
******************************************************************/
#ifdef __NVIC_PRIO_BITS
 #define configPRIO_BITS                                __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS                                4  /* 中断优先级位数 */
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15  /* 中断最低优先级 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5   /* FreeRTOS可管理的最高中断优先级 */
#define configKERNEL_INTERRUPT_PRIORITY                 ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )  /* 内核中断的优先级 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) ) /* FreeRTOS可管理的最高中断优先级 */

/****************************************************************
            FreeRTOS与中断服务函数有关的配置选项                         
****************************************************************/
#define vPortSVCHandler                                 SVC_Handler
#define xPortPendSVHandler                              PendSV_Handler
#define xPortSysTickHandler                             SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
