//
// Created by Fir on 2024/2/12.
//
#include "../hal_dreamCore.h"
#include "variable.h"

void HALDreamCore::_hc32_hal_init()
{ // NOLINT
  // BSP_Init();
  // LCD_Init();
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
