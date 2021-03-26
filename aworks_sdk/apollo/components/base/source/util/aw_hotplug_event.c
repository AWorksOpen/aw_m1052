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
 * \brief expand I/O system.
 *
 * \internal
 * \par modification history:
 * - 1.00 16-08-09 bob, first implementation.
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "aw_hotplug_event.h"
#include "aw_sem.h"
#include<string.h>

/*******************************************************************************
    local variables
*******************************************************************************/

/** \brief 事件链链头 */
static struct aw_list_head __gp_hotplug_event_list;
/** \brief 可插拔设备链链头 */
static struct aw_list_head __gp_hotplug_dev_list;

/** \brief 互斥量，用于引用计数并发保护 */
AW_MUTEX_DECL_STATIC(__g_hotplug_event_mutex);
/** \brief 互斥量，用于引用计数并发保护 */
AW_MUTEX_DECL_STATIC(__g_hotplug_dev_mutex);

struct __hotplug_wait {
    aw_semb_id_t *p_semb_id;
    int           stat;
};

/******************************************************************************/
aw_err_t aw_hotplug_event_register (aw_hotplug_event_t *p_event,
                                    const char         *p_name,
                                    int                 event_flgs,
                                    int               (*pfn_callback)(int   flag,
                                                                      void *p_arg),
                                    void               *p_arg)
{
    if (NULL == p_event ||
        NULL == p_name) {
        return -AW_EINVAL;
    }

    p_event->pfn_callback = pfn_callback;
    p_event->p_arg        = p_arg;
    p_event->p_devname    = p_name;
    p_event->event_flgs   = event_flgs;

    AW_MUTEX_LOCK(__g_hotplug_event_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_add_tail(&p_event->node, &__gp_hotplug_event_list);
    AW_MUTEX_UNLOCK(__g_hotplug_event_mutex);

    return AW_OK;
}

/******************************************************************************/
void aw_hotplug_event_unregister (aw_hotplug_event_t *p_event)
{
    if (NULL == p_event) {
        return ;
    }

    AW_MUTEX_LOCK(__g_hotplug_event_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_del(&p_event->node);
    AW_MUTEX_UNLOCK(__g_hotplug_event_mutex);
}

/******************************************************************************/
aw_err_t aw_hotplug_event_add (aw_hotplug_event_t *p_event,
                               int                *p_event_flgs)
{
    if (NULL == p_event ||
        NULL == p_event_flgs) {
        return -AW_EINVAL;
    }

    p_event->event_flgs |= (*p_event_flgs);
    *p_event_flgs        = p_event->event_flgs;

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_hotplug_event_del (aw_hotplug_event_t *p_event,
                               int                *p_event_flgs)
{
    if (NULL == p_event ||
        NULL == p_event_flgs) {
        return -AW_EINVAL;
    }

    p_event->event_flgs &= (~(*p_event_flgs));
    *p_event_flgs        = p_event->event_flgs;

    return AW_OK;
}

/******************************************************************************/
static int __wait_callback (int event, void *p_arg)
{
    struct __hotplug_wait *p_wait = (struct __hotplug_wait *)p_arg;
    if (event == p_wait->stat) {
        aw_semb_give(*(aw_semb_id_t*)p_wait->p_semb_id);
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_hotplug_event_wait (aw_hotplug_event_t *p_event,
                                int                 stat,
                                int                 time_out)
{
    aw_err_t               err;
    AW_SEMB_DECL(          semb);
    aw_semb_id_t           semb_id;
    int                    tmp_stat;

    int                  (*pfn_tmp_callback)(int, void *);
    void                  *p_tmp_cb_arg;

    struct __hotplug_wait  wait;


    if (NULL == p_event) {
        return -AW_EINVAL;
    }

    aw_hotplug_stat_get(p_event, &tmp_stat);
    if (stat == tmp_stat) {
        return AW_OK;
    }
    pfn_tmp_callback = p_event->pfn_callback;
    p_tmp_cb_arg     = p_event->p_arg;
    memset(&semb,0,sizeof(semb));
    semb_id = AW_SEMB_INIT(semb, 0, AW_SEM_Q_PRIORITY);
    if (semb_id == NULL) {
        return -AW_ENOMEM;
    }
    wait.p_semb_id = &semb_id;
    wait.stat      = stat;

    p_event->pfn_callback = __wait_callback;
    p_event->p_arg        = &wait;

    err = AW_SEMB_TAKE(semb, time_out);

    p_event->pfn_callback = pfn_tmp_callback;
    p_event->p_arg        = p_tmp_cb_arg;

    return err;
}

/******************************************************************************/
aw_err_t aw_hotplug_stat_get (aw_hotplug_event_t* p_event,
                              int                *p_stat)
{
    aw_hotplug_dev_t *p_dev = NULL;

    if (NULL == p_event) {
        return -AW_EINVAL;
    }

    *p_stat =  AW_HOTPLUG_EVT_REMOVE;

    aw_list_for_each_entry(p_dev,
                          &__gp_hotplug_dev_list,
                           aw_hotplug_dev_t,
                           node) {
        if (strcmp(p_dev->p_devname, p_event->p_devname) == 0) {
            *p_stat = p_dev->stat;
        }
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_hotplug_dev_register (aw_hotplug_dev_t *p_dev,
                                  const char       *p_name,
                                  int               flags)
{
    aw_hotplug_dev_t *p_tmpdev = NULL;

    if ((NULL == p_dev)  ||
        (NULL == p_name) ||
        (EOS == *p_name)) {
        return -AW_EINVAL;
    }

    p_dev->p_devname = p_name;


    aw_list_for_each_entry(p_tmpdev,
                          &__gp_hotplug_dev_list,
                           aw_hotplug_dev_t,
                           node) {
        if (strcmp(p_tmpdev->p_devname, p_name) == 0) {
            return -AW_EEXIST;
        }
    }

    AW_MUTEX_LOCK(__g_hotplug_dev_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_add_tail(&p_dev->node, &__gp_hotplug_dev_list);
    AW_MUTEX_UNLOCK(__g_hotplug_dev_mutex);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_hotplug_dev_unregister (aw_hotplug_dev_t *p_dev)
{
    if (NULL == p_dev) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(__g_hotplug_dev_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_del(&p_dev->node);
    AW_MUTEX_UNLOCK(__g_hotplug_dev_mutex);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_hotplug_event_send (aw_hotplug_dev_t *p_dev,
                                int               event_flgs)
{
    aw_hotplug_event_t *p_event = NULL;

    if (NULL == p_dev) {
        return -AW_EINVAL;
    }

    p_dev->stat = event_flgs;

    aw_list_for_each_entry(p_event,
                          &__gp_hotplug_event_list,
                           aw_hotplug_event_t,
                           node) {
        if (strcmp(p_dev->p_devname, p_event->p_devname) == 0 &&
            (event_flgs & p_event->event_flgs)                &&
            p_event->pfn_callback) {
            p_event->pfn_callback(event_flgs & p_event->event_flgs,
                                  p_event->p_arg);
        }
    }

    return AW_OK;
}

/******************************************************************************/
void aw_hotplug_init(void)
{
    aw_list_head_init(&__gp_hotplug_event_list);
    aw_list_head_init(&__gp_hotplug_dev_list);

    AW_MUTEX_INIT(__g_hotplug_event_mutex, AW_SEM_Q_PRIORITY);
    AW_MUTEX_INIT(__g_hotplug_dev_mutex, AW_SEM_Q_PRIORITY);
}

/* end of file */
