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
 * \file  rtk_arch.h
 * \brief 定义rtk中涉及到的arch相关函数声明
 *
 * \internal
 * \par modification history:
 * - 0.9 18-04-23 sni first version.
 * \endinternal
 */


#ifndef             __RTK_ARCH_H__
#define             __RTK_ARCH_H__


uint32_t rtk_cpu_intr_disable(void);
void rtk_cpu_intr_enable(uint32_t old_status);

uint32_t rtk_cpu_enter_critical(void);
void rtk_cpu_exit_critical(uint32_t old_status);


/**
 * \brief rtk的CPU架构初始化
 *
 * 此函数在rtk初始化时调用，主要的作用是配置一些rtk正常运行的必须条件
 * 例如关键中断的优先级，浮点协处理器的是能或禁用，fault的使能或禁用等
 * \return  无
 */
void rtk_arch_init(void);
/**
 * \brief 在任务环境下触发一次任务切换
 * \return  无
 */
void rtk_arch_context_switch(void);

/**
 * \brief 在中断环境下触发一次任务切换
 * \return  无
 */
void rtk_arch_context_switch_interrupt(void);

/**
 * \brief 开始执行第一个任务
 * rtk初始化后，会调用此函数来开始执行第一个函数
 * 相对于任务调度函数，此函数可以做些额外的工作，比如
 * 可以将主栈设置为初始值，因为此函数之后主栈就可能只用于中断处理了
 * 此前的局部变量就没有意义了，这样子可以使得中断处理栈更大
 * \return  无
 */
void rtk_arch_start_first_thread(void);

/**
 * \brief 检查CPU当前是否处于特权模式
 *
 * 在是能用户模式的情况下，此函数会被调用
 * \retval  0   当前处于非特权模式
 * \retval  非0 当前处于特权模式
 */
int rtk_arch_is_privileged_mode(void);
/**
 * \brief 执行task_exit的syscall
 * 此函数在rtk内部调用，其目标在于处理用户态的任务函数返回后
 * 因为不处于内核态，没法退出的问题
 * 用户态的任务函数返回后，会自动调用此函数
 * 将自己切换到内核态，从而实现退出
 *
 * \param[in]   status 任务的退出码
 * \return  无
 */
void rtk_arch_exec_task_exit_syscall(void *status);

/**
 * \brief 执行任务栈的初始化
 *
 * 在初始化任务时调用，此函数的主要目标是初始化任务的栈，使得目标任务可以被切换到
 *
 * \param[in]   tentry  目标任务的入口函数
 * \param[in]   parameter1 入口函数的第一个参数
 * \param[in]   parameter2 入口函数的第二个参数
 * \param[in]   stack_low 任务堆栈的低地址
 * \param[in]   stack_high 任务堆栈的最高地址
 * \param[in]   texit 任务入口函数返回后要执行的函数
 * \param[in]   option 任务创建时的一些标志选项
 * \return  无
 */
unsigned char *rtk_arch_stack_init(
        void *tentry,
        void *parameter1,
        void *parameter2,
        char *stack_low,
        char *stack_high,
        void *texit,
        rtk_task_option_t option);

/**
 * \brief 查找整数中第一个为1的位
 * 这个函数是内部函数，为了保证效率，如果掺入参数值为0，则其值是未定义的
 * 整数和返回值的示例关系为
 * 0x1:         1
 * 0x2:         2
 * 0x3:         1
 * 0x4:         3
 * 0x8:         4
 * 0x80000000   32
 * \return 第几位为1
 */
int rtk_cpu_ffs(unsigned long q );


/**
 * \brief 读取当前CPU周期计数，如果不支持，可以一直返回0
 * \return  CPU已经执行了多少周期
 */
unsigned long rtk_arch_read_cyccnt(void);


/**
 * \brief 判断CPU是否处于处理中断的环境中
 *
 * \retval  0       不是处于处理中断的环境中
 * \retval  非0      CPU正在处理中断
 */
int rtk_cpu_is_int_context(void);

/**
 * \brief 判断CPU是否处于处理中断的环境中
 *
 * \retval  0       不是处于处理中断的环境中
 * \retval  非0      CPU正在处理中断
 */
int rtk_cpu_is_fault_context(void);

/**
 * \brief RTK调用此函数表明进入了中断状态
 *
 * \retval  无
 */
void rtk_cpu_enter_int_context(void);

/**
 * \brief RTK调用此函数表明离开了中断状态
 *
 * \retval  无
 */
void rtk_cpu_exit_int_context(void);

/**
 * \brief RTK调用此函数进行原子加操作
 *
 * \retval  无
 */
int rtk_cpu_atomic_add_return(int add_val, int *counter);

/**
 * \brief RTK调用此函数进行原子减操作
 *
 * \retval  无
 */
int rtk_cpu_atomic_sub_return(int sub_val, int *counter);

/**
 * \brief RTK调用此函数进行原子比较操作
 *
 * \retval  无
 */
int rtk_cpu_atomic_cmpxchg(int old_val, int new_val, int *counter);

/*
 * 如果要实现空闲时间和任务运行时间统计，以下函数必须实现的
 * 但是这些函数可以在BSP中实现
 */
unsigned long rtk_cpu_tick_counter_get(void);
unsigned long rtk_cpu_tick_counter_diff(
        unsigned long old_counter,
        unsigned long new_counter);
unsigned long rtk_cpu_tick_counter_per_tick(void);

#endif          // __RTK_ARCH_H__
