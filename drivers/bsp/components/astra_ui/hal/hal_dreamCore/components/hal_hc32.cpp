//
// Created by Fir on 2024/2/12.
//
#include "../hal_dreamCore.h"
#include "variable.h"

void HALDreamCore::_RTOS_Init()
{ 
  // 第一次启动的时候需要把这个允许弹窗的标志置位
  xEventGroupSetBits(xProbeDataSendEventGroup, PROBE1_POPUP_FLAG | PROBE2_POPUP_FLAG | PROBE3_POPUP_FLAG | PROBE4_POPUP_FLAG);
}

void HALDreamCore::_delay(unsigned long _mill)
{
  vTaskDelay(_mill / portTICK_PERIOD_MS);
}

unsigned long HALDreamCore::_millis()
{
  return SysTick_GetTick();
}

unsigned long HALDreamCore::_getTick()
{
  return SysTick_GetTick();
}

unsigned long HALDreamCore::_getRandomSeed()
{
  uint32_t randomNumber;

  TRNG_Start();
  (void)TRNG_GenerateRandom(&randomNumber, 1U);

  return (unsigned long)randomNumber;
}
