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
 * \file  rtk_wait_queue_type_def.h
 * \brief 定义rtk_wait_queue类型
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-24  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_WAIT_QUEUE_TYPE_DEF_H__
#define         __RTK_WAIT_QUEUE_TYPE_DEF_H__

#pragma pack(push)
#pragma pack(4)

typedef enum {
    RTK_WAIT_QUEUE_TYPE_NULL = 0,
    RTK_WAIT_QUEUE_TYPE_BASE_MUTEX = 1,
    RTK_WAIT_QUEUE_TYPE_SEM_BINARY = 2,
    RTK_WAIT_QUEUE_TYPE_SEM_COUNTER = 3,
    RTK_WAIT_QUEUE_TYPE_MUTEX = 4,
    RTK_WAIT_QUEUE_TYPE_MSGQ,
}rtk_wait_queue_type_t;

typedef enum {
    RTK_WAIT_QUEUE_OPTION_PRIORITY      = 0x0,
    RTK_WAIT_QUEUE_OPTION_FIFO          = 0x1,
}rtk_wait_queue_option_t;

#if CONFIG_RTK_WAIT_QUEUE_PEND_CONST_TIME

#define __MASK_COUNT ((RTK_TOTAL_PRIORITY_COUNT + 31)/32)

typedef struct rtk_wait_queue
{
    rtk_list_node_t         pending_tasks[RTK_TOTAL_PRIORITY_COUNT];
    int                     highest_pending_prio;
    uint32_t                bitmap_group;
    uint32_t                bitmap_mask[__MASK_COUNT];
    rtk_wait_queue_type_t   type;
}rtk_wait_queue_t;

#else

typedef struct rtk_wait_queue
{
    rtk_list_node_t         pending_tasks;
    rtk_wait_queue_type_t   type;
}rtk_wait_queue_t;
#endif
#pragma pack(pop)

#endif          //__RTK_WAIT_QUEUE_H__
