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
 * \brief ��̬����ӿ�
 *
 * \internal
 * \par modification history:
 * - 1.01 17-09-05  anu, refine the description.
 * - 1.00 15-11-20  dcf, first implementation.
 * \endinternal
 */

#ifndef __AW_TASK2_H
#define __AW_TASK2_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_task2
 * \copydoc aw_task2.h
 * @{
 */

#include "aworks.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_vdebug.h"
#include "aw_isr_defer.h"

struct aw_task2;                        /**< \brief ��̬��������� */
typedef struct aw_task2 *aw_task2_id_t; /**< \brief ��̬����ID���� */
struct aw_task2_cleaner;                /**< \brief ��̬��������������� */

/**
 * \brief ��̬���������ID����
 */
typedef struct aw_task2_cleaner *aw_task2_cleaner_id_t;

/**
 * \brief ��̬�������������
 */
struct aw_task2_cleaner {

    struct aw_task2_cleaner *next;      /**< \brief ָ����һ������� */

    uint32_t is_this_dynamic;           /**< \brief ������ã���λ���Զ��ͷ� */

    void (*pfunc)(void *arg);           /**< \brief �ص����� */
    void *arg;                          /**< \brief �ص��������� */
    void *reserved;                     /**< \brief ���� */
};

/**
 * \brief �����������ʼ��
 *
 * \param[in] p_cleaner ���������ָ��
 *
 */
void aw_task2_cleaner_init (struct aw_task2_cleaner *p_cleaner);

/**
 * \brief ��������������Ƿ������Զ��ͷ�
 *
 * \param[in] p_cleaner  ���������ָ��
 * \param[in] is_dynamic �Ƿ��Զ��ͷ� AW_TRUE: �Զ��ͷ� AW_FALSE: �������Զ��ͷ�
 *
 */
#define aw_task2_cleaner_set_dynamic(p_cleaner, is_dynamic) \
    do {p_cleaner->is_this_dynamic = is_dynamic;} while (0)

/**
 * \brief �������������
 *
 * \param[in] p_cleaner  ���������ָ��
 *
 */
void aw_task2_cleaner_destroy (struct aw_task2_cleaner *p_cleaner);

/**
 * \brief ��̬���񴴽�
 *
 * \param[in] name       ��������
 * \param[in] stack_size �����ջ��С
 * \param[in] priority   �������ȼ�
 * \param[in] pfun       �������
 * \param[in] arg        ������ڲ���
 *
 * \return �ɹ����ض�̬�����ID��ʧ���򷵻�NULL
 */
aw_task2_id_t aw_task2_create (const char *name,
							   int         stack_size,
							   int         priority,
							   void      (*pfun)(void*),
							   void       *arg);

/**
 * \brief ��ȡ��̬���������ID
 *
 * \attention Ӧ�ó���������øú�����ȡ����ID
 *
 * \param[in] task2id ��̬����ID
 *
 * \return �ɹ����������ID��ʧ���򷵻�NULL
 */
aw_task_id_t aw_task2_to_task_id (aw_task2_id_t task2id);

/**
 * \brief ��̬��������
 *
 * \param[in] p_this ��̬����ID
 *
 */
void aw_task2_startup (aw_task2_id_t p_this);

/**
 * \brief ������������
 *
 * \attention һ�ε���ֻ�����һ�������������nextָ���ǿ��ΪNULL
 *
 * \param[in] task2id       ��̬����ID
 * \param[in] p_new_cleaner ����ӵ����������ID
 *
 */
void aw_task2_add_cleaner (
        aw_task2_id_t task2id, aw_task2_cleaner_id_t p_new_cleaner);

/**
 * \brief ���ö�̬�����Ƿ�ɼ�������
 *
 * \param[in] task2id                ��̬����ID
 * \param[in] joinable_true_or_false �Ƿ�ɼ��� AW_TRUE: ���Լ��� ��
 *                                   AW_FALSE: ���ɼ���
 *
 */
void aw_task2_joinable_set (aw_task2_id_t task2id, int joinable_true_or_false);

/**
 * \brief ��ȡ��̬�����Ƿ�ɼ�������
 *
 * \param[in] task2id ��̬����ID
 *
 * \retval AW_TRUE  �ɼ���
 * \retval AW_FALSE ���ɼ���
 */
int aw_task2_joinable_get (aw_task2_id_t task2id);

/**
 * \brief ��Ӷ�̬����
 *
 * \param[in]  task2id ��̬����ID
 * \param[out] pp_ret  �����˳��ķ���ֵ
 *
 * \retval  AW_OK     ��̬�������ɹ�
 * \retval -AW_EINVAL ��̬�������ʧ��
 */
int aw_task2_join (aw_task2_id_t task2id, void **pp_ret);

/**
 * \brief �Ͽ���̬����
 *
 * \param[in]  task2id ��̬����ID
 *
 * \retval  AW_OK    ��̬����Ͽ��ɹ�
 * \retval  AW_ERROR ��̬����Ͽ�ʧ��
 */
int aw_task2_detach (aw_task2_id_t task2id);

/**
 * \brief ��̬��������������
 *
 * \param[in] task2id ��̬����ID
 * \param[in] pfunc   ������Ļص�����
 * \param[in] arg     ������Ļص���������
 *
 * \retval  AW_OK    �����������ӳɹ�
 * \retval  AW_ERROR ���������ʧ��
 */
aw_err_t aw_task2_add_clean_method (
    aw_task2_id_t task2id, void (*pfunc)(void*), void *arg );

/**
 * \brief ��ȡ��ǰ�Ķ�̬����ID
 *
 * \return  ��ǰ�Ķ�̬����
 */
struct aw_task2 *aw_task2_self (void);

/**
 * \brief �˳���ǰ����
 *
 * \param[out] ret �����˳��ķ���ֵ
 *
 * \retval  AW_OK  �˳��ɹ�
 */
aw_err_t aw_task2_exit (void *ret);

/**
 * \brief ���ö�̬�����û�����
 *
 * tls: ��ֵ�߳��ڲ��洢����¼
 *
 * \param[in] task2id ��̬����ID
 * \param[in] newlts  �û�����
 *
 * \return  �ɵĵ�tls
 */
void *aw_task2_set_lts (aw_task2_id_t task2id, void *newlts);

/**
 * \brief ��ȡ��̬�����û�����
 *
 * tls: ��ֵ�߳��ڲ��洢����¼
 *
 * \param[in] task2id ����ID
 *
 * \return  ��ǰ��tls
 */
void *aw_task2_get_lts (aw_task2_id_t task2id);

/** @} grp_aw_if_task2 */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif  /* __AW_TASK2_H */

/* end of file */
