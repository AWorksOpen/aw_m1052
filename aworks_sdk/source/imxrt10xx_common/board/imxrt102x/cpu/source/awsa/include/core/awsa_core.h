/*******************************************************************************
*                                 AWorks
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
 * - 1.00 16-11-10  win, first implementation
 * \endinternal
 */

#ifndef __AWSA_CORE_H
#define __AWSA_CORE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "core/awsa_core_internal.h"

struct awsa_core_ctl;

/**
 * \brief device type
 */
typedef enum {
    AWSA_CORE_DEV_TYPE_TOPLEVEL        = 0,
    AWSA_CORE_DEV_TYPE_CTL,
    AWSA_CORE_DEV_TYPE_LOWLEVEL_PRE,
    AWSA_CORE_DEV_TYPE_LOWLEVEL_NORMAL = 0x1000,
    AWSA_CORE_DEV_TYPE_PCM,
    AWSA_CORE_DEV_TYPE_RAWMIDI,
    AWSA_CORE_DEV_TYPE_TIMER,
    AWSA_CORE_DEV_TYPE_SEQUENCER,
    AWSA_CORE_DEV_TYPE_HWDEP,
    AWSA_CORE_DEV_TYPE_CODEC,
    AWSA_CORE_DEV_TYPE_JACK,
    AWSA_CORE_DEV_TYPE_LOWLEVEL        = 0x2000,
} awsa_core_dev_type_t;

/** \brief only 0x1000 */
#define AWSA_CORE_DEV_TYPE_RANGE_SIZE    0x1000

/**
 * \brief device state
 */
typedef enum {
    AWSA_CORE_DEV_STATE_BUILD        = 0,
    AWSA_CORE_DEV_STATE_REGISTERED,
    AWSA_CORE_DEV_STATE_DISCONNECTED,
    AWSA_CORE_DEV_STATE_UNREGISTERED,
} awsa_core_dev_state_t;

/** \brief declare before define */
struct awsa_core_dev;

/**
 * \brief device service operate interface
 */
struct awsa_core_dev_servopts {

    /**
     * \brief register device
     * \param[in] p_dev : device pointer
     * \retval AW_OK : succcess
     */
    aw_err_t (*pfn_dev_register  ) (struct awsa_core_dev *p_dev);

    /**
     * \brief unregister device
     * \param[in] p_dev : device pointer
     * \retval AW_OK : succcess
     */
    aw_err_t (*pfn_dev_unregister) (struct awsa_core_dev *p_dev);

    /**
     * \brief device disconnected
     * \param[in] p_dev : device pointer
     * \retval AW_OK : succcess
     */
    aw_err_t (*pfn_dev_disconnect) (struct awsa_core_dev *p_dev);

};

/**
 * \brief sound device, as PCM, Control, Jack etc.
 */
struct awsa_core_dev {

    /** \brief list of registered devices */
    struct aw_list_head            list;

    /** \brief card which holds this device */
    struct awsa_core_card         *p_card;

    int                            id;    /**< \brief device id number */
    awsa_core_dev_state_t          state; /**< \brief state of the device */
    awsa_core_dev_type_t           type;  /**< \brief device type */

    /** \brief  device structure */
    void                          *p_dev_data;

    /** \brief operations */
    const struct awsa_core_dev_servopts *p_opts;
};

/**
 * \brief sound card, include multile sound device
 */
struct awsa_core_card {

    /** \brief index of soundcard */
    int                   id;

#if 0
    /** \brief this card is going down */
    bool_t                shutdown;
#endif

    /** \brief device lock */
    aw_spinlock_isr_t     spinlock;

    /** \brief devices list */
    struct aw_list_head   devices;
//
//    /** \brief ctl device */
//    struct awsa_core_ctl  ctl;

    /** \brief power state */
    uint32_t              power_state;

    /** \brief power lock */
    AW_MUTEX_DECL(        power_lock);

    /** \brief private data for soundcard */
    void                 *p_private_data;

    /** \brief soundcard list */
    struct aw_list_head   list;
};

/**
 * \brief initialize a soundcard object
 *
 * \param[in] p_card  : soundcard
 * \param[in] idx     : soundcard index, range from 0 to cards_max
 * \param[in] p_name  : card identification (ASCII string)
 *
 * \retval AW_OK      : success
 * \retval -AW_ENODEV : sound card index idx not exist
 */
aw_err_t awsa_core_card_init (struct awsa_core_card *p_card,
                              int                    idx,
                              void                  *p_private_data);

/**
 * \brief register the soundcard
 * \param[in] p_card : sound card structure pointer
 *
 * \retval AW_OK : success
 * \note  This function registers all the devices assigned to the soundcard.
 *        Until calling this, the AWSA control interface is blocked from the
 *        external accesses.  Thus, you should call this function at the end
 *        of the initialization of the card.
 */
aw_err_t awsa_core_card_register (struct awsa_core_card *p_card);

/**
 * \brief unregister a sound card structure
 * \param[in] p_card : sound card structure pointer
 * \retval AW_OK : success
 */
aw_err_t awsa_core_card_unregister (struct awsa_core_card *p_card);

/**
 * \brief create an device component
 * \note initialize device and add device to soundcard device list
 *
 * \param[in] p_dev         : device
 * \param[in] type          : device type, #AWSA_CORE_DEV_*
 * \param[in] p_device_data : device private data
 * \param[in] p_opts        : service functions
 * \param[in] p_card        : soundcard
 *
 * \retval AW_OK      : success
 * \retval -AM_EINVAL : parameter invalid
 */
aw_err_t awsa_core_dev_init (struct awsa_core_dev                *p_dev,
                             awsa_core_dev_type_t                 type,
                             void                                *p_dev_data,
                             const struct awsa_core_dev_servopts *p_opts,
                             struct awsa_core_card               *p_card);

/**
 * \brief register the device
 *
 * \param[in] p_card        : the card instance
 * \param[in] p_device_data : the data pointer to register
 *
 * \retval AW_OK     : success
 * \retval -AW_EBUSY : device busy, need try again
 * \retval -AW_ENXIO : device not exist
 */
aw_err_t awsa_core_dev_register (struct awsa_core_dev  *p_dev,
                                 struct awsa_core_card *p_card);

/**
 * \brief register all the devices on the card
 * \param[in] p_card : the card instance
 *
 * \retval AW_OK     : success
 * \retval -AW_EBUSY : device busy, need try again
 * \retval -AW_ENXIO : device not exist
 */
aw_err_t awsa_core_dev_register_all (struct awsa_core_card *p_card);

/**
 * \brief disconnect the device
 *
 * \param[in] p_card        : the card instance
 * \param[in] p_device_data : the data pointer to register
 *
 * \retval AW_OK     : success
 * \retval -AW_ENXIO : device not exist
 */
aw_err_t awsa_core_dev_disconnect (struct awsa_core_dev  *p_dev,
                                   struct awsa_core_card *p_card);

/**
 * \brief unregister all the devices on the card
 * \param[in] p_card : the card instance
 *
 * \retval AW_OK     : success
 * \retval -AW_ENXIO : device not exist
 */
aw_err_t awsa_core_dev_disconnect_all (struct awsa_core_card *p_card);

/**
 * \brief unregister the device
 *
 * \param[in] p_card        : the card instance
 * \param[in] p_device_data : the data pointer to register
 *
 * \retval AW_OK     : success
 * \retval -AW_ENXIO : device not exist
 */
aw_err_t awsa_core_dev_unregister (struct awsa_core_dev  *p_dev,
                                   struct awsa_core_card *p_card);

/**
 * \brief unregister all the devices on the card
 * \param[in] p_card : the card instance
 *
 * \retval AW_OK     : success
 * \retval -AW_ENXIO : device not exist
 */
aw_err_t awsa_core_dev_unregister_all (struct awsa_core_card *p_card);

/**
 * \brief sound core initialization
 * \param[in] cards_max : maximum sound card number
 */
void awsa_core_init (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_CORE_H */

/* end of file */
