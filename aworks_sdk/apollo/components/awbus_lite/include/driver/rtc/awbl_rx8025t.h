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
 * \brief AWBus-lite RTC RX8025T ����
 *
 * \internal
 * \par modification history:
 * - 1.00 18-03-14  vir, first implementation
 * \endinternal
 */

#ifndef __AWBL_RX8025T_H
#define __AWBL_RX8025T_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#include "awbl_rtc.h"
#include "awbl_i2cbus.h"

/** \brief RX8025T ������ */
#define AWBL_RX8025T_NAME   "rx8025t"

#define AWBL_RX8025T_YEAR_OFFS 100

/**
 * \brief RX8025T �豸��Ϣ
 */
typedef struct awbl_rx8025t_devinfo {

    /** \brief RTC ����������Ϣ */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief �豸�ӻ���ַ */
    uint8_t addr;

} awbl_rx8025t_devinfo_t;

/**
 * \brief RX8025T �豸ʵ��
 */
typedef struct awbl_rx8025t_dev {

    /** \brief �̳��� AWBus I2C device �豸 */
    struct awbl_i2c_device super;

    /** \brief RTC ���� */
    struct awbl_rtc_service rtc_serv;

    /** \brief �豸�� */
    AW_MUTEX_DECL(devlock);
} awbl_rx8025t_dev_t;

/**
 * \brief RX8025T driver register
 */
void awbl_rx8025t_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_RX8025T_H */

/* end of file */

