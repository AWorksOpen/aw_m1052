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
 * \brief CanFestival ����
 *
 * \internal
 * \par modification history:
 * - 1.00 18-10-08  anu, first implementation
 * \endinternal
 */

#ifndef __AW_CANFESTIVAL_CFG_H
#define __AW_CANFESTIVAL_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_canfestival_cfg
 * \copydoc aw_canfestival_cfg.h
 * @{
 */

#include "aw_types.h"
#include "aw_can.h"
#include "aw_hwtimer.h"
#include "canfestival.h"

#define AW_CANFESTIVAL_BRT_LIST_CNT  9 /** \brief �������б���� */

/** \brief CAN�������б� */
typedef struct aw_canfestival_brt_list {

    int chn;                            /**< \brief CAN BUSͨ���� */

    struct brt_list {
        int                 val;        /**< \brief CAN������ֵ ��λK */
        aw_can_baud_param_t param;      /**< \brief CAN������ */
    } brt[AW_CANFESTIVAL_BRT_LIST_CNT];

} aw_canfestival_brt_list_t;

/**
 * \brief CANfestival ��ʼ��
 *
 * \param[in] inst_cnt       ʵ��Canfestival�ĸ���
 * \param[in] recv_task_prio ������������ȼ�
 * \param[in] p_hwtimer      ��Ҫ�ṩһ����ʱ�����
 * \param[in] p_chn_brt_list ʵ���õ���CAN busͨ����Ӧ�Ĳ����ʱ�
 *
 * \return AW_OK�����ɹ�����������ԭ��ο�������
 */
aw_err_t aw_canfestival_init (
    int                              inst_cnt,
    int                              recv_task_prio,
    const aw_hwtimer_handle_t        p_hwtimer,
    const aw_canfestival_brt_list_t *p_chn_brt_list);

/**
 * \brief CANfestival ע�ᶨʱ��
 *
 * \param[in] p_hwtimer �жϲ���
 */
void aw_canfestival_timer_register (const aw_hwtimer_handle_t *p_hwtimer);

/**
 * \brief CANfestival ��ʱ���жϷ�����
 *
 * \param[in] p_arg �жϲ���
 */
void aw_canfestival_timer_irq (void *p_arg);

/** @} grp_aw_if_canfestival_cfg */

#ifdef __cplusplus
}
#endif

#endif /* __AW_CANFESTIVAL_CFG_H */

/* end of file */
