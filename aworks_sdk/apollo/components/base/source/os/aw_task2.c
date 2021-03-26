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
 * \brief aw_task2实现，(OO)
 *
 * \internal
 * \par modification history:
 * - 1.00 15-11-20, dcf, first implementation.
 * \endinternal
 */
#include <string.h>
#include "aw_task2.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "aw_vdebug.h"


struct aw_task2 {
    aw_task_id_t task_id;
    struct aw_isr_defer_job defer;
    AW_SEMB_DECL(join);
    int is_joinable;/* default true */
    struct aw_task2_cleaner *p_cleaner;
    void *exit_value;
    void *(*__entry)(void *arg);
    void *arg;
    void *lts;
};

/**
 * \brief static default.
 */
void aw_task2_cleaner_init(struct aw_task2_cleaner *p_this)
{
    p_this->next = 0;
    p_this->is_this_dynamic = 0;
    p_this->pfunc = 0;
}

void aw_task2_cleaner_destroy(struct aw_task2_cleaner *p_this)
{
    (void) p_this;
}

/**
 * \brief 如果任务未停止，先删除任务
 */
static void aw_task2_cleanup(struct aw_task2 *p_this)
{
    struct aw_task2_cleaner *p_cleaner = NULL;

    if ( (p_this->task_id != AW_TASK_ID_INVALID)
            && (p_this != aw_task2_self() )) {
        aw_task_id_t id = p_this->task_id;
        p_this->task_id = AW_TASK_ID_INVALID;
        aw_task_delete(id);
    }

    AW_SEMB_TERMINATE( p_this->join );

    p_cleaner = p_this->p_cleaner;
    p_this->p_cleaner = 0;
    while (p_cleaner) {
        struct aw_task2_cleaner *p_cleaner_prev;
        if (p_cleaner->pfunc) {
            (*p_cleaner->pfunc)(p_cleaner->arg);
        }
        p_cleaner_prev = p_cleaner;
        p_cleaner = p_cleaner->next;
        if (p_cleaner_prev->is_this_dynamic) {
            aw_mem_free(p_cleaner_prev);
        }
    }
    if ( (p_this != aw_task2_self() ) ) {
        aw_mem_free(p_this);
    }
}

static void __task_exit(struct aw_task2 *p_this, void *exit_value)
{
    AW_LOGF(("task2 %s exit(%d)\r\n",
            aw_task_name_get(aw_task2_to_task_id(p_this)), (int) exit_value));
    p_this->exit_value = exit_value;
    if (!p_this->is_joinable) {
        aw_isr_defer_job_add(&p_this->defer);
    } else {
        AW_SEMB_GIVE(p_this->join);
    }
}

static void __task_ex_entry(struct aw_task2 *p_this)
{
    __task_exit(p_this, p_this->__entry(p_this->arg));
    while (1) {
        aw_mdelay( 1000 );      /* wait for delete */
    }
}

struct aw_task2 *aw_task2_create(const char *name, int stack_size,
        int priority, void (*pfun)(void*), void *arg)
{
    struct aw_task2 *p_this;

    if ((!pfun)){
        return NULL;
    }
    p_this = aw_mem_alloc(sizeof(*p_this));
    if (!p_this) {
        return NULL;
    }

    memset(p_this, 0x00, sizeof(*p_this));
    p_this->p_cleaner = 0;
    p_this->__entry = pfun;
    p_this->arg = arg;
    AW_SEMB_INIT(p_this->join, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
    aw_isr_defer_job_init(&p_this->defer, (aw_pfuncvoid_t) aw_task2_cleanup, p_this);
    aw_task2_joinable_set(p_this, 1);
    p_this->task_id = aw_task_create(name, priority, stack_size,
            (aw_pfuncvoid_t) __task_ex_entry, p_this);
    if ( AW_TASK_ID_INVALID == p_this->task_id) {
        return NULL;
    }

    aw_task_set_tls(p_this->task_id, p_this);
    return p_this;
}

aw_task_id_t aw_task2_to_task_id(struct aw_task2 *p_this) {
    return p_this->task_id;
}

/**
 * \brief add only one cleaner( next point force to NULL )
 */
void aw_task2_add_cleaner(struct aw_task2 *p_this,
        struct aw_task2_cleaner *p_new_cleaner) {
    struct aw_task2_cleaner *p_cleaner = p_this->p_cleaner;

    p_new_cleaner->next = NULL;/* NOTICE: support only one to keep safe */

    while (p_cleaner && p_cleaner->next) {
        p_cleaner = p_cleaner->next;
    }
    if (p_cleaner) {
        p_cleaner->next = p_new_cleaner;
    }
    if (p_this->p_cleaner == NULL) {
        p_this->p_cleaner = p_new_cleaner;
    }
}

/**
 * \brief dynamic allocate cleanner
 */
aw_err_t aw_task2_add_clean_method(struct aw_task2 *p_this, void (*pfunc)(void*),
        void *arg) {
    struct aw_task2_cleaner *p_cleaner = aw_mem_alloc(sizeof(*p_cleaner));

    if (!p_cleaner) {
        return AW_ERROR;
    }

    aw_task2_cleaner_init( p_cleaner );

    p_cleaner->is_this_dynamic = 1;
    p_cleaner->next = 0;
    p_cleaner->pfunc = pfunc;
    p_cleaner->arg = arg;

    aw_task2_add_cleaner(p_this, p_cleaner);

    return AW_OK;
}

/**
 * \brief set joinable true or false
 */
void aw_task2_joinable_set(struct aw_task2 *p_this,
        int joinable_true_or_false) {
    p_this->is_joinable = joinable_true_or_false;
}

/**
 * \brief get joinable true or false
 */
int aw_task2_joinable_get(struct aw_task2 *p_this) {
    return p_this->is_joinable;
}

void aw_task2_startup(struct aw_task2 *p_this) {
    aw_task_startup(p_this->task_id);
}

int aw_task2_join(struct aw_task2 *p_this, void **pp_ret) {
    aw_err_t err = AW_OK;

    if (!p_this) {
        return -AW_EINVAL;
    }

    if (!p_this->is_joinable || (p_this == aw_task2_self())) {
        return -AW_EINVAL;
    }

    err = AW_SEMB_TAKE(p_this->join, AW_SEM_WAIT_FOREVER);
    if ((err == AW_OK) && pp_ret) {
        *pp_ret = p_this->exit_value;
    }

    aw_task2_cleanup( p_this );

    return err;
}

aw_err_t aw_task2_detach(struct aw_task2 *p_this) {
    if (p_this->is_joinable) {
        return AW_ERROR;
    }

    aw_task2_joinable_set( p_this, 0 );

    return AW_OK;
}

struct aw_task2 * aw_task2_self( ) {
    aw_task_id_t self = aw_task_id_self();
    struct aw_task2 *p_this;

    p_this = aw_task_get_tls( self );

    return p_this;
}

aw_err_t aw_task2_exit( void *ret ) {
    struct aw_task2 *p_this = aw_task2_self();

    if ( p_this ) {
        __task_exit( p_this, ret );
    }
    while (1) {
        aw_mdelay( 10000 );
    }
}

void *aw_task2_set_lts( struct aw_task2 *p_this, void *newlts ) {
    void *old;

    if ( !p_this ) {
        return NULL;
    }
    old = p_this->lts;
    p_this->lts = newlts;
    return old;
}

void *aw_task2_get_lts( struct aw_task2 *p_this ) {
    if ( !p_this ) {
        return NULL;
    }
    return p_this->lts;
}

/* end of file */
