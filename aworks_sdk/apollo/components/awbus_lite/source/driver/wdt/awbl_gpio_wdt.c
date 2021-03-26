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
 * \brief TI UPP 简单驱动，目前只支持读
 *
 * \internal
 * \par modification history:
 * - 1.00 14-8-27  may, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "driver/wdt/awbl_gpio_wdt.h"
#include "aw_gpio.h"


/*******************************************************************************
  define
*******************************************************************************/
#define __GPIO_WDT_PARAM_DECL(p_param, p_dev) \
    struct awbl_gpio_wdt_param *p_param = \
        (struct awbl_gpio_wdt_param *)AWBL_DEVINFO_GET(p_dev)

#define  __INFO_TO_GPIO_WDT(p_info) \
    AW_CONTAINER_OF(p_info, struct awbl_gpio_wdt_dev, info)


/******************************************************************************/

/** \brief watchdog feed */
aw_local void  __gpio_wdt_feed (struct awbl_hwwdt_info *pwdt_info)
{
    struct awbl_gpio_wdt_dev *p_dev = __INFO_TO_GPIO_WDT(pwdt_info);

    aw_gpio_toggle(p_dev->gpio_num);
}

/******************************************************************************/

/** instance initializing stage 1 */
aw_local void  __gpio_wdt_inst_init (struct awbl_dev *p_dev)
{
    __GPIO_WDT_PARAM_DECL(p_param, p_dev);
    struct awbl_gpio_wdt_dev  *p_wdt_dev = (struct awbl_gpio_wdt_dev *)p_dev;

    if (p_param->pfunc_plfm_init != NULL) {
        (p_param->pfunc_plfm_init)();
    }

    p_wdt_dev->info.wdt_time = p_param->wdt_time;
    p_wdt_dev->info.feed     = __gpio_wdt_feed;
    p_wdt_dev->gpio_num      = p_param->gpio_num;

    aw_gpio_pin_cfg(p_param->gpio_num, AW_GPIO_OUTPUT);

    /* feed watchdog */
    aw_gpio_toggle(p_param->gpio_num);

    awbl_wdt_device_register(&p_wdt_dev->info);
}


/*******************************************************************************
  locals
*******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_gpio_wdt = {
    __gpio_wdt_inst_init,    /* devInstanceInit */
    NULL,                    /* devInstanceInit2 */
    NULL                     /* devConnect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_gpio_wdt = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    GPIO_WDT_NAME ,                     /* p_drvname */
    &__g_awbl_drvfuncs_gpio_wdt,        /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

/******************************************************************************/
void  awbl_gpio_wdt_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_gpio_wdt);
}

/* end of file */
