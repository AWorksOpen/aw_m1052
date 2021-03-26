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

/** \brief card list */
extern struct aw_list_head __g_core_cards_list;

/* cards lock */
AW_MUTEX_IMPORT(__g_core_cards_lock);

/******************************************************************************/

/**
 * \brief whether mixer active
 */
static inline
aw_bool_t __mixer_active (struct awsa_mixer *p_mixer)
{
    return p_mixer->active;
}

/**
 * \brief open mixer device
 */
aw_err_t awsa_mixer_open (struct awsa_mixer *p_mixer, int card)
{
    struct awsa_core_card *p_card;
    struct awsa_core_dev  *p_dev;

    if (p_mixer == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(__g_core_cards_lock, AW_SEM_WAIT_FOREVER);
    aw_list_for_each_entry(p_card,
                           &__g_core_cards_list,
                           struct awsa_core_card,
                           list) {

        if (p_card->id == card) {
            p_mixer->card = card;
            AW_MUTEX_UNLOCK(__g_core_cards_lock);
            goto __card_find;
        }

    }

    AW_MUTEX_UNLOCK(__g_core_cards_lock);
    return -AW_ENODEV;

__card_find:

    aw_list_for_each_entry(p_dev,
                           &p_card->devices,
                           struct awsa_core_dev,
                           list) {
        if (p_dev->type == AWSA_CORE_DEV_TYPE_CTL) {
            p_mixer->p_cookie = p_dev;
            p_mixer->active   = AW_TRUE;
            return AW_OK;
        }
    }

    return -AW_ENODEV;
}

/**
 * \brief close mixer device
 */
aw_err_t awsa_mixer_close (struct awsa_mixer *p_mixer)
{
    if (p_mixer == NULL) {
        return -AW_EINVAL;
    }

    p_mixer->p_cookie = NULL;
    p_mixer->active   = AW_FALSE;
    return AW_OK;
}

/**
 * \brief get elements count
 */
uint32_t awsa_mixer_elem_count_get (struct awsa_mixer *p_mixer)
{
    struct awsa_core_ctl *p_ctl = (struct awsa_core_ctl *)p_mixer->p_cookie;

    if (p_mixer == NULL || p_mixer->p_cookie == NULL) {
        return 0;
    }

    if (!__mixer_active(p_mixer)) {
        return 0;
    }

    return p_ctl->elem_count;
}

/**
 * \brief find mixer element by element id
 */
aw_err_t awsa_mixer_elem_find (struct awsa_mixer_elem *p_elem,
                               struct awsa_mixer      *p_mixer,
                               const char             *p_name)
{
    struct awsa_core_ctl           *p_ctl;
    struct awsa_core_ctl_elem      *p_ctl_elem;
    struct awsa_core_ctl_elem_info  info;

    if (p_elem            == NULL ||
        p_mixer           == NULL ||
        p_mixer->p_cookie == NULL ||
        p_name            == NULL) {
        return -AW_EINVAL;
    }

    if (!__mixer_active(p_mixer)) {
        return -AW_ENODEV;
    }

    p_ctl      = (struct awsa_core_ctl *)p_mixer->p_cookie;
    p_ctl_elem = awsa_core_ctl_elem_find(p_ctl,
                                         p_name,
                                         AWSA_CORE_CTL_ELEM_IF_MIXER,
                                         0);
    if (p_ctl_elem == NULL) {
        return -AW_ENODEV;
    }

    awsa_core_ctl_elem_info_get(p_ctl_elem, &info);

    p_ctl_elem->type = info.type;

    p_elem->channels = info.count;
    p_elem->p_cookie = p_ctl_elem;
    p_elem->p_name   = p_name;
    return AW_OK;
}

/**
 * \brief Get element channel(values) count
 */
uint32_t awsa_mixer_elem_channel_count_get (struct awsa_mixer_elem *p_elem)
{
    if (p_elem == NULL) {
        return 0;
    }
    return p_elem->channels;
}

aw_err_t awsa_mixer_elem_volume_get (struct awsa_mixer_elem *p_elem,
                                     awsa_mixer_elem_chan_t  channel,
                                     int32_t                *p_volume)
{
    struct awsa_core_ctl_elem       *p_ctl_elem;
    struct awsa_core_ctl_elem_value  ctl_value;
    int32_t                          vol[AWSA_MIXER_CHAN_LAST + 1];
    aw_err_t                         ret;

    if (p_elem == NULL || p_volume == NULL || p_elem->p_cookie == NULL) {
        return -AW_EINVAL;
    }

    p_ctl_elem = (struct awsa_core_ctl_elem *)p_elem->p_cookie;

    if (p_ctl_elem->p_driver->iface != AWSA_CORE_CTL_ELEM_IF_MIXER      ||
        (channel + 1)                > p_elem->channels                 ||
        p_ctl_elem->type            != AWSA_CORE_CTL_ELEM_TYPE_INTEGER) {
        return -AW_EPERM;
    }

    ctl_value.value.integer.p_value = vol;
    ret = awsa_core_ctl_elem_value_read(p_ctl_elem, &ctl_value);
    if (ret != AW_OK) {
        return ret;
    }

    *p_volume = ctl_value.value.integer.p_value[channel];
    return ret;
}

/**
 * \brief get playback dB
 */
aw_err_t awsa_mixer_elem_db_get (struct awsa_mixer_elem *p_elem,
                                 awsa_mixer_elem_chan_t  channel,
                                 int32_t                *p_db)
{
    int32_t                         volume;
    aw_err_t                        err;
    const uint32_t                 *p_tlv = NULL;
    const uint32_t                 *p_db_rec;
    struct awsa_core_ctl_elem_info  info;
    struct awsa_core_ctl_elem      *p_ctl_elem;

    if (p_elem           == NULL  ||
        p_elem->p_cookie == NULL  ||
        p_db             == NULL) {
        return -AW_EINVAL;
    }

    p_ctl_elem = (struct awsa_core_ctl_elem *)p_elem->p_cookie;

    if (p_ctl_elem->p_driver->iface != AWSA_CORE_CTL_ELEM_IF_MIXER      ||
        (channel + 1)                > p_elem->channels                 ||
        p_ctl_elem->type            != AWSA_CORE_CTL_ELEM_TYPE_INTEGER) {
        return -AW_EPERM;
    }

    err = awsa_core_ctl_elem_info_get(p_ctl_elem, &info);
    if (err != AW_OK) {
        return err;
    }

    err = awsa_mixer_elem_volume_get(p_elem, channel, &volume);
    if (err != AW_OK) {
        return err;
    }

    p_tlv = awsa_core_ctl_elem_tlv_get(p_ctl_elem);
    awsa_core_tlv_db_info_parse(p_tlv, &p_db_rec);
    return awsa_core_tlv_convert_to_db(p_db_rec,
                                       info.info.integer.min,
                                       info.info.integer.max,
                                       volume,
                                       p_db);
}

/**
 * \brief set playback volume
 */
aw_err_t awsa_mixer_elem_volume_set (struct awsa_mixer_elem *p_elem,
                                     awsa_mixer_elem_chan_t  channel,
                                     int32_t                 value)
{
    struct awsa_core_ctl_elem       *p_ctl_elem;
    struct awsa_core_ctl_elem_value  ctl_value;
    int32_t                          vol[AWSA_MIXER_CHAN_LAST + 1];
    aw_err_t                         ret;
    aw_bool_t                        changed = AW_TRUE;

    if (p_elem == NULL || p_elem->p_cookie == NULL) {
        return -AW_EINVAL;
    }

    p_ctl_elem = (struct awsa_core_ctl_elem *)p_elem->p_cookie;

    if (p_ctl_elem->p_driver->iface != AWSA_CORE_CTL_ELEM_IF_MIXER      ||
        (channel + 1)                > p_elem->channels                 ||
        p_ctl_elem->type            != AWSA_CORE_CTL_ELEM_TYPE_INTEGER) {
        return -AW_EPERM;
    }

    ctl_value.value.integer.p_value = vol;
    ret = awsa_core_ctl_elem_value_read(p_ctl_elem, &ctl_value);
    if (ret != AW_OK) {
        return ret;
    }

    changed = !!(ctl_value.value.integer.p_value[channel] != value);
    if (!changed) {
        return AW_OK;
    }

    ctl_value.value.integer.p_value[channel] = value;
    return awsa_core_ctl_elem_value_write(p_ctl_elem, &ctl_value);
}

/**
 * \brief set element dB
 */
aw_err_t awsa_mixer_elem_db_set (struct awsa_mixer_elem *p_elem,
                                 awsa_mixer_elem_chan_t  channel,
                                 int32_t                 db,
                                 int                     dir)
{
    struct awsa_core_ctl_elem      *p_ctl_elem;
    struct awsa_core_ctl_elem_info  info;
    aw_err_t                        err;
    int32_t                         volume;
    const uint32_t                 *p_tlv;
    const uint32_t                 *p_db_rec;

    if (p_elem == NULL || p_elem->p_cookie == NULL) {
        return -AW_EINVAL;
    }

    p_ctl_elem = (struct awsa_core_ctl_elem *)p_elem->p_cookie;

    if (p_ctl_elem->p_driver->iface != AWSA_CORE_CTL_ELEM_IF_MIXER      ||
        (channel + 1)                > p_elem->channels                 ||
        p_ctl_elem->type            != AWSA_CORE_CTL_ELEM_TYPE_INTEGER) {
        return -AW_EPERM;
    }

    err = awsa_core_ctl_elem_info_get(p_ctl_elem, &info);
    if (err != AW_OK) {
        return err;
    }

    p_tlv = awsa_core_ctl_elem_tlv_get(p_ctl_elem);
    awsa_core_tlv_db_info_parse(p_tlv, &p_db_rec);
    err = awsa_core_tlv_convert_from_db(p_tlv,
                                        info.info.integer.min,
                                        info.info.integer.max,
                                        db,
                                        &volume,
                                        dir);
    if (err != AW_OK) {
        return err;
    }

    return awsa_mixer_elem_volume_set(p_elem, channel, volume);
}

/**
 * \brief playback volume range get
 */
aw_err_t awsa_mixer_elem_volume_range_get (struct awsa_mixer_elem *p_elem,
                                           int32_t                *p_min,
                                           int32_t                *p_max)
{
    struct awsa_core_ctl_elem      *p_ctl_elem;
    struct awsa_core_ctl_elem_info  info;
    aw_err_t                        ret;

    if (p_elem == NULL || p_elem->p_cookie == NULL) {
        return -AW_EINVAL;
    }

    p_ctl_elem = (struct awsa_core_ctl_elem *)p_elem->p_cookie;

    if (p_ctl_elem->p_driver->iface != AWSA_CORE_CTL_ELEM_IF_MIXER      ||
        p_ctl_elem->type            != AWSA_CORE_CTL_ELEM_TYPE_INTEGER) {
        return -AW_EPERM;
    }

    ret = awsa_core_ctl_elem_info_get(p_ctl_elem, &info);
    if (ret != AW_OK) {
        return ret;
    }

    *p_min = info.info.integer.min;
    *p_max = info.info.integer.max;
    return AW_OK;
}

/**
 * \brief set switch value
 */
aw_err_t awsa_mixer_elem_switch_set (struct awsa_mixer_elem *p_elem,
                                     awsa_mixer_elem_chan_t  channel,
                                     int                     value)
{
    struct awsa_core_ctl_elem       *p_ctl_elem;
    struct awsa_core_ctl_elem_value  ctl_value;
    int32_t                          vol[AWSA_MIXER_CHAN_LAST + 1];
    aw_err_t                         ret;
    aw_bool_t                        changed = AW_TRUE;

    if (p_elem == NULL || p_elem->p_cookie == NULL) {
        return -AW_EINVAL;
    }

    p_ctl_elem = (struct awsa_core_ctl_elem *)p_elem->p_cookie;

    if (p_ctl_elem->p_driver->iface != AWSA_CORE_CTL_ELEM_IF_MIXER      ||
        (channel + 1)                > p_elem->channels                 ||
        p_ctl_elem->type            != AWSA_CORE_CTL_ELEM_TYPE_BOOLEAN) {
        return -AW_EPERM;
    }

    ctl_value.value.integer.p_value = vol;
    ret = awsa_core_ctl_elem_value_read(p_ctl_elem, &ctl_value);
    if (ret != AW_OK) {
        return ret;
    }

    changed = !!(ctl_value.value.integer.p_value[channel] != value);
    if (!changed) {
        return AW_OK;
    }

    ctl_value.value.integer.p_value[channel] = value;
    return awsa_core_ctl_elem_value_write(p_ctl_elem, &ctl_value);
}

/**
 * \brief get switch value
 */
aw_err_t awsa_mixer_elem_switch_get (struct awsa_mixer_elem *p_elem,
                                     awsa_mixer_elem_chan_t  channel,
                                     int                    *p_value)
{
    struct awsa_core_ctl_elem       *p_ctl_elem;
    struct awsa_core_ctl_elem_value  ctl_value;
    int32_t                          vol[AWSA_MIXER_CHAN_LAST + 1];
    aw_err_t                         ret;

    if (p_elem == NULL || p_elem->p_cookie == NULL) {
        return -AW_EINVAL;
    }

    p_ctl_elem = (struct awsa_core_ctl_elem *)p_elem->p_cookie;

    if (p_ctl_elem->p_driver->iface != AWSA_CORE_CTL_ELEM_IF_MIXER      ||
        (channel + 1)                > p_elem->channels                 ||
        p_ctl_elem->type            != AWSA_CORE_CTL_ELEM_TYPE_BOOLEAN) {
        return -AW_EPERM;
    }

    ctl_value.value.integer.p_value = vol;
    ret = awsa_core_ctl_elem_value_read(p_ctl_elem, &ctl_value);
    if (ret != AW_OK) {
        return ret;
    }

    *p_value = ctl_value.value.integer.p_value[channel];
    return AW_OK;
}

/* end of file */
