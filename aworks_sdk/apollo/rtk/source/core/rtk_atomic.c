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
 * \file  rtk_atomic.c
 * \brief rtk 原子操作函数
 *
 * \internal
 * \par modification history:
 * 1.00 19-10-23  wk, first implement
 * \endinternal
 */
#include "rtk_atomic.h"
#include "rtk.h"

int rtk_atomic_add_return(int add_val, atomic_t *p_val)
{
    return rtk_cpu_atomic_add(add_val, &p_val->counter);
}

int rtk_atomic_sub_return(int sub_val, atomic_t *p_val)
{
    return rtk_cpu_atomic_sub(sub_val, &p_val->counter);
}

int rtk_atomic_cmpxchg(int old_val, int new_val, atomic_t *p_val)
{
    return rtk_cpu_atomic_cmpxchg(old_val, new_val, &p_val->counter);
}
