/**
 ****************************************************************************************************
 * @file        rtc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       RTC驱动代码
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

#include "./BSP/RTC/rtc.h"
#include "./BSP/LED/led.h"
#include <stdio.h>

RTC_HandleTypeDef g_rtc_handle = {0};

/**
 * @brief   读取后备寄存器
 * @param   bkrx: 后备寄存器编号
 * @retval  后备寄存器值
 */
uint16_t rtc_read_bkr(uint32_t bkrx)
{
    return (uint16_t)HAL_RTCEx_BKUPRead(&g_rtc_handle, bkrx);
}

/**
 * @brief   写入后背寄存器
 * @param   bkrx: 后备寄存器编号
 * @param   data: 后备寄存器值
 * @retval  无
 */
void rtc_write_bkr(uint32_t bkrx, uint16_t data)
{
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&g_rtc_handle, bkrx, data);
}

/**
 * @brief   初始化RTC
 * @param   无
 * @retval  初始化结果
 * @arg     0: 初始化成功
 * @arg     1: 初始化失败
 */
uint8_t rtc_init(void)
{
    uint16_t flag;
    
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_RTC_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    
    /* 初始化RTC */
    g_rtc_handle.Instance = RTC;
    g_rtc_handle.Init.HourFormat = RTC_HOURFORMAT_24;
    g_rtc_handle.Init.AsynchPrediv = 0x7F;
    g_rtc_handle.Init.SynchPrediv = 0xFF;
    g_rtc_handle.Init.OutPut = RTC_OUTPUT_DISABLE;
    g_rtc_handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    g_rtc_handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    
    /* 从后备寄存器读取RTC初始化标志 */
    flag = rtc_read_bkr(0);
    
    if (HAL_RTC_Init(&g_rtc_handle) != HAL_OK)
    {
        return 1;
    }
    
    /* RTC第一次初始化 */
    if ((flag != 0x5051) && (flag != 0x5050))
    {
        /* 设置RTC时间和日期信息 */
        rtc_set_time(8, 0, 0, 0);
        rtc_set_date(23, 4, 23, 7);
    }
    
    return 0;
}

/**
 * @brief   HAL库RTC初始化MSP函数
 * @param   无
 * @retval  无
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef rcc_osc_init_struct = {0};
    RCC_PeriphCLKInitTypeDef rcc_periph_clk_init_struct = {0};
    
    if (hrtc->Instance == RTC)
    {
        /* 尝试使用LSE */
        rcc_osc_init_struct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
        rcc_osc_init_struct.LSEState = RCC_LSE_ON;
        rcc_osc_init_struct.PLL.PLLState = RCC_PLL_NONE;
        if (HAL_RCC_OscConfig(&rcc_osc_init_struct) == HAL_OK)
        {
            rcc_periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
            rcc_periph_clk_init_struct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
            HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_init_struct);
            
            /* 写入RTC初始化标志 */
            rtc_write_bkr(0, 0x5050);
            
            __HAL_RCC_RTC_ENABLE();
        }
        else
        {
            /* LSE不可用，则使用LSI */
            rcc_osc_init_struct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;
            rcc_osc_init_struct.LSEState = RCC_LSE_OFF;
            rcc_osc_init_struct.LSIState = RCC_LSI_ON;
            rcc_osc_init_struct.PLL.PLLState = RCC_PLL_NONE;
            HAL_RCC_OscConfig(&rcc_osc_init_struct);
            
            rcc_periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
            rcc_periph_clk_init_struct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
            HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_init_struct);
            
            /* 写入RTC初始化标志 */
            rtc_write_bkr(0, 0x5051);
            
            __HAL_RCC_RTC_ENABLE();
        }
    }
}

/**
 * @brief   设置RTC时间信息
 * @param   hour: 时
 * @param   minute: 分
 * @param   second: 秒
 * @param   ampm: 上下午
 * @arg     0: 上午
 * @arg     1: 下午
 * @retval  设置结果
 * @arg     0: 设置成功
 * @arg     1: 设置失败
 */
uint8_t rtc_set_time(uint8_t hour, uint8_t minute, uint8_t second, uint8_t ampm)
{
    RTC_TimeTypeDef rtc_time_struct = {0};
    
    rtc_time_struct.Hours = hour;
    rtc_time_struct.Minutes = minute;
    rtc_time_struct.Seconds = second;
    rtc_time_struct.TimeFormat = ampm;
    rtc_time_struct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    rtc_time_struct.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&g_rtc_handle, &rtc_time_struct, RTC_FORMAT_BIN) != HAL_OK)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief   设置RTC日期信息
 * @param   year: 年
 * @param   month: 月
 * @param   date: 日
 * @param   week: 星期
 * @retval  设置结果
 * @arg     0: 设置成功
 * @arg     1: 设置失败
 */
uint8_t rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
    RTC_DateTypeDef rtc_date_struct = {0};
    
    rtc_date_struct.WeekDay = week;
    rtc_date_struct.Month = month;
    rtc_date_struct.Date = date;
    rtc_date_struct.Year = year;
    if (HAL_RTC_SetDate(&g_rtc_handle, &rtc_date_struct, RTC_FORMAT_BIN) != HAL_OK)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief   获取RTC时间信息
 * @param   hour: 时
 * @param   minute: 分
 * @param   second: 秒
 * @param   ampm: 上下午
 * @arg     0: 上午
 * @arg     1: 下午
 * @retval  无
 */
void rtc_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *ampm)
{
    RTC_TimeTypeDef rtc_time_struct = {0};
    
    HAL_RTC_GetTime(&g_rtc_handle, &rtc_time_struct, RTC_FORMAT_BIN);
    
    *hour = rtc_time_struct.Hours;
    *minute = rtc_time_struct.Minutes;
    *second = rtc_time_struct.Seconds;
    *ampm = rtc_time_struct.TimeFormat;
}

/**
 * @brief   获取RTC日期信息
 * @param   year: 年
 * @param   month: 月
 * @param   date: 日
 * @param   week: 星期
 * @retval  无
 */
void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *week)
{
    RTC_DateTypeDef rtc_date_struct = {0};
    
    HAL_RTC_GetDate(&g_rtc_handle, &rtc_date_struct, RTC_FORMAT_BIN);
    
    *year = rtc_date_struct.Year;
    *month = rtc_date_struct.Month;
    *date = rtc_date_struct.Date;
    *week = rtc_date_struct.WeekDay;
}

/**
 * @brief   年月日转星期
 * @param   year: 年
 * @param   month: 月
 * @param   date: 日
 * @retval  星期
 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t date)
{
    uint8_t week = 0;
    
    if (month < 3)
    {
        month += 12;
        --year;
    }
    
    week = (date + 1 + 2 * month + 3 * (month + 1) / 5 + year + (year >> 2) - year / 100 + year / 400) % 7;
    
    return week;
}

/**
 * @brief   设置RTC闹钟时间信息
 * @param   week: 星期
 * @param   hour: 时
 * @param   minute: 分
 * @param   second: 秒
 * @retval  无
 */
void rtc_set_alarm(uint8_t week, uint8_t hour, uint8_t minute, uint8_t second)
{
    RTC_AlarmTypeDef rtc_alarm_struct = {0};
    
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
    
    /* 设置闹钟中断 */
    rtc_alarm_struct.AlarmTime.Hours = hour;
    rtc_alarm_struct.AlarmTime.Minutes = minute;
    rtc_alarm_struct.AlarmTime.Seconds = second;
    rtc_alarm_struct.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    rtc_alarm_struct.AlarmTime.SubSeconds = 0;
    rtc_alarm_struct.AlarmMask = RTC_ALARMMASK_NONE;
    rtc_alarm_struct.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    rtc_alarm_struct.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    rtc_alarm_struct.AlarmDateWeekDay = week;
    rtc_alarm_struct.Alarm = RTC_ALARM_A;
    HAL_RTC_SetAlarm_IT(&g_rtc_handle, &rtc_alarm_struct, RTC_FORMAT_BIN);
}

/**
 * @brief   HAL库RTC闹钟A中断回调函数
 * @param   hrtc: RTC句柄
 * @retval  无
 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    printf("Alarm A!\r\n");
}

/**
 * @brief   RTC闹钟中断服务函数
 * @param   无
 * @retval  无
 */
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&g_rtc_handle);
}

/**
 * @brief   设置RTC周期性唤醒中断
 * @param   clock: 唤醒时钟
 * @param   count: 唤醒计数器
 * @retval  无
 */
void rtc_set_wakeup(uint8_t clock, uint8_t count)
{
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    
    HAL_RTCEx_SetWakeUpTimer_IT(&g_rtc_handle, count, clock);
}

/**
 * @brief   HAL库RTC唤醒中断回调函数
 * @param   hrtc: RTC句柄
 * @retval  无
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    LED1_TOGGLE();
}

/**
 * @brief   RTC唤醒中断服务函数
 * @param   无
 * @retval  无
 */
void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&g_rtc_handle);
}
