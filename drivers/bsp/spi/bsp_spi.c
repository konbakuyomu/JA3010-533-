
#include "bsp_spi.h"

/******************************��������*******************************/
static __IO en_flag_status_t enTxCompleteFlag = RESET;

/******************************��������*******************************/
/**
 * @brief  DMA ��������ж�
 * @param  None
 * @retval None
 */
static void DMA_TransCompleteCallback(void);

static void DMA_TransCompleteCallback(void)
{
    enTxCompleteFlag = SET;
    DMA_ClearTransCompleteStatus(DMA_UNIT, DMA_TX_INT_CH);
}

void SPI_Config(void)
{
    stc_spi_init_t stcSpiInit;
    stc_dma_init_t stcDmaInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    stc_gpio_init_t stcGpioInit;

    /* ����GPIO */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDrv = PIN_HIGH_DRV;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(SPI_SCK_PORT, SPI_SCK_PIN, &stcGpioInit);
    (void)GPIO_Init(SPI_MOSI_PORT, SPI_MOSI_PIN, &stcGpioInit);
    (void)GPIO_Init(SPI_SS_PORT, SPI_SS_PIN, &stcGpioInit);

    /* ���ö˿� */
    GPIO_SetFunc(SPI_SCK_PORT, SPI_SCK_PIN, SPI_SCK_FUNC);
    GPIO_SetFunc(SPI_MOSI_PORT, SPI_MOSI_PIN, SPI_MOSI_FUNC);
    // GPIO_SetFunc(SPI_SS_PORT, SPI_SS_PIN, SPI_SS_FUNC);

    /* ���� SPI */
    FCG_Fcg1PeriphClockCmd(SPI_CLK, ENABLE);
    (void)SPI_StructInit(&stcSpiInit);
    stcSpiInit.u32WireMode = SPI_3_WIRE;               // 3��SPI
    stcSpiInit.u32TransMode = SPI_SEND_ONLY;           // ֻ����
    stcSpiInit.u32MasterSlave = SPI_MASTER_SLAVE;      // ����ģʽ
    stcSpiInit.u32Parity = SPI_PARITY_INVD;            // ������żУ��
    stcSpiInit.u32SpiMode = SPI_MD_3;                  // SSD1306�����Ļ��ʹ��MD_0ģʽ
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV4; // ʱ�ӷ�Ƶ
    stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;       // ����λ��
    stcSpiInit.u32FirstBit = SPI_FIRST_MSB;            // ��λ��ǰ
    stcSpiInit.u32FrameLevel = SPI_1_FRAME;            // 1֡(������SPI�����ݼĴ���SPI_DR�б���д�����֡���ݺ���Щ���ݲŻᱻ���͵����仺����)
    (void)SPI_Init(SPI_UNIT, &stcSpiInit);
    SPI_Cmd(SPI_UNIT, ENABLE);

    /* ���� DC �� RST */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(SPI_DC_PORT, SPI_DC_PIN, &stcGpioInit);
    (void)GPIO_Init(SPI_RST_PORT, SPI_RST_PIN, &stcGpioInit);

    /* DMA ���� */
    FCG_Fcg0PeriphClockCmd(DMA_CLK, ENABLE);
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = SPI_BUF_LEN;
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    /* ���� TX */
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;          // ��DMA�ж�
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;   // Դ��ַ����
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX; // Ŀ���ַ�̶�
    stcDmaInit.u32DestAddr = (uint32_t)(&SPI_UNIT->DR);
    if (LL_OK != DMA_Init(DMA_UNIT, DMA_TX_CH, &stcDmaInit))
    {
        for (;;)
        {
        }
    }
    AOS_SetTriggerEventSrc(DMA_TX_TRIG_CH, SPI_TX_EVT_SRC);

    /* DMA �ж����� */
    stcIrqSignConfig.enIntSrc = DMA_TX_INT_SRC;
    stcIrqSignConfig.enIRQn = DMA_TX_IRQ_NUM;
    stcIrqSignConfig.pfnCallback = &DMA_TransCompleteCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
    NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

    /* ʹ�� DMA �� ͨ�� �����ҹر� spi �����������dma��*/
    SPI_Cmd(SPI_UNIT, DISABLE);
    DMA_Cmd(DMA_UNIT, ENABLE);
    DMA_ChCmd(DMA_UNIT, DMA_TX_CH, ENABLE);
}

void DMA_SPI_Send(void *TxBuf, uint16_t TxLen)
{
    enTxCompleteFlag = RESET;
    DMA_SetSrcAddr(DMA_UNIT, DMA_TX_CH, (uint32_t)TxBuf);
    DMA_SetTransCount(DMA_UNIT, DMA_TX_CH, TxLen);
    /* ʹ�� DMA �� SPI */
    DMA_ChCmd(DMA_UNIT, DMA_TX_CH, ENABLE);
    SPI_Cmd(SPI_UNIT, ENABLE);
    /* �ȴ� DMA ��������ж� */
    while (enTxCompleteFlag == RESET)
    {
    }
    DMA_ChCmd(DMA_UNIT, DMA_TX_CH, DISABLE);
    SPI_Cmd(SPI_UNIT, DISABLE);
}
