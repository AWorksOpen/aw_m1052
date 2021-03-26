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
 * \brief RTK 时间戳相关
 *
 * RTK 时间戳定义了一个连续运行的64位时间戳
 * 哪怕频率是4GHZ，这也可以保证145年不会溢出
 * 用于更为统计RTK任务的运行时间
 *  *
 * //更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 13-02-27  zen,   first implementation
 * \endinternal
 */

#ifndef __RTK_TIMESTAMP_H
#define __RTK_TIMESTAMP_H

#include "aw_timestamp.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/


/** \brief 定义时间戳频率的类型 */
typedef aw_timestamp_freq_t rtk_timestamp_freq_t;
typedef aw_timestamp_t      rtk_timestamp_t;

/**
 * \brief 获取当前时刻时间戳
 *
 * \return 系统当前时间戳值(时间戳定时器的计数值)
 */
#define rtk_timestamp_get aw_timestamp_get


/**
 * \brief 获取时间戳定时器频率
 *
 * \return 时间戳定时器频率, Hz
 */
static inline aw_timestamp_freq_t rtk_timestamp_freq_get(void)
{
    return aw_timestamp_freq_get();
}



/** @} grp_aw_if_system */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif    /* __RTK_TIMESTAMP_H    */

/* end of file */
