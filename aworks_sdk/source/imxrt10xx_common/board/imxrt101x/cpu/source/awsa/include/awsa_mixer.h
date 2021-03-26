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

#ifndef __AWSA_MIXER_H
#define __AWSA_MIXER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_types.h"
#include "aw_common.h"

struct awsa_mixer;
struct awsa_mixer_elem;

/**
 * \brief Mixer simple element channel
 */
typedef enum awsa_mixer_elem_chan {

    /** \brief Unknown */
    AWSA_MIXER_CHAN_UNKNOWN          = -1,

    /** \brief Front left */
    AWSA_MIXER_CHAN_FRONT_LEFT       = 0,

    /** \brief Front right */
    AWSA_MIXER_CHAN_FRONT_RIGHT,

    /** \brief Rear left */
    AWSA_MIXER_CHAN_REAR_LEFT,

    /** \brief Rear right */
    AWSA_MIXER_CHAN_REAR_RIGHT,

    /** \brief Front center */
    AWSA_MIXER_CHAN_FRONT_CENTER,

    /** \brief Woofer */
    AWSA_MIXER_CHAN_WOOFER,

    /** \brief Side Left */
    AWSA_MIXER_CHAN_SIDE_LEFT,

    /** \brief Side Right */
    AWSA_MIXER_CHAN_SIDE_RIGHT,

    /** \brief Rear Center */
    AWSA_MIXER_CHAN_REAR_CENTER,

    /** \brief last */
    AWSA_MIXER_CHAN_LAST             = 9,

    /** \brief Mono (Front left alias) */
    AWSA_MIXER_CHAN_MONO             = AWSA_MIXER_CHAN_FRONT_LEFT

} awsa_mixer_elem_chan_t;

/**
 * \brief Mixer
 */
struct awsa_mixer {
    int        card;          /**< \brief card number */
    void      *p_cookie;      /**< \brief temporary value */
    aw_bool_t  active;        /**< \brief mixer active flg */
};

/**
 * \brief Mixer elem
 */
struct awsa_mixer_elem {
    struct awsa_mixer  *p_mixer;   /**< \brief mixer */
    const char         *p_name;    /**< \brief element name */
    uint32_t            channels;  /**< \brief channel count */
    aw_bool_t           dir;       /**< \brief playback or capture */
    void               *p_cookie;  /**< \brief temp */
};

/**
 * \brief open mixer device
 *
 * \param[in] p_mixer : mixer pointer
 * \param[in] card    : sound card id
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : parameter invalid
 */
aw_err_t awsa_mixer_open (struct awsa_mixer *p_mixer, int card);

/**
 * \brief close mixer device
 *
 * \param[in] p_mixer : mixer pointer
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : parameter invalid
 */
aw_err_t awsa_mixer_close (struct awsa_mixer *p_mixer);

/**
 * \brief get elements count
 * \param[in] p_mixer : mixer pointer
 * \retval mixer elements count
 */
uint32_t awsa_mixer_elem_count_get (struct awsa_mixer *p_mixer);

/**
 * \brief find mixer element by element id
 * \note index is used to distinguish controls with the same name
 *
 * \param[in] p_mixer : mixer pointer
 * \param[in] p_id    : element id
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : parameter invalid
 */
aw_err_t awsa_mixer_elem_find (struct awsa_mixer_elem *p_elem,
                               struct awsa_mixer      *p_mixer,
                               const char             *p_name);

/**
 * \brief Get element channel(values) count
 * \param[in] p_elem : element pointer
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : parameter invalid
 */
uint32_t awsa_mixer_elem_channel_count_get (struct awsa_mixer_elem *p_elem);

/**
 * \brief get element volume
 *
 * \param[in]  p_elem  : element pointer
 * \param[in]  channel : element id
 * \param[out] p_value : volume value
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : parameter invalid
 */
aw_err_t awsa_mixer_elem_volume_get (struct awsa_mixer_elem *p_elem,
                                     awsa_mixer_elem_chan_t  channel,
                                     int32_t                *p_volume);

/**
 * \brief get playback dB
 *
 * \param[in]  p_elem  : element pointer
 * \param[in]  channel : element id
 * \param[out] p_value : dB value
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : parameter invalid
 */
aw_err_t awsa_mixer_elem_db_get (struct awsa_mixer_elem *p_elem,
                                 awsa_mixer_elem_chan_t  channel,
                                 int32_t                *p_db);

/**
 * \brief set playback volume
 *
 * \param[in]  p_elem  : element pointer
 * \param[in]  channel : element id
 * \param[out] value   : volume value
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : parameter invalid
 */
aw_err_t awsa_mixer_elem_volume_set (struct awsa_mixer_elem *p_elem,
                                     awsa_mixer_elem_chan_t  channel,
                                     int32_t                 volume);

/**
 * \brief set playback dB
 *
 * \param[in]  p_elem  : element pointer
 * \param[in]  channel : element id
 * \param[out] value   : dB value
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : parameter invalid
 */
aw_err_t awsa_mixer_elem_db_set (struct awsa_mixer_elem *p_elem,
                                 awsa_mixer_elem_chan_t  channel,
                                 int32_t                 db,
                                 int                     dir);

/**
 * \brief element volume range get
 */
aw_err_t awsa_mixer_elem_volume_range_get (struct awsa_mixer_elem *p_elem,
                                           int32_t                *p_min,
                                           int32_t                *p_max);

/**
 * \brief get element dB range
 */
aw_err_t awsa_mixer_elem_db_range_get (struct awsa_mixer_elem *p_elem,
                                       int32_t                *p_min,
                                       int32_t                *p_max);

/**
 * \brief set switch value
 */
aw_err_t awsa_mixer_elem_switch_set (struct awsa_mixer_elem *p_elem,
                                     awsa_mixer_elem_chan_t  channel,
                                     int                     value);

/**
 * \brief get switch value
 */
aw_err_t awsa_mixer_elem_switch_get (struct awsa_mixer_elem *p_elem,
                                     awsa_mixer_elem_chan_t  channel,
                                     int                    *p_value);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_MIXER_H */

/* end of file */
