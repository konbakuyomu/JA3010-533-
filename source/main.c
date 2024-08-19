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
static void LEDTask(void *pvParameters);

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
  /* EEPROM初始化 */
  BSP_24CXX_Init();
  BSP_24CXX_Read(EEPROM_BASE_ADDR, (uint8_t *)&data, sizeof(ProbeData));
  /* SPI初始化 */
  SPI_Config();
  /* 注册读取按键电平函数,按键初始化 */
  TEST_KEY_GPIO_Init();
  btn_attach_read_io_func(btn_read_level);
  btn_attach(KEY_BUTTON_1, 0);
  btn_attach(KEY_BUTTON_2, 0);
  /* 随机数生成器 TRNG 初始化配置 */
  TrngConfig();
  /* 开启通用定时器TMR0定时(3ms)：给按键检测库提供心跳 */
  TMR0_Config();
  TMR0_Start(TMR0_UNIT, TMR0_CH);
  /* 异步时钟源，写入TMR0寄存器需要等待三个异步时钟。 */
  DDL_DelayMS(1U);
  /* 初始化CAN总线 */
  CAN_Drv_Init();
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

  /* 创建LED任务 */
  xReturn = xTaskCreate((TaskFunction_t)LEDTask,            /* 任务入口函数 */
                        (const char *)"LED_Task",           /* 任务名字 */
                        (uint16_t)configMINIMAL_STACK_SIZE, /* 任务栈大小 */
                        (void *)NULL,                       /* 任务入口函数参数 */
                        (UBaseType_t)3,                     /* 任务的优先级 */
                        (TaskHandle_t *)&LEDTask_Handle);   /* 任务控制块指针 */
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
  xQueue_WarningUpdate = xQueueCreate(10, sizeof(WarningUpdateMessage));
  //-------------------------创建事件标志组----------------------------------
  xInit_EventGroup = xEventGroupCreate();
  //-------------------------创建软件定时器----------------------------------
  xDoseRateTimer = xTimerCreate("DoseRateTimer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, vDoseRateTimerCallback);
  //-------------------------结束创建----------------------------------

  vTaskDelete(AppTaskCreate_Handle); // 删除AppTaskCreate任务
  taskEXIT_CRITICAL();               // 退出临界区
}

/**
 * @brief  LED任务
 * @param pvParameters 任务参数
 * @note   LED闪烁
 * @return none
 */
static void LEDTask(void *pvParameters)
{
  for (;;)
  {
    BSP_LED_Toggle(LED_YELLOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
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