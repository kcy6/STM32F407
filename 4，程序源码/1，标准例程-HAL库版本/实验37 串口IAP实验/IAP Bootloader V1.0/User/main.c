/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       IAP BootLoader V1.0
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
#include "./BSP/STMFLASH/stmflash.h"
#include "./IAP/iap.h"

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    uint32_t lastcount = 0;
    uint32_t applenth = 0;
    uint8_t clearflag = 0;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    
    lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 50, 200, 16, 16, "IAP TEST", RED);
    lcd_show_string(30, 70, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 90, 200, 16, 16, "WKUP: Copy app 2 flash", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0: Run flash app", RED);
    
    while (1)
    {
        if (g_usart_rx_cnt != 0)
        {
            /* 新一次循环若没有接收到新的数据，则认为数据接收完毕 */
            if (lastcount == g_usart_rx_cnt)
            {
                applenth = g_usart_rx_cnt;
                lastcount = 0;
                g_usart_rx_cnt = 0;
                printf("用户程序接收完成!\r\n");
                printf("代码长度:%dBytes\r\n", applenth);
            }
            else
            {
                lastcount = g_usart_rx_cnt;
            }
        }
        
        key = key_scan(0);
        if (key == WKUP_PRES)
        {
            if (applenth != 0)
            {
                printf("开始更新固件...\r\n");
                lcd_show_string(30, 130, 200, 16, 16, "Copying app 2 flash...", BLUE);
                if (((*(volatile uint32_t *)(0x20001000 + 4)) & 0xFF000000) == 0x08000000)
                {
                    iap_write_appbin(FLASH_APP1_ADDR, g_usart_rx_buf, applenth);
                    lcd_show_string(30, 130, 200, 16, 16, "Copy app succedded!   ", BLUE);
                    printf("固件更新完成!\r\n");
                }
                else
                {
                    lcd_show_string(30, 130, 200, 16, 16, "Illegal flash app!    ", BLUE);
                    printf("非Flash应用程序!\r\n");
                }
            }
            else
            {
                printf("没有可以更新的固件!\r\n");
                lcd_show_string(30, 130, 200, 16, 16, "No app!", BLUE);
            }
            clearflag = 10;
        }
        else if (key == KEY0_PRES)
        {
            printf("flash addr :%x \r\n",(*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000);
            if (((*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000)
            {
                printf("开始执行FLASH用户代码!!\r\n\r\n");
                delay_ms(10);
                iap_load_app(FLASH_APP1_ADDR);
            }
            else
            {
                printf("没有可以运行的固件!\r\n");
                lcd_show_string(30, 130, 200, 16, 16, "No APP!", BLUE);
            }
            clearflag = 10;
        }
        
        if (++t == 2)
        {
            t = 0;
            
            if (clearflag != 0)
            {
                clearflag--;
                if (clearflag == 0)
                {
                    lcd_fill(30, 130, 240, 146, WHITE);
                }
            }
            
            LED0_TOGGLE();
        }
        
        delay_ms(100);
    }
}
