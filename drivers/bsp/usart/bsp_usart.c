#include "variable.h"

/* �������� ----------------------------------------------------------------*/
uint8_t m_au8RxBuf[APP_FRAME_LEN_MAX] = {0};

/* �������� ----------------------------------------------------------------*/
/**
 * @brief  DMA ��������жϻص�����
 * @param  None
 * @retval None
 */
static void RX_DMA_TC_IrqCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint16_t m_u16RxLen = APP_FRAME_LEN_MAX;

	// ʹ������֪ͨ����������
	xTaskNotifyFromISR(UsartTask_Handle, m_u16RxLen, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	// ���DMA������ɱ�־
	DMA_ClearTransCompleteStatus(RX_DMA_UNIT, RX_DMA_TC_FLAG);
}

/**
 * @brief  USART ���ճ�ʱ�жϻص�����
 * @param  None
 * @retval None
 */
static void USART_RxTimeout_IrqCallback(void)
{
	/* �����Ѿ����յ������ݵĳ���,singleTransCountΪ���δ�������ݳ��� */
	/* ��ʽΪ u16DataToSend = APP_FRAME_LEN_MAX (��󳤶�) - ��δ�����յ����ݵĳ���(��DMA_GetTransCount�ķ���ֵ) */
	//* DMA_GetTransCount() ��������� Probe_RxBuf(��ǰ��DMA���õĻ�����) �Ĺ�ϵ�ǣ����������� Probe_RxBuf �л��ж��ٿռ�û�б� DMA ���
	uint16_t u16DataToSend = APP_FRAME_LEN_MAX - (uint16_t)DMA_GetTransCount(RX_DMA_UNIT, RX_DMA_CH);

	if (u16DataToSend <= APP_FRAME_LEN_MAX)
	{
		// ʹ������֪ͨ����������
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		// ע�⣺ȷ�� u16DataToSend ��ֵ���ᳬ������֪ͨ�����ֵ
		xTaskNotifyFromISR(UsartTask_Handle, u16DataToSend, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	AOS_SW_Trigger();									  // ���AOS����DMA������
	TMR0_Stop(TMR0_TIMEOUT_UNIT, TMR0_TIMEOUT_CH);		  // ���ֶ����㣬��Ϊ���õ�Ӳ��������Ҫ��ʱ�Ŵ����ģ�ֹͣ��ʱ�������TMR0�Ļ������ƼĴ�����BCONR���������ã���ֹͣ��ʱ���ļ���
	USART_ClearStatus(USART_UNIT, USART_FLAG_RX_TIMEOUT); // ������ճ�ʱ��־λ
}

/**
 * @brief  USART ���մ����жϻص�����
 * @param  None
 * @retval None
 */
static void USART_RxError_IrqCallback(void)
{
	// ��ȡ���ڽ��ռĴ�������������ڽ��մ����־λ
	(void)USART_ReadData(USART_UNIT);
	// ������ڽ��մ����־λ
	USART_ClearStatus(USART_UNIT, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

/**
 * @brief  DMA ��������жϻص�����
 * @param  None
 * @retval None
 */
static void TX_DMA_TC_IrqCallback(void)
{
	// ʹ�ܴ��ڷ�������ж�
	USART_FuncCmd(USART_UNIT, USART_INT_TX_CPLT, ENABLE);
	// ���DMA������ɱ�־
	DMA_ClearTransCompleteStatus(TX_DMA_UNIT, TX_DMA_TC_FLAG);
}

/**
 * @brief  USART ��������жϻص�����
 * @param  None
 * @retval None
 */
static void USART_TxComplete_IrqCallback(void)
{
	/* �رմ��ڷ��͹��ܺͷ�������ж� */
	USART_FuncCmd(USART_UNIT, (USART_TX | USART_INT_TX_CPLT), DISABLE);
	/* ���������ɱ�־λ */
	USART_ClearStatus(USART_UNIT, USART_FLAG_TX_CPLT);
	/* ʹ��485�������Ž���ģʽ */
	CONTROL_RX();
}

/**
 * @brief  USART3 DMA ����
 * @param  None
 * @retval None
 */
static int32_t USART3_DMA_Config(void)
{
	int32_t i32Ret;
	stc_dma_init_t stcDmaInit;
	stc_dma_llp_init_t stcDmaLlpInit;			// DMA����ָ�룬����ָ�루LLP����DMA��ִ��һ��DMA����������Զ�������һ��DMA����Ļ���
	stc_irq_signin_config_t stcIrqSignConfig;	// �ж����ýṹ��
	static stc_dma_llp_descriptor_t stcLlpDesc; // DMA����ָ��

	/* DMA&AOS FCG enable��ʹ��ʱ�ӣ� */
	FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA2, ENABLE);
	FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);

	/* ���� USART_RX_DMA */
	(void)DMA_StructInit(&stcDmaInit);						// DMA��ʼ���ṹ������
	stcDmaInit.u32IntEn = DMA_INT_ENABLE;					// ����DMA�ж�
	stcDmaInit.u32BlockSize = 1UL;							// ָ��DMA�Ŀ��С��Ҳ����һ��DMA����Ӧ����������ݿ�Ĵ�С��
	stcDmaInit.u32TransCount = ARRAY_SZ(m_au8RxBuf);		// �õ����鳤��
	stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;			// ����ֶ�ָ��DMA�������ݵĿ�ȣ�Ҳ����һ���Դ�������ݵ�λ����8��16��32��
	stcDmaInit.u32DestAddr = (uint32_t)m_au8RxBuf;			// ָ��DMA��Ŀ���ַ
	stcDmaInit.u32SrcAddr = ((uint32_t)(&USART_UNIT->RDR)); // ָ��DMA��Դ��ַ��Ҳ����DMA���������ַ��ʼ��ȡ���ݣ���оƬ�ֲ��֪�������ݼĴ����Ǵ�16λ��ʼ�ģ�������RDR�Ĵ���
	stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;			// Դ��ַ�̶�����
	stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;			// ��ʾĿ���ַÿ������
	i32Ret = DMA_Init(RX_DMA_UNIT, RX_DMA_CH, &stcDmaInit); // ��ʼ��DMA
	if (LL_OK == i32Ret)
	{
		//--------------------------------------------------------------------
		(void)DMA_LlpStructInit(&stcDmaLlpInit);	   // ��ʼ��DMA����ָ��ṹ��
		stcDmaLlpInit.u32State = DMA_LLP_ENABLE;	   // DMA��LLP�����Ƿ�����
		stcDmaLlpInit.u32Mode = DMA_LLP_WAIT;		   // �ȴ���һ������������ִ��
		stcDmaLlpInit.u32Addr = (uint32_t)&stcLlpDesc; // ָ����DMA������ָ���ַ��������LLP����ʱ��DMA��ʹ�������ַ��Ϊ�������ʼ��ַ
		(void)DMA_LlpInit(RX_DMA_UNIT, RX_DMA_CH, &stcDmaLlpInit);
		//--------------------------------------------------------------------

		stcLlpDesc.SARx = stcDmaInit.u32SrcAddr;  // LLPԴ��ַ����LLPģʽ�£�DMA�Ӵ˵�ַ��ʼ��ȡ���ݡ�
		stcLlpDesc.DARx = stcDmaInit.u32DestAddr; // LLPĿ���ַ����LLPģʽ�£�DMA������д��˵�ַ��
		stcLlpDesc.DTCTLx = (stcDmaInit.u32TransCount << DMA_DTCTL_CNT_POS) | (stcDmaInit.u32BlockSize << DMA_DTCTL_BLKSIZE_POS);
		;										 // ÿ��DMA������������ݿ�������ʹ�С
		stcLlpDesc.LLPx = (uint32_t)&stcLlpDesc; // LLP��һ������ָ��
		stcLlpDesc.CHCTLx = stcDmaInit.u32SrcAddrInc | stcDmaInit.u32DestAddrInc | stcDmaInit.u32DataWidth |
							stcDmaInit.u32IntEn | stcDmaLlpInit.u32State | stcDmaLlpInit.u32Mode; // LLPͨ�����ơ����ֶζ�����DMAͨ���Ŀ�����Ϣ��

		DMA_ReconfigLlpCmd(RX_DMA_UNIT, RX_DMA_CH, ENABLE); // DMA�������ù������û��������ָ�루LLP��
		DMA_ReconfigCmd(RX_DMA_UNIT, ENABLE);				// ���û����DMA�������ù���
		/* ���ô����¼�Դ */
		// ��һ������u32Target������Ҫ��AOSԴ������Ŀ�꣬����RX_DMA_RECONF_TRIG_SEL��ֵ��AOS_DMA_RC����DMA�����ô���ѡ�����ĵ�ַ�����ֵ��AOS�ж���ΪDMA�Ĵ���ѡ���������õĵ�ַ
		// �ڶ�������enSource�Ǵ���AOSĿ���AOSԴ�����ֵ��AOS�Ĵ���Դ�������������ָ��һ���ض����¼�Դ
		// ����AOS��DMA�����ô���ѡ������ѡ��Ĵ���ԴΪEVT_SRC_AOS_STRG�¼�����ͨ���������INTSFTTRG�Ĵ�����SFTGλ���������¼���������¼�����ʱ�����ᴥ��DMA��������
		//* �����AOS�������õ���CPU��Դ����Ӳ���Զ������ģ����Բ���Ӱ��CPU������Ч��
		//! ˵����һ�仰������Դ���¼�Դ���߼�˳���ǣ���ָ����Ҫ��������Ŀ�꣨����Դ - ����1����Ȼ��ָ������Դ���¼����ͣ��¼�Դ - ����2��
		// ��USART_RxTimeout_IrqCallback��������У����ֶ�����һ���¼�Դ������¼�Դ����EVT_SRC_AOS_STRG��������¼�Դ����ʱ��Ҳ���ǳ�ʱ��ʱ�򣩣��ͻᴥ��DMA��������
		// ����ʱ��ʱ�򣬻ᴥ��EVT_SRC_AOS_STRG�¼�������¼��ᴥ��DMA�������ã�DMA�����û���������DMA�Ĵ��������Ȼ�����¿�ʼDMA���䣬������ʵ����DMA��ѭ������
		AOS_SetTriggerEventSrc(RX_DMA_RECONF_TRIG_SEL, RX_DMA_RECONF_TRIG_EVT_SRC);

		stcIrqSignConfig.enIntSrc = RX_DMA_TC_INT_SRC;		   // ָ���ж�Դ
		stcIrqSignConfig.enIRQn = RX_DMA_TC_IRQn;			   // ָ���ж���������
		stcIrqSignConfig.pfnCallback = &RX_DMA_TC_IrqCallback; // ָ���жϷ�����
		(void)INTC_IrqSignIn(&stcIrqSignConfig);
		NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
		NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
		NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

		AOS_SetTriggerEventSrc(RX_DMA_TRIG_SEL, RX_DMA_TRIG_EVT_SRC); // ���յ����ڽ����ж�ʱ������DMA2ͨ��0�Ĵ�������

		DMA_Cmd(RX_DMA_UNIT, ENABLE);								 // ʹ��DMA
		DMA_TransCompleteIntCmd(RX_DMA_UNIT, RX_DMA_TC_INT, ENABLE); // ʹ��DMA��������жϣ�����⺯��������������DMA��������жϵ�ʹ��״̬��
		(void)DMA_ChCmd(RX_DMA_UNIT, RX_DMA_CH, ENABLE);			 // ʹ��DMAͨ��
	}

	uint8_t Probe_TxBuf[APP_FRAME_LEN_MAX]; // ��ʱ̽ͷ���ڷ�������

	/* ���� USART_TX_DMA */
	(void)DMA_StructInit(&stcDmaInit); // DMA��ʼ���ṹ������
	stcDmaInit.u32IntEn = DMA_INT_ENABLE;
	stcDmaInit.u32BlockSize = 1UL;
	stcDmaInit.u32TransCount = ARRAY_SZ(Probe_TxBuf);
	stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
	stcDmaInit.u32DestAddr = (uint32_t)(&USART_UNIT->TDR); // ָ��DMA��Դ��ַ��Ҳ����DMA���������ַ��ʼ��ȡ���ݣ���оƬ�ֲ��֪�������ݼĴ����Ǵ�0λ��ʼ��,���Բ���Ҫ+2U
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

		AOS_SetTriggerEventSrc(TX_DMA_TRIG_SEL, TX_DMA_TRIG_EVT_SRC); // ���յ����ڷ�������ж�ʱ������DMA2ͨ��1�Ĵ�������

		DMA_Cmd(TX_DMA_UNIT, ENABLE);								 // ʹ��DMA
		DMA_TransCompleteIntCmd(TX_DMA_UNIT, TX_DMA_TC_INT, ENABLE); // ʹ��DMA��������ж�
	}
	return i32Ret;
}

/**
 * @brief  TMR0 ����(���ڳ�ʱ��������)
 * @param  u16TimeoutBits: ��ʱʱ��
 *
 * @note   USART1��Timer0 Unit1 A ͨ��
 * @note   USART2��Timer0 Unit1 B ͨ��
 * @note   USART3��Timer0 Unit2 A ͨ��
 * @note   USART4��Timer0 Unit2 B ͨ��
 *
 * @retval None
 */
static void USART3_TMR0_Config(uint16_t u16TimeoutBits)
{
	// ����δ����У�TMR0��ʱ�������������ڼ�⴮�ڽ������ݵĳ�ʱ�����
	// ������˵�������ڽ��յ����ݺ�TMR0��ʱ����ʼ��ʱ�������һ��ʱ����û�н��յ�����������֡���ͻᴥ�����ڽ��ճ�ʱ�жϣ�
	// �Ӷ�ֹͣ�������ݲ����к������������ʱʱ�����ͨ��USART_TIMEOUT_BITS�궨�������ã���λΪTMR0��ʱ�����ڡ�
	uint16_t u16Div;
	uint16_t u16Delay;
	uint16_t u16CompareValue;
	stc_tmr0_init_t stcTmr0Init;

	TMR0_FCG_ENABLE();

	stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_XTAL32; // ʱ��ԴΪ�ⲿ32.768KHz
	stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV8;	   // ʱ�ӷ�ƵΪ8
	stcTmr0Init.u32Func = TMR0_FUNC_CMP;		   // TMR0_CMP_MODE �����ڱȽ�ģʽ
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

	u16Div = (uint16_t)1U << (stcTmr0Init.u32ClockDiv >> TMR0_BCONR_CKDIVA_POS); // �����Ƶϵ��
	u16CompareValue = ((u16TimeoutBits + u16Div - 1U) / u16Div) - u16Delay;		 // ����Ƚ�ֵ
	stcTmr0Init.u16CompareValue = u16CompareValue;								 // ���ñȽ�ֵ
	(void)TMR0_Init(TMR0_TIMEOUT_UNIT, TMR0_TIMEOUT_CH, &stcTmr0Init);			 // ��ʼ��TMR0

	TMR0_HWStartCondCmd(TMR0_TIMEOUT_UNIT, TMR0_TIMEOUT_CH, ENABLE); // ����TMR0��Ӳ��������������Ӳ��������������ʱ��TMR0�Ὺʼ����������Ӳ�������������ǵ����ڽ��ջ��������յ���1���ֽڿ�ʼ����
	TMR0_HWClearCondCmd(TMR0_TIMEOUT_UNIT, TMR0_TIMEOUT_CH, ENABLE); // ����TMR0��Ӳ��������������Ӳ��������������ʱ��TMR0�ᱻ���㡣������Ӳ���رյ������Ǽ�⵽ TIMEOUT ���Զ��رն�ʱ��������
}

/**
 * @brief  USART3 �жϳ�ʼ��
 * @param  None
 * @retval None
 */
static void Usart3_IrqInit(void)
{
	stc_irq_signin_config_t stcIrqSigninConfig;

	/* ע�� USART ���մ����жϴ����� */
	stcIrqSigninConfig.enIRQn = USART_RX_ERR_IRQn;
	stcIrqSigninConfig.enIntSrc = USART_RX_ERR_INT_SRC;
	stcIrqSigninConfig.pfnCallback = &USART_RxError_IrqCallback;
	(void)INTC_IrqSignIn(&stcIrqSigninConfig);
	NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
	NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
	NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

	/* ע�� USART ���ճ�ʱ�жϴ����� */
	stcIrqSigninConfig.enIRQn = USART_RX_TIMEOUT_IRQn;
	stcIrqSigninConfig.enIntSrc = USART_RX_TIMEOUT_INT_SRC;
	stcIrqSigninConfig.pfnCallback = &USART_RxTimeout_IrqCallback;
	(void)INTC_IrqSignIn(&stcIrqSigninConfig);
	NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
	NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
	NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

	/* ע�� USART ��������жϴ����� */
	stcIrqSigninConfig.enIRQn = USART_TX_CPLT_IRQn;
	stcIrqSigninConfig.enIntSrc = USART_TX_CPLT_INT_SRC;
	stcIrqSigninConfig.pfnCallback = &USART_TxComplete_IrqCallback;
	(void)INTC_IrqSignIn(&stcIrqSigninConfig);
	NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
	NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
	NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);
}

/**
 * @brief  GPIO ����
 * @note   �����Ÿ���ΪRX��TX�������ÿ��ƽ�
 * @param  None
 * @retval None
 */
static void GPIO_SetFunction(void)
{
	stc_gpio_init_t stcGpioInit;

	/* Configure USART RX/TX pin. */
	GPIO_SetFunc(USART_RX_PORT, USART_RX_PIN, USART_RX_GPIO_FUNC);
	GPIO_SetFunc(USART_TX_PORT, USART_TX_PIN, USART_TX_GPIO_FUNC);

	/* GPIO���ýṹ���ʼ�� */
	(void)GPIO_StructInit(&stcGpioInit);
	/* GPIO ���� */
	stcGpioInit.u16PinDir = PIN_DIR_OUT;
	stcGpioInit.u16PinState = PIN_STAT_SET; // Ĭ������RX
	stcGpioInit.u16PullUp = PIN_PU_ON;		// ������������
	(void)GPIO_Init(CONTROL_PORT, CONTROL_PIN, &stcGpioInit);
}

/**
 * @brief  USART3	 ��ʼ��
 * @param  None
 * @retval None
 */
void USART_Config(void)
{
	stc_usart_uart_init_t stcUartInit;

	/* USART2��DMA��ʼ�� */
	USART3_DMA_Config();
	/* TMR0��ʼ�� */
	USART3_TMR0_Config(USART_TIMEOUT_BITS);
	/* ��USART2���Ÿ���ΪRX��TX,�Լ����ÿ��ƽ� */
	GPIO_SetFunction();
	/* ʹ�ܴ���ʱ�� */
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

	/* USART_RX ���� USART2 �Ľ��չ��ܡ�
		USART_INT_RX ���� USART2 �Ľ����жϹ��ܡ�
		USART_RX_TIMEOUT ���� USART2 �Ľ��ճ�ʱ���ܡ�
		USART_INT_RX_TIMEOUT ���� USART2 �Ľ��ճ�ʱ�жϹ��ܡ� */
	USART_FuncCmd(USART_UNIT, (USART_RX | USART_INT_RX | USART_RX_TIMEOUT | USART_INT_RX_TIMEOUT), ENABLE);
	CONTROL_RX();
	g_bUsartInitialized = true;
}

/**
 * @brief  USART dma ����
 * @param  pData: ��������
 * @param  u16Len: �������ݳ���
 * @retval None
 */
void USART_Send(uint8_t *pData, uint16_t u16Len)
{
	CONTROL_TX();											 // 485��ʼ����
	DMA_SetSrcAddr(TX_DMA_UNIT, TX_DMA_CH, (uint32_t)pData); // ����DMAԴ��ַ
	DMA_SetTransCount(TX_DMA_UNIT, TX_DMA_CH, u16Len);		 // ����DMA�������ݿ������
	(void)DMA_ChCmd(TX_DMA_UNIT, TX_DMA_CH, ENABLE);		 // ʹ��DMAͨ������ΪDMA ����ʼ��ʱû������ѭ��ģʽ��������Ҫ�ֶ�ʹ��
	USART_FuncCmd(USART_UNIT, USART_TX, ENABLE);			 // ʹ�ܴ��ڷ��͹��ܣ���DMA������ɺ󣬻ᴥ�����ڷ�������жϣ��Ӷ�ֹͣ����
}
