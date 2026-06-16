/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       DMA实验
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

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/DMA/dma.h"

extern UART_HandleTypeDef g_uart1_handle;

uint8_t temp[] = {"Hello, M144Z-M4 STM32F407!\r\n"};
uint8_t buf[sizeof(temp) * 200];
uint8_t uart_ready = 1;

/**
 * @brief   HAL库UART传输完成回调函数
 * @param   无
 * @retval  无
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    /* 标记传输完成，可以进行下一次传输 */
    uart_ready = 1;
}

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    uint16_t buf_index;
    uint8_t temp_index;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    dma_init();                         /* 初始化DMA */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DMA TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Start", RED);
    
    /* 准备数据 */
    for (buf_index=0; buf_index<200; buf_index++)
    {
        for (temp_index=0; temp_index<sizeof(temp); temp_index++)
        {
            buf[buf_index*sizeof(temp) + temp_index] = temp[temp_index];
        }
    }
    
    while (1)
    {
        key = key_scan(0);
        if (key == KEY0_PRES)
        {
            if (uart_ready == 1)
            {
                uart_ready = 0;
                /* DMA传输串口数据 */
                HAL_UART_Transmit_DMA(&g_uart1_handle, buf, sizeof(buf));
            }
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
