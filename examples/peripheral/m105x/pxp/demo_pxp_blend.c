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
 *   1.��ɫ���κͻ�ɫ��������Ļ���ƶ����������Ϊ��ɫ��
 */

#include <stdio.h>
#include <string.h>
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_fb.h"
#include "aw_demo_config.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "driver/pxp/awbl_imx1050_pxp.h"

#define __APP_IMG_HEIGHT  SYS_LCD_Y_RES
#define __APP_IMG_WIDTH   SYS_LCD_X_RES

/* ����ȫ�ֵ� frame buffer �豸 */
static aw_fb_fix_info_t __fix_screen_info;
static aw_fb_var_info_t __var_screen_info;

static uint8_t  *__gp_ps_buf;
static uint8_t  *__gp_as_buf;

/* PXP ��� buffer ����. */
static pxp_output_buffer_config_t __g_output_buffer_config;

/*�������*/
#define __APP_BPP         2

#define __APP_PS_WIDTH    (__APP_IMG_WIDTH  / 2)
#define __APP_PS_HEIGHT   (__APP_IMG_HEIGHT / 2)

#define __APP_AS_WIDTH    (__APP_IMG_WIDTH  / 2)
#define __APP_AS_HEIGHT   (__APP_IMG_HEIGHT / 2)

#define __APP_PS_ULC_X    ((__APP_IMG_WIDTH  / 2) - (__APP_PS_SIZE / 2))
#define __APP_PS_ULC_Y    ((__APP_IMG_HEIGHT / 2) - (__APP_PS_SIZE / 2))
#define __APP_PS_LRC_X    ((__APP_IMG_WIDTH  / 2) + (__APP_PS_SIZE / 2) - 1)

static aw_err_t __fb_init (void)
{
    void * p_fb = aw_fb_open(DE_FB, 0);
    if (p_fb == NULL) {
        aw_kprintf("open fb fail.\r\n");
        return -AW_ERROR;
    }

    /* frame buffer ��ʼ�� */
    aw_fb_init(p_fb);

    /*frame buffer �豸��Ϣ��ȡ*/
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&__var_screen_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &__fix_screen_info);

    /* ���ñ������� */
    aw_fb_backlight(p_fb, 99);
    aw_fb_start(p_fb);
    /* ��ʼ����Ļ����ɫ(��ɫ) */
    memset((void *)__fix_screen_info.vram_addr, 
           0xFF,
           __var_screen_info.buffer.buffer_size * __var_screen_info.buffer.buffer_num);

    return AW_OK;
}


static void __as_ps_buf_init (void)
{
    uint32_t i, j;
    uint16_t *ps_buf = (uint16_t *)__gp_ps_buf;
    uint16_t *as_buf = (uint16_t *)__gp_as_buf;

    /* The PS buffer is BLUE rectangle, the AS buffer is YELLOW rectangle. */
    for (i = 0; i < __APP_PS_HEIGHT; i++) {
        for (j = 0; j < __APP_PS_WIDTH; j++) {
            ps_buf[i * __APP_PS_WIDTH +  j] = 0xFF;
        }
    }

    for (i = 0; i < __APP_AS_HEIGHT; i++) {
        for (j = 0; j < __APP_AS_WIDTH; j++) {
            as_buf[i * __APP_AS_WIDTH +  j] = 0xFF00;
        }
    }

}


static void __app_pxp_config (void)
{
    /* PS configure. */
    const pxp_ps_buffer_config_t ps_buffer_config = {
        .pixel_format   = kPXP_PsPixelFormatRGB565,
        .swap_byte      = 0,
        .buffer_addr    = (uint32_t)__gp_ps_buf,
        .buffer_addr_u  = 0,
        .buffer_addr_v  = 0,
        .pitch_bytes    = __APP_PS_WIDTH * __APP_BPP,
    };

    /* ��λPXP */
    pxp_hard_reset();

    /* ����PS������ɫ */
    pxp_set_process_surface_back_ground_color(0x00);

    /* ����PS buffer */
    pxp_set_process_surface_buffer_config(&ps_buffer_config);

    /* AS config. */
    const pxp_as_buffer_config_t as_buffer_config = {
        .pixel_format = kPXP_AsPixelFormatRGB565,
        .buffer_addr  = (uint32_t)__gp_as_buf,
        .pitch_bytes  = __APP_AS_WIDTH * __APP_BPP,
    };

    /*����AS buffer*/
    pxp_set_alpha_surface_buffer_config(&as_buffer_config);

    /*AS blend config*/
    const pxp_as_blend_config_t as_blend_config = {
        .alpha          = 0,
        .invert_alpha   = 0,
        .alpha_mode     = kPXP_AlphaRop,
        .rop_mode       = kPXP_RopMergeAs
    };

    /*����AS blend*/
    pxp_set_alpha_surface_blend_config(&as_blend_config);

    /* Output config. */
    __g_output_buffer_config.pixel_format    = kPXP_OutputPixelFormatRGB565;
    __g_output_buffer_config.interlaced_mode = kPXP_OutputProgressive;
    __g_output_buffer_config.buffer0_addr    = (uint32_t)__fix_screen_info.vram_addr;
    __g_output_buffer_config.buffer1_addr    = 0;
    __g_output_buffer_config.pitch_bytes     = __APP_IMG_WIDTH * __APP_BPP;
    __g_output_buffer_config.width           = __APP_IMG_WIDTH;
    __g_output_buffer_config.height          = __APP_IMG_HEIGHT;

    /*�������pxp buffer*/
    pxp_set_output_buffer_config(&__g_output_buffer_config);

    /* ���� CSC1 */
    pxp_enable_csc1(0);
}



static void __app_blend (void)
{
    uint8_t buf_index = 0U;

    /*a pointer to a array*/
    uint8_t (*p_vddr)[__APP_IMG_WIDTH * __APP_IMG_HEIGHT * __APP_BPP] = NULL;

    int8_t ps_inc_x = 1;
    int8_t ps_inc_y = 1;
    int8_t as_inc_x = -1;
    int8_t as_inc_y = -1;
    uint16_t ps_ulc_x = 0U;
    uint16_t ps_ulc_y = 0U;
    uint16_t as_ulc_x = __APP_IMG_WIDTH - __APP_AS_WIDTH;
    uint16_t as_ulc_y = __APP_IMG_HEIGHT - __APP_AS_HEIGHT;
    uint16_t ps_lrc_x, ps_lrc_y, as_lrc_x, as_lrc_y;

    ps_lrc_x = ps_ulc_x + __APP_PS_WIDTH - 1U;
    ps_lrc_y = ps_ulc_y + __APP_PS_HEIGHT - 1U;
    as_lrc_x = as_ulc_x + __APP_AS_WIDTH - 1U;
    as_lrc_y = as_ulc_y + __APP_AS_HEIGHT - 1U;

    p_vddr = (void *)__fix_screen_info.vram_addr;

    while (1) {

        /*��buffer���뻺����*/
        aw_cache_flush((void *)__gp_ps_buf, __APP_PS_HEIGHT *  __APP_PS_WIDTH * __APP_BPP);
        aw_cache_flush((void *)__gp_as_buf, __APP_AS_HEIGHT *  __APP_AS_WIDTH * __APP_BPP);

        /*PSͼ��λ��*/
        pxp_set_process_surface_position(ps_ulc_x, ps_ulc_y, ps_lrc_x, ps_lrc_y);

        /*ASͼ��λ��*/
        pxp_set_alpha_surface_position(as_ulc_x, as_ulc_y, as_lrc_x, as_lrc_y);

        /*�������buffer���Դ��ַ*/
        __g_output_buffer_config.buffer0_addr = (uint32_t)p_vddr[buf_index];
        pxp_set_output_buffer_config(&__g_output_buffer_config);

        /* Start PXP. */
        pxp_start();

        /* �ȴ�PXPͼ�δ������ */
        pxp_complete_status_sync();

        /*��ջ����е�buffer*/
        aw_cache_invalidate((void *)__gp_ps_buf, __APP_PS_HEIGHT *  __APP_PS_WIDTH * __APP_BPP);
        aw_cache_invalidate((void *)__gp_as_buf, __APP_AS_HEIGHT *  __APP_AS_WIDTH * __APP_BPP);

        aw_cache_flush(
            (void*)p_vddr[buf_index],
            __var_screen_info.buffer.buffer_size * __var_screen_info.buffer.buffer_num);

        /*�л��Դ�*/
        buf_index++;
        if (buf_index >= __var_screen_info.buffer.buffer_num) {
            buf_index = 0;
        }

        ps_lrc_x += ps_inc_x;
        ps_lrc_y += ps_inc_y;
        as_lrc_x += as_inc_x;
        as_lrc_y += as_inc_y;

        ps_ulc_x += ps_inc_x;
        ps_ulc_y += ps_inc_y;
        as_ulc_x += as_inc_x;
        as_ulc_y += as_inc_y;

        if (0 == as_ulc_x) {
            as_inc_x = 1;
        } else if (__APP_IMG_WIDTH - 1 == as_lrc_x) {
            as_inc_x = -1;
        }

        if (0 == as_ulc_y) {
            as_inc_y = 1;
        } else if (__APP_IMG_HEIGHT - 1 == as_lrc_y) {
            as_inc_y = -1;
        }

        if (0 == ps_ulc_x) {
            ps_inc_x = 1;
        } else if (__APP_IMG_WIDTH - 1 == ps_lrc_x) {
            ps_inc_x = -1;
        }

        if (0 == ps_ulc_y) {
            ps_inc_y = 1;
        } else if (__APP_IMG_HEIGHT - 1 == ps_lrc_y) {
            ps_inc_y = -1;
        }
        aw_mdelay(50);
    }
}


void demo_pxp_blend (void)
{
    /* ��ʼ��fram buffer */
    aw_err_t ret = __fb_init();
    if(ret != AW_OK){
        aw_kprintf("fb init fail.\r\n");
        return;
    }

    __gp_ps_buf = (uint8_t *) aw_mem_align(__APP_PS_WIDTH * __APP_PS_HEIGHT * __APP_BPP,
                                           AW_CACHE_LINE_SIZE);
    __gp_as_buf = (uint8_t *) aw_mem_align(__APP_AS_WIDTH * __APP_AS_HEIGHT * __APP_BPP,
                                           AW_CACHE_LINE_SIZE);
    if (NULL == __gp_ps_buf || __gp_as_buf == NULL) {
        aw_kprintf("aw_mem_align error.\r\n");
        return ;
    }

    /* ��ʼ��buffer */
    __as_ps_buf_init();

    /* ����pxp */
    __app_pxp_config();

    /* blend���� */
    __app_blend();

    aw_mem_free(__gp_ps_buf);
    aw_mem_free(__gp_as_buf);
}

/* end of file */

