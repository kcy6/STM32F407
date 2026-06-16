/**
 ****************************************************************************************************
 * @file        gtim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       通用定时器驱动代码
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

#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

/* 通用定时器定义 */
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do { __HAL_RCC_TIM3_CLK_ENABLE(); } while (0)

#define GTIM_TIMX_PWM                       TIM3
#define GTIM_TIMX_PWM_CLK_ENABLE()          do { __HAL_RCC_TIM3_CLK_ENABLE(); } while (0)
#define GTIM_TIMX_PWM_CHY                   TIM_CHANNEL_1
#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOB
#define GTIM_TIMX_PWM_CHY_GPIO_PIN          GPIO_PIN_4
#define GTIM_TIMX_PWM_CHY_GPIO_AF           GPIO_AF2_TIM3
#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)

#define GTIM_TIMX_CAP                       TIM5
#define GTIM_TIMX_CAP_IRQn                  TIM5_IRQn
#define GTIM_TIMX_CAP_IRQHandler            TIM5_IRQHandler
#define GTIM_TIMX_CAP_CLK_ENABLE()          do { __HAL_RCC_TIM5_CLK_ENABLE(); } while (0)
#define GTIM_TIMX_CAP_CHY                   TIM_CHANNEL_1
#define GTIM_TIMX_CAP_CHY_GPIO_PORT         GPIOA
#define GTIM_TIMX_CAP_CHY_GPIO_PIN          GPIO_PIN_0
#define GTIM_TIMX_CAP_CHY_GPIO_AF           GPIO_AF2_TIM5
#define GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)

#define GTIM_TIMX_CNT                       TIM2
#define GTIM_TIMX_CNT_IRQn                  TIM2_IRQn
#define GTIM_TIMX_CNT_IRQHandler            TIM2_IRQHandler
#define GTIM_TIMX_CNT_CLK_ENABLE()          do { __HAL_RCC_TIM2_CLK_ENABLE(); } while (0)
#define GTIM_TIMX_CNT_CHY                   TIM_CHANNEL_1
#define GTIM_TIMX_CNT_CHY_GPIO_PORT         GPIOA
#define GTIM_TIMX_CNT_CHY_GPIO_PIN          GPIO_PIN_0
#define GTIM_TIMX_CNT_CHY_GPIO_AF           GPIO_AF1_TIM2
#define GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)

/* 函数声明 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc);        /* 初始化通用定时器中断 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);    /* 初始化通用定时器PWM */
void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc);    /* 初始化通用定时器输入捕获 */
void gtim_timx_cnt_chy_init(uint16_t psc);                  /* 初始化通用定时器脉冲计数 */
uint32_t gtim_timx_cnt_chy_get_count(void);                 /* 获取通用定时器脉冲计数值 */
void gtim_timx_cnt_chy_restart(void);                       /* 重启通用定时器脉冲计数 */

#endif
