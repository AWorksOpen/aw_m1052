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

#ifndef __AWSA_CORE_PCM_LIB_H
#define __AWSA_CORE_PCM_LIB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "core/awsa_core_internal.h"

/**
 * \brief start pcm substream
 * \param[in] p_substream : pcm subtream
 * \retval AW_OK : success
 * \retval <0    : fail
 */
aw_err_t awsa_core_pcm_lib_start (struct awsa_core_pcm_substream *p_substream);

/**
 * \brief stop pcm substream
 * \param[in] p_substream : pcm subtream
 * \param[in] state       : AWSA_CORE_PCM_STATE_*
 * \retval AW_OK : success
 * \retval <0    : fail
 */
aw_err_t awsa_core_pcm_lib_stop (struct awsa_core_pcm_substream *p_substream,
                                 int                             state);

/**
 * \brief prepare pcm substream
 * \param[in] p_substream : pcm subtream
 * \retval AW_OK : success
 * \retval <0    : fail
 */
aw_err_t awsa_core_pcm_lib_prepare (
    struct awsa_core_pcm_substream *p_substream);

/**
 * \brief pause pcm substream
 * \param[in] p_substream : pcm subtream
 * \retval AW_OK : success
 * \retval <0    : fail
 */
aw_err_t awsa_core_pcm_lib_pause (struct awsa_core_pcm_substream *p_substream,
                                  int                             push);

/**
 * \brief drain pcm substream
 * \param[in] p_substream : pcm subtream
 * \retval AW_OK : success
 * \retval <0    : fail
 */
aw_err_t awsa_core_pcm_lib_drain (struct awsa_core_pcm_substream *p_substream);

/**
 * \brief drain pcm substream finish
 * \param[in] p_substream : pcm subtream
 * \retval AW_OK : success
 * \retval <0    : fail
 */
aw_err_t awsa_core_pcm_lib_drain_done (
    struct awsa_core_pcm_substream *p_substream);

/**
 * \brief read pcm substream
 *
 * \param[in]  p_substream : pcm subtream
 * \param[out] p_buf       : buffer
 * \param[in]  size        : buffer size
 *
 * \return read data size
 */
awsa_pcm_sframes_t awsa_core_pcm_lib_read (
    struct awsa_core_pcm_substream *p_substream,
    void                           *p_buf,
    awsa_pcm_uframes_t              size);

/**
 * \brief write pcm substream
 *
 * \param[in] p_substream : pcm subtream
 * \param[in] p_buf       : buffer
 * \param[in] size        : buffer size
 *
 * \return write data size
 */
awsa_pcm_sframes_t awsa_core_pcm_lib_write (
    struct awsa_core_pcm_substream *p_substream,
    const void                     *p_buf,
    awsa_pcm_uframes_t              size);

/**
 * \brief update pcm substream
 *
 * \param[in] p_substream  : pcm subtream
 * \param[in] in_interrupt : TRUE  - in interrupt call
 *                           FALSE - not in interrupt call
 *
 * \retval AW_OK : success
 * \retval <0    : fail
 */
aw_err_t awsa_core_pcm_lib_update (
    struct awsa_core_pcm_substream *p_substream,
    aw_bool_t                       in_interrupt);

/**
 * \brief drop pcm substream
 * \param[in] p_substream  : pcm subtream
 * \retval AW_OK : success
 * \retval <0    : fail
 */
aw_err_t awsa_core_pcm_lib_drop (struct awsa_core_pcm_substream *p_substream);

/**
 * \brief update avail frames
 * \param[in]      p_substream  : pcm subtream
 * \retval AW_OK : success
 * \retval <0    : fail
 */
aw_err_t awsa_core_pcm_lib_avail_update (
    struct awsa_core_pcm_substream *p_substream);

/**
 * \brief Release pcm stream
 * \param[in]      p_substream  : pcm subtream
 * \retval AW_OK : success
 * \retval <0    : fail
 */
aw_err_t awsa_core_pcm_lib_release (
    struct awsa_core_pcm_substream *p_substream);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __AWSA_CORE_PCM_LIB_H */

/* end of file */
