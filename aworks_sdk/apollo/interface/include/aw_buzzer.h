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
 * \brief 蜂鸣器操作接口
 *
 * 使用本服务需要包含以下头文件:
 * \code
 * #include "aw_buzzer.h"
 * \endcode
 *
 * \par 使用示例
 * \code
 * #include "aw_buzzer.h"
 *
 * (1)GPIO驱动直流蜂鸣器
 * aw_buzzer_on();      // 蜂鸣器鸣叫
 * aw_buzzer_off();     // 关闭蜂鸣器鸣叫
 * aw_buzzer_beep(100); // 蜂鸣器鸣叫100毫秒
 *
 * (2)强度调节设备驱动无源蜂鸣器
 * aw_buzzer_loud_set(80);  // 蜂鸣器鸣叫等级
 * aw_buzzer_beep(100);     // 蜂鸣器鸣叫100毫秒
 *
 * \endcode
 *
 * \note 使用顺序约束
 *       [1]先使用aw_buzzer_loud_set
 *       [2]后使用aw_buzzer_beep/aw_buzzer_on/aw_buzzer_off
 *
 * \internal
 * \par modification history
 * - 1.01 14-07-09  ops, add aw_buzzer_loud_set
 * - 1.00 13-03-08  orz, first implementation.
 * \endinternal
 */

#ifndef __AW_BUZZER_H
#define __AW_BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_buzzer
 * \copydoc aw_buzzer.h
 * @{
 */

/**
 * \brief 打开蜂鸣器鸣叫
 *
 * \retval AW_OK       操作成功，蜂鸣器开始鸣叫
 * \retval -AW_ENODEV  操作失败，系统无蜂鸣器设备
 */
aw_err_t aw_buzzer_on (void);

/**
 * \brief 关闭蜂鸣器鸣叫
 *
 * \retval AW_OK       操作成功，蜂鸣器停止鸣叫
 * \retval -AW_ENODEV  操作失败，系统无蜂鸣器设备
 */
aw_err_t aw_buzzer_off (void);

/**
 * \brief 蜂鸣器鸣叫一声
 *
 * \param[in] beep_time_ms 鸣叫时间，单位毫秒。
 *
 * \retval AW_OK       操作成功，蜂鸣器开始鸣叫，鸣叫指定时间后自动关闭
 * \retval -AW_ENODEV  操作失败，系统无蜂鸣器设备
 *
 * \note 若操作成功，则这个接口会使调用线程休眠，休眠时间为鸣叫的时间。
 *       不建议多任务同时使用，多任务同时使用这个接口可能会造成非预期
 *       的鸣叫效果。
 */
aw_err_t aw_buzzer_beep (unsigned int beep_time_ms);

/**
 * \brief 设置蜂鸣器鸣叫的响度
 *
 * \param[in] beep_level  鸣叫响度等级 (0 - 100)，0：静音；100：最大响度。
 *
 * \retval AW_OK        操作成功，响度设置成功
 * \retval -AW_ENODEV   操作失败，系统无蜂鸣器设备
 * \retval -AW_ENOTSUP  操作失败，该蜂鸣器设备不支持响度调节
 *
 * \note 该接口仅对支持响度调节的蜂鸣器有效，如PWM控制的无源蜂鸣器，部分
 *       普通使用GPIO直接控制的有源蜂鸣器设备，响度由硬件驱动电路固定，
 *       无法调节，则调用该接口使无效的，将返回-AW_ENOTSUP。
 */
aw_err_t aw_buzzer_loud_set (unsigned int beep_level);

/** @} grp_aw_if_buzzer */

#ifdef __cplusplus
}
#endif

#endif /* __AW_BUZZER_H */

/* end of file */
