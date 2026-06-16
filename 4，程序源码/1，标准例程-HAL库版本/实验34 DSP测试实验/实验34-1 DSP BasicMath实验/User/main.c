/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       DSP BasicMath实验
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
#include "./BSP/TIMER/btim.h"
#include "./CMSIS/DSP/Include/arm_math.h"

/* 定义误差值 */
#define DELTA   0.0001f

uint8_t g_timeout;

extern TIM_HandleTypeDef g_timx_handle;

/**
 * @brief   正弦余弦测试
 * @param   angle: 起始角度
 * @param   times: 运算次数
 * @param   mode: 是否使用DSP库
 * @arg     0: 不使用DSP库
 * @arg     1: 使用DSP库
 * @retval  计算结果
 * @arg     0: 计算无误
 * @arg     1: 计算错误
 */
uint8_t sin_cos_test(float angle, uint32_t times, uint8_t mode)
{
    float sinx;
    float cosx;
    float result;
    uint32_t i = 0;
    
    if (mode == 0)
    {
        /* 不使用DSP库计算正弦余弦 */
        for (i=0; i<times; i++)
        {
            cosx = cosf(angle);
            sinx = sinf(angle);
            result = sinx * sinx + cosx * cosx;
            result = fabsf(result - 1.0f);
            
            if (result > DELTA)
            {
                return 0xFF;
            }
            
            angle += 0.001f;
        }
    }
    else
    {
        /* 使用DSP库计算正弦余弦 */
        for (i=0; i<times; i++)
        {
            cosx = arm_cos_f32(angle);
            sinx = arm_sin_f32(angle);
            result = sinx * sinx + cosx * cosx;
            result = fabsf(result - 1.0f);
            
            if (result > DELTA)
            {
                return 0xFF;
            }
            
            angle += 0.001f;
        }
    }
    
    return 0;
}

int main(void)
{
    float time;
    char buf[50];
    
    HAL_Init();                             /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);     /* 配置时钟，168MHz */
    delay_init(168);                        /* 初始化延时 */
    usart_init(115200);                     /* 初始化串口 */
    led_init();                             /* 初始化LED */
    lcd_init();                             /* 初始化LCD */
    btim_timx_int_init(0xFFFF, 8400 - 1);   /* 初始化基本定时器 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DSP BasicMath TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "No DSP runtime:", RED);
    lcd_show_string(30, 130, 200, 16, 16, "Use DSP runtime:", RED);
    
    while (1)
    {
        /* 不使用DSP库 */
        __HAL_TIM_SET_COUNTER(&g_timx_handle, 0);
        g_timeout = 0;
        if (sin_cos_test(PI / 6, 200000, 0) == 0)
        {
            time = __HAL_TIM_GET_COUNTER(&g_timx_handle) + (uint32_t)g_timeout * 0x10000;
            sprintf(buf, "%0.1fms\r\n", time / 10);
            lcd_show_string(150, 110, 200, 16, 16, buf, BLUE);
        }
        else
        {
            lcd_show_string(150, 110, 200, 16, 16, "Error", BLUE);
        }
        
        /* 使用DSP库 */
        __HAL_TIM_SET_COUNTER(&g_timx_handle, 0);
        g_timeout = 0;
        if (sin_cos_test(PI / 6, 200000, 1) == 0)
        {
            time = __HAL_TIM_GET_COUNTER(&g_timx_handle) + (uint32_t)g_timeout * 0x10000;
            sprintf(buf, "%0.1fms\r\n", time / 10);
            lcd_show_string(158, 130, 200, 16, 16, buf, BLUE);
        }
        else
        {
            lcd_show_string(158, 130, 200, 16, 16, "Error", BLUE);
        }
        
        LED0_TOGGLE();
    }
}
