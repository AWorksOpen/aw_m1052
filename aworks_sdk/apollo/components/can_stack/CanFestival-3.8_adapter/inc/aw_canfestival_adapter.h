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

#ifndef __AW_CANFESTIVAL_ADAPTER_H
#define __AW_CANFESTIVAL_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_canfestival_adapter
 * \copydoc aw_canfestival_adapter.h
 * @{
 */

#include "aw_types.h"
#include "aw_can.h"

#include "canfestival.h"

/**
 * \brief CANfestival ʵ���ľ����ȡ
 *
 * \param[in] inst_num ʵ����
 *
 * \return ��Ӧʵ���ľ��
 */
void *aw_canfestival_inst_get (int inst_num);

/**
 * \brief CANfestival ʵ�������ݻ�ȡ
 *
 * \param[in] inst_num ʵ����
 *
 * \return ��Ӧʵ��������
 */
void *aw_canfestival_inst_data_get (int inst_num);

/**
 * \brief RPDO���»ص�����ע��
 *
 * \param[in] inst_num  ʵ����
 * \param[in] p_msg     Ҫ���͵���Ϣ
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_msg_send (int inst_num, aw_can_std_msg_t *p_msg);

/**
 * \brief RPDO���»ص�����ע��
 *
 * \param[in] inst_num       ʵ����
 * \param[in] p_callback_fun �ص�����
 * \param[in] p_arg          �û�����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_rpdo_update_callback_register (
    int    inst_num,
    void (*p_callback_fun) (
        void *p_arg, uint16_t pdo_id, uint8_t *p_data, uint8_t len),
    void  *p_arg);

/**
 * \brief CANfestival ��ȡʵ����Ӧ�Ľڵ�ĸ���
 *
 * \param[in] inst_num ʵ����
 *
 * \return ������С�ڵ���0��Ϊ��Ч
 */
int8_t aw_canfestival_inst_node_cnt (int inst_num);

/**
 * \brief CANfestival ��ȡPDO ID��Ӧ��PDO NUM
 *
 * \param[in]  inst_num   ʵ����
 * \param[in]  node_id    NODE ID ��
 * \param[out] p_node_num NODE NUM ��
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_node_num_get (
    int inst_num, int node_id, int *p_node_num);

/**
 * \brief CANfestival ����ʵ����ĳ���ڵ����Ϣ
 *
 * \param[in] inst_num     ʵ����
 * \param[in] node_num     �ڵ��
 * \param[in] node_id      �ڵ�ID
 * \param[in] check_mode   ���ģʽ
 * \param[in] check_period �������
 * \param[in] retry_factor �������
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_inst_node_info_set (int      inst_num,
                                                uint8_t  node_num,
                                                uint8_t  node_id,
                                                uint8_t  check_mode,
                                                uint16_t check_period,
                                                uint16_t retry_factor);

/**
 * \brief CANfestival ��ȡʵ����ĳ���ڵ����Ϣ
 *
 * \param[in]  inst_num       ʵ����
 * \param[in]  node_num       �ڵ��
 * \param[out] p_node_id      �ڵ�ID
 * \param[out] p_check_mode   ���ģʽ
 * \param[out] p_check_period �������
 * \param[out] p_retry_factor �������
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_inst_node_info_get (int       inst_num,
                                                uint8_t   node_num,
                                                uint8_t  *p_node_id,
                                                uint8_t  *p_check_mode,
                                                uint16_t *p_check_period,
                                                uint16_t *p_retry_factor);

/**
 * \brief CANfestival ʵ����ӽڵ���Ϣ
 *
 * \param[in] inst_num ʵ����
 * \param[in] node_id      �ڵ�ID
 * \param[in] check_mode   ���ģʽ
 * \param[in] check_period �������
 * \param[in] retry_factor �������
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_inst_node_add (int      inst_num,
                                           uint8_t  node_id,
                                           uint8_t  check_mode,
                                           uint16_t check_period,
                                           uint16_t retry_factor);

/**
 * \brief CANfestival ʵ���Ƴ��ڵ�
 *
 * \param[in] inst_num ʵ����
 * \param[in] node_id  �ڵ�ID
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_inst_node_remove (int      inst_num,
                                              uint8_t  node_id);

/**
 * \brief CANfestival ��ȡʵ����Ӧ��PDO�ĸ���
 *
 * \param[in] inst_num ʵ����
 * \param[in] is_rpdo  �Ƿ���RPDO
 *
 * \return ������С�ڵ���0��Ϊ��Ч
 */
int8_t aw_canfestival_inst_pdo_cnt (int inst_num, aw_bool_t is_rpdo);

/**
 * \brief CANfestival ��ȡPDO ID��Ӧ��PDO NUM
 *
 * \param[in]  inst_num  ʵ����
 * \param[in]  pdo_id    PDO ID ��
 * \param[in]  is_rpdo   �Ƿ���RPDO
 * \param[out] p_pdo_num PDO NUM ��
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_pdo_num_get (
    int inst_num, int pdo_id, aw_bool_t is_rpdo, int *p_pdo_num);

/**
 * \brief CANfestival ʧ������RPDO��TPDO
 *
 * \param[in]  inst_num  ʵ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_all_pdo_disable (int inst_num, aw_bool_t is_rpdo);

/**
 * \brief CANfestival ʵ�����PDO
 *
 * \param[in] inst_num ʵ����
 * \param[in] pdo_id   PDO ID ��
 * \param[in] is_rpdo  �Ƿ���RPDO
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_inst_pdo_add (
    int inst_num, int pdo_id, aw_bool_t is_rpdo);

/**
 * \brief CANfestival ʵ���Ƴ�PDO
 *
 * \param[in] inst_num ʵ����
 * \param[in] pdo_id   PDO ID ��
 * \param[in] is_rpdo  �Ƿ���RPDO
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_inst_pdo_remove (
    int inst_num, int pdo_id, aw_bool_t is_rpdo);

/**
 * \brief CANfestival ����PDO����
 *
 * \param[in]  inst_num  ʵ����
 * \param[in]  pdo_num   PDO NUM ��
 * \param[in]  is_rpdo   �Ƿ���RPDO
 * \param[in]  p_data    PDO ����
 * \param[in]  length    PDO ���ݳ���
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_pdo_data_set (int        inst_num,
                                          int        pdo_num,
                                          aw_bool_t  is_rpdo,
                                          uint8_t   *p_data,
                                          uint8_t    length);

/**
 * \brief CANfestival ��ȡPDO����
 *
 * \param[in]  inst_num  ʵ����
 * \param[in]  pdo_num   PDO NUM ��
 * \param[in]  is_rpdo   �Ƿ���RPDO
 * \param[in]  p_data    PDO ����
 * \param[in]  p_len     PDO ���ݳ���
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_pdo_data_get (int        inst_num,
                                          int        pdo_num,
                                          aw_bool_t  is_rpdo,
                                          uint8_t   *p_data,
                                          uint8_t   *p_len);

/**
 * \brief CANfestival ��ȡPDO����
 *
 * \param[in]  inst_num  ʵ����
 * \param[in]  pdo_num   PDO NUM ��
 * \param[in]  p_data    PDO ����
 * \param[in]  p_len     PDO ���ݳ���
 * \param[in]  wait_time ��ȡ��ʱʱ�� ms
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_rpdo_rngbuf_data_get (int       inst_num,
                                                  int       pdo_num,
                                                  uint8_t  *p_data,
                                                  uint32_t *p_len,
                                                  uint32_t  wait_time);

/**
 * \brief CANfestival ʵ������
 *
 * \param[in] inst_num ʵ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_inst_start (int inst_num);

/**
 * \brief CANfestival ʵ��ֹͣ
 *
 * \param[in] inst_num ʵ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_inst_stop (int inst_num);

/**
 * \brief CANfestival ʵ���ĳ�ʼ��
 *
 * \param[in] inst_num ʵ����
 * \param[in] brt      ������ ��λKbps
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_inst_init (int inst_num, int brt);

/**
 * \brief CANfestival ʵ���Ľ��ʼ��
 *
 * \param[in] inst_num ʵ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 */
aw_can_err_t aw_canfestival_inst_deinit (int inst_num);

/** @} grp_aw_if_canfestival_adapter */

#ifdef __cplusplus
}
#endif

#endif /* __AW_CANFESTIVAL_ADAPTER_H */

/* end of file */
