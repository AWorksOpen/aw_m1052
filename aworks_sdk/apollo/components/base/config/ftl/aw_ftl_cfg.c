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
 * - 1.00 2017-04-1  vih, first implementation.
 * \endinternal
 */

#include "ftl/aw_ftl_core.h"
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_sem.h"


/** \brief ����ƽ̨��FTL������� */
#ifndef AW_CFG_FTL_VOLUMES
#define AW_CFG_FTL_VOLUMES 10
#endif

/** \brief ��̬�������������ջ��С */
#define __FTL_GB_COLLECT_STACK_SIZE    1024*4

/** \brief ��̬�������������� */
AW_TASK_DECL_STATIC(__ftl_sgbc_task, __FTL_GB_COLLECT_STACK_SIZE);


/**
 * \brief ��������
 * \param[in] rec : FTL��¼�ṹ��
 * \return ��
 */
extern void aw_ftl_garbage_collect (void);

/**
 * \brief ��̬���������������
 */
void __ftl_sgbc_task_entry (void *p_arg)
{
    AW_FOREVER {

        /* ��ʱ1�� */
        aw_mdelay(50);

        aw_ftl_garbage_collect();
    }
}



/**
 * \brief  FTL��ʼ��
 * \param  ��
 * \return ��
 */
void aw_ftl_lib_init (void)
{
    int task_prio;
    static struct aw_ftl_dev rec_pool[AW_CFG_FTL_VOLUMES];
    static aw_bool_t            init_flag = AW_FALSE;

    if (init_flag ) {
        return;
    }

    if (aw_ftl_pool_init(rec_pool, sizeof(rec_pool)) != 0) {
        AW_ERRF(("%s(): failed\n", __func__));
    }
    init_flag = AW_TRUE;

#ifdef AW_DRV_BLOCK_DEV
    /* rec_pool[0].blk_dev_inject = aw_blk_dev_inject;*/
#endif

    task_prio = aw_task_lowest_priority() - 2;      

    /* ��ʼ������__ftl_sgbc_task */
    AW_TASK_INIT(__ftl_sgbc_task,              /**< \brief ����ʵ�� */
                 "__ftl_sgbc_task",            /**< \brief �������� */
                 task_prio,                    /**< \brief �������ȼ� */
                 __FTL_GB_COLLECT_STACK_SIZE,  /**< \brief �����ջ��С */
                 __ftl_sgbc_task_entry,        /**< \brief ������ں��� */
                 NULL);                        /**< \brief ������ڲ��� */

    /* ��������__ftl_sgbc_task */
    AW_TASK_STARTUP(__ftl_sgbc_task);


}

/* end of file */

