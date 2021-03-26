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
 * \brief AWBus ARM Cortex-M ϵ��CPU ͨ��NVIC����
 *
 * NVIC(Ƕ�������жϿ�����)�� ARM Cortex-M ϵ�д�������һ���֡���CPU�Ľ������
 * ������͵��ж��ӳ��Լ��Գٵ��жϵ���Ч����
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "armcm_nvic"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_armcm_nvic_devinfo
 *
 * \par 2.�����豸
 *
 *  - Cortex-M0 CPU
 *  - Cortex-M1 CPU
 *  - Cortex-M3 CPU
 *  - Cortex-M4 CPU
 *  - ���������豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_armcm_nvic_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_gpio
 *
 * \par 5.ʵ�����
 *
 *  - \ref grp_awbl_armcm_nvic_impl
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-06  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_ARMCM_NVIC_H
#define __AWBL_ARMCM_NVIC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/**
\addtogroup grp_awbl_drv_armcm_nvic
\copydetails awbl_armcm_nvic.h
@{
*/

/**
 * \defgroup grp_awbl_armcm_nvic_impl  ʵ�����
 *
 * \par 1. ����
 *
 *  - ��ʵ��ֻ�ṩ�������жϵĿ��ƣ�NVIC ���쳣�� \ref grp_aw_serv_exc ����
 *  - ÿ���ж�ֻ������1���жϷ�����
 *  - ���ṩ�ж����ȼ�������
 */

/**
 * \addtogroup grp_awbl_armcm_nvic_hwconf  �豸����/����
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

/** @} grp_awbl_drv_armcm_nvic */

#include "awbl_intctlr.h"
#include "aw_spinlock.h"

#define AWBL_ARMCM_NVIC_NAME   "armcm_nvic"

/**
 * \brief NVIC �豸��Ϣ
 */
typedef struct awbl_armcm_nvic_devinfo {

    /** \brief IntCtlr ���������Ϣ */
    struct awbl_intctlr_servinfo    intctlr_servinfo;

    /** \brief ISR ��Ϣ���� */
    int                             isrinfo_cnt;
    /** \brief ISR ��Ϣ���ڴ� (��С���� isrinfo_cnt) */
    struct awbl_armcm_nvic_isrinfo *p_isrinfo;

    /** \brief �ڴ��쳣���������ַ�������Ӧ�õ���isrinfo_cnt + �����쳣���� */
    /** \brief �����֧��ֱ���޸��쳣�������˵�ַ����ΪNULL*/
    uint32_t                       *p_ram_vector_base;
    /** \brief �ڴ��쳣���������һ��Ԫ�صĵ�ַ */
    uint32_t                       *p_ram_vector_end;
} awbl_armcm_nvic_devinfo_t;

/**
 * \brief ISR information
 */
struct awbl_armcm_nvic_isrinfo {

    /** \brief trigger callback function */
    aw_pfuncvoid_t  pfunc_isr;

    /** \brief argument for\a  pfunc_callback */
    void *p_arg;
};

/**
 * \brief NVIC �豸ʵ��
 */
typedef struct awbl_armcm_nvic_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev super;

    /** \brief IntCtlr ���� */
    struct awbl_intctlr_service intctlr_serv;

} awbl_armcm_nvic_dev_t;

/**
 * \brief NVIC driver register
 */
void awbl_armcm_nvic_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_ARMCM_NVIC_H */

/* end of file */
