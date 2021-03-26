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
 * \file  rtk_task_internal.h
 * \brief ����rtk���漰��������ص��ڲ�����
 *
 * \internal
 * \par modification history:
 * - 0.9 18-04-23 sni first version.
 * \endinternal
 */


#ifndef             __RTK_TASK_INTERNAL_H__
#define             __RTK_TASK_INTERNAL_H__

/**
 * \brief rtk taskȫ�ֳ�ʼ��
 *
 *  �ڲ�����
 *  �˺��������ʼ�������е���һ�Σ��Ժ�Ͳ����ٵ�����
 * \retval    ��
 */
void __rtk_task_global_init(void);


#define PRIO_NODE_TO_PTCB(pdn)  \
    RTK_LIST_ENTRY(pdn, struct rtk_task, task_prio_q_node)
#define TICK_NODE_TO_PTCB(pdn)  \
    RTK_LIST_ENTRY(pdn, struct rtk_task, tick_node)
#define PEND_NODE_TO_PTCB(pdn)  \
    RTK_LIST_ENTRY(pdn, struct rtk_task, wait_queue_node)

/**
 * \brief rtk�ڲ���ʼ��һ������
 *
 * �˺������Բ������κμ��
 * \param[in]   task        Ҫ��ʼ��������
 * \param[in]   name        ���������
 * \param[in]   priority    ��������ȼ�
 * \param[in]   option      �����ѡ��
 * \param[in]   stack_low   ������ʹ�ö�ջ�ڴ����͵�ַ
 * \param[in]   stack_high  ����ʹ�ö�ջ�ڴ����ߵ�ַ
 * \param[in]   pfunc       �������ں���
 * \param[in]   arg1        ������ں�������1
 * \param[in]   arg2        ������ں�������2
 * \retval    ʧ�ܷ���NULL�����򷵻�ָ��������ƿ��ָ��
 */
struct rtk_task *__rtk_task_init_internal(
        struct rtk_task    *task,
        const char         *name,
        int                 priority,
        rtk_task_option_t   option,
        char *              stack_low,
        char *              stack_high,
        void               *pfunc,
        void               *arg1,
        void               *arg2);

/**
 * \brief �����񻷾���ִ��һ���������
 * �˺�����ִ�лῼ�ǵ����ȼ���ת�������Ȼ�õ�ǰ���ȼ���ߵ�����
 * ������������ȼ��������ͬ���ȼ������Ƿ����ִ��ʱ�̱ȴ�
 * ������ȼ��������磬����ǣ���ִ�����ȼ���ת
 *
 * \retval      ��
 */
void rtk_schedule(void);

/**
 * \brief ����ͨisr�д���һ���������
 * �˺�����ִ�в��ῼ�ǵ����ȼ���ת
 * �������ȡ��ǰ���ȼ���ߵ����񲢳����л�����
 *
 * ֮���������ƣ�����Ϊ��systick�жϲ�Ӧ���������ȼ���ת
 * \retval      ��
 */
void rtk_schedule_isr(void);

/**
 * \brief ��systick isr�д���һ���������
 * ��rtk_schedule���ƣ��˺�����ִ�лῼ�ǵ����ȼ���ת
 * ��Ϊ���ȼ���ת��������systick����
 * ������ע�⣬rtk_schedule_systick_isrֻ�ܹ���systick�жϵ�isr�е���
 * ���ҽ�����isr��ִ����RTK��systick�����߼�����g_systickֵ���Ӻ���ܵ���
 * ��systick�ŵ��߳��д���ʱ��isr�оͲ�Ҫ���ô˺����ˡ�
 * \retval      ��
 */
void rtk_schedule_systick_isr(void);


/**
 * \brief ʹ������ʼ�ȴ��˵ȴ�����
 * �����������Ѻ�Ҫִ�еĻص�����
 *
 * \param[in]   task            Ҫ����������
 * \param[in]   p_wait_queue    Ҫ�ȴ��ĵȴ�����
 * \retval      ��
 */
void __task_pend_on_waitq_ec(struct rtk_task  *task,
                             rtk_wait_queue_t *p_wait_queue);

void __task_set_wakeup_after_tick(struct rtk_task *task,
                                  unsigned int    tick);

void __task_unset_wakeup_after_tick(
        struct rtk_task *task);

/**
 * \brief ��������ִ��join����������
 *
 * �ڲ���������Ҫ���жϱ����������е���
 * \param[in]   task        Ҫ��������join������
 * \param[in]   err         taskִ��join�����õ����˳���
 * \retval    ��
 */
void __rtk_task_wakeup_joining_task(struct rtk_task *task,void *err);

/**
 * \brief �ͷ�һ���������������ջ��tcb����Դ
 *
 *  �ڲ�����
 * \param[in]   task        Ҫ�ͷŵ�����
 * \param[in]
 * \retval    ��
 */
void __rtk_task_release_created_task(struct rtk_task *task);

/**
 * \brief ��wait_queue����һ������
 *
 *  �ڲ�����
 * \param[in]   task        Ҫ�ͷŵ�����
 * \param[in]   err         �����Ѻ�õ��Ĵ�����
 * \retval      ��
 */
void __task_wakup_with_err_ec(struct rtk_task *task,
                              int              err);

aw_err_t __task_get_wake_reason(struct rtk_task *task_current);

/**
 * \brief ���㵱ǰ����ʵ�readyq���ȼ�
 *
 *  �ڲ������������������������ȼ�ʱ����
 * \param[in]   p_task      Ҫ���������
 * \retval      ��
 */
unsigned int _task_cal_suit_readyq_prio_ec(struct rtk_task *p_task);


#endif          /* __RTK_TASK_INTERNAL_H__*/
