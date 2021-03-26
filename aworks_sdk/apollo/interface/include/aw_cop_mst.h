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
 * \brief CANopen�����ӿ�
 *
 * \note Э��ջ����ʹ����Ҫ����5������:
 * step1: initialize CANopen stack   => aw_cop_mst_init
 * step2: register node              => aw_cop_mst_add_node
 * step3: register pdo               => aw_cop_mst_input_pdo_install
 * step4: CANopen process task start => aw_cop_mst_process
 * step5: CANopen stack start        => aw_cop_mst_start
 *
 * \internal
 * \par modification history:
 * - 1.01 18-05-31  anu, modify
 * - 1.00 15-11-16  cae, first implementation
 * \endinternal
 */

#ifndef __AW_COP_MST_H
#define __AW_COP_MST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_cop_mst
 * \copydoc aw_cop_mst.h
 * @{
 */

#include "aw_cop.h"

/**
 * \name CANopen ��վ���ô�վ�ڵ�״̬������
 * \anchor grp_aw_cop_mst_node_status_cmd_t
 * @{
 */

/** \brief ��վ���ô�վ�ڵ�״̬������ */
typedef uint32_t aw_cop_mst_node_status_cmd_t;

/** \brief  ������վ */
#define AW_COP_MST_NODE_STATUS_CMD_START              1

/** \brief  ֹͣ��վ */
#define AW_COP_MST_NODE_STATUS_CMD_STOP               2

/** \brief  ʹ��վ����Ԥ����״̬ */
#define AW_COP_MST_NODE_STATUS_CMD_PRE_OPERATIONAL    128

/** \brief  ��λ��վ  */
#define AW_COP_MST_NODE_STATUS_CMD_RESET              129

/** \brief  ��λ��վ����  */
#define AW_COP_MST_NODE_STATUS_CMD_RESET_CONNECTION   130

/** @} */

/**
 * \name CANopen ��վ����Ĵ�վ�ڵ��״̬
 * \anchor grp_aw_cop_mst_node_status_t
 * @{
 */

/** \brief ��վ����Ĵ�վ�ڵ��״̬ */
typedef uint32_t aw_cop_mst_node_status_t;

/** \brief ��վ����Ĵ�վ�ڵ����ڳ�ʼ�� */
#define AW_COP_MST_NODE_STATUS_INIT         0

/** \brief ��վ����Ĵ�վ�ڵ��Ѷ��� */
#define AW_COP_MST_NODE_STATUS_DISCONNECT   1

/** \brief ��վ����Ĵ�վ�ڵ��������� */
#define AW_COP_MST_NODE_STATUS_CONNECTING   2

/** \brief ��վ����Ĵ�վ�ڵ�׼������ͨѶ�� */
#define AW_COP_MST_NODE_STATUS_PREPARING    3

/** \brief ��վ����Ĵ�վ�ڵ���ֹͣ */
#define AW_COP_MST_NODE_STATUS_STOP         4

/** \brief ��վ����Ĵ�վ�ڵ����ڹ��� */
#define AW_COP_MST_NODE_STATUS_WORK         5

/** \brief ��վ����Ĵ�վ�ڵ㴦��Ԥ����״̬ */
#define AW_COP_MST_NODE_STATUS_PREWORK      127

/** @} */

/**
 * \name CANopen ��վ���߼��ӽڵ㷽ʽ
 * \anchor grp_aw_cop_mst_node_check_way_t
 * @{
 */

typedef uint8_t aw_cop_mst_node_check_way_t;     /**< \brief ��վ���߼�鷽ʽ */
#define AW_COP_MST_NODE_CHECK_WAY_GUARDING     0 /**< \brief �ڵ��ػ� */
#define AW_COP_MST_NODE_CHECK_WAY_HEARTBEAT    1 /**< \brief ������ */
#define AW_COP_MST_NODE_CHECK_WAY_DISABLE      2 /**< \brief ���� */

/** @} */

/**
 * \brief  aw_cop_mst_pfn_pdo_recv_t
 *  PDO �������ݶ������Ӧ�ûص�
 *
 * \param[out] p_msg: ���յ��� PDO�������ݶ�����Ϣ
 *
 * \retval  AW_FALSE: ���յ������ݲ����浽�������棬
 *                 ��ͨ��aw_cop_mst_input_pdo_get()���ܻ�ȡ�����յ�������
 * \retval  AW_TRUE:  ���յ������ݱ��浽�������棬
 *                 ��ͨ��aw_cop_mst_input_pdo_get()���Ի�ȡ�����յ�������
 *
 */
typedef aw_bool_t (*aw_cop_mst_pfn_pdo_recv_t) (aw_cop_pdo_msg_t *p_msg);

/** \brief EMCC �����¼�Ӧ�ûص� */
typedef void (*aw_cop_mst_pfn_emcc_event_t) (aw_cop_emcy_msg_t *p_emcy_msg);

/** \brief CANopen ��վ��Ϣ */
typedef struct aw_cop_mst_info {

    uint32_t node_id;          /**< \brief �ڵ�ID */

    aw_cop_baud_t baudrate;    /**< \brief CAN������ ��λkbps */

    /** \brief PDO �������ݶ������Ӧ�ûص� */
    aw_cop_mst_pfn_pdo_recv_t   pfn_pdo_recv_callback;

    /** \brief EMCC �����¼�Ӧ�ûص� */
    aw_cop_mst_pfn_emcc_event_t pfn_emcc_event_callback;

} aw_cop_mst_info_t;

/** \brief CANopen ��վ����Ĵ�վ�ڵ���Ϣ */
typedef struct aw_cop_mst_node_info {

    uint8_t  node_id;                         /**< \brief ��վ�ڵ�ID */

    /**
     * \brief ��վ���߼�鷽ʽ
     *        �μ�\ref grp_aw_cop_mst_node_check_way_t
     */
    aw_cop_mst_node_check_way_t  check_mode;

    /**
     * \brief ��վ���߼�����ڣ���λms
     *        ��check_modeΪ������ AW_COP_MST_NODE_CHECK_WAY_HEARTBEAT ʱ��
     *        ��ʾ��վ������������
     */
    uint16_t check_period;

    /**
     * \brief ���Լ���վ����
     *        ��check_modeΪ��ֹ AW_COP_MST_NODE_CHECK_WAY_DISABLE ʱ��Ч
     */
    uint16_t retry_factor;

} aw_cop_mst_node_info_t;

/**
 * \brief  aw_cop_mst_init
 *  ��ʼ��CANopen ��վ
 *
 * \param[in] inst:   CANopenʵ����
 * \param[in] p_info: ��ʼ������
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_init (uint8_t inst, aw_cop_mst_info_t *p_info);

/**
 * \brief  aw_cop_mst_deinit
 *  �ر�CANopen ��վ
 *
 * \param[in] inst: CANopenʵ����
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_deinit (uint8_t inst);

/**
 * \brief  aw_cop_mst_start
 *  ����CANopen ��վ
 *
 * \param[in] inst: CANopenʵ����
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_start (uint8_t inst);


/**
 * \brief  aw_cop_mst_stop
 *  ֹͣCANopen ��վ
 *
 * \param[in] inst: CANopenʵ����
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_stop (uint8_t inst);

/**
 * \brief  aw_cop_mst_add_node
 *  ��Ӵ�վ�ڵ�
 *
 * \param[in] inst: CANopenʵ����
 * \param[in] p_info: ��վ�ڵ���Ϣ
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 *
 */
aw_cop_err_t aw_cop_mst_add_node (uint8_t inst, aw_cop_mst_node_info_t *p_info);

/**
 * \brief  aw_cop_mst_remove_node
 *  ��Ӵ�վ�ڵ�
 *
 * \param[in] inst: CANopenʵ����
 * \param[in] node_id: ��վ��Ϣ
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 *
 */
aw_cop_err_t aw_cop_mst_remove_node (uint8_t inst, uint8_t node_id);

/**
 * \brief  aw_cop_mst_sdo_upload
 *  ���ô˺�����ȡ��վ��������
 *
 * \param[in]     inst:      CANopenʵ����
 * \param[in]     node_id:   ��վ��ַ
 * \param[in]     index:     ��������������
 * \param[in]     subindex:  ����������������
 * \param[out]    p_data:    �������ݻ�����
 * \param[in/out] p_length:  �������ݴ�С������ʵ�ʽ��յ�������
 * \param[in]     wait_time: �ȴ���ʱʱ��
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_sdo_upload (uint8_t   inst,
                                    uint8_t   node_id,
                                    uint16_t  index,
                                    uint8_t   subindex,
                                    uint8_t  *p_data,
                                    uint32_t *p_length,
                                    uint32_t  wait_time);

/**
 * \brief  aw_cop_mst_sdo_dwonload
 *  ���ô˺������ô�վ��������
 *
 * \param[in] inst:      CANopenʵ����
 * \param[in] node_id:   ��վ��ַ
 * \param[in] index:     ��������������
 * \param[in] subindex:  ����������������
 * \param[in] p_data:    �������ݻ�����
 * \param[in] length:    �������ݳ���
 * \param[in] wait_time: �ȴ���ʱʱ��
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_sdo_dwonload (uint8_t    inst,
                                      uint8_t    node_id,
                                      uint16_t   index,
                                      uint8_t    subindex,
                                      uint8_t   *p_data,
                                      uint32_t   length,
                                      uint32_t   wait_time);

/**
 * \brief  aw_cop_mst_input_pdo_install
 *  ���ָ����վ��PDO����վ�� ʹ��վ���Խ��մ�վ������������
 *
 * \param[in] inst:     CANopenʵ����
 * \param[in] node_id:  ��վ��ַ
 * \param[in] pdo_id:   PDO ID
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_input_pdo_install (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id);

/**
 * \brief  aw_cop_mst_input_pdo_remove
 *  ɾ��ע������վ�е�RPDO
 *
 * \param[in] inst:     CANopenʵ����
 * \param[in] node_id:  ��վ��ַ
 * \param[in] pdo_id:   PDO ID
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_input_pdo_remove (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id);

/**
 * \brief  aw_cop_mst_output_pdo_install
 *  ���ָ����վ��PDO����վ�� ʹ��վ���Է������ݵ���վ
 *
 * \param[in] inst:     CANopenʵ����
 * \param[in] node_id:  ��վ��ַ
 * \param[in] pdo_id:   PDO ID
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_output_pdo_install (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id);

/**
 * \brief  aw_cop_mst_output_pdo_remove
 *  ɾ��ע������վ�е�TPDO
 *
 * \param[in] inst:     CANopenʵ����
 * \param[in] node_id:  ��վ��ַ
 * \param[in] pdo_id:   PDO ID
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_output_pdo_remove (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id);

/**
 * \brief  aw_cop_mst_input_pdo_get
 *  ��ȡ��վPDO����
 *
 * \param[in]     inst:      CANopenʵ����
 * \param[in]     node_id:   ��վ��ַ
 * \param[in]     pdo_id:    PDO ID
 * \param[out]    p_data:    �������ݻ�����
 * \param[in/out] p_length:  �������ݴ�С������ʵ�ʽ��յ�������
 * \param[in]     wait_time: �ȴ���ʱʱ��
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_input_pdo_get (uint8_t   inst,
                                       uint8_t   node_id,
                                       uint32_t  pdo_id,
                                       uint8_t  *p_data,
                                       uint32_t *p_length,
                                       uint32_t  wait_time);

/**
 * \brief  aw_cop_mst_output_pdo_set
 *  ��ָ����վ����PDO�������
 *
 * \param[in] inst:      CANopenʵ����
 * \param[in] node_id:   ��վ��ַ
 * \param[in] pdo_id:    PDO ID
 * \param[in] p_data:    �������ݻ�����
 * \param[in] length:    �������ݳ���
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_output_pdo_set (uint8_t   inst,
                                        uint8_t   node_id,
                                        uint32_t  pdo_id,
                                        uint8_t  *p_data,
                                        uint8_t   length);

/**
 * \brief  aw_cop_mst_remote_pdo_send
 *  ��ָ����վ����Զ��PDO���������ݣ�����վ�յ�������վ�ط�PDO��������
 *
 * \param[in] inst:      CANopenʵ����
 * \param[in] node_id:   ��վ��ַ
 * \param[in] pdo_id:    PDO ID
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_remote_pdo_send (uint8_t   inst,
                                         uint8_t   node_id,
                                         uint32_t  pdo_id);

/**
 * \brief  aw_cop_mst_node_status_set
 *  ��վ���ô�վ״̬
 *
 * \param[in] inst:      CANopenʵ����
 * \param[in] node_id:   ��վ��ַ
 * \param[in] status:    �������ͣ��μ�\ref grp_aw_cop_mst_node_status_cmd_t
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_node_status_set (uint8_t                      inst,
                                         uint8_t                      node_id,
                                         aw_cop_mst_node_status_cmd_t status);

/**
 * \brief  aw_cop_mst_node_status_get
 *  ��ȡ��վ����Ĵ�վ�ڵ�Ĺ���״̬
 *
 * \param[in]  inst:      CANopenʵ����
 * \param[in]  node_id:   ��վ��ַ
 * \param[out] p_status:  ��վ״̬���μ�\ref grp_aw_cop_mst_node_status_t
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_node_status_get (uint8_t                    inst,
                                         uint8_t                    node_id,
                                         aw_cop_mst_node_status_t  *p_status);

/**
 * \brief  aw_cop_mst_sync_prodcer_cfg
 *  ����ͬ��֡
 *
 * \param[in] inst:      CANopenʵ����
 * \param[in] sync_id:   ͬ������ID
 * \param[in] cyc_time:  ѭ�����ڣ���λms
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_sync_prodcer_cfg (uint8_t    inst,
                                          uint32_t   sync_id,
                                          uint16_t   cyc_time);

/**
 * \brief  aw_cop_mst_timestamp_send
 *  ����ʱ���ʶ������
 *
 * \param[in] inst:      CANopenʵ����
 * \param[in] sync_id:   ͬ������ID
 *
 * \return  ������룬�μ�\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_timestamp_send (uint8_t inst, aw_cop_time_t *p_time);

/**
 * \brief  aw_cop_mst_mst_process
 *  ����CANopen�����д�����
 *
 * \param[in] inst:      CANopenʵ����
 */
void aw_cop_mst_process (uint8_t inst);

/** @} grp_aw_if_cop_mst */

#ifdef __cplusplus
}
#endif

#endif /* __AW_COP_MST_H */

/* end of file */
