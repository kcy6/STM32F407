/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       内部温度传感器实验
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
#include "./BSP/LCD/lcd.h"
#include "./BSP/ADC/adc.h"

int main(void)
{
    int16_t temperature;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    adc_temperature_init();             /* 初始化ADC采集内部温度传感器 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "Temperature TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "TEMPERATE: 00.00C", BLUE);
    
    while (1)
    {
        temperature = adc_get_temperature();    /* 获取内部温度传感器结果 */
        
        /* 显示温度 */
        if (temperature < 0)
        {
            temperature = -temperature;
            lcd_show_string(30 + 10 * 8, 110, 16, 16, 16, "-", BLUE);
        }
        else
        {
            lcd_show_string(30 + 10 * 8, 110, 16, 16, 16, " ", BLUE);
        }
        lcd_show_xnum(30 + 11 * 8, 110, temperature / 100, 2, 16, 0, BLUE);
        lcd_show_xnum(30 + 14 * 8, 110, temperature % 100, 2, 16, 0x80, BLUE);
        
        LED0_TOGGLE();
        
        delay_ms(100);
    }
}
