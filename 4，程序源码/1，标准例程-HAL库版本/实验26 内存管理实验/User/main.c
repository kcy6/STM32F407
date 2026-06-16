/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       内存管理实验
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
#include "./BSP/SRAM/sram.h"
#include "./MALLOC/malloc.h"
#include <stdio.h>

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    uint8_t *p_sramin = NULL;
    uint8_t *p_sramccm = NULL;
    uint8_t *p_sramex = NULL;
    uint32_t tp_sramin = 0;
    uint32_t tp_sramccm = 0;
    uint32_t tp_sramex = 0;
    uint8_t paddr[32];
    uint16_t memused;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    sram_init();                        /* 初始化外部SRAM */
    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMCCM);               /* 初始化CCM内存池 */
    my_mem_init(SRAMEX);                /* 初始化外部SRAM内存池 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "MALLOC TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Malloc & WR & Show", RED);
    lcd_show_string(30, 130, 200, 16, 16, "KEY_UP:Free", RED);
    
    lcd_show_string(30, 162, 200, 16, 16, "SRAMIN USED:", BLUE);
    lcd_show_string(30, 178, 200, 16, 16, "SRAMCCM USED:", BLUE);
    lcd_show_string(30, 194, 200, 16, 16, "SRAMEX USED:", BLUE);
    
    while (1)
    {
        key = key_scan(0);
        if (key == WKUP_PRES)
        {
            /* 申请内存 */
            p_sramin = mymalloc(SRAMIN, 2048);
            p_sramccm = mymalloc(SRAMCCM, 2048);
            p_sramex = mymalloc(SRAMEX, 2048);
            
            /* 判断内存申请是否成功 */
            if ((p_sramin != NULL) && (p_sramccm != NULL) && (p_sramex != NULL))
            {
                /* 使用申请到的内存 */
                sprintf((char *)p_sramin, "SRAMIN: Malloc Test%03d", t + SRAMIN);
                lcd_show_string(30, 258, 239, 16, 16, (char *)p_sramin, BLUE);
                sprintf((char *)p_sramccm, "SRAMCCM: Malloc Test%03d", t + SRAMCCM);
                lcd_show_string(30, 274, 239, 16, 16, (char *)p_sramccm, BLUE);
                sprintf((char *)p_sramex, "SRAMEX: Malloc Test%03d", t + SRAMEX);
                lcd_show_string(30, 290, 239, 16, 16, (char *)p_sramex, BLUE);
            }
            else
            {
                myfree(SRAMIN, p_sramin);
                myfree(SRAMCCM, p_sramccm);
                myfree(SRAMEX, p_sramex);
                p_sramin = NULL;
                p_sramccm = NULL;
                p_sramex = NULL;
            }
        }
        else if (key == KEY0_PRES)
        {
            /* 释放内存 */
            myfree(SRAMIN, p_sramin);
            myfree(SRAMCCM, p_sramccm);
            myfree(SRAMEX, p_sramex);
            p_sramin = NULL;
            p_sramccm = NULL;
            p_sramex = NULL;
        }
        
        /* 显示申请到内存的首地址 */
        if ((tp_sramin != (uint32_t)p_sramin) || (tp_sramccm != (uint32_t)p_sramccm) || (tp_sramex != (uint32_t)p_sramex))
        {
            tp_sramin = (uint32_t)p_sramin;
            tp_sramccm = (uint32_t)p_sramccm;
            tp_sramex = (uint32_t)p_sramex;
            
            sprintf((char *)paddr, "SRAMIN: Addr: 0x%08X", (uint32_t)p_sramin);
            lcd_show_string(30, 210, 239, 16, 16, (char *)paddr, BLUE);
            sprintf((char *)paddr, "SRAMCCN: Addr: 0x%08X", (uint32_t)p_sramccm);
            lcd_show_string(30, 226, 239, 16, 16, (char *)paddr, BLUE);
            sprintf((char *)paddr, "SRAMEX: Addr: 0x%08X", (uint32_t)p_sramex);
            lcd_show_string(30, 242, 239, 16, 16, (char *)paddr, BLUE);
        }
        else if ((p_sramin == NULL) || (p_sramccm == NULL) || (p_sramex == NULL))
        {
            lcd_fill(30, 210, 239, 319, WHITE);
        }
        
        if (++t == 20)
        {
            t = 0;
            
            /* 显示内部SRAM使用率 */
            memused = my_mem_perused(SRAMIN);
            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
            lcd_show_string(30 + 96, 162, 200, 16, 16, (char *)paddr, BLUE);
            
            /* 显示CCM使用率 */
            memused = my_mem_perused(SRAMCCM);
            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
            lcd_show_string(30 + 104, 178, 200, 16, 16, (char *)paddr, BLUE);
            
            /* 显示外部SRAM使用率 */
            memused = my_mem_perused(SRAMEX);
            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
            lcd_show_string(30 + 96, 194, 200, 16, 16, (char *)paddr, BLUE);
            
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
