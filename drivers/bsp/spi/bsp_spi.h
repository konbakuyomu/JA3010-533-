 #ifdef __cplusplus
extern "C" {
#endif

#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include <string.h>
#include "stdlib.h"
#include "hc32_ll.h"

/******************************宏定义*******************************/

#define SPI_TIME_OUT            (0x40000UL)
#define SPI_WORD_1              (1UL)
#define SPI_WORD_2              (2UL)
#define SPI_BUF_LEN             (128UL)

/* TPC112S8的SPI定义 */
#define SPI_MASTER_SLAVE        (SPI_MASTER)
#define SPI_UNIT                (CM_SPI1)
#define SPI_CLK                 (FCG1_PERIPH_SPI1)
#define SPI_TX_EVT_SRC          (EVT_SRC_SPI1_SPTI)
#define SPI_TX_FLAG             (SPI_FLAG_IDLE) 

/* 通信引脚配置 */
/* SS = PA7 */
#define SPI_SS_PORT             (GPIO_PORT_A)
#define SPI_SS_PIN              (GPIO_PIN_07)
#define SPI_SS_FUNC             (GPIO_FUNC_42)
/* SCK = PA8 */
#define SPI_SCK_PORT            (GPIO_PORT_A)
#define SPI_SCK_PIN             (GPIO_PIN_08)
#define SPI_SCK_FUNC            (GPIO_FUNC_43)
/* MOSI = PB0 */
#define SPI_MOSI_PORT           (GPIO_PORT_B)
#define SPI_MOSI_PIN            (GPIO_PIN_00)
#define SPI_MOSI_FUNC           (GPIO_FUNC_40)
/* DC = PB2 */
#define SPI_DC_PORT             (GPIO_PORT_B)
#define SPI_DC_PIN              (GPIO_PIN_02)
#define SPI_DC_CMD()             GPIO_ResetPins(SPI_DC_PORT, SPI_DC_PIN)
#define SPI_DC_DATA()            GPIO_SetPins(SPI_DC_PORT, SPI_DC_PIN)
/* RST = PE8 */
#define SPI_RST_PORT            (GPIO_PORT_E)
#define SPI_RST_PIN             (GPIO_PIN_08)
#define SPI_RST_SET()           GPIO_SetPins(SPI_RST_PORT, SPI_RST_PIN)
#define SPI_RST_RESET()         GPIO_ResetPins(SPI_RST_PORT, SPI_RST_PIN)

/* DMA definition */
#define DMA_UNIT                (CM_DMA1)
#define DMA_CLK                 (FCG0_PERIPH_DMA1 | FCG0_PERIPH_AOS)
#define DMA_TX_CH               (DMA_CH0)
#define DMA_TX_TRIG_CH          (AOS_DMA1_0)
#define DMA_TX_INT_CH           (DMA_INT_TC_CH0)
#define DMA_TX_INT_SRC          (INT_SRC_DMA1_TC0)
#define DMA_TX_IRQ_NUM          (INT006_IRQn)

#define LCD_SCL_CLR()	GPIO_ResetPins(SPI_SCK_PORT,SPI_SCK_PIN)
#define LCD_SCL_SET()	GPIO_SetPins(SPI_SCK_PORT,SPI_SCK_PIN)

#define LCD_SDA_CLR()	GPIO_ResetPins(SPI_MOSI_PORT,SPI_MOSI_PIN)
#define LCD_SDA_SET()	GPIO_SetPins(SPI_MOSI_PORT,SPI_MOSI_PIN)

#define LCD_RST_CLR()	GPIO_ResetPins(SPI_RST_PORT,SPI_RST_PIN)
#define LCD_RST_SET()	GPIO_SetPins(SPI_RST_PORT,SPI_RST_PIN)

#define LCD_DC_CLR()	GPIO_ResetPins(SPI_DC_PORT,SPI_DC_PIN)
#define LCD_DC_SET()	GPIO_SetPins(SPI_DC_PORT,SPI_DC_PIN)

/******************************变量定义*******************************/

/******************************函数定义*******************************/

#ifdef __cplusplus
inline
#else
static inline
#endif
void WaitForSPITxBufferEmpty(void)
{
    while (SPI_GetStatus(SPI_UNIT, SPI_FLAG_TX_BUF_EMPTY) == RESET)
    {
        // 可以添加超时处理
    }
}

/**
 * @brief  SPI初始化
 * @param None   
 * @return None
 */
void SPI_Config(void);

/**
 * @brief  SPI 启用 DMA 发送
 * @param  None
 * @retval None
 */
void DMA_SPI_Send(void *TxBuf, uint16_t TxLen);

#endif

#ifdef __cplusplus
}
#endif
