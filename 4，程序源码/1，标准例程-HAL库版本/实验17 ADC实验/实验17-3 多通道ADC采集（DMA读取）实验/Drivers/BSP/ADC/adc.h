/**
 ****************************************************************************************************
 * @file        adc.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       ADC驱动代码
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

#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"

/* ADC定义 */
#define ADC_ADCX                                ADC2
#define ADC_ADCX_CLK_ENABLE()                   do { __HAL_RCC_ADC2_CLK_ENABLE(); } while (0)
#define ADC_ADCX_CHY                            ADC_CHANNEL_1
#define ADC_ADCX_CHY_GPIO_PORT                  GPIOA
#define ADC_ADCX_CHY_GPIO_PIN                   GPIO_PIN_1
#define ADC_ADCX_CHY_GPIO_CLK_ENABLE()          do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)

#define ADC_DMA_ADCX                            ADC3
#define ADC_DMA_ADCX_CLK_ENABLE();              do { __HAL_RCC_ADC3_CLK_ENABLE(); } while (0)
#define ADC_DMA_ADCX_CHY                        ADC_CHANNEL_1
#define ADC_DMA_ADCX_CHY_GPIO_PORT              GPIOA
#define ADC_DMA_ADCX_CHY_GPIO_PIN               GPIO_PIN_1
#define ADC_DMA_ADCX_CHY_GPIO_CLK_ENABLE()      do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)
#define ADC_DMA_ADCX_DMASX                      DMA2_Stream1
#define ADC_DMA_ADCX_DMA_CLK_ENABLE()           do { __HAL_RCC_DMA2_CLK_ENABLE(); } while (0)
#define ADC_DMA_ADCX_DMASX_CHY                  DMA_CHANNEL_2
#define ADC_DMA_ADCX_DMASX_IRQn                 DMA2_Stream1_IRQn
#define ADC_DMA_ADCX_DMASX_IRQHandler           DMA2_Stream1_IRQHandler

#define ADC_NCH_DMA_ADCX                        ADC1
#define ADC_NCH_DMA_ADCX_CLK_ENABLE()           do { __HAL_RCC_ADC1_CLK_ENABLE(); } while (0)
#define ADC_NCH_DMA_ADCX_CH_NUM                 2
#define ADC_NCH_DMA_ADCX_CHA                    ADC_CHANNEL_5
#define ADC_NCH_DMA_ADCX_CHA_GPIO_PORT          GPIOA
#define ADC_NCH_DMA_ADCX_CHA_GPIO_PIN           GPIO_PIN_5
#define ADC_NCH_DMA_ADCX_CHA_GPIO_CLK_ENABLE()  do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)
#define ADC_NCH_DMA_ADCX_CHB                    ADC_CHANNEL_6
#define ADC_NCH_DMA_ADCX_CHB_GPIO_PORT          GPIOA
#define ADC_NCH_DMA_ADCX_CHB_GPIO_PIN           GPIO_PIN_6
#define ADC_NCH_DMA_ADCX_CHB_GPIO_CLK_ENABLE()  do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)
#define ADC_NCH_DMA_ADCX_DMASX                  DMA2_Stream0
#define ADC_NCH_DMA_ADCX_DMA_CLK_ENABLE()       do { __HAL_RCC_DMA2_CLK_ENABLE(); } while (0)
#define ADC_NCH_DMA_ADCX_DMASX_CHY              DMA_CHANNEL_0
#define ADC_NCH_DMA_ADCX_DMASX_IRQn             DMA2_Stream0_IRQn
#define ADC_NCH_DMA_ADCX_DMASX_IRQHandler       DMA2_Stream0_IRQHandler

/* 函数声明 */
void adc_init(void);                                                                                            /* 初始化ADC */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t channel, uint32_t rank, uint32_t sampling_time);   /* 设置ADC通道 */
uint16_t adc_get_result(uint32_t channel);                                                                      /* 获取ADC结果 */
uint16_t adc_get_result_average(uint32_t channel, uint8_t times);                                               /* 均值滤波获取ADC结果 */
void adc_dma_init(uint32_t memory_base);                                                                        /* 初始化ADC DMA读取 */
void adc_dma_enable(uint32_t length);                                                                           /* 开启ADC DMA读取 */
void adc_nch_dma_init(uint32_t memory_base);                                                                    /* 初始化多通道ADC DMA读取 */
void adc_nch_dma_enable(uint32_t length);                                                                       /* 开启多通道ADC DMA读取 */

#endif
