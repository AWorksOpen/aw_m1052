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
 * \brief AWBus Interrupt Controller interface implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-06  zen, first implemetation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_assert.h"
#include "awbus_lite.h"
#include "awbl_intctlr.h"
#include "aw_spinlock.h"
#include "aw_task.h"

/*******************************************************************************
  import
*******************************************************************************/

AWBL_METHOD_IMPORT(awbl_icserv_get);

/*******************************************************************************
  locals
*******************************************************************************/

/** \brief pointer to first intctlr service */
static struct awbl_intctlr_service     *__gp_intctlr_serv_head = NULL;
static aw_spinlock_isr_t                __g_spinlock_isr_intctlr_server_head;


/**
 * \brief find out a service who accept the inum
 */
static struct awbl_intctlr_service * __intctlr_get_serv_from_inum (int inum)
{

    struct awbl_intctlr_service     *p_serv_cur;

    aw_spinlock_isr_take(&__g_spinlock_isr_intctlr_server_head);

    p_serv_cur = __gp_intctlr_serv_head;

    /* todo: lock the list */

    while (p_serv_cur != NULL) {

        if ((inum >= p_serv_cur->p_servinfo->inum_start) &&
            (inum <= p_serv_cur->p_servinfo->inum_end)) {
                p_serv_cur ->ctlr_used_by_awbl ++;
                break;
        }
        p_serv_cur = p_serv_cur->p_next;
    }

    aw_spinlock_isr_give(&__g_spinlock_isr_intctlr_server_head);
    return p_serv_cur;
}



static void __intctlr_put_serv(struct awbl_intctlr_service * p_serv)
{
    aw_spinlock_isr_take(&__g_spinlock_isr_intctlr_server_head);
    p_serv ->ctlr_used_by_awbl --;
    aw_spinlock_isr_give(&__g_spinlock_isr_intctlr_server_head);
}

/******************************************************************************/
void awbl_intctlr_init (void)
{
    /* allocate intctlr services from instance tree */
    __gp_intctlr_serv_head = NULL;
    aw_spinlock_isr_init(&__g_spinlock_isr_intctlr_server_head,0);
}

static int is_intctlr_service_valid(struct awbl_intctlr_service *p_intctlr)
{
    aw_err_t                        ret = AW_OK;

    if (NULL != p_intctlr) {
        if (NULL != p_intctlr->p_servinfo) {
            if (NULL != p_intctlr->p_servfuncs ) {
                const struct awbl_intctlr_servfuncs * p;
                p = p_intctlr->p_servfuncs;
                if ( (NULL == p->pfunc_int_connect) ||
                        (NULL == p->pfunc_int_disconnect) ||
                        (NULL == p->pfunc_int_enable) ||
                        (NULL == p->pfunc_int_disable)) {
                    ret = -AW_EINVAL;
                }
            }
            else {
                ret = -AW_EINVAL;
            }
        }
        else {
            ret = -AW_EINVAL;
        }
    }
    else {
        ret = -AW_EINVAL;
    }

    return ret;
}

aw_err_t awbl_register_interrupt_controller(
        struct awbl_intctlr_service *p_intctlr)
{
    aw_err_t                        ret = AW_OK;
    struct awbl_intctlr_service    *p_cur,*p_next;

    // check parameter
    ret = is_intctlr_service_valid(p_intctlr);
    if (AW_OK != ret) {
        return ret;
    }

    aw_spinlock_isr_take(&__g_spinlock_isr_intctlr_server_head);
    p_cur = __gp_intctlr_serv_head;
    p_next = p_cur;
    while (NULL != p_next) {
        p_cur = p_next;
        p_next = p_next->p_next;
    }

    if (NULL == p_cur) {
        __gp_intctlr_serv_head = p_intctlr;
    }
    else {
        p_cur ->p_next = p_intctlr;
    }
    p_intctlr -> p_next = NULL;
    p_intctlr->ctlr_used_by_awbl = 0;
    aw_spinlock_isr_give(&__g_spinlock_isr_intctlr_server_head);
    return ret;
}

static aw_err_t __awbl_unregister_interrupt_controller(
    struct awbl_intctlr_service   *p_intctlr)
{
    aw_err_t                        ret = AW_OK;
    struct awbl_intctlr_service    *p_cur,*p_next;
    int                             find = 0;

    /* 查找到对应的中断控制器 */
    aw_spinlock_isr_take(&__g_spinlock_isr_intctlr_server_head);
    p_cur = __gp_intctlr_serv_head;
    p_next = p_cur;
    while (NULL != p_next) {
        p_cur = p_next;
        p_next = p_next->p_next;

        if (p_intctlr == p_next) {
            find = 1;
            break;
        }
    }

    /* 检查中断控制器是否被 子系统所使用 */
    if (find) {
        if ( 0 != p_intctlr->ctlr_used_by_awbl ) {
            ret = -AW_EBUSY;
        }
        else {
            p_cur ->p_next = p_intctlr->p_next;
            p_intctlr -> p_next = NULL;
        }
    }
    else {
        ret = -AW_EINVAL;
    }

    aw_spinlock_isr_give(&__g_spinlock_isr_intctlr_server_head);
    return ret;
}

/**
 * \brief 从中断子系统反注册一个终端控制器
 *
 * \param[in]   p_intctlr   要反注册的中断控制器
 *
 * \retval      AW_OK       成功
 * \retval      其他值      失败，是负的AW错误码
 */
aw_err_t awbl_unregister_interrupt_controller(
    struct awbl_intctlr_service   *p_intctlr)
{
    aw_err_t                        ret = AW_OK;

    ret = __awbl_unregister_interrupt_controller(p_intctlr);
    while (ret != AW_OK) {
        aw_task_delay(1);
        ret = __awbl_unregister_interrupt_controller(p_intctlr);
    }
    return ret;
}

/******************************************************************************/
aw_err_t aw_int_connect (int             inum,
                         aw_pfuncvoid_t  pfunc_isr,
                         void           *p_arg)
{
    aw_err_t                        ret = -AW_EINVAL;
    struct awbl_intctlr_service    *p_serv;

    if (NULL == pfunc_isr ) {
        return -AW_EINVAL;
    }

    p_serv = __intctlr_get_serv_from_inum(inum);

    if (p_serv != NULL) {
        if (NULL != p_serv->p_servfuncs->pfunc_int_connect) {
            inum -= p_serv->p_servinfo->inum_start;
            ret = p_serv->p_servfuncs->pfunc_int_connect(
                    p_serv->p_cookie,
                    inum,
                    pfunc_isr,
                    p_arg);
        }
        else {
            ret = -AW_ENOTSUP;
        }

        __intctlr_put_serv(p_serv);
    }

    return  ret;
}

/******************************************************************************/
aw_err_t aw_int_disconnect (int             inum,
                            aw_pfuncvoid_t  pfunc_isr,
                            void           *p_arg)
{
    aw_err_t                        ret = -AW_EINVAL;
    struct awbl_intctlr_service    *p_serv;

    if (NULL == pfunc_isr ) {
        return -AW_EINVAL;
    }

    p_serv = __intctlr_get_serv_from_inum(inum);

    if (p_serv != NULL) {
        if (NULL != p_serv->p_servfuncs->pfunc_int_disconnect) {
            inum -= p_serv->p_servinfo->inum_start;
            ret = p_serv->p_servfuncs->pfunc_int_disconnect(
                    p_serv->p_cookie,
                    inum,
                    pfunc_isr,
                    p_arg);
        }
        else {
            ret = -AW_ENOTSUP;
        }
        __intctlr_put_serv(p_serv);
    }

    return  ret;
}

/******************************************************************************/
aw_err_t aw_int_enable(int inum)
{
    aw_err_t                        ret = -AW_EINVAL;
    struct awbl_intctlr_service    *p_serv;

    p_serv = __intctlr_get_serv_from_inum(inum);

    if (p_serv != NULL) {
        if (NULL != p_serv->p_servfuncs->pfunc_int_enable) {
            inum -= p_serv->p_servinfo->inum_start;
            ret = p_serv->p_servfuncs->pfunc_int_enable(
                    p_serv->p_cookie,
                    inum);
        }
        else {
            ret = -AW_ENOTSUP;
        }
        __intctlr_put_serv(p_serv);
    }

    return  ret;
}

/******************************************************************************/
aw_err_t aw_int_disable(int inum)
{
    aw_err_t                        ret = -AW_EINVAL;
    struct awbl_intctlr_service    *p_serv;

    p_serv = __intctlr_get_serv_from_inum(inum);

    if (p_serv != NULL) {
        if (NULL != p_serv->p_servfuncs->pfunc_int_disable) {
            inum -= p_serv->p_servinfo->inum_start;
            ret = p_serv->p_servfuncs->pfunc_int_disable(
                    p_serv->p_cookie,
                    inum);
        }
        else {
            ret = -AW_ENOTSUP;
        }
        __intctlr_put_serv(p_serv);
    }

    return  ret;

}

aw_err_t aw_int_vector_connect(
        int inum,
        pfn_aw_int_vector_func_t pfunc_vector)
{
    aw_err_t                        ret = -AW_EINVAL;
    struct awbl_intctlr_service    *p_serv;

    if (NULL == pfunc_vector) {
        return -AW_EINVAL;
    }

    p_serv = __intctlr_get_serv_from_inum(inum);

    if (p_serv != NULL) {
        if (NULL != p_serv->p_servfuncs->pfunc_int_vector_connect) {
            inum -= p_serv->p_servinfo->inum_start;
            ret = p_serv->p_servfuncs->pfunc_int_vector_connect(
                    p_serv->p_cookie,
                    inum,
                    pfunc_vector);
        }
        else {
            ret = -AW_ENOTSUP;
        }
        __intctlr_put_serv(p_serv);
    }

    return  ret;
}

aw_err_t aw_int_vector_disconnect(
        int inum,
        pfn_aw_int_vector_func_t pfunc_vector)
{
    aw_err_t                        ret = -AW_EINVAL;
    struct awbl_intctlr_service    *p_serv;

    if (NULL == pfunc_vector) {
        return -AW_EINVAL;
    }

    p_serv = __intctlr_get_serv_from_inum(inum);

    if (p_serv != NULL) {
        if (NULL != p_serv->p_servfuncs->pfunc_int_vector_disconnect) {
            inum -= p_serv->p_servinfo->inum_start;
            ret = p_serv->p_servfuncs->pfunc_int_vector_disconnect(
                    p_serv->p_cookie,
                    inum,
                    pfunc_vector);
        }
        else {
            ret = -AW_ENOTSUP;
        }
        __intctlr_put_serv(p_serv);
    }

    return  ret;
}

aw_err_t aw_int_priority_get(int inum,int *p_cur_prio)
{
    aw_err_t                        ret = -AW_EINVAL;
    struct awbl_intctlr_service    *p_serv;

    if (NULL == p_cur_prio) {
        return -AW_EINVAL;
    }

    p_serv = __intctlr_get_serv_from_inum(inum);

    if (p_serv != NULL) {
        if (NULL != p_serv->p_servfuncs->pfunc_int_prio_get) {
            inum -= p_serv->p_servinfo->inum_start;
            ret = p_serv->p_servfuncs->pfunc_int_prio_get(
                    p_serv->p_cookie,
                    inum,
                    p_cur_prio);
        }
        else {
            ret = -AW_ENOTSUP;
        }
        __intctlr_put_serv(p_serv);
    }

    return  ret;
}

aw_err_t aw_int_priority_range_get(int inum,int *p_min_prio,int *p_max_prio)
{
    aw_err_t                        ret = -AW_EINVAL;
    struct awbl_intctlr_service    *p_serv;

    if (NULL == p_min_prio || NULL == p_max_prio) {
        return -AW_EINVAL;
    }

    p_serv = __intctlr_get_serv_from_inum(inum);

    if (p_serv != NULL) {
        if (NULL != p_serv->p_servfuncs->pfunc_int_prio_range_get) {
            inum -= p_serv->p_servinfo->inum_start;
            ret = p_serv->p_servfuncs->pfunc_int_prio_range_get(
                    p_serv->p_cookie,
                    inum,
                    p_min_prio,
                    p_max_prio);
        }
        else {
            ret = -AW_ENOTSUP;
        }
        __intctlr_put_serv(p_serv);
    }

    return  ret;
}

aw_err_t aw_int_priority_set(int inum,int new_prio)
{
    aw_err_t                        ret = -AW_EINVAL;
    struct awbl_intctlr_service    *p_serv;


    p_serv = __intctlr_get_serv_from_inum(inum);

    if (p_serv != NULL) {
        if (NULL != p_serv->p_servfuncs->pfunc_int_prio_set) {
            inum -= p_serv->p_servinfo->inum_start;
            ret = p_serv->p_servfuncs->pfunc_int_prio_set(
                    p_serv->p_cookie,
                    inum,
                    new_prio);
        }
        else {
            ret = -AW_ENOTSUP;
        }
        __intctlr_put_serv(p_serv);
    }
    return  ret;
}

aw_err_t aw_int_pend(int inum)
{
    aw_err_t                        ret = -AW_EINVAL;
    struct awbl_intctlr_service    *p_serv;


    p_serv = __intctlr_get_serv_from_inum(inum);

    if (p_serv != NULL) {
        if (NULL != p_serv->p_servfuncs->pfunc_int_pend) {
            inum -= p_serv->p_servinfo->inum_start;
            ret = p_serv->p_servfuncs->pfunc_int_pend(
                    p_serv->p_cookie,
                    inum);
        }
        else {
            ret = -AW_ENOTSUP;
        }
        __intctlr_put_serv(p_serv);
    }

    return  ret;
}

/* end of file */
