/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief IMX1020 SDK Version source
 *
 * \internal
 * \par modification history:
 * - 1.00 18-09-21  mex, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_sdk_version.h"
#include "imx1020_sdk_version.h"

/*******************************************************************************
  locals
*******************************************************************************/

/* IMX1020 sdk version */
const char *p_version = IMX1020_SDK_VERSION;

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief IMX1020 get version number
 */
const char *aw_sdk_version_get (void)
{
    return p_version;
}

/* end of file */
