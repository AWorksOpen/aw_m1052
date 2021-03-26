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

#ifndef __AW_PSP_ATOMIC_H
#define __AW_PSP_ATOMIC_H

#include "rtk.h"                                /* 目前使用RTK内核 */
#include "rtk_atomic.h"
#include "aw_compiler.h"
#include "aw_types.h"


/** \brief 原子类型变量v进行原子读操作 */
#define aw_psp_atomic_get(v) rtk_atomic_get(v)

/** \brief 设置原子类型变量v的值为i */
#define aw_psp_atomic_set(v,i) rtk_atomic_set(v,i) 

/** \brief 原子类型变量v加1,并返回结果 */
#define aw_psp_atomic_inc_return(v) rtk_atomic_inc_return(v) 
 
/** \brief 原子类型变量v减1，并返回结果 */
#define aw_psp_atomic_dec_return(v) rtk_atomic_dec_return(v) 

/** \brief 原子类型变量v加1，如果结果等于0，则返回真，否则返回假 */
#define aw_psp_atomic_inc_and_test(v) rtk_atomic_inc_and_test(v) 

/** \brief 原子类型变量v减1，如果结果等于0，则返回真，否则返回假 */
#define aw_psp_atomic_dec_and_test(v) rtk_atomic_dec_and_test(v) 

/** \brief 原子类型变量v减i，如果结果等于0，则返回真，否则返回假 */
#define aw_psp_atomic_sub_and_test(i, v) rtk_atomic_sub_and_test(i, v) 

/** \brief 原子类型变量v加i，如果结果为负数，则返回真，否则返回假 */
#define aw_psp_atomic_add_negative(i,v) rtk_atomic_add_negative(i,v) 

/**
 * \brief 原子类型变量进行加法运算,并返回结果
 *
 * \param[in] add_val 加数
 * \param[in] p_val   指向原子变量的指针
 *
 * \return    原子变量加add_val之后的计算结果
 *
 */
static aw_inline int aw_psp_atomic_add_return(int add_val, atomic_t *p_val)
{
    return rtk_atomic_add_return(add_val, &p_val->counter);
}

/**
 * \brief 原子类型变量进行减法运算，并返回结果
 *
 * \param[in] sub_val 减数
 * \param[in] p_val   指向原子变量的指针
 *
 * \return    原子变量减add_val之后的计算结果
 *
 */
static aw_inline int aw_psp_atomic_sub_return(int sub_val, atomic_t *p_val)
{
    return rtk_atomic_sub_return(sub_val, &p_val->counter);
}


/**
 * \brief 原子变量与旧的数值先进行比较，当与旧的数值不相等时，
 *        将新的数值赋值给原子变量
 *
 * \param[in] old_val 旧的数值
 * \param[in] new_val 新的数值
 * \param[in] p_val   指向原子变量的指针
 *
 * \return    旧的原子变量数值
 *
 */
static aw_inline int aw_psp_atomic_cmpxchg(int old_val, int new_val, atomic_t *p_val)
{
    return rtk_atomic_cmpxchg(old_val, new_val, &p_val->counter);
}

#endif