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
 * \brief 自旋锁(spin lock)标准接口
 *
 * 使用本服务需要包含头文件 aw_spinlock.h
 *
 * \par 简单示例一
 *
 * \code
 *  #include "aworks.h"
 *  #include "aw_spinlock.h"
 *
 *  aw_spinlock_isr_t lock;      // 定义一个自旋锁
 *
 *  aw_spinlock_isr_init(&lock); // 初始化锁
 *  aw_spinlock_isr_take(&lock); // 获取锁
 *  // 需要保护的临界代码...
 *  aw_spinlock_isr_give(&lock); // 释放锁
 * \endcode
 *
 * \par 简单示例二
 *
 * \code
 *  #include "aworks.h"
 *  #include "aw_spinlock.h"
 *
 *  aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;  // 定义并初始化一个自旋锁
 *
 *  aw_spinlock_isr_take(&lock); // 获取锁
 *  // 需要保护的临界代码...
 *  aw_spinlock_isr_give(&lock); // 释放锁
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 12-08-27  zen, first implementation
 * \endinternal
 */

#ifndef __AW_SPINLOCK_H
#define __AW_SPINLOCK_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_spinlock
 * \copydoc aw_spinlock.h
 * @{
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "aw_psp_spinlock.h"

/*******************************************************************************
  defines
*******************************************************************************/

/**
 * \brief ISR可调用自旋锁的未锁定初始值
 * \hideinitializer
 */
#define AW_SPINLOCK_ISR_UNLOCK     AW_PSP_SPINLOCK_ISR_UNLOCK

/**
 * \brief 定义并初始化一个ISR可调用自旋锁
 *
 * \param[in] x      需要定义的自旋锁
 * \param[in] flags  标志
 *
 * \hideinitializer
 */
#define AW_SPINLOCK_ISR_DECL(x, flags) AW_PSP_SPINLOCK_ISR_DECL((x), (flags))

/*******************************************************************************
  typedefs
*******************************************************************************/

/**
 * \brief ISR可调用自旋锁数据类型
 */
typedef aw_psp_spinlock_isr_t  aw_spinlock_isr_t;

/*******************************************************************************
  interfaces
*******************************************************************************/

/**
 * \brief 初始化一个ISR可调用自旋锁
 *
 * 本函数初始化一个由\a lock 指定的ISR可调用自旋锁。当前\a flags 没有被定义，
 * 保留给将来升级之用。一个自旋锁在第一次被使用前，必须被初始化！
 *
 * \attention 不可以在中断中调用此函数。
 *
 * \param[in,out]   p_lock 待初始化的锁
 * \param[in]       flags 标志
 */
void aw_spinlock_isr_init(aw_spinlock_isr_t *p_lock, int flags);

/**
 * \brief 释放一个ISR可调用自旋锁
 *
 * 本函数释放一个由\a lock 指定的ISR可调用自旋锁。
 *
 * \param[in,out]   p_lock 待初始化的锁
 */
void aw_spinlock_isr_give(aw_spinlock_isr_t *p_lock);

/**
 * \brief 请求一个ISR可调用自旋锁
 *
 * 本函数请求一个由\a lock 指定的ISR可调用自旋锁。
 *
 * \param[in,out]   p_lock 待初始化的锁
 */
void aw_spinlock_isr_take(aw_spinlock_isr_t *p_lock);

/** @}  grp_aw_if_spinlcok */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_SPINLOCK_H */

/* end of file */
