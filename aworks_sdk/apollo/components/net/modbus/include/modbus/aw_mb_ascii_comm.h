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
 * \brief modbus ASCII模式操作接口.
 *
 * \internal
 * \par modification history
 * - 1.00 2016-03-03 cod, first implementation
 * \endinternal
 */
#ifndef __AW_MB_ASCII_COMM_H
#define __AW_MB_ASCII_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_ascii_comm
 * \copydoc aw_mb_ascii_comm.h
 * @{
 */

/******************************************************************************/
/**
 * \brief 将ASCII 0~9或 A~F转换成实际数据
 *
 * \param[in] x  0~9或 A~F
 *
 * \return 转换后的值
 */
uint8_t aw_mb_xdigit2bin (uint8_t x);
/******************************************************************************/
/**
 * \brief 将0~9或0xA~0xF转换成ASCII
 *
 * \param[in] x  0~0xF
 *
 * \return 转换后的值
 */
uint8_t aw_mb_bin2xdigit (uint8_t x);
/******************************************************************************/
/**
 * \brief 判断传入参数x是否为0~9或 A~F之间的ASCII码
 *
 * \param[in] x  判断值
 * \retval    1  传入值为0~9或 A~F之间的ASCII值
 * \retval    0  传入值的不是ASCII码0~9或 A~F之间的值
 */
uint8_t aw_mb_isxhdigit (uint8_t x);
/******************************************************************************/
/**
 * \brief 获取PDU数据帧LRC校验值
 *
 * \param[in] p_frame  帧数据缓冲区
 * \param[in] len      数据长度
 *
 * \return 缓冲区不为空且数据长度大于0,返回LRC校验值； 否则，返回0
 */
uint8_t aw_mb_lrc (uint8_t *p_frame, uint16_t len);

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_ascii_comm */

#endif

/* end of file */
