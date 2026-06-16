/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       手写识别实验
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
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./TEXT/text.h"
#include "./ATKNCR/atk_ncr.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TOUCH/touch.h"

/* 手写轨迹记录 */
static atk_ncr_point ncr_input_buf[200];

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
    int incx;
    int incy;
    int row;
    int col;
    
    if ((x1 < size) || (x2 < size) || (y1 < size) || (y2 < size))
    {
        return;
    }
    
    /* 计算坐标增量 */
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;
    
    /* 设置水平单步方向 */
    if (delta_x > 0)
    {
        
        incx = 1;
    }
    else if (delta_x == 0)
    {
        incx = 0;
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
    /* 设置垂直单步方向 */
    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    /* 选取基本增量坐标轴 */
    if (delta_x > delta_y)
    {
        distance = delta_x;
    }
    else
    {
        distance = delta_y;
    }
    
    /* 画线 */
    for (t=0; t<=(distance + 1); t++)
    {
        lcd_fill_circle(row, col, size, color);
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

int main(void)
{
    uint32_t t = 0;
    uint8_t tcnt;
    uint8_t key;
    uint8_t mode = 4;
    uint16_t lastpos[2];
    uint16_t pcnt = 0;
    char sbuf[10];
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    tp_dev.init();                      /* 初始化触摸屏 */
    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMCCM);               /* 初始化CCM内存池 */
    exfuns_init();                      /* 为exfuns申请内存 */
    f_mount(fs[0], "0:", 1);            /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);            /* 挂载NOR Flash */
    alientek_ncr_init();                /* 初始化手写识别 */
    
    /* 检查字库 */
    while (fonts_init() != 0)
    {
        lcd_show_string(30, 10, 200, 16, 16, "Font error!   ", RED);
        delay_ms(200);
        lcd_show_string(30, 10, 200, 16, 16, "Please Check! ", RED);
        delay_ms(200);
    }
    
RESTART:
    
    text_show_string(30, 10, 200, 16, "正点原子STM32开发板", 16, 0, RED);
    text_show_string(30, 30, 200, 16, "手写识别实验", 16, 0, RED);
    text_show_string(30, 50, 200, 16, "ATOM@ALIENTEK", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "WKUP: Adjust KEY0: Mode", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "识别结果:", 16, 0, RED);
    
    lcd_draw_rectangle(19, 114, lcddev.width - 20, lcddev.height - 5, RED);
    text_show_string(96, 207, 200, 16, "手写区", 16, 0, BLUE);
    tcnt = 100;
    
    while (1)
    {
        key = key_scan(0);
        if (key == WKUP_PRES)
        {
            /* 电容屏触摸校准 */
            if ((tp_dev.touchtype & 0x80) == 0)
            {
                tp_adjust();
                goto RESTART;
            }
        }
        else if (key == KEY0_PRES)
        {
            /* 切换识别模式 */
            lcd_fill(20, 115, 219, 315, WHITE);
            tcnt = 100;
            if (++mode > 4)
            {
                mode = 1;
            }
            
            if (mode == 1)
            {
                text_show_string(80, 207, 200, 16, "仅识别数字", 16, 0, BLUE);
            }
            else if (mode == 2)
            {
                text_show_string(64, 207, 200, 16, "仅识别大写字母", 16, 0, BLUE);
            }
            else if (mode == 3)
            {
                text_show_string(64, 207, 200, 16, "仅识别小写字母", 16, 0, BLUE);
            }
            else if (mode == 4)
            {
                text_show_string(88, 207, 200, 16, "全部识别", 16, 0, BLUE);
            }
        }
        
        tp_dev.scan(0);
        if (tp_dev.sta & TP_PRES_DOWN)
        {
            /* 有触摸，显示并记录触摸轨迹 */
            tcnt = 0;
            
            if (((tp_dev.x[0] < (lcddev.width - 20 - 2)) && (tp_dev.x[0] >= (20 + 2))) &&
                ((tp_dev.y[0] < (lcddev.height - 5 - 2)) && (tp_dev.y[0] >= (115 + 2))))
            {
                if (lastpos[0] == 0xFFFF)
                {
                    lastpos[0] = tp_dev.x[0];
                    lastpos[1] = tp_dev.y[0];
                }
                
                lcd_draw_bline(lastpos[0], lastpos[1], tp_dev.x[0], tp_dev.y[0], 2, BLUE);
                lastpos[0] = tp_dev.x[0];
                lastpos[1] = tp_dev.y[0];
                if (pcnt < 200)
                {
                    if (pcnt != 0)
                    {
                        if ((ncr_input_buf[pcnt - 1].y != tp_dev.y[0]) &&
                            (ncr_input_buf[pcnt - 1].x != tp_dev.x[0]))
                        {
                            ncr_input_buf[pcnt].x = tp_dev.x[0];
                            ncr_input_buf[pcnt].y = tp_dev.y[0];
                            pcnt++;
                        }
                    }
                    else
                    {
                        ncr_input_buf[pcnt].x = tp_dev.x[0];
                        ncr_input_buf[pcnt].y = tp_dev.y[0];
                        pcnt++;
                    }
                }
            }
        }
        else
        {
            /* 触摸松开，进行手写识别 */
            lastpos[0] = 0xFFFF;
            tcnt++;
            if (tcnt == 40)
            {
                if (pcnt != 0)
                {
                    printf("总点数:%d\r\n", pcnt);
                    alientek_ncr(ncr_input_buf, pcnt, 6, mode, sbuf);
                    printf("识别结果:%s\r\n", sbuf);
                    pcnt = 0; 
                    lcd_show_string(60 + 72, 90, 200, 16, 16, sbuf, BLUE);
                }
                lcd_fill(20, 115, lcddev.width - 20 - 1, lcddev.height - 5 - 1, WHITE);
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
