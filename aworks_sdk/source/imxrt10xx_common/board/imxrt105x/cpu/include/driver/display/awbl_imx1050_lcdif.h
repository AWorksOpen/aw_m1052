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
 * \brief Freescale iMX RT1050 LCD控制器驱动
 *
 * \internal
 * \par modification history:
 * - 1.01  19-05-06  cat, mod interface.
 * - 1.00  17-11-21  mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX1050_LCDIF_H
#define __AWBL_IMX1050_LCDIF_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_spinlock.h"
#include "driver/intensity/awbl_pwm_it.h"
#include "driver/display/awbl_imx1050_lcdif_regs.h"
#include "aw_fb.h"
#include "aw_clk.h"
#include "aw_sem.h"

#define AWBL_MXS_LCDIF_PANEL_SYSTEM      0
#define AWBL_MXS_LCDIF_PANEL_VSYNC       1
#define AWBL_MXS_LCDIF_PANEL_DOTCLK      2
#define AWBL_MXS_LCDIF_PANEL_DVI         3

#define AWBL_FB_HORIZONTAL_SCREEN        0    /* 横屏 */
#define AWBL_FB_VERTICAL_SCREEN          1    /* 竖屏 */

/** \brief 获取面板设备的名字和单元号 */
typedef aw_err_t (* awbl_imx1050_lcdif_obtain_panel_t) (const char **pp_name, int *p_unit);

/** \brief LCDIF设备信息 */
typedef struct awbl_imx1050_lcdif_devinfo {

    const char          *default_panel_name; /**< \brief 默认面板名 */
    int                  default_panel_unit; /**< \brief 默认面板单元号 */
    int                  default_pwm_num;    /**< \brief 默认背光控制的PWM号 */
    int                  gpio_backlight;     /**< \brief GPIO管脚背光控制 */

    /** \brief 决定使用哪个面板 (为NULL或失败时，使用默认面板) */
    aw_err_t           (*pfunc_decide_panel) (const char **pp_name, int *p_unit);
    uint32_t            regbase;             /**< \brief LCDIF寄存器基地址 */
    int                 inum;                /**< \brief LCD中断号 */
    const aw_clk_id_t  *dev_clks;            /**< \brief the clocks which the timer need */
    int                 dev_clk_count;       /**< \brief count of device clocks */
    aw_clk_id_t         ref_pixel_clk_id;    /**< \brief pixel clocks */

} awbl_imx1050_lcdif_devinfo_t;


struct awbl_imx1050_panel;

/** \brief LCDIF设备 */
typedef struct awbl_imx1050_lcdif {

    awbl_dev_t dev;                          /** \brief 继承自AWBus */
    awbl_imx1050_lcdif_devinfo_t *p_devinfo; /** \brief LCDIF 设备信息 */
    struct awbl_imx1050_panel    *p_panel;   /** \brief 使用的面板 */

    AW_SEMB_DECL(lcd_sem_sync);              /** \brief 用于帧同步的信号量 */

} awbl_imx1050_lcdif_t;

typedef struct awbl_imx1050_dotclk_timing {

    uint16_t    h_pulse_width;  /** \brief 行脉冲宽度 */
    uint16_t    h_active;       /** \brief 一行的像素个数 */
    uint16_t    h_bporch;       /** \brief 水平前沿 */
    uint16_t    h_fporch;       /** \brief 水平后沿 */
    uint16_t    v_pulse_width;  /** \brief 垂直脉冲宽度 */
    uint16_t    v_active;       /** \brief 一帧画面的行数 */
    uint16_t    v_bporch;       /** \brief 垂直前沿 */
    uint16_t    v_fporch;       /** \brief 垂直后沿 */
    int         enable_present;

} awbl_imx1050_dotclk_timing_t;

/** \brief panel information */
typedef struct awbl_imx1050_panel_info {

    uint16_t x_res;          /** \brief 屏幕宽度 */
    uint16_t y_res;          /** \brief 屏幕高度 */
    uint16_t bus_bpp;        /** \brief rgb硬件总线数 */
    uint16_t word_lbpp;      /** \brief 一个像素的长度，位为单位,如rgb565数据，该值为16 */
    uint32_t dclk_f;         /** \brief 时钟频率 */
    int32_t  lcd_type;       /** \brief 时钟源类型 */
    int32_t  bl_percent;     /** \brief 默认的背光亮度 */
    int32_t  dis_direction;  /** \brief 屏幕方向 */

} awbl_imx1050_panel_info_t;


struct awbl_imx1050_panel;

typedef struct awbl_imx1050_panel {

    awbl_imx1050_lcdif_t            *p_lcdif;
    awbl_imx1050_panel_info_t       *p_panel_info;
    void                            *p_cookie;

} awbl_imx1050_panel_t;

aw_err_t awbl_imx1050_lcdif_init(awbl_imx1050_lcdif_t *p_lcdif, uint8_t bus_bpp, uint8_t word_lbpp);
aw_err_t awbl_imx1050_lcdif_run(awbl_imx1050_lcdif_t *p_lcdif);
aw_err_t awbl_imx1050_lcdif_stop(awbl_imx1050_lcdif_t *p_lcdif);
aw_err_t awbl_imx1050_lcdif_pan_triple_vram_addr_set(awbl_imx1050_lcdif_t *p_lcdif, uint32_t addr);
aw_err_t awbl_imx1050_lcdif_pan_common_vram_addr_set(awbl_imx1050_lcdif_t *p_lcdif, uint32_t addr);
aw_err_t awbl_imx1050_lcdif_pan_try_vram_addr_set(awbl_imx1050_lcdif_t *p_lcdif, uint32_t addr);
aw_err_t awbl_imx1050_lcdif_pan_blank(awbl_imx1050_lcdif_t *p_lcdif, int blank);

aw_err_t awbl_imx1050_lcdif_dma_init(awbl_imx1050_lcdif_t *p_lcdif, uint32_t phys, size_t memsize);
aw_err_t awbl_imx1050_lcdif_dma_release(awbl_imx1050_lcdif_t *p_lcdif);

aw_err_t awbl_imx1050_lcdif_setup_dotclk_panel (awbl_imx1050_lcdif_t *p_lcdif, aw_fb_var_info_t *p_vinfo);
aw_err_t awbl_imx1050_lcdif_release_dotclk_panel (awbl_imx1050_lcdif_t *p_lcdif);

aw_err_t __imx_lcdif_common_buf_isr(void *p_dev);
aw_err_t __imx_lcdif_triple_buf_isr(void *p_dev);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_IMX1050_LCDIF_H */

/* end of file */

