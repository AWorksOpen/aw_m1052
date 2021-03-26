/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief IMX1010 SDK Version source
 *
 * \internal
 * \par modification history:
 * - 1.00 18-09-21  mex, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_sdk_version.h"
#include "imx1010_sdk_version.h"

/*******************************************************************************
  locals
*******************************************************************************/

/* IMX1010 sdk version */
const char *p_version = IMX1010_SDK_VERSION;

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief IMX1010 get version number
 */
const char *aw_sdk_version_get (void)
{
    return p_version;
}

/* end of file */
