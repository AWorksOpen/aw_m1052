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
 * \brief CANopen ����
 *
 * \internal
 * \par modification history:
 * - 1.00 19-12-17  anu, first implementation
 * \endinternal
 */

#ifndef __AW_COP_CFG_H
#define __AW_COP_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_cop_cfg
 * \copydoc aw_cop_cfg.h
 * @{
 */

#include "aw_types.h"
#include "aw_can.h"
#include "aw_hwtimer.h"
#include "aw_cop_slv.h"

#include "copinc.h"

#define AW_COP_BRT_LIST_CNT  9          /**< \brief �������б���� */

/** \brief CAN�������б� */
typedef struct aw_cop_brt_list {

    int chn;                            /**< \brief CAN BUSͨ���� */

    struct brt_list {
        aw_cop_baud_t       val;        /**< \brief CAN������ֵ ��λK */
        aw_can_baud_param_t param;      /**< \brief CAN������ */
    } brt[AW_COP_BRT_LIST_CNT];

} aw_cop_brt_list_t;

/**
 * \brief CANopen ��ʼ��
 *
 * \param[in] inst_cnt       ʵ��CANopen�ĸ���
 * \param[in] p_hwtimer      ��Ҫ�ṩһ����ʱ�����
 * \param[in] p_chn_brt_list ʵ���õ���CAN busͨ����Ӧ�Ĳ����ʱ�
 *
 * \return AW_OK�����ɹ�����������ԭ��ο�������
 */
aw_err_t aw_cop_init (
    int                         inst_cnt,
    const aw_hwtimer_handle_t   p_hwtimer,
    const aw_cop_brt_list_t    *p_chn_brt_list);

/**
 * \brief CANopen ��ʱ���жϷ�����
 *
 * \param[in] p_arg �жϲ���
 */
void aw_cop_timer_irq (void *p_arg);

/** @} grp_aw_if_cop_cfg */

#ifdef __cplusplus
}
#endif

#endif /* __AW_COP_CFG_H */

/* end of file */
