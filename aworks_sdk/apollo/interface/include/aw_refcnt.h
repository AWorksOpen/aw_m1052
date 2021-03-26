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
 * \brief reference count
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-25  deo, first implementation
 * \endinternal
 */


#ifndef __AW_REFCNT_H
#define __AW_REFCNT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_refcnt
 * \copydoc aw_refcnt.h
 * @{
 */

#include "aw_common.h"
#include "aw_spinlock.h"
#include "aw_sem.h"

/**
 * \brief 引用计数结构体。
 */
struct aw_refcnt {
    int  cnt;   /**< \brief 计数值 */
};


/**
 * \brief 初始化引用计数对象。
 * \param[in,out]  p_ref: 引用计数对象指针
 */
aw_local aw_inline void aw_refcnt_init (struct aw_refcnt *p_ref)
{
    p_ref->cnt = 1;
}


/**
 * \brief 检查引用计数是否有效。
 * \param[in]  p_ref: 引用计数对象指针
 */
aw_local aw_inline aw_bool_t aw_refcnt_valid (struct aw_refcnt  *p_ref)
{
    return (p_ref->cnt != 0);
}


/**
 * \brief 引用计数加。
 * \param[in,out]  p_ref:  引用计数对象指针
 */
aw_local aw_inline int aw_refcnt_get (struct aw_refcnt *p_ref)
{
    aw_spinlock_isr_t   lock;

    aw_spinlock_isr_init(&lock, 0);
    aw_spinlock_isr_take(&lock);

    if (p_ref->cnt == 0) {
        aw_spinlock_isr_give(&lock);
        return -AW_EPERM;
    }
    p_ref->cnt++;
    aw_spinlock_isr_give(&lock);

    return AW_OK;
}


/**
 * \brief 引用计数减
 *
 * \param[in,out]  p_ref:       引用计数对象指针
 * \param[in]      pfn_release: 回调函数，引用计数释放后执行
 */
aw_local aw_inline aw_err_t aw_refcnt_put (
                        struct aw_refcnt  *p_ref,
                        void             (*pfn_release) (struct aw_refcnt *p_ref))
{
    aw_spinlock_isr_t   lock;

    aw_spinlock_isr_init(&lock, 0);

    aw_spinlock_isr_take(&lock);
    if (p_ref->cnt == 0) {
        aw_spinlock_isr_give(&lock);
        return -AW_EPERM;
    }
    p_ref->cnt--;
    if ((p_ref->cnt == 0) && (pfn_release)) {
        aw_spinlock_isr_give(&lock);
        pfn_release(p_ref);
        return AW_OK;
    }

    aw_spinlock_isr_give(&lock);
    return AW_OK;
}

/** @}  grp_aw_if_refcnt */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_REFCNT_H */

/* end of file */
