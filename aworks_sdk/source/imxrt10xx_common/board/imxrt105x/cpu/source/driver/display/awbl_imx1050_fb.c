/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief AWorks 帧缓冲驱动
 *
 * \internal
 * \par modification history:
 * - 1.01  19-04-12  cat, add interface
 * - 1.00  17-11-21  mex, first implementation.
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "aw_vdebug.h"
#include "aw_common.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "aw_delay.h"

#include "aw_assert.h"
#include "aw_hwtimer.h"
#include "aw_int.h"
#include "aw_cache.h"
#include "string.h"
#include "aw_mem.h"
#include "aw_gpio.h"

#include "driver/display/awbl_imx1050_fb.h"
#include "driver/display/awbl_imx1050_lcdif.h"
#include "driver/display/awbl_imx1050_tft_panel.h"

/*******************************************************************************
  macros
*******************************************************************************/
#define __DEV_TO_THIS(p_dev) \
    AW_CONTAINER_OF(p_dev, awbl_imx1050_fb_t, dev)

#define __FB_DEVINFO_GET(p_this) \
    ((awbl_imx1050_fb_devinfo_t *)AWBL_DEVINFO_GET((p_this)))

#define __PANEL_DEVINFO_GET(p_dev) \
   (awbl_imx1050_tft_panel_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

#define __INFO_TO_THIS(pinfo) \
    AW_CONTAINER_OF(pinfo, awbl_imx1050_fb_t, fb_info);

#define __PANEL_DEVINFO_TO_THIS(pinfo) \
    AW_CONTAINER_OF(pinfo, awbl_imx1050_tft_panel_devinfo_t, panel_info);

/*******************************************************************************
  locals
*******************************************************************************/

#define            __AWBL_FB_BUFFER_NUM            3
aw_local uint32_t  __g_fb_vram[__AWBL_FB_BUFFER_NUM];

/*******************************************************************************
    implementation
*******************************************************************************/
#if defined (__CC_ARM)
extern  unsigned int  const Image$$ER_VRAM_START$$Base;
extern  unsigned int  const Image$$ER_VRAM_END$$Base;

#define VRAM_START              ((uint32_t)&Image$$ER_VRAM_START$$Base)
#define VRAM_END                (((uint32_t)&Image$$ER_VRAM_END$$Base) - 1)

#elif defined (__GNUC__)
extern int __lcd_vram_start__;
extern int __lcd_vram_end__;

#define VRAM_START              (((uint32_t)&__lcd_vram_start__))
#define VRAM_END                (((uint32_t)&__lcd_vram_end__) - 1)

#endif

AWBL_METHOD_IMPORT(awbl_imx1050_panel_get);

extern awbl_dev_t *awbl_dev_find_by_name (const char *name, int unit);

/******************************************************************************/

awbl_imx1050_panel_t *awbl_imx1050_lcdif_panel_get (const char *name, int unit)
{
    awbl_dev_t            *p_dev         = NULL;
    awbl_method_handler_t  pfunc_handler = NULL;
    awbl_imx1050_panel_t  *p_panel       = NULL;

    /* find device by name */
    p_dev = awbl_dev_find_by_name(name, unit);
    if (p_dev == NULL) {
        return NULL;
    }

    /* find fb method on the device */
    pfunc_handler = awbl_dev_method_get(p_dev, AWBL_METHOD_CALL(awbl_imx1050_panel_get));
    if (pfunc_handler == NULL) {
        return NULL;
    }

    /* get fb_info */
    pfunc_handler(p_dev, &p_panel);

    return p_panel;
}

aw_local aw_err_t __panel_init(awbl_fb_info_t *p_fb_info,
                               awbl_imx1050_panel_t *p_panel,
                               size_t                memsize)
{
    int                               i;
    awbl_imx1050_lcdif_t             *p_lcdif   = p_panel->p_lcdif;
    awbl_imx1050_tft_panel_t         *p_this    =    \
        (awbl_imx1050_tft_panel_t *)p_panel->p_cookie;
    awbl_imx1050_tft_panel_devinfo_t *p_devinfo = __PANEL_DEVINFO_GET(p_this);

    /* set up lcdif clock */
    aw_clk_rate_set(IMX1050_CLK_LCDIF1_PRED,
                    p_fb_info->var_info.dclk_f.typ_dclk_f);

    aw_clk_rate_set(IMX1050_CLK_LCDF1_PODF,
                    p_fb_info->var_info.dclk_f.typ_dclk_f);

    AW_INFOF(("set dclk %d Hz,real lcd clk=%ld Hz\n",
              p_fb_info->var_info.dclk_f.typ_dclk_f,
              aw_clk_rate_get(p_lcdif->p_devinfo->ref_pixel_clk_id)));

    /* 使能LCDIF外设时钟 */
    for (i = 0; i< p_lcdif->p_devinfo->dev_clk_count;i++) {
        aw_clk_enable(p_lcdif->p_devinfo->dev_clks[i]);
    }

    /* 复位LCD控制器所有时序参数设置 */
    awbl_imx1050_lcdif_release_dotclk_panel(p_lcdif);

    /* 初始化LCD控制器 */
    awbl_imx1050_lcdif_init(p_lcdif, p_fb_info->var_info.bpp.bus_bpp, p_fb_info->var_info.bpp.word_lbpp);

    /* 初始化时序参数 */
    awbl_imx1050_lcdif_setup_dotclk_panel(p_lcdif, &p_fb_info->var_info);

    /* 初始化映射地址 */
    awbl_imx1050_lcdif_dma_init(p_lcdif, p_fb_info->fix_info.vram_addr, memsize);

    return AW_OK;
}

aw_local aw_err_t __panel_pan_backlight (awbl_imx1050_panel_t *p_panel, int pwm_num, int percent)
{
    awbl_imx1050_tft_panel_t  *p_this       = (awbl_imx1050_tft_panel_t *)p_panel->p_cookie;
    awbl_imx1050_lcdif_t      *p_lcdif      = p_panel->p_lcdif;
    aw_it_dev_t               *p_it         = NULL;

    int tmp = 0;

    if (percent < 0 || percent > 100) {

        AW_INFOF(("The value of percent is invalid, the range is 0 to 100. Use the default value of this panel.\n"));
        tmp = p_lcdif->p_panel->p_panel_info->bl_percent;

    } else {

        tmp = percent;
    }

    /* 当指定PWM ID时，才使用PWM设置背光 */
    if (pwm_num != -1) {

        p_it = awbl_pwm_it_ctor(&p_this->pwm_it, 100, pwm_num, 10000000);
        aw_it_level_set(p_it, tmp);

    } else if (p_lcdif->p_devinfo->gpio_backlight != -1) {
        if (percent > 0) {
            /* gpio管脚点亮背光 */
            aw_gpio_set(p_lcdif->p_devinfo->gpio_backlight, 1);
        } else {
            /* gpio管脚关闭背光 */
            aw_gpio_set(p_lcdif->p_devinfo->gpio_backlight, 0);
        }
    }

    return AW_OK;
}

aw_local int __lcd_fb_init (awbl_fb_info_t *p_fb_info)
{
    awbl_imx1050_fb_t         *p_this    = __INFO_TO_THIS(p_fb_info);
    awbl_imx1050_fb_devinfo_t *p_devinfo = __FB_DEVINFO_GET(p_this);

    if (p_fb_info->var_info.buffer.buffer_num < 3) {

        aw_int_connect(p_devinfo->lcdif_info.inum,
                      (aw_pfuncvoid_t)__imx_lcdif_common_buf_isr,
                      (void *)&(p_this->dev));
    } else {

        aw_int_connect(p_devinfo->lcdif_info.inum,
                      (aw_pfuncvoid_t)__imx_lcdif_triple_buf_isr,
                      (void *)&(p_this->dev));
    }
    aw_int_enable(p_devinfo->lcdif_info.inum);

    /* 本地保存buffer地址 */
    for (uint32_t i = 0; i < p_fb_info->var_info.buffer.buffer_num; i++) {

        __g_fb_vram[i] = p_fb_info->fix_info.vram_addr + p_fb_info->var_info.buffer.buffer_size * i;
        memset((uint8_t *)__g_fb_vram[i],  0x00, p_fb_info->var_info.buffer.buffer_size);
    }

    return __panel_init(p_fb_info, p_this->lcdif.p_panel, 0);

}

aw_local int __lcd_fb_start (awbl_fb_info_t *p_fb_info)
{
    awbl_imx1050_fb_t *p_this = __INFO_TO_THIS(p_fb_info);

    return awbl_imx1050_lcdif_run(p_this->lcdif.p_panel->p_lcdif);
}

aw_local int __lcd_fb_stop (awbl_fb_info_t *p_fb_info)
{
    awbl_imx1050_fb_t *p_this = __INFO_TO_THIS(p_fb_info);

    return awbl_imx1050_lcdif_stop(p_this->lcdif.p_panel->p_lcdif);
}

aw_local uint32_t __lcd_fb_get_online_buf(awbl_fb_info_t *p_fb_info)
{
    return __g_fb_vram[p_fb_info->fb_status.online_buffer];
}

aw_local uint32_t __lcd_fb_get_offline_buf(awbl_fb_info_t *p_fb_info)
{
    return __g_fb_vram[p_fb_info->fb_status.offline_buffer];
}

aw_local aw_err_t __lcd_fb_swap_buf (awbl_fb_info_t *p_fb_info)
{
    awbl_imx1050_fb_t *p_this  = __INFO_TO_THIS(p_fb_info);
    aw_err_t           ret     = AW_OK;

    if (p_fb_info->var_info.buffer.buffer_num >= 3) {

        ret = awbl_imx1050_lcdif_pan_triple_vram_addr_set(    \
              p_this->lcdif.p_panel->p_lcdif,                 \
              __g_fb_vram[p_fb_info->fb_status.offline_buffer]);
    } else {

        ret = awbl_imx1050_lcdif_pan_common_vram_addr_set(    \
              p_this->lcdif.p_panel->p_lcdif,                 \
              __g_fb_vram[p_fb_info->fb_status.offline_buffer]);
    }

    p_fb_info->fb_status.online_buffer   = p_fb_info->fb_status.offline_buffer++;
    p_fb_info->fb_status.offline_buffer %= p_fb_info->var_info.buffer.buffer_num;

    return ret;
}

aw_local aw_err_t __lcd_fb_try_swap_buf (awbl_fb_info_t *p_fb_info)
{
    awbl_imx1050_fb_t *p_this  = __INFO_TO_THIS(p_fb_info);
    aw_err_t           ret     = AW_OK;

    if (p_fb_info->var_info.buffer.buffer_num < 3) {
        return -AW_EPERM;
    }

    ret = awbl_imx1050_lcdif_pan_try_vram_addr_set(       \
          p_this->lcdif.p_panel->p_lcdif,                 \
          __g_fb_vram[p_fb_info->fb_status.offline_buffer]);

    if (AW_OK != ret) {
        return ret;
    }

    p_fb_info->fb_status.online_buffer   = p_fb_info->fb_status.offline_buffer++;
    p_fb_info->fb_status.offline_buffer %= p_fb_info->var_info.buffer.buffer_num;

    return AW_OK;
}

aw_local int __lcd_fb_blank (awbl_fb_info_t *p_fb_info, aw_fb_black_t blank)
{
    awbl_imx1050_fb_t *p_this = __INFO_TO_THIS(p_fb_info);

    return awbl_imx1050_lcdif_pan_blank(p_this->lcdif.p_panel->p_lcdif, blank);
}

aw_local aw_err_t __lcd_fb_backlight (awbl_fb_info_t *p_fb_info, int level)
{
    awbl_imx1050_fb_t         *p_this    = __INFO_TO_THIS(p_fb_info);
    awbl_imx1050_fb_devinfo_t *p_devinfo = __FB_DEVINFO_GET(p_this);

    return __panel_pan_backlight(p_this->lcdif.p_panel,
                                 p_devinfo->lcdif_info.default_pwm_num,
                                 level);
}

aw_local aw_err_t __lcd_fb_config (awbl_fb_info_t *p_fb_info)
{
    awbl_imx1050_fb_t         *p_this    = __INFO_TO_THIS(p_fb_info);
    aw_fb_fix_info_t *p_fb_finfo = (aw_fb_fix_info_t *)&p_fb_info->fix_info;
    aw_fb_var_info_t *p_fb_vinfo = (aw_fb_var_info_t *)&p_fb_info->var_info;
    uint32_t panel_x_res, panel_y_res, vram_size, bpp_bits;

    /* 配置之前停止lcdif */
    awbl_imx1050_lcdif_stop(p_this->lcdif.p_panel->p_lcdif);

    bpp_bits    = (p_fb_vinfo->bpp.word_lbpp + 7) / 8;
    panel_x_res =  p_fb_vinfo->res.x_res;
    panel_y_res =  p_fb_vinfo->res.y_res;

    p_fb_vinfo->buffer.buffer_size = panel_x_res * panel_y_res * bpp_bits;

    p_fb_vinfo->buffer.buffer_num = __AWBL_FB_BUFFER_NUM;

    do {
        vram_size = p_fb_vinfo->buffer.buffer_size * p_fb_vinfo->buffer.buffer_num;

        if (vram_size <= p_fb_finfo->vram_max) {

            /* 本地保存buffer地址 */
            for (uint32_t i = 0; i < p_fb_info->var_info.buffer.buffer_num; i++) {

                __g_fb_vram[i] = p_fb_info->fix_info.vram_addr + p_fb_info->var_info.buffer.buffer_size * i;
                memset((uint8_t *)__g_fb_vram[i],  0x00, p_fb_info->var_info.buffer.buffer_size);
            }
            return __panel_init(p_fb_info, p_this->lcdif.p_panel, 0);
        }

    } while (--(p_fb_vinfo->buffer.buffer_num));

    return -AW_ENOMEM;
}

aw_local aw_err_t __lcd_fb_reset (awbl_fb_info_t *p_fb_info)
{
    awbl_imx1050_fb_t         *p_this    = __INFO_TO_THIS(p_fb_info);
    awbl_imx1050_fb_devinfo_t *p_devinfo = __FB_DEVINFO_GET(p_this);
    uint32_t panel_x_res, panel_y_res, vram_size, bpp_bits;

    awbl_imx1050_lcdif_stop(p_this->lcdif.p_panel->p_lcdif);

    __panel_init(p_fb_info, p_this->lcdif.p_panel, 0);

    return awbl_imx1050_lcdif_run(p_this->lcdif.p_panel->p_lcdif);
}

/**
 * \brief frame buffer operations
 */
static const awbl_fb_ops_t __g_fb_ops = {
    __lcd_fb_init,
    __lcd_fb_start,
    __lcd_fb_stop,
    __lcd_fb_reset,
    __lcd_fb_get_online_buf,
    __lcd_fb_get_offline_buf,
    __lcd_fb_swap_buf,
    __lcd_fb_try_swap_buf,
    __lcd_fb_blank,
    __lcd_fb_backlight,
    __lcd_fb_config,
    NULL,
};

/**
 * \brief initialization routine of AWorks fb
 *
 * This routine performs the second level initialization of the
 * imx1050 fb.
 *
 * This routine is called later during system initialization. OS features
 * such as memory allocation are available at this time.
 */
aw_local void __imx1050_lcd_fb_inst_init2 (struct awbl_dev *p_dev)
{
    awbl_imx1050_fb_t         *p_this    = __DEV_TO_THIS(p_dev);
    awbl_imx1050_fb_devinfo_t *p_devinfo = __FB_DEVINFO_GET(p_this);
    awbl_imx1050_panel_t      *p_panel   = NULL;
    awbl_imx1050_tft_panel_devinfo_t * p_tft_devinfo = NULL;

    // TODO: use decide function
    p_panel = awbl_imx1050_lcdif_panel_get(p_devinfo->lcdif_info.default_panel_name,
                                           p_devinfo->lcdif_info.default_panel_unit);

    if (p_panel == NULL) {
        AW_ERRF(("can't find default panel %s %d !\n",
                p_devinfo->lcdif_info.default_panel_name,
                p_devinfo->lcdif_info.default_panel_unit));
        return;
    }

    p_tft_devinfo = __PANEL_DEVINFO_TO_THIS(p_panel->p_panel_info);

    AW_INFOF(("use panel: %s %d (%d x %d  %d bpp)\n",
            p_devinfo->lcdif_info.default_panel_name,
            p_devinfo->lcdif_info.default_panel_unit,
            p_panel->p_panel_info->x_res,
            p_panel->p_panel_info->y_res,
            p_panel->p_panel_info->bus_bpp));

    /* initialize lcdif & panel information */
    p_panel->p_lcdif         = &p_this->lcdif;
    p_this->lcdif.p_devinfo  = &p_devinfo->lcdif_info;
    p_this->lcdif.p_panel    = p_panel;

    /* initialize fb fix information */
    p_this->fb_info.fix_info.vram_addr   = VRAM_START;
    p_this->fb_info.fix_info.vram_max    = VRAM_END - VRAM_START + 1;

    /* initialize fb var information */
    p_this->fb_info.var_info.res.x_res          = p_panel->p_panel_info->x_res;
    p_this->fb_info.var_info.res.y_res          = p_panel->p_panel_info->y_res;
    p_this->fb_info.var_info.bpp.word_lbpp      = p_panel->p_panel_info->word_lbpp;
    p_this->fb_info.var_info.bpp.bus_bpp        = p_panel->p_panel_info->bus_bpp;

    p_this->fb_info.var_info.dclk_f.typ_dclk_f    = p_panel->p_panel_info->dclk_f;
    p_this->fb_info.var_info.hsync.h_active       = p_tft_devinfo->timing.h_active;
    p_this->fb_info.var_info.hsync.h_bporch       = p_tft_devinfo->timing.h_bporch;
    p_this->fb_info.var_info.hsync.h_fporch       = p_tft_devinfo->timing.h_fporch;
    p_this->fb_info.var_info.hsync.h_pulse_width  = p_tft_devinfo->timing.h_pulse_width;
    p_this->fb_info.var_info.vsync.v_active       = p_tft_devinfo->timing.v_active;
    p_this->fb_info.var_info.vsync.v_bporch       = p_tft_devinfo->timing.v_bporch;
    p_this->fb_info.var_info.vsync.v_fporch       = p_tft_devinfo->timing.v_fporch;
    p_this->fb_info.var_info.vsync.v_pulse_width  = p_tft_devinfo->timing.v_pulse_width;

    p_this->fb_info.var_info.buffer.buffer_num    = __AWBL_FB_BUFFER_NUM;
//    p_this->fb_info.var_info.buffer_size    = 0;

    p_this->fb_info.var_info.orientation    = p_panel->p_panel_info->dis_direction;


    p_this->fb_info.fb_status.online_buffer  = 0;
    p_this->fb_info.fb_status.offline_buffer = 1;

    p_this->fb_info.p_fb_ops = (const awbl_fb_ops_t *)&__g_fb_ops;
    p_this->fb_info.p_cookie = (void *)p_dev;

    /* 初始化信号量  */
    AW_SEMB_INIT(p_this->lcdif.lcd_sem_sync, 1, AW_SEM_Q_FIFO);
}

aw_local aw_err_t __imx1050_fb_get (struct awbl_dev      *p_dev,
                                    awbl_fb_info_t      **pp_fb)
{
    awbl_imx1050_fb_t *p_this = __DEV_TO_THIS(p_dev);

    // TODO: check panel

    *pp_fb = &p_this->fb_info;

    return AW_OK;
}

/*******************************************************************************
    driver register
*******************************************************************************/

/* driver functions */
aw_const struct awbl_drvfuncs __g_imx1050_fb_drvfuncs = {
    NULL,                          /* devInstanceInit */
    __imx1050_lcd_fb_inst_init2,   /* devInstanceInit2 */
    NULL                           /* devConnect */
};

/* methods for this driver */
AWBL_METHOD_IMPORT(awbl_fb_get);

aw_local aw_const struct awbl_dev_method __g_imx1050_fb_methods[] = {
    AWBL_METHOD(awbl_fb_get, __imx1050_fb_get),
    AWBL_METHOD_END
};

/**
 * supported devices:
 * imx1050 compatible fb devices
 *
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_imx1050_fb_drvinfo = {
    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_PLB,                 /* bus_id */
    AWBL_IMX1050_FB_NAME,           /* p_drvname */
    &__g_imx1050_fb_drvfuncs,       /* p_busfuncs */
    &__g_imx1050_fb_methods[0],     /* p_methods */
    NULL                            /* pfunc_drv_probe */
};

/**
 * \brief register imx1050 fb driver
 *
 * This routine registers the imx1050 fb driver and device
 * recognition data with the AWBus subsystem.
 */
void awbl_imx1050_fb_drv_register (void)
{
    awbl_drv_register(&__g_imx1050_fb_drvinfo);
}

/* end of file */
