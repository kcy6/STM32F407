/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       FLASH模拟EEPROM实验
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

static const uint8_t g_text_buf[] = {"STM32 FLASH TEST"};
#define TEXT_SIZE (((sizeof(g_text_buf) >> 2) << 2) + 4)

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    uint8_t data[TEXT_SIZE];
    uint8_t wdata[TEXT_SIZE] = {0};
    uint8_t index;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "FLASH TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "WK_UP:Write KEY0:Read", RED);
    
    for (index=0; index<sizeof(g_text_buf); index++)
    {
        wdata[index] = g_text_buf[index];
    }
    
    while (1)
    {
        key = key_scan(0);
        if (key == WKUP_PRES)
        {
            /* 往Flash写入数据 */
            lcd_fill(0, 130, 239, 319, WHITE); 
            lcd_show_string(30, 130, 200, 16, 16, "Start Write Flash....", BLUE);
            stmflash_write(FLASH_END + 1 - TEXT_SIZE, (uint32_t *)wdata, TEXT_SIZE / sizeof(uint32_t));
            lcd_show_string(30, 130, 200, 16, 16, "Flash Write Finished!", BLUE);
        }
        else if (key == KEY0_PRES)
        {
            /* 从Flash读取数据 */
            lcd_show_string(30, 130, 200, 16, 16, "Start Read Flash.... ", BLUE);
            stmflash_read(FLASH_END + 1 - TEXT_SIZE, (uint32_t *)data, TEXT_SIZE / sizeof(uint32_t));
            lcd_show_string(30, 130, 200, 16, 16, "The Data Readed Is:  ", BLUE);
            lcd_show_string(30, 150, 200, 16, 16, (char *)data, BLUE);
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
