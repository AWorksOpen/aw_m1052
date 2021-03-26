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
 * \brief NTC ������������
 *
 * �����������ڵ����ѹʽ������·��NTC��������������
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "ntc-nameless"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_ntc_therm_par
 *
 * \par 2.�����豸
 *
 *  - �κ�ʹ�÷�ѹ��·���е�ѹ������NTC��������
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_thermistor_ntc_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_ntc_thermistor
 *
 * \internal
 * \par modification history
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#ifndef __AWBL_THERM_NTC_H
#define __AWBL_THERM_NTC_H

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_types.h"

#include "awbl_temp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_ntc_thermistor
 * @{
 */

/**
 * \defgroup  grp_awbl_ntc_thermistor_hwconf �豸����/����
 *
 * ͨ�����������豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \note �ж��ٸ����������豸���ͽ�����Ĵ���Ƕ����ٴΣ�ע����ݲ�ͬ����������
 *       ���ͺż����ӷ�ʽ�޸ı�Ҫ����Ϣ��
 *
 * \include  awbl_thermistor_ntc.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1�� 
 *
 * - ��2�� ���������豸��Ϣ��(1) �¶ȴ�����ID�ţ�ʹ���������һһ��Ӧ \n
 *
 *                           (2) ��Ӧ���������R-T���ݱ� \n
 *
 *                           (3) R-T���ݱ��Ա�� \n
 * 
 *                           (5) ADC ͨ���� \n
 
 *                           (6) ������·���ӷ�ʽ \n
 *
 *                           (7) ������·��ߵ�ѹ \n
 *
 *                           (8) ����ߵ�ѹ������ֵ \n
 *
 *                           (9) �ӵص�����ֵ \n
 *
 *                           (10) ��ƽ̨��س�ʼ�� \n
 */

/** @} */

/** \brief ���ӷ�ʽѡ�� */
#define CONNECT_UP          0
#define CONNECT_DOWN        1
#define CONNECT_PARA_SER    2

/** \brief �¶������ֵƥ�� */
struct awbl_temp_ohm_match {
    int32_t   temp_c;     /**< \brief �¶�ֵ����λ �� */
    int32_t   ohm;        /**< \brief ����ֵ����λ ohm */
};

/** \brief NCT ���������豸��Ϣ */
struct awbl_ntc_therm_par {
    uint32_t                             id;     /**< \brief �豸id�� */
    aw_const struct awbl_temp_ohm_match *p_tempohm_lst;  /**< \brief ���ݱ� */
    size_t                               lst_count; /**< \brief ���ݱ���������Ŀ */
    
    uint8_t   channel;               /**< \brief ADC ͨ�� */
    
    uint8_t   connect;               /**< \brief ���ӷ�ʽ */
    int16_t   full_mv;               /**< \brief ����ѹ */
    int32_t   pullup_ohm;            /**< \brief ����������ֵ */
    int32_t   pulldown_ohm;          /**< \brief ����������ֵ */

    void (*pfunc_plfm_init) (void);  /**< \brief ƽ̨��س�ʼ�� */
};

/** \brief NTC ���������豸ʵ�� */
struct awbl_ntc_therm_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev dev;

    /** \brief  �¶ȴ��������� */
    struct awbl_temp_service  temp_serv;
};

#define NTC_THERMISTOR_NAME    "ntc-nameless"

#define SERV_GET_THERM_DEV(p_serv)   \
        (AW_CONTAINER_OF(p_serv, struct awbl_ntc_therm_dev, temp_serv))

#define __SERV_GET_DEV(p_serv)   \
        ((AW_CONTAINER_OF(p_serv, struct awbl_ntc_therm_dev, temp_serv))->dev)

#define SERV_GET_NTC_THERM_PAR(p_serv) \
        ((struct awbl_ntc_therm_par *)AWBL_DEVINFO_GET(&__SERV_GET_DEV(p_serv)))

#define DEV_TO_THERM_DEV(p_dev)  \
        (AW_CONTAINER_OF(p_dev, struct awbl_ntc_therm_dev, dev))

#define THERM_DEV_GET_PAR(p_therm_dev)  \
        ((struct awbl_ntc_therm_par *)((p_therm_dev)->dev.p_devhcf->p_devinfo))

#define DEV_GET_NTC_THERM_PAR(p_dev)   ((void *)AWBL_DEVINFO_GET(p_dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_THERM_NTC_H */

/* end of file */
