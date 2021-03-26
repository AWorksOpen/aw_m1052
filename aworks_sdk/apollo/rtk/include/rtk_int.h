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
 * \file  rtk_int.h
 * \brief rtk�����жϵĽṹ�ͺ���
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_INT_H__
#define         __RTK_INT_H__

#include "rtk_barrier.h"

/**
 * \brief �жϵ�ǰ�Ƿ�������isr������
 *
 * \retval    1             ��ǰ������isr������
 * \retval    0             ��ǰ����������isr������
 */
int rtk_is_int_context(void);

/**
 * \brief �жϵ�ǰ�Ƿ�������fault������
 *
 * \retval    1             ��ǰ������fault������
 * \retval    0             ��ǰ����������fault������
 */
int rtk_is_fault_context(void);

#define RTK_INT_STATUS_DECL(save_int_status)    uint32_t save_int_status;

/**
 * \brief ���õ�ǰCPU�ж�
 *
 * \return ���ؽ����ж�ǰ��CPU�жϱ�־
 */
uint32_t __rtk_interrupt_disable(void);

#define RTK_INTERRUPT_DISABLE(save_int_status)    \
    rtk_barrier(); (save_int_status) = __rtk_interrupt_disable(); rtk_barrier();

/**
 * \brief ���õ�ǰCPU�ж�
 *
 * \param   old     �ϴ�__rtk_interrupt_disable�ķ���ֵ
 */
void __rtk_interrupt_enable(uint32_t old);

#define RTK_INTERRUPT_ENABLE(old)    \
    rtk_barrier();__rtk_interrupt_enable((old)); rtk_barrier();


#define RTK_CRITICAL_STATUS_DECL(old) uint32_t  old

/**
 * \brief �������ϵͳ�ٽ���
 *
 * ���൱���CPU�ϣ��������ϵͳ�ٽ�����ͬ�ڽ���CPU�ж�
 * ���ǣ�����ܲ�����ȫ��ȷ��ĳЩ����½������ϵͳ�ٽ���������ζ��
 * ĳ�����ȼ�֮�µ��жϱ���ִֹ�У�֮�����ȼ����ж���Ȼ���Լ���ִ��
 * ��Ȼ��Щ�жϵ�isr���Բ��ܵ����κβ���ϵͳAPI���������ر��������Ե���
 *
 * \return �������ϵͳ�ٽ���֮ǰ���ٽ���״̬
 */
uint32_t  __rtk_enter_critical( void );
#define RTK_ENTER_CRITICAL(save_status)    \
    rtk_barrier();(save_status) = __rtk_enter_critical(); rtk_barrier();

/**
 * \brief �뿪����ϵͳ�ٽ���
 *
 * \param   old     ǰ��rtk_enter_critical�ķ���ֵ
 */
void __rtk_exit_critical(uint32_t old_status );
#define RTK_EXIT_CRITICAL(old)    \
    rtk_barrier();__rtk_exit_critical(old); rtk_barrier();




typedef void (*pfn_rtk_hook_t)(void);
void rtk_enter_critical_hook_set(pfn_rtk_hook_t hook_fn);
void rtk_exit_critical_hook_set(pfn_rtk_hook_t hook_fn);

void rtk_sched_begin_hook_set(pfn_rtk_hook_t hook_fn);
void _rtk_sched_begin_hook_exec(void);
void rtk_sched_end_hook_set(pfn_rtk_hook_t hook_fn);
void _rtk_sched_end_hook_exec(void);

void rtk_isr_enter_hook_set(pfn_rtk_hook_t hook_fn);
void rtk_isr_exit_hook_set(pfn_rtk_hook_t hook_fn);

/**
 * \brief �ս���ISRʱ���ã�������Ҫ���һЩISR�������ܣ�
 *        ���籣֤���ж������ĵ��жϣ���¼isr�Ŀ�ʼִ��ʱ�̣�����׷��hook������
 */
void __rtk_enter_int_context( void );

/**
 * \brief �˳�ISRǮ���ã�������Ҫ���һЩISR�ĸ�������
 *          ���籣֤���ж������ĵ��жϣ���¼isr���˳�ʱ�̣�����׷��hook������
 */
void __rtk_exit_int_context( void );


/**
 * \brief �ڸս���isr�е���RTK_ENTER_INT_CONTEXT��
 *        ������rtk������ǰ�������ж�������
 *
 * ͨ������£�rtk�ڲ���ͨ��rtk_is_int_context�������жϵ�ǰ�Ƿ�ִ�����ж�������
 * ��rtk_is_int_context�ڲ���ֱ�Ӷ�ȡCPU״̬�Ĵ���������жϣ�����RTK_ENTER_INT_CONTEXT
 * ����Ҫִ���κζ�����Ϊ���Ч�ʣ����䶨��Ϊ�պ�
 *
 * Ȼ��ĳЩCPU�����޷�ͨ����ȡCPU״̬�Ĵ������ж��ж�״̬������RTK_ENTER_INT_CONTEXT
 * �ڲ�ʵ�־Ϳ���ͨ������ĳ��������rtk_is_int_context�Ϳ���ͨ������������ж��Ƿ�
 * �ж�������
 *
 * ����һ����ĳЩʱ�����ǿ�����Ҫͳ��ÿ��ISR��ִ��ʱ�䣬����ͨ��ĳ���ֶ���׷��ϵ
 * ͳִ��·����ʱRTK_ENTER_INT_CONTEXT�ڲ�ʵ�־Ϳ��������Щ����
 *
 * ����������ͨ�������RTK_ENTER_INT_CONTEXT���궨��Ϊ�գ��Ա�ִ֤��Ч��
 * ����Ҫ���⸨�����ж��ж�״̬��������Ҫͳ��ISR��ִ��ʱ�䣬����׷��ϵͳִ��·��
 * �������RTK_ENTER_INT_CONTEXT���궨��Ϊ__rtk_enter_int_context
 */
 
#define RTK_ENTER_INT_CONTEXT() 

/**
 * \brief ���˳�isrǰ����RTK_EXIT_INT_CONTEXT��
 *        ������rtk������ǰ׼���뿪�ж�������
 *
 * ����RTK_ENTER_INT_CONTEXT���Ѿ����͹���ԭ��
 * ͨ�������RTK_EXIT_INT_CONTEXT���궨��Ϊ�գ��Ա�ִ֤��Ч��
 * ����Ҫ���⸨�����ж��ж�״̬��������Ҫͳ��ISR��ִ��ʱ�䣬����
 * ׷��ϵͳִ��·���������
 * RTK_EXIT_INT_CONTEXT���궨��Ϊ__rtk_exit_int_context
 */
#define RTK_EXIT_INT_CONTEXT()



#endif          //__RTK_INT_H__
