/**
 ****************************************************************************************************
 * @file        videoplayer.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-23
 * @brief       视频播放器应用代码
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
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TIMER/btim.h"
#include "./APP/videoplayer.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./TEXT/text.h"
#include "./MALLOC/malloc.h"
#include "./MJPEG/avi.h"
#include "./MJPEG/mjpeg.h"
#include <string.h>
#include <stdio.h>

uint16_t frame;
__IO uint8_t frameup;

/**
 * @brief       获取指定路径下有效视频文件的数量
 * @param       path: 指定路径
 * @retval      有效视频文件的数量
 */
static uint16_t video_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    DIR tdir;
    FILINFO *tfileinfo;
    
    tfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));   /* 申请内存 */
    res = (uint8_t)f_opendir(&tdir, (const TCHAR *)path);       /* 打开目录 */
    if ((res == 0) && tfileinfo)
    {
        while (1)                                               /* 查询总的有效文件数 */
        {
            res = (uint8_t)f_readdir(&tdir, tfileinfo);         /* 读取目录下的一个文件 */
            if ((res != 0) || (tfileinfo->fname[0] == 0))       /* 错误或到末尾，退出 */
            {
                break;
            }
            
            res = exfuns_file_type(tfileinfo->fname);
            if ((res & 0xF0) == 0x60)                           /* 是视频文件 */
            {
                rval++;
            }
        }
    }
    
    myfree(SRAMIN, tfileinfo);                                  /* 释放内存 */
    
    return rval;
}

/**
 * @brief       显示视频基本信息
 * @param       name : 视频名字
 * @param       index: 当前索引
 * @param       total: 总文件数
 * @retval      无
 */
static void video_bmsg_show(uint8_t *name, uint16_t index, uint16_t total)
{
    uint8_t *buf;
    
    buf = mymalloc(SRAMIN, 100);
    
    sprintf((char *)buf, "文件名:%s", name);
    text_show_string(10, 10, lcddev.width - 10, 16, (char *)buf, 16, 0, RED);   /* 显示文件名 */
    
    sprintf((char *)buf, "索引:%d/%d", index, total);
    text_show_string(10, 30, lcddev.width - 10, 16, (char *)buf, 16, 0, RED);   /* 显示索引 */
    
    myfree(SRAMIN, buf);
}

/**
 * @brief       显示当前视频文件的相关信息
 * @param       aviinfo: avi控制结构体
 * @retval      无
 */
static void video_info_show(AVI_INFO *aviinfo)
{
    uint8_t *buf;
    
    buf = mymalloc(SRAMIN, 100);
    
    sprintf((char *)buf, "声道数:%d,采样率:%d", aviinfo->Channels, aviinfo->SampleRate * 10);
    text_show_string(10, 50, lcddev.width - 10, 16, (char *)buf, 16, 0, RED);   /* 显示歌曲名字 */
    
    sprintf((char *)buf, "帧率:%d帧", 1000 / (aviinfo->SecPerFrame / 1000));
    text_show_string(10, 70, lcddev.width - 10, 16, (char *)buf, 16, 0, RED);   /* 显示歌曲名字 */
    
    myfree(SRAMIN, buf);
}

/**
 * @brief       显示当前播放时间
 * @param       favi   : 当前播放的视频文件
 * @param       aviinfo: avi控制结构体
 * @retval      无
 */
void video_time_show(FIL *favi, AVI_INFO *aviinfo)
{
    static uint32_t oldsec; /* 上一次的播放时间 */
    uint8_t *buf;
    uint32_t totsec = 0;    /* video文件总时间 */
    uint32_t cursec;        /* 当前播放时间 */
    
    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;                   /* 歌曲总长度(单位:ms) */
    totsec /= 1000;                                                                 /* 秒钟数. */
    cursec = ((double)favi->fptr / favi->obj.objsize) * totsec;                     /* 获取当前播放到多少秒 */
    
    if (oldsec != cursec)                                                           /* 需要更新显示时间 */
    {
        buf = mymalloc(SRAMIN, 100);
        oldsec = cursec;
        
        sprintf((char *)buf, "播放时间:%02d:%02d:%02d/%02d:%02d:%02d", cursec / 3600, (cursec % 3600) / 60, cursec % 60, totsec / 3600, (totsec % 3600) / 60, totsec % 60);
        text_show_string(10, 90, lcddev.width - 10, 16, (char *)buf, 16, 0, RED);   /* 显示歌曲名字 */
        
        myfree(SRAMIN, buf);
    }
}

/**
 * @brief       播放MJPEG视频
 * @param       pname: 视频文件名
 * @retval      按键键值
 *              KEY0_PRES: 上一个视频
 *              WKUP_PRES: 下一个视频
 *              其他值   : 错误代码
 */
static uint8_t video_play_mjpeg(uint8_t *pname)
{
    uint8_t *framebuf;
    uint8_t *pbuf;
    uint8_t res = 0;
    uint16_t offset;
    uint32_t nr;
    uint8_t key;
    FIL *favi;
    uint8_t *psaibuf;
    
    framebuf = (uint8_t *)mymalloc(SRAMIN, AVI_VIDEO_BUF_SIZE); /* 申请内存 */
    favi = (FIL *)mymalloc(SRAMIN, sizeof(FIL));
    psaibuf = (uint8_t *)mymalloc(SRAMIN, AVI_AUDIO_BUF_SIZE);
    if ((framebuf == NULL) || (favi == NULL) || (psaibuf == NULL))
    {
        printf("memory error!\r\n");
        res = 0xFF;
    }
    memset(psaibuf, 0, AVI_AUDIO_BUF_SIZE);
    
    while (res == 0)
    {
        res = (uint8_t)f_open(favi, (const TCHAR *)pname, FA_READ);                                                     /* 打开文件 */
        if (res == 0)
        {
            pbuf = framebuf;
            res = (uint8_t)f_read(favi, pbuf, AVI_VIDEO_BUF_SIZE, &nr);                                                 /* 开始读取 */
            if (res != 0)
            {
                printf("fread error:%d\r\n", res);
                break;
            }
            
            res = avi_init(pbuf, AVI_VIDEO_BUF_SIZE);                                                                   /* AVI解析 */
            if (res != 0)
            {
                printf("avi error:%d\r\n", res);
                break;
            }
            
            video_info_show(&g_avix);
            btim_tim7_int_init(g_avix.SecPerFrame / 100 - 1, 8400 - 1);                                                 /* 初始化基本定时器7中断，用于逐帧播放视频 */
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");                                                   /* 寻找movi ID */
            avi_get_streaminfo(pbuf + offset + 4);                                                                      /* 获取流信息 */
            f_lseek(favi, offset + 12);                                                                                 /* 跳过标志ID，读地址偏移到流数据开始处 */
            res = mjpegdec_init((lcddev.width - g_avix.Width) / 2, 110 + (lcddev.height - 110 - g_avix.Height) / 2);    /* 初始化JPG解码 */
            
            while (1)
            {
                if (g_avix.StreamID == AVI_VIDS_FLAG)                                                                   /* 视频流 */
                {
                    pbuf = framebuf;
                    f_read(favi, pbuf, g_avix.StreamSize + 8, &nr);                                                     /* 读取整帧+下一帧数据流ID信息 */
                    res = mjpegdec_decode(pbuf, g_avix.StreamSize);
                    if (res != 0)
                    {
                        printf("decode error!\r\n");
                    }
                    
                    while (frameup == 0);                                                                               /* 等待播放时间到达 */
                    frameup = 0;
                    frame++;
                }
                else
                {
                    video_time_show(favi, &g_avix);                                                                     /* 显示当前播放时间 */
                    f_read(favi, psaibuf, g_avix.StreamSize + 8, &nr);                                                  /* 填充psaibuf */
                    pbuf = psaibuf;
                }
                
                key = key_scan(0);
                if (key != 0)
                {
                    res = key;
                    break;
                }
                
                if (avi_get_streaminfo(pbuf + g_avix.StreamSize) != 0)                                                  /* 读取下一帧流标志 */
                {
                    printf("g_frame error\r\n");
                    res = WKUP_PRES;
                    break;
                }
            }
            
            TIM7->CR1 &= ~(TIM_CR1_CEN);                                                                                /* 关闭基本定时器7计数 */
            lcd_set_window(0, 0, lcddev.width, lcddev.height);                                                          /* 恢复窗口 */
            mjpegdec_free();                                                                                            /* 释放内存 */
            f_close(favi);                                                                                              /* 关闭文件 */
        }
    }
    
    myfree(SRAMIN, framebuf);
    myfree(SRAMIN, favi);
    myfree(SRAMIN, psaibuf);
    
    return res;
}

/**
 * @brief       播放视频
 * @param       无
 * @retval      无
 */
void video_play(void)
{
    uint8_t res;
    DIR vdir;
    FILINFO *vfileinfo;
    uint8_t *pname;
    uint16_t totavinum;
    uint16_t curindex;
    uint32_t *voffsettbl;
    uint8_t key;
    uint32_t temp;
    
    while (f_opendir(&vdir, "0:/VIDEO") != FR_OK)                               /* 检查VIDEO文件夹是否存在 */
    {
        text_show_string(60, 190, 240, 16, "VIDEO文件夹错误!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(60, 190, 240, 206, WHITE);
        delay_ms(200);
    }
    
    totavinum = video_get_tnum("0:/VIDEO");                                     /* 检查是否有视频文件 */
    while (totavinum == 0)
    {
        text_show_string(60, 190, 240, 16, "没有视频文件!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(60, 190, 240, 146, WHITE);
        delay_ms(200);
    }
    
    vfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));                   /* 申请内存 */
    pname = (uint8_t *)mymalloc(SRAMIN, 2 * FF_MAX_LFN + 1);
    voffsettbl = (uint32_t *)mymalloc(SRAMIN, totavinum  * 4);
    while ((vfileinfo == NULL) || (pname == NULL) || (voffsettbl == NULL))
    {
        text_show_string(60, 190, 240, 16, "内存分配失败!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(60, 190, 240, 146, WHITE);
        delay_ms(200);
    }
    
    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");                                /* 打开目录 */
    if (res == 0)
    {
        curindex = 0;
        while (1)
        {
            temp = vdir.dptr;                                                   /* 记录当前dptr偏移 */
            res = (uint8_t)f_readdir(&vdir, vfileinfo);                         /* 读取下一个文件 */
            if ((res != 0) || (vfileinfo->fname[0] == 0))                       /* 错误或到末尾，退出 */
            {
                break;
            }
            
            res = exfuns_file_type(vfileinfo->fname);
            if ((res & 0xF0) == 0x60)                                           /* 是图片文件 */
            {
                voffsettbl[curindex] = temp;                                    /* 记录索引 */
                curindex++;
            }
        }
    }
    
    curindex = 0;
    res = (uint8_t)f_opendir(&vdir, "0:/VIDEO");                                /* 打开目录 */
    while (res == 0)
    {
        dir_sdi(&vdir, voffsettbl[curindex]);                                   /* 改变当前目录索引 */
        res = (uint8_t)f_readdir(&vdir, vfileinfo);                             /* 读取目录的下一个文件 */
        if ((res != 0) || (vfileinfo->fname[0] == 0))                           /* 错误或到末尾，退出 */
        {
            break;
        }
        
        strcpy((char *)pname, "0:/VIDEO/");                                     /* 复制路径（目录） */
        strcat((char *)pname, (const char *)vfileinfo->fname);                  /* 将文件名接在后面 */
        
        lcd_clear(WHITE);
        video_bmsg_show((uint8_t *)vfileinfo->fname, curindex + 1, totavinum);  /* 显示视频基本信息 */
        
        key = video_play_mjpeg(pname);
        if (key == KEY0_PRES)   /* 上一个视频 */
        {
            if (curindex != 0)
            {
                curindex--;
            }
            else
            {
                curindex = totavinum - 1;
            }
        }
        else if (key == WKUP_PRES)  /* 下一个视频 */
        {
            curindex++;
            if (curindex >= totavinum)
            {
                curindex = 0;
            }
        }
        else
        {
            break;
        }
    }
    
    myfree(SRAMIN, vfileinfo);      /* 释放内存 */
    myfree(SRAMIN, pname);
    myfree(SRAMIN, voffsettbl);
}
