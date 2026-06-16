/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       随机数发生器实验
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
#include "./BSP/RNG/rng.h"

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    uint32_t random;
    int32_t random_range;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    rng_init();                         /* 初始化随机数发生器 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "RNG TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 130, 200, 16, 16, "KEY0:Get Random Num", RED);
    lcd_show_string(30, 150, 200, 16, 16, "Random Num:", RED);
    lcd_show_string(30, 170, 200, 16, 16, "Random Num[0-9]:", RED);
    
    while (1)
    {
        key = key_scan(0);
        if (key == KEY0_PRES)
        {
            /* 获取随机数 */
            random = rng_get_random();
            lcd_show_num(30 + 8 * 11, 150, random, 10, 16, BLUE);
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
            
            /* 获取指定范围的随机数 */
            random_range = rng_get_random_range(0, 9);
            lcd_show_num(30 + 8 * 16, 170, random_range, 1, 16, BLUE);
        }
        
        delay_ms(10);
    }
}
