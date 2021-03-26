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
 * \brief 事件报告接口
 *
 * \internal
 * \par modification history:
 * - 1.02 17-09-05  anu, refine the description
 * - 1.01 15-01-05  orz, move event configuration to aw_event_cfg.c.
 * - 1.00 14-06-05  orz, first implementation.
 * \endinternal
 */

#ifndef __AW_EVENT_H
#define __AW_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_event
 * \copydoc aw_event.h
 * @{
 */

#include "aw_types.h"
#include "aw_compiler.h"
#include "aw_msgq.h"

/** \brief 事件句柄标志定义 */
enum event_handler_flags {
    EVENT_HANDLER_FLAG_NONE       = 0, /**< \brief 无要求 */
    EVENT_HANDLER_FLAG_AUTO_UNREG = 1  /**< \brief 事件触发后解注册事件 */
};

/** \brief 事件处理标志定义 */
enum event_proc_flags {
    EVENT_PROC_FLAG_ASYNC    = 0, /**< \brief 异步事件触发  */
    EVENT_PROC_FLAG_SYNC     = 1, /**< \brief 同步事件触发  */
    EVENT_PROC_FLAG_CAT_ONLY = 2  /**< \brief 只触发事件种类 */
};

struct event_type;                /**< \brief 事件类型声明 */
struct event_category;            /**< \brief 事件种类声明 */

/**
 * \brief 事件触发回调函数定义
 *
 * \param[in] evt_type 事件类型指针
 * \param[in] evt_data 触发事件传输的数据
 * \param[in] hdl_data 初始化事件时保存的用户数据
 *
 * \return 无
 */
typedef void event_function_t (struct event_type *evt_type,
                               void              *evt_data,
                               void              *hdl_data);

/** \brief 事件句柄定义 */
struct event_handler {
    uint16_t              flags; /**< \brief 事件句柄的标志 */
    event_function_t     *func;  /**< \brief 触发的回调函数 */
    void                 *data;  /**< \brief 用户数据 */
    struct event_handler *next;  /**< \brief 下一个事件句柄 */
};

/**
 * \brief 初始化事件句柄
 *
 * \param[in] handler  事件句柄
 * \param[in] func     事件触发回调函数
 * \param[in] data     事件的用户数据
 * \param[in] flags    事件标志
 *
 * \return 无
 */
aw_static_inline void aw_event_handler_init (struct event_handler *handler,
                                             event_function_t     *func,
                                             void                 *data,
                                             uint16_t              flags)
{
    handler->flags = flags;
    handler->func  = func;
    handler->data  = data;
    handler->next  = NULL;
}

/**
 * \brief 销毁一个事件句柄
 *
 * \param[in] handler  事件句柄指针
 *
 * \return 无
 */
aw_static_inline void aw_event_handler_destroy (struct event_handler *handler)
{
}

/**
 * \brief 事件类型定义
 */
struct event_type {
    struct event_handler  *handler;  /**< \brief 事件句柄指针 */
    struct event_category *category; /**< \brief 归属于哪个事件种类 */
    struct event_type     *next;     /**< \brief 下一个事件类型指针 */
};

/**
 * \brief 初始化事件类型
 *
 * \param[in] event 事件类型
 *
 * \return 无
 */
void aw_event_init (struct event_type *event);

/**
 * \brief 销毁事件类型
 *
 * \param[in] event 事件类型指针
 *
 * \return 无
 */
aw_static_inline void aw_event_destroy (struct event_type *event)
{
}

/**
 * \brief 在事件类型中注册事件句柄
 *
 * \param[in] event    事件类型指针
 * \param[in] handler  事件句柄指针
 *
 * \return 无
 */
void aw_event_handler_register (struct event_type     *event,
                                struct event_handler  *handler);

/**
 * \brief 在事件类型中解注册事件句柄
 *
 * \param[in] event    事件类型指针
 * \param[in] handler  事件句柄指针
 *
 * \return 无
 */
int aw_event_handler_unregister (struct event_type     *event,
                                 struct event_handler  *handler);


/**
 * \brief 产生一个事件类型
 *
 * \note 调用该函数，将会导致事件种类中该事件类型的所有已注册句柄被回调
 *       不可在中断中调用本函数
 *
 * \param[in] event     要产生的事件类型
 * \param[in] evt_data  要传输事件的数据
 * \param[in] proc_type 事件处理标志 \sa enum event_proc_flags
 *
 * \return AW_OK则触发成功，其他错误值请参考错误代码
 */
int aw_event_raise (struct event_type     *event,
                    void                  *evt_data,
                    enum event_proc_flags  proc_type);

/**
 * \brief 事件种类的定义
 *
 * \note 一个事件种类可以包含一个或多个事件类型
 */
struct event_category {
    struct event_type    *event;    /**< \brief 事件类型指针 */
    struct event_handler *handler;  /**< \brief 事件句柄指针 */
};

/**
 * \brief 初始化事件种类
 *
 * \param[in] category 事件种类指针
 *
 * \return 无
 */
void aw_event_category_init (struct event_category *category);

/**
 * \brief 销毁事件种类
 *
 * \param[in] category 事件种类指针
 *
 * \return 无
 */
aw_static_inline void aw_event_category_destroy (
        struct event_category *category)
{
}

/**
 * \brief 注册一个事件句柄到事件种类中
 *
 * \param[in] category 事件种类指针
 * \param[in] handler  事件句柄指针
 *
 * \return 无
 */
void aw_event_category_handler_register (struct event_category *category,
                                         struct event_handler  *handler);

/**
 * \brief 解注册一个事件句柄到事件种类中
 *
 * \param[in] category 事件种类指针
 * \param[in] handler  事件句柄指针
 *
 * \return 无
 */
int aw_event_category_handler_unregister (struct event_category *category,
                                          struct event_handler  *handler);

/**
 * \brief 解注册一个事件类型到事件种类中
 *
 * \param[in] category 事件种类指针
 * \param[in] event    事件类型指针
 *
 * \return 无
 */
void aw_event_category_event_register (struct event_category *category,
                                       struct event_type     *event);

/**
 * \brief 事件消息的定义
 *
 * \note 事件消息用于异步触发事件，内部使用
 */
struct event_msg {
    struct event_type *event;     /**< \brief 事件类型指针 */
    void              *evt_data;  /**< \brief 保存的事件数据 */
    int                cat_only;  /**< \brief 是否只能够被事件种类处理 */
};

/**
 * \brief 对要发送的消息进行排队
 *
 * \param[in] msg  事件消息
 *
 * \return AW_OK成功放入队列中，其他错误值请参考错误代码
 */
aw_err_t aw_event_cfg_msg_send (struct event_msg *msg);

/**
 * \brief 事件任务入口
 *
 * \note 该任务是内部使用
 *
 * \param[in] msgq_id  消息队列的ID号
 *
 * \return 无
 */
void aw_event_task (void *msgq_id);

/**
 * \brief 事件的初始化及配置
 *
 * \note 该函数在 aw_event_cfg.c 中实现 在aw_event_lib_init()中被调用, 
 *       不要自行调用
 *
 * \return 无
 */
void aw_event_cfg_init (void);

/**
 * \brief 事件库的初始化
 *
 * \note 该函数内部使用
 *
 * \return 无
 */
void aw_event_lib_init (void);

/** @} grp_aw_if_event */

#ifdef __cplusplus
}
#endif

#endif /* } __AW_EVENT_H */

/* end of file */
