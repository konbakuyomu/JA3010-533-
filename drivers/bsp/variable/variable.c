#include "variable.h"

/* 变量声明 ----------------------------------------------------------------*/
/*-------------------- FreeRTOS --------------------*/
/* 创建任务句柄 */
TaskHandle_t AppTaskCreate_Handle = NULL;
TaskHandle_t AstraTask_Handle = NULL;
TaskHandle_t LEDTask_Handle = NULL;
TaskHandle_t BTNTask_Handle = NULL;
TaskHandle_t vMemoryDefragTask_Handle = NULL; // 内存碎片整理任务句柄
/* 创建软件定时器句柄 */
// TimerHandle_t btn_tic_taskHandle;

/* 创建事件标志组句柄 */
EventGroupHandle_t xInit_EventGroup;

/* 函数声明 ----------------------------------------------------------------*/

/**
 * @brief  测试按键GPIO初始化
 * @param  none
 * @note   按键1：PE4 按键2：PE5
 * @return none
 */
void TEST_KEY_GPIO_Init(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(KEYIN1_PORT, KEYIN1_PIN, &stcGpioInit);
    (void)GPIO_Init(KEYIN2_PORT, KEYIN2_PIN, &stcGpioInit);
}

/**
 * @brief  按键读取电平
 * @param  [in] io  0:按键1 1:按键2
 * @retval 0:按键未按下 1:按键按下
 */
uint8_t btn_read_level(uint8_t io)
{
    if (io == 1)
    {
        return GPIO_ReadInputPins(KEYIN1_PORT, KEYIN1_PIN);
    }
    else if (io == 2)
    {
        return GPIO_ReadInputPins(KEYIN2_PORT, KEYIN2_PIN);
    }
    else
    {
        return GPIO_ReadInputPins(KEYIN2_PORT, KEYIN2_PIN);
    }
}

/**
 * @brief  TRNG 初始化配置。
 * @param  None
 * @retval None
 */
void TrngConfig(void)
{
#if (TRNG_USE_INTERRUPT > 0U)
    stc_irq_signin_config_t stcIrqRegiCfg;
#endif

    /* 使能 TRNG. */
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_TRNG, ENABLE);
    /* TRNG 初始化配置 */
    TRNG_Init(TRNG_SHIFT_CNT64, TRNG_RELOAD_INIT_VAL_ENABLE);

#if (TRNG_USE_INTERRUPT > 0U)
    /* Register IRQ handler && configure NVIC. */
    stcIrqRegiCfg.enIRQn = TRNG_INT_IRQn;
    stcIrqRegiCfg.enIntSrc = TRNG_INT_SRC;
    stcIrqRegiCfg.pfnCallback = &TRNG_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqRegiCfg);
    NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
    NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
#endif

    TRNG_Start();
}

/**
 * @brief  TMR0比较中断回调函数。
 * @param  None
 * @note   按键扫描心跳
 * @retval None
 */
static void TMR0_CompareIrqCallback(void)
{
    btn_tic_ms(BOTTON_TIC_MS);
    TMR0_ClearStatus(TMR0_UNIT, TMR0_CH_FLAG);
}

/**
 * @brief  TMR0定时器配置。
 * @param  None
 * @retval None
 */
void TMR0_Config(void)
{
    stc_tmr0_init_t stcTmr0Init;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* 使能 timer0 时钟 */
    FCG_Fcg2PeriphClockCmd(TMR0_CLK, ENABLE);

    /*  timer0 配置 */
    (void)TMR0_StructInit(&stcTmr0Init);
    stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_XTAL32;
    stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV;
    stcTmr0Init.u32Func = TMR0_FUNC_CMP;
    stcTmr0Init.u16CompareValue = (uint16_t)TMR0_CMP_VALUE;
    (void)TMR0_Init(TMR0_UNIT, TMR0_CH, &stcTmr0Init);

    /* 异步时钟源，写入TMR0寄存器需要等待三个异步时钟。 */
    DDL_DelayMS(1U);
    TMR0_IntCmd(TMR0_UNIT, TMR0_CH_INT, ENABLE);

    /* 中断配置 */
    stcIrqSignConfig.enIntSrc = TMR0_INT_SRC;
    stcIrqSignConfig.enIRQn = TMR0_IRQn;
    stcIrqSignConfig.pfnCallback = &TMR0_CompareIrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
    NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
}
