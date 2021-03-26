/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-20126 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief  TFT屏显示面板通用驱动
 *
 * \internal
 * \par modification history:
 * - 1.01 19-04-09  cat, delete interface.
 * - 1.00 17-11-21  mex, first implemetation
 * \endinternal
 */


/*******************************************************************************
    includes
*******************************************************************************/
#include "aw_common.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "aw_delay.h"
#include "aw_intensity.h"
#include "aw_vdebug.h"
#include "aw_assert.h"
#include "aw_hwtimer.h"
#include "aw_int.h"
#include "aw_gpio.h"

#include "driver/display/awbl_imx1050_tft_panel.h"

/*******************************************************************************
    macros
*******************************************************************************/

#define __DEV_TO_THIS(p_dev) \
    AW_CONTAINER_OF(p_dev, awbl_imx1050_tft_panel_t, dev)

#define __DEVINFO_GET(p_dev) \
   (awbl_imx1050_tft_panel_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
    implementation
*******************************************************************************/

/**
 * \brief first level initialization routine of timers on chip
 *
 * \note  This is the function called to perform the first level initialization.
 */
aw_local void __imx1050_tft_panel_inst_init (struct awbl_dev *p_dev)
{
    awbl_imx1050_tft_panel_t         *p_this    = __DEV_TO_THIS(p_dev);
    awbl_imx1050_tft_panel_devinfo_t *p_devinfo = __DEVINFO_GET(p_this);

    /* platform initialize */
    if (p_devinfo->pfunc_plfm_init) {
        p_devinfo->pfunc_plfm_init();
    }
}

aw_local aw_err_t __imx1050_tft_panel_panel_get (struct awbl_dev     *p_dev,
                                            awbl_imx1050_panel_t   **pp_panel)
{
    awbl_imx1050_tft_panel_t         *p_this    = __DEV_TO_THIS(p_dev);
    awbl_imx1050_tft_panel_devinfo_t *p_devinfo = __DEVINFO_GET(p_this);
    awbl_imx1050_panel_t             *p_panel   = &p_this->panel;

    /* initialize panel information */
    p_panel->p_panel_info = &p_devinfo->panel_info;
    p_panel->p_lcdif      = NULL;  /* let fb fix it */
    p_panel->p_cookie     = (void *)p_this;

    /* retrieve the panel data */
    *pp_panel = p_panel;

    return AW_OK;
}

/*******************************************************************************
    driver register
*******************************************************************************/

/* driver functions */
aw_const struct awbl_drvfuncs __g_imx1050_tft_panel_drvfuncs = {
    __imx1050_tft_panel_inst_init,       /* devInstanceInit */
    NULL,                                /* devInstanceInit2 */
    NULL                                 /* devConnect */
};

AWBL_METHOD_IMPORT(awbl_imx1050_panel_get);

/* methods for this driver */
aw_local aw_const struct awbl_dev_method __g_imx1050_tft_panel_methods[] = {
    AWBL_METHOD(awbl_imx1050_panel_get, __imx1050_tft_panel_panel_get),
    AWBL_METHOD_END
};

/**
 * supported devices:
 * imx1050 compatible panel devices
 *
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_imx1050_tft_panel_drvinfo = {
    AWBL_VER_1,                          /* awb_ver */
    AWBL_BUSID_PLB,                      /* bus_id */
    AWBL_IMX1050_TFT_PANEL_NAME,         /* p_drvname */
    &__g_imx1050_tft_panel_drvfuncs,     /* p_busfuncs */
    &__g_imx1050_tft_panel_methods[0],   /* p_methods */
    NULL                                 /* pfunc_drv_probe */
};

/**
 * \brief register panel driver
 *
 * This routine registers the panel driver and device
 * recognition data with the AWBus subsystem.
 */
void awbl_imx1050_tft_panel_drv_register (void)
{
    awbl_drv_register(&__g_imx1050_tft_panel_drvinfo);
}

/* end of file */
