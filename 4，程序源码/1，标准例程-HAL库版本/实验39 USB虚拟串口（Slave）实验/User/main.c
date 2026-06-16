/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       USB虚拟串口（Slave）实验
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
#include "./MALLOC/malloc.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"

USBD_HandleTypeDef USBD_Device;             /* USB Device处理结构体 */
extern volatile uint8_t g_device_state;     /* USB连接 情况 */

int main(void)
{
    uint16_t len;
    uint16_t times = 0;
    uint8_t device_sta;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMCCM);               /* 初始化CCM内存池 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "USB CDC TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "USB Connecting...", RED);    /* 提示正在建立连接 */
    USBD_Init(&USBD_Device, &VCP_Desc, 0);                              /* 初始化USB */
    USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);                   /* 添加类 */
    USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);           /* 为MSC类添加回调函数 */
    USBD_Start(&USBD_Device);                                           /* 开启USB */
    
    while(1)
    {
        /* 连接状态改变 */
        if (device_sta != g_device_state)
        {
            if (device_sta == 1)
            {
                lcd_show_string(30, 110, 200, 16, 16, "USB Connected    ", RED);
                LED1(0);
            }
            else
            {
                lcd_show_string(30, 110, 200, 16, 16, "USB Disconnected ", RED);
                LED1(1);
            }
            device_sta = g_device_state;
        }
        
        /* 接收到数据 */
        if (g_usb_usart_rx_sta & 0x8000)
        {
            len = g_usb_usart_rx_sta & 0x3FFF;
            usb_printf("\r\n您发送的消息为：\r\n");
            cdc_vcp_data_tx(g_usb_usart_rx_buffer, len);
            usb_printf("\r\n\r\n");
            g_usb_usart_rx_sta = 0;
        }
        else
        {
            if ((times % 5000) == 0)
            {
                usb_printf("\r\n正点原子 M144Z-M4最小系统板STM32F407版 USB虚拟串口实验\r\n");
                usb_printf("正点原子@ALIENTEK\r\n\r\n\r\n");
            }
            if ((times % 200) == 0)
            {
                usb_printf("请输入数据，以回车键结束\r\n");
            }
            if ((times % 30) == 0)
            {
                LED0_TOGGLE();
            }
            times++;
        }
        
        delay_ms(10);
    }
}
