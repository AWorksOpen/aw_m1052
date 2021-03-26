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
 * \brief AWBus RTC 接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_rtc.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 13-09-05  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_RTC_H
#define __AWBL_RTC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_rtc.h"

/**
 * \brief AWBus RTC 服务实例的相关信息
 */
struct awbl_rtc_servinfo {

    /** \brief RTC 设备编号 */
    int rtc_id;
};

/**
 * \brief AWBus RTC 服务函数
 */
struct awbl_rtc_servopts {

    /** \brief 读取 RTC 时间 */
    aw_err_t (*time_get)(void *p_cookie, aw_tm_t *p_tm);

    /** \brief 设置 RTC 时间 */
    aw_err_t (*time_set)(void *p_cookie, aw_tm_t *p_tm);

    /** \brief RTC 设备控制(保留) */
    aw_err_t (*dev_ctrl)(void *p_cookie, int req, void *arg);

    /** \brief 读取 RTC 日历时间 */
    aw_err_t (*timespec_get)(void *p_cookie, aw_timespec_t *p_tv);

    /** \brief 设置 RTC 日历时间 */
    aw_err_t (*timespec_set)(void *p_cookie, aw_timespec_t *p_tv);
};

/**
 * \brief AWBus RTC 服务实例
 */
struct awbl_rtc_service {

    /** \brief 指向下一个 RTC 服务 */
    struct awbl_rtc_service *p_next;

    /** \brief RTC 服务的相关信息 */
    const struct awbl_rtc_servinfo *p_servinfo;

    /** \brief RTC 服务的相关函数 */
    const struct awbl_rtc_servopts *p_servopts;

    /**
     * \brief RTC 服务自定义的数据
     *
     * 此参数在RTC接口模块搜索服务时由驱动设置，在调用服务函数时作为第一个参数。
     */
    void *p_cookie;
};

/**
 * \brief AWBus RTC 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_init2() 之后调用
 */
void awbl_rtc_init(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus 	*/

#endif  /* __AWBL_RTC_H */

/* end of file */

