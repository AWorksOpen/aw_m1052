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
 * \brief LORA 参数保存接口,将信息写入flash
 *
 * \internal
 * \par Modification history
 * - 1.00 17-08-21  sky, first implementation
 * \endinternal
 */

#ifndef __AM_LORA_STORE_H
#define __AM_LORA_STORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @addtogroup am_if_lora_store
 * @copydoc am_at_lora_store.h
 * @{
 */

/**
 * \brief 获取芯片UID
 *
 * \param[out] p_uid_out : 用来接收UID的缓冲区
 * \param[in]  length    : 需要接收的长度
 *
 * \note KL26 UID为10Byte，接收的长度不得小于10或大于16，10~16之间的Byte用0填充
 *
 * \retval AM_OK      : 获取成功
 * \retval -AM_EINVAL : 参数错误
 */
int am_uid_get (uint8_t *p_uid_out, uint8_t length);

/**
 * \brief 保存信息
 *
 * \param[in] p_info_in  : 需要保存的信息
 * \param[in] length     : 信息的长度
 * \param[in] sector_num : 目标扇区
 *
 * \note 长度必须为4的整数倍否则会出错
 *
 * \retval AM_OK      : 保存成功
 * \retval -AM_EINVAL : 参数错误
 */
int am_lora_save (uint8_t *p_info_in, uint16_t length, uint16_t sector_num);

/**
 * \brief 加载信息
 *
 * \param[out] p_info_out : 用来接收信息的缓冲区
 * \param[in]  length     : 信息的长度
 * \param[in]  sector_num : 目标扇区
 *
 * \retval AM_OK      : 保存成功
 * \retval -AM_EINVAL : 参数错误
 */
int am_lora_load (uint8_t *p_info_out, uint16_t length, uint16_t sector_num);

/**
 * @} am_if_lora_store
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LORA_STORE_H */

/* end of file */
