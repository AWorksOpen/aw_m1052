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
 * \brief 定义rtk中idle任务相关的内部函数
 *
 * \internal
 * \par modification history:
 * - 0.9 18-04-23 sni first version.
 * \endinternal
 */


#ifndef             __RTK_IDLE_H__
#define             __RTK_IDLE_H__

struct rtk_task *__rtk_init_idle_task(void);

/**
 * \brief 启动rtk的idle任务
 *
 *  内部函数
 *  此函数必须初始化过程中调用一次，以后就不能再调用了
 * \retval    无
 */
void __rtk_start_idle_task(void);

int __rtk_is_startup(void);

/**
 * \brief 将一个任务添加到idle任务的自动清理序列中
 *
 * 在CPU空闲时，idle任务将自动释放掉自动退出任务的堆栈和任务tcb
 *
 * 此函数不对参数作任何检查且需要在中断保护中调用
 * \param[in]   task        要初始化的任务
 * \retval    无
 */
void __rtk_add_idle_cleanup_task_ec(struct rtk_task *task);

#endif          /*__RTK_IDLE_H__*/
