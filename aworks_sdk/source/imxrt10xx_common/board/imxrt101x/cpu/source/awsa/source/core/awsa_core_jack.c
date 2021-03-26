/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-22  win, first implementation
 * \endinternal
 */

#include "core/awsa_core_internal.h"

/**
 * \brief jack switch types
 */
static int __g_core_jack_switch[] = {
    AWSA_CORE_JACK_HEADPHONE_INSERT,
    AWSA_CORE_JACK_MICROPHONE_INSERT,
    AWSA_CORE_JACK_LINEOUT_INSERT,
    AWSA_CORE_JACK_MECHANICAL_INSERT,
    AWSA_CORE_JACK_VIDEOOUT_INSERT,
};

static aw_err_t __core_jack_dev_unregister (struct awsa_core_dev *p_dev)
{
    struct awsa_core_jack *p_jack = p_dev->p_dev_data;

    p_jack->registered = AW_FALSE;
    return AW_OK;
}

static aw_err_t __core_jack_dev_register (struct awsa_core_dev *p_dev)
{
    struct awsa_core_jack *p_jack = p_dev->p_dev_data;
    int                    i, bit;

    /* Add capabilities for any keys that are enabled */
    for (i = 0; i < AWSA_CORE_ARRAY_SIZE(p_jack->key); i++) {
        bit = AWSA_CORE_JACK_EVT_BTN_0 >> i;

        if (!(p_jack->evt_mask & bit)) {
            continue;
        }

        if (p_jack->key[i] == 0) {
            p_jack->key[i] = BTN_0 + i;
        }
    }

    p_jack->registered = AW_TRUE;
    return AW_OK;
}

/**
 * \brief Create a new jack
 */
aw_err_t awsa_core_jack_init (struct awsa_core_jack  *p_jack,
                              const char             *p_name,
                              int                     evt_mask,
                              void                   *p_private_data,
                              struct awsa_core_card  *p_card)
{
    static struct awsa_core_dev_servopts opts = {
        __core_jack_dev_register,
        __core_jack_dev_unregister,
        NULL,
    };

    if (p_jack == NULL || p_name == NULL || p_card == NULL) {
        return -AW_EINVAL;
    }

    p_jack->p_name         = p_name;
    p_jack->evt_mask       = evt_mask;
    p_jack->p_private_data = p_private_data;

    awsa_core_dev_init(&p_jack->dev,
                       AWSA_CORE_DEV_TYPE_JACK,
                       p_jack,
                       &opts,
                       p_card);

    return awsa_core_dev_register(&p_jack->dev, p_card);
}

/**
 * \brief Set a key code mapping on a jack
 */
aw_err_t awsa_core_jack_key_set (struct awsa_core_jack    *p_jack,
                                 enum awsa_core_jack_evt   evt,
                                 int                       code)
{
    int i = awsa_core_fls(AWSA_CORE_JACK_EVT_BTN_0) - awsa_core_fls(evt);

    if (!p_jack->registered) {
        return -AW_EPERM;
    }

    if (i >= AWSA_CORE_ARRAY_SIZE(p_jack->key)) {
        return -AW_EINVAL;
    }

    p_jack->evt_mask |= evt;
    p_jack->key[i]    = code;
    return AW_OK;
}

/**
 * \brief Report the current status of a jack
 */
void awsa_core_jack_report (struct awsa_core_jack *p_jack, int status)
{
    int i, bit;

    if (p_jack == NULL) {
        return;
    }

    for (i = 0; i < AWSA_CORE_ARRAY_SIZE(p_jack->key); i++) {
        bit = AWSA_CORE_JACK_EVT_BTN_0 >> i;

        if (p_jack->evt_mask & bit) {
            aw_input_report_key(AW_INPUT_EV_KEY,
                                p_jack->key[i],
                                status & bit);
        }
    }

    for (i = 0; i < AWSA_CORE_ARRAY_SIZE(__g_core_jack_switch); i++) {
        bit = 1 << i;

        if (p_jack->evt_mask & bit) {
            aw_input_report_key(AW_INPUT_EV_KEY,
                                __g_core_jack_switch[i],
                                status & bit);
        }
    }
}

/* end of file */
