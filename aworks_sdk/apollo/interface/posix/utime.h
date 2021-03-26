/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief standard symbolic constants and types.
 *
 * \internal
 * \par modification history:
 * - 150420 deo, first implementation.
 * \endinternal
 */

#ifndef UTIME_H_
#define UTIME_H_


#include "io/aw_utime.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
typedef struct aw_utimbuf utimbuf;

/******************************************************************************/
#define utime		aw_utime

#ifdef __cplusplus
}
#endif

#endif /* UTIME_H_ */
