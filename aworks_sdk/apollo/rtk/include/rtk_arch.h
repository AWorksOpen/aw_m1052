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
 * \brief ����rtk���漰����arch��غ�������
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
 * \brief rtk��CPU�ܹ���ʼ��
 *
 * �˺�����rtk��ʼ��ʱ���ã���Ҫ������������һЩrtk�������еı�������
 * ����ؼ��жϵ����ȼ�������Э�����������ܻ���ã�fault��ʹ�ܻ���õ�
 * \return  ��
 */
void rtk_arch_init(void);
/**
 * \brief �����񻷾��´���һ�������л�
 * \return  ��
 */
void rtk_arch_context_switch(void);

/**
 * \brief ���жϻ����´���һ�������л�
 * \return  ��
 */
void rtk_arch_context_switch_interrupt(void);

/**
 * \brief ��ʼִ�е�һ������
 * rtk��ʼ���󣬻���ô˺�������ʼִ�е�һ������
 * �����������Ⱥ������˺���������Щ����Ĺ���������
 * ���Խ���ջ����Ϊ��ʼֵ����Ϊ�˺���֮����ջ�Ϳ���ֻ�����жϴ�����
 * ��ǰ�ľֲ�������û�������ˣ������ӿ���ʹ���жϴ���ջ����
 * \return  ��
 */
void rtk_arch_start_first_thread(void);

/**
 * \brief ���CPU��ǰ�Ƿ�����Ȩģʽ
 *
 * �������û�ģʽ������£��˺����ᱻ����
 * \retval  0   ��ǰ���ڷ���Ȩģʽ
 * \retval  ��0 ��ǰ������Ȩģʽ
 */
int rtk_arch_is_privileged_mode(void);
/**
 * \brief ִ��task_exit��syscall
 * �˺�����rtk�ڲ����ã���Ŀ�����ڴ����û�̬�����������غ�
 * ��Ϊ�������ں�̬��û���˳�������
 * �û�̬�����������غ󣬻��Զ����ô˺���
 * ���Լ��л����ں�̬���Ӷ�ʵ���˳�
 *
 * \param[in]   status ������˳���
 * \return  ��
 */
void rtk_arch_exec_task_exit_syscall(void *status);

/**
 * \brief ִ������ջ�ĳ�ʼ��
 *
 * �ڳ�ʼ������ʱ���ã��˺�������ҪĿ���ǳ�ʼ�������ջ��ʹ��Ŀ��������Ա��л���
 *
 * \param[in]   tentry  Ŀ���������ں���
 * \param[in]   parameter1 ��ں����ĵ�һ������
 * \param[in]   parameter2 ��ں����ĵڶ�������
 * \param[in]   stack_low �����ջ�ĵ͵�ַ
 * \param[in]   stack_high �����ջ����ߵ�ַ
 * \param[in]   texit ������ں������غ�Ҫִ�еĺ���
 * \param[in]   option ���񴴽�ʱ��һЩ��־ѡ��
 * \return  ��
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
 * \brief ���������е�һ��Ϊ1��λ
 * ����������ڲ�������Ϊ�˱�֤Ч�ʣ�����������ֵΪ0������ֵ��δ�����
 * �����ͷ���ֵ��ʾ����ϵΪ
 * 0x1:         1
 * 0x2:         2
 * 0x3:         1
 * 0x4:         3
 * 0x8:         4
 * 0x80000000   32
 * \return �ڼ�λΪ1
 */
int rtk_cpu_ffs(unsigned long q );


/**
 * \brief ��ȡ��ǰCPU���ڼ����������֧�֣�����һֱ����0
 * \return  CPU�Ѿ�ִ���˶�������
 */
unsigned long rtk_arch_read_cyccnt(void);


/**
 * \brief �ж�CPU�Ƿ��ڴ����жϵĻ�����
 *
 * \retval  0       ���Ǵ��ڴ����жϵĻ�����
 * \retval  ��0      CPU���ڴ����ж�
 */
int rtk_cpu_is_int_context(void);

/**
 * \brief �ж�CPU�Ƿ��ڴ����жϵĻ�����
 *
 * \retval  0       ���Ǵ��ڴ����жϵĻ�����
 * \retval  ��0      CPU���ڴ����ж�
 */
int rtk_cpu_is_fault_context(void);

/**
 * \brief RTK���ô˺��������������ж�״̬
 *
 * \retval  ��
 */
void rtk_cpu_enter_int_context(void);

/**
 * \brief RTK���ô˺��������뿪���ж�״̬
 *
 * \retval  ��
 */
void rtk_cpu_exit_int_context(void);

/**
 * \brief RTK���ô˺�������ԭ�ӼӲ���
 *
 * \retval  ��
 */
int rtk_cpu_atomic_add_return(int add_val, int *counter);

/**
 * \brief RTK���ô˺�������ԭ�Ӽ�����
 *
 * \retval  ��
 */
int rtk_cpu_atomic_sub_return(int sub_val, int *counter);

/**
 * \brief RTK���ô˺�������ԭ�ӱȽϲ���
 *
 * \retval  ��
 */
int rtk_cpu_atomic_cmpxchg(int old_val, int new_val, int *counter);

/*
 * ���Ҫʵ�ֿ���ʱ�����������ʱ��ͳ�ƣ����º�������ʵ�ֵ�
 * ������Щ����������BSP��ʵ��
 */
unsigned long rtk_cpu_tick_counter_get(void);
unsigned long rtk_cpu_tick_counter_diff(
        unsigned long old_counter,
        unsigned long new_counter);
unsigned long rtk_cpu_tick_counter_per_tick(void);

#endif          // __RTK_ARCH_H__
