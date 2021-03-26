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
 * \brief �¼���־�飬����ͨ����ʵ��һ��һ��һ�Զࡢ���һ����Զ��̼߳��ͬ��������
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
 * \code
 * #include aw_flag_grp.h
 * \endcode
 *
 * \par ��ʾ��
 * \code
 *
    #include "aw_flag_grp.h"
    #include "aw_vdebug.h"
    #include "aw_delay.h"
    #include "aw_task.h"

    // �����¼��鼰�ڵ�
    aw_local aw_flag_grp_t      __g_flg_grp;
    aw_local aw_flag_node_t     __g_flg_node0;
    aw_local aw_flag_node_t     __g_flg_node1;

    #define  TEST_FLAG_TASK_STACK_SIZE    512
    #define  TEST_FLAG_TASK_PRIO          3
    AW_TASK_DECL_STATIC(test_flg_task, TEST_FLAG_TASK_STACK_SIZE);

    #define  TEST_NODE0_FLAG   0x0C
    #define  TEST_NODE1_FLAG   0x83

    aw_local void __task_flag_entry (void *p_arg)
    {
        aw_err_t  ret = AW_OK;

        for (;;) {
            aw_mdelay(500);

            ret = aw_flag_pend(&__g_flg_grp,
                               &__g_flg_node0,
                               TEST_NODE0_FLAG,
                               AW_FLAG_NODE_WAIT_AND|AW_FLAG_NODE_WAIT_CLEAR,
                               AW_SEM_WAIT_FOREVER,
                               NULL);
            if (ret != AW_OK) {
                AW_ERRF(("aw_flag_pend err: %d\n", ret));
                continue;
            }

            ret = aw_flag_post(&__g_flg_grp,
                               TEST_NODE1_FLAG,
                               AW_FLAG_OPT_SET);
            if (ret != AW_OK) {
                AW_ERRF(("aw_flag_post err: %d\n", ret));
                continue;
            }
        }
    }

    void  test_flag_grp(void)
    {
        aw_err_t  ret   = AW_OK;
        uint32_t  flag  = 0;

        // ��ʼ���¼���
        ret = aw_flag_init(&__g_flg_grp,
                           0,
                           AW_FLAG_WAIT_Q_PRIORITY);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_init err: %d\n", ret));
            return;
        }

        // ��ʼ���¼���ڵ�0
        ret = aw_flag_node_init(&__g_flg_node0);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_node_init err: %d\n", ret));
            return;
        }

        // ��ʼ���¼���ڵ�1
        ret = aw_flag_node_init(&__g_flg_node1);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_node_init err: %d\n", ret));
            return;
        }

        // ��ʼ������
        AW_TASK_INIT(test_flg_task,
                     "test_flg_task",
                     TEST_FLAG_TASK_PRIO,
                     TEST_FLAG_TASK_STACK_SIZE,
                     __task_flag_entry,
                     NULL);
        // ��������
        AW_TASK_STARTUP(test_flg_task);

        // �����¼��� TEST_NODE0_FLAG
        ret = aw_flag_post(&__g_flg_grp,
                           TEST_NODE0_FLAG,
                           AW_FLAG_OPT_SET);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_post err: %d\n", ret));
            return;
        }

        // �ȴ��¼����־�������� TEST_NODE1_FLAG
        ret = aw_flag_pend(&__g_flg_grp,
                           &__g_flg_node1,
                           TEST_NODE1_FLAG,
                           AW_FLAG_NODE_WAIT_OR|AW_FLAG_NODE_WAIT_CLEAR,
                           AW_SEM_WAIT_FOREVER,
                           &flag);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_pend err: %d\n", ret));
            return;
        }
        AW_INFOF(("return flag is: %d\n", flag));

        // ���ٽڵ�
        ret = aw_flag_node_terminate(&__g_flg_node0);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_node_terminate err: %d\n", ret));
            return;
        }
        ret = aw_flag_node_terminate(&__g_flg_node1);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_node_terminate err: %d\n", ret));
            return;
        }

        // �����¼������
        ret = aw_flag_terminate(&__g_flg_grp);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_node_terminate err: %d\n", ret));
            return;
        }

        AW_INFOF(("test aw_flag_grp successful.\n"));
    }
 *
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history
 * - 1.00 15-06-02  dav, create file
 * - 1.01 17-04-25  may, add cpu lock to protect, change interface, fix bug.
 * @endinternal
 */

#ifndef __AW_FLAG_GRP_H
#define __AW_FLAG_GRP_H

#ifdef __cplusplus
extern "C" {
#endif

 /**
  * \addtogroup grp_aw_if_flag_grp
  * \copydoc aw_flag_grp.h
  * @{
  */

#include "aworks.h"
#include "aw_sem.h"
#include "aw_int.h"
#include "aw_list.h"

/**
 * \brief �����¼���ṹ��
 */
#define AW_FLAG_DECL(flag_grp) \
            struct aw_flag_grp flag_grp;

/**
 * \brief ���徲̬�¼���ṹ��
 */
#define AW_FLAG_DECL_STATIC(flag_grp) \
            static struct aw_flag_grp flag_grp;

/**
 * \brief �����¼���ڵ�
 */
#define AW_FLAG_NODE_DECL(flag_grp_node) \
            struct aw_flag_grp_node flag_grp_node;

/**
 * \brief ���徲̬�¼���ڵ�
 */
#define AW_FLAG_NODE_DECL_STATIC(flag_grp_node) \
            static struct aw_flag_grp_node flag_grp_node;

/**
 * \brief �¼���ڵ��Ŷӵȴ���ʽ
 */
typedef enum {
    AW_FLAG_WAIT_Q_FIFO     = 0,    /**< \brief ���սڵ��Ƚ��ȼ���*/
    AW_FLAG_WAIT_Q_PRIORITY = 1     /**< \brief ���սڵ������߳����ȼ�Խ��Խ�ȼ��� */
} aw_flag_wait_q_t;

/**
 * \brief �¼���ڵ�ȴ�����
 */
#define AW_FLAG_NODE_WAIT_AND       0x01    /**< \brief �������б�־λ������ */
#define AW_FLAG_NODE_WAIT_OR        0x02    /**< \brief ����һ����־λ�������㼴�� */
#define AW_FLAG_NODE_WAIT_CLEAR     0x04    /**< \brief �ȴ���־λ����������ǰ��־λ */

/**
 * \brief �¼��������������λ���������ֵ
 */
typedef enum {
    AW_FLAG_OPT_SET = 0,        /**< \brief ��λ��Ӧ���λ */
    AW_FLAG_OPT_CLR = 1,        /**< \brief ��ն�Ӧ���λ */
    AW_FLAG_OPT_CPY = 2         /**< \brief ��ֵ��־ */
} aw_flag_opt_t;

/**
 * \brief �¼������Ͷ���
 */
typedef struct aw_flag_grp {
    uint32_t flags;                     /**< \brief ͳһ��־���� */
    struct aw_list_head wait_list_head; /**< \brief ���������׵�ַ */
    aw_flag_wait_q_t  wait_q;           /**< \brief �¼���ڵ�ȴ��Ŷӷ�ʽ */
    int               state;            /**< \brief �¼��鵱ǰ״̬ */
} aw_flag_grp_t;

/**
 * \brief �¼���ڵ����Ͷ���
 */
typedef struct aw_flag_grp_node {
    struct aw_list_head list_node;      /**< \brief ���������еĽڵ� */
    uint32_t            flags;          /**< \brief ��ǰ���ȴ��ı�־λ */
    uint32_t            flag_buf;       /**< \brief ��־���� */
    uint32_t            wait_type;      /**< \brief �ȴ���ʽ */
    int                 prio;           /**< \brief ������������ȼ� */
    int                 use_sta;        /**< \brief ��ǰ�¼��ڵ��Ƿ�ռ�� */
    AW_SEMB_DECL( sem );                /**< \brief �ȴ��ź��� */
} aw_flag_node_t;

/**
 * \brief ��ʼ���¼������
 *
 * \param[in] p_flag_grp    �¼������
 * \param[in] flags         ��ʼ�ı��
 * \param[in] wait_q        �¼���ڵ��Ŷӵȴ���ʽ������Ϊ AW_FLAG_WAIT_Q_FIFO ��
 *                          AW_FLAG_WAIT_Q_PRIORITY
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 *
 * \attention ����ֻ�ܳ�ʼ��һ��
 */
aw_err_t aw_flag_init(aw_flag_grp_t    *p_flag_grp,
                      uint32_t          flags,
                      aw_flag_wait_q_t  wait_q);

/**
 * \brief ��ʼ���¼���ڵ�
 *
 * \param[in] p_node        �¼���ڵ����
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 *
 * \attention ����ֻ�ܳ�ʼ��һ��
 */
aw_err_t aw_flag_node_init(aw_flag_node_t *p_node);

/**
 * \brief �ȴ��¼���ڵ����������(��Ӧ��־λ��������)����
 *
 * \param[in] p_flag_grp    �¼������
 * \param[in] p_node        ��Ҫ�ȴ����¼���ڵ����
 * \param[in] flags         ��Ҫ�ȴ����¼���־ֵ
 * \param[in] wait_type     �¼���ڵ�ȴ���ʽ��AW_FLAG_NODE_WAIT_AND ��
 *                          AW_FLAG_NODE_WAIT_OR�����ȴ���������Ҫ�����
 *                          ��־λ������|AW_FLAG_NODE_WAIT_CLEAR���磺
 *                          (AW_FLAG_NODE_WAIT_AND|AW_FLAG_NODE_WAIT_CLEAR)��
 *                          (AW_FLAG_NODE_WAIT_OR|AW_FLAG_NODE_WAIT_CLEAR)��
 * \param[in] timeout       �ȴ���ʱʱ��(ms)����ʱ�󷵻ش���ֵ��
 *                          ����̽�������Ƿ����㣬������Ϊ 0,
 *                          �������õȴ���������Ϊ AW_SEM_WAIT_FOREVER
 * \param[in] p_rev         ���ص�ǰ������ı�־λ���������ȡ�����Դ���NULL
 *
 * \retval  AW_OK       �ѽ��յ�ָ���¼���־λ
 * \retval  -AW_EINVAL  �����д�
 * \retval  -AW_ETIME   �ȴ���ʱ
 * \retval  -AW_EBUSY   �ڵ㱻ռ��
 * \retval  -AW_ENODEV  �ڵ���¼���������
 *
 * \attention �������жϷ����е��ã����ǲ��� timeout Ϊ 0
 */
aw_err_t aw_flag_pend(aw_flag_grp_t  *p_flag_grp,
                      aw_flag_node_t *p_node,
                      uint32_t        flags,
                      uint32_t        wait_type,
                      uint32_t        timeout,
                      uint32_t       *p_rev);

/**
 * \brief �����¼����б��
 *
 * \param[in] p_flag_grp    �¼������
 * \param[in] flags         ���õı��
 * \param[in] option        ���÷�ʽ�� AW_FLAG_OPT_SET��AW_FLAG_OPT_CLR��
 *                          AW_FLAG_OPT_CPY���磺
 *                          �� flags Ϊ 0x11���¼��鵱ǰ��־Ϊgrp_flag����
 *                             AW_FLAG_OPT_SET�� grp_flag | flags
 *                             AW_FLAG_OPT_CLR�� grp_flag & (~flags)
 *                             AW_FLAG_OPT_CPY: grp_flag = flags
 *
 * \retval AW_OK        �ɹ�
 * \retval -AW_EINVAL   �����д�
 * \retval -AW_ENODEV   �¼������������
 *
 * \attention �������жϷ�����߳��е���
 */
aw_err_t aw_flag_post(aw_flag_grp_t    *p_flag_grp,
                      uint32_t          flags,
                      aw_flag_opt_t     option);

/**
 * \brief �����¼������
 *
 * \param[in] p_flag_grp  �¼������
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_flag_terminate(aw_flag_grp_t *p_flag_grp);

/**
 * \brief �����¼���ڵ����
 *
 * \param[in] p_node  �¼���ڵ����
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_flag_node_terminate(aw_flag_node_t *p_node);

/** @} grp_aw_if_flag_grp */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FLAG_GRP_H */

 /* end of file */
