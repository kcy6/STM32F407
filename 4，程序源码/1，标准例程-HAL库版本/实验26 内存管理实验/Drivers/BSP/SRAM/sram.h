/**
 ****************************************************************************************************
 * @file        sram.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       外部SRAM驱动代码
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

#ifndef __SRAM_H
#define __SRAM_H

#include "./SYSTEM/sys/sys.h"

/* 外部SRAM部分引脚定义
 * 外部SRAM D0~D15、A0~A18和NBL0~NBL1引脚直接在函数sram_init()中定义
 */
#define SRAM_WR_GPIO_PORT           GPIOD
#define SRAM_WR_GPIO_PIN            GPIO_PIN_5
#define SRAM_WR_GPIO_AF             GPIO_AF12_FSMC
#define SRAM_WR_GPIO_CLK_ENABLE()   do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

#define SRAM_RD_GPIO_PORT           GPIOD
#define SRAM_RD_GPIO_PIN            GPIO_PIN_4
#define SRAM_RD_GPIO_AF             GPIO_AF12_FSMC
#define SRAM_RD_GPIO_CLK_ENABLE()   do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

#define SRAM_CS_GPIO_PORT           GPIOG
#define SRAM_CS_GPIO_PIN            GPIO_PIN_10
#define SRAM_CS_GPIO_AF             GPIO_AF12_FSMC
#define SRAM_CS_GPIO_CLK_ENABLE()   do { __HAL_RCC_GPIOG_CLK_ENABLE(); } while (0)

/* FSMC相关参数定义
 * 默认通过FSMC的存储块1来连接外部SRAM，存储块1有4个片选（SMC_NE1~4）对应FSMC存储块1的4个区域块
 */
#define SRAM_FSMC_NEX               3   /* 使用FSMC_NE3连接SRAM_CS，范围：1~4 */

/* 外部SRAM基地址的详细计算方法：
 * 一般使用FSMC的存储块1来驱动TFTLCD液晶屏（MCU屏），存储块1地址范围总大小为256MB，均分成4区域块：
 * 存储块1区域块1（FSMC_NE1）地址范围：0x60000000~0x63FFFFFF
 * 存储块1区域块2（FSMC_NE2）地址范围：0x64000000~0x67FFFFFF
 * 存储块1区域块3（FSMC_NE3）地址范围：0x68000000~0x6BFFFFFF
 * 存储块1区域块4（FSMC_NE4）地址范围：0x6C000000~0x6FFFFFFF
 */
#define SRAM_BASE_ADDR              (uint32_t)(0x60000000 + (0x4000000 * (SRAM_FSMC_NEX - 1)))

/* 函数声明 */
void sram_init(void);                                               /* 初始化外部SRAM */
void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen);     /* 外部SRAM读数据 */
void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen);    /* 外部SRAM写数据 */
uint8_t sram_test_read(uint32_t addr);                              /* 外部SRAM读测试 */
void sram_test_write(uint32_t addr, uint8_t data);                  /* 外部SRAM写测试 */

#endif
