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



/** \brief 配置平台中FFS对象个数 */
#ifndef AW_CFG_FFS_VOLUMES
#define AW_CFG_FFS_VOLUMES    2
#endif

/** \brief 静态垃圾回收任务堆栈大小 */
#define __FFS_GB_COLLECT_STACK_SIZE    1024

/** \brief 静态垃圾回收任务定义 */
AW_TASK_DECL_STATIC(__ffs_sgbc_task, __FFS_GB_COLLECT_STACK_SIZE);


/** \brief FFS对象链表 */
extern struct aw_list_head __g_ffs_rec_objs;


/**
 * \brief FFS对象池初始化
 *
 * \param[in] pool : 指向池内存
 * \param[in] size : 池大小
 *
 * \retval AW_OK   : 初始化成功
 * \retval -EINVAL : 参数错误
 */
extern int aw_ffs_init (struct aw_ffs_rec *pool, unsigned int size);


/**
 * \brief 垃圾回收
 * \param[in] rec : FFS记录结构体
 * \return 无
 */
extern void aw_ffs_garbage_collect (struct aw_ffs_rec *rec);


/**
 * \brief 静态垃圾回收任务入口
 */
void __ffs_sgbc_task_entry (void *p_arg)
{
    struct aw_ffs_rec *rec = NULL;
    struct aw_list_head *p_ffs_list_head = (struct aw_list_head *)p_arg;

    AW_FOREVER {

        /* 延时1秒 */
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
 * \brief  FFS初始化
 * \param  无
 * \return 无
 */
void aw_ffs_lib_init (void)
{
    int task_prio;
    static struct aw_ffs_rec rec_pool[AW_CFG_FFS_VOLUMES];

    if (aw_ffs_init(rec_pool, sizeof(rec_pool)) != 0) {
        AW_ERRF(("%s(): failed\n", __func__));
    }

    task_prio = aw_task_lowest_priority() - 2;

    /* 初始化任务__ffs_sgbc_task */
    AW_TASK_INIT(__ffs_sgbc_task,              /**< \brief 任务实体 */
                 "__ffs_sgbc_task",            /**< \brief 任务名字 */
                 task_prio,                    /**< \brief 任务优先级 */
                 __FFS_GB_COLLECT_STACK_SIZE,  /**< \brief 任务堆栈大小 */
                 __ffs_sgbc_task_entry,        /**< \brief 任务入口函数 */
                 (void *)&__g_ffs_rec_objs);   /**< \brief 任务入口参数 */

    /* 启动任务__ffs_sgbc_task */
    AW_TASK_STARTUP(__ffs_sgbc_task);
}

/* end of file */
