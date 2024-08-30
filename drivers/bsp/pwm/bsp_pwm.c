#include "variable.h"

/******************************变量定义*******************************/

/******************************函数定义*******************************/

/**
 * @name   TmrAConfig
 * @brief  TimerA PWM相关初始化
 * @param  None
 * @retval None
 */
static void TmrAConfig(void)
{
	stc_tmra_init_t stcTmraInit;
	stc_tmra_pwm_init_t stcPwmInit;

	/*******************************************************************************
	 *OLED亮度调节PWM
	 ******************************************************************************/

	/* 1.使能TimerA时钟 */
	FCG_Fcg2PeriphClockCmd(TMRA_CLK, ENABLE);

	/* 2. 为stcTmraInit设置默认的初始化值 */
	(void)TMRA_StructInit(&stcTmraInit);

	/* 3. 根据应用程序修改初始化值 */
	stcTmraInit.sw_count.u8CountMode = TMRA_MD;				 // 锯齿波（方波）
	stcTmraInit.sw_count.u8CountDir = TMRA_DIR;				 // 向上计数
	stcTmraInit.u32PeriodValue = OLED_BRIGHTNESS_PERIOD_VAL; // 周期值
	(void)TMRA_Init(TMRA_UNIT, &stcTmraInit);

	/* 4. 设置PWM 50% 占空比*/
	(void)TMRA_PWM_StructInit(&stcPwmInit);
	stcPwmInit.u32CompareValue = OLED_BRIGHTNESS_CMP_VAL;
	GPIO_SetFunc(OLED_BRIGHTNESS_GPIO_PORT, OLED_BRIGHTNESS_GPIO_PIN, OLED_BRIGHTNESS_GPIO_FUNC);
	(void)TMRA_PWM_Init(TMRA_UNIT, OLED_BRIGHTNESS_TMRA_CH, &stcPwmInit);

	/* 5. 启动PWM输出 */
	TMRA_PWM_OutputCmd(TMRA_UNIT, OLED_BRIGHTNESS_TMRA_CH, ENABLE);
}

/**
 * @brief  PWM 配置
 * @note   配置OLED亮度调节PWM
 * @param  None
 * @retval None
 */
void PWM_Config(void)
{
	TmrAConfig();
	TMRA_Start(TMRA_UNIT);
}

/**
 * @brief  设置PWM周期值
 * @param  periodValue: 周期值
 * @retval None
 */
void setPWMPeriodValue(uint32_t periodValue)
{
    TMRA_SetPeriodValue(TMRA_UNIT, periodValue);
}

/**
 * @brief  设置PWM占空比
 * @param  dutyCycle: 占空比
 * @retval None
 */
void setPWM_DutyCycle(CM_TMRA_TypeDef *TMRAx, uint32_t channel, float dutyCycle)
{
    TMRA_Update_PWM_DutyCycle(TMRAx, channel, dutyCycle);
}