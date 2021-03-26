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
 * \brief Buzzer driver.
 *
 * \internal
 * \par modification history
 * - 1.02 14-11-01  ops, redefine the abstract buzzer by using new specification.
 * - 1.01 14-07-09  ops, add __buzzer_loud_set.
 * - 1.00 13-03-08  orz, first implementation.
 * \endinternal
 */

#include "awbl_buzzer.h"
#include "aw_common.h"
#include "aw_buzzer.h"
#include "aw_delay.h"
#include "aw_assert.h"

/**
 * \addtogroup aw_buzzer
 * @{
 */

/******************************************************************************/

/** \brief the buzzer device pointer */
aw_local struct awbl_buzzer_dev *__gp_buzzer_dev = NULL;

/******************************************************************************/

/** \brief buzzer beep */
aw_local aw_err_t __buzzer_beep (aw_bool_t on)
{
    if ((NULL != __gp_buzzer_dev) && (NULL != __gp_buzzer_dev->beep)) {
        __gp_buzzer_dev->beep(__gp_buzzer_dev, on);
        return AW_OK;
    }

    return -AW_ENODEV;
}

/******************************************************************************/
aw_local aw_err_t __buzzer_loud_set (int level) {

    if (NULL != __gp_buzzer_dev) {

        if (NULL != __gp_buzzer_dev->loud_set) {
            return __gp_buzzer_dev->loud_set(__gp_buzzer_dev, level);
        }

        return -AW_ENOTSUP;
    }

    return -AW_ENODEV;
}

/******************************************************************************/
aw_err_t aw_buzzer_on (void)
{
    return __buzzer_beep(AW_TRUE);
}

/******************************************************************************/
aw_err_t aw_buzzer_off (void)
{
    return __buzzer_beep(AW_FALSE);
}

/******************************************************************************/
aw_err_t aw_buzzer_beep (unsigned int beep_time_ms)
{
    aw_err_t err = aw_buzzer_on();

    /* \TODO: use message queue to beep */
    if (err != AW_OK) {
        return err;
    }

    aw_mdelay(beep_time_ms);
    return aw_buzzer_off();
}

/******************************************************************************/
aw_err_t aw_buzzer_loud_set (unsigned int beep_level)
{
    return __buzzer_loud_set(beep_level);
}

/******************************************************************************/

/** \brief register a buzzer device */
void awbl_buzzer_register_device (struct awbl_buzzer_dev *dev)
{
    aw_assert(NULL != dev->beep);

    __gp_buzzer_dev = dev;
}

/** @} */

/* end of file */
