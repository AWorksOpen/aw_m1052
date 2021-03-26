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
 * \brief ARMv8-M systick
 *
 * \internal
 * \par modification history:
 * - 1.10 2019-08-29, A.J, armv8-m systick.
 * - 1.00 12-11-08  zen, first implemetation
 * \endinternal
 */

#include "arch/arm/armv8-m/armv8m_mpu.h"
#include "string.h"
#include "aw_bitops.h"
#include "rtk.h"
#include "arch/arm/armv8-m/armv8m_systick.h"

uint32_t armv8m_systick_init(
    unsigned int systick_freq,
    unsigned int cpu_freq,
    int          tick_per_second,
    int          use_cpu_core_clk)
{
    uint32_t        data;
    uint32_t        counter_per_tick;

    writel(0, &SYS_TICK->CTRL);
    data = readl(&SYS_TICK->CTRL);
    counter_per_tick = systick_freq / tick_per_second;
    data = counter_per_tick - 1;
    writel(0, &SYS_TICK->CTRL);

    writel(data, &SYS_TICK->LOAD);
    writel(0, &SYS_TICK->VAL);
    data = 0x3;
    if (use_cpu_core_clk) {
        data |= (1<<2);
    }
    writel(data, &SYS_TICK->CTRL);

    return counter_per_tick;
}

uint32_t armv8m_systick_counter_get(void)
{
    return SYS_TICK->VAL;
}

#if 0
void armv8m_suppress_systick_and_sleep2(unsigned int expect_idle_tick)
{
    unsigned long           ul_ctl_src_int;
    unsigned long           old_load_value,reload_value,t;
    int                     old;
    unsigned long           ul_completed_ticks;
    uint32_t                ul_cal_new_load_value;
    uint32_t                old_cyccnt,now_cyccnt;
    uint32_t                x1,x2;

    if (expect_idle_tick > __g_ul_max_suppressed_ticks) {
        expect_idle_tick = __g_ul_max_suppressed_ticks;
    }
    g_supress_ticks_count ++;

    // 暂时停止systick,停止systick是为了更好地计算它应该被设定的值
    // 但是相对于系统维护的日历时间，使用tickless模式会不可避免地引入少量误差
    AW_REG_BIT_CLR32(&SYS_TICK->CTRL,0);
    old_cyccnt = readl(DWT_CYCCNT_REG);
    ul_ctl_src_int = readl(&SYS_TICK->CTRL);
    ul_ctl_src_int &= 0x6;

    // 计算新的reload值，以便满足expect_idle_tick的需求
    t = readl(&SYS_TICK->LOAD) + 1;
    old_load_value = t;
    reload_value = readl(&SYS_TICK->VAL) + 1;
    reload_value += (expect_idle_tick -1) * t;

    // 进入中断保护区域
    old = rtk_enter_critical();
    // 检查到此处是否有中断调度pending，如果有则使能中断，退出tickless过程
    if (rtk_is_task_scheduler_needed() ) {
        // 已经有中断触发了任务调度，此时systick已经停止，需要重新打开
        AW_REG_BIT_SET32(&SYS_TICK->CTRL,0);
        // 使能中断
        rtk_exit_critical(old);
    }
    else {
        // 清除systick中断标志，并设置计数值为0
        writel(0,&SYS_TICK->VAL);
        // 设置新的加载值
        reload_value --;
        now_cyccnt = readl(DWT_CYCCNT_REG);
        x1 = readl(DWT_CYCCNT_REG);

        t = now_cyccnt - old_cyccnt;
        if (now_cyccnt > old_cyccnt) {

        }
        else {
            t = 0 -t;
        }
        t += 811;
        reload_value -= t;
        writel(reload_value,&SYS_TICK->LOAD);
        // 重启systick
        AW_REG_BIT_SET32(&SYS_TICK->CTRL,0);
        x2 = readl(DWT_CYCCNT_REG);

        x2 -= x1;
        // 睡眠直到发生了什么
        pm_cpu_idle();

        // 使能中断，使得使CPU退出睡眠的中断能够立即被处理
        rtk_exit_critical(old);
        // 再次禁用中断，因为systick将要停止，而任何在systick停止期间产生的中断处理都将拉大
        // 系统保持时间和日历时间之间的误差
        old = rtk_enter_critical();

        // 通过不读SYSTICK_CTRL寄存器来停止SYSTICK，以达到不清除SYSTICK中断标志的目的
        writel(ul_ctl_src_int,&SYS_TICK->CTRL);
        old_cyccnt = readl(DWT_CYCCNT_REG);

        // 不管systick中断是否已经产生，都先计算出距离上一次产生中断
        // 已经过了多少tick
        ul_cal_new_load_value = (reload_value - SYS_TICK->VAL);
        ul_completed_ticks = 0;
        while (old_load_value <= ul_cal_new_load_value) {
            ul_completed_ticks ++;
            ul_cal_new_load_value -= old_load_value;
        }
        // 得出新的LOAD值
        ul_cal_new_load_value = old_load_value - 1 - ul_cal_new_load_value;
        // 不允许出现一个极小的值
        if (ul_cal_new_load_value < __g_ul_systick_compensation_val) {
            ul_completed_ticks ++;
            ul_cal_new_load_value += old_load_value ;
        }

        t = readl(&SYS_TICK->CTRL);
        // 判断是否已经产生了SYS_TICK中断
        if ( t & (1 << 16) ) {
            // 已经产生了SYS_TICK中断
            // 加上引发systick所需要tick
            // 此处之所以减一，是因为一次systick中断已经在本次禁用中断前处理或
            // 稍后退出中断后会立即被处理
            ul_completed_ticks += expect_idle_tick - 1;
        }
        else {
            // 尚未产生systick中断，其它中断唤醒了CPU
        }

        SYS_TICK->VAL = 0;
        now_cyccnt = readl(DWT_CYCCNT_REG);
        t = now_cyccnt - old_cyccnt;
        if (now_cyccnt > old_cyccnt) {

        }
        else {
            t = 0 -t;
        }
        t += 811;

        ul_cal_new_load_value -=t;
        // 重启systick,使其从LOAD value开始计数
        SYS_TICK->LOAD = ul_cal_new_load_value;
        AW_REG_BIT_SET32(&SYS_TICK->CTRL,0);

        // 重置systick load
        writel(old_load_value -1 ,&SYS_TICK->LOAD);
        // 处理多余的systick
        rtk_tick_down_counter_announce_with_ticks(ul_completed_ticks);
        // 离开，并使能中断
        rtk_exit_critical(old);
    }

}
#endif
