/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       T9拼音输入法实验
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
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TOUCH/touch.h"
#include "./T9INPUT/pyinput.h"

/* 数字表 */
static const char *kbd_tbl[9] = {"←", "2", "3", "4", "5", "6", "7", "8", "9",};

/* 字符表 */
static const char *kbs_tbl[9] = {"DEL", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz",};

/* 虚拟键盘按键大小 */
static uint16_t kbdxsize;
static uint16_t kbdysize;

/**
 * @brief   加载键盘界面
 * @param   x: 界面起始X坐标
 * @param   y: 界面起始Y坐标
 * @retval  无
 */
void py_load_ui(uint16_t x, uint16_t y)
{
    uint16_t i;
    
    lcd_draw_rectangle(x, y, x + kbdxsize * 3, y + kbdysize * 3, RED);
    lcd_draw_rectangle(x + kbdxsize, y, x + kbdxsize * 2, y + kbdysize * 3, RED);
    lcd_draw_rectangle(x, y + kbdysize, x + kbdxsize * 3, y + kbdysize * 2, RED);
    
    for (i=0; i<9; i++)
    {
        text_show_string_middle(x + (i % 3)*kbdxsize, y + 4 + kbdysize * (i / 3), (char *)kbd_tbl[i], 16, kbdxsize, BLUE);
        text_show_string_middle(x + (i % 3)*kbdxsize, y + kbdysize / 2 + kbdysize * (i / 3), (char *)kbs_tbl[i], 16, kbdxsize, BLUE);
    }
}

/**
 * @brief   设置按键状态
 * @param   x: 键盘X坐标
 * @param   y: 键盘Y坐标
 * @param   key: 键值（0~8）
 * @retval  对应按键状态
 * @arg     0: 松开
 * @arg     1: 按下
 */
void py_key_staset(uint16_t x, uint16_t y, uint8_t keyx, uint8_t sta)
{
    uint16_t i;
    uint16_t j;
    
    i = keyx / 3;
    j = keyx % 3;
    
    if (keyx > 8)
    {
        return;
    }
    
    if (sta)
    {
        lcd_fill(x + j * kbdxsize + 1, y + i * kbdysize + 1, x + j * kbdxsize + kbdxsize - 1, y + i * kbdysize + kbdysize - 1, GREEN);
    }
    else
    {
        lcd_fill(x + j * kbdxsize + 1, y + i * kbdysize + 1, x + j * kbdxsize + kbdxsize - 1, y + i * kbdysize + kbdysize - 1, WHITE);
    }
    
    text_show_string_middle(x + j * kbdxsize, y + 4 + kbdysize * i, (char *)kbd_tbl[keyx], 16, kbdxsize, BLUE);
    text_show_string_middle(x + j * kbdxsize, y + kbdysize / 2 + kbdysize * i, (char *)kbs_tbl[keyx], 16, kbdxsize, BLUE);
}

/**
 * @brief   得到触摸屏的输入
 * @param   x: 键盘X坐标
 * @param   y: 键盘Y坐标
 * @retval  按键键值
 * @arg     0: 无效
 * @arg     1~9: 有效
 */
uint8_t py_get_keynum(uint16_t x, uint16_t y)
{
    uint16_t i;
    uint16_t j;
    static uint8_t key_x = 0;
    uint8_t key = 0;
    
    tp_dev.scan(0);
    if (tp_dev.sta & TP_PRES_DOWN)
    {
        for (i=0; i<3; i++)
        {
            for (j=0; j<3; j++)
            {
                if ((tp_dev.x[0] < (x + j * kbdxsize + kbdxsize)) &&
                    (tp_dev.x[0] > (x + j * kbdxsize)) && 
                    (tp_dev.y[0] < (y + i * kbdysize + kbdysize)) &&
                    (tp_dev.y[0] > (y + i * kbdysize)))
                {
                    key = i * 3 + j + 1;
                    break;
                }
            }
            if (key != 0)
            {
                if (key_x == key)
                {
                    key = 0;
                }
                else
                {
                    py_key_staset(x, y, key_x - 1, 0);
                    key_x = key;
                    py_key_staset(x, y, key_x - 1, 1);
                }
                break;
            }
        }
    }
    else if (key_x != 0)
    {
        py_key_staset(x, y, key_x - 1, 0);
        key_x = 0;
    }
    
    return key;
}

/**
 * @brief   显示结果
 * @param   index: 索引号
 * @arg     0: 表示没有一个匹配的结果，清空之前的显示
 * @arg     其他: 索引号
 * @retval  无
 */
void py_show_result(uint8_t index)
{
    lcd_show_num(30 + 144, 125, index, 1, 16, BLUE);                                                            /* 显示当前的索引 */
    lcd_fill(30 + 40, 125, 30 + 40 + 48, 130 + 16, WHITE);                                                      /* 清除之前的显示 */
    lcd_fill(30 + 40, 145, lcddev.width - 1, 145 + 48, WHITE);                                                  /* 清除之前的显示 */
    
    if (index != 0)
    {
        text_show_string(30 + 40, 125, 200, 16, (char *)t9.pymb[index - 1]->py, 16, 0, BLUE);                   /* 显示拼音 */
        text_show_string(30 + 40, 145, lcddev.width - 70, 48, (char *)t9.pymb[index - 1]->pymb, 16, 0, BLUE);   /* 显示对应的汉字 */
        printf("\r\n拼音:%s\r\n", t9.pymb[index - 1]->py);                                                        /* 串口输出拼音 */
        printf("结果:%s\r\n", t9.pymb[index - 1]->pymb);                                                          /* 串口输出结果 */
    }
}

int main(void)
{
    uint8_t inputstr[7];
    uint8_t inputlen;
    uint8_t result_num;
    uint8_t cur_index;
    uint8_t t = 0;
    uint8_t key;
    uint8_t pykey;
    
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
    
    /* 检查字库 */
    while (fonts_init() != 0)
    {
        lcd_show_string(30, 10, 200, 16, 16, "Font error!   ", RED);
        delay_ms(200);
        lcd_show_string(30, 10, 200, 16, 16, "Please Check! ", RED);
        delay_ms(200);
    }
    
RESTART:
    
    text_show_string(30, 5, 200, 16, "正点原子STM32开发板", 16, 0, RED);
    text_show_string(30, 25, 200, 16, "拼音输入法实验", 16, 0, RED);
    text_show_string(30, 45, 200, 16, "ATOM@ALIENTEK", 16, 0, RED);
    text_show_string(30, 65, 200, 16, "WKUP: 翻页", 16, 0, RED);
    text_show_string(30, 85, 200, 16, "KEY0: 清除", 16, 0, RED);
    text_show_string(30, 105, 200, 16, "输入:        匹配:  ", 16, 0, RED);
    text_show_string(30, 125, 200, 16, "拼音:        当前:  ", 16, 0, RED);
    text_show_string(30, 145, 200, 16, "结果:", 16, 0, RED);
    
    /* 根据LCD分辨率设置按键大小 */
    if (lcddev.id == 0x5310)
    {
        kbdxsize = 86;
        kbdysize = 43;
    }
    else if (lcddev.id == 0x5510)
    {
        kbdxsize = 140;
        kbdysize = 70;
    }
    else
    {
        kbdxsize = 60;
        kbdysize = 40;
    }
    
    py_load_ui(30, 195);
    my_mem_set(inputstr, 0, 7);
    inputlen = 0;
    result_num = 0;
    cur_index = 0;
    
    while (1)
    {
        /* 得到触摸屏的输入 */
        pykey = py_get_keynum(30, 195);
        if (pykey != 0)
        {
            if (pykey == 1)
            {
                /* 删除键 */
                if (inputlen != 0)
                {
                    inputlen--;
                }
                inputstr[inputlen] = '\0';
            }
            else
            {
                /* 添加字符 */
                inputstr[inputlen] = pykey + '0';
                if (inputlen < 7)
                {
                    inputlen++;
                }
            }
            
            /* 有字符，需要进行匹配 */
            if (inputstr[0] != '\0')
            {
                /* 获取匹配结果 */
                pykey = t9.getpymb(inputstr);
                if (pykey != 0)
                {
                    result_num = pykey & 0x7F;
                    cur_index = 1;
                    if ((pykey & 0x80) != 0)
                    {
                        inputlen = pykey & 0x7F;
                        inputstr[inputlen] = '\0';
                        if (inputlen > 1)
                        {
                            result_num = t9.getpymb(inputstr);
                        }
                    }
                }
                else
                {
                    inputlen--;
                    inputstr[inputlen] = '\0';
                }
            }
            else
            {
                cur_index = 0;
                result_num = 0;
            }
            
            /* 显示匹配结果 */
            lcd_fill(30 + 40, 105, 30 + 40 + 48, 110 + 16, WHITE);
            lcd_show_num(30 + 144, 105, result_num, 1, 16, BLUE);
            text_show_string(30 + 40, 105, 200, 16, (char *)inputstr, 16, 0, BLUE);
            py_show_result(cur_index);
        }
        
        /* 有匹配结果 */
        if (result_num != 0)
        {
            key = key_scan(0);
            if (key == WKUP_PRES)
            {
                /* 清除输入 */
                lcd_fill(30 + 40, 145, lcddev.width - 1, 145 + 48, WHITE);
                goto RESTART;
            }
            else if (key == KEY0_PRES)
            {
                /* 匹配结果翻页 */
                if (cur_index < result_num)
                {
                    cur_index++;
                }
                else
                {
                    cur_index = 1;
                }
                py_show_result(cur_index);
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
