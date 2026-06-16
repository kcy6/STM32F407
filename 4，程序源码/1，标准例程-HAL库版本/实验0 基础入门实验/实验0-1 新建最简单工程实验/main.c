/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       新建最简单工程实验
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

/* 基地址定义 */
#define PERIPH_BASE     0x40000000UL
#define AHB1PERIPH_BASE (PERIPH_BASE + 0x00020000UL)
#define RCC_BASE        (AHB1PERIPH_BASE + 0x3800UL)
#define GPIOF_BASE      (AHB1PERIPH_BASE + 0x1400UL)

/* 寄存器映射 */
#define RCC_AHB1ENR     *(volatile unsigned int *)(RCC_BASE + 0x30)
#define GPIOF_MODER     *(volatile unsigned int *)(GPIOF_BASE + 0x00)
#define GPIOF_OTYPER    *(volatile unsigned int *)(GPIOF_BASE + 0x04)
#define GPIOF_OSPEEDR   *(volatile unsigned int *)(GPIOF_BASE + 0x08)
#define GPIOF_PUPDR     *(volatile unsigned int *)(GPIOF_BASE + 0x0C)
#define GPIOF_ODR       *(volatile unsigned int *)(GPIOF_BASE + 0x14)

/* SystemInit函数 */
void SystemInit(void)
{
    /* 启动文件会调用该函数，不实现的话编译器会报错 */
    
#ifdef __TARGET_FPU_VFP
    /* 使能访问协处理器，否则开启FPU后会触发HardFault异常 */
    *(volatile unsigned int *)(0xE000ED88) |= ((3UL << 10*2)|(3UL << 11*2));
#endif
}

/* 延时函数 */
static void delay(volatile unsigned int t)
{
    while (t--);
}

/* main函数 */
int main(void)
{
    /* 未进行系统时钟配置，默认使用HSI（16MHz） */
    
    RCC_AHB1ENR |= (1UL << 5);          /* 使能GPIOF端口时钟 */
    GPIOF_MODER &= ~(3UL << (2*9));
    GPIOF_MODER |= (1UL << (2*9));      /* 配置PF9为通用输出模式 */
    GPIOF_OTYPER &= ~(1UL << 9);        /* 配置PF9为推挽输出 */
    GPIOF_OSPEEDR &= ~(3UL << (2*9));   /* 配置PF9为低速 */
    GPIOF_PUPDR &= ~(3UL << (2*9));
    GPIOF_PUPDR |= (2UL << (2*9));      /* 配置PF9为下拉 */
    
    while (1)
    {
        GPIOF_ODR ^= (1UL << 9);        /* 配置PF9输出状态取反 */
        delay(500000);                  /* 延时一段时间 */
    }
}
