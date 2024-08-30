#ifndef __BSP_PWM_H
#define __BSP_PWM_H

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

#include "hc32_ll.h"

/******************************电机宏定义*******************************/
#define TMRA_MD                         (TMRA_MD_SAWTOOTH)   // PWM锯齿波输出
#define TMRA_DIR                        (TMRA_DIR_UP)        // 向上计数
#define TMRA_UNIT                       (CM_TMRA_3)          // TimerA单元3
#define TMRA_CLK                        (FCG2_PERIPH_TMRA_3) // TimerA时钟

// OLED亮度调节PWM (PA6)
#define OLED_BRIGHTNESS_TMRA_CH         (TMRA_CH1)
#define OLED_BRIGHTNESS_GPIO_PORT       (GPIO_PORT_A)
#define OLED_BRIGHTNESS_GPIO_PIN        (GPIO_PIN_06)
#define OLED_BRIGHTNESS_GPIO_FUNC       (GPIO_FUNC_5)


/******************************频率及占空比定义*******************************/
#define HCLK1_FREQ                      (100000000U)                                                // 100 MHz

// OLED亮度调节PWM (PA06)
#define OLED_BRIGHTNESS_FREQ            (900U)                                                      // 800 Hz
#define OLED_BRIGHTNESS_DUTY            (0.9)                                                       // 50%占空比
#define OLED_BRIGHTNESS_PERIOD_VAL      ((HCLK1_FREQ / OLED_BRIGHTNESS_FREQ) - 1U)                  // OLED亮度调节PWM的周期值
#define OLED_BRIGHTNESS_CMP_VAL         ((OLED_BRIGHTNESS_PERIOD_VAL + 1U) * OLED_BRIGHTNESS_DUTY)  // OLED亮度调节PWM的比较值
/******************************函数定义*******************************/

void PWM_Config(void);
void setPWMPeriodValue(uint32_t periodValue);
void setPWM_DutyCycle(CM_TMRA_TypeDef *TMRAx, uint32_t channel, float dutyCycle);

#ifdef __cplusplus
}
#endif

#endif

