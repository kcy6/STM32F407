/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       摄像头实验
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
#include "./BSP/DCMI/dcmi.h"
#include "./BSP/OV2640/ov2640.h"
#include "./BSP/USART2/usart2.h"

/* OV2640输出数据格式
 * 0: RGB565格式
 * 1: JPEG格式
 */
static uint8_t g_ov_mode;

/* JPEG相关数据缓存定义 */
#define JPEG_BUF_SIZE   (29 * 1024)                                             /* JPEG数据缓存大小 */
#define JPEG_LINE_SIZE  (1 * 1024)                                              /* JPEG数据DMA双缓存大小 */
static uint32_t g_jpeg_data_buf[JPEG_BUF_SIZE] __attribute__((aligned(4)));     /* JPEG数据缓存区 */
static uint32_t g_dcmi_line_buf[2][JPEG_LINE_SIZE] __attribute__((aligned(4))); /* JPEG数据DMA双缓存区 */
volatile uint32_t g_jpeg_data_len = 0;                                          /* JPEG数据缓存区中有效数据的长度 */

/* JPEG数据采集完成标志
 * 0: 采集未完成
 * 1: 采集完成，但还未处理
 * 2: 处理完成，可以开始接收下一帧
 */
volatile uint8_t g_jpeg_data_ok = 0;

/* JPEG图片的13种尺寸名称 */
static const char *jpeg_size_tbl[13] = {"QQVGA", "QCIF", "QVGA", "WGVGA", "CIF", "VGA", "SVGA", "XGA", "WXGA", "SVGA", "WXGA+", "SXGA", "UXGA"};

/* JPEG图片的13种尺寸参数 */
static const uint16_t jpeg_img_size_tbl[][2] = {
    { 160,  120},   /* QQVGA */
    { 176,  144},   /* QCIF */
    { 320,  240},   /* QVGA */
    { 400,  240},   /* WGVGA */
    { 352,  288},   /* CIF */
    { 640,  480},   /* VGA */
    { 800,  600},   /* SVGA */
    {1024,  768},   /* XGA */
    {1280,  800},   /* WXGA */
    {1280,  960},   /* XVGA */
    {1440,  900},   /* WXGA+ */
    {1280, 1024},   /* SXGA */
    {1600, 1200},   /* UXGA */
};

/**
 * @bref        处理JPEG数据
 * @note        在DCMI捕获中断中调用
 * @param       无
 * @retval      无
 */
void jpeg_data_process(void)
{
    uint16_t rlen;
    uint32_t *pbuf;
    uint16_t i;
    
    if (g_ov_mode != 0)                                                         /* JPEG模式 */
    {
        if (g_jpeg_data_ok == 0)                                                /* 还未采集JPEG数据 */
        {
            __HAL_DMA_DISABLE(&g_dcmi_dma_handle);                              /* 禁用DCMI传出使用的DMA数据流 */
            while (DMA2_Stream1->CR & 0x01);                                    /* 等待DMA可配置 */
            
            rlen = JPEG_LINE_SIZE - __HAL_DMA_GET_COUNTER(&g_dcmi_dma_handle);  /* 计算剩余长度 */
            pbuf = g_jpeg_data_buf + g_jpeg_data_len;                           /* 偏移到JPEG数据缓存中有效数据的末尾，继续添加新的JPEG数据 */
            
            if (DMA2_Stream1->CR & (1 << 19))                                   /* DMA正在处理存储器1，因此从存储器1中读取数据 */
            {
                for (i=0; i<rlen; i++)
                {
                    pbuf[i] = g_dcmi_line_buf[1][i];
                }
            }
            else                                                                /* DMA正在处理存储器0，因此从存储器0中读取数据 */
            {
                for (i=0; i<rlen; i++)
                {
                    pbuf[i] = g_dcmi_line_buf[0][i];
                }
            }
            g_jpeg_data_len += rlen;                                            /* 更新JPEG数据缓存区中有效数据的长度 */
            g_jpeg_data_ok = 1;                                                 /* 标记已采集JPEG数据 */
        }
        else if (g_jpeg_data_ok == 2)                                           /* 上一次采集的JPEG数据已处理完成 */
        {
            __HAL_DMA_SET_COUNTER(&g_dcmi_dma_handle, JPEG_LINE_SIZE);          /* 设置DMA传输的数据项数目 */
            __HAL_DMA_ENABLE(&g_dcmi_dma_handle);                               /* 使能DMA数据流 */
            g_jpeg_data_len = 0;                                                /* 开始新的JPEG数据接收 */
            g_jpeg_data_ok = 0;                                                 /* 标记还未采集JPEG数据 */
        }
    }
    else                                                                /* RGB565模式 */
    {
        lcd_set_cursor(0, 0);                                           /* 设置光标位置 */
        lcd_write_ram_prepare();                                        /* 准备写GRAM */
    }
}

/**
 * @bref        JPEG数据处理回调
 * @note        仅使用双缓冲区模式时使用，在DMA传输完成中断中调用
 * @param       无
 * @retval      无
 */
static void jpeg_dcmi_rx_callback(void)
{
    uint16_t i;
    volatile uint32_t *pbuf;
    
    pbuf = g_jpeg_data_buf + g_jpeg_data_len;           /* 偏移到有效数据末尾 */
    if (DMA2_Stream1->CR & (1 << 19))                   /* DMA正在处理存储器1，这里处理存储器0的数据 */
    {
        for (i=0; i<JPEG_LINE_SIZE; i++)                /* 复制DMA存储器0的数据至JPEG数据缓存区 */
        {
            pbuf[i] = g_dcmi_line_buf[0][i];
        }
        g_jpeg_data_len += JPEG_LINE_SIZE;              /* 更新JPEG数据缓存区中有效数据的长度 */
    }
    else                                                /* DMA正在处理存储器0，这里处理存储器1的数据 */
    {
        for (i=0; i<JPEG_LINE_SIZE; i++)                /* 复制DMA存储器1的数据至JPEG数据缓存区 */
        {
            pbuf[i] = g_dcmi_line_buf[1][i];
        }
        g_jpeg_data_len += JPEG_LINE_SIZE;              /* 更新JPEG数据缓存区中有效数据的长度 */
    }
}

/**
 * @bref        JPEG模式测试
 * @note        获取OV2640输出的JPEG数据，并通过USART2发送至上位机
 * @param       无
 * @retval      无
 */
static void jpeg_test(void)
{
    uint8_t msgbuf[15];                                                             /* 提示信息缓冲区 */
    uint8_t size = 2;                                                               /* 默认使用QVGA（320*240） */
    uint8_t contrast = 2;                                                           /* 默认使用正常对比度 */
    uint8_t *p;
    uint32_t jpeglen;
    uint8_t headok;
    uint32_t i;
    uint32_t jpegstart;
    uint8_t key;
    
    lcd_clear(WHITE);
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "OV2640 JPEG Mode", RED);
    lcd_show_string(30, 100, 200, 16, 16, "KEY0:Contrast", RED);                    /* 对比度 */
    lcd_show_string(30, 120, 200, 16, 16, "WKUP:Size", RED);                        /* 分辨率设置 */
    sprintf((char *)msgbuf, "JPEG Size:%s", jpeg_size_tbl[size]);
    lcd_show_string(30, 140, 200, 16, 16, (char*)msgbuf, RED);                      /* 当前JPEG分辨率 */
    
    ov2640_jpeg_mode();                                                             /* 配置OV2640为JPEG模式 */
    dcmi_init();                                                                    /* 初始化DCMI */
    dcmi_rx_callback = jpeg_dcmi_rx_callback;                                       /* DMA传输完成回调 */
    dcmi_dma_init((uint32_t)g_dcmi_line_buf[0],                                     /* 配置DCMI DMA */
                 (uint32_t)g_dcmi_line_buf[1],
                 JPEG_LINE_SIZE,
                 DMA_MDATAALIGN_WORD,
                 DMA_MINC_ENABLE);
    ov2640_outsize_set(jpeg_img_size_tbl[size][0], jpeg_img_size_tbl[size][1]);     /* 设置图像输出大小 */
    dcmi_start();                                                                   /* 启动DCMI传输 */
    
    while (1)
    {
        if (g_jpeg_data_ok == 1)                                                    /* JPEG数据采集完成 */
        {
            p = (uint8_t *)g_jpeg_data_buf;
            printf("g_jpeg_data_len:%d\r\n", g_jpeg_data_len << 2);                 /* 打印JPEG数据长度 */
            lcd_show_string(30, 170, 210, 16, 16, "Sending JPEG data...", RED);     /* 提示正在传输JPEG数据 */
            jpeglen = 0;
            headok = 0;
            
            for (i=0; i<((g_jpeg_data_len << 2) - 1); i++)                          /* 遍历JPEG数据，查找JPEG数据头（0xFF，0xD8）和JPEG数据尾（0xFF，0xD9） */
            {
                if ((p[i] == 0xFF) && (p[i + 1] == 0xD8))                           /* 找到JPEG数据头 */
                {
                    jpegstart = i;
                    headok = 1;
                }
                
                if (headok != 0)                                                    /* 找到JPEG数据头后，在开始找JPEG数据尾 */
                {
                    if ((p[i] == 0xFF) && (p[i + 1] == 0xD9))                       /* 找到JPEG数据尾 */
                    {
                        jpeglen = i - jpegstart + 2;
                        break;
                    }
                }
            }
            
            if (jpeglen != 0)                                                       /* JPEG数据正常 */
            {
                p += jpegstart;                                                     /* 偏移到JPEG数据头的位置 */
                for (i=0; i<jpeglen; i++)                                           /* 发送整个有效的JPEG数据（JPEG数据头打到JPEG数据尾） */
                {
                    USART2->DR = p[i];
                    while ((USART2->SR & 0X40) == 0);
                    key = key_scan(0);                                              /* 任意按键可打断数据发送 */
                    if (key != 0)
                    {
                        break;
                    }
                }
            }
            
            if (key != 0)
            {
                lcd_show_string(30, 170, 210, 16, 16, "Quit Sending data   ", RED); /* 提示退出JPEG数据传输 */
                switch (key)
                {
                    case KEY0_PRES: /* 设置对比度 */
                    {
                        contrast++;
                        if (contrast > 4)
                        {
                            contrast = 0;
                        }
                        ov2640_contrast(contrast);
                        sprintf((char *)msgbuf, "Contrast:%d", (int8_t)contrast - 2);
                        break;
                    }
                    case WKUP_PRES: /* 设置分辨率 */
                    {
                        size++;
                        if (size > 12)
                        {
                            size = 0;
                        }
                        ov2640_outsize_set(jpeg_img_size_tbl[size][0], jpeg_img_size_tbl[size][1]);
                        sprintf((char *)msgbuf, "JPEG Size:%s", jpeg_size_tbl[size]);
                    }
                    default:
                    {
                        break;
                    }
                }
                lcd_fill(30, 140, 239, 190 + 16, WHITE);
                lcd_show_string(30, 140, 210, 16, 16, (char*)msgbuf, RED);          /* 显示提示内容 */
                delay_ms(800);
            }
            else
            {
                lcd_show_string(30, 170, 210, 16, 16, "Send data complete!!", RED); /* 提示JPEG数据传输结束 */
            }
            
            g_jpeg_data_ok = 2;                                                     /* 标记JPEG数据处理完成，可以开始接收下一帧 */
        }
    }
}

/**
 * @bref        RGB565模式测试
 * @note        获取OV2640输出的RGB数据，并直接在LCD上显示
 * @param       无
 * @retval      无
 */
static void rgb565_test(void)
{
    uint8_t key;
    uint8_t contrast = 2;   /* 默认使用正常对比度 */
    uint8_t scale = 1;      /* 默认全屏缩放 */
    uint8_t msgbuf[15];     /* 提示信息缓冲区 */
    
    lcd_clear(WHITE);
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "OV2640 RGB565 Mode", RED);
    lcd_show_string(30, 100, 200, 16, 16, "KEY0:Contrast", RED);            /* 对比度设置 */
    lcd_show_string(30, 160, 200, 16, 16, "WKUP:FullSize/Scale", RED);      /* 缩放设置 */
    
    ov2640_rgb565_mode();                                                   /* 配置OV2640为RGB565模式 */
    dcmi_init();                                                            /* 初始化DCMI */
    dcmi_dma_init((uint32_t)&(LCD->LCD_RAM),                                /* 配置DCMI DMA */
                 0,
                 1,
                 DMA_MDATAALIGN_HALFWORD,
                 DMA_MINC_DISABLE);
    ov2640_outsize_set(lcddev.width, lcddev.height);                        /* 全屏缩放 */
    dcmi_start();                                                           /* 启动DCMI传输 */
    
    while (1)
    {
        key = key_scan(0);
        
        if (key != 0)
        {
            dcmi_stop();                                                    /* 停止DCMI传输 */
            switch (key)
            {
                case KEY0_PRES:                                             /* 设置对比度 */
                {
                    contrast++;
                    if (contrast > 4)
                    {
                        contrast = 0;
                    }
                    ov2640_contrast(contrast);
                    sprintf((char *)msgbuf, "Contrast:%d", (int8_t)contrast - 2);
                    break;
                }
                case WKUP_PRES:                                             /* 设置缩放 */
                {
                    scale = !scale;
                    if (scale == 0)                                         /* 不缩放 */
                    {
                        ov2640_image_win_set((1600 - lcddev.width) >> 1,
                                             (1200 - lcddev.height) >> 1, 
                                             lcddev.width,
                                             lcddev.height);
                        ov2640_outsize_set(lcddev.width, lcddev.height);
                        sprintf((char *)msgbuf, "Full Size 1:1");
                    }
                    else                                                    /* 全屏缩放 */
                    {
                        ov2640_image_win_set(0, 0, 1600, 1200);
                        ov2640_outsize_set(lcddev.width, lcddev.height);
                        sprintf((char *)msgbuf, "Scale");
                    }
                }
                default:
                {
                    break;
                }
            }
            lcd_show_string(30, 50, 210, 16, 16, (char*)msgbuf, RED);       /* 显示提示内容 */
            delay_ms(800);
            dcmi_start();                                                   /* 重新启动DCMI传输 */
        }
        delay_ms(10);
    }
}

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);         /* 配置时钟，168MHz */
    delay_init(168);                            /* 初始化延时 */
    usart_init(115200);                         /* 初始化串口 */
    led_init();                                 /* 初始化LED */
    key_init();                                 /* 初始化按键 */
    lcd_init();                                 /* 初始化LCD */
    btim_timx_int_init(10000 - 1, 8400 - 1);    /* 初始化基本定时器 */
    usart2_init(921600);                        /* 初始化USART2 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "OV2640 TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    while (ov2640_init() != 0)                                      /* 初始化OV2640 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "OV2640 ERROR ", RED);
        delay_ms(200);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        delay_ms(500);
        LED0_TOGGLE();
    }
    lcd_show_string(30, 130, 200, 16, 16, "OV2640 OK     ", RED);
    ov2640_flash_intctrl();                                         /* 内部控制闪光灯 */
    
    while (1)
    {
        t++;
        key = key_scan(0);
        
        if (key == KEY0_PRES)                                       /* RGB565模式 */
        {
            g_ov_mode = 0;
            break;
        }
        else if (key == WKUP_PRES)                                  /* JPEG模式 */
        {
            g_ov_mode = 1;
            break;
        }
        
        if (t == 100)
        {
            lcd_show_string(30, 150, 230, 16, 16, "KEY0:RGB565  KEY_UP:JPEG", RED);
        }
        else if (t == 200)
        {
            t = 0;
            lcd_fill(30, 150, 230, 150 + 16, WHITE);
            LED0_TOGGLE();
        }
        
        delay_ms(5);
    }
    
    if (g_ov_mode == 0)
    {
        rgb565_test();                                              /* RGB565模式测试 */
    }
    else
    {
        jpeg_test();                                                /* JPEG模式测试 */
    }
}
