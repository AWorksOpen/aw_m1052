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
 * \brief device mount management.
 *
 * \internal
 * \par modification history:
 * - 1.02 14-01-04 orz, add auto mount support for block devices.
 * - 1.01 14-12-23 orz, remove block device pointer from mount info.
 * - 1.00 14-07-03 orz, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_vdebug.h"
#include "fs/aw_blk_dev.h"
#include "fs/aw_mount.h"
#include "aw_pool.h"
#include "aw_int.h"
#include "aw_compiler.h"
#include "string.h"
#include "errno.h"

/******************************************************************************/
struct __mount_info_find_struct {
    const char           *path; /* path of mount point or device */
    struct aw_mount_info *info; /* info found */
};

/******************************************************************************/
static struct aw_pool            __g_mnt_pool;      /* mount info pool */
static struct aw_mount_info     *__g_mnt_list;      /* list for device mounted */

/******************************************************************************/
static struct aw_mount_info *__mount_info_zalloc (void)
{
    struct aw_mount_info *minfo;

    if ((minfo = aw_pool_item_get(&__g_mnt_pool)) != NULL) {
        (void)memset(minfo, 0, sizeof(*minfo));
    }
    return minfo;
}

static void __mount_info_free (struct aw_mount_info *minfo)
{
    (void)aw_pool_item_return(&__g_mnt_pool, minfo);
}

/******************************************************************************/
static int __mount_info_foreach (
        struct aw_mount_info *info_list,
        int                 (*pfn)(struct aw_mount_info *info, void *arg),
        void                 *arg)
{
    struct aw_mount_info *info;
    int                   err = 0;

    for (info = info_list; NULL != info; info = info->next) {
        if (0 != (err = pfn(info, arg))) {
           break;
        }
    }
    return err;
}

/******************************************************************************/
static void __mount_info_add (struct aw_mount_info **list,
                              struct aw_mount_info  *info)
{
    AW_INT_CPU_LOCK_DECL(lock);

    AW_INT_CPU_LOCK(lock);
    info->next = *list;
   *list       = info;
    AW_INT_CPU_UNLOCK(lock);
}

/******************************************************************************/
void __mount_info_del (struct aw_mount_info **list, struct aw_mount_info *info)
{
    struct aw_mount_info *i, *l;
    AW_INT_CPU_LOCK_DECL( lock);

    AW_INT_CPU_LOCK(lock);
    if ((l = *list) == info) {
        *list = info->next; /* info is the first node */
    } else {
        for (i = l->next; NULL != i; l = i, i = i->next) {
            if (info == i) {
                l->next = i->next;
                break;
            }
        }
    }
    AW_INT_CPU_UNLOCK(lock);
}

/******************************************************************************/
static int __mount_info_find_helper (struct aw_mount_info *info, void *arg)
{
    struct __mount_info_find_struct *i = arg;

    if (!strcmp(info->mnt_point, i->path) || !strcmp(info->dev_name, i->path)) {
        i->info = info;
        return 1;
    }
    return 0;
}

/******************************************************************************/
static struct aw_mount_info *__mount_info_find (struct aw_mount_info *list,
                                                const char           *name)
{
    struct __mount_info_find_struct arg;
    AW_INT_CPU_LOCK_DECL(           lock);

    arg.path = name;
    arg.info = NULL;

    AW_INT_CPU_LOCK(lock);
    (void)__mount_info_foreach(list, __mount_info_find_helper, &arg);
    AW_INT_CPU_UNLOCK(lock);

    return arg.info;
}

/******************************************************************************/
static int __mount_info_name_init (struct aw_mount_info *info,
                                   const char           *mnt,
                                   const char           *dev,
                                   const char           *fs)
{
    /* check if the same mount point exist or not */
    if ((__mount_info_find(__g_mnt_list, mnt) != NULL) ||
        (__mount_info_find(__g_mnt_list, dev) != NULL)) {
        AW_ERRF(("mount: %s or %s already registered\n", mnt, dev));
        return -AW_EEXIST;
    }

    if ((info->fs = aw_fs_type_find(fs)) == NULL) {
        AW_ERRF(("mount: FS %s not find\n", fs));
        return -AW_EINVAL; /* file system not registered */
    }

#if (AW_CFG_MOUNT_POINT_NAME_MAX > 0)
    if (strlen(mnt) < sizeof(info->mnt_point)) { /* make sure not overflow*/
        (void)strcpy(info->mnt_point, mnt);
    } else {
        AW_ERRF(("mount: mount point name %s to long\n", mnt));
        return -AW_EINVAL;
    }
#else
    info->mnt_point = mnt;
#endif

#if (AW_CFG_MOUNT_DEV_NAME_MAX > 0)
    if (strlen(dev) < sizeof(info->dev_name)) { /* make sure not overflow */
        (void)strcpy(info->dev_name, dev);
    } else {
        AW_ERRF(("mount: device name %s to long\n", dev));
        return -AW_EINVAL;
    }
#else
    info->dev_name  = dev;
#endif

    return 0;
}

/******************************************************************************/
static void __blk_evt_cb (const char *name, int event, void *p_arg)
{

    if (event == AW_BD_EVT_EJECT) {
        AW_DBGF(("mount: block device %s removed\n", name));
        if (aw_umount(name, 0) == 0) {
            AW_DBGF(("mount: block device %s detached\n", name));
        }
    }
}

/******************************************************************************/
void aw_mount_init (struct aw_mount_info *p_info, unsigned int info_nbytes)
{
    (void)aw_pool_init(&__g_mnt_pool, p_info, info_nbytes, sizeof(*p_info));

    aw_blk_dev_event_register(__blk_evt_cb, NULL, NULL);
}

/******************************************************************************/
int aw_mount_foreach (int (*pfn)(struct aw_mount_info *info, void *arg),
                      void *arg)
{
    return __mount_info_foreach(__g_mnt_list, pfn, arg);
}

/******************************************************************************/
int aw_mount (const char *mnt, const char *dev, const char *fs, unsigned flags)
{
    int                   err;
    struct aw_mount_info *info;

    if (NULL == (info = __mount_info_zalloc())) {
        return -AW_ENOSPC;
    }

    if ((err = __mount_info_name_init(info, mnt, dev, fs)) != 0) {
        goto __info_free;
    }
    info->flags = flags;

    info->fs_data = info->fs->fs_dev_attach(info->mnt_point, info->dev_name);
    if (info->fs_data == NULL) {
        if (errno != AW_OK) {
            err = errno;
        } else {
            err = -AW_EPERM;
        }

        goto __info_free;
    }

    /* add to mount list */
  __mount_info_add(&__g_mnt_list, info);

__info_free:
    if (err != 0) {
        __mount_info_free(info);
    }
    return err;
}

/******************************************************************************/
int aw_umount (const char *path, unsigned flags)
{
    struct aw_mount_info *info;
    int                   err;

    (void)flags;

    /* find the mount point */
    info = __mount_info_find(__g_mnt_list, path);

    /* detach the mount device if possible */
    if (NULL == info) {
        return -AW_EINVAL; /* mount point not find */
    } else if (NULL == info->fs->fs_dev_detach) {
        return -AW_EPERM;
    }
    if ((err = info->fs->fs_dev_detach(info->fs_data)) == 0) {
      __mount_info_del(&__g_mnt_list, info);
      __mount_info_free(info);
    }
    return err;
}

/* end of file */
