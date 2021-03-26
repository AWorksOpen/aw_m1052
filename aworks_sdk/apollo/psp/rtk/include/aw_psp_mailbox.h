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
 * \brief 消息邮箱平台相关标准定义头文件
 *
 * \internal
 * \par modification history:
 * - 1.10 17-04-01  may, first implementation
 * \endinternal
 */
#ifndef __AW_PSP_MAILBOX_H
#define __AW_PSP_MAILBOX_H

#include "aw_msgq.h"
#include "rtk.h"
#include "aw_types.h"
#include "aw_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/******************************************************************************/
#define AW_PSP_MAILBOX_WAIT_FOREVER        AW_MSGQ_WAIT_FOREVER
#define AW_PSP_MAILBOX_NO_WAIT             AW_MSGQ_NO_WAIT

/******************************************************************************/
#define AW_PSP_MAILBOX_Q_FIFO              AW_MSGQ_Q_FIFO
#define AW_PSP_MAILBOX_Q_PRIORITY          AW_MSGQ_Q_PRIORITY
    
/******************************************************************************/
#define AW_PSP_MAILBOX_RPI_NORMAL          AW_MSGQ_PRI_NORMAL
#define AW_PSP_MAILBOX_PRI_URGENT          AW_MSGQ_PRI_URGENT
    
/******************************************************************************/
typedef aw_msgq_id_t  aw_psp_mailbox_id_t;

/******************************************************************************/
aw_static_inline  aw_err_t aw_mailbox_send(aw_psp_mailbox_id_t mailbox_id,
                                           uint32_t            data,
                                           int                 timeout,
                                           int                 priority)
{                                                           
    void *p_buf = (void *)&data;

    if (mailbox_id == NULL) {
        return -AW_EINVAL;
    }

    return aw_msgq_send((aw_msgq_id_t)mailbox_id,
                        p_buf,
                        sizeof(void *),
                        timeout,
                        priority);
}

/******************************************************************************/
aw_static_inline aw_err_t aw_mailbox_recv(aw_psp_mailbox_id_t  mailbox_id,
                                          void                *p_data,
                                          int                  timeout)
{
    if (mailbox_id == NULL || p_data == NULL) {
        return -AW_EINVAL;
    }

    return aw_msgq_receive((aw_msgq_id_t)mailbox_id,
                           p_data,
                           sizeof(void *),
                           timeout);
}

/******************************************************************************/
aw_static_inline aw_err_t aw_mailbox_terminate(aw_psp_mailbox_id_t mailbox_id)
{
    return aw_msgq_terminate((aw_msgq_id_t)mailbox_id);
}

#define AW_PSP_MAILBOX_DECL(mailbox, mail_num) \
            AW_MSGQ_DECL(mailbox, mail_num, sizeof(void*))

#define AW_PSP_MAILBOX_DECL_STATIC(mailbox, mail_num) \
            AW_MSGQ_DECL_STATIC(mailbox, mail_num, sizeof(void*))

#define AW_PSP_MAILBOX_INIT(mailbox, mail_num, options) \
            AW_MSGQ_INIT(mailbox, mail_num, sizeof(void*), options)

#define AW_PSP_MAILBOX_SEND(mailbox, data, timeout, priority) \
            aw_mailbox_send(&(mailbox).msgq, data, timeout, priority)

#define AW_PSP_MAILBOX_RECV(mailbox, p_data, timeout) \
            aw_mailbox_recv(&(mailbox).msgq, p_data, timeout)

#define AW_PSP_MAILBOX_TERMINATE(mailbox) \
            AW_MSGQ_TERMINATE(mailbox)

#ifdef __cplusplus
}
#endif

#endif /* __AW_PSP_MAILBOX_H */
