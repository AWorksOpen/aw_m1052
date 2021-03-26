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
 * \brief modbus port header file
 *
 * \internal
 * \par modification history
 * - 1.04 2019-03-26  cml, use serial interface replace int serial.
 * - 1.03 2016-04-27  cod, redefine midd-layer interface to support muti-instance.
 * - 1.02 2015-05-18  cod, fix code style and comments, a little improve.
 * - 1.00 2012-05-03  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_PORT_H
#define __AW_MB_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_port
 * \copydoc aw_mb_port.h
 * @{
 */

#include "modbus/aw_mb_config.h"
#include "modbus/aw_mb_err.h"
#include "modbus/aw_mb_comm.h"
#include "aw_sem.h"
#include "aw_serial.h"
#include "aw_timer.h"
#include "aw_hwtimer.h"

/******************************************************************************/
/**
 * \brief ��ʼ������.
 *
 * \param[in] port                ���ں�
 * \param[in] dataBits            ����λ����(7bits or 8bits)
 * \param[in] baud_rate           ������
 * \param[in] parity              ��żУ��ģʽ
 * \param[in] is_fast_reaction    �Ƿ�ʹ�ÿ�����Ӧģʽ��ֻ��RTUʹ�ã�
 * \param[in] rd_timeout          �ܳ�ʱʱ��
 * \param[in] rd_interval_timeout ��䳬ʱʱ��
 *
 * \return ���û�д��󷵻� AW_MB_ERR_NOERR�����򷵻� AW_MB_ERR_EPORTERR
 */
aw_mb_err_t aw_mb_serial_port_init (uint8_t            port,
                                    uint8_t            databits,
                                    uint32_t           baud_rate,
                                    enum aw_mb_parity  parity,
                                    uint8_t            is_fast_reaction,
                                    uint32_t           rd_timeout,
                                    uint32_t           rd_interval_timeout);

/**
 * \brief ��������
 *
 * \param[in] port ���ں�
 *
 * \return ��
 */
void aw_mb_serial_port_start (uint32_t port);

/**
 * \brief stop����
 *
 * \param[in] port ���ں�
 *
 * \return ��
 */
void aw_mb_serial_port_stop (uint32_t port);

/**
 * \brief �رմ���
 *
 * \param[in] port ���ں�
 *
 * \return ��
 */
void aw_mb_serial_port_close (uint32_t port);

/**
 * \brief ��ȡһ���ַ�
 *
 * \param[in]  port   ���ں�
 * \param[out] p_data ָ����ջ�������ָ��
 * \param[in]  len    ������ȡ���ַ�����
 *
 * \return ʵ�ʻ�ȡ���ĳ���
 */
int aw_mb_serial_port_gets (uint32_t port, char *p_data ,uint32_t len);

/**
 * \brief ����һ���ַ�
 *
 * \param[in]  port   ���ں�
 * \param[out] p_data ָ���ͻ�������ָ��
 * \param[in]  len    �������͵��ַ�����
 *
 * \return ʵ�ʷ��͵ĳ���
 */
int aw_mb_serial_port_puts (uint32_t port, const char *p_data, uint32_t len);

/******************************************************************************/
/**
 * \brief Modbus TCP��ʼ��
 * \note  ���ö˿ڣ���ʼ����
 *
 * \param[in] p_cookie  TCPģʽ���
 * \param[in] tcpport   TCP�˿�
 *
 * \retval AW_MB_ERR_NOERR ��ʼ���ɹ�
 */
aw_mb_err_t aw_mb_tcp_port_init (void *p_cookie, uint16_t tcpport);

/**
 * \brief �ر�TCP����
 *
 * \param[in] p_cookie  TCPģʽ���
 *
 * \return ��
 */
void aw_mb_tcp_port_close (void *p_cookie);

/**
 * \brief TCP����ֹͣ
 *
 * \param[in] p_cookie  TCPģʽ���
 *
 * \return ��
 */
void aw_mb_tcp_port_disable (void *p_cookie);


/**
 * \brief ��ȡTCP��������
 *
 * \note ��ȡһ֡������Modbus TCP����, �ڽ��յ����ݺ���á���ȡ���ݺ�������գ����ó�ʼ����ƫ����
 *
 * \param[in]  p_cookie     TCPģʽ���
 * \param[out] pp_tcpframe  ��������֡����
 * \param[out] p_tcplen     ֡���ݳ���
 *
 */
void aw_mb_tcp_port_getrequest (void     *p_cookie,
                                uint8_t **pp_tcpframe,
                                uint16_t *p_tcplen);

/**
 * \brief TCP�������Ӧ����
 * \note  ��Ӧ���󣬷���һ֡��Ӧ���ݻؿͻ���
 *
 * \param[in] p_cookie    TCPģʽ���
 * \param[in] p_tcpframe  ֡����
 * \param[in] tcplen      ֡���ݳ���
 *
 * \retval AW_MB_ERR_NOERR ����֡���ͳɹ�
 */
aw_mb_err_t aw_mb_tcp_port_sndresponse (void          *p_cookie,
                                        const uint8_t *p_tcpframe,
                                        uint16_t       tcplen);

/**
 * \brief  ��ѯ�����׽��ֺ͵�ǰ���ӵ�Modbus TCP�ͻ��˵����¼���
 * \note   �˹��ܼ���¿ͻ��˵����Ӻ������ӿͻ��˷��͵�����.
 *         ���һ���µĿͻ������ӣ�������ʣ�µĿͻ��˲�ۣ�ֻ��һ��Ŀǰ�豸֧�֣���
 *         ��ô���ӱ����ܣ���Ϊ�µĿͻ����׽��ֵ��¼����󱻼��
 *         �ͻ����ڷ���FD_CLOSE�󣬿ͻ������ӱ��ͷŵġ�
 *
 * \retval AW_MB_ERR_EIO    �ڲ�IO����
 * \retval AW_MB_ERR_NOERR  û�д���
 */
aw_mb_err_t aw_mb_port_tcp_poll (void *p_cookie);

/**
 * @} grp_aw_if_mb_port
 */

#ifdef __cplusplus
}
#endif


#endif /* } __AW_MB_PORT_H */

/* end of file */

