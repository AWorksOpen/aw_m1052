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
 * \brief Event library configure file.
 *
 * \internal
 * \par modification history:
 * - 1.00 15-01-05 orz, first implementation.
 * \endinternal
 */

#include "aw_event.h"
#include "aw_task.h"

/******************************************************************************/
#ifndef AW_CFG_EVENT_TASK_STK_SIZE
#define AW_CFG_EVENT_TASK_STK_SIZE  512
#endif

#ifndef AW_CFG_EVENT_TASK_PRIO
#define AW_CFG_EVENT_TASK_PRIO      8
#endif

#ifndef AW_CFG_EVENT_MSG_NUM
#define AW_CFG_EVENT_MSG_NUM        4
#endif

/******************************************************************************/
AW_TASK_DECL_STATIC(__g_event_task, AW_CFG_EVENT_TASK_STK_SIZE);

AW_MSGQ_DECL_STATIC(__g_event_msgq,
                      AW_CFG_EVENT_MSG_NUM,
                      sizeof(struct event_msg));

/******************************************************************************/
void aw_event_cfg_init (void)
{
    aw_msgq_id_t msgq = AW_MSGQ_INIT(__g_event_msgq,
                                       AW_CFG_EVENT_MSG_NUM,
                                       sizeof(struct event_msg),
                                       AW_MSGQ_Q_FIFO);

    AW_TASK_INIT(__g_event_task,
                  "event",
                   AW_CFG_EVENT_TASK_PRIO,
                   AW_CFG_EVENT_TASK_STK_SIZE,
                   aw_event_task,
                   (void *)msgq);
    AW_TASK_STARTUP(__g_event_task);
}

aw_err_t aw_event_cfg_msg_send (struct event_msg *msg)
{
    return AW_MSGQ_SEND(__g_event_msgq,
                          msg,
                          sizeof(*msg),
                          AW_MSGQ_WAIT_FOREVER,
                          AW_MSGQ_Q_FIFO);
}

/* end of file */

