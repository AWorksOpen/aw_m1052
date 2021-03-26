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
 * \brief RTOS信号量操作抽象接口
 *
 * 使用本服务需要包含头文件 aw_sem.h
 *
 * \par 简单示例
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_TASK_DECL_STATIC(task_a, 512);   // 定义任务 task_a
 *  AW_TASK_DECL_STATIC(task_b, 512);   // 定义任务 task_b
 *
 *  AW_SEMB_DECL_STATIC(sem);           // 定义二进制信号量 sem
 *
 *  void func_a (void *arg)
 *  {
 *      while (1) {
 *          //...
 *          AW_SEMB_GIVE(sem);          //释放信号量
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      while (1) {
 *          AW_SEMB_TAKE(sem, AW_SEM_WAIT_FOREVER); //等待信号量
 *          //...
 *      }
 *  }
 *
 *  int main ()
 *  {
 *
 *      // 初始化信号量
 *      AW_SEMB_INI(sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
 *
 *      // 初始化任务：
 *      AW_TASK_INIT(task_a,            // 任务实体
 *                   "task_a",          // 任务名字
 *                   5,                 // 任务优先级
 *                   512,               // 任务堆栈大小
 *                   func_a,            // 任务入口函数
 *                   NULL);             // 任务入口参数
 *
 *      AW_TASK_INIT(task_b,            // 任务实体
 *                   "task_b",          // 任务名字
 *                   6,                 // 任务优先级
 *                   512,               // 任务堆栈大小
 *                   func_b,            // 任务入口函数
 *                   NULL);             // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(task_a);
 *      AW_TASK_STARTUP(task_b);
 *
 *      return 0;
 *  }
 * \endcode
 *
 * //更多内容待添加。。。
 *
 * \internal
 * \par modification history
 * - 1.12 17-09-05  anu, refine the description
 * - 1.11 17-09-06  zen, refine the description
 * - 1.10 13-03-04  zen, refine the description
 * - 1.02 12-11-20  lyzh, rename some MACRO name. Merge aw_semb.h, aw_semc.h,
 *                  and aw_mutex.h
 * - 1.01 12-11-05  orz, implement with aw_task.h
 * - 1.00 12-10-23  orz, created
 * \endinternal
 */

#ifndef __AW_SEM_H
#define __AW_SEM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_sem
 * \copydoc aw_sem.h
 * @{
 */

#include "aw_psp_sem.h"

/**
 * \name 超时参数定义
 * @{
 */
/**
 * \brief 永久等待
 * \hideinitializer
 */
#define AW_SEM_WAIT_FOREVER     AW_PSP_SEM_WAIT_FOREVER

/**
 * \brief 无等待
 * \hideinitializer
 */
#define AW_SEM_NO_WAIT          AW_PSP_SEM_NO_WAIT
/** @} 超时参数定义 */


/**
 * \name 信号量初始化选项
 * @{
 */
/**
 * \brief 排队方式为 FIFO (先进先出)
 * \hideinitializer
 */
#define AW_SEM_Q_FIFO         AW_PSP_SEM_Q_FIFO

/**
 * \brief 排队方式为优先级
 * \hideinitializer
 */
#define AW_SEM_Q_PRIORITY     AW_PSP_SEM_Q_PRIORITY

/**
 * \brief 互斥信号量删除安全 (用于互斥信号量)
 * \hideinitializer
 */
#define AW_SEM_DELETE_SAFE    AW_PSP_SEM_DELETE_SAFE

/**
 * \brief 防止互斥信号量优先级反转 (用于互斥信号量)
 * \hideinitializer
 */
#define AW_SEM_INVERSION_SAFE   AW_PSP_SEM_INVERSION_SAFE
/** @} 信号量初始化选项 */



#include "_aw_semb.h"
#include "_aw_semc.h"
#include "_aw_mutex.h"




/** @} grp_aw_if_sem */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SEMAPHORE */

/* end of file */
