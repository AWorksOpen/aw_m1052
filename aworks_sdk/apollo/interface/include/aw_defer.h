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
 * \brief �ӳ���ҵ���
 *
 * \note  �û����Խ�һϵ�й����ڵ�������ӳ���ҵ�д���ʹ��ʱ��Ҫ�û����ϵ���
 *        aw_defer_task_entry(���Խ�����Ϊ�߳����)���ߵ��� AW_DEFER_START
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
 * \code
 * #include aw_defer.h
 * \endcode
 *
 * \par ��ʾ��
 * \code

    #include "aw_defer.h"
    #include "aw_vdebug.h"
    #include "aw_sem.h"

    aw_local struct aw_defer        __g_defer;
    aw_local struct aw_defer_djob   __g_djob;
    AW_SEMB_DECL_STATIC(__g_sem);

    aw_local void  __job_call(void *p_arg)
    {
        AW_INFOF(("job done, job data: 0x%08x\n", (uint32_t)p_arg));
        AW_SEMB_GIVE(__g_sem);
    }

    void  test_defer(void)
    {
        aw_err_t  ret = AW_OK;

        // ��ʼ��һ���ӳ���ҵ
        aw_defer_init(&__g_defer);

        // �����ӳ���ҵ
        AW_DEFER_START(&__g_defer, 3, 0, 4096, "defer");

        // ��ʼ��һ�������ڵ�
        aw_defer_job_init(&__g_djob, __job_call, (void *)0x12345678);

        // ��ʼ���ź���
        AW_SEMB_INIT(__g_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

        // ��һ�������ڵ�������ӳ���ҵ����
        ret = aw_defer_commit(&__g_defer, &__g_djob);
        if (ret != AW_OK) {
            AW_ERRF(("aw_defer_commit err: %d\n", ret));
        }

        // �ȴ���ǰ���������
        AW_SEMB_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);

        // �жϵ�ǰ�����ڵ��Ƿ����
        if (aw_defer_job_is_usable(&__g_defer, &__g_djob)) {
            AW_INFOF(("curr job can usable\n"));
        }
    }

 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.10 17-03-24  may, first implementation
 * \endinternal
 */

#ifndef __AW_DEFER_H
#define __AW_DEFER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_defer
 * \copydoc aw_defer.h
 * @{
 */

#include "aw_task.h"
#include "aw_sem.h"
#include "aw_list.h"

/**
 * \brief �ӳ���ҵ����
 */
struct aw_defer
{
    struct aw_list_head head;   /**< \brief �ڵ� */
    AW_SEMC_DECL(sem);          /**< \brief ʹ�õļ����ź��� */
};

/**
 * \brief ��������
 */
struct aw_defer_djob
{
    struct aw_list_head node;   /**< \brief �ڵ� */
    void (*func)(void *p_arg);  /**< \brief ִ�к��� */
    void *param;                /**< \brief ִ�к������� */
};

/**
 * \brief ��ʼ���ӳ���ҵ
 *
 * \param[in] p_this       �ӳ���ҵ�ڵ�ָ��
 *
 */
void aw_defer_init (struct aw_defer   *p_this);

/**
 * \brief �����ӳ���ҵ�����ﴴ�� tcnt ���߳�ִ���ӳ���ҵ����Щ�߳����ȼ���Ϊtprio��
 *        ��ջ��Сstack_size���߳����� tname������ǰϵͳ��֧�ִ������ȼ���ͬ���̣߳�
 *        �û������Լ��������ɸ��̣߳�Ȼ�� aw_defer_task_entry ��Ϊ��Щ�̵߳���
 *        �ڣ�����Ϊ p_defer
 *
 * \param[in] p_defer       �ӳ���ҵָ��
 * \param[in] tcnt          �߳�����
 * \param[in] tprio         �߳����ȼ�
 * \param[in] stack_size    �̶߳�ջ��С
 * \param[in] tname         �߳�����
 *
 */
#define AW_DEFER_START(p_defer, tcnt, tprio, stack_size, tname) \
    do { \
        struct  aw_defer_task {     \
            struct  __aw_defer_task  {              \
                AW_TASK_DECL(task, (stack_size));   \
            } task[tcnt];   \
        };                  \
                            \
        aw_local struct  aw_defer_task  __g_defer_task;  \
            \
        int          i = 0;         \
        aw_task_id_t task_id;       \
                                    \
        for (i = 0; i < AW_NELEMENTS(__g_defer_task.task); i++) {   \
            task_id = AW_TASK_INIT(__g_defer_task.task[i].task,     \
                                   (tname), \
                                   (tprio), \
                                   (stack_size),        \
                                   (aw_pfuncvoid_t *)aw_defer_task_entry, \
                                   (void*)(p_defer));   \
            if (task_id != NULL) {          \
                aw_task_startup(task_id);   \
            }   \
        }       \
    } while(0)

/**
 * \brief ��ʼ��һ�������ڵ�
 *
 * \param[in] p_this    job �ڵ�ָ��
 * \param[in] pfunc     job �ص�����
 * \param[in] p_arg     job �ص������Ĳ���
 *
 */
void aw_defer_job_init(struct aw_defer_djob   *p_this,
                       void                  (*pfunc) (void *),
                       void                   *p_arg);

/**
 * \brief �жϹ����ڵ��Ƿ����
 *
 * \param[in] p_this  �ӳ���ҵ�ڵ�ָ��
 * \param[in] p_job   �����ڵ�ָ��
 *
 * \retval  AW_TRUE     �ڵ����
 * \retval  AW_FALSE    �ڵ㲻����, �Ѿ�����ӵ�������
 *
 * \par ʾ��
 * \code
 *  void timer_isr (void *param)
 *  {
 *      if (aw_defer_job_is_usable(&g_myjob) ) {
 *          // �ڵ���ã�������ӵ�������
 *          aw_defer_commit(&g_myjob);
 *      } else {
 *          // �ڵ㲻���ã��Ѿ�����ӵ�������
 *      }
 *  }
 * \endcode
 */
aw_bool_t aw_defer_job_is_usable (struct aw_defer      *p_this, 
                                  struct aw_defer_djob *p_job);

/**
 * \brief ��һ�������ڵ�������ӳ���ҵ�д����ڵ���Ӻ�ֻ�ܱ�ִ��һ��
 *       �����ᱻ�Զ������,����ǰ�����ڵ�����ӣ���ֱ�ӷ��ش���
 *
 * \param[in] p_this    ��ʱ��ҵ�ڵ�ָ��
 * \param[in] p_job     job �ڵ�ָ��
 *
 * \retval  AW_OK    �� list_empty(&p_job->node)Ϊ�����
 *                  (p_job->node.next == NULL && p_job->node.prev == NULL)ʱ��
 *                  �ɹ��ӵ������С�
 *
 * \retval -AW_EEXIST  �������ʧ��, �ڵ���Ȼ�ڶ����С�
 *
 * \attention       �ڵ㱻�������, list_empty(&p_job->node)Ϊ�档
 *                  �ر�أ���ʹ��list_empty(&p_job->node)ʱ��Ҫע����жϣ�
 *                  ����ʹ�� aw_defer_job_is_usable()���档
 */
aw_err_t aw_defer_commit( struct aw_defer *p_this, struct aw_defer_djob *p_job);

/**
 * \brief ����ʱ��ҵ��ȡ����ǰ����
 *
 * \param[in] p_this  �ӳ���ҵ�ڵ�ָ��
 * \param[in] p_job   �����ڵ�ָ��
 *
 * \retval  AW_OK       ȡ���ɹ�
 * \retval  -AW_EINVAL  �����д�
 *
 */
aw_err_t aw_defer_abort(struct aw_defer *p_this, struct aw_defer_djob *p_job);

/**
 * \brief ���ӳ���ҵ�й���������ִ�еĵط����û����Դ������ɸ��̣߳�
 *        ������Ϊ�߳�ִ����ڣ�����Ҫ���ӳ���ҵָ����Ϊ�̲߳�������
 *
 * \param[in] p_this    ��ʱ��ҵ�ڵ�ָ��
 * 
 * \return ��
 */
void aw_defer_task_entry (struct aw_defer *p_this);

/** @} grp_aw_if_defer */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_DEFER_H */

/* end of file */
