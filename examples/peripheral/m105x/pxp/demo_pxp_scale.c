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
 * \brief PXP���ش�������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��ʾ���豸��
 *   2. ���Ӵ����豸����ʾ��
 *
 * - ʵ������
 *   1.��ʾ���� ����ɫ+��ɫ+��ɫ+��ɫ����������Ļ�ϳ������Ŵ�+��С����
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

/* ����ȫ�ֵ� frame buffer �豸 */
static aw_fb_fix_info_t __fix_green_info;
static aw_fb_var_info_t __var_screen_info;

/* �������ŵ�ͼ�񻺴� */
static uint8_t  *__gp_pxp_buf;

/* PXP Output buffer config. */
static pxp_output_buffer_config_t __g_output_buffer_config;


#define __APP_IMG_HEIGHT  SYS_LCD_Y_RES
#define __APP_IMG_WIDTH   SYS_LCD_X_RES

#define __APP_BPP       2   /*�������*/
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

    /* frame buffer ��ʼ�� */
    aw_fb_init(p_fb);

    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&__var_screen_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &__fix_green_info);

    /* ���ñ������� */
    aw_fb_backlight(p_fb, 99);

    aw_fb_start(p_fb);

    /* ��ʼ����Ļ����ɫ(��ɫ) */
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

    /* ��λPXP */
    pxp_hard_reset();

    /* ���ñ�����ɫ */
    pxp_set_process_surface_back_ground_color(0x0);

    /* ����PS buffer */
    pxp_set_process_surface_buffer_config(&ps_buffer_config);

    /* ����AS */
    pxp_set_alpha_surface_position(0xFFFF, 0xFFFF, 0, 0);

    /* Output config. */
    __g_output_buffer_config.pixel_format    = kPXP_OutputPixelFormatRGB565;
    __g_output_buffer_config.interlaced_mode = kPXP_OutputProgressive;
    __g_output_buffer_config.buffer0_addr    = (uint32_t)__fix_green_info.vram_addr;
    __g_output_buffer_config.buffer1_addr    = 0;
    __g_output_buffer_config.pitch_bytes     = __APP_IMG_WIDTH * __APP_BPP;
    __g_output_buffer_config.width           = __APP_IMG_WIDTH;
    __g_output_buffer_config.height          = __APP_IMG_HEIGHT;

    /*�������buffer*/
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

    /* �Դ��ַ */
    p_vddr = (void *)__fix_green_info.vram_addr;

    while (1) {

        for (scale_fact  = scale_start;
             scale_fact <= scale_end;
             scale_fact *= scale_step) {

            width  = (uint16_t)((float)__APP_PS_SIZE / scale_fact);
            height = (uint16_t)((float)__APP_PS_SIZE / scale_fact);

            aw_cache_flush((void *)__gp_pxp_buf, __APP_PS_SIZE *  __APP_PS_SIZE * __APP_BPP);

            /* ���ŵ�ͼ���������� */
            pxp_set_process_surface_scaler(__APP_PS_SIZE, __APP_PS_SIZE, width, height);

            /*���ͼ�ε�λ��*/
            pxp_set_process_surface_position(__APP_PS_ULC_X,
                                             __APP_PS_ULC_Y,
                                             __APP_PS_ULC_X + width - 1U,
                                             __APP_PS_ULC_Y + height - 1U);

            /*���bufer���Դ�*/
            __g_output_buffer_config.buffer0_addr = (uint32_t)p_vddr[buf_index];
            pxp_set_output_buffer_config(&__g_output_buffer_config);

            /*����PXP*/
            pxp_start();

            /* �ȴ�PXPͼ�δ������ */
            pxp_complete_status_sync();

            aw_cache_invalidate((void *)__gp_pxp_buf, __APP_PS_SIZE *  __APP_PS_SIZE * __APP_BPP);

            aw_cache_flush(
                (void*)p_vddr[buf_index],
                __var_screen_info.buffer.buffer_size * __var_screen_info.buffer.buffer_num);

            /*�л��Դ�*/
            buf_index++;
            if (buf_index >= __var_screen_info.buffer.buffer_num) {
                buf_index = 0;
            }

            aw_mdelay(10);
        }

    }
}


void demo_pxp_scale (void){

    /* ��ʼ��fram buffer */
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

    /* ��ʼ��buffer */
    __scale_buf_init();

    /* ����pxp */
    __app_pxp_config();

    /* ���Ų��� */
    __app_scale();

    aw_mem_free(__gp_pxp_buf);
}
/* end of file */
