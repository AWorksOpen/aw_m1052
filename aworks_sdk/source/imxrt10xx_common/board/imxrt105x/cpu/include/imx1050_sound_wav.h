/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief The IMX1050 Synchronous Audio Interface driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-13, tee, first implementation.
 * \endinternal
 */
#ifndef __AWBL_IMX1050_SOUND_WAV_H
#define __AWBL_IMX1050_SOUND_WAV_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "aw_spinlock.h"

/**
 * \brief Play (sync mode)
 *
 * \param[in] p_wav_buf  : The buffer store the wav data
 * \param[in] p_wav_buf  : The total length of the wav data
 *
 * \retval AW_OK play finished with no error
 * \retval < 0   play failed
 *
 */
aw_err_t  aw_imx1050_sound_wav_play_sync (const uint8_t *p_wav_buf, size_t len);

/**
 * \brief Play (sync mode)
 *
 * \param[in] filename  : the wave file want to play
 *
 * \retval AW_OK play finished with no error
 * \retval < 0   play failed
 *
 */
aw_err_t  aw_imx1050_sound_wav_file_play_sync (const char *filename);

/**
 * \brief Play (async mode)
 *
 * \param[in] sai_id        : The SAI channel
 * \param[in] p_trans       : The data to be transferred
 * \param[in] pfn_complete  : The transmit complete callback function
 * \param[in] p_arg         : The user arg for the pfn_complete
 *
 * \retval AW_OK start play successful
 * \retval < 0   start paly failed
 */
aw_err_t  aw_imx1050_sound_wav_play_async (const uint8_t    *p_wav_buf,
                                          size_t            len,
                                          aw_pfuncvoid_t    pfn_complete,
                                          void             *p_arg);
/**
 * \brief Play (async mode)
 *
 * \param[in] filename      : the wave file want to play
 * \param[in] pfn_complete  : The transmit complete callback function
 * \param[in] p_arg         : The user arg for the pfn_complete
 *
 * \retval AW_OK start play successful
 * \retval < 0   start paly failed
 */
aw_err_t  aw_imx1050_sound_wav_file_play_async (const char *filename,
                                          aw_pfuncvoid_t   pfn_complete,
                                          void            *p_arg);

/**
 * \brief Stop an async play
 * \return AW_OK
 */
aw_err_t aw_imx1050_sound_wav_play_stop (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */


