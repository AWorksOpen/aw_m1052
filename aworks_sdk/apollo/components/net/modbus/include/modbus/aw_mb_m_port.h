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
 * \brief Modbus Master Port interface from AWorks.
 *
 * \internal
 * \par modification history
 * - 1.01 2019-03-26  cml, use serial interface replace int serial.
 * - 1.00 2016-01-19  cod, first implementation
 * \endinternal
 */
#ifndef __AW_MB_M_PORT_H  /* { */
#define __AW_MB_M_PORT_H

/**
 * \addtogroup grp_aw_if_mb_m_port
 * \copydoc aw_mb_m_port.h
 * @{
 */
#include "modbus/aw_mb_err.h"
#include "modbus/aw_mb_comm.h"
#include "aw_sem.h"
#include "awbl_sio.h"
#include "aw_timer.h"
#include "sys/socket.h"
#include "aw_vdebug.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief ��վ���ڳ�ʼ��
 *
 * \param[in] port                �˿ں�
 * \param[in] databits            ����λ
 * \param[in] baud_rate           ������
 * \param[in] parity              У��λ
 * \param[in] is_fast_reaction    �Ƿ�ʹ�ÿ�����Ӧģʽ��ֻ��RTUʹ�ã�
 * \param[in] rd_timeout          �ܳ�ʱʱ��
 * \param[in] rd_interval_timeout ��䳬ʱʱ��
 *
 * \return ���û�д��󷵻�AW_MB_ERR_NOERR����֮������Ӧ�Ĵ�������.
 */
aw_mb_err_t aw_mb_m_serial_init (uint32_t          port,
                                 uint8_t           databits,
                                 uint32_t          baud_rate,
                                 enum aw_mb_parity parity,
                                 uint8_t           is_fast_reaction,
                                 uint32_t          rd_timeout,
                                 uint32_t          rd_interval_timeout);

/**
 * \brief ���������豸
 *
 * \param[in] port ���ں�
 *
 * \return ��
 */
void aw_mb_m_serial_port_start (uint32_t port);

/**
 * \brief ֹͣ�����豸
 *
 * \param[in] port ���ں�
 *
 * \return ��
 */
void aw_mb_m_serial_port_stop (uint32_t port);

/**
 * \brief �رմ����豸
 *
 * \param[in] port ���ں�
 *
 * \return ��
 */
void aw_mb_m_serial_port_close (uint32_t port);

/**
 * \brief ��ȡһ���ַ�
 *
 * \param[in]  port   ���ں�
 * \param[out] p_data ָ����ջ�������ָ��
 * \param[in]  len    ������ȡ���ַ�����
 *
 * \return ʵ�ʻ�ȡ���ĳ���
 */
int aw_mb_m_serial_port_gets (uint32_t port, char *p_data ,uint32_t len);

/**
 * \brief ����һ���ַ�
 *
 * \param[in]  port   ���ں�
 * \param[out] p_data ָ���ͻ�������ָ��
 * \param[in]  len    �������͵��ַ�����
 *
 * \return ʵ�ʷ��͵ĳ���
 */
int aw_mb_m_serial_port_puts (uint32_t port, const char *p_data, uint32_t len);

/*****************************************************************************/
/** \brief modbus master tcp client */
typedef struct aw_mb_m_tcp_client {
    struct sockaddr_in server;       /**< \brief server address information */
    int                connect_sock; /**< \brief connect sock */
    AW_MUTEX_DECL(     lock);        /**< \brief socket lock */
} aw_mb_m_tcp_client_t;

/**
 * \brief ��ʼ�������
 *
 * \param[in] p_client  �ͻ��˲�������
 * \param[in] ip_addr   �����IP��ַ
 * \param[in] port      �˿�
 *
 * \return �����ɹ�����AW_MB_ERR_NOERR.���󷵻���Ӧ������.
 */
aw_mb_err_t aw_mb_tcp_client_init (aw_mb_m_tcp_client_t *p_client,
                                   char                 *ip_addr,
                                   uint16_t              port);

/**
 * \brief ����socket
 *
 * \return �����ɹ�����AW_MB_ERR_NOERR.���󷵻���Ӧ������.
 */
aw_static_inline int aw_mb_tcp_socket_create (void)
{
    return socket( AF_INET, SOCK_STREAM, 0 );
}

/**
 * \brief ���ӵ������
 *
 * \param[in] p_client  �ͻ��˲�������
 * \param[in] timeout   ���ӳ�ʱʱ��
 *
 * \return �����ɹ�����AW_MB_ERR_NOERR.���󷵻���Ӧ������.
 */
aw_mb_err_t aw_mb_tcp_client_start (aw_mb_m_tcp_client_t *p_client, uint32_t timeout);

/**
 * \brief �Ͽ�����
 *
 * \param[in] p_client  �ͻ��˲�������
 *
 * \return �ɹ�����AW_MB_ERR_NOERR.���󷵻���Ӧ������.
 */
aw_mb_err_t aw_mb_tcp_client_stop (aw_mb_m_tcp_client_t *p_client);

/**
 * \brief �����˷�������
 *
 * \param[in] p_client  �ͻ��˲�������
 * \param[in] p_sndbuf  �������ݻ�����
 * \param[in] snd_len   �������ݳ���
 *
 * \return �ɹ�����AW_MB_ERR_NOERR.���󷵻���Ӧ������.
 */
aw_mb_err_t aw_mb_tcp_client_send (aw_mb_m_tcp_client_t *p_client,
                                   uint8_t              *p_sndbuf,
                                   uint16_t              snd_len);

/**
 * \brief ���շ��������
 *
 * \param[in]     p_client  �ͻ��˲�������
 * \param[out]    p_rcvbuf  ���ջ�����
 * \param[out]    p_rcvlen  ���յ����ݳ���
 * \param[in,out] p_time    [in]��ʱʱ�䣬 [out]��������ʱ��, ����һ֡�ɹ�����ֵ��Ч.
 *
 * \return �ɹ�����AW_MB_ERR_NOERR.���󷵻���Ӧ������.
 */
aw_mb_err_t aw_mb_tcp_client_recv (aw_mb_m_tcp_client_t *p_client,
                                   uint8_t              *p_rcvbuf,
                                   uint16_t             *p_rcvlen,
                                   uint32_t             *p_time);

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_m_port */

#endif /* } __AW_MB_M_PORT_H */

/* end of file */

