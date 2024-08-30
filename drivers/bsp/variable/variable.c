#include "variable.h"

/* 变量声明 ----------------------------------------------------------------*/
/*-------------------- FreeRTOS --------------------*/
/* 创建任务句柄 */
TaskHandle_t AppTaskCreate_Handle = NULL;
TaskHandle_t AstraTask_Handle = NULL;
TaskHandle_t LEDTask_Handle = NULL;
TaskHandle_t BTNTask_Handle = NULL;
TaskHandle_t CanTxTask_Handle = NULL;
TaskHandle_t CanRxTask_Handle = NULL;
TaskHandle_t UsartTask_Handle = NULL;
/* 创建队列句柄 */
QueueHandle_t xQueue_CanTx = NULL;             // CAN发送队列
QueueHandle_t xQueue_ProbeInfoTransfer = NULL; // 警告更新队列
/* 创建事件标志组句柄 */
EventGroupHandle_t xInit_EventGroup;
EventGroupHandle_t xProbeDataSendEventGroup;
/* 创建软件定时器句柄 */
TimerHandle_t xDoseRateTimer = NULL;
TimerHandle_t xBeepTimer = NULL;
TimerHandle_t xProbe1AlarmTimer = NULL;
TimerHandle_t xProbe2AlarmTimer = NULL;
TimerHandle_t xProbe3AlarmTimer = NULL;
TimerHandle_t xProbe4AlarmTimer = NULL;

/*-------------------- EEPROM --------------------*/
ProbeData data = {0};

/*-------------------- 标志位 --------------------*/
bool g_bUsartInitialized = false; // 串口初始化完成标志位

/* 函数声明 ----------------------------------------------------------------*/

/**
 * @brief 将浮点数格式化为字符串，根据数值大小自动调整小数位数
 * @param value 要格式化的浮点数
 * @param buffer 用于存储格式化结果的字符串缓冲区
 * @param bufferSize 缓冲区大小
 */
void formatFloat(float value, char *buffer, size_t bufferSize)
{
    int scaledValue;
    if (value < 10.0f)
    {
        // 对于小于10的数，保留三位小数
        scaledValue = (int)(value * 1000);
        snprintf(buffer, bufferSize, "%d.%03d", scaledValue / 1000, scaledValue % 1000);
        scaledValue = 0; // 重置scaledValue，避免后续使用出错
    }
    else if (value < 100.0f && value >= 10.0f)
    {
        // 对于10到100之间的数，保留两位小数
        scaledValue = (int)(value * 100);
        snprintf(buffer, bufferSize, "%d.%02d", scaledValue / 100, scaledValue % 100);
    }
    else if (value < 1000.0f && value >= 100.0f)
    {
        // 对于100到1000之间的数，保留一位小数
        scaledValue = (int)(value * 10);
        snprintf(buffer, bufferSize, "%d.%01d", scaledValue / 10, scaledValue % 10);
    }
    else
    {
        // 对于1000及以上的数，只保留整数部分
        scaledValue = (int)value;
        snprintf(buffer, bufferSize, "%d", scaledValue);
    }
}

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
    (void)GPIO_Init(KEYIN3_PORT, KEYIN3_PIN, &stcGpioInit);
    (void)GPIO_Init(KEYIN4_PORT, KEYIN4_PIN, &stcGpioInit);
}

/**
 * @brief  蜂鸣器GPIO初始化
 * @param  none
 * @note   蜂鸣器：PD15
 * @return none
 */
void BEEP_GPIO_Init(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(BEEP_PORT, BEEP_PIN, &stcGpioInit);
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
    else if (io == 3)
    {
        return GPIO_ReadInputPins(KEYIN3_PORT, KEYIN3_PIN);
    }
    else if (io == 4)
    {
        return GPIO_ReadInputPins(KEYIN4_PORT, KEYIN4_PIN);
    }
    else
    {
        return 1;
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

/**
 * @brief  剂量率上传定时器回调函数
 * @param  xTimer 定时器句柄
 * @retval None
 */
void vDoseRateTimerCallback(TimerHandle_t xTimer)
{
    // 在这里添加提醒探头上传剂量率和累积剂量的代码
    uint8_t probeID = CAN_PROBE1_ID;
    key_value_msg("CAN_UploadDose", &probeID, sizeof(probeID));
    probeID = CAN_PROBE2_ID;
    key_value_msg("CAN_UploadDose", &probeID, sizeof(probeID));
    probeID = CAN_PROBE3_ID;
    key_value_msg("CAN_UploadDose", &probeID, sizeof(probeID));
    probeID = CAN_PROBE4_ID;
    key_value_msg("CAN_UploadDose", &probeID, sizeof(probeID));
}

/**
 * @brief  探头1报警定时器回调函数
 * @param  xTimer 定时器句柄
 * @note   探头1报警弹窗(设定时间查看探头1报警标志位，如果没有报警标志表示可以使能下一次弹窗)
 * @retval None
 */
void vProbe1AlarmTimerCallback(TimerHandle_t xTimer)
{
    if (!(xEventGroupGetBits(xProbeDataSendEventGroup) & PROBE1_ALARM_FLAG))
    {
        xEventGroupSetBits(xProbeDataSendEventGroup, PROBE1_POPUP_FLAG);
        xTimerStop(xTimer, 0);  // 关闭自身软件定时器
    }
}

/**
 * @brief  探头2报警定时器回调函数
 * @param  xTimer 定时器句柄
 * @note   探头2报警弹窗(设定时间查看探头2报警标志位，如果没有报警标志表示可以使能下一次弹窗)
 * @retval None
 */
void vProbe2AlarmTimerCallback(TimerHandle_t xTimer)
{
    if (!(xEventGroupGetBits(xProbeDataSendEventGroup) & PROBE2_ALARM_FLAG))
    {
        xEventGroupSetBits(xProbeDataSendEventGroup, PROBE2_POPUP_FLAG);
        xTimerStop(xTimer, 0);  // 关闭自身软件定时器
    }
}

/**
 * @brief  探头3报警定时器回调函数
 * @param  xTimer 定时器句柄
 * @note   探头3报警弹窗(设定时间查看探头3报警标志位，如果没有报警标志表示可以使能下一次弹窗)
 * @retval None
 */
void vProbe3AlarmTimerCallback(TimerHandle_t xTimer)
{
    if (!(xEventGroupGetBits(xProbeDataSendEventGroup) & PROBE3_ALARM_FLAG))
    {
        xEventGroupSetBits(xProbeDataSendEventGroup, PROBE3_POPUP_FLAG);
        xTimerStop(xTimer, 0);  // 关闭自身软件定时器
    }
}

/**
 * @brief  探头4报警定时器回调函数
 * @param  xTimer 定时器句柄
 * @note   探头4报警弹窗(设定时间查看探头4报警标志位，如果没有报警标志表示可以使能下一次弹窗)
 * @retval None
 */
void vProbe4AlarmTimerCallback(TimerHandle_t xTimer)
{
    if (!(xEventGroupGetBits(xProbeDataSendEventGroup) & PROBE4_ALARM_FLAG))
    {
        xEventGroupSetBits(xProbeDataSendEventGroup, PROBE4_POPUP_FLAG);
        xTimerStop(xTimer, 0);  // 关闭自身软件定时器
    }
}

/**
 * @brief  蜂鸣器定时器回调函数
 * @param  xTimer 定时器句柄
 * @retval None
 */
void vBeepTimerCallback(TimerHandle_t xTimer)
{
    BEEP_TOGGLE();
}

/**
 * @brief  串口发送数据键值对回调函数
 * @param  value 发送数据
 * @param  lenth 发送数据长度
 * @retval None
 */
static void UART_sendData_cb(void *value, size_t lenth)
{
    USART_Send((uint8_t *)value, (uint16_t)lenth);
}

/**
 * @brief  CAN总线发送自检命令的键值对回调函数
 * @param  value 探头ID的指针
 * @param  lenth 数据长度（此处未使用）
 * @retval 无
 */
static void CAN_SendSelfCheckCommand_cb(void *value, size_t lenth)
{
    uint8_t u8ProbeID = *(uint8_t *)value;
    CAN_SendSelfCheckCommand(u8ProbeID);
}

/**
 * @brief  CAN总线发送累计剂量清零命令的键值对回调函数
 * @param  value 探头ID的指针
 * @param  lenth 数据长度（此处未使用）
 * @retval 无
 */
static void CAN_SendClearCumulativeDoseCommand_cb(void *value, size_t lenth)
{
    uint8_t u8ProbeID = *(uint8_t *)value;
    CAN_SendClearCumulativeDoseCommand(u8ProbeID);
}

/**
 * @brief  CAN总线发送上传剂量率和累积剂量命令的键值对回调函数
 * @param  value 探头ID的指针
 * @param  lenth 数据长度（此处未使用）
 * @retval 无
 */
static void CAN_SendUploadDoseCommand_cb(void *value, size_t lenth)
{
    uint8_t u8ProbeID = *(uint8_t *)value;
    CAN_SendUploadDoseRateAndCumulativeDoseCommand(u8ProbeID);
}

/**
 * @brief  蜂鸣器定时器开始回调函数
 * @param  value 探头ID的指针
 * @param  lenth 数据长度（此处未使用）
 * @retval 无
 */
static void Beep_Start_cb(void *value, size_t lenth)
{
    BEEP_OFF();
    xTimerStart(xBeepTimer, 0);
}

/**
 * @brief  蜂鸣器定时器停止回调函数
 * @param  value 探头ID的指针
 * @param  lenth 数据长度（此处未使用）
 * @retval 无
 */
static void Beep_Stop_cb(void *value, size_t lenth)
{
    xTimerStop(xBeepTimer, 0);
    BEEP_OFF();
}

/**
 * @brief  探头1报警定时器开始回调函数
 * @param  value 探头ID的指针
 * @param  lenth 数据长度（此处未使用）
 * @retval 无
 */
static void Probe1_Alarm_Start_cb(void *value, size_t lenth)
{
    xTimerStart(xProbe1AlarmTimer, 0);
}

/**
 * @brief  探头2报警定时器开始回调函数
 * @param  value 探头ID的指针
 * @param  lenth 数据长度（此处未使用）
 * @retval 无
 */
static void Probe2_Alarm_Start_cb(void *value, size_t lenth)
{
    xTimerStart(xProbe2AlarmTimer, 0);
}

/**
 * @brief  探头3报警定时器开始回调函数
 * @param  value 探头ID的指针
 * @param  lenth 数据长度（此处未使用）
 * @retval 无
 */
static void Probe3_Alarm_Start_cb(void *value, size_t lenth)
{
    xTimerStart(xProbe3AlarmTimer, 0);
}

/**
 * @brief  探头4报警定时器开始回调函数
 * @param  value 探头ID的指针
 * @param  lenth 数据长度（此处未使用）
 * @retval 无
 */
static void Probe4_Alarm_Start_cb(void *value, size_t lenth)
{
    xTimerStart(xProbe4AlarmTimer, 0);
}

/**
 * @brief  键值对注册
 * @param  None
 * @retval None
 */
void Key_Value_Init(void)
{
    key_value_register(NULL, "UART_sendData", UART_sendData_cb);                      // 发送串口数据
    key_value_register(NULL, "CAN_SelfCheck", CAN_SendSelfCheckCommand_cb);           // 发送CAN自检命令
    key_value_register(NULL, "CAN_ClearDose", CAN_SendClearCumulativeDoseCommand_cb); // 发送CAN累计剂量清零命令
    key_value_register(NULL, "CAN_UploadDose", CAN_SendUploadDoseCommand_cb);         // 发送CAN上传剂量率和累积剂量命令
    key_value_register(NULL, "Beep_Start", Beep_Start_cb);                            // 蜂鸣器开始
    key_value_register(NULL, "Beep_Stop", Beep_Stop_cb);                              // 蜂鸣器停止
    key_value_register(NULL, "Probe1_Alarm_Start", Probe1_Alarm_Start_cb);            // 探头1报警开始
    key_value_register(NULL, "Probe2_Alarm_Start", Probe2_Alarm_Start_cb);            // 探头2报警开始
    key_value_register(NULL, "Probe3_Alarm_Start", Probe3_Alarm_Start_cb);            // 探头3报警开始
    key_value_register(NULL, "Probe4_Alarm_Start", Probe4_Alarm_Start_cb);            // 探头4报警开始
}
