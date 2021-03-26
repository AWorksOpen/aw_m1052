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
 * \brief FLASH device manage sub system
 *
 * \internal
 * \par modification history:
 * - 1.00 2016-03-21  cyl, first implementation.
 * \endinternal
 */

#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_sem.h"
#include "aw_ffs.h"



/** \brief ����ƽ̨��FFS������� */
#ifndef AW_CFG_FFS_VOLUMES
#define AW_CFG_FFS_VOLUMES    2
#endif

/** \brief ��̬�������������ջ��С */
#define __FFS_GB_COLLECT_STACK_SIZE    1024

/** \brief ��̬�������������� */
AW_TASK_DECL_STATIC(__ffs_sgbc_task, __FFS_GB_COLLECT_STACK_SIZE);


/** \brief FFS�������� */
extern struct aw_list_head __g_ffs_rec_objs;


/**
 * \brief FFS����س�ʼ��
 *
 * \param[in] pool : ָ����ڴ�
 * \param[in] size : �ش�С
 *
 * \retval AW_OK   : ��ʼ���ɹ�
 * \retval -EINVAL : ��������
 */
extern int aw_ffs_init (struct aw_ffs_rec *pool, unsigned int size);


/**
 * \brief ��������
 * \param[in] rec : FFS��¼�ṹ��
 * \return ��
 */
extern void aw_ffs_garbage_collect (struct aw_ffs_rec *rec);


/**
 * \brief ��̬���������������
 */
void __ffs_sgbc_task_entry (void *p_arg)
{
    struct aw_ffs_rec *rec = NULL;
    struct aw_list_head *p_ffs_list_head = (struct aw_list_head *)p_arg;

    AW_FOREVER {

        /* ��ʱ1�� */
        aw_mdelay(1000);

        if (aw_list_empty(p_ffs_list_head)) {
            continue;
        }

        aw_list_for_each_entry(rec, p_ffs_list_head, struct aw_ffs_rec, node) {
            AW_MUTEX_LOCK(rec->op_mutex, AW_SEM_WAIT_FOREVER);
            aw_ffs_garbage_collect(rec);
            AW_MUTEX_UNLOCK(rec->op_mutex);
        }

    }
}


/**
 * \brief  FFS��ʼ��
 * \param  ��
 * \return ��
 */
void aw_ffs_lib_init (void)
{
    int task_prio;
    static struct aw_ffs_rec rec_pool[AW_CFG_FFS_VOLUMES];

    if (aw_ffs_init(rec_pool, sizeof(rec_pool)) != 0) {
        AW_ERRF(("%s(): failed\n", __func__));
    }

    task_prio = aw_task_lowest_priority() - 2;

    /* ��ʼ������__ffs_sgbc_task */
    AW_TASK_INIT(__ffs_sgbc_task,              /**< \brief ����ʵ�� */
                 "__ffs_sgbc_task",            /**< \brief �������� */
                 task_prio,                    /**< \brief �������ȼ� */
                 __FFS_GB_COLLECT_STACK_SIZE,  /**< \brief �����ջ��С */
                 __ffs_sgbc_task_entry,        /**< \brief ������ں��� */
                 (void *)&__g_ffs_rec_objs);   /**< \brief ������ڲ��� */

    /* ��������__ffs_sgbc_task */
    AW_TASK_STARTUP(__ffs_sgbc_task);
}

/* end of file */
