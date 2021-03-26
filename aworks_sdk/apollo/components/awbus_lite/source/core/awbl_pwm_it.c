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
 * \brief 基于PWM的强度调节设备驱动
 *
 * \internal
 * \par modification history:
 * - 1.01 14-11-01  ops, redefine the intensity device by using new specification.
 * - 1.00 14-07-03  ops, first implemetation
 * \endinternal
 */

#include "apollo.h"
#include "awbus_lite.h"
#include "driver/intensity/awbl_pwm_it.h"
#include "aw_pwm.h"
#include "aw_vdebug.h"
#include "aw_assert.h"

#define __PWM_IT_DEV_GET(p_dev)   AW_CONTAINER_OF(p_dev, awbl_pwm_it_dev_t, idev)

int awbl_pwm_it_level_set (aw_it_dev_t *p_dev, int level) {

    awbl_pwm_it_dev_t *p_pwm_it_dev = __PWM_IT_DEV_GET(p_dev);
    
    int           ret     = 0;
    unsigned long duty_ns = 0;

    if (level > p_pwm_it_dev->idev.max_level) {

        return -AW_EINVAL;
    }

    duty_ns = (p_pwm_it_dev->param.period_ns /p_pwm_it_dev->idev.max_level )* level;

    if (duty_ns == 0) {

        ret = aw_pwm_disable(p_pwm_it_dev->param.pwm_channel_id);
    }
    else {

        ret = aw_pwm_config(p_pwm_it_dev->param.pwm_channel_id, 
                            duty_ns, 
                            p_pwm_it_dev->param.period_ns);
        if (ret != AW_OK)
            return ret;
        ret = aw_pwm_enable(p_pwm_it_dev->param.pwm_channel_id);
    }

    return ret;
}

int awbl_pwm_it_level_get (aw_it_dev_t *p_dev) {

    awbl_pwm_it_dev_t *p_pwm_it_dev = __PWM_IT_DEV_GET(p_dev);

    return p_pwm_it_dev->idev.current_level;
}

/*
 * \brief the constructor of derived class pwm_it
 */
aw_it_dev_t* awbl_pwm_it_ctor(awbl_pwm_it_dev_t *p_dev, 
                              int                max_level,
                              int                pwm_channel_id,
                              unsigned long      period_ns) 
{

    aw_assert(p_dev);

    p_dev->param.period_ns = period_ns;
    p_dev->param.pwm_channel_id = pwm_channel_id;
    p_dev->idev.current_level = 0;
    p_dev->idev.max_level = max_level;
    p_dev->idev.pfn_it_level_get = awbl_pwm_it_level_get;
    p_dev->idev.pfn_it_level_set = awbl_pwm_it_level_set;

    return &p_dev->idev;
}

/* end of file*/
