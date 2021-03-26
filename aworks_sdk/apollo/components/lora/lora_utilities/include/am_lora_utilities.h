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
 * \brief 工具函数实现
 *
 * \internal
 * \par Modification history
 * - 1.0.0 17-09-21  sky, first implementation.
 * \endinternal
 */

#ifndef __AM_LORA_UTILITIES_H
#define __AM_LORA_UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

/**
 * @addtogroup am_if_lora_utilities
 * @copydoc am_lora_utilities.h
 * @{
 */

/**
 * \brief utilities get rtc_time
 * \param [in] p_time : point to the time struct
 */
void am_lora_utilities_rtc_time_get (struct tm *p_time);

/**
 * \brief utilities set rtc_time
 * \param [in] p_time : point to the time struct
 */
void am_lora_utilities_rtc_time_set (struct tm *p_time);

/**
 * \brief utilities get the utc second (from 1979 0 0)
 * \param [in] none
 * \retval utc second
 */
uint32_t am_lora_utilities_rtc_sec_get (void);

/**
 * \brief utilities set the utc second (from 1979 0 0)
 * \param [in] value : second which is wait for set
 */
void am_lora_utilities_rtc_sec_set (uint32_t value);

/**
 * \brief enter sleep mode (for lowpower)
 * \param [in] none
 */
void am_lora_utilities_sys_sleep (void);

/**
 * \brief 关闭CPU中断
 * \attention 关闭时间一定要短且必须与 am_lora_cpu_unlock() 函数搭配使用
 * \return 中断关闭信息
 */
uint32_t am_lora_cpu_lock (void);

/**
 * \brief 打开CPU中断
 * \attention 必须与 am_lora_cpu_lock() 函数搭配使用
 * \param[in] key : 中断关闭信息
 * \return    无
 */
void am_lora_cpu_unlock (uint32_t cpu_key);

/**
 * \brief 毫秒级别延时
 * \param[in] nms : 延时的毫秒数
 * \return 无
 */
void am_lora_mdelay (uint32_t nms);

/**
 * \brief 微秒级别延时
 * \param[in] nus : 延时的微秒数
 * \return 无
 */
void am_lora_udelay (uint32_t nus);

/**
 * \brief 声明标准输出函数
 */
extern int (*am_lora_kprintf)(const char *fmt, ...);

/**
 * \brief 初始化标准输出函数
 * \param[in] pfunc_printf : 平台的标准输出函数
 * \return 无
 */
void am_lora_kprintf_init (int (*pfunc_printf)(const char *fmt, ...));

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LORA_UTILITIES_H */

/* end of file */
