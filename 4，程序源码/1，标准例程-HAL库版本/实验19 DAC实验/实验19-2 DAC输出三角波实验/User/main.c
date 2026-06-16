/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       DAC输出三角波实验
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
#include "./BSP/DAC/dac.h"

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    dac_init();                         /* 初始化DAC */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DAC Triangular Wave TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "WK_UP:Wave1 KEY0:Wave2", RED);
    
    while (1)
    {
        key = key_scan(0);
        if (key == WKUP_PRES)
        {
            /* DAC输出三角波1，幅值3.3V，频率100Hz，采样点2000 */
            lcd_show_string(30, 130, 200, 16, 16, "DAC Wave1", BLUE);
            dac_triangular_wave(4095, 5, 2000, 100);
            lcd_show_string(30, 130, 200, 16, 16, "DAC None", BLUE);
        }
        else if (key == KEY0_PRES)
        {
            /* DAC输出三角波2，幅值3.3V，频率100Hz，采样点20 */
            lcd_show_string(30, 130, 200, 16, 16, "DAC Wave2", BLUE);
            dac_triangular_wave(4095, 500, 20, 100);
            lcd_show_string(30, 130, 200, 16, 16, "DAC None", BLUE);
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
