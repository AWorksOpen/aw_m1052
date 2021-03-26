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
 * - 1.10 13-02-28  zen, refine the description
 * - 1.00 12-09-14  orz, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_TASK_H
#define __AW_PSP_TASK_H

#include "rtk.h"                                /* 目前使用RTK内核 */
#include "aw_compiler.h"
#include "aw_types.h"

/******************************************************************************/
typedef struct rtk_task *aw_psp_task_id_t ;     /* 任务ID类型 */

/******************************************************************************/
#define AW_PSP_TASK_ID_INVALID  NULL            /* 无效任务ID */

#define __HIGH_THAN_USR_PRIORITY_COUNT \
    (rtk_get_realtime_priority_count() + rtk_get_sys_priority_count())

/******************************************************************************/


void rtk_stk_chk_shell_init (void);
void rtk_stk_chk_print_all (void);

typedef struct {
    struct rtk_task        *p_tcb;
    char                   *stack;
    unsigned int            __stack_size;
}aw_const rtk_psp_task_info_t ;


/******************************************************************************/
#define _AW_PSP_TASK_DECL_COMMON(task,stack_size) \
            struct { \
                struct rtk_task     tcb; \
                char                stack[stack_size]; \
            }task


#define AW_PSP_TASK_DECL(task, stack_size) \
            _AW_PSP_TASK_DECL_COMMON(task,stack_size) \



/******************************************************************************/
#define AW_PSP_TASK_DECL_STATIC(task, stack_size)  \
            static _AW_PSP_TASK_DECL_COMMON(task,stack_size) \

/******************************************************************************/
#define AW_PSP_TASK_INIT(task, name, priority, stack_size, func, arg1) \
            rtk_task_init(&(task).tcb, name, \
                    priority + __HIGH_THAN_USR_PRIORITY_COUNT, \
                    0, \
                    task.stack, \
                    task.stack + sizeof(task.stack), \
                    func, arg1,(void *)0)

/******************************************************************************/
#define AW_PSP_TASK_STARTUP(task)      aw_task_startup(&(task).tcb)

/******************************************************************************/
#define AW_PSP_TASK_TERMINATE(task)    aw_task_terminate( &(task).tcb )

/******************************************************************************/
#define AW_PSP_TASK_DELAY(tick)         aw_task_delay(tick)

/******************************************************************************/
#define AW_PSP_TASK_SUSPEND(task)       aw_task_suspend(&(task).tcb)

/******************************************************************************/
#define AW_PSP_TASK_RESUME(task)        aw_task_resume(&(task).tcb)

/******************************************************************************/
#define AW_PSP_TASK_VALID(task)         (aw_task_valid(&(task).tcb) )


/******************************************************************************/
#define AW_PSP_TASK_SAFE()              aw_task_safe()

/******************************************************************************/
#define AW_PSP_TASK_UNSAFE()            aw_task_unsafe()

/******************************************************************************/
#define AW_PSP_TASK_STACK_CHECK(task,p_total,p_free)   \
                                aw_task_stack_check(&(task).tcb,p_total,p_free)

/******************************************************************************/
#define AW_PSP_TASK_STACK_CHECK_SELF(p_total,p_free)   \
                           aw_task_stack_check(aw_task_id_self(),p_total,p_free)

/******************************************************************************/
#define AW_PSP_TASK_SYS_PRIORITY(priority) \
    (priority - rtk_get_sys_priority_count())

#define AW_TASK_REALTIME_PRIORITY(priority) \
            AW_PSP_TASK_REALTIME_PRIORITY(priority)

#define AW_TASK_SYS_LOW_PRIORITY(priority)  \
            AW_PSP_TASK_SYS_LOW_PRIORITY(priority)


static aw_inline int AW_PSP_TASK_REALTIME_PRIORITY(int priority)
{
    if (priority < 0 || priority >= rtk_get_realtime_priority_count() ) {
        while(1);
    }

    return priority - __HIGH_THAN_USR_PRIORITY_COUNT;
}

static aw_inline int AW_PSP_TASK_SYS_LOW_PRIORITY(int priority)
{
    if (priority < 0 || priority >= rtk_get_sys_low_priority_count() ) {
        while(1);
    }

    return priority + rtk_get_usr_priority_count();
}

#define AW_PSP_TASK_JOIN(task,p_status)  rtk_task_join((&(task).tcb),(p_status))

/******************************************************************************/

static aw_inline void kernel_init( void )
{
    rtk_init();
}

static aw_inline void os_startup( void )
{
    rtk_startup();
}

static aw_inline aw_err_t aw_task_startup (aw_psp_task_id_t task_id)
{
    return rtk_task_startup(task_id);
}

static aw_inline aw_err_t aw_task_delay (unsigned int ticks)
{
    return rtk_task_delay(ticks);
}

/******************************************************************************/
static aw_inline aw_bool_t aw_task_valid (aw_psp_task_id_t task_id)
{

    return rtk_task_check_exist(task_id)>0?AW_TRUE:AW_FALSE;
}

/******************************************************************************/
static aw_inline aw_err_t aw_task_priority_set(aw_psp_task_id_t task_id,
                                               unsigned int new_priority)
{
    if (0 > (int)new_priority) {
        return -AW_EINVAL;
    }
    return rtk_task_priority_set(task_id, new_priority + __HIGH_THAN_USR_PRIORITY_COUNT);
}

/******************************************************************************/
static aw_inline unsigned int aw_task_priority_count(void) {
    return rtk_get_usr_priority_count();
}

/******************************************************************************/
static aw_inline unsigned int aw_task_lowest_priority(void) {
    return rtk_get_usr_priority_count() - 1;
}

/******************************************************************************/
static aw_inline aw_psp_task_id_t aw_task_id_self(void) {
    return rtk_task_self();
}

/******************************************************************************/
static aw_inline int aw_task_yield(void) {
    return rtk_task_yield();
}

/******************************************************************************/
static aw_inline const char *aw_task_name_get (aw_psp_task_id_t task_id) {
    return task_id->name;
}

/******************************************************************************/
static aw_inline unsigned int aw_task_priority_get (aw_psp_task_id_t task_id) {
    return task_id->waitq_prio - __HIGH_THAN_USR_PRIORITY_COUNT;
}

/******************************************************************************/
static aw_inline int aw_task_stack_check (aw_psp_task_id_t task_id,
                                          unsigned int     *p_total,
                                          unsigned int     *p_free)
{
    return rtk_task_stack_check(task_id, p_total, p_free);
}

static aw_inline void *aw_task_set_tls(aw_psp_task_id_t task_id, void *data )
{
    return rtk_task_set_tls( task_id, data );
}

static aw_inline void *aw_task_get_tls(aw_psp_task_id_t task_id )
{
    return rtk_task_get_tls( task_id );
}

static aw_inline aw_err_t aw_task_terminate (aw_psp_task_id_t task_id)
{
    return rtk_task_terminate(task_id);
}

static aw_inline aw_err_t aw_task_join(aw_psp_task_id_t task_id,void **p_status)
{
    return rtk_task_join(task_id,p_status);
}

#endif      /* __AW_PSP_TASK_H */

/* end of file */
