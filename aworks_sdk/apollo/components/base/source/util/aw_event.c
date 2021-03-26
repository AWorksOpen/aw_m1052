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
 * \brief Event reporting framework.
 *
 * \internal
 * \par modification history:
 * - 1.01 15-01-05 orz, move event configuration to aw_event_cfg.c.
 * - 1.00 14-06-05 orz, first implementation.
 * \endinternal
 */
#include "apollo.h"
#include "aw_event.h"
#include "aw_sem.h"

/******************************************************************************/
#define __event_lock_init() AW_MUTEX_INIT(__g_event_mutex, AW_SEM_Q_FIFO)
#define __event_lock()      AW_MUTEX_LOCK(__g_event_mutex, AW_SEM_WAIT_FOREVER)
#define __event_unlock()    AW_MUTEX_UNLOCK(__g_event_mutex)

/******************************************************************************/
AW_MUTEX_DECL_STATIC(__g_event_mutex);

/******************************************************************************/
static int __event_handler_delete (struct event_handler **head,
                                   struct event_handler  *handler)
{
    int                   ret  = -AW_EINVAL;
    struct event_handler *prev = container_of(head, struct event_handler, next);
    struct event_handler *hdlr;

    __event_lock();

    for (hdlr = *head; NULL != hdlr; prev = hdlr, hdlr = hdlr->next) {
        if (hdlr == handler) {
            prev->next    = handler->next;
            handler->next = NULL;
            ret           = 0;
            break;
        }
    }

    __event_unlock();

    return ret;
}

/******************************************************************************/

/* go through an event handler list and call each handler functions */
static void __event_handler_process (struct event_handler **head,
                                     struct event_type     *event,
                                     void                  *evt_data)
{
    struct event_handler *next, *handler;

    for (handler = *head; NULL != handler; handler = next) {
        next = handler->next;   /* handlers may delete themselves */
        if (NULL != handler->func) {
            handler->func(event, evt_data, handler->data);
        }
        if (handler->flags & EVENT_HANDLER_FLAG_AUTO_UNREG) {
            /* the handler request to delete it's self */
            handler->next = NULL; /* this handler point to NULL */
            *head         = next; /* previous handler pointer point to next */
        } else {
            head = &handler->next; /* now this handler is 'previous' */
        }
    }
}

/* raise an event immediately. */
static int __event_raise(struct event_type *event, void *evt_data, int cat_only)
{
    __event_lock();

    /* process category handler for this event first */
    __event_handler_process(&event->category->handler, event, evt_data);
    if (!cat_only) {
        /* process event handler */
        __event_handler_process(&event->handler, event, evt_data);
    }

    __event_unlock();

    return 0;
}

/******************************************************************************/
void aw_event_task (void *msgq_id)
{
    aw_msgq_id_t     mq = (aw_msgq_id_t)msgq_id;
    struct event_msg msg;
    aw_err_t         err;

    for (;;) {
        err = aw_msgq_receive(mq, &msg, sizeof(msg), AW_MSGQ_WAIT_FOREVER);
        if (AW_OK == err) {
            (void)__event_raise(msg.event, msg.evt_data, msg.cat_only);
        }
    }
}

/******************************************************************************/
void aw_event_lib_init (void)
{
  __event_lock_init();
    aw_event_cfg_init();
}

/******************************************************************************/
void aw_event_category_init (struct event_category *category)
{
    category->event   = NULL;
    category->handler = NULL;
}

/******************************************************************************/
void aw_event_category_handler_register (struct event_category *category,
                                         struct event_handler  *handler)
{
    __event_lock();

    /* add the handler to event category handler list */
    handler->next     = category->handler;
    category->handler = handler;

    __event_unlock();
}

/******************************************************************************/
int aw_event_category_handler_unregister (struct event_category *category,
                                          struct event_handler  *handler)
{
    return __event_handler_delete(&category->handler, handler);
}

/******************************************************************************/
void aw_event_category_event_register (struct event_category *category,
                                       struct event_type     *event)
{
    event->category = category;

    __event_lock();

    /* add the event type to event category event list */
    event->next     = category->event;
    category->event = event;

    __event_unlock();
}

/******************************************************************************/
void aw_event_init (struct event_type *event)
{
    event->category = NULL;
    event->handler  = NULL;
    event->next     = NULL;
}

/******************************************************************************/
void aw_event_handler_register (struct event_type     *event,
                                struct event_handler  *handler)
{
    __event_lock();

    /* add the handler to event handler list */
    handler->next  = event->handler;
    event->handler = handler;

    __event_unlock();
}

/******************************************************************************/
int aw_event_handler_unregister (struct event_type     *event,
                                 struct event_handler  *handler)
{
    return __event_handler_delete(&event->handler, handler);
}

/******************************************************************************/
int aw_event_raise (struct event_type *event, void *evt_data, enum event_proc_flags proc_type)
{
    struct event_msg msg;
    int              cat_only = proc_type & EVENT_PROC_FLAG_CAT_ONLY;

    if (proc_type & EVENT_PROC_FLAG_SYNC) {
        return __event_raise(event, evt_data, cat_only);
    } else {
        /* add to event process queue */
        msg.event    = event;
        msg.evt_data = evt_data;
        msg.cat_only = cat_only;
        return aw_event_cfg_msg_send(&msg);
    }
}

/* end of file */
