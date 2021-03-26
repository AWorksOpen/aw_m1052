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
 * \brief Contains internal routine called by xscanf to handle %e and %f formats.
 *
 * \internal
 * \par modification history:
 * - 1.00 15-01-08 zen, first implementation
 * \endinternal
 */

#include "apollo.h"
#include <stdarg.h>
#include <stdlib.h>

/**
 * \brief Internal routine called by xscanf to handle %e and %f formats.
 *
 * \param pvar     Pointer to the variable to assign to.
 *
 * \param str      Pointer to the character string to convert.
 *
 * \param isdouble Assign to a double instead of a float..
 */
void xsfef(char *pvar, char *str, int isdouble)
{
    double dval = atof(str);

    if(isdouble) {
        * (double *) pvar = dval;
    } else {
        * (float *) pvar = (float) dval;
    }
}

/* endif file */


