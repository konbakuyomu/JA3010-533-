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
static void BTNTask(void *pvParameters);
void btn_tic_task(TimerHandle_t xTimer);

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
  /* SPI初始化 */
  SPI_Config();
  /*  (测试)LED初始化 */
  BSP_LED_Init();
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
                        (uint16_t)512,                      /* 任务栈大小 */
                        (void *)NULL,                       /* 任务入口函数参数 */
                        (UBaseType_t)3,                     /* 任务的优先级 */
                        (TaskHandle_t *)&AstraTask_Handle); /* 任务控制块指针 */
  if (xReturn != pdPASS)
  {
  }

  /* 创建LED任务 */
  xReturn = xTaskCreate((TaskFunction_t)LEDTask,          /* 任务入口函数 */
                        (const char *)"LED_Task",         /* 任务名字 */
                        (uint16_t)128,                    /* 任务栈大小 */
                        (void *)NULL,                     /* 任务入口函数参数 */
                        (UBaseType_t)3,                   /* 任务的优先级 */
                        (TaskHandle_t *)&LEDTask_Handle); /* 任务控制块指针 */
  if (xReturn != pdPASS)
  {
    
  }
  //-------------------------创建软件定时器----------------------------------
  // 参数说明：
  // - 定时器周期（以系统时钟节拍为单位）
  // - pdTRUE 表示周期性定时器，pdFALSE 表示一次性定时器
  // - 定时器ID（用于识别定时器，这里设为0）
  // - 回调函数
  // btn_tic_taskHandle = xTimerCreate((const char *)"btn_tic_task",           /* 定时器名字 */
  //                                   pdMS_TO_TICKS(3),                       /* 定时器周期 */
  //                                   (UBaseType_t)pdTRUE,                    /* 是否自动重载 */
  //                                   (void *)0,                              /* 定时器ID */
  //                                   (TimerCallbackFunction_t)btn_tic_task); /* 定时器回调函数 */
  // if (btn_tic_taskHandle != NULL)
  // {
  //   // 启动定时器
  //   // 0 表示不阻塞等待
  //   xTimerStart(btn_tic_taskHandle, 0);
  // }
  //-------------------------创建事件标志组----------------------------------
  xInit_EventGroup = xEventGroupCreate();
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
 * @brief  按键任务
 * @param pvParameters 任务参数
 * @note   按键检测
 * @return none
 */
static void BTNTask(void *pvParameters)
{
  btn_event_t ret;
  uint8_t btn_io_num;

  for (;;)
  {
    if (btn_available() > 0)
    {
      btn_read_event(&btn_io_num, &ret);

      // 根据返回的事件类型执行相应的操作
      switch (ret)
      {
      case btn_not_press:
      {

        break;
      }

      case btn_up:
      {

        break;
      }

      case btn_long_press:
      {

        break;
      }

      case btn_long_press_trig:
      {

        break;
      }

      case btn_down:
      {
        if (btn_io_num == KEY_BUTTON_1)
        {
          BSP_LED_Toggle(LED_RED);
        }
        else if (btn_io_num == KEY_BUTTON_2)
        {
          BSP_LED_Toggle(LED_BLUE);
        }
        break;
      }

      case btn_click:
      {
        break;
      }

      case btn_double_click:
      {

        break;
      }

      case btn_event_all:
      {
        // main分支修改
        break;
      }
      }
    }
    vTaskDelay(15 / portTICK_PERIOD_MS);
  }
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/