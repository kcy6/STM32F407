/**
 ****************************************************************************************************
 * @file        spi.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       SPI驱动代码
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

#ifndef __SPI_H
#define __SPI_H

#include "./SYSTEM/sys/sys.h"

/* SPI定义 */
#define SPI1_SPI                    SPI1
#define SPI1_SPI_CLK_ENABLE()       do { __HAL_RCC_SPI1_CLK_ENABLE(); } while (0)
#define SPI1_SCK_GPIO_PORT          GPIOB
#define SPI1_SCK_GPIO_PIN           GPIO_PIN_3
#define SPI1_SCK_GPIO_AF            GPIO_AF5_SPI1
#define SPI1_SCK_GPIO_CLK_ENABLE()  do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)
#define SPI1_MISO_GPIO_PORT         GPIOB
#define SPI1_MISO_GPIO_PIN          GPIO_PIN_4
#define SPI1_MISO_GPIO_AF            GPIO_AF5_SPI1
#define SPI1_MISO_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)
#define SPI1_MOSI_GPIO_PORT         GPIOB
#define SPI1_MOSI_GPIO_PIN          GPIO_PIN_5
#define SPI1_MOSI_GPIO_AF            GPIO_AF5_SPI1
#define SPI1_MOSI_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)

/* 函数声明 */
void spi1_init(void);                           /* 初始化SPI1 */
void spi1_set_speed(uint32_t speed);            /* 设置SPI1通信波特率 */
uint8_t spi1_read_write_byte(uint8_t txdata);   /* SPI1读写一字节数据 */

#endif
