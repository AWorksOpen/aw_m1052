/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      AWorks.support@zlg.cn
*******************************************************************************/
/**
 * \file  rtk_atomic.h
 * \brief 原子操作接口头文件
 *
 * \internal
 * \par modification history:
 * - 2019-10-23  wk, first implementation.
 * \endinternal
 */
#ifndef __RTK_ATOMIC_H
#define __RTK_ATOMIC_H


#ifdef __cplusplus
extern "C" {
#endif


/** \brief 原子类型变量v进行原子读操作 */
#define rtk_atomic_get(v)  (*(volatile int *)&(v)->counter)

/** \brief 设置原子类型变量v的值为i */
#define rtk_atomic_set(v,i) (((v)->counter) = (i))

/** \brief 原子类型变量v加1,并返回结果 */
#define rtk_atomic_inc_return(v)    (rtk_atomic_add_return(1, v))

/** \brief 原子类型变量v减1，并返回结果 */
#define rtk_atomic_dec_return(v)    (rtk_atomic_sub_return(1, v))

/** \brief 原子类型变量v加1，如果结果等于0，则返回真，否则返回假 */
#define rtk_atomic_inc_and_test(v)  (rtk_atomic_add_return(1, v) == 0)

/** \brief 原子类型变量v减1，如果结果等于0，则返回真，否则返回假 */
#define rtk_atomic_dec_and_test(v)  (rtk_atomic_sub_return(1, v) == 0)

/** \brief 原子类型变量v减i，如果结果等于0，则返回真，否则返回假 */
#define rtk_atomic_sub_and_test(i, v) (rtk_atomic_sub_return(i, v) == 0)

/** \brief 原子类型变量v加i，如果结果为负数，则返回真，否则返回假 */
#define rtk_atomic_add_negative(i,v) (rtk_atomic_add_return(i, v) < 0)


/**
 * \brief 定义原子变量类型
 */
typedef struct {

    volatile int counter;

} atomic_t ;

/**
 * \brief 原子类型变量进行加法运算,并返回结果
 *
 * \param[in] add_val 加数
 * \param[in] p_val   指向原子变量的指针
 *
 * \return    原子变量加add_val之后的计算结果
 *
 */
int rtk_atomic_add_return(int add_val, atomic_t *p_val);

/**
 * \brief 原子类型变量进行减法运算，并返回结果
 *
 * \param[in] sub_val 减数
 * \param[in] p_val   指向原子变量的指针
 *
 * \return    原子变量减add_val之后的计算结果
 *
 */
int rtk_atomic_sub_return(int sub_val, atomic_t *p_val);


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
int rtk_atomic_cmpxchg(int old_val, int new_val, atomic_t *p_val);


#ifdef __cplusplus
}
#endif

#endif /* __ATOMIC_H */
