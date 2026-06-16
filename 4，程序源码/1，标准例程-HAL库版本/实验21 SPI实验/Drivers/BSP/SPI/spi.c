/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       SPI驱动代码
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

#include "./BSP/SPI/spi.h"

/* SPI句柄 */
SPI_HandleTypeDef g_spi1_handle = {0};

/**
 * @brief   初始化SPI1
 * @param   无
 * @retval  无
 */
void spi1_init(void)
{
    /* 配置SPI1 */
    g_spi1_handle.Instance = SPI1_SPI;
    g_spi1_handle.Init.Mode = SPI_MODE_MASTER;
    g_spi1_handle.Init.Direction = SPI_DIRECTION_2LINES;
    g_spi1_handle.Init.DataSize = SPI_DATASIZE_8BIT;
    g_spi1_handle.Init.CLKPolarity = SPI_POLARITY_HIGH;
    g_spi1_handle.Init.CLKPhase = SPI_PHASE_2EDGE;
    g_spi1_handle.Init.NSS = SPI_NSS_SOFT;
    g_spi1_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    g_spi1_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    g_spi1_handle.Init.TIMode = SPI_TIMODE_DISABLE;
    g_spi1_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    g_spi1_handle.Init.CRCPolynomial = 1;
    HAL_SPI_Init(&g_spi1_handle);
}

/**
 * @brief   HAL库SPI1初始化MSP函数
 * @param   无
 * @retval  无
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    if (hspi->Instance == SPI1_SPI)
    {
        /* 时钟使能 */
        SPI1_SPI_CLK_ENABLE();
        SPI1_SCK_GPIO_CLK_ENABLE();
        SPI1_MISO_GPIO_CLK_ENABLE();
        SPI1_MOSI_GPIO_CLK_ENABLE();
        
        /* 配置SCK引脚 */
        gpio_init_struct.Pin = SPI1_SCK_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = SPI1_SCK_GPIO_AF;
        HAL_GPIO_Init(SPI1_SCK_GPIO_PORT, &gpio_init_struct);
        
        /* 配置MISO引脚 */
        gpio_init_struct.Pin = SPI1_MISO_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = SPI1_MISO_GPIO_AF;
        HAL_GPIO_Init(SPI1_MISO_GPIO_PORT, &gpio_init_struct);
        
        /* 配置MOSI引脚 */
        gpio_init_struct.Pin = SPI1_MOSI_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = SPI1_MOSI_GPIO_AF;
        HAL_GPIO_Init(SPI1_MOSI_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief   设置SPI1通信波特率
 * @param   speed: SPI1波特率分频系数
 * @arg     SPI_BAUDRATEPRESCALER_2: 2分频
 * @arg     SPI_BAUDRATEPRESCALER_4: 2分频
 * @arg     SPI_BAUDRATEPRESCALER_8: 8分频
 * @arg     SPI_BAUDRATEPRESCALER_16: 16分频
 * @arg     SPI_BAUDRATEPRESCALER_32: 32分频
 * @arg     SPI_BAUDRATEPRESCALER_64: 64分频
 * @arg     SPI_BAUDRATEPRESCALER_128: 128分频
 * @arg     SPI_BAUDRATEPRESCALER_256: 256分频
 * @retval  无
 */
void spi1_set_speed(uint32_t speed)
{
    __HAL_SPI_DISABLE(&g_spi1_handle);
    g_spi1_handle.Instance->CR1 &= ~SPI_CR1_BR_Msk;
    g_spi1_handle.Instance->CR1 |= speed;
    __HAL_SPI_ENABLE(&g_spi1_handle);
}
/**
 * @brief   SPI1读写一字节数据
 * @param   txdata: 待写入的一字节数据
 * @retval  读取到的一字节数据
 */
uint8_t spi1_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    
    if (HAL_SPI_TransmitReceive(&g_spi1_handle, &txdata, &rxdata, 1, 1000) != HAL_OK)
    {
        return 0;
    }
    
    return rxdata;
}
