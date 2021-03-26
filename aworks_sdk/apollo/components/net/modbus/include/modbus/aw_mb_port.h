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
 * \brief 初始化串口.
 *
 * \param[in] port                串口号
 * \param[in] dataBits            数据位长度(7bits or 8bits)
 * \param[in] baud_rate           波特率
 * \param[in] parity              奇偶校验模式
 * \param[in] is_fast_reaction    是否使用快速响应模式（只有RTU使用）
 * \param[in] rd_timeout          总超时时间
 * \param[in] rd_interval_timeout 码间超时时间
 *
 * \return 如果没有错误返回 AW_MB_ERR_NOERR，否则返回 AW_MB_ERR_EPORTERR
 */
aw_mb_err_t aw_mb_serial_port_init (uint8_t            port,
                                    uint8_t            databits,
                                    uint32_t           baud_rate,
                                    enum aw_mb_parity  parity,
                                    uint8_t            is_fast_reaction,
                                    uint32_t           rd_timeout,
                                    uint32_t           rd_interval_timeout);

/**
 * \brief 启动串口
 *
 * \param[in] port 串口号
 *
 * \return 无
 */
void aw_mb_serial_port_start (uint32_t port);

/**
 * \brief stop串口
 *
 * \param[in] port 串口号
 *
 * \return 无
 */
void aw_mb_serial_port_stop (uint32_t port);

/**
 * \brief 关闭串口
 *
 * \param[in] port 串口号
 *
 * \return 无
 */
void aw_mb_serial_port_close (uint32_t port);

/**
 * \brief 获取一串字符
 *
 * \param[in]  port   串口号
 * \param[out] p_data 指向接收缓冲区的指针
 * \param[in]  len    期望获取的字符长度
 *
 * \return 实际获取到的长度
 */
int aw_mb_serial_port_gets (uint32_t port, char *p_data ,uint32_t len);

/**
 * \brief 发送一串字符
 *
 * \param[in]  port   串口号
 * \param[out] p_data 指向发送缓冲区的指针
 * \param[in]  len    期望发送的字符长度
 *
 * \return 实际发送的长度
 */
int aw_mb_serial_port_puts (uint32_t port, const char *p_data, uint32_t len);

/******************************************************************************/
/**
 * \brief Modbus TCP初始化
 * \note  配置端口，开始监听
 *
 * \param[in] p_cookie  TCP模式句柄
 * \param[in] tcpport   TCP端口
 *
 * \retval AW_MB_ERR_NOERR 初始化成功
 */
aw_mb_err_t aw_mb_tcp_port_init (void *p_cookie, uint16_t tcpport);

/**
 * \brief 关闭TCP连接
 *
 * \param[in] p_cookie  TCP模式句柄
 *
 * \return 无
 */
void aw_mb_tcp_port_close (void *p_cookie);

/**
 * \brief TCP连接停止
 *
 * \param[in] p_cookie  TCP模式句柄
 *
 * \return 无
 */
void aw_mb_tcp_port_disable (void *p_cookie);


/**
 * \brief 获取TCP请求数据
 *
 * \note 读取一帧完整的Modbus TCP数据, 在接收到数据后调用。获取数据后清零接收，设置初始接收偏移数
 *
 * \param[in]  p_cookie     TCP模式句柄
 * \param[out] pp_tcpframe  请求数据帧缓冲
 * \param[out] p_tcplen     帧数据长度
 *
 */
void aw_mb_tcp_port_getrequest (void     *p_cookie,
                                uint8_t **pp_tcpframe,
                                uint16_t *p_tcplen);

/**
 * \brief TCP服务端响应请求
 * \note  响应请求，发送一帧响应数据回客户端
 *
 * \param[in] p_cookie    TCP模式句柄
 * \param[in] p_tcpframe  帧缓冲
 * \param[in] tcplen      帧数据长度
 *
 * \retval AW_MB_ERR_NOERR 数据帧发送成功
 */
aw_mb_err_t aw_mb_tcp_port_sndresponse (void          *p_cookie,
                                        const uint8_t *p_tcpframe,
                                        uint16_t       tcplen);

/**
 * \brief  轮询监听套接字和当前连接的Modbus TCP客户端的新事件。
 * \note   此功能检查新客户端的连接和已连接客户端发送的请求.
 *         如果一个新的客户端连接，并仍有剩下的客户端插槽（只有一个目前设备支持），
 *         那么连接被接受，并为新的客户端套接字的事件对象被激活。
 *         客户端在发生FD_CLOSE后，客户端连接被释放的。
 *
 * \retval AW_MB_ERR_EIO    内部IO错误
 * \retval AW_MB_ERR_NOERR  没有错误
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

