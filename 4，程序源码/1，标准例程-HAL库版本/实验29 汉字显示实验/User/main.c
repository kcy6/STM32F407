/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       汉字显示实验
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
#include "./FATFS/exfuns/exfuns.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "./TEXT/text.h"

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    uint8_t res;
    uint32_t fontcnt;
    uint8_t i;
    uint8_t j;
    uint8_t fontx[2];
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMCCM);               /* 初始化CCM内存池 */
    exfuns_init();                      /* 为exfuns申请内存 */
    f_mount(fs[0], "0:", 1);            /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);            /* 挂载NOR Flash */
    
    /* 检查字库 */
    while (fonts_init() != 0)
    {
UPD:
        lcd_clear(WHITE);
        lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);
        
        /* 初始化SD卡 */
        while (sd_init() != 0)
        {
            lcd_show_string(30, 30, 200, 16, 16, "SD Card Error!", RED);
            delay_ms(500);
            lcd_show_string(30, 30, 200, 16, 16, "Please Check! ", RED);
            delay_ms(500);
            LED0_TOGGLE();
        }
        
        lcd_show_string(30, 50, 200, 16, 16, "SD Card OK", RED);
        lcd_show_string(30, 70, 200, 16, 16, "Font Updating...", RED);
        
        /* 更新字库 */
        res = fonts_update_font(30, 90, 16, (uint8_t *)"0:", RED);
        while (res != 0)
        {
            lcd_show_string(30, 90, 200, 16, 16, "Font Update Failed!", RED);
            delay_ms(200);
            lcd_show_string(30, 90, 200, 16, 16, "Please Check!      ", RED);
            delay_ms(200);
        }
        
        lcd_show_string(30, 90, 200, 16, 16, "Font Update Success!   ", RED);
        delay_ms(1500);
        lcd_clear(WHITE);
    }
    
    text_show_string(30, 30, 200, 16, "正点原子STM32开发板", 16, 0, RED);
    text_show_string(30, 50, 200, 16, "GBK字库测试程序", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "ATOM@ALIENTEK", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "WKUP: 更新字库", 16, 0, RED);
    
    text_show_string(30, 110, 200, 16, "内码高字节:", 16, 0, BLUE);
    text_show_string(30, 130, 200, 16, "内码低字节:", 16, 0, BLUE);
    text_show_string(30, 150, 200, 16, "汉字计数器:", 16, 0, BLUE);
    
    text_show_string(30, 180, 200, 24, "对应汉字为:", 24, 0, BLUE);
    text_show_string(30, 204, 200, 16, "对应汉字(16*16)为:", 16, 0, BLUE);
    text_show_string(30, 220, 200, 12, "对应汉字(12*12)为:", 12, 0, BLUE);
    
    while (1)
    {
        fontcnt = 0;
        
        /* GBK内码高字节范围为0x81~0xFE */
        for (i=0x81; i<0xFF; i++)
        {
            fontx[0] = i;
            lcd_show_num(118, 110, i, 3, 16, BLUE);
            
            /* GBK内码低字节范围为0x40~0x7E、0x80~0xFE) */
            for (j=0x40; j<0xFE; j++)
            {
                if (j == 0x7F)
                {
                    continue;
                }
                
                fontcnt++;
                lcd_show_num(118, 130, j, 3, 16, BLUE);
                lcd_show_num(118, 150, fontcnt, 5, 16, BLUE);
                fontx[1] = j;
                text_show_font(30 + 132, 180, fontx, 24, 0, BLUE);
                text_show_font(30 + 144, 204, fontx, 16, 0, BLUE);
                text_show_font(30 + 108, 220, fontx, 12, 0, BLUE);
                
                t = 200;
                while ((t--) != 0)
                {
                    delay_ms(1);
                    
                    key = key_scan(0);
                    if (key == WKUP_PRES)
                    {
                        /* 更新字库 */
                        goto UPD;
                    }
                }
                
                LED0_TOGGLE();
            }
        }
    }
}
