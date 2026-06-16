/**
 ****************************************************************************************************
 * @file        sccb.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       SCCB驱动代码
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

#include "./BSP/OV2640/sccb.h"
#include "./SYSTEM/delay/delay.h"

/**
 * @brief   SCCB延时函数
 * @note    用于控制SCCB通信速率
 * @param   无
 * @retval  无
 */
static void sccb_delay(void)
{
    delay_us(50);
}

/**
 * @brief   初始化SCCB
 * @param   无
 * @retval  无
 */
void sccb_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* 时钟使能 */
    SCCB_SCL_GPIO_CLK_ENABLE();
    SCCB_SDA_GPIO_CLK_ENABLE();
    
    /* 配置SCL引脚 */
    gpio_init_struct.Pin = SCCB_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SCCB_SCL_GPIO_PORT, &gpio_init_struct);
    
    /* 配置SDA引脚 */
    gpio_init_struct.Pin = SCCB_SDA_GPIO_PIN;
#if (OV_SCCB_TYPE_NOD != 0)
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
#else
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
#endif
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SCCB_SDA_GPIO_PORT, &gpio_init_struct);
    
    sccb_stop();
}

/**
 * @brief   产生SCCB起始信号
 * @param   无
 * @retval  无
 */
void sccb_start(void)
{
    SCCB_SDA(1);
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SDA(0);
    sccb_delay();
    SCCB_SCL(0);
}

/**
 * @brief   产生SCCB停止信号
 * @param   无
 * @retval  无
 */
void sccb_stop(void)
{
    SCCB_SDA(0);
    sccb_delay();
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SDA(1);
    sccb_delay();
}

/**
 * @brief   产生SCCB NACK信号
 * @param   无
 * @retval  无
 */
void sccb_nack(void)
{
    sccb_delay();
    SCCB_SDA(1);
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SCL(0);
    sccb_delay();
    SCCB_SDA(0);
    sccb_delay();
}

/**
 * @brief   SCCB发送一个字节
 * @param   data: 待发送的一字节数据
 * @retval  发送结果
 * @arg     0: 发送成功
 * @arg     1: 发送失败
 */
uint8_t sccb_send_byte(uint8_t data)
{
    uint8_t t;
    uint8_t res;
    
    for (t=0; t<8; t++)
    {
        SCCB_SDA((data & 0x80) >> 7);
        sccb_delay();
        SCCB_SCL(1);
        sccb_delay();
        SCCB_SCL(0);
        data <<= 1;
    }
#if (OV_SCCB_TYPE_NOD != 0)
    SCCB_SDA_IN();
    sccb_delay();
#endif
    SCCB_SDA(1);
    sccb_delay();
    
    SCCB_SCL(1);
    sccb_delay();
    if (SCCB_SDA_READ)
    {
        res = 1;
    }
    else
    {
        res = 0;
    }
    SCCB_SCL(0);
#if (OV_SCCB_TYPE_NOD != 0)
    SCCB_SDA_OUT();
    sccb_delay();
#endif
    
    return res;
}

/**
 * @brief   SCCB读取一个字节
 * @param   无
 * @retval  读取到的一字节数据
 */
uint8_t sccb_read_byte(void)
{
    uint8_t i;
    uint8_t receive = 0;
    
#if (OV_SCCB_TYPE_NOD != 0)
    SCCB_SDA_IN();
    sccb_delay();
#endif
    for (i=0; i<8; i++)
    {
        receive <<= 1;
        SCCB_SCL(1);
        sccb_delay();
        
        if (SCCB_SDA_READ)
        {
            receive++;
        }
        
        SCCB_SCL(0);
        sccb_delay();
    }
    
#if (OV_SCCB_TYPE_NOD != 0)
    SCCB_SDA_OUT();         /* 设置SDA为输出 */
    sccb_delay();
#endif
    
    return receive;
}
