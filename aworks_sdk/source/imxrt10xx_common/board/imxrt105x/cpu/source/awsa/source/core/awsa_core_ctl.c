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

#include "core/awsa_core_internal.h"

/**
 * \brief find the control instance with the given number-id
 */
struct awsa_core_ctl_elem *__core_ctl_numid_find (struct awsa_core_ctl *p_ctl,
                                                  uint32_t              num_id)
{
    struct awsa_core_ctl_elem *p_tmp;

    if (p_ctl == NULL || num_id == 0) {
        return NULL;
    }

    aw_list_for_each_entry(p_tmp,
                           &p_ctl->list,
                           struct awsa_core_ctl_elem,
                           list) {
        if (p_tmp->num_id == num_id) {
            return p_tmp;
        }
    }

    return NULL;
}

/******************************************************************************/

/**
 * \brief control device create
 */
aw_err_t awsa_core_ctl_init (struct awsa_core_ctl  *p_ctl,
                             struct awsa_core_card *p_card)
{
    const static struct awsa_core_dev_servopts ctl_opts = {
        NULL,
        NULL,
        NULL,
    };

    awsa_core_assert(p_card != NULL);
    AW_INIT_LIST_HEAD(&p_ctl->list);
    return awsa_core_dev_init(&p_ctl->dev,
                              AWSA_CORE_DEV_TYPE_CTL,
                              p_ctl,
                              &ctl_opts,
                              p_card);
}

/**
 * \brief find ctl device
 */
struct awsa_core_ctl *awsa_core_ctl_get (struct awsa_core_card *p_card)
{
    struct awsa_core_dev  *p_dev = NULL;

    aw_list_for_each_entry(p_dev,
                           &p_card->devices,
                           struct awsa_core_dev,
                           list) {
        if (p_dev->type == AWSA_CORE_DEV_TYPE_CTL) {
            break;
        }
    }

    return (struct awsa_core_ctl *)p_dev;
}

/**
 * \brief add the control instance to the card
 */
aw_err_t awsa_core_ctl_elem_add (
    struct awsa_core_ctl_elem              *p_elem,
    const struct awsa_core_ctl_elem_driver *p_driver,
    void                                   *p_private_data,
    struct awsa_core_ctl                   *p_ctl)
{
    if (p_elem == NULL || p_ctl == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_ctl->mutex, AW_SEM_WAIT_FOREVER);
    if (awsa_core_ctl_elem_find(p_ctl,
                                p_driver->p_name,
                                p_driver->iface,
                                0) != NULL) {
        AWSA_CORE_DEBUG_ERROR("element is already present");
        AW_MUTEX_UNLOCK(p_ctl->mutex);
        return -AW_EPERM;
    }

    p_elem->p_driver        = p_driver;
    aw_list_add_tail(&p_elem->list, &p_ctl->list);
    p_ctl->elem_count      += 1;
    p_elem->num_id          = p_ctl->last_num_id + 1;
    p_ctl->last_num_id     += 1;
    p_elem->p_private_data  = p_private_data;
    AW_MUTEX_UNLOCK(p_ctl->mutex);
    return AW_OK;
}

/**
 * \brief find the control instance with the given id
 */
struct awsa_core_ctl_elem *awsa_core_ctl_elem_find (
    struct awsa_core_ctl    *p_ctl,
    const char              *p_name,
    awsa_core_ctl_elem_if_t  iface,
    uint32_t                 num_id)
{
    struct awsa_core_ctl_elem *p_tmp;

    if (p_ctl == NULL) {
        return NULL;
    }

    if (num_id != 0) {
        return __core_ctl_numid_find(p_ctl, num_id);
    }

    aw_list_for_each_entry(p_tmp,
                           &p_ctl->list,
                           struct awsa_core_ctl_elem,
                           list) {
        if (p_tmp->p_driver->iface != iface) {
            continue;
        }

        if (strcmp(p_tmp->p_driver->p_name, p_name)) {
            continue;
        }

        return p_tmp;
    }

    return NULL;
}

/**
 * \brief read control device value
 */
aw_err_t awsa_core_ctl_elem_value_read (
    struct awsa_core_ctl_elem       *p_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    aw_err_t ret;

    if (p_elem == NULL || p_value == NULL) {
        return -AW_EINVAL;
    }

    if (p_elem->p_driver->pfn_value_get != NULL) {
        ret = p_elem->p_driver->pfn_value_get(p_elem, p_value);
    } else {
        ret = -AW_EPERM;
    }

    return ret;
}

/**
 * \brief write control device value
 *
 * \param[in] p_card    : the sound card pointer
 * \param[in] p_ctl : the control instance
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_core_ctl_elem_value_write (
    struct awsa_core_ctl_elem       *p_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    aw_err_t ret;

    if (p_elem == NULL || p_value == NULL) {
        return -AW_EINVAL;
    }

    if (p_elem->p_driver->pfn_value_set != NULL) {
        ret = p_elem->p_driver->pfn_value_set(p_elem, p_value);
    } else {
        ret = -AW_EPERM;
    }

    return ret;
}

/**
 * \brief get ctl device information
 */
aw_err_t awsa_core_ctl_elem_info_get (struct awsa_core_ctl_elem      *p_elem,
                                      struct awsa_core_ctl_elem_info *p_info)
{
    aw_err_t ret;

    if (p_elem == NULL || p_info == NULL) {
        return -AW_EINVAL;
    }

    if (p_elem->p_driver->pfn_info_get != NULL) {
        ret = p_elem->p_driver->pfn_info_get(p_elem, p_info);
    } else {
        ret = -AW_EPERM;
    }

    return ret;
}

/* end of file */
