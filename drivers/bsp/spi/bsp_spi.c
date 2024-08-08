
#include "bsp_spi.h"

/******************************变量定义*******************************/
static __IO en_flag_status_t enTxCompleteFlag = RESET;

/******************************函数定义*******************************/
/**
 * @brief  DMA 传输完成中断
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

    /* 配置GPIO */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDrv = PIN_HIGH_DRV;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(SPI_SCK_PORT, SPI_SCK_PIN, &stcGpioInit);
    (void)GPIO_Init(SPI_MOSI_PORT, SPI_MOSI_PIN, &stcGpioInit);
    (void)GPIO_Init(SPI_SS_PORT, SPI_SS_PIN, &stcGpioInit);

    /* 配置端口 */
    GPIO_SetFunc(SPI_SCK_PORT, SPI_SCK_PIN, SPI_SCK_FUNC);
    GPIO_SetFunc(SPI_MOSI_PORT, SPI_MOSI_PIN, SPI_MOSI_FUNC);
    // GPIO_SetFunc(SPI_SS_PORT, SPI_SS_PIN, SPI_SS_FUNC);

    /* 配置 SPI */
    FCG_Fcg1PeriphClockCmd(SPI_CLK, ENABLE);
    (void)SPI_StructInit(&stcSpiInit);
    stcSpiInit.u32WireMode = SPI_3_WIRE;               // 3线SPI
    stcSpiInit.u32TransMode = SPI_SEND_ONLY;           // 只发送
    stcSpiInit.u32MasterSlave = SPI_MASTER_SLAVE;      // 主机模式
    stcSpiInit.u32Parity = SPI_PARITY_INVD;            // 禁用奇偶校验
    stcSpiInit.u32SpiMode = SPI_MD_3;                  // SSD1306这个屏幕得使用MD_0模式
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV4; // 时钟分频
    stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;       // 数据位宽
    stcSpiInit.u32FirstBit = SPI_FIRST_MSB;            // 高位在前
    stcSpiInit.u32FrameLevel = SPI_1_FRAME;            // 1帧(就是在SPI的数据寄存器SPI_DR中必须写入多少帧数据后，这些数据才会被发送到传输缓冲区)
    (void)SPI_Init(SPI_UNIT, &stcSpiInit);
    SPI_Cmd(SPI_UNIT, ENABLE);

    /* 配置 DC 和 RST */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(SPI_DC_PORT, SPI_DC_PIN, &stcGpioInit);
    (void)GPIO_Init(SPI_RST_PORT, SPI_RST_PIN, &stcGpioInit);

    /* DMA 配置 */
    FCG_Fcg0PeriphClockCmd(DMA_CLK, ENABLE);
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = SPI_BUF_LEN;
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    /* 配置 TX */
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;          // 打开DMA中断
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;   // 源地址自增
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX; // 目标地址固定
    stcDmaInit.u32DestAddr = (uint32_t)(&SPI_UNIT->DR);
    if (LL_OK != DMA_Init(DMA_UNIT, DMA_TX_CH, &stcDmaInit))
    {
        for (;;)
        {
        }
    }
    AOS_SetTriggerEventSrc(DMA_TX_TRIG_CH, SPI_TX_EVT_SRC);

    /* DMA 中断配置 */
    stcIrqSignConfig.enIntSrc = DMA_TX_INT_SRC;
    stcIrqSignConfig.enIRQn = DMA_TX_IRQ_NUM;
    stcIrqSignConfig.pfnCallback = &DMA_TransCompleteCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
    NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

    /* 使能 DMA 和 通道 ，并且关闭 spi （方便后面用dma）*/
    SPI_Cmd(SPI_UNIT, DISABLE);
    DMA_Cmd(DMA_UNIT, ENABLE);
    DMA_ChCmd(DMA_UNIT, DMA_TX_CH, ENABLE);
}

void DMA_SPI_Send(void *TxBuf, uint16_t TxLen)
{
    enTxCompleteFlag = RESET;
    DMA_SetSrcAddr(DMA_UNIT, DMA_TX_CH, (uint32_t)TxBuf);
    DMA_SetTransCount(DMA_UNIT, DMA_TX_CH, TxLen);
    /* 使能 DMA 和 SPI */
    DMA_ChCmd(DMA_UNIT, DMA_TX_CH, ENABLE);
    SPI_Cmd(SPI_UNIT, ENABLE);
    /* 等待 DMA 传输完成中断 */
    while (enTxCompleteFlag == RESET)
    {
    }
    DMA_ChCmd(DMA_UNIT, DMA_TX_CH, DISABLE);
    SPI_Cmd(SPI_UNIT, DISABLE);
}
