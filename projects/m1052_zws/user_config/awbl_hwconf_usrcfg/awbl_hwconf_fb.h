/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_FB_H
#define __AWBL_HWCONF_FB_H

#ifdef AW_DEV_FB

/*******************************************************************************
  HW480272 配置信息
*******************************************************************************/
#include "driver/display/awbl_imx1050_fb.h"


/**
 * \brief 当需要使用PWM控制背光时，PWM_ID需指定为对应的PWM号，
 */
#define  PWM_ID     PWM12

const static aw_clk_id_t fb_clks[2] = {

        IMX1050_CLK_CG_LCD,
        IMX1050_CLK_CG_LCDIF_PIX
};


/* 设备信息 */
aw_local aw_const awbl_imx1050_fb_devinfo_t  __g_fb_param = {

    /* lcdif_info */
    {
         "tft_panel",              /** \brief 默认面板名 */
         0,                        /** \brief 默认面板单元号 */
         PWM_ID,                   /** \brief 默认背光控制的PWM号 */
         -1,                       /** \brief 默认背光控制的GPIO管脚 */
         NULL,                     /** \brief 决定使用哪个面板 (为NULL或失败时，使用默认面板) */
         IMX1050_LCDIF_BASE,       /** \brief LCDIF寄存器基地址 */
         INUM_LCDIF,               /** \brief LCD中断号 */
         fb_clks,
         2,
         IMX1050_CLK_CG_LCDIF_PIX
    }
};

/* 设备实例内存静态分配 */
aw_local awbl_imx1050_fb_t __g_fb;

#define AWBL_HWCONF_FB              \
    {                               \
        AWBL_IMX1050_FB_NAME,       \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_fb.dev),              \
        &(__g_fb_param)             \
    },

#else
#define AWBL_HWCONF_FB

#endif  /* AW_DEV_FB */

#endif  /* __AWBL_HWCONF_EMWIN_FB_H */

/* end of file */
