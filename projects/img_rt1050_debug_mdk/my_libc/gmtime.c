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
 * \brief implementation of gmtime().
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-26 orz, first implementation.
 * \endinternal
 */

#include <time.h>
extern struct tm *gmtime_r (const time_t *timer, struct tm *result);
/******************************************************************************/
#ifndef  _TMS320C6X
struct tm *gmtime (const time_t *timer)
{
    static struct tm result;

    return gmtime_r(timer, &result);
}
#endif

/* end of file */
