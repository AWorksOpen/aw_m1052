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
 * \brief AWBus-lite RTC DS1302 ����
 *
 * \internal
 * \par modification history:
 * - 1.00 18-03-07  vir, first implementation
 * \endinternal
 */

#ifndef __AWBL_DS1302_H
#define __AWBL_DS1302_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#include "awbl_rtc.h"
#include "aw_spinlock.h"
#include "awbl_spibus.h"


/** \brief DS1302 ������ */
#define AWBL_DS1302_NAME   "ds1302"

/**
 * \brief DS1302 �豸��Ϣ
 */
typedef struct awbl_ds1302_devinfo {

    /** \brief RTC ����������Ϣ */
    struct awbl_rtc_servinfo  rtc_servinfo;

    uint32_t   spi_speed;

    uint32_t   spi_cs_pin;

} awbl_ds1302_devinfo_t;

/**
 * \brief DS1302 �豸ʵ��
 */
typedef struct awbl_ds1302_dev {

    /** \brief �̳��� AWBus SPI device �豸 */
    struct awbl_spi_device super;

    /** \brief RTC ���� */
    struct awbl_rtc_service rtc_serv;

    /** \brief �豸�� */
    AW_MUTEX_DECL(devlock);
} awbl_ds1302_dev_t;

/**
 * \brief DS1302 driver register
 */
void awbl_ds1302_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_DS1302_H */

/* end of file */

