/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       DSP FFT实验
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
#include "./BSP/TIMER/btim.h"
#include "./CMSIS/DSP/Include/arm_math.h"

/* 定义FFT数据量 */
#define FFT_LENGTH  1024

float fft_inputbuf[FFT_LENGTH * 2];
float fft_outputbuf[FFT_LENGTH];

uint8_t g_timeout;

extern TIM_HandleTypeDef g_timx_handle;

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    float time;
    char buf[50];
    arm_cfft_radix4_instance_f32 s;
    uint16_t i;
    
    HAL_Init();                             /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);     /* 配置时钟，168MHz */
    delay_init(168);                        /* 初始化延时 */
    usart_init(115200);                     /* 初始化串口 */
    led_init();                             /* 初始化LED */
    key_init();                             /* 初始化按键 */
    lcd_init();                             /* 初始化LCD */
    btim_timx_int_init(0xFFFF, 8400 - 1);   /* 初始化基本定时器 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DSP FFT TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "WKUP:Run FFT", RED);
    lcd_show_string(30, 130, 200, 16, 16, "FFT runtime:", RED);
    
    /* FFT计算初始化 */
    arm_cfft_radix4_init_f32(&s, FFT_LENGTH, 0, 1);
    
    while (1)
    {
        key = key_scan(0);
        if (key == WKUP_PRES)
        {
            /* 生成输入数据 */
            for (i=0; i<FFT_LENGTH; i++)
            {
                fft_inputbuf[2 * i] = 100 +
                                      10 * arm_sin_f32(2 * PI * i / FFT_LENGTH) +
                                      30 * arm_sin_f32(2 * PI * i * 4 / FFT_LENGTH) +
                                      50 * arm_cos_f32(2 * PI * i * 8 / FFT_LENGTH);
                fft_inputbuf[2 * i + 1] = 0;
            }
            
            /* FFT计算并计算耗时 */
            __HAL_TIM_SET_COUNTER(&g_timx_handle, 0);
            g_timeout = 0;
            arm_cfft_radix4_f32(&s, fft_inputbuf);
            time = __HAL_TIM_GET_COUNTER(&g_timx_handle) + (uint32_t)g_timeout * 0x10000;
            sprintf(buf, "%0.1fms\r\n", time / 10);
            lcd_show_string(126, 130, 200, 16, 16, buf, BLUE);
            
            /* 对FFT计算结果取模并通过串口输出 */
            arm_cmplx_mag_f32(fft_inputbuf, fft_outputbuf, FFT_LENGTH);
            printf("\r\n%d point FFT runtime:%0.1fms\r\n", FFT_LENGTH, time / 10);
            printf("FFT Result:\r\n");
            for (i=0; i<FFT_LENGTH; i++)
            {
                printf("fft_outputbuf[%d]:%f\r\n", i, fft_outputbuf[i]);
            }
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
