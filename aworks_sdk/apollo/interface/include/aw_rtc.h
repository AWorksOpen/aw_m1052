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
 * \brief RTC (实时时钟) 标准服务
 *
 * 使用本服务需要包含头文件 aw_rtc.h
 *
 * \par 使用示例
 * \code
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 13-09-05  zen, first implementation
 * \endinternal
 */

#ifndef __AW_RTC_H
#define __AW_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_time.h"

/**
 * \addtogroup grp_aw_if_rtc
 * \copydoc aw_rtc.h
 * @{
 */


/**
 * \brief 设置RTC时间
 * \param[in] rtc_id   RTC 设备编号, 从0开始递增
 * \param[in] p_tm     要设置的时间
 *
 * \retval  AW_OK      成功
 * \retval -AW_ENODEV  RTC设备不存在
 * \retval -AW_EIO     设置失败
 */
aw_err_t aw_rtc_time_set (int rtc_id, aw_tm_t *p_tm);


/**
 * \brief 读取RTC时间
 * \param[in]  rtc_id  RTC 设备编号, 从0开始递增
 * \param[out] p_tm    读取的时间
 *
 * \retval  AW_OK      成功
 * \retval -AW_ENODEV  RTC设备不存在
 * \retval -AW_EIO     读取失败
 */
aw_err_t aw_rtc_time_get (int rtc_id, aw_tm_t *p_tm);


/**
 * \brief 读取RTC日历时间 (秒、纳秒表示)
 * \param[in]  rtc_id  RTC 设备编号, 从0开始递增
 * \param[out] p_tv    读取的日历时间
 *
 * \retval  AW_OK      成功
 * \retval -AW_ENODEV  RTC设备不存在
 * \retval -AW_EIO     读取失败
 */
aw_err_t aw_rtc_timespec_get(int rtc_id, aw_timespec_t *p_tv);


/**
 * \brief 设置RTC日历时间 (秒、纳秒表示)
 * \param[in] rtc_id   RTC 设备编号, 从0开始递增
 * \param[in] p_tv     要设置的日历时间
 *
 * \retval  AW_OK      成功
 * \retval -AW_ENODEV  RTC设备不存在
 * \retval -AW_EIO     设置失败
 */
aw_err_t aw_rtc_timespec_set(int rtc_id, aw_timespec_t *p_tv);


/** @} grp_aw_if_rtc */

#ifdef __cplusplus
}
#endif

#endif /* __AW_RTC_H */

/* end of file */

