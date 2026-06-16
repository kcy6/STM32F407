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
ADC_HandleTypeDef g_adc_temperature_handle = {0};

/**
 * @brief   初始化ADC采集内部温度传感器
 * @param   无
 * @retval  无
 */
void adc_temperature_init(void)
{
    /* 配置ADC */
    g_adc_temperature_handle.Instance = ADC1;
    g_adc_temperature_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    g_adc_temperature_handle.Init.Resolution = ADC_RESOLUTION_12B;
    g_adc_temperature_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    g_adc_temperature_handle.Init.ScanConvMode = DISABLE;
    g_adc_temperature_handle.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    g_adc_temperature_handle.Init.ContinuousConvMode = DISABLE;
    g_adc_temperature_handle.Init.NbrOfConversion = 1;
    g_adc_temperature_handle.Init.DiscontinuousConvMode = DISABLE;
    g_adc_temperature_handle.Init.NbrOfDiscConversion = 1;
    g_adc_temperature_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    g_adc_temperature_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    g_adc_temperature_handle.Init.DMAContinuousRequests = DISABLE;
    HAL_ADC_Init(&g_adc_temperature_handle);
}

/**
 * @brief   HAL库ADC初始化MSP函数
 * @param   hadc: ADC句柄
 * @retval  无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        __HAL_RCC_ADC1_CLK_ENABLE();
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
    
    adc_channel_set(&g_adc_temperature_handle, channel, 1, ADC_SAMPLETIME_480CYCLES);   /* 设置ADC通道 */
    HAL_ADC_Start(&g_adc_temperature_handle);                                           /* 开启ADC */
    HAL_ADC_PollForConversion(&g_adc_temperature_handle, HAL_MAX_DELAY);                /* ADC轮询转换 */
    result = HAL_ADC_GetValue(&g_adc_temperature_handle);                               /* 获取ADC值 */
    
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
 * @brief   获取内部温度传感器结果
 * @param   无
 * @retval  内部温度传感器结果（扩大100倍）
 */
int16_t adc_get_temperature(void)
{
    uint16_t result;
    double voltage;
    double temperature;
    int16_t temperature_x100;
    
    result = adc_get_result_average(ADC_CHANNEL_TEMPSENSOR, 10);
    voltage = ((double)result * 3.3) / 4095;
    temperature = (voltage - 0.76) * 400 + 25;
    temperature_x100 = (int16_t)(temperature * 100);
    
    return temperature_x100;
}
