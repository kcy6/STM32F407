/**
 ****************************************************************************************************
 * @file        iap.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       IAP代码
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

#ifndef __IAP_H
#define __IAP_H

#include "./SYSTEM/sys/sys.h"

/* 用于保存APP的起始地址 */
#define FLASH_APP1_ADDR 0x08010000

/* 函数声明 */
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);   /* 在指定地址写入APP的bin数据 */
void iap_load_app(uint32_t appxaddr);                                       /* 跳转到APP程序执行 */

#endif
