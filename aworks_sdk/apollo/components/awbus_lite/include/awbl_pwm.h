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
 * \brief Freescale i.MX283 PWM外设
 *
 * 使用本模块需要包含以下头文件：
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
 * \brief AWBus PWM 服务实例的相关信息
 */
struct awbl_pwm_servinfo {

    /** \brief 支持的最小PWM通道id号 */
    int pwm_min_id;

    /** \brief 支持的最大PWM通道id号 */
    int pwm_max_id;
};

/**
 * \brief AWBus PWM 服务函数
 */
struct awbl_pwm_servopts {

    /** \brief 配置PWM有效时间和周期 */
    aw_err_t (*pfunc_pwm_config) (void         *p_cookie,
                                  int           pid,
                                  unsigned long duty_ns,
                                  unsigned long period_ns);

    /** \brief 使能PWM */
    aw_err_t (*pfunc_pwm_enable) (void *p_cookie,
                                  int   pid);

    /** \brief 禁能PWM */
    aw_err_t (*pfunc_pwm_disable) (void *p_cookie,
                                   int   pid);
  
    /** \brief PWM精准输出 */
    aw_err_t (*pfunc_pwm_output) (void *p_cookie,
                                  int   pid,
                                  unsigned long period_num);
};

/**
 * \brief AWBus PWM 服务实例
 */
struct awbl_pwm_service {

    /** \brief 指向下一个 PWM 服务 */
    struct awbl_pwm_service *p_next;

    /** \brief GPIO 服务的相关信息 */
    const struct awbl_pwm_servinfo *p_servinfo;

    /** \brief GPIO 服务的相关函数 */
    const struct awbl_pwm_servopts *p_servfuncs;

    /**
     * \brief PWM 服务自定义的数据
     *
     * 此参数在PWM接口模块搜索服务时由驱动设置，在调用服务函数时作为第一个参数。
     */
    void *p_cookie;
};

/**
 * \brief AWBus PWM 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_init2() 之后调用
 */
void awbl_pwm_init(void);


#ifdef __cplusplus
}
#endif

#endif

/* end of file */
