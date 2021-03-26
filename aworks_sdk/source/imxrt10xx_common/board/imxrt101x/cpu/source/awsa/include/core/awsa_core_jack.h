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
 * - 1.00 16-11-11  win, first implementation
 * \endinternal
 */

#ifndef __AWSA_CORE_JACK_H
#define __AWSA_CORE_JACK_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "core/awsa_core_internal.h"

/**
 * \name jack evt code
 * @{
 */

#define AWSA_CORE_JACK_HEADPHONE_INSERT        (KEY_MAX + 0x02)
#define AWSA_CORE_JACK_MICROPHONE_INSERT       (KEY_MAX + 0x04)
#define AWSA_CORE_JACK_LINEOUT_INSERT          (KEY_MAX + 0x06)
#define AWSA_CORE_JACK_MECHANICAL_INSERT       (KEY_MAX + 0x07)
#define AWSA_CORE_JACK_VIDEOOUT_INSERT         (KEY_MAX + 0x08)

/** @} */

/**
 * \brief Jack types which can be reported.  These values are used as a
 *        bitmask.
 *        Note that this must be kept in sync with the lookup table in
 *        source/core/awsa_core_jack.c.
 */
enum awsa_core_jack_evt {

    /** \brief headphone */
    AWSA_CORE_JACK_EVT_HEADPHONE  = 0x0001,

    /** \brief micophone */
    AWSA_CORE_JACK_EVT_MICROPHONE = 0x0002,

    /** \brief headhone and micophone */
    AWSA_CORE_JACK_EVT_HEADSET    = AWSA_CORE_JACK_EVT_HEADPHONE |
                                    AWSA_CORE_JACK_EVT_MICROPHONE,

    /** \brief lineout */
    AWSA_CORE_JACK_EVT_LINEOUT    = 0x0004,

    /** \brief mechanical */
    AWSA_CORE_JACK_EVT_MECHANICAL = 0x0008,

    /** \brief videoout */
    AWSA_CORE_JACK_EVT_VIDEOOUT   = 0x0010,

    /** \brief lineout and videoout */
    AWSA_CORE_JACK_EVT_AVOUT      = AWSA_CORE_JACK_EVT_LINEOUT |
                                    AWSA_CORE_JACK_EVT_VIDEOOUT,

    /** \brief key0 */
    AWSA_CORE_JACK_EVT_BTN_0      = 0x4000,

    /** \brief key1 */
    AWSA_CORE_JACK_EVT_BTN_1      = 0x2000,

    /** \brief key2 */
    AWSA_CORE_JACK_EVT_BTN_2      = 0x1000,

    /** \brief key3 */
    AWSA_CORE_JACK_EVT_BTN_3      = 0x0800,

    /** \brief key4 */
    AWSA_CORE_JACK_EVT_BTN_4      = 0x0400,

    /** \brief key5 */
    AWSA_CORE_JACK_EVT_BTN_5      = 0x0200,
};

/**
 * \brief JACK device structure
 */
struct awsa_core_jack {

    struct awsa_core_dev  dev;

    /** \brief register success */
    aw_bool_t             registered;

    /** \brief AWSA_CORE_JACK_EVT_* */
    int                   evt_mask;

    /** \brief jack identifiers */
    const char           *p_name;

    /**
     * \brief Keep in sync with definitions above,
     *        AWSA_CORE_JACK_TYPE_BTN_0 ~ 5
     */
    uint32_t              key[6];

    /** \brief private for jack device */
    void                 *p_private_data;
};

/**
 * \brief Create a new jack
 *
 * \param[in] p_card  : the card instance
 * \param[in] id      : an identifying string for this jack
 * \param[in] evt     : a bitmask of enum awsa_core_jack_evt values that can
 *                      be detected by this jack
 * \param[in] pp_jack : Used to provide the allocated jack object to the
 *                      caller.
 *
 * \retval AW_OK      : success
 * \retval -AW_ENOMEM : allocate fail
 */
aw_err_t awsa_core_jack_init (struct awsa_core_jack  *p_jack,
                              const char             *p_name,
                              int                     evt_mask,
                              void                   *p_private_data,
                              struct awsa_core_card  *p_card);

/**
 * \brief Set a key code mapping on a jack
 *
 * \param[in] jack    : The jack to configure
 * \param[in] type    : Jack report type for this key
 * \param[in] key     : Input layer key type to be reported
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : invalid parameter
 *
 * \note Map a AWSA_CORE_JACK_BTN_ button type to an input layer key, allowing
 *       reporting of keys on accessories via the jack abstraction.  If no
 *       mapping is provided but keys are enabled in the jack type then
 *       BTN_n numeric buttons will be reported.
 *
 *       Note that this is intended to be use by simple devices with small
 *       numbers of keys that can be reported.  It is also possible to
 *       access the input device directly - devices with complex input
 *       capabilities on accessories should consider doing this rather than
 *       using this abstraction.
 *
 *       This function may only be called prior to registration of the jack.
 */
aw_err_t awsa_core_jack_key_set (struct awsa_core_jack     *p_jack,
                                 enum awsa_core_jack_evt    evt,
                                 int                        code);

/**
 * \brief Report the current status of a jack
 *
 * \param[in] p_jack : The jack to report status for
 * \param[in] status : The current status of the jack
 *
 * \return no
 */
void awsa_core_jack_report (struct awsa_core_jack *p_jack, int status);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_CORE_JACK_H */

/* end of file */
