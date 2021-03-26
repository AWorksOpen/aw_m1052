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
 * \brief CNT226 ˫ģBDS/GPS������
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-30  vih, first implementation.
 * \endinternal
 */

#ifndef __AWBL_CNT226_H
#define __AWBL_CNT226_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbl_gnss.h"
#include "awbl_nmea_0183_decode.h"
#include "aw_delayed_work.h"
#include "aw_time.h"
#include "aw_sys_defer.h"

/** \brief   cnt226 ������ */
#define AWBL_CNT226_GNSS  "cnt226_gnss"

/**
 * \brief   cnt226 GNSS �豸ʵ���ڲ�ʹ�õ����ò���
 */
struct awbl_cnt226_dev_cfg_par {
    /** \brief ���ô���*/
    int                     uart_num;
    uint32_t                reserved;
    uint32_t                baud;
    uint32_t                inProto;
    uint32_t                outProto;

    /** \brief  ��Ϣ���Ƶ������  */
    uint32_t                msg_type;
    char                    msg_id;
    uint32_t                msg_rate;

    /** \brief  ��λ����  */
    uint32_t                meas_rate;
    uint32_t                nav_rate;
    uint32_t                correction_mask;

    /** \brief  ��ʱ��������  */
    uint32_t                interval;
    uint32_t                length;
    uint32_t                flag;
    int                     ant_delay;
    int                     rf_delay;
    int                     usr_delay;

    /** \brief  �ⲿ�����¼�����  */
    uint32_t                enable;
    uint32_t                palarity;
    uint32_t                clock_sync;

    /** \brief  ����ϵͳ����  */
    uint32_t                sys_mask;

    /** \brief  ��̬����  */
    uint32_t                dyn_mask;
    uint32_t                dyn_model;
    uint32_t                static_hold_thresh;

};

/**
 * \brief  GNSS �豸ʵ����Ϣ
 */
typedef struct awbl_cnt226_devinfo {

    struct awbl_gnss_com       com;

    /** \brief ����������Ϣ */
    struct awbl_gnss_servinfo  gnss_servinfo;

    int rst_pin;
    int en_pin;

} awbl_cnt226_devinfo_t;


/**
 * \brief  GNSS �豸ʵ��
 */
typedef struct awbl_cnt226_dev {

    /** \brief �����豸 */
    struct awbl_dev        super;

    /** \brief �豸�����ò��� */
    struct awbl_cnt226_dev_cfg_par cfg_par;

    /** \brief GNSS ���� */
    struct awbl_gnss_service gnss_serv;

    /** \brief ���һ����������ʱ�䣬��λms */
    uint32_t                data_rate;

    struct aw_defer_djob    djob;

} awbl_cnt226_dev_t;


#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_CNT226_H */

/* end of file */

