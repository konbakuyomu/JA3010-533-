#ifndef __DRV_CAN_H
#define __DRV_CAN_H

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

#include "hc32_ll.h"

/******************************宏定义*******************************/

#define CAN_RX_DATA_TYPE_SELF_CHECK_HIGH_VOLTAGE_NORMAL         (0xF0)  // 接收数据:自检高压正常
#define CAN_RX_DATA_TYPE_SELF_CHECK_HIGH_VOLTAGE_ABNORMAL       (0xF1)  // 接收数据:自检高压异常
#define CAN_RX_DATA_TYPE_SELF_CHECK_LOW_RANGE_COUNTER_NORMAL    (0xF0)  // 接收数据:自检低量程计数管正常
#define CAN_RX_DATA_TYPE_CLEAR_CUMULATIVE_DOSE_SUCCESS          (0x00)  // 接收数据:累计剂量清零成功
#define CAN_RX_DATA_TYPE_CLEAR_CUMULATIVE_DOSE_FAIL             (0x01)  // 接收数据:累计剂量清零失败

#define CAN_TX_BASE_DATA                                        (0xA1)  // 本底数据帧头
#define CAN_TX_SELF_POLLUTION_DATA                              (0xA2)  // 自污染检测数据帧头
#define CAN_TX_PERSON_DETECTION_DATA                            (0xA3)  // 人员检测数据帧头
#define CAN_TX_NORMAL_DATA                                      (0xA4)  // 正常数据帧头


/******************************函数定义*******************************/
void CAN_IrqCallback(void);
void CAN_ProcessReceivedData(void);
void ProcessCANFrame(stc_can_rx_frame_t Rx_Data);
void CAN_SendSelfCheckCommand(uint8_t u8ProbeID);
void CAN_SendClearCumulativeDoseCommand(uint8_t u8ProbeID);
void CAN_SendDoseRateAlarmThresholdCommand(uint8_t u8ProbeID, float f32DoseRateAlarmThreshold);
void CAN_SendUploadDoseRateAlarmThresholdCommand(uint8_t u8ProbeID);
void CAN_SendUploadDoseRateAndCumulativeDoseCommand(uint8_t u8ProbeID);

#ifdef __cplusplus
}
#endif

#endif
