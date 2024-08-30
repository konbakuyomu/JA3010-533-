/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "variable.h"
#include "astra_rocket.h"
// #include "memory_pool.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
static void AppTaskCreate(void *pvParameters);
static void AstraTask(void *pvParameters);
static void UsartTask(void *pvParameters);

/**
 * @brief  初始化板级外设
 *
 * @details 此函数执行以下操作：
 *          1. 初始化系统时钟
 *          2. 初始化32k外部晶振
 *          3. SPI初始化
 * @note
 *
 * @return NULL 无返回值
 */
void BSP_Init(void)
{
  /* 将系统时钟配置为200MHz */
  BSP_CLK_Init();
  /* 外部时钟源32kb初始化 */
  BSP_XTAL32_Init();
  /*  (测试)LED初始化 */
  BSP_LED_Init();
  /* USART初始化 */
  USART_Config();
  /* EEPROM初始化 */
  BSP_24CXX_Init();
  memset(&data, 0, sizeof(ProbeData));
  // data.probe1_realtime_dose = 0;
  // data.probe2_realtime_dose = 0;
  // data.probe3_realtime_dose = 0;
  // data.probe4_realtime_dose = 0;
  // data.probe1_cumulative_dose = 0;
  // data.probe2_cumulative_dose = 0;
  // data.probe3_cumulative_dose = 0;
  // data.probe4_cumulative_dose = 0;
  // data.probe1_cumulative_alarm_threshold = 100000;
  // data.probe2_cumulative_alarm_threshold = 10;
  // data.probe3_cumulative_alarm_threshold = 10;
  // data.probe4_cumulative_alarm_threshold = 10;
  // data.probe1_realtime_alarm_threshold = 10;
  // data.probe2_realtime_alarm_threshold = 10;
  // data.probe3_realtime_alarm_threshold = 10;
  // data.probe4_realtime_alarm_threshold = 10;
  // BSP_24CXX_Write(EEPROM_BASE_ADDR, (uint8_t *)&data, sizeof(ProbeData));
  BSP_24CXX_Read(EEPROM_BASE_ADDR, (uint8_t *)&data, sizeof(ProbeData));
  /* SPI初始化 */
  SPI_Config();
  /* PWM初始化 */
  PWM_Config();
  /* 注册读取按键电平函数,按键初始化 */
  TEST_KEY_GPIO_Init();
  btn_attach_read_io_func(btn_read_level);
  btn_attach(KEY_BUTTON_1, 0);
  btn_attach(KEY_BUTTON_2, 0);
  btn_attach(KEY_BUTTON_3, 0);
  btn_attach(KEY_BUTTON_4, 0);
  /* 蜂鸣器初始化 */
  BEEP_GPIO_Init();
  /* 随机数生成器 TRNG 初始化配置 */
  TrngConfig();
  /* 开启通用定时器TMR0定时(3ms)：给按键检测库提供心跳 */
  TMR0_Config();
  TMR0_Start(TMR0_UNIT, TMR0_CH);
  /* 异步时钟源，写入TMR0寄存器需要等待三个异步时钟。 */
  DDL_DelayMS(1U);
  /* 初始化CAN总线 */
  CAN_Drv_Init();
  /* 注册键值线程锁回调函数及键值初始化 */
  key_value_mutex_init();
  Key_Value_Init();
}

/**
 * @brief  程序的主入口函数
 * @details 此函数执行以下操作：
 *          1. 解锁所有外设寄存器
 *          2. 初始化板级外设
 *          3. 进入无限循环
 *
 * @note   正常情况下，程序不会执行到无限循环
 *
 * @return int32_t 返回值在此函数中未使用
 */
int32_t main(void)
{
  /* 对指定的外设寄存器解锁（解锁后才能写入） */
  LL_PERIPH_WE(LL_PERIPH_ALL);

  /* 定义一个创建信息返回值，默认为pdPASS */
  BaseType_t xReturn = pdPASS;

  /* 初始化板级外设 */
  BSP_Init();

  /* 创建任务创建任务 */
  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,          /* 任务入口函数 */
                        (const char *)"AppTaskCreate",          /* 任务名字 */
                        (uint16_t)256,                          /* 任务栈大小 */
                        (void *)NULL,                           /* 任务入口函数参数 */
                        (UBaseType_t)3,                         /* 任务的优先级 */
                        (TaskHandle_t *)&AppTaskCreate_Handle); /* 任务控制块指针 */

  /* 启动任务调度 */
  if (pdPASS == xReturn)
  {
    vTaskStartScheduler(); /* 启动任务，开启调度 */
  }
  else
    return -1;
  /* 正常不会执行到这里 */
  for (;;)
  {
  }
}

/**
 * @brief  Astraui任务
 * @param pvParameters 任务参数
 * @note   C++ ui库
 * @return none
 */
static void AstraTask(void *pvParameters)
{
  astraCoreInit();
  astraCoreStart();
}

/**
 * @brief  创建任务
 * @param pvParameters 任务参数
 * @note   创建其他任务
 * @return none
 */
static void AppTaskCreate(void *pvParameters)
{
  BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为pdPASS */
  taskENTER_CRITICAL();        // 进入临界区

  //-------------------------创建任务----------------------------------
  /* 创建任务创建任务 */
  xReturn = xTaskCreate((TaskFunction_t)AstraTask,          /* 任务入口函数 */
                        (const char *)"Astra_ui_Task",      /* 任务名字 */
                        (uint16_t)1024,                     /* 任务栈大小 */
                        (void *)NULL,                       /* 任务入口函数参数 */
                        (UBaseType_t)3,                     /* 任务的优先级 */
                        (TaskHandle_t *)&AstraTask_Handle); /* 任务控制块指针 */
  if (xReturn != pdPASS)
  {
  }

  /* 创建USART任务 */
  xReturn = xTaskCreate((TaskFunction_t)UsartTask,          /* 任务入口函数 */
                        (const char *)"UsartTask",          /* 任务名字 */
                        (uint16_t)configMINIMAL_STACK_SIZE, /* 任务栈大小 */
                        (void *)NULL,                       /* 任务入口函数参数 */
                        (UBaseType_t)3,                     /* 任务的优先级 */
                        (TaskHandle_t *)&UsartTask_Handle); /* 任务控制块指针 */
  if (xReturn != pdPASS)
  {
  }

  /* 创建CAN发送任务 */
  xReturn = xTaskCreate((TaskFunction_t)CanTxTask,          /* 任务入口函数 */
                        (const char *)"CanTxTask",          /* 任务名字 */
                        (uint16_t)configMINIMAL_STACK_SIZE, /* 任务栈大小 */
                        (void *)NULL,                       /* 任务入口函数参数 */
                        (UBaseType_t)3,                     /* 任务的优先级 */
                        (TaskHandle_t *)&CanTxTask_Handle); /* 任务控制块指针 */
  if (xReturn != pdPASS)
  {
  }

  /* 创建CAN接收任务 */
  xReturn = xTaskCreate((TaskFunction_t)CANRxTask,          /* 任务入口函数 */
                        (const char *)"CanRxTask",          /* 任务名字 */
                        (uint16_t)configMINIMAL_STACK_SIZE, /* 任务栈大小 */
                        (void *)NULL,                       /* 任务入口函数参数 */
                        (UBaseType_t)3,                     /* 任务的优先级 */
                        (TaskHandle_t *)&CanRxTask_Handle); /* 任务控制块指针 */
  if (xReturn != pdPASS)
  {
  }
  //----------------------------创建队列------------------------------------
  xQueue_CanTx = xQueueCreate(10, sizeof(stc_can_tx_frame_t));
  xQueue_ProbeInfoTransfer = xQueueCreate(10, sizeof(WarningUpdateMessage));
  //-------------------------创建事件标志组----------------------------------
  xInit_EventGroup = xEventGroupCreate();
  xProbeDataSendEventGroup = xEventGroupCreate();
  //-------------------------创建软件定时器----------------------------------
  xDoseRateTimer = xTimerCreate("DoseRateTimer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, vDoseRateTimerCallback);
  xBeepTimer = xTimerCreate("BeepTimer", pdMS_TO_TICKS(500), pdTRUE, (void *)0, vBeepTimerCallback);
  xProbe1AlarmTimer = xTimerCreate("Probe1AlarmTimer", pdMS_TO_TICKS(5000), pdTRUE, (void *)0, vProbe1AlarmTimerCallback);
  xProbe2AlarmTimer = xTimerCreate("Probe2AlarmTimer", pdMS_TO_TICKS(5000), pdTRUE, (void *)0, vProbe2AlarmTimerCallback);
  xProbe3AlarmTimer = xTimerCreate("Probe3AlarmTimer", pdMS_TO_TICKS(5000), pdTRUE, (void *)0, vProbe3AlarmTimerCallback);
  xProbe4AlarmTimer = xTimerCreate("Probe4AlarmTimer", pdMS_TO_TICKS(5000), pdTRUE, (void *)0, vProbe4AlarmTimerCallback);
  //-------------------------结束创建----------------------------------

  vTaskDelete(AppTaskCreate_Handle); // 删除AppTaskCreate任务
  taskEXIT_CRITICAL();               // 退出临界区
}

/**
 * @brief  USART任务
 * @param pvParameters 任务参数
 * @note   接收USART数据并发送
 * @return none
 */
static void UsartTask(void *pvParameters)
{
  uint32_t u32ReceiveLen = 0;
  uint8_t *pUsartBuffer = (uint8_t *)mymalloc(APP_FRAME_LEN_MAX);

  if (pUsartBuffer == NULL)
  {
    // 内存分配失败，处理错误
    for (;;)
    {
      // 可以在这里添加错误处理代码，如闪烁LED等
    }
  }

  for (;;)
  {
    if (xTaskNotifyWait(0x00, ULONG_MAX, &u32ReceiveLen, portMAX_DELAY) == pdTRUE)
    {
      memcpy(pUsartBuffer, m_au8RxBuf, u32ReceiveLen);
      key_value_msg("UART_sendData", pUsartBuffer, u32ReceiveLen);
      memset(m_au8RxBuf, 0, sizeof(m_au8RxBuf));
    }
  }

  // 注意：这里的myfree不会被执行到，因为任务是一个无限循环
  // 如果需要释放内存，应该在任务删除的回调函数中进行
  // myfree(pUsartBuffer);
}

/**
 * @brief  CAN发送任务
 * @param pvParameters 任务参数
 * @note   发送CAN总线
 * @return none
 */
void CanTxTask(void *pvParameters)
{
  stc_can_tx_frame_t tx_frame;
  bool firstRun = true;

  for (;;)
  {
    while (xQueueReceive(xQueue_CanTx, &tx_frame, portMAX_DELAY) == pdTRUE) // 从CAN发送队列中接收数据
    {
      // 第一次不执行等待CAN总线
      if (!firstRun)
      {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // 等待CAN总线发送完成
      }
      else
      {
        firstRun = false;
      }

      CAN_IntCmd(CAN_UNIT, CAN_INT_PTB_TX, ENABLE);               // 使能发送中断
      (void)CAN_FillTxFrame(CAN_UNIT, CAN_TX_BUF_PTB, &tx_frame); // 将发送帧的数据填充到发送缓冲区
      CAN_StartTx(CAN_UNIT, CAN_TX_REQ_PTB);                      // 启动发送
    }
  }
}

/**
 * @brief  CAN接收任务
 * @param pvParameters 任务参数
 * @note   接收CAN总线
 * @return none
 */
void CANRxTask(void *pvParameters)
{
  uint32_t u32NotificationValue;

  for (;;)
  {
    // 等待CAN总线接收完成
    u32NotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // 如果中断在这个处理期间再次发生，ulNotificationValue将会大于1，可以来处理所有积累的消息
    while (u32NotificationValue > 0)
    {
      CAN_ProcessReceivedData();
      u32NotificationValue--;
    }
  }
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/