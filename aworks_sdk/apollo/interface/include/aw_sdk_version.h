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
 * \brief SDK Version interface
 *
 * \internal
 * \par modification history:
 * - 1.00 16-07-06  sky, first implementation
 * \endinternal
 */

#ifndef __AW_SDK_VERSION_H
#define __AW_SDK_VERSION_H

#include "aworks.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief get version number
 * \return sdk version information
 */
const char *aw_sdk_version_get (void);

#ifdef __cplusplus
}
#endif

#endif /* __AW_SDK_VERSION_H */

/* end of file */
