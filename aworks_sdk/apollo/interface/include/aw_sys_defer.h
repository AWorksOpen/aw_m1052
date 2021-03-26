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
 * \brief  ϵͳ�ӳ���ҵ���
 *
 * \note
 *      1. ���û���һЩ������Ҫ���첽���ã��������ύ��һЩ�������߳�ȥִ�У�
 *         ��ô defer �ͺ������ˣ�ֻ�趨�� defer ����������Ȼ�󽫹����ڵ� 
 *         commit ���� defer �м���
 *
 *      2. ��ǰϵͳĬ�������������ӳ���ҵ��������� nor_defer �� isr_defer��
 *         ISR defer��    һ�����ڴ����������
 *         Normal defer�� һ�����ڴ���ǽ�����
 *         aw_prj_params.h������������ defer �̵߳Ķ�ջ��С��
 *         \#define AW_CFG_ISR_DEFER_TASK_STACK_SIZE    (1024 * 10)
 *         \#define AW_CFG_NOR_DEFER_TASK_STACK_SIZE    (1024 * 10)
 *
 *      3.  ISR defer:        aw_isr_defer_cfg.c
 *          Normal defer:     aw_nor_defer_cfg.c
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ�
 * \code
 * #include "aw_sys_defer.h"
 * \endcode
 *
 * \par ʹ��ʾ��
 * \code

    #include "aw_sys_defer.h"
    #include "aw_vdebug.h"
    #include "aw_sem.h"

    aw_local struct aw_defer_djob   __g_djob;
    AW_SEMB_DECL_STATIC(__g_sem);

    aw_local void  __job_call(void *p_arg)
    {
        AW_INFOF(("job done, job data: 0x%08x\n", (uint32_t)p_arg));
        AW_SEMB_GIVE(__g_sem);
    }

    void test_sys_defer(void)
    {
        aw_err_t  ret = AW_OK;

        // ��ʼ���ź���
        AW_SEMB_INIT(__g_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

        // ��ʼ��һ�������ڵ�
        aw_defer_job_init(&__g_djob, __job_call, (void *)0x12345678);

        // ��һ�������ڵ������ϵͳ�ж��ӳ���ҵ�д���
        ret = aw_sys_defer_commit(AW_SYS_DEFER_TYPE_ISR, &__g_djob);
        if (ret != AW_OK) {
            AW_ERRF(("aw_sys_defer_commit err: %d\n", ret));
        }

        // �ȴ���ǰ���������
        AW_SEMB_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);

        // �жϵ�ǰ�����ڵ��Ƿ����
        if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_ISR, &__g_djob)) {
            AW_INFOF(("curr job can usable\n"));
        }

        // ��һ�������ڵ������ϵͳ��ͨ�ӳ���ҵ�д���
        ret = aw_sys_defer_commit(AW_SYS_DEFER_TYPE_NOR, &__g_djob);
        if (ret != AW_OK) {
            AW_ERRF(("aw_sys_defer_commit err: %d\n", ret));
        }

        // �ȴ���ǰ���������
        AW_SEMB_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);

        // �жϵ�ǰ�����ڵ��Ƿ����
        if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, &__g_djob)) {
            AW_INFOF(("curr job can usable\n"));
        }
    }

 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 17-03-31  may, first implementation.
 * \endinternal
 */

#ifndef __AW_SYS_DEFER_H
#define __AW_SYS_DEFER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_sys_defer
 * \copydoc aw_sys_defer.h
 * @{
 */

#include "aworks.h"
#include "aw_defer.h"

/** \brief sys_defer ����  */
typedef enum {
    AW_SYS_DEFER_TYPE_ISR,  /**< \brief �������� */
    AW_SYS_DEFER_TYPE_NOR   /**< \brief ��ͨ���� */
} aw_sys_defer_t;

/**
 * \brief ����ϵͳȫ��defer���󣬾��嶨��ֱ���
 *        aw_isr_defer_cfg.c��aw_nor_defer_cfg.c 
 */
 
/** \brief ����ȫ�ֽ�������defer���� */
aw_export struct aw_defer g_sys_isr_defer;  

/** \brief ����ȫ����ͨ����defer���� */
aw_export struct aw_defer g_sys_nor_defer;  

/**
 * \brief �жϹ����ڵ��Ƿ����
 *
 * \param[in] defer_t  ϵͳ�ӳ���ҵ����
 * \param[in] p_job    �����ڵ�ָ��
 *
 * \retval  AW_TRUE     �ڵ����
 * \retval  AW_FALSE    �ڵ㲻����, �Ѿ�����ӵ�������
 *
 * \par ʾ��
 * \code
 *  void timer_isr (void *param)
 *  {
 *      if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_ISR, &g_myjob) ) {
 *          // �ڵ���ã�������ӵ�������
 *          aw_sys_defer_commit(AW_SYS_DEFER_TYPE_ISR, &g_myjob);
 *      } else {
 *          // �ڵ㲻���ã��Ѿ�����ӵ�������
 *      }
 *  }
 * \endcode
 */
aw_static_inline aw_bool_t aw_sys_defer_job_is_usable (
    aw_sys_defer_t defer_t, struct aw_defer_djob *p_job)
{
    if (defer_t == AW_SYS_DEFER_TYPE_ISR) {
        return  aw_defer_job_is_usable(&g_sys_isr_defer, p_job);
    } else if (defer_t == AW_SYS_DEFER_TYPE_NOR) {
        return  aw_defer_job_is_usable(&g_sys_nor_defer, p_job);
    }

    return  AW_FALSE;
}

/**
 * \brief ��һ�������ڵ������ϵͳ�ӳ���ҵ�д����ڵ���Ӻ�ֻ�ܱ�ִ��һ�Σ�����
 *        ���Զ������,����ǰ�����ڵ�����ӣ���ֱ�ӷ��ش���
 *
 * \param[in] defer_t  ϵͳ�ӳ���ҵ����
 * \param[in] p_job    job �ڵ�ָ��
 *
 * \retval  AW_OK    �� list_empty(&p_job->node)Ϊ�����
 *                  (p_job->node.next == NULL && p_job->node.prev == NULL)ʱ��
 *                  �ɹ��ӵ������С�
 *
 * \retval -AW_EEXIST  �������ʧ��, �ڵ���Ȼ�ڶ����С�
 * \retval -AW_EINVAL  �����������
 *
 * \attention       �ڵ㱻�������, list_empty(&p_job->node)Ϊ�档
 *                  �ر�أ���ʹ��list_empty(&p_job->node)ʱ��Ҫע����жϣ�
 *                  ����ʹ�� aw_defer_job_is_usable()���档
 */
aw_static_inline aw_err_t aw_sys_defer_commit(aw_sys_defer_t        defer_t,
                                              struct aw_defer_djob *p_job)
{
    if (defer_t == AW_SYS_DEFER_TYPE_ISR) {
        return  aw_defer_commit(&g_sys_isr_defer, p_job);
    } else if (defer_t == AW_SYS_DEFER_TYPE_NOR) {
        return  aw_defer_commit(&g_sys_nor_defer, p_job);
    }

    return  -AW_EINVAL;
}

/**
 * \brief ��ϵͳ��ʱ��ҵ��ȡ����ǰ����
 *
 * \param[in] defer_t  ϵͳ�ӳ���ҵ����
 * \param[in] p_job    �����ڵ�ָ��
 *
 * \retval  AW_OK       ȡ���ɹ�
 * \retval  -AW_EINVAL  �����д�
 *
 */
aw_static_inline aw_err_t aw_sys_defer_abort(aw_sys_defer_t        defer_t,
                                             struct aw_defer_djob *p_job)
{
    if (defer_t == AW_SYS_DEFER_TYPE_ISR) {
        return  aw_defer_abort(&g_sys_isr_defer, p_job);
    } else if (defer_t == AW_SYS_DEFER_TYPE_NOR) {
        return  aw_defer_abort(&g_sys_nor_defer, p_job);
    }

    return  -AW_EINVAL;
}

/** @} grp_aw_if_sys_defer */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_SYS_DEFER_H */

/* end of file */

