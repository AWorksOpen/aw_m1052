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
#include "aw_cop_mst.h"

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

typedef struct aw_cop_data {

    BYTE *p_slave_tab;
    int   slave_tab_size;

    tSdocParam *p_sdo_client_tab;
    int         sdo_client_tab_size;

    tPdoParam  *p_pdo_tab;
    int         pdo_tab_size;

    tHbcProdParam   *p_hbt_producer_param;
    int              hbt_producer_param_size;

    tNmtmSlaveParam *p_lifeguard_default_param;

    tVarParam  *p_var_tab;
    int         var_tab_size;

    BYTE  *p_digit_in_8bit_tab;
    int    digit_in_8bit_tab_size;
    WORD  *p_digit_in_16bit_tab;
    int    digit_in_16bit_tab_size;

    BYTE  *p_digit_out_8bit_tab;
    int    digit_out_8bit_tab_size;
    WORD  *p_digit_out_16bit_tab;
    int    digit_out_16bit_tab_size;

    WORD  *p_analog_in_16bit_tab;
    int    analog_in_16bit_tab_size;
    DWORD *p_analog_in_32bit_tab;
    int   analog_in_32bit_tab_size;

    WORD  *p_analog_out_16bit_tab;
    int    analog_out_16bit_tab_size;
    DWORD *p_analog_out_32bit_tab;
    int    analog_out_32bit_tab_size;

    DWORD *p_analog_up_limit_tab;
    int    analog_up_limit_tab_size;
    DWORD *p_analog_lo_limit_tab;
    int    analog_lo_limit_tab_size;

} aw_cop_data_t;

/**
 * \brief CANopen ��ʼ��
 *
 * \param[in] inst_cnt       ʵ��CANopen�ĸ���
 * \param[in] p_hwtimer      ��Ҫ�ṩһ����ʱ�����
 * \param[in] p_inst_data    ʵ������
 * \param[in] p_chn_brt_list ʵ���õ���CAN busͨ����Ӧ�Ĳ����ʱ�
 *
 * \return AW_OK�����ɹ�����������ԭ��ο�������
 */
aw_err_t aw_cop_init (
    int                         inst_cnt,
    const aw_hwtimer_handle_t   p_hwtimer,
    aw_cop_data_t              **p_inst_data,
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
