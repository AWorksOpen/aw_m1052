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
 * \brief 动态任务接口
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

struct aw_task2;                        /**< \brief 动态任务的声明 */
typedef struct aw_task2 *aw_task2_id_t; /**< \brief 动态任务ID定义 */
struct aw_task2_cleaner;                /**< \brief 动态任务清除器的声明 */

/**
 * \brief 动态任务清除器ID定义
 */
typedef struct aw_task2_cleaner *aw_task2_cleaner_id_t;

/**
 * \brief 动态任务清除器定义
 */
struct aw_task2_cleaner {

    struct aw_task2_cleaner *next;      /**< \brief 指向下一个清除器 */

    uint32_t is_this_dynamic;           /**< \brief 如果设置，该位将自动释放 */

    void (*pfunc)(void *arg);           /**< \brief 回调函数 */
    void *arg;                          /**< \brief 回调函数参数 */
    void *reserved;                     /**< \brief 保留 */
};

/**
 * \brief 任务清除器初始化
 *
 * \param[in] p_cleaner 任务清除器指针
 *
 */
void aw_task2_cleaner_init (struct aw_task2_cleaner *p_cleaner);

/**
 * \brief 设置任务清除器是否启动自动释放
 *
 * \param[in] p_cleaner  任务清除器指针
 * \param[in] is_dynamic 是否自动释放 AW_TRUE: 自动释放 AW_FALSE: 不进行自动释放
 *
 */
#define aw_task2_cleaner_set_dynamic(p_cleaner, is_dynamic) \
    do {p_cleaner->is_this_dynamic = is_dynamic;} while (0)

/**
 * \brief 销毁任务清除器
 *
 * \param[in] p_cleaner  任务清除器指针
 *
 */
void aw_task2_cleaner_destroy (struct aw_task2_cleaner *p_cleaner);

/**
 * \brief 动态任务创建
 *
 * \param[in] name       任务名称
 * \param[in] stack_size 任务堆栈大小
 * \param[in] priority   任务优先级
 * \param[in] pfun       任务入口
 * \param[in] arg        任务入口参数
 *
 * \return 成功返回动态任务的ID，失败则返回NULL
 */
aw_task2_id_t aw_task2_create (const char *name,
							   int         stack_size,
							   int         priority,
							   void      (*pfun)(void*),
							   void       *arg);

/**
 * \brief 获取动态任务的任务ID
 *
 * \attention 应用程序请勿调用该函数获取任务ID
 *
 * \param[in] task2id 动态任务ID
 *
 * \return 成功返回任务的ID，失败则返回NULL
 */
aw_task_id_t aw_task2_to_task_id (aw_task2_id_t task2id);

/**
 * \brief 动态任务启动
 *
 * \param[in] p_this 动态任务ID
 *
 */
void aw_task2_startup (aw_task2_id_t p_this);

/**
 * \brief 添加任务清除器
 *
 * \attention 一次调用只能添加一个任务清除器，next指针会强制为NULL
 *
 * \param[in] task2id       动态任务ID
 * \param[in] p_new_cleaner 新添加的任务清除器ID
 *
 */
void aw_task2_add_cleaner (
        aw_task2_id_t task2id, aw_task2_cleaner_id_t p_new_cleaner);

/**
 * \brief 设置动态任务是否可加入属性
 *
 * \param[in] task2id                动态任务ID
 * \param[in] joinable_true_or_false 是否可加入 AW_TRUE: 可以加入 ，
 *                                   AW_FALSE: 不可加入
 *
 */
void aw_task2_joinable_set (aw_task2_id_t task2id, int joinable_true_or_false);

/**
 * \brief 获取动态任务是否可加入属性
 *
 * \param[in] task2id 动态任务ID
 *
 * \retval AW_TRUE  可加入
 * \retval AW_FALSE 不可加入
 */
int aw_task2_joinable_get (aw_task2_id_t task2id);

/**
 * \brief 添加动态任务
 *
 * \param[in]  task2id 动态任务ID
 * \param[out] pp_ret  任务退出的返回值
 *
 * \retval  AW_OK     动态任务加入成功
 * \retval -AW_EINVAL 动态任务加入失败
 */
int aw_task2_join (aw_task2_id_t task2id, void **pp_ret);

/**
 * \brief 断开动态任务
 *
 * \param[in]  task2id 动态任务ID
 *
 * \retval  AW_OK    动态任务断开成功
 * \retval  AW_ERROR 动态任务断开失败
 */
int aw_task2_detach (aw_task2_id_t task2id);

/**
 * \brief 动态创建清除器并添加
 *
 * \param[in] task2id 动态任务ID
 * \param[in] pfunc   清除器的回调函数
 * \param[in] arg     清除器的回调函数参数
 *
 * \retval  AW_OK    清除器分配添加成功
 * \retval  AW_ERROR 清除器分配失败
 */
aw_err_t aw_task2_add_clean_method (
    aw_task2_id_t task2id, void (*pfunc)(void*), void *arg );

/**
 * \brief 获取当前的动态任务ID
 *
 * \return  当前的动态任务
 */
struct aw_task2 *aw_task2_self (void);

/**
 * \brief 退出当前任务
 *
 * \param[out] ret 任务退出的返回值
 *
 * \retval  AW_OK  退出成功
 */
aw_err_t aw_task2_exit (void *ret);

/**
 * \brief 设置动态任务用户数据
 *
 * tls: 该值线程内部存储、记录
 *
 * \param[in] task2id 动态任务ID
 * \param[in] newlts  用户数据
 *
 * \return  旧的的tls
 */
void *aw_task2_set_lts (aw_task2_id_t task2id, void *newlts);

/**
 * \brief 获取动态任务用户数据
 *
 * tls: 该值线程内部存储、记录
 *
 * \param[in] task2id 任务ID
 *
 * \return  当前的tls
 */
void *aw_task2_get_lts (aw_task2_id_t task2id);

/** @} grp_aw_if_task2 */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif  /* __AW_TASK2_H */

/* end of file */
