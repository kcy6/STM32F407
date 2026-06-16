/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       外部SRAM实验
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

/* 定义测试数组
 * 起始地址为SRAM_BASE_ADDR
 */
uint32_t g_test_buffer[250000] __attribute__((at(SRAM_BASE_ADDR)));

/**
 * @brief       测试外部SRAM容量
 * @note        最大支持1MB容量的SRAM
 * @param       x: LCD上显示提示信息的起始X坐标
 * @param       y: LCD上显示提示信息的起始Y坐标
 * @retval      无
 */
static void smc_sram_test(uint16_t x, uint16_t y)
{
    uint32_t i;
    uint8_t temp;
    uint8_t sval;
    
    lcd_show_string(x, y, 239, y + 16, 16, "Ex Memory Test:   0KB", BLUE);
    
    /* 每间隔4KB写入一个数据，总共写入256个数据，刚好为1MB */
    for (temp=0, i=0; i<(1 * 1024 * 1024); i+=4096)
    {
        sram_write(&temp, i, 1);
        temp++;
    }
    
    /* 读出写入的数据进行校验 */
    for (i=0; i<(1 * 1024 * 1024); i+=4096)
    {
        sram_read(&temp, i, 1);
        
        if (i == 0)
        {
            sval = temp;
        }
        else if (temp <= sval)
        {
            break;
        }
        
        /* 显示内存容量 */
        lcd_show_xnum(x + 15 * 8, y, (uint16_t)(temp - sval + 1) * 4, 4, 16, 0, BLUE);
    }
}

int main(void)
{
    uint32_t ts;
    uint8_t t = 0;
    uint8_t key;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    sram_init();                        /* 初始化外部SRAM */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "SRAM TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Test SRAM", RED);
    lcd_show_string(30, 130, 200, 16, 16, "KEY_UP:Test Data", RED);
    
    for (ts=0; ts<250000; ts++)                                             /* 创建测试数据 */
    {
        g_test_buffer[ts] = ts;
    }
    
    while (1)
    {
        t++;
        key = key_scan(0);
        
        if (key == KEY0_PRES)                                               /* 测试外部SRAM容量 */
        {
            smc_sram_test(30, 150);
        }
        else if (key == WKUP_PRES)
        {
            for (ts=0; ts<250000; ts++)
            {
                lcd_show_xnum(30, 170, g_test_buffer[ts], 6, 16, 0, BLUE);  /* 显示测试数据 */
            }
        }
        
        if (t == 20)
        {
            LED0_TOGGLE();
            t = 0;
        }
        
        delay_ms(10);
    }
}
