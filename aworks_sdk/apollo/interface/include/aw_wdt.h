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
 * \brief 看门狗标准接口
 *
 * 本模块提供硬件定时器服务。
 * 使用本服务需要包含头文件 aw_wdt.h
 *
 * 本服务提供了对软件看门狗添加、喂狗的接口，如果系统未在规定时间内
 * 进行喂狗，那么就会导致系统复位。
 *
 * 注意：在系统中规定，硬件看门狗的时间必须大于1ms。
 *
 * \par 简单示例
 * \code
 *
 * // 设置看门狗时间为1200ms
 * struct awbl_wdt   wdt_dev;
 * aw_wdt_add(&wdt_dev, 1200);
 *
 * // 喂狗
 * aw_wdt_feed(&wdt_dev);
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-08-27  may, first implementation
 * \endinternal
 */


#ifndef __AW_WDT_H
#define __AW_WDT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "aw_timer.h"

/**
 * \brief 看门狗设备
 */
struct awbl_wdt {

    /** \brief 看门狗时间 */
    uint32_t   period_ms;

    /** \brief 用于看门狗定时的软件定时器 */
    aw_timer_t sftimer;
};

/**
 * \addtogroup grp_aw_if_watchdog
 * \copydoc aw_wdt.h
 * @{
 */

/**
 * \brief 添加看门狗，并设置看门狗时间
 *
 * \param[in] p_wdt   看门狗设备
 * \param[in] t_ms    看门狗时间(ms)
 *
 * \retval   AW_OK       成功
 * \retval  -AW_EINVAL   \e p_wdt 值无效
 * \retval  -AW_EPERM    操作不允许
 */
aw_err_t  aw_wdt_add (struct awbl_wdt  *p_wdt, uint32_t  t_ms);

/**
 * \brief 执行喂狗操作
 *
 * \param[in] p_wdt    看门狗设备
 *
 * \retval  AW_OK        成功
 * \retval  -AW_EINVAL   \e p_wdt 值无效
 * \retval  -AW_EPERM    操作不允许
 */
aw_err_t  aw_wdt_feed (struct awbl_wdt  *p_wdt);

/** @} grp_aw_if_watchdog */

#ifdef __cplusplus
}
#endif

#endif /* __AW_WDT_H */

/* end of file */
