/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https:/*www.zlg.cn
*******************************************************************************/

/**
 * \file armv4_exc.c
 * \brief 
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-29  or2, first implementation
 * \endinternal
 */

#include "rtk.h"
#include "aw_vdebug.h"
#include "aw_bitops.h"
#include "backtrace.h"

#define kprintf aw_kprintf
static const char * get_cur_task_name()
{
    struct rtk_task         *p_task;
    p_task = rtk_task_self();

    if (NULL != p_task) {
        return p_task->name;
    }
    else {
        return "no task";
    }
}

typedef struct {
    uint32_t        R0;         /*异常发生前的R0*/
    uint32_t        R1;         /*异常发生前的R1*/
    uint32_t        R2;         /*异常发生前的R2*/
    uint32_t        R3;         /*异常发生前的R3*/
    uint32_t        R4;         /*异常发生前的R4*/
    uint32_t        R5;         /*异常发生前的R5*/
    uint32_t        R6;         /*异常发生前的R6*/
    uint32_t        R7;         /*异常发生前的R7*/
    uint32_t        R8;         /*异常发生前的R8*/
    uint32_t        R9;         /*异常发生前的R9*/
    uint32_t        R10;        /*异常发生前的R10*/
    uint32_t        R11;        /*异常发生前的R11*/
    uint32_t        R12;        /*异常发生前的R12*/
    uint32_t        SP;         /*异常发生前的SP*/
    uint32_t        LR;         /*异常发生前的LR*/
    uint32_t        LR_FATAL;   /*致命错误异常的返回地址*/
    uint32_t        CPSR;
    uint32_t        SPSR;
    uint32_t        fault_status;
    uint32_t        fault_address;
}fatal_errror_context_t;

fatal_errror_context_t      __g_fatal_err_context;

/* store return address */
static void *__backtrace_buffer[32];

void task_for_each(
        int ( *func )( void *, struct rtk_task * ),
        void *arg
        );
int task_stack_check (
        struct rtk_task *p_task,
        unsigned int    *p_total,
        unsigned int    *p_free
        );
#if 0
/* from rtk porting */
extern int rtk_get_frame_regs( void );

int __task_backtrace( void *arg, struct rtk_task *task )
{
    int             count=0;
    int             i;
    unsigned int    stack_total = 0;
    unsigned int    stack_free = 0;
    void            *frame_base = 0;

    int regs = rtk_get_frame_regs();

    if ( task == task_self() ) {
        /*we don't process current task*/
        return 0;
    }
    else {
        kprintf("{task %d:%s:", task->tid, task->name );
    }
    __backtrace_buffer[0] = ((void**)task->sp)[regs-1];
    frame_base = ((void**)task->sp)[regs-4];

    count = task_backtrace(
            task,
            &__backtrace_buffer[1],
            sizeof(__backtrace_buffer)/sizeof(__backtrace_buffer[0]) -1,
            frame_base
            );
    for (i=count; i>=0; i-- ) {
        kprintf("0x%X>", (char*)__backtrace_buffer[i] - 4 );
    }


    task_stack_check( task, &stack_total, &stack_free );
    kprintf(",priority=%d/%d,stack=(%uB/%uB %u)"
#if CONFIG_NEWLIB_REENT_EN
            ",errno=%u"
#endif
            "}\n",
            task->priority, task->current_priority,
            stack_total, stack_free, ((stack_free*100)/stack_total)
#if CONFIG_NEWLIB_REENT_EN
            ,task->newlib_reent._errno
#endif
            );

    return 0;
}


static void backtrace_cur_tasks_data_abort(uint32_t abort_lr,void *frame_base,void *lr)
{
    int             count=0;
    int             i;
    unsigned int    stack_total = 0;
    unsigned int    stack_free = 0;
    struct rtk_task *task = task_self();

    if (NULL == task) {
        return;
    }

    kprintf("{task %d:%s<focus>:", task->tid, task->name );

    __backtrace_buffer[0] = (void*)(abort_lr);
    count = task_backtrace_cur_task(
            task,
            &__backtrace_buffer[1],
            sizeof(__backtrace_buffer)/sizeof(__backtrace_buffer[0]) -1,
            frame_base,
            lr);

    for (i=count; i>0; i-- ) {
        kprintf("0x%X>", (char*)__backtrace_buffer[i] - 4 );
    }
    kprintf("0x%X", (char*)__backtrace_buffer[0] - 4 );


    task_stack_check( task, &stack_total, &stack_free );
    kprintf(",priority=%d/%d,stack=(%uB/%uB %u)"
#if CONFIG_NEWLIB_REENT_EN
            ",errno=%u"
#endif
            "}\n",
            task->priority, task->current_priority,
            stack_total, stack_free, ((stack_free*100)/stack_total)
#if CONFIG_NEWLIB_REENT_EN
            ,task->newlib_reent._errno
#endif
            );
}

void backtrace_all_other_tasks(void)
{
    kprintf("rtk task backtrace begin:\n");
    task_for_each(__task_backtrace,  0 );
    kprintf("rtk task backtrace end\n");
}

#endif

void undefined_instruction(fatal_errror_context_t *pContext)
{
    volatile int            not_exited = 1;
    kprintf("undefined instruction @ 0x%08X @%s\n",
            pContext->LR_FATAL-4, get_cur_task_name() );

    /*kprintf("The accessing address is 0x%08x\n", pContext->fault_address );*/
    /*show_data_abort_status(pContext->fault_status);*/
    kprintf("R0=0x%08x\n",pContext->R0);
    kprintf("R1=0x%08x\n",pContext->R1);
    kprintf("R2=0x%08x\n",pContext->R2);
    kprintf("R3=0x%08x\n",pContext->R3);
    kprintf("R4=0x%08x\n",pContext->R4);
    kprintf("R5=0x%08x\n",pContext->R5);
    kprintf("R6=0x%08x\n",pContext->R6);
    kprintf("R7=0x%08x\n",pContext->R7);
    kprintf("R8=0x%08x\n",pContext->R8);
    kprintf("R9=0x%08x\n",pContext->R9);
    kprintf("R10=0x%08x\n",pContext->R10);
    kprintf("R11=0x%08x\n",pContext->R11);
    kprintf("R12=0x%08x\n",pContext->R12);
    kprintf("SP=0x%08x\n",pContext->SP);
    kprintf("LR=0x%08x\n",pContext->LR);
    kprintf("LR_FATAL=0x%08x\n",pContext->LR_FATAL);
    kprintf("CPSR=0x%08x\n",pContext->CPSR);
    kprintf("SPSR=0x%08x\n",pContext->SPSR);

    kprintf("call func:\n",pContext->SPSR);
    /*backtrace_cur_tasks_data_abort((pContext->LR_FATAL),
                                     (void *)pContext->R11,
                                     (void *)pContext->LR);*/
    kprintf("no call func:\n",pContext->SPSR);
    /*backtrace_cur_tasks_data_abort((pContext->LR_FATAL),
                                      (void *)pContext->R11+4,
                                      (void *)pContext->LR);*/
    /*backtrace_all_other_tasks();*/

    while (not_exited);
}

void undefined_instruction_intr_vfp(void)
{
    kprintf("You cannot use float in the interrupt process\n\n");
}

void software_interrupt(fatal_errror_context_t *pContext )
{
    kprintf("software interrupt @ 0x%08X @%s\n",
            pContext->LR_FATAL-4, get_cur_task_name() );

    /*kprintf("The accessing address is 0x%08x\n", pContext->fault_address );*/
    /*show_data_abort_status(pContext->fault_status);*/
    kprintf("R0=0x%08x\n",pContext->R0);
    kprintf("R1=0x%08x\n",pContext->R1);
    kprintf("R2=0x%08x\n",pContext->R2);
    kprintf("R3=0x%08x\n",pContext->R3);
    kprintf("R4=0x%08x\n",pContext->R4);
    kprintf("R5=0x%08x\n",pContext->R5);
    kprintf("R6=0x%08x\n",pContext->R6);
    kprintf("R7=0x%08x\n",pContext->R7);
    kprintf("R8=0x%08x\n",pContext->R8);
    kprintf("R9=0x%08x\n",pContext->R9);
    kprintf("R10=0x%08x\n",pContext->R10);
    kprintf("R11=0x%08x\n",pContext->R11);
    kprintf("R12=0x%08x\n",pContext->R12);
    kprintf("SP=0x%08x\n",pContext->SP);
    kprintf("LR=0x%08x\n",pContext->LR);
    kprintf("LR_FATAL=0x%08x\n",pContext->LR_FATAL);
    kprintf("CPSR=0x%08x\n",pContext->CPSR);
    kprintf("SPSR=0x%08x\n",pContext->SPSR);
    while (1);
}

void prefetch_abort( fatal_errror_context_t *pContext )
{
    volatile int        not_exit = 1;
    kprintf("prefetch abort @ 0x%08X @%s\n",
            pContext->LR_FATAL-4, get_cur_task_name() );
    kprintf("The accessing address is 0x%08x\n", pContext->fault_address );
    /*show_data_abort_status(pContext->fault_status);*/
    kprintf("R0=0x%08x\n",pContext->R0);
    kprintf("R1=0x%08x\n",pContext->R1);
    kprintf("R2=0x%08x\n",pContext->R2);
    kprintf("R3=0x%08x\n",pContext->R3);
    kprintf("R4=0x%08x\n",pContext->R4);
    kprintf("R5=0x%08x\n",pContext->R5);
    kprintf("R6=0x%08x\n",pContext->R6);
    kprintf("R7=0x%08x\n",pContext->R7);
    kprintf("R8=0x%08x\n",pContext->R8);
    kprintf("R9=0x%08x\n",pContext->R9);
    kprintf("R10=0x%08x\n",pContext->R10);
    kprintf("R11=0x%08x\n",pContext->R11);
    kprintf("R12=0x%08x\n",pContext->R12);
    kprintf("SP=0x%08x\n",pContext->SP);
    kprintf("LR=0x%08x\n",pContext->LR);
    kprintf("LR_FATAL=0x%08x\n",pContext->LR_FATAL);
    kprintf("CPSR=0x%08x\n",pContext->CPSR);
    kprintf("SPSR=0x%08x\n",pContext->SPSR);



    kprintf("call func:\n",pContext->SPSR);
    /*backtrace_cur_tasks_data_abort((pContext->LR_FATAL),
                                    (void *)pContext->R11,
                                    (void *)pContext->LR);*/
    kprintf("no call func:\n",pContext->SPSR);
    /*backtrace_cur_tasks_data_abort((pContext->LR_FATAL),
                                    (void *)pContext->R11+4,
                                    (void *)pContext->LR);*/
    /*backtrace_all_other_tasks();*/
    while (not_exit);
}

static const char *g_data_abort_status[16] = {
        "Reserved",
        "Alignment fault.",
        "Reserved",
        "Alignment fault.",

        "Reserved",
        "Translation fault, section.",
        "Reserved.",
        "Translation fault, page.",

        "External abort,section",
        "Domain fault, section.",
        "External abort,page",
        "Domain fault, page.",

        "Synchronous external abort on translation table walk, 1st level.",
        "Permission fault, section.",
        "Synchronous external abort on translation table walk, 2nd level.",
        "Permission fault, page."
};
static void show_data_abort_status(int status)
{
    int         s = AW_BITS_GET(status,0,4);
    int         domain;


    kprintf("The abort status is 0x%08x\n", status );
    domain = AW_BITS_GET(status,4,4);
    kprintf("Abort domain %d.\n",domain);

    kprintf("Type of abort is:");
    kprintf(g_data_abort_status[s]);
    kprintf("\n");

}

void data_abort(fatal_errror_context_t *pContext)
{
    kprintf("data abort @ 0x%08X @%s\n",
            pContext->LR_FATAL-8, get_cur_task_name() );
    kprintf("The accessing address is 0x%08x\n", pContext->fault_address );
    show_data_abort_status(pContext->fault_status);
    kprintf("R0=0x%08x\n",pContext->R0);
    kprintf("R1=0x%08x\n",pContext->R1);
    kprintf("R2=0x%08x\n",pContext->R2);
    kprintf("R3=0x%08x\n",pContext->R3);
    kprintf("R4=0x%08x\n",pContext->R4);
    kprintf("R5=0x%08x\n",pContext->R5);
    kprintf("R6=0x%08x\n",pContext->R6);
    kprintf("R7=0x%08x\n",pContext->R7);
    kprintf("R8=0x%08x\n",pContext->R8);
    kprintf("R9=0x%08x\n",pContext->R9);
    kprintf("R10=0x%08x\n",pContext->R10);
    kprintf("R11=0x%08x\n",pContext->R11);
    kprintf("R12=0x%08x\n",pContext->R12);
    kprintf("SP=0x%08x\n",pContext->SP);
    kprintf("LR=0x%08x\n",pContext->LR);
    kprintf("LR_FATAL=0x%08x\n",pContext->LR_FATAL);
    kprintf("CPSR=0x%08x\n",pContext->CPSR);
    kprintf("SPSR=0x%08x\n",pContext->SPSR);

    kprintf("call func:\n");
    /*backtrace_cur_tasks_data_abort((pContext->LR_FATAL-4),
                                     (void *)pContext->R11,
                                     (void *)pContext->LR);*/
    kprintf("no call func:\n");
    /*backtrace_cur_tasks_data_abort((pContext->LR_FATAL-4),
                                     (void *)pContext->R11+4,
                                     (void *)pContext->LR);*/
    /*backtrace_all_other_tasks();*/
}


void fiq_handle (fatal_errror_context_t *pContext)
{
    kprintf("fiq interrupt @ 0x%08X @%s\n",
            pContext->LR_FATAL-4, get_cur_task_name() );

    /*kprintf("The accessing address is 0x%08x\n", pContext->fault_address );*/
    /*show_data_abort_status(pContext->fault_status);*/
    kprintf("R0=0x%08x\n",pContext->R0);
    kprintf("R1=0x%08x\n",pContext->R1);
    kprintf("R2=0x%08x\n",pContext->R2);
    kprintf("R3=0x%08x\n",pContext->R3);
    kprintf("R4=0x%08x\n",pContext->R4);
    kprintf("R5=0x%08x\n",pContext->R5);
    kprintf("R6=0x%08x\n",pContext->R6);
    kprintf("R7=0x%08x\n",pContext->R7);
    kprintf("R8=0x%08x\n",pContext->R8);
    kprintf("R9=0x%08x\n",pContext->R9);
    kprintf("R10=0x%08x\n",pContext->R10);
    kprintf("R11=0x%08x\n",pContext->R11);
    kprintf("R12=0x%08x\n",pContext->R12);
    kprintf("SP=0x%08x\n",pContext->SP);
    kprintf("LR=0x%08x\n",pContext->LR);
    kprintf("LR_FATAL=0x%08x\n",pContext->LR_FATAL);
    kprintf("CPSR=0x%08x\n",pContext->CPSR);
    kprintf("SPSR=0x%08x\n",pContext->SPSR);
    while (1);
}

