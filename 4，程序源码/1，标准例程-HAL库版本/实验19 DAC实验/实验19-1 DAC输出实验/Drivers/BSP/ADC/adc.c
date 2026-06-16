/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       ADC驱动代码
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

#include "./BSP/ADC/adc.h"

/* ADC句柄 */
ADC_HandleTypeDef g_adc_handle = {0};
ADC_HandleTypeDef g_adc_dma_handle = {0};
ADC_HandleTypeDef g_adc_nch_dma_handle = {0};

/* 单通道ADC采集（DMA读取）相关变量 */
DMA_HandleTypeDef g_adc_dma_dma_handle = {0};
uint8_t g_adc_dma_sta = 0;
uint32_t g_adc_dma_memory_base;

/* 多通道ADC采集（DMA读取）相关变量 */
DMA_HandleTypeDef g_adc_nch_dma_dma_handle = {0};
uint8_t g_adc_nch_dma_sta = 0;
uint32_t g_adc_nch_dma_memory_base;

/**
 * @brief   初始化ADC
 * @param   无
 * @retval  无
 */
void adc_init(void)
{
    /* 配置ADC */
    g_adc_handle.Instance = ADC_ADCX;
    g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    g_adc_handle.Init.ScanConvMode = DISABLE;
    g_adc_handle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    g_adc_handle.Init.ContinuousConvMode = DISABLE;
    g_adc_handle.Init.NbrOfConversion = 1;
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;
    g_adc_handle.Init.NbrOfDiscConversion = 1;
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    g_adc_handle.Init.DMAContinuousRequests = DISABLE;
    HAL_ADC_Init(&g_adc_handle);
}

/**
 * @brief   HAL库ADC初始化MSP函数
 * @param   hadc: ADC句柄
 * @retval  无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    
    if (hadc->Instance == ADC_ADCX)
    {
        ADC_ADCX_CLK_ENABLE();
        ADC_ADCX_CHY_GPIO_CLK_ENABLE();
        
        /* 初始化ADC采样引脚 */
        gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;
        gpio_init_struct.Pull = GPIO_NOPULL;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);
    }
    else if (hadc->Instance == ADC_DMA_ADCX)
    {
        ADC_DMA_ADCX_CLK_ENABLE();
        ADC_DMA_ADCX_CHY_GPIO_CLK_ENABLE();
        ADC_DMA_ADCX_DMA_CLK_ENABLE();
        
        /* 初始化ADC采样引脚 */
        gpio_init_struct.Pin = ADC_DMA_ADCX_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;
        gpio_init_struct.Pull = GPIO_NOPULL;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(ADC_DMA_ADCX_CHY_GPIO_PORT, &gpio_init_struct);
        
        /* 配置DMA */
        g_adc_dma_dma_handle.Instance = ADC_DMA_ADCX_DMASX;
        g_adc_dma_dma_handle.Init.Channel = ADC_DMA_ADCX_DMASX_CHY;
        g_adc_dma_dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
        g_adc_dma_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
        g_adc_dma_dma_handle.Init.MemInc = DMA_MINC_ENABLE;
        g_adc_dma_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        g_adc_dma_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        g_adc_dma_dma_handle.Init.Mode = DMA_NORMAL;
        g_adc_dma_dma_handle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        g_adc_dma_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        g_adc_dma_dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
        g_adc_dma_dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;
        g_adc_dma_dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
        HAL_DMA_Init(&g_adc_dma_dma_handle);
        
        __HAL_LINKDMA(&g_adc_dma_handle, DMA_Handle, g_adc_dma_dma_handle);
        
        HAL_NVIC_SetPriority(ADC_DMA_ADCX_DMASX_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(ADC_DMA_ADCX_DMASX_IRQn);
    }
    else if (hadc->Instance == ADC_NCH_DMA_ADCX)
    {
        ADC_NCH_DMA_ADCX_CLK_ENABLE();
        ADC_NCH_DMA_ADCX_CHA_GPIO_CLK_ENABLE();
        ADC_NCH_DMA_ADCX_CHB_GPIO_CLK_ENABLE();
        ADC_NCH_DMA_ADCX_DMA_CLK_ENABLE();
        
        /* 初始化ADC采样引脚 */
        gpio_init_struct.Pin = ADC_NCH_DMA_ADCX_CHA_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;
        gpio_init_struct.Pull = GPIO_NOPULL;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(ADC_NCH_DMA_ADCX_CHA_GPIO_PORT, &gpio_init_struct);
        
        gpio_init_struct.Pin = ADC_NCH_DMA_ADCX_CHB_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;
        gpio_init_struct.Pull = GPIO_NOPULL;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(ADC_NCH_DMA_ADCX_CHB_GPIO_PORT, &gpio_init_struct);
        
        /* 配置DMA */
        g_adc_nch_dma_dma_handle.Instance = ADC_NCH_DMA_ADCX_DMASX;
        g_adc_nch_dma_dma_handle.Init.Channel = ADC_NCH_DMA_ADCX_DMASX_CHY;
        g_adc_nch_dma_dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
        g_adc_nch_dma_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
        g_adc_nch_dma_dma_handle.Init.MemInc = DMA_MINC_ENABLE;
        g_adc_nch_dma_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        g_adc_nch_dma_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        g_adc_nch_dma_dma_handle.Init.Mode = DMA_NORMAL;
        g_adc_nch_dma_dma_handle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        g_adc_nch_dma_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        g_adc_nch_dma_dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
        g_adc_nch_dma_dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;
        g_adc_nch_dma_dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
        HAL_DMA_Init(&g_adc_nch_dma_dma_handle);
        
        __HAL_LINKDMA(&g_adc_nch_dma_handle, DMA_Handle, g_adc_nch_dma_dma_handle);
        
        HAL_NVIC_SetPriority(ADC_NCH_DMA_ADCX_DMASX_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(ADC_NCH_DMA_ADCX_DMASX_IRQn);
    }
}

/**
 * @brief   设置ADC通道
 * @param   adc_handle: ADC句柄
 * @param   channel: ADC通道
 * @param   rank: 规则采样的编号
 * @param   sampling_time: 采样时间
 * @retval  无
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t channel, uint32_t rank, uint32_t sampling_time)
{
    ADC_ChannelConfTypeDef adc_channel_conf_struct = {0};
    
    /* 配置ADC通道 */
    adc_channel_conf_struct.Channel = channel;
    adc_channel_conf_struct.Rank = rank;
    adc_channel_conf_struct.SamplingTime = sampling_time;
    adc_channel_conf_struct.Offset = 0;
    HAL_ADC_ConfigChannel(adc_handle, &adc_channel_conf_struct);
}

/**
 * @brief   获取ADC结果
 * @param   channel: ADC通道
 * @retval  ADC结果
 */
uint16_t adc_get_result(uint32_t channel)
{
    uint16_t result;
    
    adc_channel_set(&g_adc_handle, channel, 1, ADC_SAMPLETIME_480CYCLES);   /* 设置ADC通道 */
    HAL_ADC_Start(&g_adc_handle);                                           /* 开启ADC */
    HAL_ADC_PollForConversion(&g_adc_handle, HAL_MAX_DELAY);                /* ADC轮询转换 */
    result = HAL_ADC_GetValue(&g_adc_handle);                               /* 获取ADC值 */
    
    return result;
}

/**
 * @brief   均值滤波获取ADC结果
 * @param   channel: ADC通道
 * @param   times: 均值滤波的原始数据个数
 * @retval  ADC结果
 */
uint16_t adc_get_result_average(uint32_t channel, uint8_t times)
{
    uint32_t sum_result = 0;
    uint8_t index;
    uint16_t result;
    
    for (index=0; index<times; index++)
    {
        sum_result += adc_get_result(channel);
    }
    
    result = sum_result / times;
    
    return result;
}

/**
 * @brief   初始化ADC DMA读取
 * @param   memory_base: 读取目标内存基地址
 * @retval  无
 */
void adc_dma_init(uint32_t memory_base)
{
    /* 配置ADC */
    g_adc_dma_handle.Instance = ADC_DMA_ADCX;
    g_adc_dma_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    g_adc_dma_handle.Init.Resolution = ADC_RESOLUTION_12B;
    g_adc_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    g_adc_dma_handle.Init.ScanConvMode = DISABLE;
    g_adc_dma_handle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    g_adc_dma_handle.Init.ContinuousConvMode = ENABLE;
    g_adc_dma_handle.Init.NbrOfConversion = 1;
    g_adc_dma_handle.Init.DiscontinuousConvMode = DISABLE;
    g_adc_dma_handle.Init.NbrOfDiscConversion = 1;
    g_adc_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    g_adc_dma_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    g_adc_dma_handle.Init.DMAContinuousRequests = ENABLE;
    HAL_ADC_Init(&g_adc_dma_handle);
    
    g_adc_dma_memory_base = memory_base;
}

/**
 * @brief   开启ADC DMA读取
 * @param   length: DMA读取次数
 * @retval  无
 */
void adc_dma_enable(uint32_t length)
{
    HAL_ADC_Stop_DMA(&g_adc_dma_handle);
    
    adc_channel_set(&g_adc_dma_handle, ADC_DMA_ADCX_CHY, 1, ADC_SAMPLETIME_480CYCLES);
    
    HAL_ADC_Start_DMA(&g_adc_dma_handle, (uint32_t *)g_adc_dma_memory_base, length);
}

/**
 * @brief   初始化多通道ADC DMA读取
 * @param   memory_base: 读取目标内存基地址
 * @retval  无
 */
void adc_nch_dma_init(uint32_t memory_base)
{
    /* 配置ADC */
    g_adc_nch_dma_handle.Instance = ADC_NCH_DMA_ADCX;
    g_adc_nch_dma_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    g_adc_nch_dma_handle.Init.Resolution = ADC_RESOLUTION_12B;
    g_adc_nch_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    g_adc_nch_dma_handle.Init.ScanConvMode = ENABLE;
    g_adc_nch_dma_handle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    g_adc_nch_dma_handle.Init.ContinuousConvMode = ENABLE;
    g_adc_nch_dma_handle.Init.NbrOfConversion = ADC_NCH_DMA_ADCX_CH_NUM;
    g_adc_nch_dma_handle.Init.DiscontinuousConvMode = DISABLE;
    g_adc_nch_dma_handle.Init.NbrOfDiscConversion = 1;
    g_adc_nch_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    g_adc_nch_dma_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    g_adc_nch_dma_handle.Init.DMAContinuousRequests = ENABLE;
    HAL_ADC_Init(&g_adc_nch_dma_handle);
    
    g_adc_nch_dma_memory_base = memory_base;
}

/**
 * @brief   HAL库ADC转换完成回调函数
 * @param   hadc: ADC句柄
 * @retval  无
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC_DMA_ADCX)
    {
        g_adc_dma_sta = 1;
    }
    else if (hadc->Instance == ADC_NCH_DMA_ADCX)
    {
        g_adc_nch_dma_sta = 1;
    }
}

/**
 * @brief   DMA中断服务函数
 * @param   无
 * @retval  无
 */
void ADC_DMA_ADCX_DMASX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_adc_dma_dma_handle);
}

/**
 * @brief   开启多通道ADC DMA读取
 * @param   length: DMA读取次数
 * @retval  无
 */
void adc_nch_dma_enable(uint32_t length)
{
    HAL_ADC_Stop_DMA(&g_adc_nch_dma_handle);
    
    adc_channel_set(&g_adc_nch_dma_handle, ADC_NCH_DMA_ADCX_CHA, 1, ADC_SAMPLETIME_480CYCLES);
    adc_channel_set(&g_adc_nch_dma_handle, ADC_NCH_DMA_ADCX_CHB, 2, ADC_SAMPLETIME_480CYCLES);
    
    HAL_ADC_Start_DMA(&g_adc_nch_dma_handle, (uint32_t *)g_adc_nch_dma_memory_base, length);
}

/**
 * @brief   DMA中断服务函数
 * @param   无
 * @retval  无
 */
void ADC_NCH_DMA_ADCX_DMASX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_adc_nch_dma_dma_handle);
}
