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
 * \file  rtk_task_type_def.h
 * \brief 定义rtk_task类型
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-24  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_TASK_TYPE_DEF_H__
#define         __RTK_TASK_TYPE_DEF_H__

typedef enum __rtk_task_option {
    ENUM_RTK_TASK_OPTION_CAN_NOT_BE_FROZE   = 0x1,
    ENUM_RTK_TASK_OPTION_UNPRIVILEGED       = 0x2,
    ENUM_RTK_TASK_OPTION_CREATE             = 0x40000000,
}rtk_task_option_t;

#define RTK_TASK_WAKE_BY_WAITQ              (1 << 0)
#define RTK_TASK_WAKE_BY_TIMEOUT            (1 << 1)
#define RTK_TASK_WAKE_BY_TERMINATE          (1 << 2)
/**
 * \defgroup task_status   task status definition
 *  @{
 */
typedef enum __rtk_task_status {
    TASK_NOT_READY          = 0x00,  /**< \brief 任务不就绪，但也不是pending或delay状态 */
    TASK_READY              = 0x01,  /**< \brief 任务已经就绪，接下来可能被调度到 */
    TASK_PENDING            = 0x02,  /**< \brief 任务pending在某个等待队列上 */
    TASK_DELAY              = 0x04,  /**< \brief 任务处于延时休眠状态 */
    TASK_NOT_STARTUP        = 0x08,
    TASK_JOINING            = 0x10,
    TASK_DEAD               = 0x20,
    TASK_FROZE              = 0x4000,/**< \brief 任务已经被冻结 */
    TASK_FREEZING           = 0x8000,/**< \brief 任务正在被冻结 */
}rtk_task_status_t;

/**
 *  @}
 */

#endif          /*__RTK_WAIT_QUEUE_H__*/
