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
 * \brief IMX1050 SDK Version source
 *
 * \internal
 * \par modification history:
 * - 1.00 18-04-27  mex, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_sdk_version.h"
#include "imx1050_sdk_version.h"

/*******************************************************************************
  locals
*******************************************************************************/

/* imx1050 sdk version */
const char *p_version = IMX1050_SDK_VERSION;

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief imx1050 get version number
 */
const char *aw_sdk_version_get (void)
{
    return p_version;
}

/* end of file */
