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
 * \file
 * \brief ARM 栈帧结构.
 *
 * \internal
 * \par History
 * - 1.00 14-09-23  orz, first implementation.
 * \endinternal
 */
#include "backtrace.h"

/*
 * Only GCC support now.
 */
#ifndef   __GNUC__
#error support gcc only
#endif

 
struct layout {
    void *next_frame;
    void *return_address;
};

#define FRAME2LAYOUT(frame) (struct layout *)((char *)frame - 4)


/* 在连接脚本中定义这两个位置, 用于限定当前任务的PC值  */
extern int aw_backtrace_text_start;
extern int aw_backtrace_text_end;

int task_backtrace(
        struct rtk_task *task,
        void           **array,
        int              max,
        void            *stack_frame_base)
{
    struct layout       *current = NULL;
    void                *frame = NULL;
    char                *stack_low = NULL;
    char                *stack_high = NULL;
    int                 cnt = 0;
    void                *code_low = &aw_backtrace_text_start;
    void                *code_high = &aw_backtrace_text_end;



    frame = (void *) stack_frame_base;
    stack_low = task->stack_low;
    stack_high = task->stack_high;
    current = FRAME2LAYOUT(frame);

    while (cnt < max)
    {
        void *return_address;
        if (NULL == current) {
            /* out of range */
            break;
        }
        /* check the stack is not out of range*/
        if ((((char *) current < stack_low)
                || ((char *) current > stack_high))) {
            break;
        }

        if ( (unsigned int)current & 0x01 ) {
            break;
        }

        return_address = current->return_address;
        array[cnt++] = return_address;
        /* check the return address is in the range of code section*/
        if ( return_address < code_low
                || return_address > code_high ) {
            break;
        }
        /*if next frame's address is NULL,we arrive the bottom of stack*/
        if(NULL == current->next_frame) {
            break;
        }
        current = FRAME2LAYOUT(current->next_frame);
    }

    return cnt;
}

int task_backtrace_cur_task(
        struct rtk_task *task,
        void **array,
        int max,
        void *stack_frame_base,
        void *lr
        )
{
    struct layout       *current = NULL;
    void                *frame = NULL;
    char                *stack_low = NULL;
    char                *stack_high = NULL;
    int                 cnt = 0;
    void                *code_low = &aw_backtrace_text_start;
    void                *code_high = &aw_backtrace_text_end;
    int                 first = 1;



    frame = (void *) stack_frame_base;
    stack_low = task->stack_low;
    stack_high = task->stack_high;
    current = FRAME2LAYOUT(frame);

    while (cnt < max)
    {
        void *return_address;
        if (NULL == current) {
            /* out of range */
            break;
        }
        /* check the stack is not out of range*/
        if ((((char *) current < stack_low)
                || ((char *) current > stack_high))) {
            break;
        }

        if ( (unsigned int)current & 0x01 ) {
            break;
        }

        return_address = current->return_address;
        if(first) {
            return_address = lr;
            first  = 0;
        }
        array[cnt++] = return_address;
        /* check the return address is in the range of code section*/
        if ( return_address < code_low
                || return_address > code_high ) {
            break;
        }
        /*if next frame's address is NULL,we arrive the bottom of stack*/
        if(NULL == current->next_frame) {
            break;
        }
        current = FRAME2LAYOUT(current->next_frame);
    }

    return cnt;
}
