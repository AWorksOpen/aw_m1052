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
 * \brief modbus crc header file
 *
 * \internal
 * \par modification history
 * - 1.02 2015-05-18  cod, fix code style and comments, a little improve.
 * - 1.01 2013-03-29  liuweiyun, fix code style and comments, a little improve.
 * - 1.00 2012-05-10  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_CRC_H /* {  */
#define __AW_MB_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_crc
 * \copydoc aw_mb_crc.h
 * @{
 */


/**
 * \brief 获取PDU数据帧CRC校验值
 *
 * \param[in] p_frame  帧数据缓冲区
 * \param[in] len      数据长度
 *
 * \return 缓冲区不为空且数据长度大于0,返回CRC校验值； 否则，返回0
 */
uint16_t aw_mb_crc16(uint8_t *p_frame, uint16_t len );

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_crc */

#endif /* } __AW_MB_CRC_H */

/* end of file */
