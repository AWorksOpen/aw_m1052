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
 * \file
 * \brief ����ϵͳ(OS)��ʱ��ʵ��
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-08-14  sni, first implementation
 * \endinternal
 */
#include "aw_exc.h"
#include "aw_int.h"
#include "aw_clk.h"
#include "aw_vdebug.h"
#ifdef CRP_ENABLE
#include "awbl_stack_overflow_check.h"
#endif
#include "arch/arm/armv7-m/armv7m_systick.h"

/* ȫ�ֱ��� */
unsigned long  g_aw_sys_ticks_per_second;    /* ϵͳʱ�ӽ���Ƶ�� */


void systick_isr (void *param)
{
    ENTER_INT_CONTEXT();
    rtk_tick_down_counter_announce();
    EXIT_INT_CONTEXT();
}

void awbl_stack_overflow_check_init(char *devicetype);
/**
 * \brief Systick ��ʼ��
 *
 * \param ticks_per_second  ϵͳʱ�ӽ���Ƶ��
 */
void systick_init (unsigned long ticks_per_second)
{
    uint32_t            rate;
    aw_exc_int_connect(EXCNUM_SYSTICK, systick_isr);

    rate = aw_clk_rate_get(IMX1020_CLK_AHB_CLK_ROOT);
    armv7m_systick_init(rate,rate,ticks_per_second,1);
    g_aw_sys_ticks_per_second = ticks_per_second;

#ifdef CRP_ENABLE
    if (awbl_stack_overflow_check() != 0) {
        volatile int    *p = NULL;
        volatile int    div0 = 0;
        *p = 0;
        aw_kprintf("the stack overflow check failed,%d\n",(*p)/div0);
    }

#endif


}

/**
 * \brief ��ȡϵͳʱ�ӽ���Ƶ��
 */
unsigned long aw_sys_clkrate_get (void)
{
    return g_aw_sys_ticks_per_second;
}

#if CONFIG_USE_TICKLESS_IDLE
void armv7m_suppress_systick_and_sleep(unsigned int expect_idle_tick);
void pm_suppress_systick_and_sleep(unsigned int expect_idle_tick)
{
    armv7m_suppress_systick_and_sleep(expect_idle_tick);
}
#endif
/* end of file */
