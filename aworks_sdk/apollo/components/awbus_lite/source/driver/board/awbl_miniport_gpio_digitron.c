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
 * \brief MINIPORT 数码管模块GPIO驱动
 *
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-19  ops, first implementation.
 * \endinternal
 */

#include "driver/board/awbl_miniport_gpio_digitron.h"
#include "aw_vdebug.h"
#include "aw_delay.h"

#define __DEV_TO_MINIPORT_DIGITRON_DEV(p_dev)         \
            AW_CONTAINER_OF(p_dev, struct awbl_miniport_digitron_dev, dev)

#define __SCANNER_TO_MINIPORT_DIGITRON_DEV(p_scan)    \
            AW_CONTAINER_OF(p_scan, struct awbl_miniport_digitron_dev, scan_dev)

#define __DEV_GET_MINIPORT_DIGITRON_PAR(p_dev)   ((void *)AWBL_DEVINFO_GET(p_dev))

aw_local struct awbl_scan_cb_handler __g_miniport_gpio_digitron_cb_handler;

aw_local void __miniport_gpio_digitron_digitron_cb (void *p_arg)
{
    struct awbl_scanner_dev *p_scan = (struct awbl_scanner_dev *)p_arg;
    struct awbl_miniport_digitron_dev *p_miniport_digitron   = \
                                __SCANNER_TO_MINIPORT_DIGITRON_DEV(p_scan);

    if (++p_miniport_digitron->gpio_digitron.digitron_dev.blink_cnt >= \
                p_miniport_digitron->gpio_digitron.digitron_dev.blink_cnt_max) {

        p_miniport_digitron->gpio_digitron.digitron_dev.blink_cnt = 0;
        p_miniport_digitron->gpio_digitron.digitron_dev.blinking  = \
                    !p_miniport_digitron->gpio_digitron.digitron_dev.blinking;
    }

    awbl_gpio_digitron_com_data_send(&p_miniport_digitron->gpio_digitron, 
                                      p_miniport_digitron->scan_dev.scan_index);

    awbl_gpio_digitron_black_cb(&p_miniport_digitron->gpio_digitron, 
                                 p_miniport_digitron->scan_dev.scan_index);

    awbl_gpio_digitron_seg_data_send(&p_miniport_digitron->gpio_digitron, 
                                      p_miniport_digitron->scan_dev.scan_index);
}


aw_local void __miniport_gpio_digitron_inst_init2 (struct awbl_dev *p_dev)
{
    struct awbl_miniport_digitron_dev   *p_miniport_digitron  = \
                                        __DEV_TO_MINIPORT_DIGITRON_DEV(p_dev);

    struct awbl_miniport_digitron_param *p_par = \
                      __DEV_GET_MINIPORT_DIGITRON_PAR(&p_miniport_digitron->dev);

    /* 构造扫描器 */
    awbl_scanner_dev_ctor(&p_miniport_digitron->scan_dev, &p_par->scpar);

    /* 向扫描器添加回调函数 , 注意回调函数的注册顺序, 先注册先执行 */
    __g_miniport_gpio_digitron_cb_handler.pfn_scan_cb   = \
                            __miniport_gpio_digitron_digitron_cb;

    awbl_scan_cb_list_add(&p_miniport_digitron->scan_dev, 
                          &__g_miniport_gpio_digitron_cb_handler);

    /* 启动扫描器 */
    awbl_scanner_start(&p_miniport_digitron->scan_dev);

    /* 构造miniport数码管设备 */
    awbl_gpio_digitron_dev_ctor(&p_miniport_digitron->gpio_digitron,
                                &p_par->gpio_ddpar,
                                 p_miniport_digitron->dev.p_devhcf->unit);

    /* 把miniport数码管设备设备注册进数码管子系统, 构造通用数码管 */
    awbl_digitron_disp_register(&p_miniport_digitron->gpio_digitron.digitron_dev,
                                &p_miniport_digitron->gpio_digitron.p_par->ddparam);

}

aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_miniport_gpio_digitron = {
    NULL,                                  /* devInstanceInit */
    __miniport_gpio_digitron_inst_init2,   /* devInstanceInit2 */
    NULL                                   /* devConnect */
};

aw_local aw_const struct awbl_drvinfo __g_drvinfo_miniport_gpio_digitron = {
    AWBL_VER_1,                                /* awb_ver */
    AWBL_BUSID_PLB,                            /* bus_id */
    MINIPORT_GPIO_DIGITRON_NAME,               /* p_drvname */
    &__g_awbl_drvfuncs_miniport_gpio_digitron, /* p_busfuncs */
    NULL,                                      /* p_methods */
    NULL                                       /* pfunc_drv_probe */
};

void awbl_miniport_digitron_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_miniport_gpio_digitron);
}

/* end of file*/
