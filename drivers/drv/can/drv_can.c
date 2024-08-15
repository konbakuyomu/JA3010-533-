#include "variable.h"

/******************************函数定义*******************************/

/***********************************************************************
 * @ 函数名  ： CAN_IrqCallback
 * @ 功能说明： CAN中断处理函数
 * @ 参数    ：
 * @ 返回值  ： 无
 *********************************************************************/
void CAN_IrqCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (CAN_GetStatus(CAN_UNIT, CAN_FLAG_PTB_TX) == SET)
	{
		CAN_ClearStatus(CAN_UNIT, CAN_FLAG_PTB_TX);
		CAN_IntCmd(CAN_UNIT, CAN_INT_PTB_TX, DISABLE);
	}

	if (CAN_GetStatus(CAN_UNIT, CAN_FLAG_RX) == SET)
	{
		CAN_ClearStatus(CAN_UNIT, CAN_FLAG_RX);
		// vTaskNotifyGiveFromISR(vCanRxHandle, &xHigherPriorityTaskWoken);
		CAN_IntCmd(CAN_UNIT, CAN_INT_RX, DISABLE);
	}

	CAN_ClearStatus(CAN_UNIT, CAN_FLAG_PTB_TX | CAN_FLAG_RX);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/***********************************************************************
 * @ 函数名  ： CAN_ProcessReceivedData
 * @ 功能说明： 接收CAN数据处理函数，1.CAN总线自检接收处理 2.正常CAN数据收发处理
 * @ 参数    ：
 * @ 返回值  ： 无
 *********************************************************************/
void CAN_ProcessReceivedData(void)
{
	uint8_t u8RxFrameNum = 0U;
	int32_t i32Ret;
	static stc_can_rx_frame_t m_astRxFrame[CAN_RX_FRAME_NUM];

	/* 从接收缓冲区获取接收到的数据帧，如果接收缓冲区有数据，就一直获取 */
	do
	{
		i32Ret = CAN_GetRxFrame(CAN_UNIT, &m_astRxFrame[u8RxFrameNum]); // 从接收缓冲区获取接收到的数据帧
		/* 如果输入的数据帧结构指针是NULL，函数将返回错误代码LL_ERR_INVD_PARAM */
		if (i32Ret == LL_OK)
		{
			u8RxFrameNum++;
			/* 如果接收缓冲区已满，将从第一个数据帧开始覆盖 */
			if (u8RxFrameNum >= CAN_RX_FRAME_NUM)
			{
				u8RxFrameNum = 0;
			}
		}
	} while (i32Ret == LL_OK); // 如果接收缓冲区有数据，就一直获取

	/* 处理接收到的帧. */
	uint8_t i = 0UL;
	while (i < u8RxFrameNum)
	{
		// 收到8字节即为正常收发数据
		if (m_astRxFrame[i].DLC == CAN_TX_DLC)
		{
			ProcessCANFrame(m_astRxFrame[i]);
		}
		i++;
	}
	CAN_IntCmd(CAN_UNIT, CAN_INT_RX, ENABLE);
}

/***********************************************************************
 * @ 函数名  ： ProcessCANFrame
 * @ 功能说明： CAN正常接收到数据处理函数
 * @ 参数    ： @stc_can_rx_frame_t：接收的数据帧
 * @ 返回值  ： 无
 *********************************************************************/
void ProcessCANFrame(stc_can_rx_frame_t Rx_Data)
{
	if (Rx_Data.u32ID == CAN_FILTER1_ID || Rx_Data.u32ID == CAN_FILTER2_ID || Rx_Data.u32ID == CAN_FILTER3_ID || Rx_Data.u32ID == CAN_FILTER4_ID)
	{
		switch (Rx_Data.au8Data[0])
		{
		case CAN_TX_SELF_CHECK:
		{
			// 检查 Rx_Data.au8Data[3] 到 Rx_Data.au8Data[7] 是否都是 0xFF
			if (Rx_Data.au8Data[3] == 0xFF &&
				Rx_Data.au8Data[4] == 0xFF &&
				Rx_Data.au8Data[5] == 0xFF &&
				Rx_Data.au8Data[6] == 0xFF &&
				Rx_Data.au8Data[7] == 0xFF)
			{
				// 自检高压正常
				if (Rx_Data.au8Data[1] == CAN_RX_DATA_TYPE_SELF_CHECK_HIGH_VOLTAGE_NORMAL)
				{
				}
				// 自检高压异常
				else if (Rx_Data.au8Data[1] == CAN_RX_DATA_TYPE_SELF_CHECK_HIGH_VOLTAGE_ABNORMAL)
				{
				}

				// 自检低量程计数管正常
				if (Rx_Data.au8Data[2] == CAN_RX_DATA_TYPE_SELF_CHECK_LOW_RANGE_COUNTER_NORMAL)
				{
				}
				else
				{
				}
			}
			break;
		}
		case CAN_TX_CLEAR_CUMULATIVE_DOSE:
		{
			// 检查 Rx_Data.au8Data[2] 到 Rx_Data.au8Data[7] 是否都是 0xFF
			if (Rx_Data.au8Data[2] == 0xFF &&
				Rx_Data.au8Data[3] == 0xFF &&
				Rx_Data.au8Data[4] == 0xFF &&
				Rx_Data.au8Data[5] == 0xFF &&
				Rx_Data.au8Data[6] == 0xFF &&
				Rx_Data.au8Data[7] == 0xFF)
			{
				if (Rx_Data.au8Data[1] == CAN_RX_DATA_TYPE_CLEAR_CUMULATIVE_DOSE_SUCCESS)
				{
					// 累计剂量清零成功
				}
				else if (Rx_Data.au8Data[1] == CAN_RX_DATA_TYPE_CLEAR_CUMULATIVE_DOSE_FAIL)
				{
					// 累计剂量清零失败
				}
			}
			break;
		}
		case CAN_TX_SET_DOSE_RATE_ALARM:
		{
			// 四字节剂量率报警阈值数据float,低字节在前，高字节在后
			union Float_T4o_4Byte converter;

			converter.array[0] = Rx_Data.au8Data[1];
			converter.array[1] = Rx_Data.au8Data[2];
			converter.array[2] = Rx_Data.au8Data[3];
			converter.array[3] = Rx_Data.au8Data[4];
			break;
		}

		default:
			break;
		}

		// 设置探头连接标志位
		// xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE_CONNECT_CHECK);
	}
}

/***********************************************************************
 * @ 函数名  ： HostCanBusSendData
 * @ 功能说明： 主机CAN总线发送数据 函数
 * @ 参数    ： @uint8_t CAN_ID: 发送到具体对应的探头
 * @ 参数    ： @uint16_t data_type：判断是下发高压还是甄别电压
 * @ 参数    ： @uint8_t Voltage：电压值
 * @ 返回值  ： 无
 *********************************************************************/
// void HostCanBusSendData(uint8_t CAN_ID, uint8_t data_type, uint8_t *Voltage)
// {
// 	stc_can_tx_frame_t stcTx;

// 	stcTx.u32Ctrl = 0x0UL;						 // 初始化CAN发送帧的控制域
// 	stcTx.u32ID = CAN_TX_DISCRIMINATION_VOLTAGE; // 主机往探头端下发 甄别电压/高压数据 ID
// 	stcTx.IDE = 0UL;							 // CAN发送帧的ID类型，这里是标准帧
// 	stcTx.DLC = CAN_TX_DLC;

// 	for (uint8_t i = 0; i < CAN_TX_DATA_SIZE; i++)
// 	{
// 		stcTx.au8Data[i] = 0xFF;
// 	}

// 	if (data_type == CAN_TX_DATA_TYPE_HIGH_VOLTAGE)
// 	{
// 		stcTx.au8Data[0] = CAN_ID;
// 		stcTx.au8Data[1] = CAN_TX_DATA_TYPE_HIGH_VOLTAGE;
// 		stcTx.au8Data[2] = Voltage[0];
// 		stcTx.au8Data[3] = Voltage[1];
// 		stcTx.au8Data[4] = Voltage[2];
// 	}
// 	else if (data_type == CAN_TX_DATA_TYPE_DETECT_VOLTAGE)
// 	{
// 		stcTx.au8Data[0] = CAN_ID;
// 		stcTx.au8Data[1] = CAN_TX_DATA_TYPE_DETECT_VOLTAGE;
// 		stcTx.au8Data[2] = Voltage[0];
// 		stcTx.au8Data[3] = Voltage[1];
// 		stcTx.au8Data[4] = Voltage[2];
// 		stcTx.au8Data[5] = Voltage[3];
// 	}
// 	else
// 	{
// 		return;
// 	}

// 	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY);		 // 发送CAN总线发送队列
// 	xEventGroupSetBits(ScreenEventGroup, CAN_TX_START_FLAG); // 发送CAN总线发送开始标志
// }
