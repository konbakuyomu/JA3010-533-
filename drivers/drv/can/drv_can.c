#include "variable.h"

/* 变量声明 ----------------------------------------------------------------*/
static stc_can_rx_frame_t *m_astRxFrame = NULL;

/* 函数声明 ----------------------------------------------------------------*/

/**
 * @brief  初始化CAN接收缓冲区
 * @param  无
 * @retval 0: 成功, -1: 失败
 */
static int CAN_InitReceiveBuffer(void)
{
	if (m_astRxFrame == NULL)
	{
		m_astRxFrame = (stc_can_rx_frame_t *)mymalloc(sizeof(stc_can_rx_frame_t) * CAN_RX_FRAME_NUM);
		if (m_astRxFrame == NULL)
		{
			// 内存分配失败，处理错误
			return -1;
		}
	}
	return 0;
}

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

	CAN_ClearStatus(CAN_UNIT, CAN_FLAG_ALL);
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

	// 确保缓冲区已初始化
	if (m_astRxFrame == NULL)
	{
		if (CAN_InitReceiveBuffer() != 0)
		{
			// 初始化失败，处理错误
			return;
		}
	}

	// 清空接收缓冲区
	memset(m_astRxFrame, 0, sizeof(stc_can_rx_frame_t) * CAN_RX_FRAME_NUM);

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
	// 提取探头号和命令项
	uint8_t probeNumber = (Rx_Data.u32ID >> 8) & 0xFF;
	uint8_t commandItem = Rx_Data.u32ID & 0xFF;

	switch (commandItem)
	{
	case CAN_TX_SELF_CHECK:
	{
		// 检查 Rx_Data.au8Data[3] 到 Rx_Data.au8Data[7] 是否都是 0xFF
		if (Rx_Data.au8Data[2] == 0xFF &&
			Rx_Data.au8Data[3] == 0xFF &&
			Rx_Data.au8Data[4] == 0xFF &&
			Rx_Data.au8Data[5] == 0xFF &&
			Rx_Data.au8Data[6] == 0xFF &&
			Rx_Data.au8Data[7] == 0xFF)
		{
			// 自检高压正常
			if (Rx_Data.au8Data[0] == CAN_RX_DATA_TYPE_SELF_CHECK_HIGH_VOLTAGE_NORMAL)
			{
				switch (probeNumber)
				{
				case CAN_PROBE1_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE1_CONNECT_CHECK | WAIT_PROBE1_HV_CHECK);
					break;
				case CAN_PROBE2_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE2_CONNECT_CHECK | WAIT_PROBE2_HV_CHECK);
					break;
				case CAN_PROBE3_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE3_CONNECT_CHECK | WAIT_PROBE3_HV_CHECK);
					break;
				case CAN_PROBE4_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE4_CONNECT_CHECK | WAIT_PROBE4_HV_CHECK);
					break;
				default:
					break;
				}
			}
			// 自检低量程计数管正常
			if (Rx_Data.au8Data[1] == CAN_RX_DATA_TYPE_SELF_CHECK_LOW_RANGE_COUNTER_NORMAL)
			{
				switch (probeNumber)
				{
				case CAN_PROBE1_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE1_CONNECT_CHECK | WAIT_PROBE1_COUNT_CHECK);
					break;
				case CAN_PROBE2_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE2_CONNECT_CHECK | WAIT_PROBE2_COUNT_CHECK);
					break;
				case CAN_PROBE3_ID:
					xEventGroupSetBits(xInit_EventGroup, WAIT_PROBE3_CONNECT_CHECK | WAIT_PROBE3_COUNT_CHECK);
					break;
				case CAN_PROBE4_ID:
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
		union Float_T4o_4Byte doseRate_unit, cumulativeDose_unit;
		memcpy(doseRate_unit.array, Rx_Data.au8Data, 4);
		memcpy(cumulativeDose_unit.array, Rx_Data.au8Data + 4, 4);

		WarningUpdateMessage UpdateMessage = {0};
		UpdateMessage.doseRate = doseRate_unit.buff;
		UpdateMessage.cumulativeDose = cumulativeDose_unit.buff;
		UpdateMessage.probeNumber = probeNumber;

		char *labels[] = {"探头1", "探头2", "探头3", "探头4"};
		uint32_t realtime_threshold;
		uint32_t cumulative_threshold;

		switch (probeNumber)
		{
		case CAN_PROBE1_ID:
			realtime_threshold = data.probe1_realtime_alarm_threshold;
			cumulative_threshold = data.probe1_cumulative_alarm_threshold;
			break;
		case CAN_PROBE2_ID:
			realtime_threshold = data.probe2_realtime_alarm_threshold;
			cumulative_threshold = data.probe2_cumulative_alarm_threshold;
			break;
		case CAN_PROBE3_ID:
			realtime_threshold = data.probe3_realtime_alarm_threshold;
			cumulative_threshold = data.probe3_cumulative_alarm_threshold;
			break;
		case CAN_PROBE4_ID:
			realtime_threshold = data.probe4_realtime_alarm_threshold;
			cumulative_threshold = data.probe4_cumulative_alarm_threshold;
			break;
		}

		UpdateMessage.p = (UpdateMessage.doseRate > realtime_threshold);
		UpdateMessage.d = (UpdateMessage.cumulativeDose > cumulative_threshold);

		UpdateMessage.label = labels[probeNumber - CAN_PROBE1_ID];

		xQueueSend(xQueue_ProbeInfoTransfer, &UpdateMessage, 0);

		break;
	}

	default:
		break;
	}
}

/**
 * @brief  CAN总线发送自检命令的函数
 * @param  u8ProbeID 探头的ID
 * @retval 无
 * @note   此函数发送自检命令，使用扩展帧格式
 */
void CAN_SendSelfCheckCommand(uint8_t u8ProbeID)
{
	if (u8ProbeID != CAN_PROBE1_ID && u8ProbeID != CAN_PROBE2_ID &&
		u8ProbeID != CAN_PROBE3_ID && u8ProbeID != CAN_PROBE4_ID)
	{
		return;
	}

	stc_can_tx_frame_t stcTx;

	// 组合探头ID和命令ID成为扩展ID
	uint32_t u32ExtendedID = ((uint32_t)u8ProbeID << 8) | CAN_TX_SELF_CHECK;

	for (uint8_t i = 0; i < CAN_TX_DLC; i++)
	{
		stcTx.au8Data[i] = 0xFF;
	}

	stcTx.u32Ctrl = 0x0UL;							 // 初始化CAN发送帧的控制域
	stcTx.IDE = 1UL;								 // CAN发送帧的ID类型, 1: 扩展帧
	stcTx.u32ID = u32ExtendedID;					 // 设置扩展ID
	stcTx.DLC = CAN_TX_DLC;							 // CAN发送帧的数据长度
	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); // 将发送帧加入到发送队列
}

/**
 * @brief  CAN总线发送累计剂量清零命令的函数
 * @param  u8ProbeID 探头的ID
 * @retval 无
 * @note   此函数根据接收到的CAN数据帧的内容进行相应的处理
 */
void CAN_SendClearCumulativeDoseCommand(uint8_t u8ProbeID)
{
	if (u8ProbeID != CAN_PROBE1_ID && u8ProbeID != CAN_PROBE2_ID &&
		u8ProbeID != CAN_PROBE3_ID && u8ProbeID != CAN_PROBE4_ID)
	{
		return;
	}

	stc_can_tx_frame_t stcTx;

	// 组合探头ID和命令ID成为扩展ID
	uint32_t u32ExtendedID = ((uint32_t)u8ProbeID << 8) | CAN_TX_CLEAR_CUMULATIVE_DOSE;

	for (uint8_t i = 0; i < CAN_TX_DLC; i++)
	{
		stcTx.au8Data[i] = 0xFF;
	}

	stcTx.u32Ctrl = 0x0UL;							 //  初始化CAN发送帧的控制域
	stcTx.IDE = 1UL;								 //  CAN发送帧的ID类型, 1: 扩展帧
	stcTx.u32ID = u32ExtendedID;					 //  CAN发送帧的ID
	stcTx.DLC = CAN_TX_DLC;							 //  CAN发送帧的数据长度
	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); //  将发送帧加入到发送队列
}

/**
 * @brief  CAN总线发送剂量报警阈值设置命令的函数
 * @param  u8ProbeID 探头的ID
 * @param  f32DoseRateAlarmThreshold 剂量报警阈值
 * @retval 无
 * @note   此函数根据接收到的CAN数据帧的内容进行相应的处理
 */
void CAN_SendDoseRateAlarmThresholdCommand(uint8_t u8ProbeID, float f32DoseRateAlarmThreshold)
{
	if (u8ProbeID != CAN_PROBE1_ID && u8ProbeID != CAN_PROBE2_ID &&
		u8ProbeID != CAN_PROBE3_ID && u8ProbeID != CAN_PROBE4_ID)
	{
		return;
	}

	stc_can_tx_frame_t stcTx;

	// 组合探头ID和命令ID成为扩展ID
	uint32_t u32ExtendedID = ((uint32_t)u8ProbeID << 8) | CAN_TX_SET_DOSE_RATE_ALARM;

	union Float_T4o_4Byte converter;
	converter.buff = f32DoseRateAlarmThreshold;
	stcTx.au8Data[0] = converter.array[0];
	stcTx.au8Data[1] = converter.array[1];
	stcTx.au8Data[2] = converter.array[2];
	stcTx.au8Data[3] = converter.array[3];
	for (uint8_t i = 4; i < CAN_TX_DLC; i++)
	{
		stcTx.au8Data[i] = 0xFF;
	}

	stcTx.u32Ctrl = 0x0UL;							 //  初始化CAN发送帧的控制域
	stcTx.IDE = 1UL;								 //  CAN发送帧的ID类型, 1: 扩展帧
	stcTx.u32ID = u32ExtendedID;					 //  CAN发送帧的ID
	stcTx.DLC = CAN_TX_DLC;							 //  CAN发送帧的数据长度
	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); //  将发送帧加入到发送队列
}

/**
 * @brief  CAN总线发送上传剂量报警阈值命令的函数
 * @param  u8ProbeID 探头的ID
 * @retval 无
 * @note   此函数根据接收到的CAN数据帧的内容进行相应的处理
 */
void CAN_SendUploadDoseRateAlarmThresholdCommand(uint8_t u8ProbeID)
{
	if (u8ProbeID != CAN_PROBE1_ID && u8ProbeID != CAN_PROBE2_ID &&
		u8ProbeID != CAN_PROBE3_ID && u8ProbeID != CAN_PROBE4_ID)
	{
		return;
	}

	stc_can_tx_frame_t stcTx;

	// 组合探头ID和命令ID成为扩展ID
	uint32_t u32ExtendedID = ((uint32_t)u8ProbeID << 8) | CAN_TX_UPDATE_DOSE_RATE_ALARM;

	for (uint8_t i = 0; i < CAN_TX_DLC; i++)
	{
		stcTx.au8Data[i] = 0xFF;
	}

	stcTx.u32Ctrl = 0x0UL;							 //  初始化CAN发送帧的控制域
	stcTx.IDE = 1UL;								 //  CAN发送帧的ID类型, 1: 扩展帧
	stcTx.u32ID = u32ExtendedID;					 //  CAN发送帧的ID
	stcTx.DLC = CAN_TX_DLC;							 //  CAN发送帧的数据长度
	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); //  将发送帧加入到发送队列
}

/**
 * @brief  CAN总线发送上传剂量率和累计剂量命令的函数
 * @param  u8ProbeID 探头的ID
 * @retval 无
 * @note   此函数根据接收到的CAN数据帧的内容进行相应的处理
 */
void CAN_SendUploadDoseRateAndCumulativeDoseCommand(uint8_t u8ProbeID)
{
	if (u8ProbeID != CAN_PROBE1_ID && u8ProbeID != CAN_PROBE2_ID &&
		u8ProbeID != CAN_PROBE3_ID && u8ProbeID != CAN_PROBE4_ID)
	{
		return;
	}

	stc_can_tx_frame_t stcTx;

	// 组合探头ID和命令ID成为扩展ID
	uint32_t u32ExtendedID = ((uint32_t)u8ProbeID << 8) | CAN_TX_UPDATE_DOSE_RATE;

	for (uint8_t i = 0; i < CAN_TX_DLC; i++)
	{
		stcTx.au8Data[i] = 0xFF;
	}

	stcTx.u32Ctrl = 0x0UL;							 //  初始化CAN发送帧的控制域
	stcTx.IDE = 1UL;								 //  CAN发送帧的ID类型, 1: 扩展帧
	stcTx.u32ID = u32ExtendedID;					 //  CAN发送帧的ID
	stcTx.DLC = CAN_TX_DLC;							 //  CAN发送帧的数据长度
	xQueueSend(xQueue_CanTx, &stcTx, portMAX_DELAY); //  将发送帧加入到发送队列
}
