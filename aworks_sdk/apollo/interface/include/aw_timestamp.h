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
 * \brief 时间戳相关服务
 *
 * 需要包含了 aw_timestamp.h 就可以使用本服务。
 *
 * \par 简单示例:计算程序执行时间,微秒级别甚至纳秒级别的时间测量
 * \code
 *
 * #include "aw_timestamp.h"
 * 
 * int aw_main()
 * {
 *     aw_timestamp_t          stamps;
 *     aw_timestamp_freq_t     freq;
 *     uint32_t                ns,us;
 * 
 *     stamps = aw_timestamp_get();
 *     //  do something
 *     stamps = aw_timestamp_get() - stamps;
 * 
 *     us = aw_timestamps_to_us(stamps);
 *     ns = aw_timestamps_to_ns(stamps);
 * 
 *     aw_kprintf("stamps used=%d, time=%d us,%d ns",
 *                 (uint32_t)stamps,
 *                 us,
 *                 ns);
 *     return 0;
 * }
 * \endcode
 *
 * //更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.02 19-05-16  sni,   精简接口，使用更容易
 * - 1.01 14-08-18  tee,   modified and tested.
 * - 1.00 13-02-27  zen,   first implementation
 * \endinternal
 */

#ifndef __AW_TIMESTAMP_H
#define __AW_TIMESTAMP_H

#include "aw_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_timestamp
 * \copydoc aw_timestamp.h
 * @{
 */

/** \brief 定义时间戳频率的类型 */
typedef uint32_t    aw_timestamp_freq_t;

/** \brief 定义时间戳类型 */
typedef uint64_t    aw_timestamp_t;

/**
 * \brief 获取当前时刻时间戳
 *
 * 此函数可以在中断中调用，如果当前系统因为缺乏定时器而不支持
 * 获取时间戳，则会返回0
 *
 * \return 系统当前时间戳值(时间戳定时器的计数值)
 */
aw_timestamp_t aw_timestamp_get(void);


/**
 * \brief 获取系统时间戳定时器的计数频率
 *
 * 不支持频率为1Hz以下情况，如果系统中不存在时间戳定时器，
 * 则会返回1
 * 
 * 在isr中调用的行为是不确定的
 *
 * \return 时间戳定时器频率, Hz
 */
aw_timestamp_freq_t aw_timestamp_freq_get(void);

/**
 * \brief  将时间戳转换为时间(us)
 *
 * 此函数会四舍五入，这是值得注意的地方
 * 例如如果实际对应的是1.499999us,则转换结果为1 us
 * 而实际对应1.5000000us的话，则转换结果为2us
 * 如果四舍五入不是你期望的行为，则可以根据频率自行计算
 *
 * 在ISR中调用也许会延长中断处理时间
 *
 * \param  stamps 时间戳个数
 * \return 根据时间戳个数，转换得到的时间us数
 */
uint32_t aw_timestamps_to_us(aw_timestamp_t stamps);

/**
 * \brief  将时间戳转换为时间(ns)
 *
 * 此函数会四舍五入，这是值得注意的地方
 * 例如如果实际对应的是1.499999ns,则转换结果为1 ns
 * 而实际对应1.5000000ns的话，则转换结果为2ns
 * 如果四舍五入不是你期望的行为，则可以根据频率自行计算
 *
 * 在ISR中调用也许会延长中断处理时间
 *
 * \param  stamps 时间戳个数
 * \return 根据时间戳个数，转换得到的时间ns数
 */
uint32_t aw_timestamps_to_ns(aw_timestamp_t stamps);

/** @} grp_aw_if_system */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif    /* __AW_TIMESTAMP_H    */

/* end of file */
