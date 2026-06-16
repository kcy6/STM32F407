/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       通用定时器PWM输出实验
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
#include "./BSP/TIMER/gtim.h"

/* 通用定时器句柄 */
extern TIM_HandleTypeDef g_timx_pwm_handle;

int main(void)
{
    uint16_t compare = 0;
    uint8_t counter = 0;
    
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);         /* 配置时钟，168MHz */
    delay_init(168);                            /* 初始化延时 */
    usart_init(115200);                         /* 初始化串口 */
    led_init();                                 /* 初始化LED */
    gtim_timx_pwm_chy_init(500 - 1, 84 - 1);    /* 初始化通用定时器PWM，PWM频率为2KHz */
    
    while (1)
    {
        if (compare++ >= 300)
        {
            compare = 100;
        }
        __HAL_TIM_SET_COMPARE(&g_timx_pwm_handle, GTIM_TIMX_PWM_CHY, compare);
        
        if (counter++ == 20)
        {
            counter = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
