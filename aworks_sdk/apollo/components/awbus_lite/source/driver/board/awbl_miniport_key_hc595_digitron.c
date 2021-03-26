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
 * \brief MINIPORT digitron + hc595 ģ������
 *
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-06-26  nwt, first implementation.
 * \endinternal
 */

#include "driver/board/awbl_miniport_key_hc595_digitron.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_gpio.h"
#include "aw_common.h"



#define __DEV_TO_MINIPORT_KEY_HC595_DIGITRON_DEV(p_dev)         \
          AW_CONTAINER_OF(p_dev, struct awbl_miniport_key_hc595_digitron_dev, dev)

#define __SCANNER_TO_MINIPORT_KEY_HC595_DIGITRON_DEV(p_scan)    \
          AW_CONTAINER_OF(p_scan, struct awbl_miniport_key_hc595_digitron_dev, scan_dev)

#define __DEV_GET_MINIPORT_KEY_HC595_DIGITRON_PAR(p_dev)   \
          ((void *)AWBL_DEVINFO_GET(p_dev))

#define __THIS_TO_MINIPORT_KEY_HC595_DIGITRON_DEV(p_this) \
          ((struct awbl_miniport_key_hc595_digitron_dev *)(p_this))

aw_local struct awbl_scan_cb_handler __g_miniport_key_hc595_digitron_cb_handler;

void __miniport_key_hc595_digitron_cb (void *p_arg)
{
    struct awbl_scanner_dev *p_scan = (struct awbl_scanner_dev *)p_arg;
    struct awbl_miniport_key_hc595_digitron_dev *p_miniport_key_hc595_digitron = \
                           __SCANNER_TO_MINIPORT_KEY_HC595_DIGITRON_DEV(p_scan);

    if (++p_miniport_key_hc595_digitron->hc595_digitron.digitron_dev.blink_cnt >= \
                p_miniport_key_hc595_digitron->hc595_digitron.digitron_dev.blink_cnt_max) {

        p_miniport_key_hc595_digitron->hc595_digitron.digitron_dev.blink_cnt = 0;
        p_miniport_key_hc595_digitron->hc595_digitron.digitron_dev.blinking  = \
           !p_miniport_key_hc595_digitron->hc595_digitron.digitron_dev.blinking;
    }

    aw_hc595_lock_enable(&p_miniport_key_hc595_digitron->gpio_hc595_dev.hc595_dev);

    /* ����λ�� */
    awbl_gpio_hc595_digitron_com_data_send(
        &p_miniport_key_hc595_digitron->hc595_digitron, 
         p_miniport_key_hc595_digitron->scan_dev.scan_index);

    /* ��ȡ��ֵ */
    awbl_gpio_hc595_digitron_black_cb(
        &p_miniport_key_hc595_digitron->hc595_digitron, 
         p_miniport_key_hc595_digitron->scan_dev.scan_index);

    /* ���Ͷ��� */
    awbl_gpio_hc595_digitron_seg_data_send(
        &p_miniport_key_hc595_digitron->hc595_digitron, 
         p_miniport_key_hc595_digitron->scan_dev.scan_index);

    aw_hc595_lock_disable(&p_miniport_key_hc595_digitron->gpio_hc595_dev.hc595_dev);
}


aw_local void __miniport_key_hc595_digitron_inst_init2 (struct awbl_dev *p_dev)
{
    struct awbl_miniport_key_hc595_digitron_dev   *p_miniport_key_hc595_digitron = \
                               __DEV_TO_MINIPORT_KEY_HC595_DIGITRON_DEV(p_dev);
    struct awbl_miniport_key_hc595_digitron_param *p_par = \
           __DEV_GET_MINIPORT_KEY_HC595_DIGITRON_PAR(&p_miniport_key_hc595_digitron->dev);

    /* ����ɨ���� */
    awbl_scanner_dev_ctor(&p_miniport_key_hc595_digitron->scan_dev, &p_par->scpar);

    /* ��ɨ������ӻص����� , ע��ص�������ע��˳��, ��ע����ִ�� */
    __g_miniport_key_hc595_digitron_cb_handler.pfn_scan_cb   = \
                                     __miniport_key_hc595_digitron_cb;

    awbl_scan_cb_list_add(&p_miniport_key_hc595_digitron->scan_dev, 
                          &__g_miniport_key_hc595_digitron_cb_handler);


    /* ����ɨ���� */
    awbl_scanner_start(&p_miniport_key_hc595_digitron->scan_dev);

    /* �������GPIO������hc595�豸 */
    awbl_gpio_hc595_dev_ctor(&p_miniport_key_hc595_digitron->gpio_hc595_dev, 
                             &p_par->gpio_hc595_par);

    /* ��ʼ��hc595���� */
    aw_hc595_pin_init(&p_miniport_key_hc595_digitron->gpio_hc595_dev.hc595_dev);

    /* ����hc595������豸(GPIO�ܽ���ΪCOM��) */
    awbl_gpio_hc595_digitron_dev_ctor(
        &p_miniport_key_hc595_digitron->hc595_digitron,
        &p_miniport_key_hc595_digitron->gpio_hc595_dev.hc595_dev,
        &p_par->hc_ddpar,
         p_miniport_key_hc595_digitron->dev.p_devhcf->unit);

    /* ��hc595������豸ע����������ϵͳ��ͬʱҲ������ͨ��������豸 */
    awbl_digitron_disp_register(
        &p_miniport_key_hc595_digitron->hc595_digitron.digitron_dev,
        &p_miniport_key_hc595_digitron->hc595_digitron.p_par->ddparam);

}



aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_miniport_key_hc595_digitron = {
    NULL,                                      /* devInstanceInit */
    __miniport_key_hc595_digitron_inst_init2,  /* devInstanceInit2 */
    NULL                                       /* devConnect */
};

aw_local aw_const struct awbl_drvinfo __g_drvinfo_miniport_key_hc595_digitron = {
    AWBL_VER_1,                                     /* awb_ver */
    AWBL_BUSID_PLB,                                 /* bus_id */
    MINIPORT_KEY_HC595_DIGITRON_NAME,               /* p_drvname */
    &__g_awbl_drvfuncs_miniport_key_hc595_digitron, /* p_busfuncs */
    NULL,                                           /* p_methods */
    NULL                                            /* pfunc_drv_probe */
};

void awbl_miniport_key_hc595_digitron_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_miniport_key_hc595_digitron);
}

/* end of file*/
