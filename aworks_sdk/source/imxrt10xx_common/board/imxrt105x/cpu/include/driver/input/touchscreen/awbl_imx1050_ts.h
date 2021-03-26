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
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief iMX RT1050 ����������������ͷ�ļ�
 *
 * \internal
 * \par modification history:
 * - 1.00  17-11-21  mex, first implementation.
 * \endinternal
 */
 
#ifndef __AWBL_IMX1050_TS_H
#define __AWBL_IMX1050_TS_H

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_input.h"
#include "awbl_ts.h"
#include "aw_delayed_work.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief iMX RT1050 ������������ */
#define AWBL_IMX1050_TOUCHSCREEN_NAME        "imx1050_touch_screen"

/** \brief iMX RT1050 ���������豸��Ϣ */
typedef struct awbl_imx1050_ts_devinfo {
    uint32_t            regbase;        /**< \brief TSC�Ĵ�������ַ */
    uint8_t             x_plus_ch;      /**< \brief X����ADCͨ�� */
    uint8_t             x_minus_ch;     /**< \brief X����ADCͨ�� */
    uint8_t             y_plus_ch;      /**< \brief Y����ADCͨ�� */
    uint8_t             y_minus_ch;     /**< \brief Y����ADCͨ�� */

    awbl_ts_service_info_t ts_serv_info;
    void (*pfunc_plfm_init)(void);      /**< \brief ƽ̨��س�ʼ�� */

} awbl_imx1050_ts_devinfo_t;

/** \brief imx1050 ���������豸 */
typedef struct awbl_imx1050_ts_dev {

    /** \bried �̳���Touch SCreen�豸 */
    struct awbl_ts_service ts_serv;

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev adev;

    /** \brief �������Ƿ��� */
    aw_bool_t is_pressed;
    
    /** \brief ��һ�°��±�־ */
    uint8_t is_debounce;

}awbl_imx1050_ts_dev_t;

/**
 * \brief iMX RT1050 ��������ע�ắ��
 */
void awbl_imx1050_touch_screen_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX1050_TS_H */

/* end of file */
