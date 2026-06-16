/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       触摸屏实验
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
#include "./BSP/TOUCH/touch.h"

/**
 * @brief   清空屏幕并在右上角显示"RST"
 * @param   无
 * @retval  无
 */
static void load_draw_dialog(void)
{
    lcd_clear(WHITE);                                                /* 清屏 */
    lcd_show_string(lcddev.width - 24, 0, 200, 16, 16, "RST", BLUE); /* 显示清屏区域 */
}

/**
 * @brief   画粗线
 * @param   x1: 起点X坐标
 * @param   y1: 起点Y坐标
 * @param   x2: 终点X坐标
 * @param   y2: 终点Y坐标
 * @param   size: 线条粗细程度
 * @param   color: 线的颜色
 * @retval  无
 */
static void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0;
    int yerr = 0;
    int delta_x;
    int delta_y;
    int distance;
    int incx, incy, row, col;
    
    if ((x1 < size) || (x2 < size) || (y1 < size) || (y2 < size))
    {
        return;
    }
    
    delta_x = x2 - x1;                          /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;
    if (delta_x > 0)
    {
        incx = 1;                               /* 设置单步方向 */
    }
    else if (delta_x == 0)
    {
        incx = 0;                               /* 垂直线 */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;                               /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    if (delta_x > delta_y)
    {
        distance = delta_x;                     /* 选取基本增量坐标轴 */
    }
    else
    {
        distance = delta_y;
    }
    
    for (t=0; t<=(distance + 1); t++)           /* 画线输出 */
    {
        lcd_fill_circle(row, col, size, color); /* 画点 */
        xerr += delta_x;
        yerr += delta_y;
        
        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }
        
        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief   电阻触摸屏测试
 * @param   无
 * @retval  无
 */
void rtp_test(void)
{
    uint8_t key;
    uint8_t i = 0;
    
    while (1)
    {
        key = key_scan(0);
        tp_dev.scan(0);
        
        if (tp_dev.sta & TP_PRES_DOWN)                                          /* 触摸屏被按下 */
        {
            if ((tp_dev.x[0] < lcddev.width) && (tp_dev.y[0] < lcddev.height))
            {
                if ((tp_dev.x[0] > (lcddev.width - 24)) && (tp_dev.y[0] < 16))
                {
                    load_draw_dialog();                                         /* 清除 */
                }
                else
                {
                    tp_draw_big_point(tp_dev.x[0], tp_dev.y[0], RED);           /* 画点 */
                }
            }
        }
        else
        {
            delay_ms(10);                                                       /* 没有按键按下的时候 */
        }
        
        if (key == KEY0_PRES)                                                   /* KEY0按下，则执行校准程序 */
        {
            tp_adjust();                                                        /* 屏幕校准 */
            tp_save_adjust_data();
            load_draw_dialog();
        }
        
        i++;
        if ((i % 20) == 0)
        {
            LED0_TOGGLE();
        }
    }
}

/* 10个触控点的颜色（电容触摸屏用） */
static const uint16_t POINT_COLOR_TBL[10] = {RED, GREEN, BLUE, BROWN, YELLOW,
                                             MAGENTA, CYAN, LIGHTBLUE, BRRED, GRAY};

/**
 * @brief   电容触摸屏测试
 * @param   无
 * @retval  无
 */
static void ctp_test(void)
{
    uint8_t t = 0;
    uint8_t i = 0;
    uint16_t lastpos[10][2];
    uint8_t maxp = 5;
    
    if (lcddev.id == 0x1018)
    {
        maxp = 10;
    }
    
    while (1)
    {
        tp_dev.scan(0);
        for (t=0; t<maxp; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
                if ((tp_dev.x[t] < lcddev.width) && (tp_dev.y[t] < lcddev.height))                                  /* 坐标在屏幕范围内 */
                {
                    if (lastpos[t][0] == 0xFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }
                    
                    lcd_draw_bline(lastpos[t][0], lastpos[t][1], tp_dev.x[t], tp_dev.y[t], 2, POINT_COLOR_TBL[t]);  /* 画线 */
                    lastpos[t][0] = tp_dev.x[t];
                    lastpos[t][1] = tp_dev.y[t];
                    
                    if (tp_dev.x[t] > (lcddev.width - 24) && tp_dev.y[t] < 20)
                    {
                        load_draw_dialog();                                                                         /* 清除 */
                    }
                }
            }
            else
            {
                lastpos[t][0] = 0xFFFF;
            }
        }
        
        delay_ms(5);
        i++;
        if ((i % 20) == 0)
        {
            LED0_TOGGLE();
        }
    }
}

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    tp_dev.init();                      /* 初始化触摸屏 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "TOUCH TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    /* 电阻屏显示触摸校准提示 */
    if ((tp_dev.touchtype & 0x80) == 0)
    {
        lcd_show_string(30, 110, 200, 16, 16, "Press KEY0 to Adjust", RED);
    }
    delay_ms(1500);
    load_draw_dialog();
    
    if (tp_dev.touchtype & 0x80)
    {
        /* 电容屏测试 */
        ctp_test();
    }
    else
    {
        /* 电阻屏测试 */
        rtp_test();
    }
}
