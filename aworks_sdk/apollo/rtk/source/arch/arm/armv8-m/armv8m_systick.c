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

    // ��ʱֹͣsystick,ֹͣsystick��Ϊ�˸��õؼ�����Ӧ�ñ��趨��ֵ
    // ���������ϵͳά��������ʱ�䣬ʹ��ticklessģʽ�᲻�ɱ���������������
    AW_REG_BIT_CLR32(&SYS_TICK->CTRL,0);
    old_cyccnt = readl(DWT_CYCCNT_REG);
    ul_ctl_src_int = readl(&SYS_TICK->CTRL);
    ul_ctl_src_int &= 0x6;

    // �����µ�reloadֵ���Ա�����expect_idle_tick������
    t = readl(&SYS_TICK->LOAD) + 1;
    old_load_value = t;
    reload_value = readl(&SYS_TICK->VAL) + 1;
    reload_value += (expect_idle_tick -1) * t;

    // �����жϱ�������
    old = rtk_enter_critical();
    // ��鵽�˴��Ƿ����жϵ���pending���������ʹ���жϣ��˳�tickless����
    if (rtk_is_task_scheduler_needed() ) {
        // �Ѿ����жϴ�����������ȣ���ʱsystick�Ѿ�ֹͣ����Ҫ���´�
        AW_REG_BIT_SET32(&SYS_TICK->CTRL,0);
        // ʹ���ж�
        rtk_exit_critical(old);
    }
    else {
        // ���systick�жϱ�־�������ü���ֵΪ0
        writel(0,&SYS_TICK->VAL);
        // �����µļ���ֵ
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
        // ����systick
        AW_REG_BIT_SET32(&SYS_TICK->CTRL,0);
        x2 = readl(DWT_CYCCNT_REG);

        x2 -= x1;
        // ˯��ֱ��������ʲô
        pm_cpu_idle();

        // ʹ���жϣ�ʹ��ʹCPU�˳�˯�ߵ��ж��ܹ�����������
        rtk_exit_critical(old);
        // �ٴν����жϣ���Ϊsystick��Ҫֹͣ�����κ���systickֹͣ�ڼ�������жϴ���������
        // ϵͳ����ʱ�������ʱ��֮������
        old = rtk_enter_critical();

        // ͨ������SYSTICK_CTRL�Ĵ�����ֹͣSYSTICK���Դﵽ�����SYSTICK�жϱ�־��Ŀ��
        writel(ul_ctl_src_int,&SYS_TICK->CTRL);
        old_cyccnt = readl(DWT_CYCCNT_REG);

        // ����systick�ж��Ƿ��Ѿ����������ȼ����������һ�β����ж�
        // �Ѿ����˶���tick
        ul_cal_new_load_value = (reload_value - SYS_TICK->VAL);
        ul_completed_ticks = 0;
        while (old_load_value <= ul_cal_new_load_value) {
            ul_completed_ticks ++;
            ul_cal_new_load_value -= old_load_value;
        }
        // �ó��µ�LOADֵ
        ul_cal_new_load_value = old_load_value - 1 - ul_cal_new_load_value;
        // ���������һ����С��ֵ
        if (ul_cal_new_load_value < __g_ul_systick_compensation_val) {
            ul_completed_ticks ++;
            ul_cal_new_load_value += old_load_value ;
        }

        t = readl(&SYS_TICK->CTRL);
        // �ж��Ƿ��Ѿ�������SYS_TICK�ж�
        if ( t & (1 << 16) ) {
            // �Ѿ�������SYS_TICK�ж�
            // ��������systick����Ҫtick
            // �˴�֮���Լ�һ������Ϊһ��systick�ж��Ѿ��ڱ��ν����ж�ǰ�����
            // �Ժ��˳��жϺ������������
            ul_completed_ticks += expect_idle_tick - 1;
        }
        else {
            // ��δ����systick�жϣ������жϻ�����CPU
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
        // ����systick,ʹ���LOAD value��ʼ����
        SYS_TICK->LOAD = ul_cal_new_load_value;
        AW_REG_BIT_SET32(&SYS_TICK->CTRL,0);

        // ����systick load
        writel(old_load_value -1 ,&SYS_TICK->LOAD);
        // ��������systick
        rtk_tick_down_counter_announce_with_ticks(ul_completed_ticks);
        // �뿪����ʹ���ж�
        rtk_exit_critical(old);
    }

}
#endif
