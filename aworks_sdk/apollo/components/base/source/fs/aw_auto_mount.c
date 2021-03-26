/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Automatic file system mount management.
 *
 * \internal
 * \par modification history:
 * - 1.00 20-09-30 vih, first implementation.
 * \endinternal
 */


#include "aworks.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "string.h"
#include "stdlib.h"


#include "io/sys/aw_stat.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "fs/aw_auto_mount.h"
#include "fs/aw_mount.h"
#include "fs/aw_blk_dev.h"

#include "mtd/aw_mtd.h"

/*****************************************************************************/
/* 宏定义 */
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
         AW_INFOF((fmt, ##__VA_ARGS__));\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)

#define PFX  "AUTO-MOUNT"

#define __MAKE_SYSFILE_NAME  "~systemmkiffd.reserved"
/*****************************************************************************/
extern int aw_mount_foreach (int (*pfn)(struct aw_mount_info *info, void *arg),
                             void *arg);
/*****************************************************************************/
struct __mount_info_find_struct {
    const char           *path; /* path of mount point or device */
    struct aw_mount_info *info; /* info found */
};

typedef struct {
    const char  *fs;
    void *     (*pfn_dev_get) (const char *p_devname);
    aw_bool_t  (*pfn_format_check) (aw_auto_mount_cfg_t  *p_cfg,
                                    aw_bool_t            *mounted);
    aw_err_t   (*pfn_format) (aw_auto_mount_cfg_t *p_cfg,
                              aw_bool_t           *mounted);
} fs_opr_t;

static aw_auto_mount_cfg_t *__gp_cfg_arr = NULL;
static int                  __g_blk_evt_id;

/*****************************************************************************/
static void * __mtd_dev_get (const char *p_devname)
{
    return (void *)aw_mtd_dev_find(p_devname);
}

static void * __blk_dev_get (const char *p_devname)
{
    return (void *)aw_blk_dev_get(p_devname);
}

/*****************************************************************************/
static aw_bool_t __format_check (aw_auto_mount_cfg_t *p_cfg, aw_bool_t *mounted)
{
    aw_err_t         ret;

    ret = aw_mount(p_cfg->mnt, p_cfg->dev, p_cfg->fs, p_cfg->mount_flags);
    if (ret != 0) {
        return AW_FALSE;
    }
    *mounted = 1;
    return AW_TRUE;
}
static aw_err_t __format (aw_auto_mount_cfg_t *p_cfg, aw_bool_t *mounted)
{
    struct aw_fs_format_arg  fmt = {"awdisk", 0, 1};    /* Hard format. */
    aw_err_t                 ret;

    aw_assert(*mounted == AW_FALSE);

    ret = aw_make_fs(p_cfg->dev, p_cfg->fs, &fmt);
    if (ret != AW_OK) {
        __LOG_PFX(1, "Hard format %s failed. (ret:%d)", p_cfg->dev, ret);
        return ret;
    }
    return AW_OK;
}
/*****************************************************************************/
static aw_bool_t __vfat_format_check (aw_auto_mount_cfg_t *p_cfg, aw_bool_t *mounted)
{
    aw_err_t         ret;

    ret = aw_mount(p_cfg->mnt, p_cfg->dev, p_cfg->fs, p_cfg->mount_flags);
    /* vfat 返回该错误则未格式化 */
    if (ret == -AW_ENOENT) {
        return AW_FALSE;
    } else if (ret < 0) {
        *mounted = 0;
    } else {
        *mounted = 1;
    }
    return AW_TRUE;
}
/*****************************************************************************/

static aw_bool_t __lffs_format_check (aw_auto_mount_cfg_t *p_cfg, aw_bool_t *mounted)
{
    aw_err_t         ret;
    struct aw_stat   file_stat;
    char             path[512];

    aw_assert(*mounted == AW_FALSE);
    ret = aw_mount(p_cfg->mnt, p_cfg->dev, p_cfg->fs, p_cfg->mount_flags);
    if (ret != 0) {
        return AW_FALSE;
    }
    *mounted = 1;

    ret = aw_snprintf(path, sizeof(path), "%s/%s", p_cfg->mnt,
                      __MAKE_SYSFILE_NAME);
    aw_assert(ret < sizeof(path) - 1);

    ret = aw_stat(path, &file_stat);
    if (ret < 0) {
        goto umount;
    }
    return AW_TRUE;

umount:
    *mounted = AW_FALSE;
    aw_umount(p_cfg->mnt, p_cfg->mount_flags);
    return AW_FALSE;
}

static aw_err_t __lffs_format (aw_auto_mount_cfg_t *p_cfg, aw_bool_t *mounted)
{
    struct aw_fs_format_arg  fmt = {"awdisk", 0, 1};    /* Hard format. */
    aw_err_t                 ret;
    char                     path[512];
    int                      fd = -1;

    aw_assert(*mounted == AW_FALSE);

    ret = aw_make_fs(p_cfg->dev, p_cfg->fs, &fmt);
    if (ret != AW_OK) {
        __LOG_PFX(1, "Hard format %s failed. (ret:%d)", p_cfg->dev, ret);
        return ret;
    }

    /*
     *  Create a system format flag file.
     */
    ret = aw_mount(p_cfg->mnt, p_cfg->dev, p_cfg->fs, p_cfg->mount_flags);
    if (ret != AW_OK) {
        __LOG_PFX(1, "Mount %s failed. (ret:%d)", p_cfg->dev, ret);
        return ret;
    }
    *mounted = AW_TRUE;

    ret = aw_snprintf(path, sizeof(path), "/%s/%s", p_cfg->mnt,
                      __MAKE_SYSFILE_NAME);
    aw_assert(ret < sizeof(path) - 1);
    fd = aw_create(path, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd <= 0) {
        __LOG_PFX(1, "Failed to create a system format flag file. "
                "(ret:%d, file:%s)", ret, path);
        goto umount;
    }
    aw_close(fd);

    __LOG_PFX(1, "Hard format %s successfully. (ret:%d)", p_cfg->dev, ret);
    return AW_OK;

umount:
    *mounted = AW_FALSE;
    aw_umount(p_cfg->mnt, p_cfg->mount_flags);
    return ret;
}

/*****************************************************************************/
static fs_opr_t __g_fopr_arr[] = {
        {"lffs",  __mtd_dev_get, __lffs_format_check, __lffs_format},
        {"yaffs", __mtd_dev_get, __format_check, __format},
        {"vfat",  __blk_dev_get, __vfat_format_check, __format},
        {NULL},
};

static fs_opr_t *__fs_opr_find (const char *fs)
{
    fs_opr_t *p_fopr;
    for (p_fopr = __g_fopr_arr; p_fopr->fs != NULL; p_fopr++) {
        if (strcmp(fs, p_fopr->fs) == 0) {
            aw_assert(p_fopr->pfn_dev_get);
            aw_assert(p_fopr->pfn_format_check);
            aw_assert(p_fopr->pfn_format);
            return p_fopr;
        }
    }
    return NULL;
}

/*****************************************************************************/
#if 0
static aw_auto_mount_cfg_t * __config_find_by_mnt (const char *mnt)
{
    aw_auto_mount_cfg_t *p_cfg;

    aw_assert(mnt);
    for (p_cfg = __gp_cfg_arr; p_cfg->mnt != NULL; p_cfg++) {
        if (strcmp(mnt, p_cfg->mnt) == 0) {
            return p_cfg;
        }
    }
    return NULL;
}
#endif

static aw_auto_mount_cfg_t * __config_find_by_dev (const char *dev)
{
    aw_auto_mount_cfg_t *p_cfg;
    aw_assert(dev);
    for (p_cfg = __gp_cfg_arr; p_cfg->dev != NULL; p_cfg++) {
        if (strcmp(dev, p_cfg->dev) == 0) {
            return p_cfg;
        }
    }
    return NULL;
}


static int __mount_info_find_helper (struct aw_mount_info *info, void *arg)
{
    struct __mount_info_find_struct *i = arg;

    if (!strcmp(info->mnt_point, i->path) || !strcmp(info->dev_name, i->path)) {
        i->info = info;
        return 1;
    }
    return 0;
}

static struct aw_mount_info *__mount_info_find (const char           *name)
{
    struct __mount_info_find_struct arg;
    AW_INT_CPU_LOCK_DECL(           lock);

    arg.path = name;
    arg.info = NULL;

    AW_INT_CPU_LOCK(lock);
    aw_mount_foreach(__mount_info_find_helper, &arg);
    AW_INT_CPU_UNLOCK(lock);

    return arg.info;
}

static aw_err_t __cfg_mount (aw_auto_mount_cfg_t *p_cfg, fs_opr_t *p_fopr)
{
    aw_err_t     ret;
    aw_bool_t    fmt_ok;
    aw_bool_t    mounted = AW_FALSE;
    aw_assert(p_cfg && p_fopr);

    /* Check to see if it has been mounted. */
    if (__mount_info_find(p_cfg->dev) || __mount_info_find(p_cfg->mnt)) {
        __LOG_PFX(1, "mount: %s or %s already registered\n",
                  p_cfg->mnt, p_cfg->dev);
        return -AW_EEXIST;
    }

    if (p_cfg->auto_format) {
        fmt_ok = p_fopr->pfn_format_check(p_cfg, &mounted);
        if (!fmt_ok) {
            ret = p_fopr->pfn_format(p_cfg, &mounted);
            if (ret != AW_OK) {
                goto exit;
            }
        }
    }

    if (!mounted) {
        ret = aw_mount(p_cfg->mnt, p_cfg->dev, p_cfg->fs, p_cfg->mount_flags);
    } else {
        ret = AW_OK;
    }

exit:
    if (ret != 0) {
        __LOG_PFX(1, "Mount %s failed. (ret:%d)", p_cfg->dev, ret);
        return ret;
    }
    __LOG_PFX(1, "Mount %s successfully.", p_cfg->dev);

    return AW_OK;
}

static void __blk_evt_cb (const char *name, int event, void *p_arg)
{
    aw_auto_mount_cfg_t *p_cfg;
    fs_opr_t            *p_fopr;

    if (event == AW_BD_EVT_INJECT) {
        struct aw_mount_info  *p_info;

        p_info = __mount_info_find(name);
        p_cfg  = __config_find_by_dev(name);
        if (p_cfg) {
            p_fopr = __fs_opr_find(p_cfg->fs);
        }
        if (!p_info && p_cfg && p_fopr) {
            __cfg_mount(p_cfg, p_fopr);
        }
    }

    if (event == AW_BD_EVT_EJECT) {
        __LOG_PFX(1, "block device %s removed\n", name);
        if (aw_umount(name, 0) == 0) {
            __LOG_PFX(1, "block device %s detached\n", name);
        }
    }
}


aw_err_t aw_auto_mount_tbl_add (aw_auto_mount_cfg_t *p_arr)
{
    aw_auto_mount_cfg_t *p_cfg;
    fs_opr_t            *p_fopr;
    void                *p_dev;
    AW_INT_CPU_LOCK_DECL( lock);

    aw_assert(p_arr);

    AW_INT_CPU_LOCK(lock);
    if (__gp_cfg_arr) {
        AW_INT_CPU_UNLOCK(lock);
        return -AW_EEXIST;
    }
    __gp_cfg_arr = p_arr;
    AW_INT_CPU_UNLOCK(lock);

    aw_blk_dev_event_register(__blk_evt_cb, NULL, &__g_blk_evt_id);

    for (p_cfg = __gp_cfg_arr; p_cfg->mnt != NULL; p_cfg++) {
        /* Check the parameters. */
        aw_assert(p_cfg->dev);
        aw_assert(p_cfg->mnt);
        aw_assert(p_cfg->fs);

        p_fopr = __fs_opr_find(p_cfg->fs);
        p_dev = p_fopr->pfn_dev_get(p_cfg->dev);

        /*
         * Check if the device exists and mount if it does.
         */
        if (p_dev) {
            __cfg_mount(p_cfg, p_fopr);
            continue;
        }
    }

    if (p_cfg->mnt == NULL) {
        aw_assert(p_cfg->dev == NULL);
        aw_assert(p_cfg->fs == NULL);
    }

    return AW_OK;
}

void aw_auto_mount_tbl_clear (void)
{
    AW_INT_CPU_LOCK_DECL( lock);
    AW_INT_CPU_LOCK(lock);
    __gp_cfg_arr = NULL;
    AW_INT_CPU_UNLOCK(lock);
    aw_blk_dev_event_unregister(__g_blk_evt_id);
}

