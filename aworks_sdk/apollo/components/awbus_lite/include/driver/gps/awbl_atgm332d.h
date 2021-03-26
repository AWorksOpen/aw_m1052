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
 * \brief ATGM332D BDS/GNSSȫ������λ����ģ�������
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 20-09-03  vih, first implementation.
 * \endinternal
 */

#ifndef __AWBL_ATGM332D_H
#define __AWBL_ATGM332D_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbl_gnss.h"
#include "awbl_nmea_0183_decode.h"
#include "aw_delayed_work.h"
#include "aw_time.h"
#include "aw_sys_defer.h"

/** \brief   atgm332d ������ */
#define AWBL_ATGM332D_GNSS  "atgm332d_gnss"
/**
 * \brief   atgm332d GNSS �豸ʵ���ڲ�ʹ�õ����ò���
 */
struct awbl_atgm332d_dev_cfg_par {
    /** \brief ���ô���*/
    uint32_t                reserved;
    uint32_t                baud;

    /** \brief  ����ϵͳ����  */
    uint32_t                sys_mask;
};

/**
 * \brief  GNSS �豸ʵ����Ϣ
 */
typedef struct awbl_atgm332d_devinfo {

    struct awbl_gnss_com       com;

    /** \brief ����������Ϣ */
    struct awbl_gnss_servinfo  gnss_servinfo;

} awbl_atgm332d_devinfo_t;


/**
 * \brief  GNSS �豸ʵ��
 */
typedef struct awbl_atgm332d_dev {

    /** \brief �����豸 */
    struct awbl_dev        super;

    /** \brief �豸�����ò��� */
    struct awbl_atgm332d_dev_cfg_par cfg_par;

    /** \brief GNSS ���� */
    struct awbl_gnss_service gnss_serv;

    /** \brief ���һ����������ʱ�䣬��λms */
    uint32_t                data_rate;

    struct aw_defer_djob    djob;

} awbl_atgm332d_dev_t;

void awbl_atgm332d_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_ATGM332D_H */

/* end of file */

