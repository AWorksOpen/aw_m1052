/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file riscv_exc.c
 * \brief riscv exception c functions
 *
 * \internal
 * \par History
 * \endinternal
 */

#include "rtk.h"
#include "aw_vdebug.h"

/* When an lsu error or illegal instruction error occur,
 * the interrupt will be disabled, and will not be enabled when
 * the exception handler return!
 */

#define kprintf aw_kprintf

static const char* get_cur_task_name()
{
    struct rtk_task* p_task;

    p_task = rtk_task_self();

    if (NULL != p_task) {
        return p_task->name;
    } else {
        return "no task";
    }
}

typedef struct {
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t mepc;
    uint32_t mstatus;
    uint32_t ra;
    uint32_t sp;
} fatal_error_context_t;

static void output_register_message(fatal_error_context_t* pContext) {
    kprintf("gp=0x%08x\n", pContext->gp);
    kprintf("tp=0x%08x\n", pContext->tp);
    kprintf("t0=0x%08x\n", pContext->t0);
    kprintf("t1=0x%08x\n", pContext->t1);
    kprintf("t2=0x%08x\n", pContext->t2);
    kprintf("a0=0x%08x\n", pContext->a0);
    kprintf("a1=0x%08x\n", pContext->a1);
    kprintf("a2=0x%08x\n", pContext->a2);
    kprintf("a3=0x%08x\n", pContext->a3);
    kprintf("a4=0x%08x\n", pContext->a4);
    kprintf("a5=0x%08x\n", pContext->a5);
    kprintf("a6=0x%08x\n", pContext->a6);
    kprintf("a7=0x%08x\n", pContext->a7);
    kprintf("sp=0x%08x\n", pContext->sp);
    kprintf("ra=0x%08x\n", pContext->ra);
    kprintf("mepc=0x%08x\n", pContext->mepc);
    kprintf("mstatus=0x%08x\n", pContext->mstatus);
}

/* lsu error can return */
void lsu_error(fatal_error_context_t *pContext)
{
    volatile int            not_exited = 1;
    kprintf("lsu error @ 0x%08X @%s\n",
            pContext->mepc, get_cur_task_name());
    output_register_message(pContext);
    while (not_exited);
}

/* illegal_instruction handler will never return */
void illegal_instruction(fatal_error_context_t *pContext)
{
    volatile int            not_exited = 1;
    kprintf("illegal instruction error @ 0x%08X @%s\n",
            pContext->mepc, get_cur_task_name());
    output_register_message(pContext);
    while (not_exited);
}

/* end of file */
