/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT1064 ʱ���
 *
 * ������ʹ��systickʵ��ʱ���
 *
 * \internal
 * \par modification history
 * - 1.00 20-04-04  wk, first implementation
 * \endinternal
 */


#ifndef __AWBL_IMX1010_TIMESTAMP_H_
#define __AWBL_IMX1010_TIMESTAMP_H_

#define AWBL_IMX1010_TIMESTAMP_NAME     "awbl_imx1010_timestamp"

#include "aw_timer.h"
#include "aw_sem.h"

#define MAX_TIMESTAMP_FREQUENCY		1000000

/** \brief �豸������Ϣ */
typedef struct awbl_imx1010_timestamp_devinfo {
    uint32_t        update_period;      /**< \brief ���¼���������ʱ�䣨ms��. */
    uint32_t        freq;               /**< \brief �Զ����ʱ��Ƶ��. */

    /** \brief ָ��ƽ̨��ʼ��������ָ�� */
    void             (*pfn_plfm_init) (void);
} awbl_imx1010_timestamp_devinfo_t;

/** \brief �豸ʵ�� */
typedef struct awbl_imx1010_timestamp_dev {
    awbl_dev_t      dev;                /**< \brief AWBus �豸ʵ���ṹ�� */

    aw_timer_t      p_timer;            /**< \brief �����ʱ��. */

} awbl_imx1010_timestamp_dev_t;

/**
 * \brief �� IMX10xx PIT ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note ������Ӧ�� aw_prj_early_init() �г�ʼ�����������н�ֹ���ò���ϵͳ��غ���
 */
void awbl_imx1010_timestamp_drv_register (void);


#endif /* __AWBL_IMX1010_TIMESTAMP_H_ */
