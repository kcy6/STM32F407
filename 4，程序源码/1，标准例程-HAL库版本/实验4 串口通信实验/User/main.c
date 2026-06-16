/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       串口通信实验
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

int main(void)
{
    uint16_t len;
    uint16_t times = 0;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    
    while (1)
    {
        if (g_usart_rx_sta & 0x8000)    /* 接收到数据 */
        {
            len = g_usart_rx_sta & 0x3FFF;
            printf("\r\n您发送的消息为：\r\n");
            HAL_UART_Transmit(&g_uart1_handle, (uint8_t *)g_usart_rx_buf, len, HAL_MAX_DELAY);
            printf("\r\n\r\n");
            g_usart_rx_sta = 0;
        }
        else
        {
            if ((times % 5000) == 0)
            {
                printf("\r\n正点原子 M144Z-M4最小系统板STM32F407版 串口通信实验\r\n");
                printf("正点原子@ALIENTEK\r\n\r\n\r\n");
            }
            if ((times % 200) == 0)
            {
                printf("请输入数据，以回车键结束\r\n");
            }
            if ((times % 30) == 0)
            {
                LED0_TOGGLE();
            }
            times++;
            delay_ms(10);
        }
    }
}
