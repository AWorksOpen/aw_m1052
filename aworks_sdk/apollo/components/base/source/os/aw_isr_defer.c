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
 * \brief isr deferal job library configure file
 *
 * \internal
 * \par modification history:
 * - 1.10 16-05-02  dcf, to support node delete operation.
 * - 1.10 14-03-12  zen, split task-create
 * - 1.10 13-02-26  zen, modify module name from aw_isr_defer_xxxx to
 *                  aw_isr_deffer_xxxx
 * - 1.00 12-12-10  or2, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_int.h"
#include "aw_isr_defer.h"
#include "aw_sys_defer.h"
#include "aw_assert.h"

/*******************************************************************************
  macros
*******************************************************************************/
#define __IRQ_SAVE_DISABLE(olk) AW_INT_CPU_LOCK(old)
#define __IRQ_SAVE_RESTORE(old) AW_INT_CPU_UNLOCK(old)

/**
 *  \brief isr deferal job initialize
 */
void aw_isr_defer_job_init (struct aw_isr_defer_job *p_jq,
                            void                   (*pfunc) (void *),
                            void                    *p_arg)
{
    AW_INIT_LIST_HEAD(&p_jq->node);

    p_jq->func  = pfunc;
    p_jq->param = p_arg;
}


/**
 * \brief 判断节点是否可用
 *
 * \param[in] p_jq  节点指针。
 *
 * \retval  true    节点可用
 * \retval  false   节点不可用, 已经被添加到队列中
 */
aw_bool_t aw_isr_defer_job_is_usable (struct aw_isr_defer_job *p_jq)
{
    return  aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_ISR, 
                                       (struct aw_defer_djob *)p_jq);
}

aw_err_t aw_isr_defer_job_add(struct aw_isr_defer_job *p_jq)
{
    return aw_sys_defer_commit(AW_SYS_DEFER_TYPE_ISR, 
                               (struct aw_defer_djob *)p_jq);
}

aw_err_t aw_isr_defer_job_del(struct aw_isr_defer_job *p_jq)
{
    return aw_sys_defer_abort(AW_SYS_DEFER_TYPE_ISR, 
                              (struct aw_defer_djob *)p_jq);
}

