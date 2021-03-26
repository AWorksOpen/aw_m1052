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
 * \brief Freescale i.MX283 PWM����
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_pwm.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.01 14-07-01  ops, modified the definition of awbl_pwm_servinfo
 * - 1.00 14-06-05  ops, first implementation
 * \endinternal
 */

#ifndef __AWBL_PWM_H
#define __AWBL_PWM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief AWBus PWM ����ʵ���������Ϣ
 */
struct awbl_pwm_servinfo {

    /** \brief ֧�ֵ���СPWMͨ��id�� */
    int pwm_min_id;

    /** \brief ֧�ֵ����PWMͨ��id�� */
    int pwm_max_id;
};

/**
 * \brief AWBus PWM ������
 */
struct awbl_pwm_servopts {

    /** \brief ����PWM��Чʱ������� */
    aw_err_t (*pfunc_pwm_config) (void         *p_cookie,
                                  int           pid,
                                  unsigned long duty_ns,
                                  unsigned long period_ns);

    /** \brief ʹ��PWM */
    aw_err_t (*pfunc_pwm_enable) (void *p_cookie,
                                  int   pid);

    /** \brief ����PWM */
    aw_err_t (*pfunc_pwm_disable) (void *p_cookie,
                                   int   pid);
  
    /** \brief PWM��׼��� */
    aw_err_t (*pfunc_pwm_output) (void *p_cookie,
                                  int   pid,
                                  unsigned long period_num);
};

/**
 * \brief AWBus PWM ����ʵ��
 */
struct awbl_pwm_service {

    /** \brief ָ����һ�� PWM ���� */
    struct awbl_pwm_service *p_next;

    /** \brief GPIO ����������Ϣ */
    const struct awbl_pwm_servinfo *p_servinfo;

    /** \brief GPIO �������غ��� */
    const struct awbl_pwm_servopts *p_servfuncs;

    /**
     * \brief PWM �����Զ��������
     *
     * �˲�����PWM�ӿ�ģ����������ʱ���������ã��ڵ��÷�����ʱ��Ϊ��һ��������
     */
    void *p_cookie;
};

/**
 * \brief AWBus PWM �ӿ�ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_init2() ֮�����
 */
void awbl_pwm_init(void);


#ifdef __cplusplus
}
#endif

#endif

/* end of file */
