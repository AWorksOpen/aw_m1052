/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Stock Co.,  Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 
 *
 * \internal
 * \par Modification history
 * - 1.00 20-10-15  win, first implementation.
 * \endinternal
 */

#ifndef __AW_MP3_SRC_CFG_H
#define __AW_MP3_SRC_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AW_MP3_DEC_OPT_STRICT
#define AW_MP3_DEC_OPT_STRICT    0
#endif

#ifndef AW_MP3_FPM_DEFAULT
#define AW_MP3_FPM_DEFAULT       1
#endif

#if (AW_MP3_FPM_DEFAULT)

#ifndef AW_MP3_OPT_SPEED
#define AW_MP3_OPT_SPEED         1
#endif

#endif

#ifndef AW_MP3_DEC_OPT_ACCURACY
#define AW_MP3_DEC_OPT_ACCURACY   0
#endif

#if (AW_MP3_OPT_SPEED) && (AW_MP3_DEC_OPT_ACCURACY)
#error "cannot optimize for both speed and accuracy"
# endif

#if (AW_MP3_OPT_SPEED)

#ifndef AW_MP3_DEC_OPT_SSO
#define AW_MP3_DEC_OPT_SSO         1
#endif

# endif

/** \brief  π”√ª„±‡±‡“Î */
#ifndef AW_MP3_DEC_ASO_IMDCT

#if defined(__CC_ARM)

#define AW_MP3_DEC_ASO_IMDCT         0

#elif defined(__ICCARM__)

#define AW_MP3_DEC_ASO_IMDCT         0

#elif defined(__GNUC__)

#define AW_MP3_DEC_ASO_IMDCT         0

#elif defined(__TMS470__)

#define AW_MP3_DEC_ASO_IMDCT         0

#elif defined(__TASKING__)

#define AW_MP3_DEC_ASO_IMDCT         0

#else

#define AW_MP3_DEC_ASO_IMDCT         0

#endif

#ifndef AW_MP3_DEC_ASO_INTERLEAVE1
#define AW_MP3_DEC_ASO_INTERLEAVE1   0
#endif

#ifndef AW_MP3_DEC_ASO_INTERLEAVE2
#define AW_MP3_DEC_ASO_INTERLEAVE2   0
#endif

#endif

#ifndef AW_MP3_ID3V2_3_FRAME_NUM
#define AW_MP3_ID3V2_3_FRAME_NUM      6
#endif

#ifndef AW_MP3_DEC_ASO_ZEROCHECK
#define AW_MP3_DEC_ASO_ZEROCHECK      0
#endif

#ifdef __cplusplus
}
#endif

#endif /* __AW_MP3_SRC_CFG_H */

/* end of file */
