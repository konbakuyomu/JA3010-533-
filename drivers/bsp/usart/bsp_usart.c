#include "variable.h"

/* 变量声明 ----------------------------------------------------------------*/
uint8_t *m_au8RxBuf = NULL;

/* 函数声明 ----------------------------------------------------------------*/
/**
 * @brief  DMA 接收完成中断回调函数
 * @param  None
 * @retval None
 */
static void RX_DMA_TC_IrqCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint16_t m_u16RxLen = APP_FRAME_LEN_MAX;

	// 使用任务通知并发送数据
	xTaskNotifyFromISR(UsartTask_Handle, m_u16RxLen, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	// 清除DMA接收完成标志
	DMA_ClearTransCompleteStatus(RX_DMA_UNIT, RX_DMA_TC_FLAG);
}

/**
 * @brief  USART 接收超时中断回调函数
 * @param  None
 * @retval None
 */
static void USART_RxTimeout_IrqCallback(void)
{
	/* 计算已经接收到的数据的长度,singleTransCount为单次传输的数据长度 */
	/* 公式为 u16DataToSend = APP_FRAME_LEN_MAX (最大长度) - 还未被接收的数据的长度(即DMA_GetTransCount的返回值) */
	//* DMA_GetTransCount() 这个函数与 Probe_RxBuf(即前面DMA配置的缓冲区) 的关系是：它告诉我们 Probe_RxBuf 中还有多少空间没有被 DMA 填充
	uint16_t u16DataToSend = APP_FRAME_LEN_MAX - (uint16_t)DMA_GetTransCount(RX_DMA_UNIT, RX_DMA_CH);

	if (u16DataToSend <= APP_FRAME_LEN_MAX)
	{
		// 使用任务通知并发送数据
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		// 注意：确保 u16DataToSend 的值不会超过任务通知的最大值
		xTaskNotifyFromISR(UsartTask_Handle, u16DataToSend, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	AOS_SW_Trigger();									  // 软件AOS触发DMA重配置
	TMR0_Stop(TMR0_TIMEOUT_UNIT, TMR0_TIMEOUT_CH);		  // （手动清零，因为配置的硬件清零是要超时才触发的）停止定时器，会对TMR0的基本控制寄存器（BCONR）进行配置，以停止定时器的计数
	USART_ClearStatus(USART_UNIT, USART_FLAG_RX_TIMEOUT); // 清除接收超时标志位
}

/**
 * @brief  USART 接收错误中断回调函数
 * @param  None
 * @retval None
 */
static void USART_RxError_IrqCallback(void)
{
	// 读取串口接收寄存器，以清除串口接收错误标志位
	(void)USART_ReadData(USART_UNIT);
	// 清除串口接收错误标志位
	USART_ClearStatus(USART_UNIT, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

/**
 * @brief  DMA 发送完成中断回调函数
 * @param  None
 * @retval None
 */
static void TX_DMA_TC_IrqCallback(void)
{
	// 使能串口发送完成中断
	USART_FuncCmd(USART_UNIT, USART_INT_TX_CPLT, ENABLE);
	// 清除DMA发送完成标志
	DMA_ClearTransCompleteStatus(TX_DMA_UNIT, TX_DMA_TC_FLAG);
}

/**
 * @brief  USART 发送完成中断回调函数
 * @param  None
 * @retval None
 */
static void USART_TxComplete_IrqCallback(void)
{
	/* 关闭串口发送功能和发送完成中断 */
	USART_FuncCmd(USART_UNIT, (USART_TX | USART_INT_TX_CPLT), DISABLE);
	/* 清除发送完成标志位 */
	USART_ClearStatus(USART_UNIT, USART_FLAG_TX_CPLT);
	/* 使能485控制引脚接收模式 */
	CONTROL_RX();
}

/**
 * @brief  USART3 DMA 配置
 * @param  None
 * @retval None
 */
static int32_t USART3_DMA_Config(void)
{
	int32_t i32Ret;
	stc_dma_init_t stcDmaInit;
	stc_dma_llp_init_t stcDmaLlpInit;			// DMA链表指针，链表指针（LLP）是DMA在执行一个DMA传输任务后，自动加载下一个DMA任务的机制
	stc_irq_signin_config_t stcIrqSignConfig;	// 中断配置结构体
	static stc_dma_llp_descriptor_t stcLlpDesc; // DMA链表指针

	/* DMA&AOS FCG enable（使能时钟） */
	FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA2, ENABLE);
	FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);

	/* 配置 USART_RX_DMA */
	(void)DMA_StructInit(&stcDmaInit);						// DMA初始化结构体清零
	stcDmaInit.u32IntEn = DMA_INT_ENABLE;					// 启动DMA中断
	stcDmaInit.u32BlockSize = 1UL;							// 指定DMA的块大小，也就是一次DMA操作应当传输的数据块的大小。
	stcDmaInit.u32TransCount = APP_FRAME_LEN_MAX;		    // 得到数组长度，原来的写法是 ARRAY_SZ(m_au8RxBuf)
	stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;			// 这个字段指定DMA传输数据的宽度，也就是一次性传输的数据的位数（8，16，32）
	stcDmaInit.u32DestAddr = (uint32_t)m_au8RxBuf;			// 指定DMA的目标地址
	stcDmaInit.u32SrcAddr = ((uint32_t)(&USART_UNIT->RDR)); // 指定DMA的源地址，也就是DMA将从这个地址开始读取数据，由芯片手册可知接收数据寄存器是从16位开始的，所以是RDR寄存器
	stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;			// 源地址固定不变
	stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;			// 表示目标地址每次增加
	i32Ret = DMA_Init(RX_DMA_UNIT, RX_DMA_CH, &stcDmaInit); // 初始化DMA
	if (LL_OK == i32Ret)
	{
		//--------------------------------------------------------------------
		(void)DMA_LlpStructInit(&stcDmaLlpInit);	   // 初始化DMA链表指针结构体
		stcDmaLlpInit.u32State = DMA_LLP_ENABLE;	   // DMA的LLP功能是否启用
		stcDmaLlpInit.u32Mode = DMA_LLP_WAIT;		   // 等待下一个请求还是立即执行
		stcDmaLlpInit.u32Addr = (uint32_t)&stcLlpDesc; // 指定了DMA的链表指针地址。当启用LLP功能时，DMA会使用这个地址作为链表的起始地址
		(void)DMA_LlpInit(RX_DMA_UNIT, RX_DMA_CH, &stcDmaLlpInit);
		//--------------------------------------------------------------------

		stcLlpDesc.SARx = stcDmaInit.u32SrcAddr;  // LLP源地址。在LLP模式下，DMA从此地址开始读取数据。
		stcLlpDesc.DARx = stcDmaInit.u32DestAddr; // LLP目标地址。在LLP模式下，DMA将数据写入此地址。
		stcLlpDesc.DTCTLx = (stcDmaInit.u32TransCount << DMA_DTCTL_CNT_POS) | (stcDmaInit.u32BlockSize << DMA_DTCTL_BLKSIZE_POS);
		;										 // 每个DMA传输任务的数据块的数量和大小
		stcLlpDesc.LLPx = (uint32_t)&stcLlpDesc; // LLP下一个链表指针
		stcLlpDesc.CHCTLx = stcDmaInit.u32SrcAddrInc | stcDmaInit.u32DestAddrInc | stcDmaInit.u32DataWidth |
							stcDmaInit.u32IntEn | stcDmaLlpInit.u32State | stcDmaLlpInit.u32Mode; // LLP通道控制。此字段定义了DMA通道的控制信息。

		DMA_ReconfigLlpCmd(RX_DMA_UNIT, RX_DMA_CH, ENABLE); // DMA的重配置功能启用或禁用链表指针（LLP）
		DMA_ReconfigCmd(RX_DMA_UNIT, ENABLE);				// 启用或禁用DMA的重配置功能
		/* 设置触发事件源 */
		// 第一个参数u32Target，是需要被AOS源触发的目标，这里RX_DMA_RECONF_TRIG_SEL的值是AOS_DMA_RC，即DMA重配置触发选择器的地址，这个值在AOS中定义为DMA的触发选择器重配置的地址
		// 第二个参数enSource是触发AOS目标的AOS源，这个值是AOS的触发源索引，这个索引指向一个特定的事件源
		// 设置AOS的DMA重配置触发选择器所选择的触发源为EVT_SRC_AOS_STRG事件，即通过软件设置INTSFTTRG寄存器的SFTG位来触发的事件。当这个事件发生时，将会触发DMA的重配置
		//* 这里的AOS触发不用调用CPU资源，是硬件自动触发的，所以不会影响CPU的运行效率
		//! 说白了一句话，触发源和事件源的逻辑顺序是：先指定需要被触发的目标（触发源 - 参数1），然后指定触发源的事件类型（事件源 - 参数2）
		// 在USART_RxTimeout_IrqCallback这个函数中，我手动设置一个事件源，这个事件源就是EVT_SRC_AOS_STRG，当这个事件源发生时（也就是超时的时候），就会触发DMA的重配置
		// 当超时的时候，会触发EVT_SRC_AOS_STRG事件，这个事件会触发DMA的重配置，DMA重配置会重新配置DMA的传输参数，然后重新开始DMA传输，这样就实现了DMA的循环接收
		AOS_SetTriggerEventSrc(RX_DMA_RECONF_TRIG_SEL, RX_DMA_RECONF_TRIG_EVT_SRC);

		stcIrqSignConfig.enIntSrc = RX_DMA_TC_INT_SRC;		   // 指定中断源
		stcIrqSignConfig.enIRQn = RX_DMA_TC_IRQn;			   // 指定中断请求类型
		stcIrqSignConfig.pfnCallback = &RX_DMA_TC_IrqCallback; // 指定中断服务函数
		(void)INTC_IrqSignIn(&stcIrqSignConfig);
		NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
		NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
		NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

		AOS_SetTriggerEventSrc(RX_DMA_TRIG_SEL, RX_DMA_TRIG_EVT_SRC); // 当收到串口接收中断时，触发DMA2通道0的传输请求

		DMA_Cmd(RX_DMA_UNIT, ENABLE);								 // 使能DMA
		DMA_TransCompleteIntCmd(RX_DMA_UNIT, RX_DMA_TC_INT, ENABLE); // 使能DMA传输完成中断（这个库函数的作用是配置DMA传输完成中断的使能状态）
		(void)DMA_ChCmd(RX_DMA_UNIT, RX_DMA_CH, ENABLE);			 // 使能DMA通道
	}

	uint8_t Probe_TxBuf[APP_FRAME_LEN_MAX]; // 临时探头串口发送数组

	/* 配置 USART_TX_DMA */
	(void)DMA_StructInit(&stcDmaInit); // DMA初始化结构体清零
	stcDmaInit.u32IntEn = DMA_INT_ENABLE;
	stcDmaInit.u32BlockSize = 1UL;
	stcDmaInit.u32TransCount = ARRAY_SZ(Probe_TxBuf);
	stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
	stcDmaInit.u32DestAddr = (uint32_t)(&USART_UNIT->TDR); // 指定DMA的源地址，也就是DMA将从这个地址开始读取数据，由芯片手册可知接收数据寄存器是从0位开始的,所以不需要+2U
	stcDmaInit.u32SrcAddr = (uint32_t)Probe_TxBuf;
	stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
	stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
	i32Ret = DMA_Init(TX_DMA_UNIT, TX_DMA_CH, &stcDmaInit);
	if (LL_OK == i32Ret)
	{
		stcIrqSignConfig.enIntSrc = TX_DMA_TC_INT_SRC;
		stcIrqSignConfig.enIRQn = TX_DMA_TC_IRQn;
		stcIrqSignConfig.pfnCallback = &TX_DMA_TC_IrqCallback;
		(void)INTC_IrqSignIn(&stcIrqSignConfig);
		NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
		NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
		NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

		AOS_SetTriggerEventSrc(TX_DMA_TRIG_SEL, TX_DMA_TRIG_EVT_SRC); // 当收到串口发送完成中断时，触发DMA2通道1的传输请求

		DMA_Cmd(TX_DMA_UNIT, ENABLE);								 // 使能DMA
		DMA_TransCompleteIntCmd(TX_DMA_UNIT, TX_DMA_TC_INT, ENABLE); // 使能DMA传输完成中断
	}
	return i32Ret;
}

/**
 * @brief  TMR0 配置(串口超时接收配置)
 * @param  u16TimeoutBits: 超时时间
 *
 * @note   USART1：Timer0 Unit1 A 通道
 * @note   USART2：Timer0 Unit1 B 通道
 * @note   USART3：Timer0 Unit2 A 通道
 * @note   USART4：Timer0 Unit2 B 通道
 *
 * @retval None
 */
static void USART3_TMR0_Config(uint16_t u16TimeoutBits)
{
	// 在这段代码中，TMR0定时器的作用是用于检测串口接收数据的超时情况。
	// 具体来说，当串口接收到数据后，TMR0定时器开始计时，如果在一定时间内没有接收到完整的数据帧，就会触发串口接收超时中断，
	// 从而停止接收数据并进行后续处理。这个超时时间可以通过USART_TIMEOUT_BITS宏定义来设置，单位为TMR0的时钟周期。
	uint16_t u16Div;
	uint16_t u16Delay;
	uint16_t u16CompareValue;
	stc_tmr0_init_t stcTmr0Init;

	TMR0_FCG_ENABLE();

	stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_XTAL32; // 时钟源为外部32.768KHz
	stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV8;	   // 时钟分频为8
	stcTmr0Init.u32Func = TMR0_FUNC_CMP;		   // TMR0_CMP_MODE 工作在比较模式
	if (TMR0_CLK_DIV1 == stcTmr0Init.u32ClockDiv)
	{
		u16Delay = 7U;
	}
	else if (TMR0_CLK_DIV2 == stcTmr0Init.u32ClockDiv)
	{
		u16Delay = 5U;
	}
	else if ((TMR0_CLK_DIV4 == stcTmr0Init.u32ClockDiv) ||
			 (TMR0_CLK_DIV8 == stcTmr0Init.u32ClockDiv) ||
			 (TMR0_CLK_DIV16 == stcTmr0Init.u32ClockDiv))
	{
		u16Delay = 3U;
	}
	else
	{
		u16Delay = 2U;
	}

	u16Div = (uint16_t)1U << (stcTmr0Init.u32ClockDiv >> TMR0_BCONR_CKDIVA_POS); // 计算分频系数
	u16CompareValue = ((u16TimeoutBits + u16Div - 1U) / u16Div) - u16Delay;		 // 计算比较值
	stcTmr0Init.u16CompareValue = u16CompareValue;								 // 设置比较值
	(void)TMR0_Init(TMR0_TIMEOUT_UNIT, TMR0_TIMEOUT_CH, &stcTmr0Init);			 // 初始化TMR0

	TMR0_HWStartCondCmd(TMR0_TIMEOUT_UNIT, TMR0_TIMEOUT_CH, ENABLE); // 配置TMR0的硬件启动条件，当硬件启动条件满足时，TMR0会开始计数（这里硬件启动的条件是当串口接收缓冲区接收到第1个字节开始）。
	TMR0_HWClearCondCmd(TMR0_TIMEOUT_UNIT, TMR0_TIMEOUT_CH, ENABLE); // 配置TMR0的硬件清零条件，当硬件清零条件满足时，TMR0会被清零。（这里硬件关闭的条件是检测到 TIMEOUT 后自动关闭定时器计数）
}

/**
 * @brief  USART3 中断初始化
 * @param  None
 * @retval None
 */
static void Usart3_IrqInit(void)
{
	stc_irq_signin_config_t stcIrqSigninConfig;

	/* 注册 USART 接收错误中断处理函数 */
	stcIrqSigninConfig.enIRQn = USART_RX_ERR_IRQn;
	stcIrqSigninConfig.enIntSrc = USART_RX_ERR_INT_SRC;
	stcIrqSigninConfig.pfnCallback = &USART_RxError_IrqCallback;
	(void)INTC_IrqSignIn(&stcIrqSigninConfig);
	NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
	NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
	NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

	/* 注册 USART 接收超时中断处理函数 */
	stcIrqSigninConfig.enIRQn = USART_RX_TIMEOUT_IRQn;
	stcIrqSigninConfig.enIntSrc = USART_RX_TIMEOUT_INT_SRC;
	stcIrqSigninConfig.pfnCallback = &USART_RxTimeout_IrqCallback;
	(void)INTC_IrqSignIn(&stcIrqSigninConfig);
	NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
	NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
	NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

	/* 注册 USART 发送完成中断处理函数 */
	stcIrqSigninConfig.enIRQn = USART_TX_CPLT_IRQn;
	stcIrqSigninConfig.enIntSrc = USART_TX_CPLT_INT_SRC;
	stcIrqSigninConfig.pfnCallback = &USART_TxComplete_IrqCallback;
	(void)INTC_IrqSignIn(&stcIrqSigninConfig);
	NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
	NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
	NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);
}

/**
 * @brief  GPIO 配置
 * @note   将引脚复用为RX和TX，并配置控制脚
 * @param  None
 * @retval None
 */
static void GPIO_SetFunction(void)
{
	stc_gpio_init_t stcGpioInit;

	/* Configure USART RX/TX pin. */
	GPIO_SetFunc(USART_RX_PORT, USART_RX_PIN, USART_RX_GPIO_FUNC);
	GPIO_SetFunc(USART_TX_PORT, USART_TX_PIN, USART_TX_GPIO_FUNC);

	/* GPIO配置结构体初始化 */
	(void)GPIO_StructInit(&stcGpioInit);
	/* GPIO 配置 */
	stcGpioInit.u16PinDir = PIN_DIR_OUT;
	stcGpioInit.u16PinState = PIN_STAT_SET; // 默认拉高RX
	stcGpioInit.u16PullUp = PIN_PU_ON;		// 开启上拉电阻
	(void)GPIO_Init(CONTROL_PORT, CONTROL_PIN, &stcGpioInit);
}

/**
 * @brief  USART3	 初始化
 * @param  None
 * @retval None
 */
void USART_Config(void)
{
	stc_usart_uart_init_t stcUartInit;

	/* USART2的DMA初始化 */
	USART3_DMA_Config();
	/* TMR0初始化 */
	USART3_TMR0_Config(USART_TIMEOUT_BITS);
	/* 将USART2引脚复用为RX和TX,以及配置控制脚 */
	GPIO_SetFunction();
	/* 使能串口时钟 */
	USART_FCG_ENABLE();

	/* Initialize UART. */
	(void)USART_UART_StructInit(&stcUartInit);
	stcUartInit.u32ClockDiv = USART_CLK_DIV64;
	stcUartInit.u32CKOutput = USART_CK_OUTPUT_ENABLE;
	stcUartInit.u32Baudrate = USART_BAUDRATE;
	stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
	if (LL_OK != USART_UART_Init(USART_UNIT, &stcUartInit, NULL))
	{
		BSP_LED_On(LED_RED);
		for (;;)
		{
		}
	}
	Usart3_IrqInit();

	/* USART_RX 启用 USART2 的接收功能。
		USART_INT_RX 启用 USART2 的接收中断功能。
		USART_RX_TIMEOUT 启用 USART2 的接收超时功能。
		USART_INT_RX_TIMEOUT 启用 USART2 的接收超时中断功能。 */
	USART_FuncCmd(USART_UNIT, (USART_RX | USART_INT_RX | USART_RX_TIMEOUT | USART_INT_RX_TIMEOUT), ENABLE);
	CONTROL_RX();
	g_bUsartInitialized = true;
}

/**
 * @brief  USART dma 发送
 * @param  pData: 发送数据
 * @param  u16Len: 发送数据长度
 * @retval None
 */
void USART_Send(uint8_t *pData, uint16_t u16Len)
{
	CONTROL_TX();											 // 485开始发送
	DMA_SetSrcAddr(TX_DMA_UNIT, TX_DMA_CH, (uint32_t)pData); // 设置DMA源地址
	DMA_SetTransCount(TX_DMA_UNIT, TX_DMA_CH, u16Len);		 // 设置DMA传输数据块的数量
	(void)DMA_ChCmd(TX_DMA_UNIT, TX_DMA_CH, ENABLE);		 // 使能DMA通道，因为DMA 被初始化时没有启用循环模式，所以需要手动使能
	USART_FuncCmd(USART_UNIT, USART_TX, ENABLE);			 // 使能串口发送功能，在DMA发送完成后，会触发串口发送完成中断，从而停止发送
}
