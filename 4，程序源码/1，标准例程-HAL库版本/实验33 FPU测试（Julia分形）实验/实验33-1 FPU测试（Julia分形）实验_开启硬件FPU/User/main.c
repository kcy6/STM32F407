/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       FPU测试（Julia分形）实验_开启硬件FPU
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

/* FPU模式提示 */
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
#define FPU_MODE        "FPU ON"
#else
#define FPU_MODE        "FPU OFF"
#endif

#define ITERATION       128     /* 迭代次数 */
#define REAL_CONSTANT   0.285f  /* 实部常量 */
#define IMG_CONSTANT    0.01f   /* 虚部常量 */

/* 颜色表 */
static uint16_t g_color_map[ITERATION];

/* 缩放因子列表 */
static const uint16_t zoom_ratio[] =
{
    120, 110, 100, 150, 200, 275, 350, 450,
    600, 800, 1000, 1200, 1500, 2000, 1500,
    1200, 1000, 800, 600, 450, 350, 275, 200,
    150, 100, 110,
};

uint8_t g_timeout;

extern TIM_HandleTypeDef g_timx_handle;

/**
 * @brief   初始化颜色表
 * @param   clut: 颜色表指针
 * @retval  无
 */
void julia_clut_init(uint16_t *clut)
{
    uint32_t i;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    
    for (i=0; i<ITERATION; i++)
    {
        /* 产生RGB颜色值 */
        red = (i * 8 * 256 / ITERATION) % 256;
        green = (i * 6 * 256 / ITERATION) % 256;
        blue = (i * 4 * 256 / ITERATION) % 256;
        
        /* 将RGB888转换为RGB565 */
        red = red >> 3;
        red = red << 11;
        green = green >> 2;
        green = green << 5;
        blue = blue >> 3;
        clut[i] = red + green + blue;
    }
}

/**
 * @brief   产生Julia分形图形
 * @param   size_x: 屏幕X方向的尺寸
 * @param   size_y: 屏幕Y方向的尺寸
 * @param   offset_x: 屏幕X方向的偏移
 * @param   offset_y: 屏幕Y方向的偏移
 * @param   zoom: 缩放因子
 * @retval  无
 */
void julia_generate_fpu(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
    uint8_t i;
    uint16_t x;
    uint16_t y;
    float tmp1;
    float tmp2;
    float num_real;
    float num_img;
    float radius;
    
    for (y=0; y<size_y; y++)
    {
        for (x=0; x<size_x; x++)
        {
            num_real = y - offset_y;
            num_real = num_real / zoom;
            num_img = x - offset_x;
            num_img = num_img / zoom;
            i = 0;
            radius = 0;
            
            while ((i < ITERATION - 1) && (radius < 4))
            {
                tmp1 = num_real * num_real;
                tmp2 = num_img * num_img;
                num_img = 2 * num_real * num_img + IMG_CONSTANT;
                num_real = tmp1 - tmp2 + REAL_CONSTANT;
                radius = tmp1 + tmp2;
                i++;
            }
            
            LCD->LCD_RAM = g_color_map[i];
        }
    }
}

int main(void)
{
    uint8_t key;
    uint8_t zoom = 0;
    uint8_t autorun = 0;
    float time;
    char buf[50];
    
    HAL_Init();                             /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);     /* 配置时钟，168MHz */
    delay_init(168);                        /* 初始化延时 */
    usart_init(115200);                     /* 初始化串口 */
    led_init();                             /* 初始化LED */
    key_init();                             /* 初始化按键 */
    lcd_init();                             /* 初始化LCD */
    btim_timx_int_init(0xFFFF, 8400 - 1);   /* 初始化基本定时器 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "FPU TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "KEY_UP:+", RED);
    lcd_show_string(30, 130, 200, 16, 16, "KEY0:Auto/Manul", RED);
    
    delay_ms(500);
    
    /* 初始化颜色表 */
    julia_clut_init(g_color_map);
    
    while (1)
    {
        key = key_scan(0);
        if (key == WKUP_PRES)
        {
            if (++zoom > (sizeof(zoom_ratio) / sizeof(uint16_t) - 1))
            {
                zoom = 0;
            }
        }
        else if (key == KEY0_PRES)
        {
            autorun = !autorun;
        }
        
        if (autorun != 0)
        {
            LED1(0);
            if (++zoom > (sizeof(zoom_ratio) / sizeof(uint16_t) - 1))
            {
                zoom = 0;
            }
        }
        else
        {
            LED1(1);
        }
        
        /* 绘图及计时准备工作 */
        lcd_set_window(0, 0, lcddev.width, lcddev.height);
        lcd_write_ram_prepare();
        __HAL_TIM_SET_COUNTER(&g_timx_handle, 0);
        g_timeout = 0;
        
        /* 绘图及计算耗时 */
        julia_generate_fpu(lcddev.width, lcddev.height, lcddev.width / 2, lcddev.height / 2, zoom_ratio[zoom]);
        time = __HAL_TIM_GET_COUNTER(&g_timx_handle) + (uint32_t)g_timeout * 0x10000;
        sprintf(buf, "%s: zoom:%d  runtime:%0.1fms\r\n", FPU_MODE, zoom_ratio[zoom], time / 10);
        lcd_show_string(5, lcddev.height - 5 - 12, lcddev.width - 5, 12, 12, buf, RED);
        printf("%s", buf);
        LED0_TOGGLE();
    }
}
