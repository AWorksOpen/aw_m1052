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
 * \brief block device
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-25  deo, first implementation
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "apollo.h"
#include "aw_sem.h"
#include "fs/aw_blk_dev.h"
#include "aw_refcnt.h"
#include "aw_list.h"
#include "aw_mem.h"
#include "stdio.h"
#include "string.h"
#include "aw_vdebug.h"



#define __MUTEX_LOCK_TIMEOUT   AW_WAIT_FOREVER
#define __BD_LIST_TAIL         ((struct aw_blk_dev *)(&__gp_bd_list_head))

#define __BD_TRACE(info)       AW_WARNF(info)

#define __BD_DEF_EVT_CNT       10


extern int aw_blk_cache_init (struct aw_blk_cache *p_cache,
                              struct aw_blk_dev   *p_bd,
                              void                *p_buf,
                              uint_t               size,
                              int                  b_blks);
extern void aw_blk_cache_uninit (struct aw_blk_cache *p_cache);
extern int aw_blk_cache_ioctl (struct aw_blk_cache *p_cache,
                               int                  cmd,
                               void                *p_arg);
extern int aw_blk_cache_areq (struct aw_blk_cache *p_cache,
                              struct aw_blk_arp   *p_arp);



struct __inject_evt_arg {
    const char      *name;
    aw_semb_id_t     semb;
};




/** \brief block event */
struct __blk_event {
    aw_blk_dev_evt_fun_t     pfn_cb;
    void                    *p_arg;
    AW_SEMB_DECL(            semb);
    int                      refcnt;
};






/*******************************************************************************
    local variables
*******************************************************************************/
AW_MUTEX_DECL_STATIC(          __g_bd_list_lock);
aw_local struct aw_blk_dev    *__gp_bd_list_head = NULL;
aw_local struct __blk_event   *__g_bd_evt_tab = NULL;
aw_local int                   __g_bd_evt_cnt = 0;
aw_local aw_bool_t                __g_bd_init = AW_FALSE;



#define __BD_LIST_LOCK()   AW_MUTEX_LOCK(__g_bd_list_lock, __MUTEX_LOCK_TIMEOUT)
#define __BD_LIST_UNLOCK() AW_MUTEX_UNLOCK(__g_bd_list_lock)

#define __BD_LOCK(p_bd, ret) \
            do { \
                ret = AW_MUTEX_LOCK(p_bd->lock, __MUTEX_LOCK_TIMEOUT); \
                if ((ret == AW_OK) && \
                    (!AW_BD_VALID(p_bd))) { \
                    AW_MUTEX_UNLOCK(p_bd->lock); \
                    ret = -AW_ENODEV; \
                } \
            } while (0)

#define __BD_UNLOCK(p_bd) AW_MUTEX_UNLOCK(p_bd->lock)


/******************************************************************************/
aw_local aw_bool_t __bd_list_find (const char         *name,
                                struct aw_blk_dev **p_bd)
{
    struct aw_blk_dev *p_tmp;
    int                len;

    /* limit length */
    len = strlen(name);
    if (len >= AW_BD_NAME_MAX) {
        return NULL;
    }

    __BD_LIST_LOCK();
    for (p_tmp = __gp_bd_list_head;
         p_tmp != __BD_LIST_TAIL;
         p_tmp = p_tmp->p_next) {

        /* match length */
        if (strlen(p_tmp->name) == len) {
            /* match name */
            if (strncmp(p_tmp->name, name, len) == 0) {
                break;
            }
        }
    }

    if (p_bd) {
        if ((p_tmp != __BD_LIST_TAIL) &&
            (aw_refcnt_get(&p_tmp->ref) == AW_OK)) {
            *p_bd = p_tmp;
        } else {
            *p_bd = NULL;
        }
    }

    __BD_LIST_UNLOCK();

    return ((p_tmp != __BD_LIST_TAIL) ? AW_TRUE : AW_FALSE);
}




/******************************************************************************/
aw_local aw_err_t __bd_list_add (struct aw_blk_dev *p_bd)
{
    aw_err_t ret = AW_OK;

    __BD_LIST_LOCK();
    /* check if device had already existed */
    if (!__bd_list_find(p_bd->name, NULL)) {
        p_bd->p_next      = __gp_bd_list_head;
        __gp_bd_list_head = p_bd;
    } else {
        ret = -AW_EEXIST;
    }
    __BD_LIST_UNLOCK();
    return ret;
}




/******************************************************************************/
aw_local void __bd_list_del (struct aw_blk_dev *p_bd)
{
    struct aw_blk_dev *p_tmp;

    __BD_LIST_LOCK();
    if (p_bd == __gp_bd_list_head) {
        /* in head */
        __gp_bd_list_head = p_bd->p_next;
        p_bd->p_next      = NULL;
    } else {
        p_tmp = __gp_bd_list_head;

        /* search it */
        while (p_tmp) {
            if (p_tmp->p_next == p_bd) {
                p_tmp->p_next = p_bd->p_next;
                p_bd->p_next  = NULL;
                break;
            }
            p_tmp = p_tmp->p_next;
        }
    }
    __BD_LIST_UNLOCK();
}




/******************************************************************************/
aw_local aw_err_t __bd_def_ioctl (struct aw_blk_dev *p_dev,
                                  int                cmd,
                                  void              *p_arg)
{
    return -AW_ENOTSUP;
}




/******************************************************************************/
aw_local aw_err_t __bd_def_areq (struct aw_blk_dev *p_dev,
                                 struct aw_blk_arp *p_arp)
{
    p_arp->error = -AW_ENOTSUP;
    p_arp->resid = p_arp->nbytes;

    if (p_arp->pfn_done) {
        p_arp->pfn_done(p_arp);
    }

    return -AW_ENOTSUP;
}




/******************************************************************************/
aw_local void __bd_def_release (struct aw_blk_dev *p_dev)
{
    return;
}




/******************************************************************************/
aw_local void __bd_release (struct aw_refcnt *p_ref)
{
    struct aw_blk_dev *p_dev;

    p_dev = AW_CONTAINER_OF(p_ref, struct aw_blk_dev, ref);

    //destroy cache

    AW_MUTEX_TERMINATE(p_dev->lock);

    p_dev->pfn_release(p_dev);
}




/******************************************************************************/
aw_local void __bd_evt_cb (struct aw_blk_dev *p_dev,
                           int                event)
{
    int                    i;
    aw_blk_dev_evt_fun_t   pfn;
    void                  *p_arg;
    aw_spinlock_isr_t      lock;

    aw_spinlock_isr_init(&lock, 0);

    for (i = 0; i < __g_bd_evt_cnt; i++) {
        aw_spinlock_isr_take(&lock);
        pfn   = __g_bd_evt_tab[i].pfn_cb;
        p_arg = __g_bd_evt_tab[i].p_arg;
        if (pfn) {
            __g_bd_evt_tab[i].refcnt++;
        }
        aw_spinlock_isr_give(&lock);

        if (pfn) {
            pfn(p_dev->name, event, p_arg);
            aw_spinlock_isr_take(&lock);
            __g_bd_evt_tab[i].refcnt--;
            if (__g_bd_evt_tab[i].refcnt == 0) {
                AW_SEMB_GIVE(__g_bd_evt_tab[i].semb);
            }
            aw_spinlock_isr_give(&lock);
        }
    }
}




/******************************************************************************/
aw_local void __wait_install (const char *name,
                              int         event,
                              void       *p_arg)
{
    if (event == AW_BD_EVT_INJECT) {
        struct __inject_evt_arg *p_evt_arg = (struct __inject_evt_arg *)p_arg;
        if (strcmp(p_evt_arg->name, name) == 0) {
            aw_semb_give(p_evt_arg->semb);
        }
    }
}




/******************************************************************************/
aw_err_t aw_blk_dev_lib_init (int blk_evt_cnt)
{
    int i;

    if (__g_bd_init) {
        return AW_OK;
    }

    if (AW_MUTEX_INIT(__g_bd_list_lock,
                      AW_SEM_Q_PRIORITY) == NULL) {
        return -AW_ENOMEM;
    }

    __g_bd_evt_tab = aw_mem_alloc(sizeof(*__g_bd_evt_tab) * blk_evt_cnt);
    if (__g_bd_evt_tab == NULL) {
        AW_MUTEX_TERMINATE(__g_bd_list_lock);
        return -AW_ENOMEM;
    }

    for (i = 0; i < blk_evt_cnt; i++) {
        /* 阻止内存里面的内容复位的过程中没清空*/
        memset(&__g_bd_evt_tab[i].semb,0,sizeof(__g_bd_evt_tab[i].semb));
        if (AW_SEMB_INIT(__g_bd_evt_tab[i].semb,
                         0,
                         AW_SEM_Q_PRIORITY) == NULL) {
            __BD_TRACE(("not enough SEMB for block event.\n"));
            blk_evt_cnt = i;
            break;
        }
        __g_bd_evt_tab[i].pfn_cb = NULL;
        __g_bd_evt_tab[i].p_arg  = NULL;
        __g_bd_evt_tab[i].refcnt = 0;
    }

    __g_bd_evt_cnt    = blk_evt_cnt;
    __gp_bd_list_head = __BD_LIST_TAIL;
    __g_bd_init       = AW_TRUE;

    return AW_OK;
}




/******************************************************************************/
aw_err_t aw_blk_dev_create (struct aw_blk_dev  *p_dev,
                            const char         *name)
{
    if ((p_dev == NULL) ||
        (name == NULL) ||
        (*name == '\0')) {
        return -AW_EINVAL;
    }

    if (strlen(name) > AW_BD_NAME_MAX) {
        return -AW_EINVAL;
    }

    if (AW_MUTEX_INIT(p_dev->lock, AW_SEM_Q_PRIORITY) == NULL) {
        return -AW_ENOMEM;
    }

    strncpy(p_dev->name, name, AW_BD_NAME_MAX);
    p_dev->status      = AW_BD_STA_EJECTED;
    p_dev->p_cache     = NULL;
    p_dev->pfn_ioctl   = __bd_def_ioctl;
    p_dev->pfn_areq    = __bd_def_areq;
    p_dev->pfn_release = __bd_def_release;
    p_dev->p_next      = NULL;
    p_dev->p_base      = NULL;

    aw_refcnt_init(&p_dev->ref);

    return AW_OK;
}




/******************************************************************************/
aw_err_t aw_blk_dev_destroy (struct aw_blk_dev *p_dev)
{
    aw_err_t  ret;
    int       i;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    ret = AW_MUTEX_LOCK(p_dev->lock, __MUTEX_LOCK_TIMEOUT); //TODO
    if (ret != AW_OK) {
        __BD_TRACE(("lock block device failed when destroy.\n"));
        return ret;
    }

    if (p_dev->status & AW_BD_STA_REMOVED) {
        /* had removed */
        AW_MUTEX_UNLOCK(p_dev->lock);
        return AW_OK;
    }

    /* mark device had removed */
    p_dev->status |= AW_BD_STA_REMOVED;

    /* eject device */
    aw_blk_dev_eject(p_dev); //todo

    AW_MUTEX_UNLOCK(p_dev->lock);

    /* release */
    aw_refcnt_put(&p_dev->ref, __bd_release);

    return AW_OK;
}




/******************************************************************************/
aw_err_t aw_blk_dev_inject (struct aw_blk_dev *p_dev)
{
    aw_err_t  ret;

    if ((p_dev == NULL) ||
        (p_dev->name == NULL) ||
        (*p_dev->name == '\0') ||
        (p_dev->p_next)) {
        return -AW_EINVAL;
    }

    ret = AW_MUTEX_LOCK(p_dev->lock, __MUTEX_LOCK_TIMEOUT);
    if (ret != AW_OK) {
        return ret;
    }

    if (!(p_dev->status & AW_BD_STA_EJECTED)) {
        /* already injected */
        ret = -AW_EEXIST;
        goto __exit;
    }

    /* try to initialize LIB */
    ret = aw_blk_dev_lib_init(__BD_DEF_EVT_CNT);
    if (ret != AW_OK) {
        goto __exit;
    }

    /* check if device had already existed */
    ret = __bd_list_add(p_dev);
    if (ret == AW_OK) {
        /* mark device injected */
        p_dev->status &= ~AW_BD_STA_EJECTED;
        __bd_evt_cb(p_dev, AW_BD_EVT_INJECT);//TODO if out of lock
    }

__exit:
    AW_MUTEX_UNLOCK(p_dev->lock);
    return ret;
}




/******************************************************************************/
aw_err_t aw_blk_dev_eject (struct aw_blk_dev *p_dev)
{
    aw_err_t ret;
    int      i;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    if (!__g_bd_init) {
        return -AW_ESRCH;
    }

    ret = AW_MUTEX_LOCK(p_dev->lock, __MUTEX_LOCK_TIMEOUT);
    if (ret != AW_OK) {
        __BD_TRACE(("lock block device failed when eject.\n"));
        return ret;
    }

    if (p_dev->status & AW_BD_STA_EJECTED) {
        /* already ejected */
        AW_MUTEX_UNLOCK(p_dev->lock);
        return AW_OK;
    }

    p_dev->status |= AW_BD_STA_EJECTED;

    if (p_dev->p_next == NULL) {
        AW_MUTEX_UNLOCK(p_dev->lock);
        return AW_OK;
    }

    /* remove it */
    __bd_list_del(p_dev);

    AW_MUTEX_UNLOCK(p_dev->lock);

    __bd_evt_cb(p_dev, AW_BD_EVT_EJECT);

    return AW_OK;
}








/******************************************************************************/
aw_err_t aw_blk_dev_event_register (aw_blk_dev_evt_fun_t  pfn,
                                    void                 *p_arg,
                                    int                  *p_idx)
{
    aw_spinlock_isr_t  lock;
    int                i;

    if (!__g_bd_init) {
        return -AW_ESRCH;
    }

    aw_spinlock_isr_init(&lock, 0);

    for (i = 0; i < __g_bd_evt_cnt; i++) {
        aw_spinlock_isr_take(&lock);
        if (__g_bd_evt_tab[i].refcnt) {
            aw_spinlock_isr_give(&lock);
            continue;
        }
        __g_bd_evt_tab[i].pfn_cb = pfn;
        __g_bd_evt_tab[i].p_arg  = p_arg;
        __g_bd_evt_tab[i].refcnt++;
        AW_SEMB_RESET(__g_bd_evt_tab[i].semb, 0);
        aw_spinlock_isr_give(&lock);
        if (p_idx) {
            *p_idx = i;
        }
        return AW_OK;
    }

    return -AW_ENOSPC;
}




/******************************************************************************/
aw_err_t aw_blk_dev_event_unregister (int idx)
{
    aw_spinlock_isr_t  lock;
    aw_err_t           ret;

    if (!__g_bd_init) {
        return -AW_ESRCH;
    }

    if (idx >= __g_bd_evt_cnt) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_init(&lock, 0);

    aw_spinlock_isr_take(&lock);
    __g_bd_evt_tab[idx].pfn_cb = NULL;
    __g_bd_evt_tab[idx].p_arg  = NULL;

    if (__g_bd_evt_tab[idx].refcnt) {
        __g_bd_evt_tab[idx].refcnt--;
    }
    aw_spinlock_isr_give(&lock);

    if (__g_bd_evt_tab[idx].refcnt) {
        ret = AW_SEMB_TAKE(__g_bd_evt_tab[idx].semb, AW_WAIT_FOREVER);
        if (ret != AW_OK) {
            /* TODO*/
        }
    }
    return AW_OK;
}




/******************************************************************************/
aw_err_t aw_blk_dev_wait_install (const char *name, int timeout)
{
    struct __inject_evt_arg  arg;
    aw_err_t                 ret;
    AW_SEMB_DECL(            semb);
    aw_semb_id_t             semb_id;
    int                      idx;

    if ((name == NULL) ||
        (*name == EOS)) {
        return -AW_EINVAL;
    }

    if (!__g_bd_init) {
        return -AW_ESRCH;
    }

    /* check if device had already existed */
    if (__bd_list_find(name, NULL)) {/* TODO LOCK ALL*/
        return AW_OK;
    }

    memset(&semb,0,sizeof(semb));
    semb_id = AW_SEMB_INIT(semb, 0, AW_SEM_Q_PRIORITY);
    if (semb_id == NULL) {
        return -AW_ENOMEM;
    }

    arg.name = name;
    arg.semb = semb_id;

    /* wait for device install */
    ret = aw_blk_dev_event_register(__wait_install, &arg, &idx);
    if (ret != AW_OK) {
        goto __exit;
    }

    ret = AW_SEMB_TAKE(semb, timeout);
    aw_blk_dev_event_unregister(idx);

__exit:
    AW_SEMB_TERMINATE(semb);

    return ret;
}




/******************************************************************************/
struct aw_blk_dev *aw_blk_dev_get (const char *name)
{
    struct aw_blk_dev *p_dev;

    if ((name == NULL) ||
        (*name == EOS)) {
        return NULL;
    }

    if (!__g_bd_init) {
        return NULL;
    }

    __bd_list_find(name, &p_dev);

    return p_dev;
}




/******************************************************************************/
aw_err_t aw_blk_dev_put (struct aw_blk_dev *p_dev)
{
    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    aw_refcnt_put(&p_dev->ref, __bd_release);

    return AW_OK;
}




/******************************************************************************/
aw_err_t aw_blk_dev_lock (struct aw_blk_dev *p_dev)
{
    aw_err_t ret;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    __BD_LOCK(p_dev, ret);
    return ret;
}




/******************************************************************************/
aw_err_t aw_blk_dev_unlock (struct aw_blk_dev *p_dev)
{
    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    return __BD_UNLOCK(p_dev);
}




/******************************************************************************/
aw_err_t aw_blk_dev_ioctl (struct aw_blk_dev *p_dev,
                           int                cmd,
                           void              *p_arg)
{
    aw_err_t  ret;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    /* lock device */
    __BD_LOCK(p_dev, ret);
    if (ret != AW_OK) {
        return ret;
    }

    ret = -AW_ENOTSUP;
    if (p_dev->p_cache) {
        ret = aw_blk_cache_ioctl(p_dev->p_cache, cmd, p_arg);
    }

    if (ret == -AW_ENOTSUP) {
        ret = p_dev->pfn_ioctl(p_dev, cmd, p_arg);
    }
    __BD_UNLOCK(p_dev);

    return ret;
}




/******************************************************************************/
aw_err_t aw_blk_dev_areq (struct aw_blk_dev *p_dev,
                          struct aw_blk_arp *p_arp)
{
    aw_err_t  ret = -AW_ENOTSUP;

    if ((p_dev == NULL) ||
        (p_arp == NULL)) {
        return -AW_EINVAL;
    }

    /* lock device */
    __BD_LOCK(p_dev, ret);
    if (ret != AW_OK) {
        return ret;
    }

    ret = -AW_ENOTSUP;
    if (p_dev->p_cache) {
        ret = aw_blk_cache_areq(p_dev->p_cache, p_arp);
    }

    if (ret == -AW_ENOTSUP) {
        ret = p_dev->pfn_areq(p_dev, p_arp);
    }

    __BD_UNLOCK(p_dev);

    return ret;
}




/******************************************************************************/
aw_err_t aw_blk_dev_name (struct aw_blk_dev *p_dev,
                          const char        *name)
{
    aw_err_t  ret;

    if ((p_dev == NULL) ||
        (name == NULL) ||
        (*name == EOS)) {
        return -AW_EINVAL;
    }

    /* lock device */
    __BD_LOCK(p_dev, ret);
    if (ret != AW_OK) {
        return ret;
    }

    strncpy(p_dev->name, name, AW_BD_NAME_MAX);

    __BD_UNLOCK(p_dev);

    return ret;
}




/******************************************************************************/
aw_err_t aw_blk_dev_cache_create (const char          *name,
                                  struct aw_blk_cache *p_cache,
                                  void                *p_buf,
                                  uint_t               size,
                                  uint_t               b_blks)
{
    struct aw_blk_dev *p_dev;
    aw_err_t           ret;

    p_dev = aw_blk_dev_get(name);
    if (p_dev == NULL) {
        return -AW_ENODEV;
    }

    /* lock device */
    __BD_LOCK(p_dev, ret);
    if (ret != AW_OK) {
        aw_blk_dev_put(p_dev);
        return ret;
    }

    ret = aw_blk_cache_init(p_cache,
                            p_dev,
                            p_buf,
                            size,
                            b_blks);
    __BD_UNLOCK(p_dev);

    if (ret != AW_OK) {
        aw_blk_dev_put(p_dev);
    }

    return ret;
}




/******************************************************************************/
aw_err_t aw_blk_dev_cache_destroy (struct aw_blk_cache *p_cache)
{
    aw_err_t  ret;

    if ((p_cache == NULL) ||
        (p_cache->p_bd == NULL)) {
        return -AW_EINVAL;
    }

    /* lock device */
    /* lock device */
    __BD_LOCK(p_cache->p_bd, ret);
    if (ret != AW_OK) {
        return ret;
    }

    aw_blk_cache_uninit(p_cache);

    __BD_UNLOCK(p_cache->p_bd);


    aw_blk_dev_put(p_cache->p_bd);

    return AW_OK;
}








