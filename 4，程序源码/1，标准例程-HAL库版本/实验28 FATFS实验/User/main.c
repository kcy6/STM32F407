/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       FATFS实验
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
#include "./BSP/SDIO/sdio_sdcard.h"
#include "./FATFS/exfuns/exfuns.h"

int main(void)
{
    FRESULT res;
    uint32_t total;
    uint32_t free;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMCCM);               /* 初始化CCM内存池 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "FATFS TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "Use USMART for test", RED);
    
    while (sd_init() != 0)
    {
        lcd_show_string(30, 130, 200, 16, 16, "SD Card Error!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        delay_ms(500);
        LED0_TOGGLE();
    }
    
    exfuns_init();
    
    /* 挂载SD卡 */
    f_mount(fs[0], "0:", 1);
    
    /* 挂载NOR Flash，如果NOR Flash没有文件系统则需要进行格式化 */
    res = f_mount(fs[1], "1:", 1);
    if (res == FR_NO_FILESYSTEM)
    {
        lcd_show_string(30, 130, 200, 16, 16, "Flash Disk Formatting...", RED);
        res = f_mkfs("1:", NULL, NULL, FF_MAX_SS);
        if (res == FR_OK)
        {
            f_setlabel((const TCHAR *)"1:ALIENTEK");
            lcd_show_string(30, 130, 200, 16, 16, "Flash Disk Format Finish", RED);
        }
        else
        {
            lcd_show_string(30, 130, 200, 16, 16, "Flash Disk Format Error ", RED);
        }
        delay_ms(1000);
    }
    lcd_fill(30, 130, 239, 145, WHITE);
    
    /* 获取SD卡容量 */
    while (exfuns_get_free("0", &total, &free) != 0)
    {
        lcd_show_string(30, 130, 200, 16, 16, "SD Card FatFs Error!", RED);
        delay_ms(200);
        lcd_fill(30, 130, 240, 150 + 16, WHITE);
        delay_ms(200);
        LED0_TOGGLE();
    }
    
    lcd_show_string(30, 130, 200, 16, 16, "FATFS OK!", BLUE);
    lcd_show_string(30, 150, 200, 16, 16, "SD Total Size:     MB", BLUE);
    lcd_show_string(30, 170, 200, 16, 16, "SD Free Size:     MB", BLUE);
    lcd_show_num(142, 150, total >> 10, 5, 16, BLUE);
    lcd_show_num(134, 170, free >> 10, 5, 16, BLUE);
    
    while (1)
    {
        delay_ms(200);
        LED0_TOGGLE();
    }
}
