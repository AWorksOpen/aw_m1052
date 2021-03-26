/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief (浮点计算) 性能测试
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能 对应平台UART宏。
 *
 * - 实验现象：
 *   1. 串口打印计算信息。
 *
 * \par 源代码
 * \snippet demo_vfp_float_param.c src_vfp_float_param
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-09  mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_vfp_float_param 浮点计算性能测试1
 * \copydoc demo_vfp_float_param.c
 */

/** [src_vfp_float_param] */

#include "aworks.h"
#include "aw_vdebug.h"
#include <stdarg.h>

static double __vfp_float_param_mul (int count, ...)
{
    double          r = 1.0f;
    double          a;
    int             i;

    va_list         ap;
    va_start(ap, count);

    for (i = 0;i < count;i++) {
        a = va_arg(ap,double);
        r *= a;
    }
    va_end(ap);
    return r;
}

void demo_vfp_float_param_entry (void)
{
    double r1, r2;
    double a, b, c, d, e, f, g, h;

    a = 3.1415926;
    b = 9.4892358;
    c = 0.61859;
    d = 3.698733;
    e = 3.61477855;
    f = 5.689423;
    g = 0.058945236;
    h = 32.0145895;

    r1 = a * b * c * d * e * f * g * h;
    r2 = __vfp_float_param_mul(8, a, b, c, d, e, f, g, h);
    if (r1 == r2) {
        aw_kprintf("demo_vfp_float_param_entry test OK.\r\n");
    }

    return ;
}


/** [src_vfp_float_param] */

/* end of file */
