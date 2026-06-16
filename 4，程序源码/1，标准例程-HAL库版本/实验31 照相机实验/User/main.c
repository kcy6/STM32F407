/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       照相机实验
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
#include "./PICTURE/piclib.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "./BSP/DCMI/dcmi.h"
#include "./BSP/OV2640/ov2640.h"

/* OV2640输出数据格式
 * 0: RGB565格式
 * 1: JPEG格式
 */
static uint8_t g_ov_mode;

/* JPEG数据采集完成标志
 * 0: 采集未完成
 * 1: 采集完成，但还未处理
 * 2: 处理完成，可以开始接收下一帧
 */
volatile uint8_t g_jpeg_data_ok = 0;

/**
 * @bref        处理JPEG数据
 * @note        在DCMI捕获中断中调用
 * @param       无
 * @retval      无
 */
void jpeg_data_process(void)
{
    if (g_ov_mode == 0)             /* RGB565模式 */
    {
        lcd_set_cursor(0, 0);       /* 设置光标位置 */
        lcd_write_ram_prepare();    /* 准备写GRAM */
    }
}

/**
 * @bref        切换为OV2640模式
 * @note        切换PC8/PC9/PC11为DCMI复用功能（AF13）
 * @param       无
 * @retval      无
 */
static void sw_ov2640_mode(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    OV2640_PWDN(0);
    gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);
}

/**
 * @bref        切换为SD卡模式
 * @note        切换PC8/PC9/PC11为SDIO复用功能（AF12）
 * @param       无
 * @retval      无
 */
static void sw_sdcard_mode(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    OV2640_PWDN(1);
    gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);
}

/**
 * @brief       文件名自增（避免覆盖）
 * @note        bmp组合成形如“0:PHOTO/PIC13141.bmp”的文件名
 *              jpg组合成形如“0:PHOTO/PIC13141.jpg”的文件名
 * @param       pname: 有效的文件名
 * @param       mode : 文件格式
 *   @arg       0: BMP
 *   @arg       1: JPG
 * @retval      无
 */
static void camera_new_pathname(uint8_t *pname, uint8_t mode)
{
    uint8_t res;
    uint16_t index = 0;
    FIL *ftemp;
    
    ftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));
    if (ftemp == NULL)
    {
        return;
    }
    
    while (index < 0xFFFF)
    {
        if (mode == 0)                                                  /* 创建.bmp文件名 */
        {
            sprintf((char *)pname, "0:PHOTO/PIC%05d.bmp", index);
        }
        else                                                            /* 创建.jpg文件名 */
        {
            sprintf((char *)pname, "0:PHOTO/PIC%05d.jpg", index);
        }
        
        res = (uint8_t)f_open(ftemp, (const TCHAR *)pname, FA_READ);    /* 尝试打开这个文件 */
        if (res == (uint8_t)FR_NO_FILE)                                 /* 该文件名不存在，正是所需的文件名 */
        {
            break;
        }
        
        index++;
    }
    
    myfree(SRAMIN, ftemp);
}

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    uint8_t res;
    uint8_t sd_ok = 1;
    uint8_t *pname;
    
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);         /* 配置时钟，168MHz */
    delay_init(168);                            /* 初始化延时 */
    usart_init(115200);                         /* 初始化串口 */
    led_init();                                 /* 初始化LED */
    key_init();                                 /* 初始化按键 */
    lcd_init();                                 /* 初始化LCD */
    piclib_init();                              /* 初始化画图 */
    my_mem_init(SRAMIN);                        /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMCCM);                       /* 初始化CCM内存池 */
    exfuns_init();                              /* 为exfuns申请内存 */
    f_mount(fs[0], "0:", 1);                    /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);                    /* 挂载NOR Flash */
    
    while (fonts_init() != 0)                                                               /* 检查字库 */
    {
        lcd_show_string(30, 90, 200, 16, 16, "Font error!", RED);
        delay_ms(200);
        lcd_fill(20, 90, 200 + 20, 90 + 16, WHITE);
        delay_ms(200);
    }
    
    while (ov2640_init() != 0)                                                              /* 初始化OV2640 */
    {
        text_show_string(30, 170, 240, 16, "OV2640 错误!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 150, 239, 206, WHITE);
        delay_ms(200);
    }
    
    sw_sdcard_mode();
    while (sd_init() != 0)                                                                  /* 初始化SD卡 */
    {
        lcd_show_string(30, 50, 200, 16, 16, "SD Card Failed!", RED);
        delay_ms(200);
        lcd_fill(30, 50, 200 + 30, 50 + 16, WHITE);
        delay_ms(200);
    }
    
    text_show_string(30, 50, 200, 16, "正点原子STM32开发板", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "照相机实验", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "WKUP:拍照(bmp格式)", 16, 0, RED);
    
    res = (uint8_t)f_mkdir("0:/PHOTO");                                                     /* 创建PHOTO文件夹 */
    if ((res != (uint8_t)FR_EXIST) && (res != 0))                                           /* PHOTO文件夹不存在且创建失败 */
    {
        text_show_string(30, 150, 240, 16, "SD卡错误!", 16, 0, RED);
        text_show_string(30, 150, 240, 16, "拍照功能将不可用!", 16, 0, RED);
        sd_ok = 0;
    }
    
    pname = (uint8_t *)mymalloc(SRAMIN, 30);                                                /* 为带路径的文件名申请内存 */
    while (pname == NULL)
    {
        text_show_string(30, 150, 240, 16, "内存分配失败!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 150, 240, 146, WHITE);
        delay_ms(200);
    }
    
    sw_ov2640_mode();
    ov2640_rgb565_mode();                                                                   /* 配置OV2640为RGB565模式 */
    dcmi_init();                                                                            /* 初始化DCMI */
    dcmi_dma_init((uint32_t)&(LCD->LCD_RAM),                                                /* 配置DCMI DMA */
                 0,
                 1,
                 DMA_MDATAALIGN_HALFWORD,
                 DMA_MINC_DISABLE);
    ov2640_outsize_set(lcddev.width, lcddev.height);                                        /* 全屏缩放 */
    dcmi_start();                                                                           /* 启动DCMI传输 */
    ov2640_flash_intctrl();                                                                 /* 内部控制闪光灯 */
    
    while (1)
    {
        t++;
        key = key_scan(0);
        
        if (key != 0)                                                                       /* 有按键按下 */
        {
            dcmi_stop();                                                                    /* 先禁止DCMI传输 */
            if (sd_ok == 1)                                                                 /* SD卡正常 */
            {
                sw_sdcard_mode();                                                           /* 切换为SD卡模式 */
                switch (key)
                {
                    case WKUP_PRES:
                    {
                        camera_new_pathname(pname, 0);                                      /* 得到BMP格式文件名 */
                        res = bmp_encode(pname, 0, 0, lcddev.width, lcddev.height, 0);      /* 编码并保存BMP文件 */
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                sw_ov2640_mode();                                                           /* 切换为OV2640模式 */
                if (res != 0)
                {
                    text_show_string(30, 130, 240, 16, "写入文件错误!", 16, 0, RED);
                }
                else
                {
                    text_show_string(10, 130, 240, 16, "拍照成功!", 16, 0, RED);
                    text_show_string(10, 150, 240, 16, "保存为:", 16, 0, RED);
                    text_show_string(10 + 56, 150, 240, 16, (char*)pname, 16, 0, RED);
                }
                
                delay_ms(1000);
            }
            else                                                                            /* SD卡不可用 */
            {
                text_show_string(30, 130, 240, 16, "SD卡错误!", 16, 0, RED);
                text_show_string(30, 150, 240, 16, "拍照功能不可用!", 16, 0, RED);
            }
            delay_ms(2000);
            dcmi_start();                                                                   /* 恢复DCMI传输 */
        }
        
        if (t == 20)
        {
            LED0_TOGGLE();
            t = 0;
        }
        
        delay_ms(10);
    }
}
