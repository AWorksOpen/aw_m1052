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
 * \brief AWBus Ti C6000 ϵ��DSP ͨ���жϿ���������
 *
 * ����������BIOS6.3����Ҫ���������ṩ��4��ISR
 * (g_awbl_tic6000_intc_isr0 ~ g_awbl_tic6000_intc_isr3) �ֱ����ӵ�BIOS TCONF ��
 * HWI_INT4 ~ HWI_INT7��
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "tic6000_intc"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_tic6000_intc_devinfo
 *
 * \par 2.�����豸
 *
 *  - Ti C674x ϵ��DSP
 *  - ���������豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_tic6000_intc_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_int
 *
 * \par 5.ʵ�����
 *
 *  - \ref grp_awbl_tic6000_intc_impl
 *
 * \internal
 * \par modification history:
 * - 1.00 14-03-06  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_TIC6000_INTC_H
#define __AWBL_TIC6000_INTC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/**
\addtogroup grp_awbl_drv_tic6000_intc
\copydetails awbl_tic6000_intc.h
@{
*/

/**
 * \defgroup grp_awbl_tic6000_intc_impl  ʵ�����
 *
 * \par 1. ����
 *
 *  - ��ʵ��ֻ�ṩ�������жϵĿ���
 *  - ÿ���ж�ֻ������1���жϷ�����
 *  - ���ṩ�ж����ȼ�������
 */

/**
 * \addtogroup grp_awbl_tic6000_intc_hwconf  �豸����/����
 *
 * ARM NVIC Ӳ���豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \include  hwconf_usrcfg_nvic_armcm.c
 *
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1�� �жϸ��� \n
 *      ��ο�MCU���û��ֲᣬ��дNVICʵ��֧�ֵ��жϸ�����
 *
 * - ��2�� ISR��Ϣ�������û����á� \n
 *      �˲��������˵��� aw_int_connect() ����ISR(�жϷ�����)����NVIC������
 *      ��(����)��������ӵ�ISR��������������Ķ��壬�򽫷��ش���ֱ����ISR����
 *      ��(���ú��� aw_int_disconnect())��MCU�ڴ���Դ����ʱ���û����Խ��˲�����
 *      ��Ϊʵ����Ҫ���ӵ�ISR�������Խ�Լ�ڴ�(1��ISR��Ϣռ��8���ֽ�)��
 *      \note �Ƽ����˲�����Ϊ"�û�����"���� aw_prj_params.h �����ã�����:
 *      \code
 *      #define AW_CFG_ARMCM_NVIC_ISRINFO_COUNT     20
 *      \endcode
 *
 * - ��3����4�� �жϺŷ�Χ \n
 *     ��3����4���ֱ�����ʼ�жϺźͽ����жϺ�,����ϵͳ���жϺŷ��䲻�����ص���
 *      ʹ��Apollo��׼�жϽӿ�ʱ������˷�Χ���жϺţ�����Զ�λ�����ﶨ���
 *      NVIC�豸��ϵͳ���жϺŷ�����ο� \ref grp_aw_plfm_inums ��
 */

/** @} grp_awbl_drv_tic6000_intc */

#include "awbl_intctlr.h"
#include "aw_spinlock.h"

/** \brief ������ */
#define AWBL_TIC6000_INTC_NAME   "tic6000_intc"

/**
 * \brief INTC �豸��Ϣ
 */
typedef struct awbl_tic6000_intc_devinfo {

    /** \brief IntCtlr ���������Ϣ */
    struct awbl_intctlr_servinfo intctlr_servinfo;

    /** \brief Event Combiner 0-3 �жϺ� (4~15) */
    int  ecm_intnum[4];

} awbl_tic6000_intc_devinfo_t;

/**
 * \brief NVIC �豸ʵ��
 */
typedef struct awbl_tic6000_intc_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev super;

    /** \brief IntCtlr ���� */
    struct awbl_intctlr_service intctlr_serv;

    /** \brief �豸�� */
    aw_spinlock_isr_t devlock;

} awbl_tic6000_intc_dev_t;

/**
 * \brief tic6000 intc driver register
 */
void awbl_tic6000_intc_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_TIC6000_INTC_H */

/* end of file */
