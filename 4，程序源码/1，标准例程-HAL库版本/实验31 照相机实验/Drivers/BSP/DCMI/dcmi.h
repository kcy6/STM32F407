/**
 ****************************************************************************************************
 * @file        dcmi.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       DCMI驱动代码
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

#ifndef __DCMI_H
#define __DCMI_H

#include "./SYSTEM/sys/sys.h"

extern DCMI_HandleTypeDef g_dcmi_handle;
extern DMA_HandleTypeDef g_dcmi_dma_handle;
extern void (*dcmi_rx_callback)(void);

/* 函数声明 */
void dcmi_init(void);                                                                                           /* 初始化DCMI */
void dcmi_dma_init(uint32_t mem0addr, uint32_t mem1addr, uint16_t memsize, uint32_t memblen, uint32_t meminc);  /* 初始化DCMI DMA */
void dcmi_start(void);                                                                                          /* 启动DCMI传输 */
void dcmi_stop(void);                                                                                           /* 停止DCMI传输 */
void dcmi_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);                                /* 设置DCMI显示窗口 */

#endif
