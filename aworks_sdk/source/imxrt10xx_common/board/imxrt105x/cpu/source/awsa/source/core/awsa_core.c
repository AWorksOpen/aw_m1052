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
 * - 1.00 16-11-10  win, first implementation
 * \endinternal
 */

#include "core/awsa_core_internal.h"

/** \brief card list */
struct aw_list_head __g_core_cards_list;

/* pcm device lsit */
extern
struct aw_list_head __g_core_pcm_devices;

/** \brief core cards list lock */
AW_MUTEX_DECL(__g_core_cards_lock);

/** \brief pcm devices */
extern struct aw_list_head __g_core_pcm_devices;

/**
 * \brief initialize a soundcard object
 */
aw_err_t awsa_core_card_init (struct awsa_core_card *p_card,
                              int                    idx,
                              void                  *p_private_data)
{
    awsa_core_assert(p_card != NULL);

    AW_MUTEX_LOCK(__g_core_cards_lock, AW_SEM_WAIT_FOREVER);
    p_card->id = idx;
    aw_list_add(&p_card->list, &__g_core_cards_list);
    AW_MUTEX_UNLOCK(__g_core_cards_lock);

    AW_INIT_LIST_HEAD(&p_card->devices);

#if 0
    ret = awsa_core_ctl_init(&p_card->ctl, p_card);
    if (ret != AW_OK) {
        AWSA_CORE_DEBUG_ERROR("unable to register ctl device");
        goto __error;
    }
#endif

    p_card->p_private_data = p_private_data;
    aw_spinlock_isr_init(&p_card->spinlock, 0);
    return AW_OK;
}

/**
 * \brief unregister a sound card structure
 */
aw_err_t awsa_core_card_unregister (struct awsa_core_card *p_card)
{
    aw_err_t ret;

    awsa_core_assert(p_card != NULL);

#if 0
    aw_spinlock_isr_take(&p_card->spinlock);
    if (p_card->shutdown) {
        aw_spinlock_isr_give(&p_card->spinlock);
        return AW_OK;
    }
    p_card->shutdown = TRUE;
    aw_spinlock_isr_give(&p_card->spinlock);
#endif

    /* notify all devices that we are disconnected */
    ret = awsa_core_dev_unregister_all(p_card);
    if (ret != AW_OK) {
        AWSA_CORE_DEBUG_WARN(
            "not all devices for card %i can be unregister",
             p_card->id);
    }

    return ret;
}

/**
 * \brief register the soundcard
 */
aw_err_t awsa_core_card_register (struct awsa_core_card *p_card)
{
    aw_err_t ret;

    awsa_core_assert(p_card != NULL);

#if 0
    aw_spinlock_isr_take(&p_card->spinlock);


    aw_spinlock_isr_give(&p_card->spinlock);
#endif

    ret = awsa_core_dev_register_all(p_card);
    if (ret != AW_OK) {
        return ret;
    }

    AWSA_CORE_DEBUG_INFO("soundcard %d register successfully!", p_card->id);
    return AW_OK;
}

/**
 * \brief create an device component
 * \note initialize device and add device to soundcard device list
 */
aw_err_t awsa_core_dev_init (struct awsa_core_dev                *p_dev,
                             awsa_core_dev_type_t                 type,
                             void                                *p_dev_data,
                             const struct awsa_core_dev_servopts *p_opts,
                             struct awsa_core_card               *p_card)
{
    awsa_core_assert(p_dev      != NULL);
    awsa_core_assert(p_card     != NULL);
    awsa_core_assert(p_dev_data != NULL);
    awsa_core_assert(p_opts     != NULL);

    p_dev->p_card     = p_card;
    p_dev->type       = type;
    p_dev->state      = AWSA_CORE_DEV_STATE_BUILD;
    p_dev->p_dev_data = p_dev_data;
    p_dev->p_opts     = p_opts;

    /* add to the head of list */
    aw_list_add(&p_dev->list, &p_card->devices);
    return AW_OK;
}

/**
 * \brief register the device
 */
aw_err_t awsa_core_dev_register (struct awsa_core_dev  *p_dev,
                                 struct awsa_core_card *p_card)
{
    struct awsa_core_dev *p_tmp = NULL;
    aw_err_t              ret;

    awsa_core_assert(p_card  != NULL);
    awsa_core_assert(p_dev   != NULL);

    aw_list_for_each_entry(p_tmp,
                           &p_card->devices,
                           struct awsa_core_dev,
                           list) {
        if (p_tmp != p_dev) {
            continue;
        }

        if (p_tmp->state == AWSA_CORE_DEV_STATE_BUILD &&
            p_tmp->p_opts->pfn_dev_register != NULL) {
            ret = p_tmp->p_opts->pfn_dev_register(p_dev);
            if (ret != AW_OK) {
                return ret;
            }
            p_dev->state = AWSA_CORE_DEV_STATE_REGISTERED;
            return AW_OK;
        }

        AWSA_CORE_DEBUG_INFO("awsa core dev register busy");
        return -AW_EBUSY;
    }

    return -AW_ENXIO;
}

/**
 * \brief register all the devices on the card
 */
aw_err_t awsa_core_dev_register_all (struct awsa_core_card *p_card)
{
    struct awsa_core_dev *p_dev;
    aw_err_t              ret;

    awsa_core_assert(p_card != NULL);

    aw_list_for_each_entry(p_dev,
                           &p_card->devices,
                           struct awsa_core_dev,
                           list) {
        if (p_dev->state == AWSA_CORE_DEV_STATE_BUILD &&
            p_dev->p_opts->pfn_dev_register != NULL) {
            ret = p_dev->p_opts->pfn_dev_register(p_dev);
            if (ret != AW_OK) {
                return ret;
            }
            p_dev->state = AWSA_CORE_DEV_STATE_REGISTERED;
        }
    }

    return AW_OK;
}

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
                                   struct awsa_core_card *p_card)
{
    struct awsa_core_dev *p_tmp = NULL;

    awsa_core_assert(p_card  != NULL);
    awsa_core_assert(p_dev   != NULL);

    aw_list_for_each_entry(p_tmp,
                           &p_card->devices,
                           struct awsa_core_dev,
                           list) {
        if (p_tmp != p_dev) {
            continue;
        }

        if (p_dev->state == AWSA_CORE_DEV_STATE_REGISTERED &&
            p_dev->p_opts->pfn_dev_disconnect != NULL) {
            if (p_dev->p_opts->pfn_dev_disconnect(p_dev)) {
                AWSA_CORE_DEBUG_ERROR("device disconnect failure\n");
            }
            p_dev->state =  AWSA_CORE_DEV_STATE_DISCONNECTED;
        }
        return AW_OK;
    }

    return -AW_ENXIO;
}

/**
 * \brief unregister all the devices on the card
 * \param[in] p_card : the card instance
 *
 * \retval AW_OK     : success
 * \retval -AW_ENXIO : device not exist
 */
aw_err_t awsa_core_dev_disconnect_all (struct awsa_core_card *p_card)
{
    struct awsa_core_dev *p_dev = NULL;
    aw_err_t              ret   = AW_OK;

    awsa_core_assert(p_card != NULL);

    aw_list_for_each_entry(p_dev,
                           &p_card->devices,
                           struct awsa_core_dev,
                           list) {
        if (awsa_core_dev_disconnect(p_dev, p_card) != AW_OK) {
            ret = -AW_ENXIO;
        }
    }

    return ret;
}

/**
 * \brief unregister the device
 */
aw_err_t awsa_core_dev_unregister (struct awsa_core_dev  *p_dev,
                                   struct awsa_core_card *p_card)
{
    struct awsa_core_dev *p_tmp = NULL;
    aw_err_t              ret   = -AW_ENXIO;

    awsa_core_assert(p_card != NULL);
    awsa_core_assert(p_dev  != NULL);

    aw_list_for_each_entry(p_tmp,
                           &p_card->devices,
                           struct awsa_core_dev,
                           list) {
        if (p_tmp != p_dev) {
            continue;
        }

        /* unlink */
        aw_list_del(&p_dev->list);

        if (p_dev->state == AWSA_CORE_DEV_STATE_REGISTERED &&
            p_dev->p_opts->pfn_dev_disconnect != NULL) {
            if (p_dev->p_opts->pfn_dev_disconnect(p_dev)) {
                AWSA_CORE_DEBUG_ERROR("device disconnect failure\n");
            }
        }

        if (p_dev->p_opts->pfn_dev_unregister != NULL) {
            ret = p_dev->p_opts->pfn_dev_unregister(p_dev);
            if (ret != AW_OK) {
                AWSA_CORE_DEBUG_WARN("device unregister failure");
            }
        }

        p_dev->state = AWSA_CORE_DEV_STATE_UNREGISTERED;
        return AW_OK;
    }

    AWSA_CORE_DEBUG_INFO("device disconnect %p, not found", p_dev);
    return ret;
}

/**
 * \brief unregister all the devices on the card
 */
aw_err_t awsa_core_dev_unregister_all (struct awsa_core_card *p_card)
{
    struct awsa_core_dev *p_dev;
    aw_err_t              ret = 0;

    awsa_core_assert(p_card != NULL);

    aw_list_for_each_entry(p_dev,
                           &p_card->devices,
                           struct awsa_core_dev,
                           list) {
        if (awsa_core_dev_unregister(p_dev, p_card) != AW_OK) {
            ret = -AW_ENXIO;
        }
    }

    return ret;
}

/**
 * \brief sound core initialization
 */
void awsa_core_init (void)
{
    AW_INIT_LIST_HEAD(&__g_core_cards_list);
    AW_INIT_LIST_HEAD(&__g_core_pcm_devices);

    AW_MUTEX_INIT(__g_core_cards_lock, AW_SEM_Q_PRIORITY | AW_SEM_DELETE_SAFE);
}

/* end of file */
