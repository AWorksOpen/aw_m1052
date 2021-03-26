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
 * \file  rtk_task_sync_lock.h
 * \brief 定义rtk_mutex的内部操作函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_TASK_SYNC_LOCK_H__
#define         __RTK_TASK_SYNC_LOCK_H__


void __rtk_mutex_sync_lock_init(void);
void __rtk_mutex_sync_lock(void);
void __rtk_mutex_sync_unlock(void);


#endif          /*__RTK_TASK_SYNC_LOCK_H__*/
