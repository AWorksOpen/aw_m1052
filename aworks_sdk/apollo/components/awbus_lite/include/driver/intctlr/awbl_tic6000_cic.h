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
 * \brief AWBus Ti C6000 ϵ��DSP ͨ��CIC����������
 *
 * C6XXX SOC �ж�ϵͳ�ṹ:
 *__________________________________________________________________________
 *|                   Peripheral                     |     CorePac          |
 *|__________________________________________________|______________________|
 *|Peripheral |         CIC                          | EventCombiner| Core  |
 *|  Rise ----|->SysInt--(map)->HostInt -(fixed-map)-|-> Event -----|-> HWI |
 *|___________|______________________________________|______________|_______|
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "tic6000_cic"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_tic6000_cic_devinfo
 *
 * \par 2.�����豸
 *
 *  - Ti C66xx ϵ��DSP
 *  - ���������豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_tic6000_cic_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_int
 *
 * \par 5.ʵ�����
 *
 *  - \ref grp_awbl_tic6000_cic_impl
 *
 * \internal
 * \par modification history:
 * - 1.00 14-03-06  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_TIC6000_CIC_H
#define __AWBL_TIC6000_CIC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_intctlr.h"
#include "aw_spinlock.h"

/** \brief ������ */
#define AWBL_TIC6000_CIC_NAME   "tic6000_cic"

/** \breif HostInt ӳ����Ϣ */
typedef struct awbl_tic6000_cic_hostint_map {

    /** \brief HostInt */
    int hostint;

    /** \brief Event ID */
    int event_id;
} awbl_tic6000_cic_hostint_map_t;

/** \brief �豸��Ϣ */
typedef struct awbl_tic6000_cic_devinfo {

    /** \brief IntCtlr ���������Ϣ */
    struct awbl_intctlr_servinfo intctlr_servinfo;

    /** \brief HostInt-EventId ӳ��� */
    const struct awbl_tic6000_cic_hostint_map *p_hostint_event_map;

    /** \brief HostInt-EventId ӳ����Ա�� */
    size_t hostint_event_map_nelements;

    /** \brief SysInt-HostInt ӳ��� (��Ա0��SysInt0 ӳ��� HostInt...) */
    const int *p_sysint_hostint_map;

    /** \brief SysInt-HostInt ӳ����Ա�� */
    size_t sysint_hostint_map_nelements;

    /**
     *  \brief �ܷ�(�ж�)�¼�·�ɵ�CPU��
     * ֻ��Ϊtrue��������ISR (aw_int_connect())
     */
    bool can_route_to_cpu;

} awbl_tic6000_cic_devinfo_t;

/** \brief �豸ʵ�� */
typedef struct awbl_tic6000_cic_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev super;

    /** \brief IntCtlr ���� */
    struct awbl_intctlr_service intctlr_serv;

    /** \brief �豸�� */
    aw_spinlock_isr_t devlock;

} awbl_tic6000_cic_dev_t;

/** \brief tic6000 intc driver register */
void awbl_tic6000_cic_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_TIC6000_CIC_H */

/* end of file */
