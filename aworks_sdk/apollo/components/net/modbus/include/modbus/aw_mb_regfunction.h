/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief modbus register function code head file.
 *
 * \internal
 * \par modification history
 * - 1.02 2015-05-22  cod, fix code style and comments, a little improve.
 * - 1.01 2013-03-29  liuweiyun, fix code style and comments, a little improve.
 * - 1.00 2012-07-05  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_REGFUNCTION_H /* { __AW_MB_REGFUNCTION_H */
#define __AW_MB_REGFUNCTION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_regfunction
 * \copydoc aw_mb_regfunction.h
 * @{
 */

#include "modbus/aw_mb_slave.h"
#include "modbus/aw_mb_utils.h"

/** \brief �����봦������ */
typedef struct {
    uint8_t                 funcode; /**< \brief ������ */
    aw_mb_fn_code_handler_t handler; /**< \brief �����봦���� */
} aw_funcode_handle;

/**
 * \brief ��ȡ��վ��Ϣ
 * \note  ��ȡModbus��ջ����˵������ǰ״̬������һЩ������Ϣ��
 *
 * \param[in]     slave    �ӻ����
 * \param[in,out] p_frame  ��վ��Ϣ������
 *
 * \return ID��վ��Ϣ����
 */
uint16_t aw_mb_get_slave_id (aw_mb_slave_t slave, uint8_t *p_frame);

/**
 * \brief ��ȡ������Ĵ�����
 *
 * \param[in] slave    �ӻ����
 * \param[in] funcode  ������
 *
 * \return �������Ӧ�Ĵ�����
 */
aw_mb_fn_code_handler_t aw_mb_fn_code_handler_get (aw_mb_slave_t slave,
                                                   uint8_t       funcode);

/**
 * \brief ����Ȧ�����봦����.
 * \note  �����Ҫ�ù��ܣ���Ҫʹ�ú���\a aw_mb_slave_register_handler ��ע�ᡣ
 *        ͬʱҪʹ��\a aw_mb_slave_register_callback ע�������Ȧ��������ص�����
 *
 * \param[in]     slave  �ӻ����
 * \param[in,out] p_pdu  ���������PDU������
 * \param[in,out] p_len  PDU����ָ��
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 */
aw_mb_exception_t aw_mb_fn_read_coils (aw_mb_slave_t  slave,
                                       uint8_t       *p_pdu,
                                       uint16_t      *p_len);

/**
 * \brief ����ɢ�������봦����.
 * \note  �����Ҫ�ù��ܣ���Ҫʹ�ú���\a aw_mb_slave_register_handler ��ע�ᡣ
 *        ͬʱҪʹ��\a aw_mb_slave_register_callback ע�������ɢ����������ص�����
 *
 * \param[in]     slave   �ӻ����
 * \param[in,out] p_pdu  ���������PDU������
 * \param[in,out] p_len  PDU����
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 */
aw_mb_exception_t aw_mb_fn_read_discrete_inputs (aw_mb_slave_t  slave,
                                                 uint8_t       *p_pdu,
                                                 uint16_t      *p_len);

/**
 * \brief ������Ĵ��������봦����.
 * \note  �����Ҫ�ù��ܣ���Ҫʹ�ú���\a aw_mb_slave_register_handler ��ע�ᡣ
 *        ͬʱҪʹ��\a aw_mb_slave_register_callback ע���������Ĵ�����������ص�����
 *
 * \param[in]     slave  �ӻ����
 * \param[in,out] p_pdu  ���������PDU������
 * \param[in,out] p_len  PDU����
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 */
aw_mb_exception_t aw_mb_fn_read_hold_regs (aw_mb_slave_t slave,
                                           uint8_t       *p_pdu,
                                           uint16_t      *p_len);

/**
 * \brief ������Ĵ��������봦����.
 * \note  �����Ҫ�ù��ܣ���Ҫʹ�ú���\a aw_mb_slave_register_handler ��ע�ᡣ
 *        ͬʱҪʹ��\a aw_mb_slave_register_callback
 *        ע���������Ĵ�����������ص�����
 *
 * \param[in]     slave  �ӻ����
 * \param[in,out] p_pdu  ���������PDU������
 * \param[in,out] p_len  PDU����
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 */
aw_mb_exception_t aw_mb_fn_read_input_regs (aw_mb_slave_t slave,
                                            uint8_t       *p_pdu,
                                            uint16_t      *p_len);

/**
 * \brief д������Ȧ�����봦����.
 * \note  �����Ҫ�ù��ܣ���Ҫʹ�ú���\a aw_mb_slave_register_handler ��ע�ᡣ
 *        ͬʱҪʹ��\a aw_mb_slave_register_callback
 *        ע����д��Ȧ��������ص����������д�����Ȧ�Ѿ���ע��д��Ȧ��������ص�������
 *        �Ͳ���Ҫ��ע�ᡣ
 *
 * \param[in]     slave  �ӻ����
 * \param[in,out] p_pdu  ���������PDU������
 * \param[in,out] p_len  PDU����
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 */
aw_mb_exception_t aw_mb_fn_write_single_coil (aw_mb_slave_t slave,
                                               uint8_t      *p_pdu,
                                              uint16_t      *p_len);

/**
 * \brief д�����Ĵ��������봦����.
 * \note  �����Ҫ�ù��ܣ���Ҫʹ�ú���\a aw_mb_slave_register_handler ��ע�ᡣ
 *        ͬʱҪʹ��\a aw_mb_slave_register_callback
 *        ע����д�Ĵ�����������ص����������д����Ĵ����Ѿ���ע��д�Ĵ����������
 *        �ص��������Ͳ���Ҫ��ע��
 *
 * \param[in]     slave  �ӻ����
 * \param[in,out] p_pdu  ���������PDU������
 * \param[in,out] p_len  PDU����
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 */
aw_mb_exception_t aw_mb_fn_write_single_reg (aw_mb_slave_t slave,
                                             uint8_t       *p_pdu,
                                             uint16_t      *p_len);

/**
 * \brief д�����Ȧ�����봦����.
 * \note  �����Ҫ�ù��ܣ���Ҫʹ�ú���\a aw_mb_slave_register_handler ��ע�ᡣ
 *        ͬʱҪʹ��\a aw_mb_slave_register_callback
 *        ע����д��Ȧ��������ص����������д������Ȧ�Ѿ���ע��д��Ȧ��������ص�������
 *        �Ͳ���Ҫ��ע��
 *
 * \param[in]     slave  �ӻ����
 * \param[in,out] p_pdu  ���������PDU������
 * \param[in,out] p_len  PDU����
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 */
aw_mb_exception_t aw_mb_fn_write_multiple_coils (aw_mb_slave_t slave,
                                                 uint8_t       *p_pdu,
                                                 uint16_t      *p_len);

/**
 * \brief д����Ĵ��������봦����.
 * \note  �����Ҫ�ù��ܣ���Ҫʹ�ú���\a aw_mb_slave_register_handler ��ע�ᡣ
 *        ͬʱҪʹ��\a aw_mb_slave_register_callback
 *        ע����д�Ĵ�����������ص����������д����Ĵ����Ѿ���ע��д�Ĵ����������
 *        �ص��������Ͳ���Ҫ��ע��
 *
 * \param[in]     slave  �ӻ����
 * \param[in,out] p_pdu  ���������PDU������
 * \param[in,out] p_len  PDU����
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 */
aw_mb_exception_t aw_mb_fn_write_multiple_regs (aw_mb_slave_t slave,
                                                uint8_t      *p_pdu,
                                                uint16_t     *p_len);

/**
 * \brief ��д����Ĵ��������봦����.
 * \note  �����Ҫ�ù��ܣ���Ҫʹ�ú���\a aw_mb_slave_register_handler ��ע�ᡣ
 *        ͬʱҪʹ��\a aw_mb_slave_register_callback
 *        ע�������д�Ĵ�����������ص�����������Ѿ�ע�������д�Ĵ����������
 *        �ص��������Ͳ���Ҫ��ע��
 *
 * \param[in]     slave  �ӻ����
 * \param[in,out] p_pdu  ���������PDU������
 * \param[in,out] p_len  PDU����
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 */
aw_mb_exception_t aw_mb_fn_rw_multiple_regs (aw_mb_slave_t  slave,
                                             uint8_t       *p_pdu,
                                             uint16_t      *p_len);
#if 0
/**
 * \brief ����ӻ���Ϣ�����봦����.
 * \note  �����Ҫ�ù��ܣ���Ҫʹ�ú���\a aw_mb_slave_register_handler ��ע�ᡣ
 *        �ӻ�����ʹ��\a aw_mb_slave_set_id �����ôӻ�����Ϣ��
 *
 * \param[in]     slave  �ӻ����
 * \param[in,out] p_pdu  ���������PDU������
 * \param[in,out] p_len  PDU����
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 *
 * \see aw_mb_slave_set_id
 */
aw_mb_exception_t aw_mb_fn_report_slave_id (aw_mb_slave_t  slave,
                                            uint8_t       *p_pdu,
                                            uint16_t      *p_len);
#endif

/** @} grp_aw_if_mb_regfunction */

#ifdef __cplusplus
}
#endif

#endif /* } __AW_MB_REGFUNCTION_H */

/* end of file */

