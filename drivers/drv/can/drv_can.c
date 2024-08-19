#include "variable.h"

/******************************函数定义*******************************/

/**
 * @brief  CAN中断处理函数
 * @param  无
 * @retval 无
 * @note   此函数处理CAN的发送和接收中断，并通知相应的任务
 */
void CAN_IrqCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (CAN_GetStatus(CAN_UNIT, CAN_FLAG_PTB_TX) == SET)
	{
		CAN_ClearStatus(CAN_UNIT, CAN_FLAG_PTB_TX);
		vTaskNotifyGiveFromISR(CanTxTask_Handle, &xHigherPriorityTaskWoken);
		CAN_IntCmd(CAN_UNIT, CAN_INT_PTB_TX, DISABLE);
	}

	if (CAN_GetStatus(CAN_UNIT, CAN_FLAG_RX) == SET)
	{
		CAN_ClearStatus(CAN_UNIT, CAN_FLAG_RX);
		vTaskNotifyGiveFromISR(CanRxTask_Handle, &xHigherPriorityTaskWoken);
		CAN_IntCmd(CAN_UNIT, CAN_INT_RX, DISABLE);
	}

	CAN_ClearStatus(CAN_UNIT, CAN_FLAG_PTB_TX | CAN_FLAG_RX);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief  接收CAN数据处理函数
 * @param  无
 * @retval 无
 * @note   此函数处理接收到的CAN数据，包括CAN总线自检接收处理和正常CAN数据收发处理
 */
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

/**
 * @brief  CAN正常接收到数据处理函数
 * @param  Rx_Data 接收的数据帧
 * @retval 无
 * @note   此函数根据接收到的CAN数据帧的内容进行相应的处理
 */
void ProcessCANFrame(stc_can_rx_frame_t Rx_Data)
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
				switch (Rx_Data.u32ID)
				{
				case CAN_FILTER1_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE1_CONNECT_CHECK | WAIT_PROBE1_HV_CHECK);
					break;
				case CAN_FILTER2_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE2_CONNECT_CHECK | WAIT_PROBE2_HV_CHECK);
					break;
				case CAN_FILTER3_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE3_CONNECT_CHECK | WAIT_PROBE3_HV_CHECK);
					break;
				case CAN_FILTER4_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE4_CONNECT_CHECK | WAIT_PROBE4_HV_CHECK);
					break;
				default:
					break;
				}
			}
			// 自检低量程计数管正常
			if (Rx_Data.au8Data[2] == CAN_RX_DATA_TYPE_SELF_CHECK_LOW_RANGE_COUNTER_NORMAL)
			{
				switch (Rx_Data.u32ID)
				{
				case CAN_FILTER1_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE1_CONNECT_CHECK | WAIT_PROBE1_COUNT_CHECK);
					break;
				case CAN_FILTER2_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE2_CONNECT_CHECK | WAIT_PROBE2_COUNT_CHECK);
					break;
				case CAN_FILTER3_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE3_CONNECT_CHECK | WAIT_PROBE3_COUNT_CHECK);
					break;
				case CAN_FILTER4_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE4_CONNECT_CHECK | WAIT_PROBE4_COUNT_CHECK);
					break;
				default:
					break;
				}
			}
		}
		break;
	}
	case CAN_TX_UPDATE_DOSE_RATE:
	{
		// 四字节剂量率数据float,低字节在前，高字节在后
		union Float_T4o_4Byte doseRate_unit;
		doseRate_unit.array[0] = Rx_Data.au8Data[1];
		doseRate_unit.array[1] = Rx_Data.au8Data[2];
		doseRate_unit.array[2] = Rx_Data.au8Data[3];
		doseRate_unit.array[3] = Rx_Data.au8Data[4];

		// 四字节累计剂量数据float,低字节在前，高字节在后
		// union Float_T4o_4Byte cumulativeDose_unit;
		// cumulativeDose_unit.array[0] = Rx_Data.au8Data[5];
		// cumulativeDose_unit.array[1] = Rx_Data.au8Data[6];
		// cumulativeDose_unit.array[2] = Rx_Data.au8Data[7];
		// cumulativeDose_unit.array[3] = Rx_Data.au8Data[8];

		WarningUpdateMessage UpdateMessage = {0};
		UpdateMessage.doseRate = doseRate_unit.buff;
		UpdateMessage.cumulativeDose = doseRate_unit.buff;

		// 判断剂量率是否大于10000 μSv/h
		if (UpdateMessage.doseRate > 10000)
			UpdateMessage.p = true;
		else
			UpdateMessage.p = false;

		// 判断累计剂量是否大于10000 μSv
		// if (UpdateMessage.cumulativeDose > 10000)
		// 	UpdateMessage.d = true;
		// else
		// 	UpdateMessage.d = false;

		// 根据探头ID发送更新消息
		switch (Rx_Data.u32ID)
		{
		case CAN_FILTER1_ID:
			UpdateMessage.label = "前方";
			xQueueSend(xQueue_WarningUpdate, &UpdateMessage, portMAX_DELAY);
			break;
		case CAN_FILTER2_ID:
			UpdateMessage.label = "后方";
			xQueueSend(xQueue_WarningUpdate, &UpdateMessage, portMAX_DELAY);
			break;
		case CAN_FILTER3_ID:
			UpdateMessage.label = "左方";
			xQueueSend(xQueue_WarningUpdate, &UpdateMessage, portMAX_DELAY);
			break;
		case CAN_FILTER4_ID:
			UpdateMessage.label = "右方";
			xQueueSend(xQueue_WarningUpdate, &UpdateMessage, portMAX_DELAY);
			break;
		default:
			break;
		}

		break;
	}

	default:
		break;
	}
}

/**
 * @brief  CAN总线发送自检命令的函数
 * @param  u32ID 探头的ID
 * @retval 无
 * @note   此函数根据接收到的CAN数据帧的内容进行相应的处理
 */
void CAN_SendSelfCheckCommand(uint32_t u32ID)
{
	if (u32ID != CAN_FILTER1_ID && u32ID != CAN_FILTER2_ID && u32ID != CAN_FILTER3_ID && u32ID != CAN_FILTER4_ID)
	{
		return;
	}

	stc_can_tx_frame_t stcTx;

	stcTx.au8Data[0] = CAN_TX_SELF_CHECK;
	for (uint8_t i = 1; i < CAN_TX_DLC; i++)
	{
		stcTx.au8Data[i] = 0xFF;
	}

	stcTx.u32Ctrl = 0x0UL;							 //  初始化CAN发送帧的控制域
	stcTx.IDE = 0UL;								 //  CAN发送帧的ID类型, 0: 标准帧, 1: 扩展帧
	stcTx.u32ID = u32ID;							 //  CAN发送帧的ID
	stcTx.DLC = CAN_TX_DLC;							 //  CAN发送帧的数据长度
	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); //  将发送帧加入到发送队列
}

/**
 * @brief  CAN总线发送累计剂量清零命令的函数
 * @param  u32ID 探头的ID
 * @retval 无
 * @note   此函数根据接收到的CAN数据帧的内容进行相应的处理
 */
void CAN_SendClearCumulativeDoseCommand(uint32_t u32ID)
{
	if (u32ID != CAN_FILTER1_ID && u32ID != CAN_FILTER2_ID && u32ID != CAN_FILTER3_ID && u32ID != CAN_FILTER4_ID)
	{
		return;
	}

	stc_can_tx_frame_t stcTx;

	stcTx.au8Data[0] = CAN_TX_CLEAR_CUMULATIVE_DOSE;
	for (uint8_t i = 1; i < CAN_TX_DLC; i++)
	{
		stcTx.au8Data[i] = 0xFF;
	}

	stcTx.u32Ctrl = 0x0UL;							 //  初始化CAN发送帧的控制域
	stcTx.IDE = 0UL;								 //  CAN发送帧的ID类型, 0: 标准帧, 1: 扩展帧
	stcTx.u32ID = u32ID;							 //  CAN发送帧的ID
	stcTx.DLC = CAN_TX_DLC;							 //  CAN发送帧的数据长度
	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); //  将发送帧加入到发送队列
}

/**
 * @brief  CAN总线发送剂量报警阈值设置命令的函数
 * @param  u32ID 探头的ID
 * @param  f32DoseRateAlarmThreshold 剂量报警阈值
 * @retval 无
 * @note   此函数根据接收到的CAN数据帧的内容进行相应的处理
 */
void CAN_SendDoseRateAlarmThresholdCommand(uint32_t u32ID, float f32DoseRateAlarmThreshold)
{
	if (u32ID != CAN_FILTER1_ID && u32ID != CAN_FILTER2_ID && u32ID != CAN_FILTER3_ID && u32ID != CAN_FILTER4_ID)
	{
		return;
	}

	stc_can_tx_frame_t stcTx;

	stcTx.au8Data[0] = CAN_TX_SET_DOSE_RATE_ALARM;
	union Float_T4o_4Byte converter;
	converter.buff = f32DoseRateAlarmThreshold;
	stcTx.au8Data[1] = converter.array[0];
	stcTx.au8Data[2] = converter.array[1];
	stcTx.au8Data[3] = converter.array[2];
	stcTx.au8Data[4] = converter.array[3];
	for (uint8_t i = 5; i < CAN_TX_DLC; i++)
	{
		stcTx.au8Data[i] = 0xFF;
	}

	stcTx.u32Ctrl = 0x0UL;							 //  初始化CAN发送帧的控制域
	stcTx.IDE = 0UL;								 //  CAN发送帧的ID类型, 0: 标准帧, 1: 扩展帧
	stcTx.u32ID = u32ID;							 //  CAN发送帧的ID
	stcTx.DLC = CAN_TX_DLC;							 //  CAN发送帧的数据长度
	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); //  将发送帧加入到发送队列
}

/**
 * @brief  CAN总线发送上传剂量报警阈值命令的函数
 * @param  u32ID 探头的ID
 * @retval 无
 * @note   此函数根据接收到的CAN数据帧的内容进行相应的处理
 */
void CAN_SendUploadDoseRateAlarmThresholdCommand(uint32_t u32ID)
{
	if (u32ID != CAN_FILTER1_ID && u32ID != CAN_FILTER2_ID && u32ID != CAN_FILTER3_ID && u32ID != CAN_FILTER4_ID)
	{
		return;
	}

	stc_can_tx_frame_t stcTx;

	stcTx.au8Data[0] = CAN_TX_UPDATE_DOSE_RATE_ALARM;
	for (uint8_t i = 1; i < CAN_TX_DLC; i++)
	{
		stcTx.au8Data[i] = 0xFF;
	}

	stcTx.u32Ctrl = 0x0UL;							 //  初始化CAN发送帧的控制域
	stcTx.IDE = 0UL;								 //  CAN发送帧的ID类型, 0: 标准帧, 1: 扩展帧
	stcTx.u32ID = u32ID;							 //  CAN发送帧的ID
	stcTx.DLC = CAN_TX_DLC;							 //  CAN发送帧的数据长度
	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); //  将发送帧加入到发送队列
}

/**
 * @brief  CAN总线发送上传剂量率和累计剂量命令的函数
 * @param  u32ID 探头的ID
 * @retval 无
 * @note   此函数根据接收到的CAN数据帧的内容进行相应的处理
 */
void CAN_SendUploadDoseRateAndCumulativeDoseCommand(uint32_t u32ID)
{
	if (u32ID != CAN_FILTER1_ID && u32ID != CAN_FILTER2_ID && u32ID != CAN_FILTER3_ID && u32ID != CAN_FILTER4_ID)
	{
		return;
	}

	stc_can_tx_frame_t stcTx;

	stcTx.au8Data[0] = CAN_TX_UPDATE_DOSE_RATE;
	for (uint8_t i = 1; i < CAN_TX_DLC; i++)
	{
		stcTx.au8Data[i] = 0xFF;
	}

	stcTx.u32Ctrl = 0x0UL;							 //  初始化CAN发送帧的控制域
	stcTx.IDE = 0UL;								 //  CAN发送帧的ID类型, 0: 标准帧, 1: 扩展帧
	stcTx.u32ID = u32ID;							 //  CAN发送帧的ID
	stcTx.DLC = CAN_TX_DLC;							 //  CAN发送帧的数据长度
	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); //  将发送帧加入到发送队列
}
