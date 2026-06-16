/**
 ****************************************************************************************************
 * @file        dac.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       DAC驱动代码
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

#ifndef __DAC_H
#define __DAC_H

#include "./SYSTEM/sys/sys.h"

/* DAC定义 */
#define DAC_DACX                        DAC
#define DAC_DACX_CLK_ENABLE()           do { __HAL_RCC_DAC_CLK_ENABLE(); } while (0)
#define DAC_DACX_CHY                    DAC_CHANNEL_1
#define DAC_DACX_CHY_GPIO_PORT          GPIOA
#define DAC_DACX_CHY_GPIO_PIN           GPIO_PIN_4
#define DAC_DACX_CHY_GPIO_CLK_ENABLE()  do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)

/* 函数声明 */
void dac_init(void);                                                                                /* 初始化DAC */
void dac_set_voltage(uint16_t voltage);                                                             /* 设置DAC输出电压 */
void dac_triangular_wave(uint16_t max_value, uint16_t interval, uint16_t samples, uint16_t number); /* 设置DAC输出三角波 */

#endif
