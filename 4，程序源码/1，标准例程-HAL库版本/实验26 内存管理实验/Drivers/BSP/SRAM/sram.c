/**
 ****************************************************************************************************
 * @file        sram.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       外部SRAM驱动代码
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

#include "./BSP/SRAM/sram.h"

static SRAM_HandleTypeDef g_sram_handle = {0};   /* SRAM句柄 */

/**
 * @brief       初始化外部SRAM
 * @param       无
 * @retval      无
 */
void sram_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    FSMC_NORSRAM_TimingTypeDef fsmc_timing_struct = {0};
    
    /* 使能时钟 */
    __HAL_RCC_FSMC_CLK_ENABLE();
    SRAM_WR_GPIO_CLK_ENABLE();
    SRAM_RD_GPIO_CLK_ENABLE();
    SRAM_CS_GPIO_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    
    /* 配置FSMC_NWE引脚 */
    gpio_init_struct.Pin = SRAM_WR_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = SRAM_WR_GPIO_AF;
    HAL_GPIO_Init(SRAM_WR_GPIO_PORT, &gpio_init_struct);
    
    /* 配置FSMC_NOE引脚 */
    gpio_init_struct.Pin = SRAM_RD_GPIO_PIN;
    HAL_GPIO_Init(SRAM_RD_GPIO_PORT, &gpio_init_struct);
    
    /* 配置FSMC_NEx引脚 */
    gpio_init_struct.Pin = SRAM_CS_GPIO_PIN;
    HAL_GPIO_Init(SRAM_CS_GPIO_PORT, &gpio_init_struct);
    
    /* 配置FSMC_NBLn引脚 */
    gpio_init_struct.Pin = GPIO_PIN_0 |
                           GPIO_PIN_1;
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);
    
    /* 配置FSMC_Dn引脚 */
    gpio_init_struct.Pin = GPIO_PIN_14 |
                           GPIO_PIN_15 |
                           GPIO_PIN_0 |
                           GPIO_PIN_1 |
                           GPIO_PIN_8 |
                           GPIO_PIN_9 |
                           GPIO_PIN_10;
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);
    gpio_init_struct.Pin = GPIO_PIN_7 |
                           GPIO_PIN_8 |
                           GPIO_PIN_9 |
                           GPIO_PIN_10 |
                           GPIO_PIN_11 |
                           GPIO_PIN_12 |
                           GPIO_PIN_13 |
                           GPIO_PIN_14 |
                           GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);
    
    /* 配置FSMC_An引脚 */
    gpio_init_struct.Pin = GPIO_PIN_0 |
                           GPIO_PIN_1 |
                           GPIO_PIN_2 |
                           GPIO_PIN_3 |
                           GPIO_PIN_4 |
                           GPIO_PIN_5 |
                           GPIO_PIN_12 |
                           GPIO_PIN_13 |
                           GPIO_PIN_14 |
                           GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &gpio_init_struct);
    gpio_init_struct.Pin = GPIO_PIN_0 |
                           GPIO_PIN_1 |
                           GPIO_PIN_2 |
                           GPIO_PIN_3 |
                           GPIO_PIN_4 |
                           GPIO_PIN_5;
    HAL_GPIO_Init(GPIOG, &gpio_init_struct);
    gpio_init_struct.Pin = GPIO_PIN_11 |
                           GPIO_PIN_12 |
                           GPIO_PIN_13;
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);
    
    /* 配置FSMC时序 */
    fsmc_timing_struct.AddressSetupTime = 0x02;
    fsmc_timing_struct.AddressHoldTime = 0x00;
    fsmc_timing_struct.DataSetupTime = 0x08;
    fsmc_timing_struct.AccessMode = FSMC_ACCESS_MODE_A;
    
    /* 配置FSMC */
    g_sram_handle.Instance = FSMC_NORSRAM_DEVICE;
    g_sram_handle.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
    g_sram_handle.Init.NSBank = (SRAM_FSMC_NEX == 1) ? FSMC_NORSRAM_BANK1 :
                                (SRAM_FSMC_NEX == 2) ? FSMC_NORSRAM_BANK2 :
                                (SRAM_FSMC_NEX == 3) ? FSMC_NORSRAM_BANK3 :
                                                       FSMC_NORSRAM_BANK4;
    g_sram_handle.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
    g_sram_handle.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
    g_sram_handle.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    g_sram_handle.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    g_sram_handle.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    g_sram_handle.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
    g_sram_handle.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    g_sram_handle.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    g_sram_handle.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    g_sram_handle.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
    g_sram_handle.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    g_sram_handle.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
    g_sram_handle.Init.PageSize = FSMC_PAGE_SIZE_NONE;
    
    HAL_SRAM_Init(&g_sram_handle, &fsmc_timing_struct, NULL);
}

/**
 * @brief       从外部SRAM的指定地址读指定长度数据
 * @param       pbuf   : 保存读出数据的起始地址
 * @param       addr   : 指定读出数据的起始地址
 * @param       datalen: 指定读出数据的长度
 * @retval      无
 */
void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen!=0; datalen--)
    {
        *pbuf = *(volatile uint8_t *)(SRAM_BASE_ADDR + addr);
        pbuf++;
        addr++;
    }
}

/**
 * @brief       往外部SRAM的指定地址写指定长度的数据
 * @param       pbuf   : 待写入的数据
 * @param       addr   : 指定写入数据的起始地址
 * @param       datalen: 指定写入数据的长度
 * @retval      无
 */
void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen!=0; datalen--)
    {
        *(volatile uint8_t *)(SRAM_BASE_ADDR + addr) = *pbuf;
        pbuf++;
        addr++;
    }
}

/**
 * @brief       外部SRAM读测试
 * @param       addr: 指定读出数据的地址
 * @retval      测试读出的一字节数据
 */
uint8_t sram_test_read(uint32_t addr)
{
    uint8_t data;
    
    sram_read(&data, addr, 1);
    
    return data;
}

/**
 * @brief       外部SRAM写测试
 * @param       addr: 指定写入数据的地址
 * @param       data: 待写入的一字节数据
 * @retval      无
 */
void sram_test_write(uint32_t addr, uint8_t data)
{
    sram_write(&data, addr, 1);
}
