#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
/* Ensure definitions are only used by the compiler, and not by the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
  #include "hc32_ll.h"
  extern uint32_t SystemCoreClock;
#endif
#define configENABLE_FPU                          0 
#define configENABLE_MPU                          0
#define configUSE_PREEMPTION                      1 /* 1: ��ռʽ������, 0: Э��ʽ������, ��Ĭ���趨�� */
#define configSUPPORT_STATIC_ALLOCATION           0 /* 1: ֧�־�̬�����ڴ�, Ĭ��: 0 */
#define configSUPPORT_DYNAMIC_ALLOCATION          1 /* 1: ֧�ֶ�̬�����ڴ�, Ĭ��: 1 */
#define configUSE_IDLE_HOOK                       0 //���й��Ӻ�������ѡ����붨��void vApplicationIdleHook( void )
#define configUSE_TICK_HOOK                       0 /* 1: ʹ��ϵͳʱ�ӽ����жϹ��Ӻ���, ��Ĭ���趨�� */
#define configCPU_CLOCK_HZ                        ( SystemCoreClock )//ϵͳ��Ƶ��һ��SystemCoreClock���ϵͳ��Ƶ��ȡ���ͬʱֱ��������Ƶ������λHz
#define configTICK_RATE_HZ                        ((TickType_t)1000)//��઼�ʱƵ�ʣ�1ms����һ��
#define configMAX_PRIORITIES                      ( 7 )//������ȼ���-1��0Ϊ������ȼ�
#define configMINIMAL_STACK_SIZE                  ((uint16_t)128)//idle��ջ��С ��������������ջ�ռ��С, ��λ: Word, ��Ĭ���趨��
#define configTOTAL_HEAP_SIZE                     ((size_t)15360)//ջ�ռ��С FreeRTOS���п��õ�RAM����, ��λ: Byte, ��Ĭ���趨��
#define configMAX_TASK_NAME_LEN                   ( 16 ) /* ��������������ַ���, Ĭ��: 16 */
#define configUSE_16_BIT_TICKS                    0  /* 1: ����ϵͳʱ�ӽ��ļ���������������Ϊ16λ�޷�����, ��Ĭ���趨�� */
#define configUSE_MUTEXES                         1  /* 1: ʹ�ܻ����ź���, Ĭ��: 0 */
#define configQUEUE_REGISTRY_SIZE                 8  /* �������ע����ź�������Ϣ���еĸ���, Ĭ��: 0 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION   1 //�ܹ��Ż� ��ʹ��Ӳ��������һ��Ҫ���е�����, 0: ʹ�������㷨������һ��Ҫ���е�����, Ĭ��: 0 
#define configMESSAGE_BUFFER_LENGTH_TYPE          size_t /* ������Ϣ����������Ϣ���ȵ���������, Ĭ��: size_t */
#define configUSE_CO_ROUTINES                     0  /* 1: ����Э��, Ĭ��: 0 */
#define configMAX_CO_ROUTINE_PRIORITIES           ( 2 )  /* ����Э�̵�������ȼ�, ������ȼ�=configMAX_CO_ROUTINE_PRIORITIES-1, ��Ĭ��configUSE_CO_ROUTINESΪ1ʱ�趨�� */
#define INCLUDE_vTaskPrioritySet                  1  /* �����������ȼ� */
#define INCLUDE_uxTaskPriorityGet                 1  /* ��ȡ�������ȼ� */
#define INCLUDE_vTaskDelete                       1  /* ɾ������ */
#define INCLUDE_vTaskCleanUpResources             0
#define INCLUDE_vTaskSuspend                      1  /* �������� */
#define INCLUDE_vTaskDelayUntil                   1  /* ���������ʱ */
#define INCLUDE_vTaskDelay                        1  /* ������ʱ */
#define INCLUDE_xTaskGetSchedulerState            1  /* ��ȡ���������״̬ */
#define INCLUDE_xTimerPendFunctionCall            1  /* ��������ִ�йҵ���ʱ���������� */
#define configUSE_COUNTING_SEMAPHORES             1  /* ���ü����ź��� */

/* ������ʱ����ض��� */
#define configUSE_TIMERS                          1                               /* 1: ʹ��������ʱ��, Ĭ��: 0 */
#define configTIMER_TASK_PRIORITY                 ( configMAX_PRIORITIES - 1 )    /* ����������ʱ����������ȼ�, ��Ĭ��configUSE_TIMERSΪ1ʱ�趨�� */
#define configTIMER_QUEUE_LENGTH                  5                               /* ����������ʱ��������еĳ���, ��Ĭ��configUSE_TIMERSΪ1ʱ�趨�� */
#define configTIMER_TASK_STACK_DEPTH              ( configMINIMAL_STACK_SIZE * 2) /* ����������ʱ�������ջ�ռ��С, ��Ĭ��configUSE_TIMERSΪ1ʱ�趨�� */

/* Cortex-M specific definitions.�ж�Ƕ����Ϊ���� */
#ifdef __NVIC_PRIO_BITS
 /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
 #define configPRIO_BITS                          __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS                          4
#endif
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY       15                                                                        /* �ж�������ȼ� */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  5                                                                         /* FreeRTOS�ɹ���������ж����ȼ� */
#define configKERNEL_INTERRUPT_PRIORITY 		          ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )      /* �ں��жϵ����ȼ� */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	        ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) ) /* FreeRTOS �ɹ���������ж����ȼ� */
/* USER CODE BEGIN 1 */
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}
/* USER CODE END 1 */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler
/* USER CODE END Defines */
#endif
