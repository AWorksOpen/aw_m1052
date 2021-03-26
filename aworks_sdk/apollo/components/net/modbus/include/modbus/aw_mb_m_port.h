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
 * \brief 主站串口初始化
 *
 * \param[in] port                端口号
 * \param[in] databits            数据位
 * \param[in] baud_rate           波特率
 * \param[in] parity              校验位
 * \param[in] is_fast_reaction    是否使用快速响应模式（只有RTU使用）
 * \param[in] rd_timeout          总超时时间
 * \param[in] rd_interval_timeout 码间超时时间
 *
 * \return 如果没有错误返回AW_MB_ERR_NOERR，反之返回相应的错误类型.
 */
aw_mb_err_t aw_mb_m_serial_init (uint32_t          port,
                                 uint8_t           databits,
                                 uint32_t          baud_rate,
                                 enum aw_mb_parity parity,
                                 uint8_t           is_fast_reaction,
                                 uint32_t          rd_timeout,
                                 uint32_t          rd_interval_timeout);

/**
 * \brief 启动串口设备
 *
 * \param[in] port 串口号
 *
 * \return 无
 */
void aw_mb_m_serial_port_start (uint32_t port);

/**
 * \brief 停止串口设备
 *
 * \param[in] port 串口号
 *
 * \return 无
 */
void aw_mb_m_serial_port_stop (uint32_t port);

/**
 * \brief 关闭串口设备
 *
 * \param[in] port 串口号
 *
 * \return 无
 */
void aw_mb_m_serial_port_close (uint32_t port);

/**
 * \brief 获取一串字符
 *
 * \param[in]  port   串口号
 * \param[out] p_data 指向接收缓冲区的指针
 * \param[in]  len    期望获取的字符长度
 *
 * \return 实际获取到的长度
 */
int aw_mb_m_serial_port_gets (uint32_t port, char *p_data ,uint32_t len);

/**
 * \brief 发送一串字符
 *
 * \param[in]  port   串口号
 * \param[out] p_data 指向发送缓冲区的指针
 * \param[in]  len    期望发送的字符长度
 *
 * \return 实际发送的长度
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
 * \brief 初始化服务端
 *
 * \param[in] p_client  客户端操作数据
 * \param[in] ip_addr   服务端IP地址
 * \param[in] port      端口
 *
 * \return 启动成功返回AW_MB_ERR_NOERR.错误返回相应错误码.
 */
aw_mb_err_t aw_mb_tcp_client_init (aw_mb_m_tcp_client_t *p_client,
                                   char                 *ip_addr,
                                   uint16_t              port);

/**
 * \brief 创建socket
 *
 * \return 启动成功返回AW_MB_ERR_NOERR.错误返回相应错误码.
 */
aw_static_inline int aw_mb_tcp_socket_create (void)
{
    return socket( AF_INET, SOCK_STREAM, 0 );
}

/**
 * \brief 连接到服务端
 *
 * \param[in] p_client  客户端操作数据
 * \param[in] timeout   连接超时时间
 *
 * \return 启动成功返回AW_MB_ERR_NOERR.错误返回相应错误码.
 */
aw_mb_err_t aw_mb_tcp_client_start (aw_mb_m_tcp_client_t *p_client, uint32_t timeout);

/**
 * \brief 断开连接
 *
 * \param[in] p_client  客户端操作数据
 *
 * \return 成功返回AW_MB_ERR_NOERR.错误返回相应错误码.
 */
aw_mb_err_t aw_mb_tcp_client_stop (aw_mb_m_tcp_client_t *p_client);

/**
 * \brief 向服务端发送数据
 *
 * \param[in] p_client  客户端操作数据
 * \param[in] p_sndbuf  发送数据缓冲区
 * \param[in] snd_len   发送数据长度
 *
 * \return 成功返回AW_MB_ERR_NOERR.错误返回相应错误码.
 */
aw_mb_err_t aw_mb_tcp_client_send (aw_mb_m_tcp_client_t *p_client,
                                   uint8_t              *p_sndbuf,
                                   uint16_t              snd_len);

/**
 * \brief 接收服务端数据
 *
 * \param[in]     p_client  客户端操作数据
 * \param[out]    p_rcvbuf  接收缓冲区
 * \param[out]    p_rcvlen  接收到数据长度
 * \param[in,out] p_time    [in]超时时间， [out]接收所用时间, 接收一帧成功，该值有效.
 *
 * \return 成功返回AW_MB_ERR_NOERR.错误返回相应错误码.
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

