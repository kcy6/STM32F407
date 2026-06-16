/**
 ****************************************************************************************************
 * @file        iap.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-15
 * @brief       IAP代码
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

#include "./IAP/iap.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/STMFLASH/stmflash.h"

typedef void (*iapfun)(void);

static iapfun jump2app;
static uint32_t g_iapbuf[512];

/**
 * @brief   在指定地址写入APP的bin数据
 * @param   appxaddr: 保存APP的bin数据的起始地址
 * @param   appbuf: APP的bin数据
 * @param   appsize: APP的大小，单位：字节
 * @retval  无
 */
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize)
{
    uint32_t t;
    uint16_t i = 0;
    uint32_t temp;
    uint8_t * dfu;
    uint32_t fwaddr;
    
    dfu = appbuf;
    fwaddr = appxaddr;
    for (t=0; t<appsize; t+=4)
    {
        temp = (uint32_t)dfu[3] << 24;
        temp |= (uint32_t)dfu[2] << 16;
        temp |= (uint32_t)dfu[1] << 8;
        temp |= (uint32_t)dfu[0];
        dfu += 4;
        g_iapbuf[i++] = temp;
        if (i == 512)
        {
            i = 0;
            stmflash_write(fwaddr, g_iapbuf, 512);
            fwaddr += 2048;
        }
    }
    if (i != 0)
    {
        stmflash_write(fwaddr, g_iapbuf, i);
    }
}

/**
 * @brief   跳转到APP程序执行(执行APP)
 * @param   appxaddr: 保存APP的bin数据的起始地址
 * @retval  无
 */
void iap_load_app(uint32_t appxaddr)
{
    /* 检查栈顶地址是否合法.可以放在内部SRAM共64KB(0x20000000) */
    if (((*(volatile uint32_t *)appxaddr) & 0x2FFE0000) == 0x20000000)
    {
        /* 用户代码区第二个字为程序开始地址 */
        jump2app = (iapfun)*(volatile uint32_t *)(appxaddr + 4);
        
        /* 初始化APP堆栈指针（用户代码区的第一个字用于存放栈顶地址） */
        sys_msr_msp(*(volatile uint32_t *)appxaddr);
        
        /* 跳转到APP */
        jump2app();
    }
}
