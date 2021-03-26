/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief CANopen�ӻ��ӿ�
 *
 * \note Э��ջ����ʹ����Ҫ����3������:
 * step1: initialize CANopen stack   => aw_cop_slv_init
 * step2: slave connect to the net   => aw_cop_slv_connect_net
 * step3: CANopen process task start => aw_cop_slv_process
 *
 * \internal
 * \par modification history:
 * - 1.01 19-12-02  anu, modify
 * - 1.00 15-11-16  cae, first implementation
 * \endinternal
 */

#ifndef __AW_COP_SLV_H
#define __AW_COP_SLV_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_cop_slv
 * \copydoc aw_cop_slv.h
 * @{
 */

#include "aw_cop.h"

/**
 * \name CANopen ���ô�վ״̬������
 * \anchor grp_aw_cop_slv_status_cmd_t
 * @{
 */

/** \brief ���ô�վ״̬������ */
typedef uint32_t aw_cop_slv_status_cmd_t;

/** \brief  ������վ */
#define AW_COP_SLV_STATUS_CMD_INIT                  0

/** \brief  ʹ��վ����Ԥ����״̬ */
#define AW_COP_SLV_STATUS_CMD_OPERATIONAL           1

/** \brief  ֹͣ��վ */
#define AW_COP_SLV_STATUS_CMD_STOP                  2

/** \brief  ʹ��վ����Ԥ����״̬ */
#define AW_COP_SLV_STATUS_CMD_PRE_OPERATIONAL       128

/** \brief  ��λ��վ  */
#define AW_COP_SLV_STATUS_CMD_RESET                 129

/** \brief  ��λ��վͨ��  */
#define AW_COP_SLV_STATUS_CMD_RESET_COMMUNICATION   130

/** @} */

/**
 * \name CANopen ��վ״̬
 * \anchor grp_aw_cop_slv_status_t
 * @{
 */

/** \brief ��վ״̬ */
typedef uint32_t aw_cop_slv_status_t;

/** \brief ��վ���ڳ�ʼ��״̬ */
#define AW_COP_SLV_STATUS_INIT                0

/** \brief ��վ����ֹͣ״̬ */
#define AW_COP_SLV_STATUS_STOP                4

/** \brief ��վ���ڲ���״̬ */
#define AW_COP_SLV_STATUS_OPERATIONAL         5

/** \brief ��վ����Ԥ����״̬ */
#define AW_COP_SLV_STATUS_PRE_OPERATIONAL     127

/** @} */

/** \brief CANopen ��վ��Ϣ */
typedef struct aw_cop_slv_info {

    uint32_t node_id;          /**< \brief �ڵ�ID */
    uint32_t baudrate;         /**< \brief CAN������ ��λkbps */

    const uint8_t  *p_seg_cont;      /**< \brief �����Σ��������ж���Ĺ������� */
    uint32_t        seg_cont_size;   /**< \brief �����δ�С */

    uint8_t        *p_process_image;      /**< \brief ���̾��� */
    uint32_t        process_image_size;   /**< \brief ���̾����С */

} aw_cop_slv_info_t;

/**
 * \brief  aw_cop_slv_init
 *  ��ʼ��CANopen��վ
 *
 * \param[in] inst:   CANopenʵ����
 * \param[in] p_info: ��ʼ������
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_init (uint8_t inst, aw_cop_slv_info_t *p_info);

/**
 * \brief  aw_cop_slv_deinit
 *  ���ʼ��CANopen��վ
 *
 * \param[in] inst: CANopenʵ����
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_deinit (uint8_t inst);

/**
 * \brief  aw_cop_slv_connect_net
 *  ���ӵ�CANopen����
 *
 * \param[in] inst: CANopenʵ����
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_connect_net (uint8_t inst);

/**
 * \brief  aw_cop_slv_boot
 *  ��վ�Զ�������������վ��������ѭCANopen�淶������ʹ��
 *
 * \param[in] inst: CANopenʵ����
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_boot (uint8_t inst);

/**
 * \brief  aw_cop_slv_status_set
 *  ���ô�վ״̬
 *
 * \param[in] inst:   CANopenʵ����
 * \param[in] status: �������ͣ��μ�\ref grp_aw_cop_slv_status_cmd_t
 *
 * \retval aw_cop_err_t:      ���������ش����롣
 *
 */
aw_cop_err_t aw_cop_slv_status_set (
    uint8_t inst, aw_cop_slv_status_cmd_t status);

/**
 * \brief  aw_cop_slv_status_get
 *  ��ȡ��վ��״̬
 *
 * \param[in]  inst:     CANopenʵ����
 * \param[out] p_status: ��վ״̬���μ�\ref grp_aw_cop_slv_status_t
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_status_get (
    uint8_t inst, aw_cop_slv_status_t *p_status);

/**
 * \brief  aw_cop_slv_obd_read
 *  CANopen�ڲ������ֵ��ȡ
 *
 * \param[in]     inst:     CANopenʵ����
 * \param[in]     index:    ��������������
 * \param[in]     subindex: ����������������
 * \param[out]    p_data:   �������ݻ�����
 * \param[in/out] p_length: �������ݴ�С������ʵ�ʽ��յ�������
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_obd_read (uint8_t   inst,
                                  uint16_t  index,
                                  uint8_t   subindex,
                                  uint8_t  *p_data,
                                  uint32_t *p_length);

/**
 * \brief  aw_cop_slv_obd_write
 *  CANopen�ڲ������ֵ��ȡ
 *
 * \param[in] inst:     CANopenʵ����
 * \param[in] index:    ��������������
 * \param[in] subindex: ����������������
 * \param[in] p_data:   �������ݻ�����
 * \param[in] length:   �������ݳ���
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 *
 */
aw_cop_err_t aw_cop_slv_obd_write (uint8_t   inst,
                                   uint16_t  index,
                                   uint8_t   subindex,
                                   uint8_t  *p_data,
                                   uint32_t  length);

/**
 * \brief  aw_cop_slv_tpdo_send
 *  CANopen ��������
 *
 * \param[in] inst:     CANopenʵ����
 * \param[in] offset:   ���͹��̾���ƫ��
 * \param[in] p_data:   �������ݻ�����
 * \param[in] length:   �������ݳ���
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 *
 */
aw_cop_err_t aw_cop_slv_tpdo_send (uint8_t   inst,
                                   uint32_t  offset,
                                   uint8_t  *p_data,
                                   uint32_t  length);

/**
 * \brief  aw_cop_slv_rpdo_read
 *  CANopen ��ȡ����
 *
 * \param[in]  inst:     CANopenʵ����
 * \param[in]  offset:   ���չ��̾���ƫ��
 * \param[out] p_data:   ��ȡ���ݻ�����
 * \param[in]  length:   ��ȡ���ݳ���
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 *
 */
aw_cop_err_t aw_cop_slv_rpdo_read (uint8_t   inst,
                                   uint32_t  offset,
                                   uint8_t  *p_data,
                                   uint32_t  length);

/**
 * \brief  aw_cop_slv_process
 *  CANopen������
 *
 * \param[in] inst: CANopenʵ����
 */
void aw_cop_slv_process (uint8_t inst);

/** @} grp_aw_if_cop_slv */

#ifdef __cplusplus
}
#endif

#endif /* __AW_COP_SLV_H */

/* end of file */
