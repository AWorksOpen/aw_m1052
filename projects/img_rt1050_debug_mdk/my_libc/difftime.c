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
 * \brief implementation of clock().
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-26 orz, first implementation.
 * \endinternal
 */

#include <time.h>
#ifndef  _TMS320C6X
/******************************************************************************/
double difftime (time_t time1, time_t time0)
{
    return (double)(time1 - time0);
}
#endif
/* end of file */
