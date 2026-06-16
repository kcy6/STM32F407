/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       单通道ADC采集实验
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
    uint16_t adc_result;
    uint16_t voltage;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    adc_init();                         /* 初始化ADC */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "ADC TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "ADC2_CH1_VAL:0", BLUE);
    lcd_show_string(30, 130, 200, 16, 16, "ADC2_CH1_VOL:0.000V", BLUE);
    
    while (1)
    {
        adc_result = adc_get_result_average(ADC_ADCX_CHY, 10);      /* 均值滤波获取ADC结果 */
        lcd_show_xnum(134, 110, adc_result, 5, 16, 0, BLUE);
        
        voltage = (adc_result * 3300) / 4095;                       /* 计算实际电压值（扩大1000倍） */
        lcd_show_xnum(134, 130, voltage / 1000, 1, 16, 0, BLUE);
        lcd_show_xnum(150, 130, voltage % 1000, 3, 16, 0x80, BLUE);
        
        LED0_TOGGLE();
        
        delay_ms(100);
    }
}
