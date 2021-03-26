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
 * \brief implementation of ctime().
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-25 orz, first implementation.
 * \endinternal
 */

#include <time.h>
#ifndef  _TMS320C6X
/******************************************************************************/
char *ctime (const time_t *clock)
{
    struct tm *loc_time;
    struct tm  tm_buf;

    if (NULL != clock) {
        if ((loc_time = localtime_r(clock, &tm_buf)) != NULL) {
            return asctime(loc_time);
        }
    }
    return NULL;
}
#endif

/* end of file */
