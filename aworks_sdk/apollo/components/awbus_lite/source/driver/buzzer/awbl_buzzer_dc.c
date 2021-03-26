/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief driver for DC-Buzzer driven by GPIO.
 *
 * \internal
 * \par modification history:
 * - 1.00 13-03-08  orz, first implementation.
 * \endinternal
 */
#include "driver/buzzer/awbl_buzzer_dc.h"
#include "awbl_driver_if.h"
#include "aw_compiler.h"
#include "aw_gpio.h"

/** \brief dc-buzzer loud set */
static aw_err_t __dc_buzzer_loud_set (struct awbl_buzzer_dev *p_dev, int level)
{
    (void)p_dev;
    (void)level;

    return AW_OK;
}

/** \brief dc-buzzer beep */
static void __dc_buzzer_beep (struct awbl_buzzer_dev *dev, aw_bool_t on)
{
    struct dc_buzzer *db = buzzer_to_dc_buzzer(dev);

    (void)aw_gpio_set(db->gpio, on ^ db->active_low);
}

/******************************************************************************/

/** instance initializing stage 1 */
static void __dc_buzzer_inst_init (struct awbl_dev *p_dev)
{
    struct dc_buzzer       *db  = dev_to_dc_buzzer(p_dev);
    struct dc_buzzer_param *par = dev_get_dc_buzzer_par(p_dev);

    if (par->pfn_plfm_init != NULL) {
        par->pfn_plfm_init();
    }

    db->gpio       = par->gpio;
    db->active_low = par->active_low;
    db->bdev.beep  = __dc_buzzer_beep;
    db->bdev.loud_set  = __dc_buzzer_loud_set;

    (void)aw_gpio_pin_cfg(db->gpio, AW_GPIO_OUTPUT);
    (void)aw_gpio_set(db->gpio, ((db->active_low ) ? 1 : 0));
    (void)awbl_buzzer_register_device(&db->bdev);
}

/******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_dc_buzzer = {
    __dc_buzzer_inst_init,      /**< \brief devInstanceInit */
    NULL,                       /**< \brief devInstanceInit2 */
    NULL                        /**< \brief devConnect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_dc_buzzer = {
    AWBL_VER_1,                         /**< \brief awb_ver */
    AWBL_BUSID_PLB,                     /**< \brief bus_id */
    DC_BUZZER_NAME,                     /**< \brief p_drvname */
    &__g_awbl_drvfuncs_dc_buzzer,       /**< \brief p_busfuncs */
    NULL,                               /**< \brief p_methods */
    NULL                                /**< \brief pfunc_drv_probe */
};

/** \brief register DC-Buzzer driver */
void awbl_dc_buzzer_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_dc_buzzer);
}

/* end of file */
