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
 * \brief An array of strings used to hold the current time zone names.
 * \internal
 * \par modification history:
 * - 1.00 14-09-28 orz, first implementation.
 * \endinternal
 */

#include <time.h>
#include "mylib.h"

#ifdef  _TMS320C6X
#ifndef TZNAME_MAX
#define TZNAME_MAX  6
#endif
#endif
/******************************************************************************/
static char __g_tzname_0[TZNAME_MAX] = {"std"};
static char __g_tzname_1[TZNAME_MAX] = {"dst"};

/******************************************************************************/

/** \brief An array of strings used to hold the current time zone names. */
char *tzname[2] = {__g_tzname_0, __g_tzname_1};

/* end of file */
