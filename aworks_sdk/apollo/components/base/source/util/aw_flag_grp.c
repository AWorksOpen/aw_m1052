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
 * \brief flag group component
 *
 * \internal
 * \par modification history:
 * - 1.01 17-04-25  may, add cpu lock to protect, fix bug.
 * - 1.00 15-06-01  dav, first implementation
 * \endinternal
 */

#include "aw_flag_grp.h"
#include "aw_sem.h"
#include "aw_int.h"
#include "aw_task.h"

#ifdef  __FLAG_DEBUG
#include "aw_imx28x_micro_sec.h"
#include "aw_vdebug.h"
#endif

#define  AW_FLAG_NODE_STATE_INUSE   0x01    /**< \brief 节点正在使用 */
#define  AW_FLAG_NODE_STATE_ISTERM  0x02    /**< \brief 节点已销毁*/

/******************************************************************************/
aw_err_t aw_flag_init(aw_flag_grp_t    *p_flag_grp,
                      uint32_t          flags,
                      aw_flag_wait_q_t  wait_q)
{
    if (NULL == p_flag_grp) {
        return -AW_EINVAL;
    }

    aw_list_head_init( &p_flag_grp->wait_list_head );
    p_flag_grp->flags = flags;
    p_flag_grp->wait_q = wait_q;
    p_flag_grp->state  = 0;

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_flag_node_init(aw_flag_node_t  *p_node)
{
    if (NULL == p_node) {
        return -AW_EINVAL;
    }

    AW_INIT_LIST_HEAD(&p_node->list_node);

    AW_SEMB_INIT( p_node->sem, 0, AW_SEM_Q_PRIORITY);
    p_node->use_sta = 0;
    p_node->prio    = aw_task_lowest_priority();

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_flag_node_terminate(aw_flag_node_t *p_node)
{
    AW_INT_CPU_LOCK_DECL(old);

    if (NULL == p_node) {
        return -AW_EINVAL;
    }

    AW_INT_CPU_LOCK(old);
    p_node->use_sta |= AW_FLAG_NODE_STATE_ISTERM;
    p_node->flags = 0;
    p_node->flag_buf  = 0;
    aw_list_del_init(&p_node->list_node);
    AW_INT_CPU_UNLOCK(old);

    AW_SEMB_TERMINATE( p_node->sem );

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_flag_pend(aw_flag_grp_t  *p_flag_grp,
                      aw_flag_node_t *p_node,
                      uint32_t        flags,
                      uint32_t        wait_type,
                      uint32_t        timeout,
                      uint32_t       *p_rev)
{
    int         ret         = AW_OK;
    uint32_t    flag_ready  = 0;
    aw_bool_t      match_ok    = AW_FALSE;
    AW_INT_CPU_LOCK_DECL(old);

    if ((NULL == p_flag_grp) ||
        (NULL == p_node)) {
        return -AW_EINVAL;
    }

    if (0 == flags) {
        return  -AW_EINVAL;
    }

    AW_INT_CPU_LOCK(old);

#ifdef  __FLAG_DEBUG
    uint32_t pre_us = aw_imx28_micro_sec_get();
#endif

    /* 判断当前节点是否已销毁  */
    if ((AW_FLAG_NODE_STATE_ISTERM & p_flag_grp->state) ||
        (AW_FLAG_NODE_STATE_ISTERM & p_node->use_sta) ) {
        AW_INT_CPU_UNLOCK(old);
        return  -AW_ENODEV;
    }

    /* 当前节点被占用，防止被多个线程使用 */
    if (AW_FLAG_NODE_STATE_INUSE & p_node->use_sta) {
        AW_INT_CPU_UNLOCK(old);
        return  -AW_EBUSY;
    }

    p_node->wait_type = wait_type;
    p_node->flags     = flags;
    p_node->flag_buf  = 0;

    /* 用同或操作来判断条件是否符合 */
    flag_ready = (~(flags ^ p_flag_grp->flags)) & flags;

    /* 不管或与，说明条件都不满足 */
    if (!flag_ready) {
        goto __no_flag_match;
    }

    if (wait_type & AW_FLAG_NODE_WAIT_AND) {
        if (flag_ready == p_node->flags) {
            match_ok = AW_TRUE;
        }
    } else if (wait_type & AW_FLAG_NODE_WAIT_OR) {
        if (flag_ready & p_node->flags) {
            match_ok = AW_TRUE;
        }
    } else {
        AW_INT_CPU_UNLOCK(old);
        return -AW_EINVAL;
    }

    /* 条件符合 */
    if (match_ok) {
        if (p_rev != NULL) {
            *p_rev = flag_ready;
        }
        /* 如果需要清除标志 */
        if (wait_type & AW_FLAG_NODE_WAIT_CLEAR) {
            p_flag_grp->flags &= (~flag_ready);
        }
        p_node->use_sta &= ~AW_FLAG_NODE_STATE_INUSE;
        AW_INT_CPU_UNLOCK(old);
        return AW_OK;
    }

__no_flag_match:

    /* 说明当前节点需要等待事件发生 */
    if (timeout > 0) {
        /* 如果当前事件组选择以优先级排序 */
        if (AW_FLAG_WAIT_Q_PRIORITY == p_flag_grp->wait_q) {

            aw_flag_node_t          *p_node0     = NULL;
            struct aw_list_head     *p_pos       = NULL;
            struct aw_list_head     *p_pos_tmp   = NULL;
            aw_bool_t                   insert      = AW_FALSE;

            /* 获取当前线程优先级 */
            p_node->prio = aw_task_priority_get(aw_task_id_self());
            AW_INIT_LIST_HEAD(&p_node->list_node);

            /* 找到正确的优先级插入位置 */
            aw_list_for_each_safe(p_pos, p_pos_tmp, &p_flag_grp->wait_list_head) {
                p_node0 = aw_list_entry(p_pos, aw_flag_node_t, list_node);
                if (p_node->prio < p_node0->prio) {
                    aw_list_add_tail(&p_node->list_node, &p_node0->list_node);
                    insert = AW_TRUE;
                    break;
                }
            }
            if (!insert) {
                aw_list_add_tail(&p_node->list_node, &p_flag_grp->wait_list_head);
            }
        } else {
            aw_list_add_tail(&p_node->list_node, &p_flag_grp->wait_list_head);
        }

    /* 超时时间为0，说明当前只想探测，当前没有满足条件 */
    } else {
        ret = -AW_ETIME;
        if (p_rev != NULL) {
            *p_rev = 0;
        }
        AW_INT_CPU_UNLOCK(old);
        return  ret;
    }

    p_node->use_sta |= AW_FLAG_NODE_STATE_INUSE;

#ifdef  __FLAG_DEBUG
    {
        uint32_t    cur_us = aw_imx28_micro_sec_get();
        AW_INFOF(("\n\n\n------prio: %d, pend use mico: %d\n", 
                 p_node->prio, 
                 cur_us - pre_us));
    }
#endif

    AW_INT_CPU_UNLOCK(old);

    ret = AW_SEMB_TAKE(p_node->sem, timeout);

    AW_INT_CPU_LOCK(old);

    aw_list_del_init(&p_node->list_node);
    p_node->use_sta &= ~AW_FLAG_NODE_STATE_INUSE;

    /* 等待成功 */
    if (AW_OK == ret) {
        if (p_rev != NULL) {
            *p_rev = p_node->flag_buf;
        }
        p_node->flag_buf  = 0;
    }

    AW_INT_CPU_UNLOCK(old);

    return  ret;
}

/******************************************************************************/
aw_err_t aw_flag_post(aw_flag_grp_t    *p_flag_grp,
                      uint32_t          flags,
                      aw_flag_opt_t     option)
{
    aw_err_t                ret         = AW_OK;
    uint32_t                flag_ready  = 0;
    aw_flag_node_t         *p_node      = NULL;
    struct aw_list_head    *p_pos       = NULL;
    struct aw_list_head    *p_pos_tmp   = NULL;
    aw_bool_t                  match_ok    = AW_FALSE;

    struct aw_list_head     pend_ok_list;

    AW_INT_CPU_LOCK_DECL(old);

    if (NULL == p_flag_grp) {
        return -AW_EINVAL;
    }

    AW_INT_CPU_LOCK(old);

    if (p_flag_grp->state & AW_FLAG_NODE_STATE_ISTERM) {
        AW_INT_CPU_UNLOCK(old);
        return  -AW_ENODEV;
    }

    switch (option) {
        case AW_FLAG_OPT_CLR:
           p_flag_grp->flags &= (uint32_t)~flags;
           break;

       case AW_FLAG_OPT_SET:
           p_flag_grp->flags |= flags;
           break;

       case AW_FLAG_OPT_CPY:
           p_flag_grp->flags = flags;
           break;

       default:
           AW_INT_CPU_UNLOCK(old);
           return -AW_EINVAL;
    }

    aw_list_head_init(&pend_ok_list);

    aw_list_for_each_safe(p_pos, p_pos_tmp, &p_flag_grp->wait_list_head) {
#ifdef  __FLAG_DEBUG
    uint32_t pre_us = aw_imx28_micro_sec_get();
#endif
        p_node = aw_list_entry(p_pos, aw_flag_node_t, list_node);

        /* 用同或操作来判断条件是否符合 */
        flag_ready = (~(p_node->flags ^ p_flag_grp->flags)) & p_node->flags;

        if (flag_ready) {

            match_ok = AW_FALSE;

            if (AW_FLAG_NODE_WAIT_AND & p_node->wait_type) {
                if (flag_ready == p_node->flags) {
                    match_ok = AW_TRUE;
                }
            } else if (AW_FLAG_NODE_WAIT_OR & p_node->wait_type) {
                if (flag_ready & p_node->flags) {
                    match_ok = AW_TRUE;
                }
            }

            /* 条件符合 */
            if (match_ok) {
                p_node->flag_buf = flag_ready;
                if (p_node->wait_type & AW_FLAG_NODE_WAIT_CLEAR) {
                    p_flag_grp->flags &= (~flag_ready);
                }
                /* 先清除该节点，防止其他线程调用该接口，该节点重复匹配 */
                aw_list_del_init(&p_node->list_node);

                /* 将其添加至 pend_ok 链表中 */
                aw_list_add_tail(&p_node->list_node, &pend_ok_list);

#ifdef  __FLAG_DEBUG
    {
        uint32_t    cur_us = aw_imx28_micro_sec_get();
        AW_INFOF(("\n\n\n****** post use mico: %d\n", cur_us - pre_us));
    }
#endif
            }
        }
    }

    AW_INT_CPU_UNLOCK(old);

    /* 循环处理条件成立的等待节点 */
    {
        aw_list_for_each_safe(p_pos, p_pos_tmp, &pend_ok_list) {
            p_node = aw_list_entry(p_pos, aw_flag_node_t, list_node);
            aw_list_del_init(&p_node->list_node);
            AW_SEMB_GIVE( p_node->sem );
        }
    }

    return ret;
}

aw_err_t aw_flag_terminate(aw_flag_grp_t *p_flag_grp)
{
    int                     ret         = AW_OK;
    aw_flag_node_t         *p_node      = NULL;
    struct aw_list_head    *p_pos       = NULL;
    struct aw_list_head    *p_pos_tmp   = NULL;
    AW_INT_CPU_LOCK_DECL(old);

    if (NULL == p_flag_grp) {
        return -AW_EINVAL;
    }

    AW_INT_CPU_LOCK(old);
    p_flag_grp->state |= AW_FLAG_NODE_STATE_ISTERM;
    p_flag_grp->flags = 0;

    aw_list_for_each_safe(p_pos, p_pos_tmp, &p_flag_grp->wait_list_head) {
        p_node = aw_list_entry(p_pos, aw_flag_node_t, list_node);
        AW_INT_CPU_UNLOCK(old);
        aw_flag_node_terminate(p_node);
        AW_INT_CPU_LOCK(old);
    }

    aw_list_head_init( &p_flag_grp->wait_list_head );

    AW_INT_CPU_UNLOCK(old);

    return AW_OK;
}
