/**
 ****************************************************************************************************
 * @file        sdio_sdcard.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       SD卡驱动代码
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

#include "./BSP/SDIO/sdio_sdcard.h"

/* SD句柄 */
SD_HandleTypeDef g_sd_handle = {0};

/* SD信息 */
HAL_SD_CardInfoTypeDef g_sd_card_info = {0};

/**
 * @brief   初始化SD卡
 * @param   无
 * @retval  初始化结果
 * @arg     0: 初始化成功
 * @arg     1: 初始化失败
 * @arg     2: 总线宽度设置失败
 */
uint8_t sd_init(void)
{
    /* 配置SD */
    g_sd_handle.Instance = SDIO;
    g_sd_handle.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    g_sd_handle.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    g_sd_handle.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    g_sd_handle.Init.BusWide = SDIO_BUS_WIDE_1B;
    g_sd_handle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    g_sd_handle.Init.ClockDiv = 1;
    if (HAL_SD_Init(&g_sd_handle) != HAL_OK)
    {
        return 1;
    }
    
    /* 获取SD信息 */
    HAL_SD_GetCardInfo(&g_sd_handle, &g_sd_card_info);
    
    /* 配置4bit总线宽度 */
    if (HAL_SD_ConfigWideBusOperation(&g_sd_handle, SDIO_BUS_WIDE_4B) != HAL_OK)
    {
        return 2;
    }
    
    return 0;
}

/**
 * @brief   HAL库SD初始化MSP函数
 * @param   hsd: SD句柄
 * @retval  无
 */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    if (hsd->Instance == SDIO)
    {
        __HAL_RCC_SDIO_CLK_ENABLE();
        SD_D0_GPIO_CLK_ENABLE();
        SD_D1_GPIO_CLK_ENABLE();
        SD_D2_GPIO_CLK_ENABLE();
        SD_D3_GPIO_CLK_ENABLE();
        SD_SCK_GPIO_CLK_ENABLE();
        SD_CMD_GPIO_CLK_ENABLE();
        
        /* 配置D0引脚 */
        gpio_init_struct.Pin = SD_D0_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = SD_D0_GPIO_AF;
        HAL_GPIO_Init(SD_D0_GPIO_PORT, &gpio_init_struct);
        
        /* 配置D1引脚 */
        gpio_init_struct.Pin = SD_D1_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = SD_D1_GPIO_AF;
        HAL_GPIO_Init(SD_D1_GPIO_PORT, &gpio_init_struct);
        
        /* 配置D2引脚 */
        gpio_init_struct.Pin = SD_D2_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = SD_D2_GPIO_AF;
        HAL_GPIO_Init(SD_D2_GPIO_PORT, &gpio_init_struct);
        
        /* 配置D3引脚 */
        gpio_init_struct.Pin = SD_D3_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = SD_D3_GPIO_AF;
        HAL_GPIO_Init(SD_D3_GPIO_PORT, &gpio_init_struct);
        
        /* 配置SCK引脚 */
        gpio_init_struct.Pin = SD_SCK_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = SD_SCK_GPIO_AF;
        HAL_GPIO_Init(SD_SCK_GPIO_PORT, &gpio_init_struct);
        
        /* 配置CMD引脚 */
        gpio_init_struct.Pin = SD_CMD_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = SD_CMD_GPIO_AF;
        HAL_GPIO_Init(SD_CMD_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief   获取SD卡信息
 * @param   info: SD卡信息
 * @retval  获取结果
 * @arg     0:获取成功
 * @arg     1:获取失败
 */
uint8_t sd_get_card_info(HAL_SD_CardInfoTypeDef *info)
{
    if (HAL_SD_GetCardInfo(&g_sd_handle, info) != HAL_OK)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief   读SD卡指定数量的块数据
 * @param   buf: 数据保存的起始地址
 * @param   addr: 块地址
 * @param   count: 块数量
 * @retval  读取结果
 * @arg     0: 读取成功
 * @arg     1: 读取失败
 */
uint8_t sd_read_disk(uint8_t *buf, uint32_t addr, uint32_t count)
{
    uint32_t timeout = SD_DATATIMEOUT;
    
    if (HAL_SD_ReadBlocks(&g_sd_handle, buf, addr, count, SD_DATATIMEOUT) != HAL_OK)
    {
        return 1;
    }
    
    while ((HAL_SD_GetCardState(&g_sd_handle) != HAL_SD_CARD_TRANSFER) && (--timeout != 0));
    
    if (timeout == 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief   写SD卡指定数量的块数据
 * @param   buf: 数据保存的起始地址
 * @param   addr: 块地址
 * @param   count: 块数量
 * @retval  写入结果
 * @arg     0: 写入成功
 * @arg     1: 写入失败
 */
uint8_t sd_write_disk(uint8_t *buf, uint32_t addr, uint32_t count)
{
    uint32_t timeout = SD_DATATIMEOUT;
    
    if (HAL_SD_WriteBlocks(&g_sd_handle, buf, addr, count, SD_DATATIMEOUT) != HAL_OK)
    {
        return 1;
    }
    
    while ((HAL_SD_GetCardState(&g_sd_handle) != HAL_SD_CARD_TRANSFER) && (--timeout != 0));
    
    if (timeout == 0)
    {
        return 1;
    }
    
    return 0;
}
