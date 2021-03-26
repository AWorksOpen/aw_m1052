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
 * \brief define days per month array.
 *
 * \internal
 * \par modification history:
 * - 1.00 140926 orz, first implementation.
 * \endinternal
 */

#include <time.h>
#include "mylib.h"

#ifndef  _TMS320C6X
/******************************************************************************/
struct tm *localtime (const time_t *timer)
{
    time_t time = *timer;   /* in case *timer changing asynchronously */

    time += timezone;       /*  Perform the time zone adjustment */

    return gmtime(&time);
}
#endif
/* end of file */
