/**
 ****************************************************************************************************
 * @file        btim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       基本定时器驱动代码
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

#include "./BSP/TIMER/btim.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"

extern uint16_t frame;
extern __IO uint8_t frameup;

/* 基本定时器句柄 */
TIM_HandleTypeDef g_tim6_handle = {0};
TIM_HandleTypeDef g_tim7_handle = {0};

/**
 * @brief   初始化基本定时器6
 * @param   arr: 自动重装载值
 * @param   psc: 预分频系数
 * @retval  无
 */
void btim_tim6_int_init(uint16_t arr, uint16_t psc)
{
    g_tim6_handle.Instance = BTIM_TIM6_INT;
    g_tim6_handle.Init.Prescaler = psc;
    g_tim6_handle.Init.Period = arr;
    HAL_TIM_Base_Init(&g_tim6_handle);
    HAL_TIM_Base_Start_IT(&g_tim6_handle);
}

/**
 * @brief   初始化基本定时器7
 * @param   arr: 自动重装载值
 * @param   psc: 预分频系数
 * @retval  无
 */
void btim_tim7_int_init(uint16_t arr, uint16_t psc)
{
    g_tim7_handle.Instance = BTIM_TIM7_INT;
    g_tim7_handle.Init.Prescaler = psc;
    g_tim7_handle.Init.Period = arr;
    HAL_TIM_Base_Init(&g_tim7_handle);
    HAL_TIM_Base_Start_IT(&g_tim7_handle);
}

/**
 * @brief   HAL库TIM初始化MSP函数
 * @param   无
 * @retval  无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIM6_INT)
    {
        /* 使能基本定时器时钟 */
        BTIM_TIM6_INT_CLK_ENABLE();
        
        /* 配置中断优先级并使能中断 */
        HAL_NVIC_SetPriority(BTIM_TIM6_INT_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(BTIM_TIM6_INT_IRQn);
    }
    else if (htim->Instance == BTIM_TIM7_INT)
    {
        /* 使能基本定时器时钟 */
        BTIM_TIM7_INT_CLK_ENABLE();
        
        /* 配置中断优先级并使能中断 */
        HAL_NVIC_SetPriority(BTIM_TIM7_INT_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(BTIM_TIM7_INT_IRQn);
    }
}

/**
 * @brief   基本定时器6中断服务函数
 * @param   无
 * @retval  无
 */
void BTIM_TIM6_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim6_handle);
}

/**
 * @brief   基本定时器7中断服务函数
 * @param   无
 * @retval  无
 */
void BTIM_TIM7_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim7_handle);
}

/**
 * @brief   HAL库基本定时器超时中断回调函数
 * @param   无
 * @retval  无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIM6_INT)
    {
        printf("Video frame: %d\r\n", frame);
        frame = 0;
        LED0_TOGGLE();
    }
    if (htim->Instance == BTIM_TIM7_INT)
    {
        frameup = 1;
        LED1_TOGGLE();
    }
}
