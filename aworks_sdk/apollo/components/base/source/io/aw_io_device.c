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
 * \brief I/O device
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-08  deo, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_list.h"
#include "aw_sem.h"
#include "aw_mem.h"
#include "io/aw_io_device.h"
#include "aw_refcnt.h"
#include "string.h"

/*******************************************************************************
  local global variables
*******************************************************************************/
AW_MUTEX_DECL_STATIC(       __g_iodev_mutex);
static struct aw_iodev     *__g_iodev_list;
static aw_bool_t               __g_iodev_init = AW_FALSE;




/******************************************************************************/
static void __iodev_list_lock (void)
{
    AW_MUTEX_LOCK(__g_iodev_mutex, AW_SEM_WAIT_FOREVER);
}




/******************************************************************************/
static void __iodev_list_unlock (void)
{
    AW_MUTEX_UNLOCK(__g_iodev_mutex);
}




/******************************************************************************/
static struct aw_iodev *__iodev_find (const char *name, int *len, aw_bool_t get)
{
    struct aw_iodev *p_tmp, *p_best;
    int              tmp, max;

    p_best = NULL;
    max    = 0;

    for (p_tmp = __g_iodev_list;
         p_tmp != NULL;
         p_tmp = p_tmp->p_next) {

        tmp = strlen(p_tmp->name);

        /* match name */
        if (strncmp(p_tmp->name, name, tmp) == 0) {
            char ch = *(name+strlen(p_tmp->name));
            if ((ch != '/') && (ch != '\0')) {
                continue;
            }
            if (tmp > max) {
                p_best = p_tmp;
                max    = tmp;
            }
        }
    }

    if (p_best) {
        if (len) {
            *len = max;
        }
        if (get) {
            /* get it */
            if ((p_best->removed) ||
                (aw_refcnt_get(&p_best->ref) != AW_OK)) {
                p_best = NULL;
            }
        }
    }

    return p_best;
}




/******************************************************************************/
static void __iodev_add (struct aw_iodev *p_dev)
{
    p_dev->p_next  = __g_iodev_list;
    __g_iodev_list = p_dev;
}




/******************************************************************************/
static void __iodev_del (struct aw_iodev *p_dev)
{
    struct aw_iodev *p_tmp;

    if (p_dev == __g_iodev_list) {
        __g_iodev_list = p_dev->p_next;
    } else {
        p_tmp = __g_iodev_list;

        while (p_tmp) {
            if (p_tmp->p_next == p_dev) {
                p_tmp->p_next = p_dev->p_next;
                p_dev->p_next  = NULL;
                break;
            }
            p_tmp = p_tmp->p_next;
        }
    }
}




/******************************************************************************/
static int __def_ent_create (struct aw_iodev *p_dev,
                             const char      *name,
                             int              mode)
{
    return -AW_ENOTSUP;
}




/******************************************************************************/
static int __def_ent_delete (struct aw_iodev *p_dev,
                             const char      *name)
{
    return -AW_ENOTSUP;
}




/******************************************************************************/
static void *__def_ent_open (struct aw_iodev *p_dev,
                             const char      *name,
                             int              oflag,
                             mode_t           mode,
                             int             *err)
{
    if (err) {
        *err = -AW_ENOTSUP;
    }
    return NULL;
}




/******************************************************************************/
static int __def_ent_close (void *p_ent)
{
    return -AW_ENOTSUP;
}




/******************************************************************************/
static int __def_ent_read (void    *p_ent,
                           void    *p_buf,
                           size_t   nbyte)
{
    return -AW_ENOTSUP;
}




/******************************************************************************/
static int __def_ent_write (void           *p_ent,
                            const void     *p_buf,
                            size_t          nbyte)
{
    return -AW_ENOTSUP;
}




/******************************************************************************/
static int __def_ent_ioctrl (void    *p_ent,
                             int      cmd,
                             void    *p_arg)
{
    return -AW_ENOTSUP;
}




/******************************************************************************/
static void __def_dev_release (struct aw_iodev *p_dev)
{
    return;
}




/******************************************************************************/
static void __iodev_release (struct aw_refcnt *p_ref)
{
    struct aw_iodev *p_dev;

    p_dev = AW_CONTAINER_OF(p_ref, struct aw_iodev, ref);

    AW_MUTEX_TERMINATE(p_dev->mutex);

    p_dev->pfn_dev_release(p_dev);
}




/******************************************************************************/
int aw_iodev_lib_init (void)
{
    int  ret;

    if (__g_iodev_init) {
        return AW_OK;
    }

    if (AW_MUTEX_INIT(__g_iodev_mutex,
                      AW_SEM_Q_PRIORITY) == NULL) {
        return -AW_ENOMEM;
    }

    __g_iodev_list = NULL;
    __g_iodev_init = AW_TRUE;

    return AW_OK;
}




/******************************************************************************/
int aw_iodev_create (struct aw_iodev *p_dev,
                     const char      *name)
{
    int                ret;
    struct aw_iodev   *p_match;
    char              *p_name;

    if ((p_dev == NULL) ||
        (name == NULL) ||
        (*name == EOS)) {
        return -AW_EINVAL;
    }

    ret = aw_iodev_lib_init();
    if (ret != AW_OK) {
        return ret;
    }

    __iodev_list_lock();

    p_match = __iodev_find(name, NULL, AW_FALSE);
    if ((p_match) && (strcmp(p_match->name, name) == 0)) {
        ret = -AW_EEXIST;
        goto __exit;
    }

    memset(&p_dev->mutex,0,sizeof(p_dev->mutex));
    if (AW_MUTEX_INIT(p_dev->mutex,
                      AW_SEM_Q_PRIORITY) == NULL) {
        ret = -AW_ENOMEM;
        goto __exit;
    }

    strncpy(p_dev->name, name, AW_IODEV_NAME_MAX);
    p_dev->removed         = AW_FALSE;
    p_dev->p_next          = NULL;
    p_dev->pfn_ent_create  = __def_ent_create;
    p_dev->pfn_ent_delete  = __def_ent_delete;
    p_dev->pfn_ent_open    = __def_ent_open;
    p_dev->pfn_ent_close   = __def_ent_close;
    p_dev->pfn_ent_read    = __def_ent_read;
    p_dev->pfn_ent_write   = __def_ent_write;
    p_dev->pfn_ent_ioctrl  = __def_ent_ioctrl;
    p_dev->pfn_dev_release = __def_dev_release;

    aw_refcnt_init(&p_dev->ref);

    __iodev_add(p_dev);

    ret = AW_OK;
__exit:
    __iodev_list_unlock();
    return ret;
}




/******************************************************************************/
int aw_iodev_destroy (struct aw_iodev *p_dev)
{
    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    if (!__g_iodev_init) {
        return -AW_ENOENT;
    }

    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);
    if (p_dev->removed) {
        AW_MUTEX_UNLOCK(p_dev->mutex);
        return AW_OK;
    }
    p_dev->removed = AW_TRUE;
    AW_MUTEX_UNLOCK(p_dev->mutex);

    __iodev_list_lock();
    __iodev_del(p_dev);
    __iodev_list_unlock();
    aw_refcnt_put(&p_dev->ref, __iodev_release);

    return AW_OK;
}




/******************************************************************************/
struct aw_iodev *aw_iodev_get (const char *name, const char **tail)
{
    int              len;
    struct aw_iodev *p_dev = NULL;

    if ((name == NULL) ||
        (*name == EOS)) {
        return NULL;
    }

    if (!__g_iodev_init) {
        return NULL;
    }

    __iodev_list_lock();
    p_dev = __iodev_find(name, &len, AW_TRUE);
    __iodev_list_unlock();

    if ((p_dev != NULL) && (tail != NULL)) {
        *tail = name + len;
    }

    return p_dev;
}




/******************************************************************************/
int aw_iodev_put (struct aw_iodev *p_dev)
{
    int  ret;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);

    ret = aw_refcnt_put(&p_dev->ref, __iodev_release);

    AW_MUTEX_UNLOCK(p_dev->mutex);

    return ret;
}




/******************************************************************************/
int aw_iodev_foreach (int (*pfn)(struct aw_iodev *p_dev, void *arg), void *arg)
{
    struct aw_iodev *p_dev;
    int              ret = 0;

    if (pfn == NULL) {
        return -AW_EINVAL;
    }

    __iodev_list_lock();

    for (p_dev = __g_iodev_list;
         p_dev != NULL;
         p_dev = p_dev->p_next) {
        if ((ret = pfn(p_dev, arg)) != 0) {
            break;
        }
    }

    __iodev_list_unlock();

    return ret;
}




/******************************************************************************/
int aw_iodev_ent_create (struct aw_iodev *p_dev,
                         const char      *name,
                         mode_t           mode)
{
    int  ret;

    if ((p_dev == NULL) ||
        (name == NULL) ||
        (*name == EOS)) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);
    ret = p_dev->pfn_ent_create(p_dev, name, mode);
    AW_MUTEX_UNLOCK(p_dev->mutex);

    return ret;
}




/******************************************************************************/
int aw_iodev_ent_delete (struct aw_iodev *p_dev,
                         const char      *name)
{
    int  ret;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);
    ret = p_dev->pfn_ent_delete(p_dev, name);
    AW_MUTEX_UNLOCK(p_dev->mutex);

    return ret;
}




/******************************************************************************/
void * aw_iodev_ent_open (struct aw_iodev *p_dev,
                          const char      *name,
                          int              flag,
                          mode_t           mode,
                          int             *err)
{
    void  *p_ent;

    if (p_dev == NULL) {
        return NULL;
    }

    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);
    p_ent = p_dev->pfn_ent_open(p_dev, name, flag, mode, err);
    AW_MUTEX_UNLOCK(p_dev->mutex);

    return p_ent;
}




/******************************************************************************/
int aw_iodev_ent_close (struct aw_iodev *p_dev,
                        void            *p_ent)
{
    int  ret;

    if ((p_dev == NULL) ||
        (p_ent == NULL)) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);
    ret = p_dev->pfn_ent_close(p_ent);
    AW_MUTEX_UNLOCK(p_dev->mutex);

    return ret;
}




/******************************************************************************/
int aw_iodev_ent_read (struct aw_iodev *p_dev,
                       void            *p_ent,
                       void            *p_buf,
                       size_t           nbyte)
{
    int  ret;

    if ((p_dev == NULL) ||
        (p_ent == NULL) ||
        (p_buf == NULL)) {
        return -AW_EINVAL;
    }

    if (nbyte == 0) {
        return 0;
    }

    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);
    ret = p_dev->pfn_ent_read(p_ent, p_buf, nbyte);
    AW_MUTEX_UNLOCK(p_dev->mutex);

    return ret;
}




/******************************************************************************/
int aw_iodev_ent_write (struct aw_iodev *p_dev,
                        void            *p_ent,
                        const void      *p_buf,
                        size_t           nbyte)
{
    int  ret;

    if ((p_dev == NULL) ||
        (p_ent == NULL) ||
        (p_buf == NULL)) {
        return -AW_EINVAL;
    }

    if (nbyte == 0) {
        return 0;
    }

    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);
    ret = p_dev->pfn_ent_write(p_ent, p_buf, nbyte);
    AW_MUTEX_UNLOCK(p_dev->mutex);

    return ret;
}




/******************************************************************************/
int aw_iodev_ent_ioctrl (struct aw_iodev *p_dev,
                         void            *p_ent,
                         int              cmd,
                         void            *p_arg)
{
    int  ret;

    if ((p_dev == NULL) ||
        (p_ent == NULL)) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);
    ret = p_dev->pfn_ent_ioctrl(p_ent, cmd, p_arg);
    AW_MUTEX_UNLOCK(p_dev->mutex);

    return ret;
}




