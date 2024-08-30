#ifndef __BSP_USART_H
#define __BSP_USART_H

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include "stdlib.h"
#include "hc32_ll.h"

/********************************DMA*********************************/
#define RX_DMA_UNIT                     (CM_DMA2)
#define RX_DMA_CH                       (DMA_CH0)
#define RX_DMA_TRIG_SEL                 (AOS_DMA2_0)
#define RX_DMA_TRIG_EVT_SRC             (EVT_SRC_USART3_RI)
#define RX_DMA_RECONF_TRIG_SEL          (AOS_DMA_RC)
#define RX_DMA_RECONF_TRIG_EVT_SRC      (EVT_SRC_AOS_STRG)
#define RX_DMA_TC_INT                   (DMA_INT_TC_CH0)
#define RX_DMA_TC_FLAG                  (DMA_FLAG_TC_CH0)
#define RX_DMA_TC_IRQn                  (INT039_IRQn)
#define RX_DMA_TC_INT_SRC               (INT_SRC_DMA2_TC0)

#define TX_DMA_UNIT                     (CM_DMA2)
#define TX_DMA_CH                       (DMA_CH1)
#define TX_DMA_TRIG_SEL                 (AOS_DMA2_1)
#define TX_DMA_TRIG_EVT_SRC             (EVT_SRC_USART3_TI)
#define TX_DMA_TC_INT                   (DMA_INT_TC_CH1)
#define TX_DMA_TC_FLAG                  (DMA_FLAG_TC_CH1)
#define TX_DMA_TC_IRQn                  (INT038_IRQn)
#define TX_DMA_TC_INT_SRC               (INT_SRC_DMA2_TC1)

/******************************超时TMR0*******************************/
#define TMR0_TIMEOUT_UNIT               (CM_TMR0_2)
#define TMR0_TIMEOUT_CH                 (TMR0_CH_A)
#define TMR0_FCG_ENABLE()               (FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMR0_2, ENABLE))

/********************************串口*********************************/
#define USART_BAUDRATE                  (9600UL)

/* USART RX/TX/Control  */
#define USART_RX_PORT                   (GPIO_PORT_E)   /* PE2: USART3_RX */
#define USART_RX_PIN                    (GPIO_PIN_02)
#define USART_RX_GPIO_FUNC              (GPIO_FUNC_33)
#define USART_TX_PORT                   (GPIO_PORT_E)   /* PE4: USART3_TX */
#define USART_TX_PIN                    (GPIO_PIN_04)
#define USART_TX_GPIO_FUNC              (GPIO_FUNC_32)
#define CONTROL_PORT                    (GPIO_PORT_E)   
#define CONTROL_PIN                     (GPIO_PIN_05)    
#define CONTROL_TX()                    (GPIO_ResetPins(CONTROL_PORT, CONTROL_PIN))
#define CONTROL_RX()                    (GPIO_SetPins(CONTROL_PORT, CONTROL_PIN))

/* USART unit definition */
#define USART_UNIT                      (CM_USART3)
#define USART_FCG_ENABLE()              (FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART3, ENABLE))

/* USART timeout bits definition */
#define USART_TIMEOUT_BITS              (2000U)

/* USART interrupt definition */
#define USART_TX_CPLT_IRQn              (INT086_IRQn)
#define USART_TX_CPLT_INT_SRC           (INT_SRC_USART3_TCI) // USART3串口传输完成中断
#define USART_RX_ERR_IRQn               (INT087_IRQn)
#define USART_RX_ERR_INT_SRC            (INT_SRC_USART3_EI)  // USART3串口接收错误中断
#define USART_RX_TIMEOUT_IRQn           (INT088_IRQn)
#define USART_RX_TIMEOUT_INT_SRC        (INT_SRC_USART3_RTO) // USART3串口接收超时中断

#define APP_FRAME_LEN_MAX               (50U)                // 串口接收数组长度

/******************************函数定义*******************************/
void USART_Config(void);
void USART_Send(uint8_t *pData, uint16_t u16Len);

#ifdef __cplusplus
}
#endif

#endif

