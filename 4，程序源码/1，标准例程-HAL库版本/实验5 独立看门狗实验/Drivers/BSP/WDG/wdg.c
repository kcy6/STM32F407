/**
 ****************************************************************************************************
 * @file        wdg.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       看门狗驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 M144Z-M4最小系统板STM32F407版
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#include "./BSP/WDG/wdg.h"
#include "./BSP/LED/led.h"

static IWDG_HandleTypeDef g_iwdg_handler = {0}; /* 独立看门狗句柄 */
static WWDG_HandleTypeDef g_wwdg_handler = {0}; /* 窗口看门狗句柄 */

/**
 * @brief   初始化独立看门狗
 * @param   prescaler: 预分频器系数
 * @arg     IWDG_PRESCALER_4  : 4分频
 * @arg     IWDG_PRESCALER_8  : 8分频
 * @arg     IWDG_PRESCALER_16 : 16分频
 * @arg     IWDG_PRESCALER_32 : 32分频
 * @arg     IWDG_PRESCALER_64 : 64分频
 * @arg     IWDG_PRESCALER_128: 128分频
 * @arg     IWDG_PRESCALER_256: 256分频
 * @param   reload: 自动重装载值（0~0xFFF）
 * @retval  无
 */
void iwdg_init(uint32_t prescaler, uint32_t reload)
{
    g_iwdg_handler.Instance = IWDG;
    g_iwdg_handler.Init.Prescaler = prescaler;
    g_iwdg_handler.Init.Reload = reload;
    HAL_IWDG_Init(&g_iwdg_handler);
}

/**
 * @brief   喂狗独立看门狗
 * @param   无
 * @retval  无
 */
void iwdg_feed(void)
{
    HAL_IWDG_Refresh(&g_iwdg_handler);
}

/**
 * @brief   初始化窗口看门狗
 * @param   prescaler: 预分频器系数
 * @arg     WWDG_PRESCALER_1: 1分频
 * @arg     WWDG_PRESCALER_2: 2分频
 * @arg     WWDG_PRESCALER_4: 4分频
 * @arg     WWDG_PRESCALER_8: 8分频
 * @param   window: 窗口值
 * @param   counter: 计数器值
 * @retval  无
 */
void wwdg_init(uint32_t prescaler, uint32_t window, uint32_t counter)
{
    g_wwdg_handler.Instance = WWDG;
    g_wwdg_handler.Init.Prescaler = prescaler;
    g_wwdg_handler.Init.Window = window;
    g_wwdg_handler.Init.Counter = counter;
    g_wwdg_handler.Init.EWIMode = WWDG_EWI_ENABLE;
    HAL_WWDG_Init(&g_wwdg_handler);
}

/**
 * @brief   HAL库窗口看门狗初始化MSP函数
 * @param   无
 * @retval  无
 */
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
    /* 使能窗口看门狗时钟 */
    __HAL_RCC_WWDG_CLK_ENABLE();
    
    /* 配置中断优先级并使能中断 */
    HAL_NVIC_SetPriority(WWDG_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(WWDG_IRQn);
}

/**
 * @brief   HAL库窗口看门狗提前唤醒中断回调函数
 * @param   hwwdg: 窗口看门狗句柄
 * @retval  无
 */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    /* 喂狗窗口看门狗 */
    HAL_WWDG_Refresh(&g_wwdg_handler);
    LED1_TOGGLE();
}

/**
 * @brief   WWDG中断服务函数
 * @param   无
 * @retval  无
 */
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&g_wwdg_handler);
}
