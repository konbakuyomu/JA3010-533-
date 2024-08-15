#include "variable.h"

/******************************变量定义*******************************/

/******************************函数定义*******************************/

/**
 * @brief  CAN初始化
 * @param  无
 * @retval 无
 */
void CAN_Drv_Init(void)
{
    CanPinConfig();
    CanInitConfig();
    CanIrqConfig();
}

/**
 * @brief  配置CAN引脚
 * @param  无
 * @retval 无
 */
void CanPinConfig(void)
{
    GPIO_SetFunc(CAN_TX_PORT, CAN_TX_PIN, CAN_TX_PIN_FUNC); // TX引脚复用为CAN
    GPIO_SetFunc(CAN_RX_PORT, CAN_RX_PIN, CAN_RX_PIN_FUNC); // RX引脚复用为CAN
}

/**
 * @brief  CAN初始配置
 * @param  无
 * @retval 无
 */
void CanInitConfig(void)
{
    stc_can_init_t stcCanInit;
    /**
     *CAN_FILTERx_ID：过滤器x的ID值，x的取值范围是1~28。
     *CAN_FILTERx_ID_MASK：过滤器x的ID掩码，用于指定哪些位需要匹配，哪些位不需要匹配。
     *CAN_FILTERx_ID_TYPE：过滤器x的ID类型，指定是标准帧还是扩展帧。
     **/
    stc_can_filter_config_t astcFilter[CAN_FILTER_NUM] = {
        {CAN_FILTER1_ID, CAN_FILTER1_ID_MASK, CAN_FILTER1_ID_TYPE},
        {CAN_FILTER2_ID, CAN_FILTER2_ID_MASK, CAN_FILTER2_ID_TYPE},
        {CAN_FILTER3_ID, CAN_FILTER3_ID_MASK, CAN_FILTER3_ID_TYPE},
        {CAN_FILTER4_ID, CAN_FILTER4_ID_MASK, CAN_FILTER4_ID_TYPE}};

    /* CAN 波特率设定，这里时钟来源是外部晶振 8MHz ，最后波特率 = 8M/(SEG1+SEG2)*PRESC */
    /* PRESC:2  SEG1:6  SEG2:2 SJW:2  --->  500K */
    /* PRESC:2  SEG1:12  SEG2:4 SJW:3  --->  250K */
    /* PRESC:2  SEG1:3  SEG2:1 SJW:1  --->  1M */
    /* 当前波特率: 250K */
    (void)CAN_StructInit(&stcCanInit);
    stcCanInit.stcBitCfg.u32Prescaler = 2U;        // 预分频
    stcCanInit.stcBitCfg.u32TimeSeg1 = 12U;        // 时间段1
    stcCanInit.stcBitCfg.u32TimeSeg2 = 4U;         // 时间段2
    stcCanInit.stcBitCfg.u32SJW = 3U;              // 同步跳跃宽
    stcCanInit.pstcFilter = astcFilter;            // 这里先自己配置任意数量的过滤器
    stcCanInit.u16FilterSelect = CAN_FILTER_SEL;   // 然后在这里具体选择要哪些过滤器
    stcCanInit.u8WorkMode = CAN_WORK_MD_NORMAL;    // 正常工作模式

    /* 初始化使能CAN时钟 */
    FCG_Fcg1PeriphClockCmd(CAN_PERIPH_CLK, ENABLE);
    /* 初始化CAN */
    (void)CAN_Init(CAN_UNIT, &stcCanInit);
    /* 禁用所有的CAN中断 */
    CAN_IntCmd(CAN_UNIT, CAN_INT_ALL, DISABLE);
    /* 启用所需要的CAN中断 */
    CAN_IntCmd(CAN_UNIT, CAN_INT_SEL, ENABLE);
}

/**
 * @brief  配置CAN中断
 * @param  无
 * @retval 无
 */
void CanIrqConfig(void)
{
    stc_irq_signin_config_t stcIrq;

    stcIrq.enIntSrc = CAN_INT_SRC;                 // 设置中断源
    stcIrq.enIRQn = CAN_INT_IRQn;                  // 设置中断号
    stcIrq.pfnCallback = &CAN_IrqCallback;         // 设置回调函数
    (void)INTC_IrqSignIn(&stcIrq);                 // 将以上中断配置信息注册到中断控制器
    NVIC_ClearPendingIRQ(stcIrq.enIRQn);           // 清除中断标志，以避免在设置中断配置之前已经有的中断请求影响到新的中断配置
    NVIC_SetPriority(stcIrq.enIRQn, CAN_INT_PRIO); // 设置中断优先级，此处优先级为 15 ，一共有15个优先级
    NVIC_EnableIRQ(stcIrq.enIRQn);                 // 使能中断
}
