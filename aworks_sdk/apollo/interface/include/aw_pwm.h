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
 * \brief PWM (脉冲宽度调制) 标准服务
 *
 * 使用本服务需要包含以下头文件:
 * \code
 * #include "aw_pwm.h"
 * \endcode
 *
 * \par 使用示例
 * \code
 * #include "aw_pwm.h"
 *
 * //duty_ns   = 7000000    有效时间 = 7ms
 * //period_ns = 10000000   周期           = 10ms
 * //PWM通道0，输出占空比为70%的波形
 * aw_pwm_config(PWM0, 70000000, 100000000);
 * aw_pwm_enable(PWM0);
 *
 * \endcode
 *
 * \par 使用说明
 * aw_pwm_config/aw_pwm_enable/aw_pwm_disable之间无使用顺序约束
 *
 * \internal
 * \par modification history
 * - 1.00 14-06-05  chenshuyi, first implementation
 * \endinternal
 */

#ifndef __AW_PWM_H
#define __AW_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_pwm
 * \copydoc aw_pwm.h
 * @{
 */

/**
 * \brief 设置PWM设备的有效时间和周期
 *
 * \param[in] pid        PWM设备通道号
 * \param[in] duty_ns    有效时间(单位: 纳秒)
 * \param[in] period_ns  PWM波形输出周期(单位: 纳秒)
 *
 * \retval    AW_OK       成功
 * \retval   -AW_ENXIO    \a pid 为不存在的通道号
 * \retval   -AW_EINVAL   \a 无效参数
 *
 */
aw_err_t aw_pwm_config(int pid, unsigned long duty_ns, unsigned long period_ns);

/**
 * \brief 使能PWM设备
 *
 * \param[in] pid  PWM设备通道号
 *
 */
aw_err_t aw_pwm_enable(int pid);

/**
 * \brief 禁用PWM设备
 *
 * \param[in] pid  PWM设备通道号
 *
 */
aw_err_t aw_pwm_disable(int pid);

/**
 * \brief PWM精确输出
 *
 * \param[in] pid  PWM设备通道号
 * \param[in] period_num 要输出的pwm周期个数
 *
 */
aw_err_t aw_pwm_output(int pid, unsigned long period_num);

/** @} grp_aw_if_pwm */

#ifdef __cplusplus
}
#endif

#endif /* __AW_PWM_H */

/* end of file */
