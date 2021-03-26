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

/** \brief 功能码处理类型 */
typedef struct {
    uint8_t                 funcode; /**< \brief 功能码 */
    aw_mb_fn_code_handler_t handler; /**< \brief 功能码处理函数 */
} aw_funcode_handle;

/**
 * \brief 获取从站信息
 * \note  获取Modbus从栈类型说明，当前状态和其他一些特殊信息。
 *
 * \param[in]     slave    从机句柄
 * \param[in,out] p_frame  从站信息缓冲区
 *
 * \return ID从站信息长度
 */
uint16_t aw_mb_get_slave_id (aw_mb_slave_t slave, uint8_t *p_frame);

/**
 * \brief 获取功能码的处理函数
 *
 * \param[in] slave    从机句柄
 * \param[in] funcode  功能码
 *
 * \return 功能码对应的处理函数
 */
aw_mb_fn_code_handler_t aw_mb_fn_code_handler_get (aw_mb_slave_t slave,
                                                   uint8_t       funcode);

/**
 * \brief 读线圈功能码处理函数.
 * \note  如果需要该功能，需要使用函数\a aw_mb_slave_register_handler 来注册。
 *        同时要使用\a aw_mb_slave_register_callback 注册其读线圈具体操作回调函数
 *
 * \param[in]     slave  从机句柄
 * \param[in,out] p_pdu  带功能码的PDU缓冲区
 * \param[in,out] p_len  PDU长度指针
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 */
aw_mb_exception_t aw_mb_fn_read_coils (aw_mb_slave_t  slave,
                                       uint8_t       *p_pdu,
                                       uint16_t      *p_len);

/**
 * \brief 读离散量功能码处理函数.
 * \note  如果需要该功能，需要使用函数\a aw_mb_slave_register_handler 来注册。
 *        同时要使用\a aw_mb_slave_register_callback 注册其读离散量具体操作回调函数
 *
 * \param[in]     slave   从机句柄
 * \param[in,out] p_pdu  带功能码的PDU缓冲区
 * \param[in,out] p_len  PDU长度
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 */
aw_mb_exception_t aw_mb_fn_read_discrete_inputs (aw_mb_slave_t  slave,
                                                 uint8_t       *p_pdu,
                                                 uint16_t      *p_len);

/**
 * \brief 读保存寄存器功能码处理函数.
 * \note  如果需要该功能，需要使用函数\a aw_mb_slave_register_handler 来注册。
 *        同时要使用\a aw_mb_slave_register_callback 注册其读保存寄存器具体操作回调函数
 *
 * \param[in]     slave  从机句柄
 * \param[in,out] p_pdu  带功能码的PDU缓冲区
 * \param[in,out] p_len  PDU长度
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 */
aw_mb_exception_t aw_mb_fn_read_hold_regs (aw_mb_slave_t slave,
                                           uint8_t       *p_pdu,
                                           uint16_t      *p_len);

/**
 * \brief 读输入寄存器功能码处理函数.
 * \note  如果需要该功能，需要使用函数\a aw_mb_slave_register_handler 来注册。
 *        同时要使用\a aw_mb_slave_register_callback
 *        注册其读输入寄存器具体操作回调函数
 *
 * \param[in]     slave  从机句柄
 * \param[in,out] p_pdu  带功能码的PDU缓冲区
 * \param[in,out] p_len  PDU长度
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 */
aw_mb_exception_t aw_mb_fn_read_input_regs (aw_mb_slave_t slave,
                                            uint8_t       *p_pdu,
                                            uint16_t      *p_len);

/**
 * \brief 写单个线圈功能码处理函数.
 * \note  如果需要该功能，需要使用函数\a aw_mb_slave_register_handler 来注册。
 *        同时要使用\a aw_mb_slave_register_callback
 *        注册其写线圈具体操作回调函数；如果写多个线圈已经有注册写线圈具体操作回调函数，
 *        就不需要再注册。
 *
 * \param[in]     slave  从机句柄
 * \param[in,out] p_pdu  带功能码的PDU缓冲区
 * \param[in,out] p_len  PDU长度
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 */
aw_mb_exception_t aw_mb_fn_write_single_coil (aw_mb_slave_t slave,
                                               uint8_t      *p_pdu,
                                              uint16_t      *p_len);

/**
 * \brief 写单个寄存器功能码处理函数.
 * \note  如果需要该功能，需要使用函数\a aw_mb_slave_register_handler 来注册。
 *        同时要使用\a aw_mb_slave_register_callback
 *        注册其写寄存器具体操作回调函数；如果写多个寄存器已经有注册写寄存器具体操作
 *        回调函数，就不需要再注册
 *
 * \param[in]     slave  从机句柄
 * \param[in,out] p_pdu  带功能码的PDU缓冲区
 * \param[in,out] p_len  PDU长度
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 */
aw_mb_exception_t aw_mb_fn_write_single_reg (aw_mb_slave_t slave,
                                             uint8_t       *p_pdu,
                                             uint16_t      *p_len);

/**
 * \brief 写多个线圈功能码处理函数.
 * \note  如果需要该功能，需要使用函数\a aw_mb_slave_register_handler 来注册。
 *        同时要使用\a aw_mb_slave_register_callback
 *        注册其写线圈具体操作回调函数；如果写单个线圈已经有注册写线圈具体操作回调函数，
 *        就不需要再注册
 *
 * \param[in]     slave  从机句柄
 * \param[in,out] p_pdu  带功能码的PDU缓冲区
 * \param[in,out] p_len  PDU长度
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 */
aw_mb_exception_t aw_mb_fn_write_multiple_coils (aw_mb_slave_t slave,
                                                 uint8_t       *p_pdu,
                                                 uint16_t      *p_len);

/**
 * \brief 写多个寄存器功能码处理函数.
 * \note  如果需要该功能，需要使用函数\a aw_mb_slave_register_handler 来注册。
 *        同时要使用\a aw_mb_slave_register_callback
 *        注册其写寄存器具体操作回调函数；如果写多个寄存器已经有注册写寄存器具体操作
 *        回调函数，就不需要再注册
 *
 * \param[in]     slave  从机句柄
 * \param[in,out] p_pdu  带功能码的PDU缓冲区
 * \param[in,out] p_len  PDU长度
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 */
aw_mb_exception_t aw_mb_fn_write_multiple_regs (aw_mb_slave_t slave,
                                                uint8_t      *p_pdu,
                                                uint16_t     *p_len);

/**
 * \brief 读写多个寄存器功能码处理函数.
 * \note  如果需要该功能，需要使用函数\a aw_mb_slave_register_handler 来注册。
 *        同时要使用\a aw_mb_slave_register_callback
 *        注册其读和写寄存器具体操作回调函数；如果已经注册其读和写寄存器具体操作
 *        回调函数，就不需要再注册
 *
 * \param[in]     slave  从机句柄
 * \param[in,out] p_pdu  带功能码的PDU缓冲区
 * \param[in,out] p_len  PDU长度
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 */
aw_mb_exception_t aw_mb_fn_rw_multiple_regs (aw_mb_slave_t  slave,
                                             uint8_t       *p_pdu,
                                             uint16_t      *p_len);
#if 0
/**
 * \brief 报告从机信息功能码处理函数.
 * \note  如果需要该功能，需要使用函数\a aw_mb_slave_register_handler 来注册。
 *        从机可以使用\a aw_mb_slave_set_id 来设置从机的信息。
 *
 * \param[in]     slave  从机句柄
 * \param[in,out] p_pdu  带功能码的PDU缓冲区
 * \param[in,out] p_len  PDU长度
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
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

