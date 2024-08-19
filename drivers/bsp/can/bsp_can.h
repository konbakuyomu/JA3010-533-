#ifndef __BSP_CAN_H
#define __BSP_CAN_H

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

#include "hc32_ll.h"

/******************************宏定义*******************************/

#define CAN_UNIT            (CM_CAN)
#define CAN_PERIPH_CLK      (FCG1_PERIPH_CAN)

#define CAN_TX_PORT         (GPIO_PORT_B)
#define CAN_TX_PIN          (GPIO_PIN_07)
#define CAN_TX_PIN_FUNC     (GPIO_FUNC_50)

#define CAN_RX_PORT         (GPIO_PORT_B)
#define CAN_RX_PIN          (GPIO_PIN_06)
#define CAN_RX_PIN_FUNC     (GPIO_FUNC_51)

#define CAN_INT_PRIO        (DDL_IRQ_PRIO_DEFAULT)
#define CAN_INT_SRC         (INT_SRC_CAN_INT)
#define CAN_INT_IRQn        (INT122_IRQn)

/* CAN interrupt type selection. */
#define CAN_INT_SEL         (CAN_INT_PTB_TX | CAN_INT_RX)

/* Acceptance filter. */
#define CAN_FILTER_SEL      (CAN_FILTER1 | CAN_FILTER2 | CAN_FILTER3 | CAN_FILTER4)
#define CAN_FILTER_NUM      (4U)

#define CAN_FILTER1_ID      (0x40UL) // 探头1 ID
#define CAN_FILTER1_ID_MASK (0x0UL)
#define CAN_FILTER1_ID_TYPE (CAN_ID_STD_EXT) /*接受标准帧和扩展帧*/

#define CAN_FILTER2_ID      (0x41UL) // 探头2 ID
#define CAN_FILTER2_ID_MASK (0x0UL)
#define CAN_FILTER2_ID_TYPE (CAN_ID_STD_EXT)

#define CAN_FILTER3_ID      (0x42UL) // 探头3 ID
#define CAN_FILTER3_ID_MASK (0x0UL)
#define CAN_FILTER3_ID_TYPE (CAN_ID_STD_EXT)

#define CAN_FILTER4_ID      (0x43UL) // 探头4 ID
#define CAN_FILTER4_ID_MASK (0x0UL)
#define CAN_FILTER4_ID_TYPE (CAN_ID_STD_EXT)

/* 发送命令定义 */
#define CAN_TX_SELF_CHECK               (0x41UL)    // 自检命令
#define CAN_TX_CLEAR_CUMULATIVE_DOSE    (0x42UL)    // 累计剂量清零命令
#define CAN_TX_SET_DOSE_RATE_ALARM      (0x43UL)    // 剂量率报警阈值设置命令
#define CAN_TX_UPDATE_DOSE_RATE_ALARM   (0x44UL)    // 上传剂量报警阈值命令
#define CAN_TX_UPDATE_DOSE_RATE         (0x45UL)    // 上传剂量率和累计剂量命令

/* CAN发送相关定义 */
#define CAN_TX_DLC                      (CAN_DLC8)  // CAN发送数据长度代码，设置为8字节

/* Number of RX frame */
#define CAN_RX_FRAME_NUM                (50U)       // 接收帧数

/*************************************************************/
void CAN_Drv_Init(void);
void CanPinConfig(void);
void CanInitConfig(void);
void CanIrqConfig(void);

#ifdef __cplusplus
}
#endif

#endif
