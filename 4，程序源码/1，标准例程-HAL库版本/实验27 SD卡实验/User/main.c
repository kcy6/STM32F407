/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       SD卡实验
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
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/SDIO/sdio_sdcard.h"

/**
 * @brief   显示SD卡信息
 * @param   无
 * @retval  无
 */
static void show_sd_info(void)
{
    HAL_SD_CardCIDTypeDef sd_card_cid = {0};
    
    HAL_SD_GetCardCID(&g_sd_handle, &sd_card_cid);
    
    printf("Card Type: %s\r\n", (g_sd_card_info.CardType == CARD_SDSC) ? ((g_sd_card_info.CardVersion == CARD_V1_X) ? ("SDSC V1") :
                                                                         ((g_sd_card_info.CardVersion == CARD_V1_X) ? ("SDSC V2") :
                                                                         (""))) :
                                ((g_sd_card_info.CardType == CARD_SDHC_SDXC) ? ("SDHC") :
                                ((g_sd_card_info.CardType == CARD_SECURED) ? ("SECURE") :
                                (""))));
    printf("Card ManufacturerID:%d\r\n", sd_card_cid.ManufacturerID);
    printf("Card RCA:%d\r\n", g_sd_card_info.RelCardAdd);
    printf("LogBlockNbr:%d \r\n", g_sd_card_info.LogBlockNbr);
    printf("LogBlockSize:%d \r\n", g_sd_card_info.LogBlockSize);
    printf("Card Capacity:%d MB\r\n", (uint32_t)(((uint64_t)g_sd_card_info.LogBlockNbr * g_sd_card_info.LogBlockSize) >> 20));
    printf("Card BlockSize:%d\r\n\r\n", g_sd_card_info.BlockSize);
    
    lcd_show_string(30, 146, 200, 16, 16, "SD Card Size:     MB", BLUE);
    lcd_show_num(30 + 13 * 8, 146, (uint32_t)(((uint64_t)g_sd_card_info.LogBlockNbr * g_sd_card_info.LogBlockSize) >> 20), 5, 16, BLUE);
}

static void sd_read_test(void)
{
    uint8_t *buf;
    uint16_t index;
    
    buf = (uint8_t *)mymalloc(SRAMIN, g_sd_card_info.BlockSize);
    if (buf == NULL)
    {
        return;
    }
    
    /* 读取并打印SD卡第0个块的数据 */
    if (sd_read_disk(buf, 0, 1) == 0)
    {
        lcd_show_string(30, 170, 200, 16, 16, "USART1 Sending Data...", BLUE);
        printf("Block 0 Data:\r\n");
        for (index=0; index<g_sd_card_info.BlockSize; index++)
        {
            printf("%02X ", buf[index]);
        }
        printf("\r\nData End\r\n");
        lcd_show_string(30, 170, 200, 16, 16, "USART1 Send Data Over!", BLUE);
    }
    else
    {
        printf("SD read Failure!\r\n");
        lcd_show_string(30, 170, 200, 16, 16, "SD read Failure!      ", BLUE);
    }
    
    myfree(SRAMIN, buf);
}

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
    my_mem_init(SRAMCCM);               /* 初始化CCM内存池 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "SD TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 110, 200, 16, 16, "WKUP: Read Block 0", RED);
    
    while (sd_init() != 0)
    {
        lcd_show_string(30, 130, 200, 16, 16, "SD Card Error!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        delay_ms(500);
        LED0_TOGGLE();
    }
    
    lcd_show_string(30, 130, 200, 16, 16, "SD Card OK    ", BLUE);
    
    /* 显示SD卡信息 */
    show_sd_info();
    
    while (1)
    {
        key = key_scan(0);
        if (key == WKUP_PRES)
        {
            /* 进行SD卡读测试 */
            sd_read_test();
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
