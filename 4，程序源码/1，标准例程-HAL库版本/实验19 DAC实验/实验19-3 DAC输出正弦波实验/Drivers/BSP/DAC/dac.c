/**
 ****************************************************************************************************
 * @file        dac.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       DAC驱动代码
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

#include "./BSP/DAC/dac.h"
#include "./SYSTEM/delay/delay.h"

/* DAC句柄 */
DAC_HandleTypeDef g_dac_dma_handle = {0};

/* DAC DMA输出相关变量 */
DMA_HandleTypeDef g_dac_dma_dma_handle = {0};
TIM_HandleTypeDef g_dac_dma_tim_handle = {0};
extern uint16_t g_dac_sin_buf[4096];

/**
 * @brief   初始化DAC DMA波形输出
 * @param   无
 * @retval  无
 */
void dac_dma_wave_init(void)
{
    /* 配置DAC */
    g_dac_dma_handle.Instance = DAC_DMA_DACX;
    HAL_DAC_Init(&g_dac_dma_handle);
}

/**
 * @brief   HAL库DAC初始化MSP函数
 * @param   hdac: DAC句柄
 * @retval  无
 */
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    DAC_ChannelConfTypeDef dac_channel_conf_struct = {0};
    TIM_MasterConfigTypeDef tim_master_config_struct = {0};
    
    if (hdac->Instance == DAC_DMA_DACX)
    {
        DAC_DMA_DACX_CLK_ENABLE();
        DAC_DMA_DACX_CHY_GPIO_CLK_ENABLE();
        DAC_DMA_DACX_DMA_CLK_ENABLE();
        DAC_DMA_DACX_TIMX_CLK_ENABLE();
        
        /* 配置DAC输出引脚 */
        gpio_init_struct.Pin = DAC_DMA_DACX_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;
        gpio_init_struct.Pull = GPIO_NOPULL;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DAC_DMA_DACX_CHY_GPIO_PORT, &gpio_init_struct);
        
        /* 配置DAC通道 */
        dac_channel_conf_struct.DAC_Trigger = DAC_TRIGGER_T7_TRGO;
        dac_channel_conf_struct.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
        HAL_DAC_ConfigChannel(hdac, &dac_channel_conf_struct, DAC_DMA_DACX_CHY);
        
        /* 配置DMA */
        g_dac_dma_dma_handle.Instance = DAC_DMA_DACX_DMASX;
        g_dac_dma_dma_handle.Init.Channel = DAC_DMA_DACX_DMASX_CHY;
        g_dac_dma_dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
        g_dac_dma_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
        g_dac_dma_dma_handle.Init.MemInc = DMA_MINC_ENABLE;
        g_dac_dma_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        g_dac_dma_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        g_dac_dma_dma_handle.Init.Mode = DMA_CIRCULAR;
        g_dac_dma_dma_handle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        g_dac_dma_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        g_dac_dma_dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
        g_dac_dma_dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;
        g_dac_dma_dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
        HAL_DMA_Init(&g_dac_dma_dma_handle);
        
        __HAL_LINKDMA(hdac, DMA_Handle1, g_dac_dma_dma_handle);
        
        HAL_NVIC_SetPriority(DAC_DMA_DACX_DMASX_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DAC_DMA_DACX_DMASX_IRQn);
        
        /* 配置TIM */
        g_dac_dma_tim_handle.Instance = DAC_DMA_DACX_TIMX;
        HAL_TIM_Base_Init(&g_dac_dma_tim_handle);
        
        tim_master_config_struct.MasterOutputTrigger = TIM_TRGO_UPDATE;
        HAL_TIMEx_MasterConfigSynchronization(&g_dac_dma_tim_handle, &tim_master_config_struct);
    }
}

/**
 * @brief   DMA中断服务函数
 * @param   无
 * @retval  无
 */
void DAC_DMA_DACX_DMASX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dac_dma_dma_handle);
}

/**
 * @brief   使能DAC DMA波形输出
 * @param   ndtr: DMA数据流传输一次数据项的数目
 * @param   arr: DAC触发定时器的自动重装载值
 * @param   psc: DAC触发定时器的预分频器数值
 * @retval  无
 */
void dac_dma_wave_enable(uint16_t ndtr, uint16_t arr, uint16_t psc)
{
    /* 配置TIM */
    g_dac_dma_tim_handle.Init.Prescaler = psc;
    g_dac_dma_tim_handle.Init.Period = arr;
    HAL_TIM_Base_Init(&g_dac_dma_tim_handle);
    HAL_TIM_Base_Start(&g_dac_dma_tim_handle);
    
    /* 重新开启DAC DMA传输 */
    HAL_DAC_Stop_DMA(&g_dac_dma_handle, DAC_DMA_DACX_CHY);
    HAL_DAC_Start_DMA(&g_dac_dma_handle, DAC_DMA_DACX_CHY, (uint32_t *)g_dac_sin_buf, ndtr, DAC_ALIGN_12B_R);
}
