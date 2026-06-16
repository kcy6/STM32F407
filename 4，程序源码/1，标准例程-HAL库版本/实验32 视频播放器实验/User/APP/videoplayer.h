/**
 ****************************************************************************************************
 * @file        videoplayer.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       视频播放器应用代码
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

#ifndef __VIDEOPLAYER_H
#define __VIDEOPLAYER_H

#include "./SYSTEM/sys/sys.h"

/* 缓存空间相关定义 */
#define AVI_VIDEO_BUF_SIZE (60 * 1024)
#define AVI_AUDIO_BUF_SIZE (5 * 1024)

/* 函数声明 */
void video_play(void);  /* 播放视频 */

#endif
