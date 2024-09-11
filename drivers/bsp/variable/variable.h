
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
#include "bsp_pwm.h"
#include "bsp_usart.h"
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

/* components 头文件 */
#include "key_value_transation.h"

/* 宏定义 ------------------------------------------------------------------*/

// 按键初始化
#define KEYIN1_PORT (GPIO_PORT_B)
#define KEYIN1_PIN (GPIO_PIN_15)
#define KEYIN2_PORT (GPIO_PORT_B)
#define KEYIN2_PIN (GPIO_PIN_13)
#define KEYIN3_PORT (GPIO_PORT_B)
#define KEYIN3_PIN (GPIO_PIN_14)
#define KEYIN4_PORT (GPIO_PORT_B)
#define KEYIN4_PIN (GPIO_PIN_12)
#define KEY_BUTTON_1 1
#define KEY_BUTTON_2 2
#define KEY_BUTTON_3 3
#define KEY_BUTTON_4 4
#define BOTTON_TIC_MS 3 // 按键扫描心跳速度(单位:ms)

// 蜂鸣器初始化
#define BEEP_PORT (GPIO_PORT_D)
#define BEEP_PIN (GPIO_PIN_15)
#define BEEP_ON() GPIO_SetPins(BEEP_PORT, BEEP_PIN)
#define BEEP_OFF() GPIO_ResetPins(BEEP_PORT, BEEP_PIN)
#define BEEP_TOGGLE() GPIO_TogglePins(BEEP_PORT, BEEP_PIN)

/* 初始化页面时等待的事件标志组位 -> xInit_EventGroup事件标志组 */
// 定时器消除的连接标志位
#define WAIT_PROBE1_CONNECT_CHECK (1 << 0)
#define WAIT_PROBE2_CONNECT_CHECK (1 << 1)
#define WAIT_PROBE3_CONNECT_CHECK (1 << 2)
#define WAIT_PROBE4_CONNECT_CHECK (1 << 3)
// HV状态标志位
#define WAIT_PROBE1_HV_CHECK (1 << 4)
#define WAIT_PROBE2_HV_CHECK (1 << 5)
#define WAIT_PROBE3_HV_CHECK (1 << 6)
#define WAIT_PROBE4_HV_CHECK (1 << 7)
// 计数管自检标志位
#define WAIT_PROBE1_COUNT_CHECK (1 << 8)
#define WAIT_PROBE2_COUNT_CHECK (1 << 9)
#define WAIT_PROBE3_COUNT_CHECK (1 << 10)
#define WAIT_PROBE4_COUNT_CHECK (1 << 11)

/* 探头数据发送及蜂鸣器处理及探头状态事件标志位 -> xProbeDataSendEventGroup事件标志组 */
// 判断当前界面标志位
#define CURRENT_INTERFACE_FLAG_CIRCLE (1 << 0)
#define CURRENT_INTERFACE_FLAG_TREE (1 << 1)
#define CURRENT_INTERFACE_FLAG_ANOTHER (1 << 2)
#define BEEP_FLAG_CHECK (1 << 3)
// 探头1状态标志位
#define PROBE1_ALARM_FLAG (1 << 4)
#define PROBE1_POPUP_FLAG (1 << 5)
// 探头2状态标志位
#define PROBE2_ALARM_FLAG (1 << 6)
#define PROBE2_POPUP_FLAG (1 << 7)
// 探头3状态标志位
#define PROBE3_ALARM_FLAG (1 << 8)
#define PROBE3_POPUP_FLAG (1 << 9)
// 探头4状态标志位
#define PROBE4_ALARM_FLAG (1 << 10)
#define PROBE4_POPUP_FLAG (1 << 11)

// 所有探头连接检查
#define ALL_CONNECT_CHECK (WAIT_PROBE1_CONNECT_CHECK | WAIT_PROBE2_CONNECT_CHECK)
// 所有探头HV检查
#define ALL_HV_CHECK (WAIT_PROBE1_HV_CHECK | WAIT_PROBE2_HV_CHECK)
// 所有探头计数管自检
#define ALL_COUNT_CHECK (WAIT_PROBE1_COUNT_CHECK | WAIT_PROBE2_COUNT_CHECK)

/* 探头连接状态事件标志位 -> xProbeConnectionEventGroup事件标志组 */
// 探头1-4当前连接状态
#define PROBE1_CURRENT_CONNECT_STATUS (1 << 0)
#define PROBE2_CURRENT_CONNECT_STATUS (1 << 1)
#define PROBE3_CURRENT_CONNECT_STATUS (1 << 2)
#define PROBE4_CURRENT_CONNECT_STATUS (1 << 3)

// 探头1-4连接与否的标志位
#define PROBE1_CONNECTED (1 << 4)
#define PROBE2_CONNECTED (1 << 5)
#define PROBE3_CONNECTED (1 << 6)
#define PROBE4_CONNECTED (1 << 7)

// 探头1-4断线标志位
#define PROBE1_DISCONNECTED (1 << 8)
#define PROBE2_DISCONNECTED (1 << 9)
#define PROBE3_DISCONNECTED (1 << 10)
#define PROBE4_DISCONNECTED (1 << 11)

// 探头1-4重连标志位
#define PROBE1_RECONNECTED (1 << 12)
#define PROBE2_RECONNECTED (1 << 13)
#define PROBE3_RECONNECTED (1 << 14)
#define PROBE4_RECONNECTED (1 << 15)

// TMR0定时器配置
#define TMR0_UNIT (CM_TMR0_1)
#define TMR0_CLK (FCG2_PERIPH_TMR0_1)
#define TMR0_CH (TMR0_CH_B)
#define TMR0_CH_INT (TMR0_INT_CMP_B)
#define TMR0_CH_FLAG (TMR0_FLAG_CMP_B)
#define TMR0_INT_SRC (INT_SRC_TMR0_1_CMP_B)
#define TMR0_IRQn (INT007_IRQn)
#define TMR0_CLK_DIV (TMR0_CLK_DIV2)
#define TMR0_CMP_VALUE ((XTAL32_VALUE / 2U / (1000U / 3U)) - 1U)

// EEPROM 基地址
#define EEPROM_BASE_ADDR (0x0000)
// 定义一个宏来简化EEPROM的读写操作
#define EEPROM_WRITE(var, member) writeMemberToEEPROM(EEPROM_BASE_ADDR, &(var), offsetof(ProbeData, member), sizeof((var).member))
#define EEPROM_READ(var, member) readMemberFromEEPROM(EEPROM_BASE_ADDR, &(var), offsetof(ProbeData, member), sizeof((var).member))

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
        uint8_t probeNumber;  // 探头编号
        bool p;               // 是否有p报警
        bool d;               // 是否有d报警
        float doseRate;       // 剂量率
        float cumulativeDose; // 累计剂量
    } WarningUpdateMessage;

    typedef struct
    {
        float probe1_cumulative_dose;            // 探头1累计剂量
        float probe2_cumulative_dose;            // 探头2累计剂量
        float probe3_cumulative_dose;            // 探头3累计剂量
        float probe4_cumulative_dose;            // 探头4累计剂量
        float probe1_realtime_dose;              // 探头1实时剂量
        float probe2_realtime_dose;              // 探头2实时剂量
        float probe3_realtime_dose;              // 探头3实时剂量
        float probe4_realtime_dose;              // 探头4实时剂量
        float probe1_cumulative_alarm_threshold; // 探头1累计剂量报警阈值
        float probe2_cumulative_alarm_threshold; // 探头2累计剂量报警阈值
        float probe3_cumulative_alarm_threshold; // 探头3累计剂量报警阈值
        float probe4_cumulative_alarm_threshold; // 探头4累计计量报警阈值
        float probe1_realtime_alarm_threshold;   // 探头1实时剂量报警阈值
        float probe2_realtime_alarm_threshold;   // 探头2实时剂量报警阈值
        float probe3_realtime_alarm_threshold;   // 探头3实时剂量报警阈值
        float probe4_realtime_alarm_threshold;   // 探头4实时剂量报警阈值
    } ProbeData;

    typedef struct
    {
        bool connected;
        TickType_t lastUpdateTime;
    } ProbeStatus;
    /* 全局变量 ----------------------------------------------------------------*/
    /*-------------------- FreeRTOS --------------------*/
    /* freertos 任务句柄 */
    extern TaskHandle_t AppTaskCreate_Handle; // 创建其他任务的任务句柄
    extern TaskHandle_t AstraTask_Handle;     // Astra_ui任务句柄
    extern TaskHandle_t CanTxTask_Handle;     // CAN发送任务句柄
    extern TaskHandle_t CanRxTask_Handle;     // CAN接收任务句柄
    extern TaskHandle_t UsartTask_Handle;     // 串口任务句柄
    /* freertos 队列句柄 */
    extern QueueHandle_t xQueue_CanTx;             // CAN发送队列
    extern QueueHandle_t xQueue_ProbeInfoTransfer; // 探头信息队列
    /* freertos 事件标志组句柄 */
    extern EventGroupHandle_t xInit_EventGroup;         // 初始化事件标志组
    extern EventGroupHandle_t xProbeDataSendEventGroup; // 根据界面判断是否发送探头数据事件标志组
    /* freertos 软件定时器句柄 */
    extern TimerHandle_t xDoseRateTimer;    // 提醒探头上传剂量率和累积剂量软件定时器句柄
    extern TimerHandle_t xBeepTimer;        // 蜂鸣器控制软件定时器句柄
    extern TimerHandle_t xProbe1AlarmTimer; // 探头1报警软件定时器句柄
    extern TimerHandle_t xProbe2AlarmTimer; // 探头2报警软件定时器句柄
    extern TimerHandle_t xProbe3AlarmTimer; // 探头3报警软件定时器句柄
    extern TimerHandle_t xProbe4AlarmTimer; // 探头4报警软件定时器句柄
    extern TimerHandle_t xPopInfoTimer;     // popinfo 弹窗倒计时定时器句柄

    /*-------------------- EEPROM --------------------*/
    extern ProbeData data;

    /*-------------------- 标志位 --------------------*/
    extern bool g_bUsartInitialized; // 串口初始化完成标志位

    /*--------------------- 串口 ---------------------*/
    extern uint8_t *m_au8RxBuf; // 串口接收缓冲区

    /*------------------- 探头状态 -------------------*/
    extern ProbeStatus Gloabal_ProbeStatus[2];

    /* 函数声明 ----------------------------------------------------------------*/

    // 初始化函数
    uint8_t *init_dynamic_buffer(uint8_t **buffer, size_t size);
    void formatFloat(float value, char *buffer, size_t bufferSize);
    void BSP_Init(void);
    void TEST_KEY_GPIO_Init(void);
    void BEEP_GPIO_Init(void);
    uint8_t btn_read_level(uint8_t io);
    void TrngConfig(void);
    void TMR0_Config(void);

    // FreeRTOS任务及定时器回调函数
    void CanTxTask(void *pvParameters);
    void CANRxTask(void *pvParameters);
    void vDoseRateTimerCallback(TimerHandle_t xTimer);
    void vBeepTimerCallback(TimerHandle_t xTimer);
    void vProbe1AlarmTimerCallback(TimerHandle_t xTimer);
    void vProbe2AlarmTimerCallback(TimerHandle_t xTimer);
    void vProbe3AlarmTimerCallback(TimerHandle_t xTimer);
    void vProbe4AlarmTimerCallback(TimerHandle_t xTimer);
    void vPopInfoTimerCallback(TimerHandle_t xTimer);

    // 修改软件定时器的触发时间
    BaseType_t modifyTimerPeriod(TimerHandle_t xTimer, TickType_t newPeriod);

    // EEPROM读写函数
    int32_t writeMemberToEEPROM(uint16_t baseAddr, const ProbeData *sensor, size_t memberOffset, size_t memberSize);
    int32_t readMemberFromEEPROM(uint16_t baseAddr, ProbeData *sensor, size_t memberOffset, size_t memberSize);

    // 键值对注册函数
    void Key_Value_Init(void);

#ifdef __cplusplus
}
#endif

#endif
