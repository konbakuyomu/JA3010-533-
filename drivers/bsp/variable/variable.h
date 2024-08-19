
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __VARIABLE_H
#define __VARIABLE_H

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include "hc32_ll.h"
#include "ev_hc32f460_lqfp100_v2_bsp.h"

/* 内存池管理头文件 */
#include "malloc.h"

/* 图形库头文件 */
#include "u8g2.h"

/* bsp 头文件 */
#include "bsp_spi.h"
#include "bsp_can.h"
#include "button.h"

/* drv 头文件 */
#include "drv_can.h"

/* freertos 头文件 */
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* 宏定义 ------------------------------------------------------------------*/
#define KEYIN1_PORT                (GPIO_PORT_E)
#define KEYIN1_PIN                 (GPIO_PIN_04)
#define KEYIN2_PORT                (GPIO_PORT_E)
#define KEYIN2_PIN                 (GPIO_PIN_05)
#define KEY_BUTTON_1               1
#define KEY_BUTTON_2               2
#define BOTTON_TIC_MS              3 // 按键扫描心跳速度(单位:ms)

// 初始化页面时等待的事件标志组位
// 连接状态标志位
#define WAIT_PROBE1_CONNECT_CHECK  (1 << 0)
#define WAIT_PROBE2_CONNECT_CHECK  (1 << 1)
#define WAIT_PROBE3_CONNECT_CHECK  (1 << 2)
#define WAIT_PROBE4_CONNECT_CHECK  (1 << 3)
// HV状态标志位
#define WAIT_PROBE1_HV_CHECK       (1 << 4)
#define WAIT_PROBE2_HV_CHECK       (1 << 5)
#define WAIT_PROBE3_HV_CHECK       (1 << 6)
#define WAIT_PROBE4_HV_CHECK       (1 << 7)
// 计数管自检标志位
#define WAIT_PROBE1_COUNT_CHECK    (1 << 8)
#define WAIT_PROBE2_COUNT_CHECK    (1 << 9)
#define WAIT_PROBE3_COUNT_CHECK    (1 << 10)
#define WAIT_PROBE4_COUNT_CHECK    (1 << 11)
// 所有探头连接检查
#define ALL_CONNECT_CHECK          (WAIT_PROBE1_CONNECT_CHECK | WAIT_PROBE2_CONNECT_CHECK | WAIT_PROBE3_CONNECT_CHECK | WAIT_PROBE4_CONNECT_CHECK)
// 所有探头HV检查
#define ALL_HV_CHECK               (WAIT_PROBE1_HV_CHECK | WAIT_PROBE2_HV_CHECK | WAIT_PROBE3_HV_CHECK | WAIT_PROBE4_HV_CHECK)
// 所有探头计数管自检
#define ALL_COUNT_CHECK            (WAIT_PROBE1_COUNT_CHECK | WAIT_PROBE2_COUNT_CHECK | WAIT_PROBE3_COUNT_CHECK | WAIT_PROBE4_COUNT_CHECK)

// TMR0定时器配置
#define TMR0_UNIT                  (CM_TMR0_1)
#define TMR0_CLK                   (FCG2_PERIPH_TMR0_1)
#define TMR0_CH                    (TMR0_CH_B)
#define TMR0_CH_INT                (TMR0_INT_CMP_B)
#define TMR0_CH_FLAG               (TMR0_FLAG_CMP_B)
#define TMR0_INT_SRC               (INT_SRC_TMR0_1_CMP_B)
#define TMR0_IRQn                  (INT007_IRQn)
#define TMR0_CLK_DIV               (TMR0_CLK_DIV2)
#define TMR0_CMP_VALUE             ((XTAL32_VALUE / 2U / (1000U / 3U)) - 1U)

// EEPROM 基地址
#define EEPROM_BASE_ADDR           (0x0000)
// 定义一个宏来简化EEPROM的读写操作
#define EEPROM_WRITE(var, member)   writeMemberToEEPROM(EEPROM_BASE_ADDR, &(var), offsetof(ProbeData, member), sizeof((var).member))
#define EEPROM_READ(var, member)    readMemberFromEEPROM(EEPROM_BASE_ADDR, &(var), offsetof(ProbeData, member), sizeof((var).member))

/* 联合体 ------------------------------------------------------------------*/
    typedef union
    {
        float f;
        uint32_t i;
    } FloatIntUnion;

    union Float_T4o_4Byte // 4字节转float专用联合体
    {
        unsigned char array[4];
        float buff;
    };

    /* 结构体 ------------------------------------------------------------------*/
    typedef struct
    {
        char *label;          // 标签
        bool p;               // 是否有p报警
        bool d;               // 是否有d报警
        float doseRate;       // 剂量率
        float cumulativeDose; // 累计剂量
    } WarningUpdateMessage;

    typedef struct
    {
        uint32_t probe1_cumulative_dose;
        uint32_t probe2_cumulative_dose;
        uint32_t probe3_cumulative_dose;
        uint32_t probe4_cumulative_dose;
        uint32_t probe1_alarm_threshold;
        uint32_t probe2_alarm_threshold;
        uint32_t probe3_alarm_threshold;
        uint32_t probe4_alarm_threshold;
    } ProbeData;
    /* 全局变量 ----------------------------------------------------------------*/
    /*-------------------- FreeRTOS --------------------*/
    /* freertos 任务句柄 */
    extern TaskHandle_t AppTaskCreate_Handle; // 创建其他任务的任务句柄
    extern TaskHandle_t AstraTask_Handle;     // Astra_ui任务句柄
    extern TaskHandle_t LEDTask_Handle;       // LED任务句柄
    extern TaskHandle_t BTNTask_Handle;       // 按键任务句柄
    extern TaskHandle_t CanTxTask_Handle;     // CAN发送任务句柄
    extern TaskHandle_t CanRxTask_Handle;     // CAN接收任务句柄
    /* freertos 队列句柄 */
    extern QueueHandle_t xQueue_CanTx;         // CAN发送队列
    extern QueueHandle_t xQueue_WarningUpdate; // 警告更新队列
    /* freertos 事件标志组句柄 */
    extern EventGroupHandle_t xInit_EventGroup; // 初始化事件标志组
    /* freertos 软件定时器句柄 */
    extern TimerHandle_t xDoseRateTimer; // 定时器服务任务句柄

    /*-------------------- EEPROM --------------------*/
    extern ProbeData data;

    /*-------------------- 图形库 --------------------*/
    extern const unsigned char *epd_bitmap_allArray[29];

    /* 函数声明 ----------------------------------------------------------------*/

    void BSP_Init(void);
    void TEST_KEY_GPIO_Init(void);
    uint8_t btn_read_level(uint8_t io);
    void TrngConfig(void);
    void TMR0_Config(void);

    // FreeRTOS任务及定时器回调函数
    void CanTxTask(void *pvParameters);
    void CANRxTask(void *pvParameters);
    void vDoseRateTimerCallback(TimerHandle_t xTimer);

    // EEPROM读写函数
    int32_t writeMemberToEEPROM(uint16_t baseAddr, const ProbeData *sensor, size_t memberOffset, size_t memberSize);
    int32_t readMemberFromEEPROM(uint16_t baseAddr, ProbeData *sensor, size_t memberOffset, size_t memberSize);

#ifdef __cplusplus
}
#endif

#endif
