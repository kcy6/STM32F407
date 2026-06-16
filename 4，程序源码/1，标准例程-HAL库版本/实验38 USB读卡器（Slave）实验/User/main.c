/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       USB读卡器（Slave）实验
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
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "usbd_core.h"
#include "usbd_msc.h"
#include "usbd_desc.h"
#include "usbd_storage.h"

USBD_HandleTypeDef USBD_Device;             /* USB Device处理结构体 */
extern volatile uint8_t g_usb_state_reg;    /* USB状态 */
extern volatile uint8_t g_device_state;     /* USB连接 情况 */

int main(void)
{
    uint16_t norflash_id;
    uint8_t offline_cnt = 0;
    uint8_t t = 0;
    uint8_t usb_sta;
    uint8_t device_sta;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    norflash_init();                    /* 初始化NOR Flash */
    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMCCM);               /* 初始化CCM内存池 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "USB MSC TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    /* 检测SD卡 */
    if (sd_init() != 0)
    {
       
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);
    }
    else
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Size:     MB", RED);
        lcd_show_num(134, 110, (uint32_t)(((uint64_t)g_sd_card_info.LogBlockNbr * g_sd_card_info.LogBlockSize) >> 20), 5, 16, RED);
    }
    
    /* 检测NOR Flash */
    norflash_id = norflash_read_id();
    if ((norflash_id == 0x0000) || (norflash_id == 0xFFFF))
    {
        lcd_show_string(30, 130, 200, 16, 16, "NOR Flash Error!", RED);
    }
    else
    {
        lcd_show_string(30, 130, 200, 16, 16, "NOR Flash Size:12MB", RED);
    }
    
    lcd_show_string(30, 150, 200, 16, 16, "USB Connecting...", RED);    /* 提示正在建立连接 */
    USBD_Init(&USBD_Device, &MSC_Desc, 0);                              /* 初始化USB */
    USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);                   /* 添加类 */
    USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);            /* 为MSC类添加回调函数 */
    USBD_Start(&USBD_Device);                                           /* 开启USB */
    delay_ms(1000);
    
    while (1)
    {
        delay_ms(1);
        
        /* 状态改变了 */
        if (usb_sta != g_usb_state_reg)
        {
            lcd_fill(30, 170, 240, 186, WHITE);
            
            /* 正在写 */
            if (g_usb_state_reg & 0x01)
            {
                LED1(0);
                lcd_show_string(30, 170, 200, 16, 16, "USB Writing...", RED);
            }
            
            /* 正在读 */
            if (g_usb_state_reg & 0x02)   
            {
                LED1(0);
                lcd_show_string(30, 170, 200, 16, 16, "USB Reading...", RED);
            }
            
            /* 写入错误 */
            if (g_usb_state_reg & 0x04)
            {
                lcd_show_string(30, 190, 200, 16, 16, "USB Write Err ", RED);
            }
            else
            {
                lcd_fill(30, 190, 240, 206, WHITE);
            }
            
            /* 读出错误 */
            if (g_usb_state_reg & 0x08)
            {
                lcd_show_string(30, 210, 200, 16, 16, "USB Read  Err ", RED);
            }
            else
            {
                lcd_fill(30, 210, 240, 226, WHITE);
            }
            
            usb_sta = g_usb_state_reg;
        }
        
        /* 连接状态改变 */
        if (device_sta != g_device_state)
        {
            if (g_device_state == 1)
            {
                lcd_show_string(30, 150, 200, 16, 16, "USB Connected    ", RED);
            }
            else
            {
                lcd_show_string(30, 150, 200, 16, 16, "USB DisConnected ", RED);
            }
            
            device_sta = g_device_state;
        }
        
        if (++t == 200)
        {
            t = 0;
            LED1(1);
            LED0_TOGGLE();
            
            if (g_usb_state_reg & 0x10)
            {
                offline_cnt = 0;
                g_device_state = 1;
            }
            else
            {
                offline_cnt++;
                
                if (offline_cnt > 10)
                {
                    g_device_state = 0;
                }
            }
            
            g_usb_state_reg = 0;
        }
    }
}
