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
DAC_HandleTypeDef g_dac_handle = {0};

/**
 * @brief   初始化DAC
 * @param   无
 * @retval  无
 */
void dac_init(void)
{
    /* 配置DAC */
    g_dac_handle.Instance = DAC_DACX;
    HAL_DAC_Init(&g_dac_handle);
    HAL_DAC_Start(&g_dac_handle, DAC_DACX_CHY);
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
    
    if (hdac->Instance == DAC_DACX)
    {
        DAC_DACX_CLK_ENABLE();
        DAC_DACX_CHY_GPIO_CLK_ENABLE();
        
        /* 配置DAC输出引脚 */
        gpio_init_struct.Pin = DAC_DACX_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;
        gpio_init_struct.Pull = GPIO_NOPULL;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DAC_DACX_CHY_GPIO_PORT, &gpio_init_struct);
        
        /* 配置DAC通道 */
        dac_channel_conf_struct.DAC_Trigger = DAC_TRIGGER_NONE;
        dac_channel_conf_struct.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
        HAL_DAC_ConfigChannel(&g_dac_handle, &dac_channel_conf_struct, DAC_DACX_CHY);
    }
}

/**
 * @brief   设置DAC输出电压
 * @param   voltage: DAC输出电压（扩大1000倍）
 * @retval  无
 */
void dac_set_voltage(uint16_t voltage)
{
    uint16_t value;
    
    value = (voltage * 4095) / 3300;
    value &= 0xFFF;
    HAL_DAC_SetValue(&g_dac_handle, DAC_DACX_CHY, DAC_ALIGN_12B_R, value);
}

/**
 * @brief   设置DAC输出三角波
 * @param   max_value: 三角波波峰电压数字量
 * @param   interval: 每个采样点的时间间隔，单位：微妙
 * @param   samples: 一个三角波周期采样点的个数
 * @param   number: 输出三角波的个数
 * @retval  无
 */
void dac_triangular_wave(uint16_t max_value, uint16_t interval, uint16_t samples, uint16_t number)
{
    uint16_t incval;
    uint16_t curval;
    uint16_t sample_index;
    uint16_t wave_index;
    
    /* 确保采样点个数为偶数 */
    samples = ((samples + 1) >> 1) << 1;
    /* 计算每个采样点的步进值 */
    incval = max_value / (samples >> 1);
    if (incval == 0)
    {
        return;
    }
    
    for (wave_index=0; wave_index<number; wave_index++)
    {
        curval = 0;
        HAL_DAC_SetValue(&g_dac_handle, DAC_DACX_CHY, DAC_ALIGN_12B_R, curval);
        
        /* 输出上升沿 */
        for (sample_index=0; sample_index<(samples >> 1); sample_index++)
        {
            curval += incval;
            HAL_DAC_SetValue(&g_dac_handle, DAC_DACX_CHY, DAC_ALIGN_12B_R, curval);
            delay_us(interval);
        }
        
        /* 输出下降沿 */
        for (sample_index=0; sample_index<(samples >> 1); sample_index++)
        {
            curval -= incval;
            HAL_DAC_SetValue(&g_dac_handle, DAC_DACX_CHY, DAC_ALIGN_12B_R, curval);
            delay_us(interval);
        }
    }
}
