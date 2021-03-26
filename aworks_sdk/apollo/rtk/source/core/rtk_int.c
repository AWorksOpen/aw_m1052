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
 * \file  rtk_systick.c
 * \brief rtk 整体初始化相关函数
 *
 * \internal
 * \par modification history:
 * 1.00 18-04-24  sni, first implement
 * \endinternal
 */
#include "rtk.h"
#include "rtk_mem.h"
#include "rtk_arch.h"
#include "rtk_task_internal.h"
#include "rtk_priority_q_internal.h"
#include "aw_errno.h"
#include "aw_common.h"
#include <string.h>
#include "os_trace_events.h"

#if CONFIG_RTK_ISR_HOOK

static pfn_rtk_hook_t volatile      _g_hook_enter_isr;
static pfn_rtk_hook_t volatile      _g_hook_exit_isr;

void rtk_isr_enter_hook_set(pfn_rtk_hook_t hook_fn)
{
    _g_hook_enter_isr = hook_fn;
}

void rtk_isr_exit_hook_set(pfn_rtk_hook_t hook_fn)
{
    _g_hook_exit_isr = hook_fn;
}

static void _rtk_isr_enter_hook_exec(void)
{
    pfn_rtk_hook_t      fn = _g_hook_enter_isr;
    if (NULL != fn) {
        fn ();
    }
}

static void _rtk_isr_exit_hook_exec(void)
{
    pfn_rtk_hook_t      fn = _g_hook_exit_isr;
    if (NULL != fn) {
        fn ();
    }
}

#endif


void __rtk_enter_int_context( void )
{
#if CONFIG_RTK_ISR_HOOK
    _rtk_isr_enter_hook_exec();
#endif
    rtk_cpu_enter_int_context();
}

void __rtk_exit_int_context( void )
{
    rtk_cpu_exit_int_context();
#if CONFIG_RTK_ISR_HOOK
    _rtk_isr_exit_hook_exec();
#endif
}

int rtk_is_int_context(void)
{
    return rtk_cpu_is_int_context();
}

int rtk_is_fault_context(void)
{
    return rtk_cpu_is_fault_context();
}

#ifdef CONFIG_RTK_CRITICAL_HOOK
static pfn_rtk_hook_t   __fn_enter_critical_hook = NULL;

void rtk_enter_critical_hook_set(pfn_rtk_hook_t hook_fn)
{
    __fn_enter_critical_hook = hook_fn;
}

#endif


uint32_t __rtk_enter_critical(void)
{
    uint32_t        cri_status;

    cri_status = rtk_cpu_enter_critical();

#if CONFIG_RTK_CRITICAL_HOOK
    if (NULL != __fn_enter_critical_hook) {
        __fn_enter_critical_hook();
    }
#endif

    return (uint32_t)cri_status;
}


void __rtk_exit_critical(uint32_t old)
{
    rtk_cpu_exit_critical(old);
}

uint32_t __rtk_interrupt_disable(void)
{
    return rtk_cpu_intr_disable();
}

void __rtk_interrupt_enable(uint32_t old)
{
    rtk_cpu_intr_enable(old);
}
