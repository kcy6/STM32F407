/**
 ****************************************************************************************************
 * @file        dcmi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       DCMI驱动代码
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

#include "./BSP/DCMI/dcmi.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/LED/led.h"

DCMI_HandleTypeDef g_dcmi_handle = {0};     /* DCMI句柄 */
DMA_HandleTypeDef g_dcmi_dma_handle = {0};  /* DMA句柄 */

uint8_t g_ov_frame = 0;                     /* 帧率 */
extern void jpeg_data_process(void);        /* JPEG数据处理函数 */

/**
 * @brief   初始化DCMI
 * @param   无
 * @retval  无
 */
void dcmi_init(void)
{
    /* 配置DCMI */
    g_dcmi_handle.Instance = DCMI;
    g_dcmi_handle.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
    g_dcmi_handle.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
    g_dcmi_handle.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
    g_dcmi_handle.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
    g_dcmi_handle.Init.CaptureRate = DCMI_CR_ALL_FRAME;
    g_dcmi_handle.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
    g_dcmi_handle.Init.JPEGMode = DCMI_JPEG_DISABLE;
    HAL_DCMI_Init(&g_dcmi_handle);
    
    DCMI->IER = 0x0;

    __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);        /* 使能帧中断 */
    __HAL_DCMI_ENABLE(&g_dcmi_handle);                          /* 使能DCMI */
}

/**
 * @brief   HAL库DCMI初始化MSP函数
 * @param   hdcmi: DCMI句柄
 * @retval  无
 */
void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    if (hdcmi->Instance == DCMI)
    {
        __HAL_RCC_DCMI_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        
        gpio_init_struct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init(GPIOA, &gpio_init_struct);
        
        gpio_init_struct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);
        
        gpio_init_struct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
        HAL_GPIO_Init(GPIOC, &gpio_init_struct);
        
        gpio_init_struct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
        HAL_GPIO_Init(GPIOE, &gpio_init_struct);
        
        HAL_NVIC_SetPriority(DCMI_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DCMI_IRQn);
    }
}

/**
 * @brief   初始化DCMI DMA
 * @param   mem0addr: 存储器0地址
 * @param   mem1addr: 存储器1地址
 * @param   memsize: 存储器大小
 * @param   memblen: 存储器位宽
 * @param   meminc: 存储器增长模式
 * @retval  无
 */
void dcmi_dma_init(uint32_t mem0addr, uint32_t mem1addr, uint16_t memsize, uint32_t memblen, uint32_t meminc)
{
    /* 配置DMA */
    __HAL_RCC_DMA2_CLK_ENABLE();
    g_dcmi_dma_handle.Instance = DMA2_Stream1;
    g_dcmi_dma_handle.Init.Channel = DMA_CHANNEL_1;
    g_dcmi_dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    g_dcmi_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_dcmi_dma_handle.Init.MemInc = meminc;
    g_dcmi_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    g_dcmi_dma_handle.Init.MemDataAlignment = memblen;
    g_dcmi_dma_handle.Init.Mode = DMA_CIRCULAR;
    g_dcmi_dma_handle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    g_dcmi_dma_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    g_dcmi_dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    g_dcmi_dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;
    g_dcmi_dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_DeInit(&g_dcmi_dma_handle);
    HAL_DMA_Init(&g_dcmi_dma_handle);
    
    __HAL_LINKDMA(&g_dcmi_handle, DMA_Handle, g_dcmi_dma_handle);
    
    if (mem1addr == 0)
    {
        HAL_DMA_Start(&g_dcmi_dma_handle, (uint32_t)&DCMI->DR, mem0addr, memsize);
    }
    else
    {
        HAL_DMAEx_MultiBufferStart(&g_dcmi_dma_handle, (uint32_t)&DCMI->DR, mem0addr, mem1addr, memsize);
        __HAL_DMA_ENABLE_IT(&g_dcmi_dma_handle, DMA_IT_TC);
        HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    }
}

/**
 * @brief   启动DCMI传输
 * @param   无
 * @retval  无
 */
void dcmi_start(void)
{
    lcd_set_cursor(0, 0);                   /* 设置坐标到原点 */
    lcd_write_ram_prepare();                /* 开始写入GRAM */
    __HAL_DMA_ENABLE(&g_dcmi_dma_handle);   /* 使能DMA */
    DCMI->CR |= DCMI_CR_CAPTURE;            /* DCMI捕获使能 */
}

/**
 * @brief   停止DCMI传输
 * @param   无
 * @retval  无
 */
void dcmi_stop(void)
{ 
    DCMI->CR &= ~(DCMI_CR_CAPTURE);         /* DCMI捕获关闭 */
    while (DCMI->CR & 0X01);                /* 等待传输结束 */
    __HAL_DMA_DISABLE(&g_dcmi_dma_handle);  /* 关闭DMA */
}

/**
 * @brief   DCMI中断服务函数
 * @param   无
 * @retval  无
 */
void DCMI_IRQHandler(void)
{
    HAL_DCMI_IRQHandler(&g_dcmi_handle);
}

/**
 * @brief   DCMI中断回调服务函数
 * @param   hdcmi: DCMI句柄
 * @retval  无
 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    __HAL_DCMI_CLEAR_FLAG(&g_dcmi_handle, DCMI_FLAG_FRAMERI);    /* 清除帧中断 */
    jpeg_data_process();    /* jpeg数据处理 */
    LED1_TOGGLE();          /* LED1闪烁 */
    g_ov_frame++;

    /* 重新使能帧中断,因为HAL_DCMI_IRQHandler()函数会关闭帧中断 */
    __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);
}

/* DCMI DMA接收回调函数, 仅双缓冲模式用到, 配合中断服务函数使用 */
void (*dcmi_rx_callback)(void);

/**
 * @brief       DMA2_Stream1中断服务函数(仅双缓冲模式会用到)
 * @param       无
 * @retval      无
 */
void DMA2_Stream1_IRQHandler(void)
{    
    if (__HAL_DMA_GET_FLAG(&g_dcmi_dma_handle, DMA_FLAG_TCIF1_5) != RESET)  /* DMA传输完成 */
    {
        __HAL_DMA_CLEAR_FLAG(&g_dcmi_dma_handle, DMA_FLAG_TCIF1_5); /* 清除DMA传输完成中断标志位 */
        dcmi_rx_callback();     /* 执行摄像头接收回调函数,读取数据等操作在这里面处理 */
    } 
}
