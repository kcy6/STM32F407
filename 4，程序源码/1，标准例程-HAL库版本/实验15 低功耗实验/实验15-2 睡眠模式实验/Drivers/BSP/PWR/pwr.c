/**
 ****************************************************************************************************
 * @file        pwr.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       PWR驱动代码
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

#include "./BSP/PWR/pwr.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"

/**
 * @brief   初始化PVD
 * @param   pl: 电压等级
 * @retval  无
 */
void pwr_pvd_init(uint32_t pl)
{
    PWR_PVDTypeDef pwr_pvd_struct = {0};
    
    __HAL_RCC_PWR_CLK_ENABLE();
    
    HAL_NVIC_SetPriority(PVD_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(PVD_IRQn);
    
    /* 配置并使能PVD */
    pwr_pvd_struct.PVDLevel = pl;
    pwr_pvd_struct.Mode = PWR_PVD_MODE_IT_RISING_FALLING;
    HAL_PWR_ConfigPVD(&pwr_pvd_struct);
    HAL_PWR_EnablePVD();
}

/**
 * @brief   进入睡眠模式
 * @param   无
 * @retval  无
 */
void pwr_enter_sleep(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    __HAL_RCC_PWR_CLK_ENABLE();
    PWR_WKUP_INT_GPIO_CLK_ENABLE();
    
    HAL_NVIC_SetPriority(PWR_WKUP_INT_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(PWR_WKUP_INT_IRQn);
    
    /* 配置唤醒引脚 */
    gpio_init_struct.Pin = PWR_WKUP_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING;
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(PWR_WKUP_INT_GPIO_PORT, &gpio_init_struct);
    
    /* 关闭SysTick中断 */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    
    /* 进入睡眠模式 */
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

/**
 * @brief   进入停止模式
 * @param   无
 * @retval  无
 */
void pwr_enter_stop(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    __HAL_RCC_PWR_CLK_ENABLE();
    PWR_WKUP_INT_GPIO_CLK_ENABLE();
    
    HAL_NVIC_SetPriority(PWR_WKUP_INT_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(PWR_WKUP_INT_IRQn);
    
    /* 配置唤醒引脚 */
    gpio_init_struct.Pin = PWR_WKUP_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING;
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(PWR_WKUP_INT_GPIO_PORT, &gpio_init_struct);
    
    /* 关闭SysTick中断 */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    
    /* 进入停止模式 */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

/**
 * @brief   进入待机模式
 * @param   无
 * @retval  无
 */
void pwr_enter_standby(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    
    /* 配置唤醒引脚 */
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    
    /* 进入待机模式 */
    HAL_PWR_EnterSTANDBYMode();
}

/**
 * @brief   HAL库PVD中断回调函数
 * @param   无
 * @retval  无
 */
void HAL_PWR_PVDCallback(void)
{
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO) != RESET)
    {
        /* 电压低于设置的电压等级，进行相应提示 */
        LED1(0);
        lcd_show_string(30, 130, 200, 16, 16, "PVD Low Voltage!", RED);
    }
    else
    {
        /* 电压不低于设置的电压等级，进行相应提示  */
        LED1(1);
        lcd_show_string(30, 130, 200, 16, 16, "PVD Voltage OK! ", BLUE);
    }
}

/**
 * @brief   HAL库外部中断中断回调函数
 * @param   无
 * @retval  无
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == PWR_WKUP_INT_GPIO_PIN)
    {
        /* 重新开启SysTick中断 */
        SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
    }
}

/**
 * @brief   PVD中断服务函数
 * @param   无
 * @retval  无
 */
void PVD_IRQHandler(void)
{
    HAL_PWR_PVD_IRQHandler();
}

/**
 * @brief   唤醒引脚外部中断服务函数
 * @param   无
 * @retval  无
 */
void PWR_WKUP_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(PWR_WKUP_INT_GPIO_PIN);
}
