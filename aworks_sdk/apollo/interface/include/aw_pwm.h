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
 * \brief PWM (�����ȵ���) ��׼����
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ�:
 * \code
 * #include "aw_pwm.h"
 * \endcode
 *
 * \par ʹ��ʾ��
 * \code
 * #include "aw_pwm.h"
 *
 * //duty_ns   = 7000000    ��Чʱ�� = 7ms
 * //period_ns = 10000000   ����           = 10ms
 * //PWMͨ��0�����ռ�ձ�Ϊ70%�Ĳ���
 * aw_pwm_config(PWM0, 70000000, 100000000);
 * aw_pwm_enable(PWM0);
 *
 * \endcode
 *
 * \par ʹ��˵��
 * aw_pwm_config/aw_pwm_enable/aw_pwm_disable֮����ʹ��˳��Լ��
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
 * \brief ����PWM�豸����Чʱ�������
 *
 * \param[in] pid        PWM�豸ͨ����
 * \param[in] duty_ns    ��Чʱ��(��λ: ����)
 * \param[in] period_ns  PWM�����������(��λ: ����)
 *
 * \retval    AW_OK       �ɹ�
 * \retval   -AW_ENXIO    \a pid Ϊ�����ڵ�ͨ����
 * \retval   -AW_EINVAL   \a ��Ч����
 *
 */
aw_err_t aw_pwm_config(int pid, unsigned long duty_ns, unsigned long period_ns);

/**
 * \brief ʹ��PWM�豸
 *
 * \param[in] pid  PWM�豸ͨ����
 *
 */
aw_err_t aw_pwm_enable(int pid);

/**
 * \brief ����PWM�豸
 *
 * \param[in] pid  PWM�豸ͨ����
 *
 */
aw_err_t aw_pwm_disable(int pid);

/**
 * \brief PWM��ȷ���
 *
 * \param[in] pid  PWM�豸ͨ����
 * \param[in] period_num Ҫ�����pwm���ڸ���
 *
 */
aw_err_t aw_pwm_output(int pid, unsigned long period_num);

/** @} grp_aw_if_pwm */

#ifdef __cplusplus
}
#endif

#endif /* __AW_PWM_H */

/* end of file */
