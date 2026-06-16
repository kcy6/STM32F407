/**
 ****************************************************************************************************
 * @file        sdio_sdcard.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       SD卡驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 M144Z-M4最小系统板STM32F407版
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * GPIO_AF12_SDIO
 ****************************************************************************************************
 */

#ifndef __SDIO_SDCARD_H
#define __SDIO_SDCARD_H

#include "./SYSTEM/sys/sys.h"

/* 引脚定义 */
#define SD_D0_GPIO_PORT             GPIOC
#define SD_D0_GPIO_PIN              GPIO_PIN_8
#define SD_D0_GPIO_AF               GPIO_AF12_SDIO
#define SD_D0_GPIO_CLK_ENABLE()     do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)
#define SD_D1_GPIO_PORT             GPIOC
#define SD_D1_GPIO_PIN              GPIO_PIN_9
#define SD_D1_GPIO_AF               GPIO_AF12_SDIO
#define SD_D1_GPIO_CLK_ENABLE()     do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)
#define SD_D2_GPIO_PORT             GPIOC
#define SD_D2_GPIO_PIN              GPIO_PIN_10
#define SD_D2_GPIO_AF               GPIO_AF12_SDIO
#define SD_D2_GPIO_CLK_ENABLE()     do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)
#define SD_D3_GPIO_PORT             GPIOC
#define SD_D3_GPIO_PIN              GPIO_PIN_11
#define SD_D3_GPIO_AF               GPIO_AF12_SDIO
#define SD_D3_GPIO_CLK_ENABLE()     do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)
#define SD_SCK_GPIO_PORT            GPIOC
#define SD_SCK_GPIO_PIN             GPIO_PIN_12
#define SD_SCK_GPIO_AF              GPIO_AF12_SDIO
#define SD_SCK_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)
#define SD_CMD_GPIO_PORT            GPIOD
#define SD_CMD_GPIO_PIN             GPIO_PIN_2
#define SD_CMD_GPIO_AF              GPIO_AF12_SDIO
#define SD_CMD_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

/* SD卡操作超时时间定义 */
#define SD_DATATIMEOUT              ((uint32_t)100000000)

/* 变量导出 */
extern SD_HandleTypeDef g_sd_handle;
extern HAL_SD_CardInfoTypeDef g_sd_card_info;

/* 函数声明 */
uint8_t sd_init(void);                                              /* 初始化SD卡 */
uint8_t sd_read_disk(uint8_t *buf, uint32_t addr, uint32_t count);  /* 读SD卡指定数量的块数据 */
uint8_t sd_write_disk(uint8_t *buf, uint32_t addr, uint32_t count); /* 写SD卡指定数量的块数据 */

#endif
