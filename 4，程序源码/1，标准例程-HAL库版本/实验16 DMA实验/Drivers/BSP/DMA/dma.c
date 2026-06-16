/**
 ****************************************************************************************************
 * @file        dma.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       DMA驱动代码
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

#include "./BSP/DMA/dma.h"

/* DMA句柄 */
DMA_HandleTypeDef g_dma_handle = {0};

extern UART_HandleTypeDef g_uart1_handle;

/**
 * @brief   初始化DMA
 * @param   无
 * @retval  无
 */
void dma_init(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE();
    
    /* 配置DMA */
    g_dma_handle.Instance = DMA2_Stream7;
    g_dma_handle.Init.Channel = DMA_CHANNEL_4;
    g_dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    g_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_dma_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    g_dma_handle.Init.Mode = DMA_NORMAL;
    g_dma_handle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    g_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    g_dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
    g_dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;
    g_dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&g_dma_handle);
    
    /* 与UART句柄关联 */
    __HAL_LINKDMA(&g_uart1_handle, hdmatx, g_dma_handle);
    
    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

/**
 * @brief   DMA2 Stream7中断服务函数
 * @param   无
 * @retval  无
 */
void DMA2_Stream7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma_handle);
}
