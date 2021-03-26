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
 * \brief Contains the routine to read from the specified string.
 *
 * \internal
 * \par modification history:
 * - 1.0.0 15-01-08 zen, first implementation
 * \endinternal
 */

/******************************************************************************/
#include "apollo.h"
#include <stdarg.h>

/******************************************************************************/
extern int xscanf(int (*getcfn)(void *),int (*ungetcfn)(int, void *),
           void *param, const char *fmt, va_list parglist);

/******************************************************************************/
#ifndef  _TMS320C6X
int sscanf (const char * str, const char * fmt, ...)
{
    int         ret;
    va_list     arg_list;

    va_start(arg_list, fmt);

    ret = xscanf((int (*)(void *))0, (int (*)(int, void *))0, (void *) str, fmt, arg_list);

    va_end(arg_list);

    return ((int)ret);
}
#endif
/* end of file */
