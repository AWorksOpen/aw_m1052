/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief PXP像素处理例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 显示屏设备宏
 *   2. 连接串口设备、显示屏
 *
 * - 实验现象：
 *   1.显示屏上 “蓝色+红色+绿色+白色”矩形在屏幕上持续“放大+缩小”。
 */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include <stdio.h>
#include "driver/pxp/awbl_imx1050_pxp.h"
#include "aw_fb.h"
#include "string.h"
#include "aw_mem.h"
#include "aw_cache.h"
#include "aw_demo_config.h"
#include "aw_errno.h"

/* 定义全局的 frame buffer 设备 */
static aw_fb_fix_info_t __fix_green_info;
static aw_fb_var_info_t __var_screen_info;

/* 定义缩放的图像缓存 */
static uint8_t  *__gp_pxp_buf;

/* PXP Output buffer config. */
static pxp_output_buffer_config_t __g_output_buffer_config;


#define __APP_IMG_HEIGHT  SYS_LCD_Y_RES
#define __APP_IMG_WIDTH   SYS_LCD_X_RES

#define __APP_BPP       2   /*像素深度*/
#define __APP_PS_SIZE   (__APP_IMG_HEIGHT / 2U)


#define __APP_PS_ULC_X     0
#define __APP_PS_ULC_Y     0


static aw_err_t __fb_init (void)
{
    void *p_fb = aw_fb_open(DE_FB, 0);

    if (p_fb == NULL) {
        aw_kprintf("open fb fail.\r\n");
        return -AW_ERROR;
    }

    /* frame buffer 初始化 */
    aw_fb_init(p_fb);

    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&__var_screen_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &__fix_green_info);

    /* 设置背光亮度 */
    aw_fb_backlight(p_fb, 99);

    aw_fb_start(p_fb);

    /* 初始化屏幕背景色(白色) */
    memset((void *)__fix_green_info.vram_addr, 
           0xFF,
           __var_screen_info.buffer.buffer_size * __var_screen_info.buffer.buffer_num);

    return AW_OK;
}


static void __scale_buf_init()
{
    uint32_t i, j;
    uint16_t *pxp_buf = (uint16_t *)__gp_pxp_buf;
    /*
     * The PS input buffer is:
     *
     *  -----------------------------
     *  |             |             |
     *  |             |             |
     *  |   BLUE      |   GREEN     |
     *  |             |             |
     *  |             |             |
     *  |-------------+-------------|
     *  |             |             |
     *  |             |             |
     *  |   WHITE     |    RED      |
     *  |             |             |
     *  |             |             |
     *  -----------------------------
     */
    for (i = 0; i < (__APP_PS_SIZE / 2); i++) {

        for (j = 0; j < (__APP_PS_SIZE / 2); j++) {
            pxp_buf[i * __APP_PS_SIZE  +  j] = 0x1F;
        }

        for (; j < __APP_PS_SIZE; j++) {
            pxp_buf[i * __APP_PS_SIZE  +  j] = 0x7E0;
        }
    }

    for (; i < __APP_PS_SIZE; i++) {

        for (j = 0; j < (__APP_PS_SIZE / 2); j++) {
            pxp_buf[i * __APP_PS_SIZE  +  j] = 0xFFFF;
        }

        for (; j < __APP_PS_SIZE; j++) {
            pxp_buf[i * __APP_PS_SIZE  +  j] = 0xF800;
        }
    }
}


static void __app_pxp_config (void)
{
    /* PS configure. */
    const pxp_ps_buffer_config_t ps_buffer_config = {
        .pixel_format   = kPXP_PsPixelFormatRGB565,
        .swap_byte      = 0,
        .buffer_addr    = (uint32_t)__gp_pxp_buf,
        .buffer_addr_u  = 0,
        .buffer_addr_v  = 0,
        .pitch_bytes    = __APP_PS_SIZE * __APP_BPP,
    };

    /* 复位PXP */
    pxp_hard_reset();

    /* 设置背景颜色 */
    pxp_set_process_surface_back_ground_color(0x0);

    /* 配置PS buffer */
    pxp_set_process_surface_buffer_config(&ps_buffer_config);

    /* 禁能AS */
    pxp_set_alpha_surface_position(0xFFFF, 0xFFFF, 0, 0);

    /* Output config. */
    __g_output_buffer_config.pixel_format    = kPXP_OutputPixelFormatRGB565;
    __g_output_buffer_config.interlaced_mode = kPXP_OutputProgressive;
    __g_output_buffer_config.buffer0_addr    = (uint32_t)__fix_green_info.vram_addr;
    __g_output_buffer_config.buffer1_addr    = 0;
    __g_output_buffer_config.pitch_bytes     = __APP_IMG_WIDTH * __APP_BPP;
    __g_output_buffer_config.width           = __APP_IMG_WIDTH;
    __g_output_buffer_config.height          = __APP_IMG_HEIGHT;

    /*配置输出buffer*/
    pxp_set_output_buffer_config(&__g_output_buffer_config);

    /* Disable CSC1, it is enabled by default. */
    pxp_enable_csc1(0);
}


static void __app_scale (void)
{
    uint8_t buf_index = 0U;
    uint8_t (*p_vddr)[__APP_IMG_WIDTH * __APP_IMG_HEIGHT * __APP_BPP] = NULL;

    float scale_start = 0.5f;
    float scale_end   = 4.0f;
    float scale_step  = 1.01f;
    float scale_fact;
    uint16_t width, height;

    /* 显存地址 */
    p_vddr = (void *)__fix_green_info.vram_addr;

    while (1) {

        for (scale_fact  = scale_start;
             scale_fact <= scale_end;
             scale_fact *= scale_step) {

            width  = (uint16_t)((float)__APP_PS_SIZE / scale_fact);
            height = (uint16_t)((float)__APP_PS_SIZE / scale_fact);

            aw_cache_flush((void *)__gp_pxp_buf, __APP_PS_SIZE *  __APP_PS_SIZE * __APP_BPP);

            /* 缩放的图形属性配置 */
            pxp_set_process_surface_scaler(__APP_PS_SIZE, __APP_PS_SIZE, width, height);

            /*输出图形的位置*/
            pxp_set_process_surface_position(__APP_PS_ULC_X,
                                             __APP_PS_ULC_Y,
                                             __APP_PS_ULC_X + width - 1U,
                                             __APP_PS_ULC_Y + height - 1U);

            /*输出bufer到显存*/
            __g_output_buffer_config.buffer0_addr = (uint32_t)p_vddr[buf_index];
            pxp_set_output_buffer_config(&__g_output_buffer_config);

            /*启动PXP*/
            pxp_start();

            /* 等待PXP图形处理完成 */
            pxp_complete_status_sync();

            aw_cache_invalidate((void *)__gp_pxp_buf, __APP_PS_SIZE *  __APP_PS_SIZE * __APP_BPP);

            aw_cache_flush(
                (void*)p_vddr[buf_index],
                __var_screen_info.buffer.buffer_size * __var_screen_info.buffer.buffer_num);

            /*切换显存*/
            buf_index++;
            if (buf_index >= __var_screen_info.buffer.buffer_num) {
                buf_index = 0;
            }

            aw_mdelay(10);
        }

    }
}


void demo_pxp_scale (void){

    /* 初始化fram buffer */
    aw_err_t ret = __fb_init();
    if(ret !=AW_OK){
        aw_kprintf("fb init fail.\r\n");
        return ;
    }

    __gp_pxp_buf = (uint8_t *) aw_mem_align(__APP_PS_SIZE * __APP_PS_SIZE * __APP_BPP,
                                            AW_CACHE_LINE_SIZE);

    if(NULL == __gp_pxp_buf){
        aw_kprintf("__gp_pxp_buf error.\r\n");
        return ;
    }

    /* 初始化buffer */
    __scale_buf_init();

    /* 配置pxp */
    __app_pxp_config();

    /* 缩放测试 */
    __app_scale();

    aw_mem_free(__gp_pxp_buf);
}
/* end of file */
