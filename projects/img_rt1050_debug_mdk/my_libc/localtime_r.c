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
 * \brief implementation of localtime_r().
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-25 orz, first implementation.
 * \endinternal
 */

#include <time.h>
#include "mylib.h"
extern struct tm *gmtime_r (const time_t *timer, struct tm *result);

#ifndef  _TMS320C6X
/******************************************************************************/
struct tm *localtime_r (const time_t *timer, struct tm *result)
{
    time_t time = *timer;   /* in case *timer changing asynchronously */

    time += timezone;       /*  Perform the time zone adjustment */
		
	  struct tm * test = gmtime_r(&time, result);
    return (test);
}
#endif
/* end of file */
