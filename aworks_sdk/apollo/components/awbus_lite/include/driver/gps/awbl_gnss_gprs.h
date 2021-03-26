/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ���� GPRS ģ��� GNSS ����
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 20-07-09  vih, first implementation.
 * \endinternal
 */

#ifndef __AWBL_GNSS_GPRS_H
#define __AWBL_GNSS_GPRS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_gnss.h"
#include "aw_gnss.h"
#include "aw_gprs.h"

#include "awbl_nmea_0183_decode.h"
#include "aw_delayed_work.h"
#include "aw_time.h"
#include "aw_sys_defer.h"

/** \brief  ������ */
#define AWBL_GNSS_GPRS  "gnss_gprs"

/**
 * \brief GNSS �豸״̬
 */
typedef enum {
    AWBL_GNSS_GPRS_STS_PWR_OFF = 0,
    AWBL_GNSS_GPRS_STS_PWR_OFF_PRE,
    AWBL_GNSS_GPRS_STS_PWR_ON,
    AWBL_GNSS_GPRS_STS_RUNNING,
} awbl_gnss_gprs_sts_t;

/**
 * \brief  GNSS �豸ʵ����Ϣ
 */
typedef struct awbl_gnss_gprs_devinfo {

    /** \brief ʹ�� gprs �豸�� id  */
    int                         gprs_dev_id;

    aw_gprs_gnss_cfg_t          gnss_cfg;

    /** \brief ����������Ϣ */
    struct awbl_gnss_servinfo   gnss_servinfo;

} awbl_gnss_gprs_devinfo_t;


/**
 * \brief  GNSS �豸ʵ��
 */
typedef struct awbl_gnss_gprs_dev {

    /** \brief �����豸 */
    struct awbl_dev          super;

    /** \brief GNSS ���� */
    struct awbl_gnss_service gnss_serv;

    /** \brief gprs ״̬��ֻ���� PWR_ON ʱ�ſ�ʹ�� gnss ���� */
    awbl_gnss_gprs_sts_t       gnss_sts;

    struct awbl_gnss_gprs_dev *p_next;
} awbl_gnss_gprs_dev_t;

void awbl_gnss_gprs_drv_register (void);

#endif

