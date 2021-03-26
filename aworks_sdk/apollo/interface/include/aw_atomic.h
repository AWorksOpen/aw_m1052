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
 * \brief task 平台相关标准定义头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 19-10-23  zen, refine the description
 * \endinternal
 */

#ifndef __AW_ATOMIC_H
#define __AW_ATOMIC_H

#include "aw_psp_atomic.h"


/** \brief 原子类型变量v进行原子读操作 */
#define aw_atomic_get(v) aw_psp_atomic_get(v)

/** \brief 设置原子类型变量v的值为i */
#define aw_atomic_set(v,i) aw_psp_atomic_set(v,i) 

/** \brief 原子类型变量v加1,并返回结果 */
#define aw_atomic_inc_return(v) aw_psp_atomic_inc_return(v) 
 
/** \brief 原子类型变量v减1，并返回结果 */
#define aw_atomic_dec_return(v) aw_psp_atomic_dec_return(v) 

/** \brief 原子类型变量v加1,并返回结果 */
#define aw_atomic_add_return(i, v) aw_psp_atomic_add_return(i, v)

/** \brief 原子类型变量v减1，并返回结果 */
#define aw_atomic_sub_return(i, v) aw_psp_atomic_sub_return(i, v)

/** \brief 原子类型变量v加1，如果结果等于0，则返回真，否则返回假 */
#define aw_atomic_inc_and_test(v) aw_psp_atomic_inc_and_test(v) 

/** \brief 原子类型变量v减1，如果结果等于0，则返回真，否则返回假 */
#define aw_atomic_dec_and_test(v) aw_psp_atomic_dec_and_test(v) 

/** \brief 原子类型变量v减i，如果结果等于0，则返回真，否则返回假 */
#define aw_atomic_sub_and_test(i, v) aw_psp_atomic_sub_and_test(i, v) 

/** \brief 原子类型变量v加i，如果结果为负数，则返回真，否则返回假 */
#define aw_atomic_add_negative(i,v) aw_psp_atomic_add_negative(i,v) 

/** \brief 原子变量v与x先进行比较，当x不相等时， 将y赋值给原子变量 */
#define aw_atomic_cmpxchg(x, y, v) return aw_psp_atomic_cmpxchg(x, y, v);

#endif