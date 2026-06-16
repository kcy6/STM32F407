/**
 ****************************************************************************************************
 * @file        stmflash.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       片上Flash驱动代码
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

#ifndef __STMFLASH_H
#define __STMFLASH_H

#include "./SYSTEM/sys/sys.h"

/* 函数声明 */
uint32_t stmflash_read_word(uint32_t addr);                                 /* 从指定地址读取一字的数据 */
void stmflash_read(uint32_t addr, uint32_t *buf, uint32_t length);          /* 从指定地址读取指定字的数据 */
void stmflash_write_nocheck(uint32_t addr, uint32_t *buf, uint16_t length); /* 往指定地址不检查地写入指定字的数据 */
void stmflash_write(uint32_t addr, uint32_t *buf, uint16_t length);         /* 往指定地址写入指定字的数据 */

#endif
