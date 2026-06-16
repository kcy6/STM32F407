/**
 ****************************************************************************************************
 * @file        usart2.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       USART2驱动代码
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

#include "./BSP/USART2/usart2.h"

/* USART2句柄 */
UART_HandleTypeDef g_usart2_handle = {0};

/**
 * @brief   初始化USART2
 * @param   baudrate: 通信波特率
 * @retval  无
 */
void usart2_init(uint32_t baudrate)
{
    g_usart2_handle.Instance = USART2_UX;
    g_usart2_handle.Init.BaudRate = baudrate;

    g_usart2_handle.Init.WordLength = UART_WORDLENGTH_8B;
    g_usart2_handle.Init.StopBits = UART_STOPBITS_1;
    g_usart2_handle.Init.Parity = UART_PARITY_NONE;
    g_usart2_handle.Init.Mode = UART_MODE_TX;
    g_usart2_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_usart2_handle.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&g_usart2_handle);
}
