/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       高级定时器PWM输入模式实验
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
#include "./BSP/TIMER/atim.h"

extern uint8_t g_timx_chy_pwmin_sta;
extern uint32_t g_timx_chy_pwmin_hval;
extern uint32_t g_timx_chy_pwmin_cval;

int main(void)
{
    uint8_t t = 0;
    
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);         /* 配置时钟，168MHz */
    delay_init(168);                            /* 初始化延时 */
    usart_init(115200);                         /* 初始化串口 */
    led_init();                                 /* 初始化LED */
    gtim_timx_pwm_chy_init(100 - 1, 84 - 1);    /* 初始化通用定时器PWM，PWM频率为10KHz */
    atim_timx_pwmin_chy_init(168 - 1);          /* 初始化高级定时器PWM输入 */
    
    while (1)
    {
        /* 捕获成功 */
        if (g_timx_chy_pwmin_sta == 1)
        {
            g_timx_chy_pwmin_sta = 0;
            
            printf("高电平时间：%d us\r\n", g_timx_chy_pwmin_hval);
            printf("PWM周期：%d us\r\n", g_timx_chy_pwmin_cval);
            printf("PWM频率：%d Hz\r\n", 1000000 / g_timx_chy_pwmin_cval);
            printf("\r\n");
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
