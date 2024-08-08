
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __VARIABLE_H
#define __VARIABLE_H

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "hc32_ll.h"
#include "ev_hc32f460_lqfp100_v2_bsp.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

/* 图形库头文件 */
#include "u8g2.h"

/* bsp 头文件 */
#include "bsp_spi.h"
#include "button.h"

/* freertos 头文件 */
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* 宏定义 ------------------------------------------------------------------*/
#define KEYIN1_PORT             (GPIO_PORT_E)
#define KEYIN1_PIN              (GPIO_PIN_04)
#define KEYIN2_PORT             (GPIO_PORT_E)
#define KEYIN2_PIN              (GPIO_PIN_05)
#define KEY_BUTTON_1            1
#define KEY_BUTTON_2            2
#define BOTTON_TIC_MS           3     // 按键扫描心跳速度(单位:ms)

// 初始化页面时等待的事件标志组位
#define WAIT_BATTERY_CHECK      0x01
#define WAIT_STORAGE_CHECK      0x02
#define WAIT_CLOCK_CHECK        0x03
#define WAIT_PROBE_CHECK        0x04
#define WAIT_PROBE_HV_CHECK     0x05
#define WAIT_PROBE_STORAGE_CHECK 0x06
#define WAIT_GJ6401_CHECK       0x06
#define WAIT_J405_CHECK         0x07

// TMR0定时器配置
#define TMR0_UNIT               (CM_TMR0_1)
#define TMR0_CLK                (FCG2_PERIPH_TMR0_1)
#define TMR0_CH                 (TMR0_CH_B)
#define TMR0_CH_INT             (TMR0_INT_CMP_B)
#define TMR0_CH_FLAG            (TMR0_FLAG_CMP_B)
#define TMR0_INT_SRC            (INT_SRC_TMR0_1_CMP_B)
#define TMR0_IRQn               (INT007_IRQn)
#define TMR0_CLK_DIV            (TMR0_CLK_DIV2)
#define TMR0_CMP_VALUE          ((XTAL32_VALUE / 2U / (1000U / 3U)) - 1U)

/* 联合体 ------------------------------------------------------------------*/
union Float_T4o_4Byte // 4字节转float专用联合体
{
    unsigned char array[4];
    float buff;
};

/* 联合体 ------------------------------------------------------------------*/


/* 全局变量 ----------------------------------------------------------------*/
/* freertos 任务句柄 */
extern TaskHandle_t AppTaskCreate_Handle;    // 创建其他任务的任务句柄
extern TaskHandle_t AstraTask_Handle;        // Astra_ui任务句柄
extern TaskHandle_t LEDTask_Handle;          // LED任务句柄
extern TaskHandle_t BTNTask_Handle;          // 按键任务句柄

/* 软件定时器句柄 */
// extern TimerHandle_t btn_tic_taskHandle;

/* 创建事件标志组句柄 */
extern EventGroupHandle_t xInit_EventGroup;

// 电子纸显示的图片数组
extern const unsigned char *epd_bitmap_allArray[29]; 

/* 函数声明 ----------------------------------------------------------------*/

void BSP_Init(void);
void TEST_KEY_GPIO_Init(void);
uint8_t btn_read_level (uint8_t io);
void TrngConfig(void);
void TMR0_Config(void);

#ifdef __cplusplus
}
#endif


#endif

