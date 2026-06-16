/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       通用定时器脉冲计数实验
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
#include "./BSP/TIMER/gtim.h"

int main(void)
{
    uint8_t key;
    uint32_t count;
    uint32_t count_prev = 0;
    uint8_t t = 0;
    
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);         /* 配置时钟，168MHz */
    delay_init(168);                            /* 初始化延时 */
    usart_init(115200);                         /* 初始化串口 */
    led_init();                                 /* 初始化LED */
    key_init();                                 /* 初始化按键 */
    gtim_timx_cnt_chy_init(0);                  /* 初始化通用定时器脉冲计数 */
    gtim_timx_cnt_chy_restart();                /* 重启通用定时器脉冲计数 */
    
    while (1)
    {
        key = key_scan(0);
        if (key == KEY0_PRES)
        {
            gtim_timx_cnt_chy_restart();        /* 重启通用定时器脉冲计数 */
        }
        
        count = gtim_timx_cnt_chy_get_count();  /* 获取通用定时器脉冲计数值 */
        if (count_prev != count)                /* 脉冲计数值有更新 */
        {
            count_prev = count;
            printf("Cnt: %d\r\n", count);
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
