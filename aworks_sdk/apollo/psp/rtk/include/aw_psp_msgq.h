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
 * \brief AWorks1.0 消息队列平台相关标准定义头文件
 *
 * \internal
 * \par modification history:
 * - 1.10 15-09-15  dcf, 变更msgq, memcpy不关中断
 * - 1.10 13-03-06  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_MSGQ_H
#define __AW_PSP_MSGQ_H

#include "rtk.h"
#include "aw_types.h"
#include "aw_compiler.h"

/******************************************************************************/
#define AW_PSP_MSGQ_WAIT_FOREVER        AW_WAIT_FOREVER
#define AW_PSP_MSGQ_NO_WAIT             0

/******************************************************************************/
#define AW_PSP_MSGQ_Q_FIFO              0x00
#define AW_PSP_MSGQ_Q_PRIORITY          0x01

/******************************************************************************/
#define AW_PSP_MSGQ_RPI_NORMAL          0
#define AW_PSP_MSGQ_PRI_URGENT          1

/******************************************************************************/
typedef struct rtk_msgq *aw_psp_msgq_id_t;

/******************************************************************************/
#define _AW_PSP_MSGQ_DECL_COMMON(name,msg_count,msg_size) \
            struct { \
                struct rtk_msgq      msgq; \
                struct rtk_msgq_node node_storage[(msg_count)]; \
                uint8_t              storage[(msg_count)*(msg_size)]; \
            } name;

#define AW_PSP_MSGQ_DECL(name, msg_count, msg_size)  \
            _AW_PSP_MSGQ_DECL_COMMON(name,msg_count,msg_size); \


#define AW_PSP_MSGQ_DECL_STATIC(name, msg_count, msg_size)  \
            static _AW_PSP_MSGQ_DECL_COMMON(name,msg_count,msg_size); 



/* todo: options are not used */
#define AW_PSP_MSGQ_INIT(name, msg_num, msg_size, options)  \
            rtk_msgq_init( \
                    &(name).msgq, \
                    &(name).node_storage[0], \
                    &(name).storage[0], \
                    sizeof((name).storage), \
                    msg_size )

/* todo: priority is not used; the return value is different */
#define AW_PSP_MSGQ_RECEIVE(name, p_buf, nbytes, timeout) \
            aw_msgq_receive(&(name).msgq, p_buf, nbytes, timeout)

/* todo: priority is not used; the return value is different */
#define AW_PSP_MSGQ_SEND(name, p_buf, nbytes, timeout, priority) \
            aw_msgq_send(&(name).msgq, p_buf, nbytes, timeout,priority)

#define AW_PSP_MSGQ_TERMINATE(name) \
            aw_msgq_terminate(&(name).msgq)

#define AW_PSP_MSGQ_VALID(name)         (aw_msgq_valid((&(name).msgq) ) )
#define AW_PSP_MSGQ_SET_INVALID(name)   aw_msgq_terminate(&(name).msgq)


/******************************************************************************/
aw_static_inline aw_err_t aw_msgq_receive(aw_psp_msgq_id_t msgq_id,
                                          void             *p_buf,
                                          size_t            nbytes,
                                          aw_tick_t         timeout)
{
    return rtk_msgq_receive(msgq_id, p_buf, nbytes, timeout);
}

/******************************************************************************/
aw_static_inline aw_err_t aw_msgq_send(aw_psp_msgq_id_t msgq_id,
                                       aw_const void    *p_buf,
                                       size_t            nbytes,
                                       int               timeout,
                                       int               priority)
{
    return rtk_msgq_send(msgq_id, p_buf, nbytes, timeout);
}

/******************************************************************************/
aw_static_inline aw_err_t aw_msgq_terminate(aw_psp_msgq_id_t msgq_id)
{
    return rtk_msgq_terminate(msgq_id);
}

/******************************************************************************/
aw_static_inline aw_bool_t aw_msgq_valid(aw_psp_msgq_id_t msgq_id)
{
    return rtk_msgq_is_valid(msgq_id);
}

#endif  /* __AW_PSP_MSGQ_H */

/* end of file */
