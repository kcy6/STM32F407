/**
 ****************************************************************************************************
 * @file        rng.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       随机数发生器驱动代码
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

#include "./BSP/RNG/rng.h"

RNG_HandleTypeDef g_rng_handle = {0};

/**
 * @brief   初始化随机数发生器
 * @param   无
 * @retval  初始化结果
 * @arg     0: 初始化成功
 * @arg     1: 初始化失败
 */
uint8_t rng_init(void)
{
    g_rng_handle.Instance = RNG;
    
    if (HAL_RNG_DeInit(&g_rng_handle) != HAL_OK)
    {
        return 1;
    }
    
    if (HAL_RNG_Init(&g_rng_handle) != HAL_OK)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief   HAL库RNG初始化MSP函数
 * @param   无
 * @retval  无
 */
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
    if (hrng->Instance == RNG)
    {
        __HAL_RCC_RNG_CLK_ENABLE();
    }
}

/**
 * @brief   获取随机数
 * @param   无
 * @retval  随机数
 */
uint32_t rng_get_random(void)
{
    uint32_t random;
    
    if (HAL_RNG_GenerateRandomNumber(&g_rng_handle, &random) != HAL_OK)
    {
        return 0;
    }
    
    return random;
}

/**
 * @brief   获取指定范围的随机数
 * @param   min: 指定范围的最小值
 * @param   max: 指定范围的最大值
 * @retval  随机数
 */
int32_t rng_get_random_range(int32_t min, int32_t max)
{
    uint32_t random;
    
    if (HAL_RNG_GenerateRandomNumber(&g_rng_handle, &random) != HAL_OK)
    {
        return 0;
    }
    
    return ((int32_t)random % (max - min + 1) + min);
}
