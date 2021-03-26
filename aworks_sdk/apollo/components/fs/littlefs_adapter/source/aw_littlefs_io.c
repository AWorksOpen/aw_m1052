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

#include "AWorks.h"
#include "aw_vdebug.h"
#include "unistd.h"
#include "aw_ioctl.h"
#include "aw_time.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_utime.h"
#include "io/aw_dirent.h"
#include "io/sys/aw_stat.h"
#include "io/sys/aw_statvfs.h"
#include "io/aw_io_path.h"
#include "fs/aw_fs_type.h"
#include "mtd/aw_mtd.h"
#include "fs/aw_blk_dev.h"
#include "aw_assert.h"
#include "aw_pool.h"
#include "aw_littlefs.h"
#include <stdio.h>
#include <string.h>

static struct aw_pool    __g_vol_pool;
static struct aw_pool    __g_fil_pool;

typedef struct __littlefs_attr{
    mode_t             mode;
    time_t            ctime;
    time_t            mtime;
} __littlefs_attr_t;

enum attributes_type{
    __attr,
};

static void __littlefs_path_build (char *buf, const char *path);

static void *__littlefs_vol_attach(const char *vol_path, const char *dev_path);
static void *__littlefs_vol_detach(struct aw_littlefs_volume *p_vol);
static void *__littlefs_dev_attach (const char *vol_path, const char *dev_path);
static int __littlefs_dev_detach (void *volume);
static int __littlefs_mkfs (const char *dev_path, const struct aw_fs_format_arg *fmt_arg);

/*****************************************************************************/

static struct aw_littlefs_volume *__littlefs_volume_alloc(void)
{
    struct aw_littlefs_volume *p_vol;

    p_vol = aw_pool_item_get(&__g_vol_pool);
    if (p_vol) {
        (void)memset(p_vol, 0, sizeof(*p_vol));
    }

    return p_vol;
}

static void* __littlefs_volume_free(struct aw_littlefs_volume *p_vol)
{
    int ret;
    ret = aw_pool_item_return(&__g_vol_pool, p_vol);
    aw_assert(ret == 0);
    return NULL;
}

static struct aw_littlefs_file *__littlefs_file_alloc(void)
{
    struct aw_littlefs_file *p_file;

    p_file = aw_pool_item_get(&__g_fil_pool);
    if (p_file) {
        (void)memset(p_file, 0, sizeof(*p_file));
    }

    return p_file;
}

static int __littlefs_file_free(struct aw_littlefs_file *p_file)
{
    int ret;
    ret = aw_pool_item_return(&__g_fil_pool, p_file);
    return ret;
}

/*****************************************************************************/

static int __littlefs_info_set(struct aw_littlefs_file  *p_file,
                               mode_t                   *mode,
                               time_t                   *ctime,
                               time_t                   *mtime)
{
    int ret;
    __littlefs_attr_t attr;

    ret = lfs_getattr(&p_file->volume->lfs,
                      p_file->path,
                      __attr,
                      &attr,
                      sizeof(__littlefs_attr_t));
    if (ret < 0) {
        return ret;
    }

    if (mode != NULL) {
        attr.mode = *(uint32_t *)mode;
    }
    if (ctime != NULL) {
        attr.ctime = *(uint32_t *)ctime;
    }
    if (mtime != NULL) {
        attr.mtime = *(uint32_t *)mtime;
    }

    ret = lfs_setattr(&p_file->volume->lfs,
                      p_file->path,
                      __attr,
                      &attr,
                      sizeof(__littlefs_attr_t));
    if (ret < 0) {
        return ret;
    }

    return AW_OK;
}


/*****************************************************************************/

static int __littlefs_readdir(struct aw_littlefs_file *p_file, struct aw_dir *d)
{
    int ret;
    struct lfs_info info;

    if (p_file->flags != LFS_TYPE_DIR) {
        return -AW_ENOTDIR;
    }

    ret = lfs_dir_read(&p_file->volume->lfs, &p_file->dir, &info);
    if (ret < 0) {
        return ret;
    }
    memcpy(d->d_dirent.d_name, info.name, AW_IO_PATH_LEN_MAX + 1);
    if (*(char *)d->d_dirent.d_name == '\0') {
        return LFS_ERR_NOENT;
    }

    return LFS_ERR_OK;
}

static int __littlefs_flush(struct aw_littlefs_file *p_file)
{
    if (p_file->flags != LFS_TYPE_DIR) {
        return -AW_EISDIR;
    }
    return lfs_file_sync(&p_file->volume->lfs, &p_file->fil);
}

static int __littlefs_lseek(struct aw_littlefs_file *p_file,
                            struct aw_seek_struct   *par)
{
    if (p_file->flags == LFS_TYPE_DIR) {
        return -AW_EISDIR;
    }
    return lfs_file_seek(&p_file->volume->lfs, &p_file->fil, par->offset, par->whence);
}

static int __littlefs_truncate(struct aw_littlefs_file *p_file, size_t length)
{
    if (p_file->flags == LFS_TYPE_DIR) {
        return -AW_ENOTDIR;
    }
    return lfs_file_truncate(&p_file->volume->lfs, &p_file->fil, length);
}

static int __littlefs_utime(struct aw_littlefs_file *p_file,
                            struct aw_utimbuf       *p_utime)
{
    return __littlefs_info_set(p_file, NULL, NULL, &p_utime->modtime);
}

static int __littlefs_chmod(struct aw_littlefs_file *p_file,
                            mode_t mode)
{
    return __littlefs_info_set(p_file, &mode, NULL, NULL);
}

static int __littlefs_rename(struct aw_littlefs_file *p_file,
                             struct rename_struct    *r)
{
    char old_path[LFS_NAME_MAX];
    char new_path[LFS_NAME_MAX];

    __littlefs_path_build(new_path, r->new_name);
    __littlefs_path_build(old_path, r->old_name);

    return lfs_rename(&p_file->volume->lfs, r->old_name, r->new_name);
}

static int __littlefs_stat(struct aw_littlefs_file *p_file,
                           struct aw_stat          *p_stat)
{
    int ret;
    struct lfs_info info;
    __littlefs_attr_t attr;

    ret = lfs_stat(&p_file->volume->lfs, p_file->path, &info);
    if (ret < 0) {
        return ret;
    }

    ret = lfs_getattr(&p_file->volume->lfs,
                      p_file->path,
                      __attr,
                      &attr,
                      sizeof(__littlefs_attr_t));
    if (ret < 0) {
        p_stat->st_atim.tv_sec  = 0;
        p_stat->st_mtim.tv_sec  = 0;
        p_stat->st_ctim.tv_sec  = 0;
    } else {
        p_stat->st_atim.tv_sec  = attr.mtime;
        p_stat->st_mtim.tv_sec  = attr.mtime;
        p_stat->st_ctim.tv_sec  = attr.ctime;
    }

    p_stat->st_dev          = 0;
    p_stat->st_ino          = 0;
    p_stat->st_nlink        = 0;
    p_stat->st_uid          = 0;
    p_stat->st_gid          = 0;
    p_stat->st_rdev         = 0;
    p_stat->st_size         = p_file->fil.ctz.size;
    p_stat->st_blksize      = p_file->volume->config.prog_size;
    p_stat->st_blocks       = 0;
    p_stat->st_atim.tv_nsec = 0;
    p_stat->st_mtim.tv_nsec = 0;
    p_stat->st_ctim.tv_nsec = 0;
    p_stat->st_mode         = ((info.type == LFS_TYPE_DIR) ? S_IFDIR : S_IFREG) ;

    return LFS_ERR_OK;
}

static int __littlefs_statvfs(struct aw_littlefs_file *p_file, struct aw_statvfs *buf)
{
    struct aw_statvfs *param = (struct aw_statvfs *)buf;
    struct aw_littlefs_volume *p_vol = (struct aw_littlefs_volume *)p_file->volume;

    param->f_bsize   = p_vol->config.block_size;
    param->f_frsize  = p_vol->config.prog_size;
    param->f_blocks  = p_vol->config.block_count;
    param->f_bfree   = 0;
    param->f_bavail  = 0;
    param->f_files   = 0;
    param->f_ffree   = 0;
    param->f_favail  = 0;
    param->f_fsid    = 0;
    param->f_flag    = 0;
    param->f_namemax = LFS_NAME_MAX;

    return LFS_ERR_OK;
}




/*****************************************************************************/


static inline void __littlefs_path_build (char *buf, const char *path)
{
    snprintf(buf, LFS_NAME_MAX, "%s", path);
}

static void *__littlefs_open(struct aw_iodev *p_dev,
                           const char      *name,
                           int              oflag,
                           mode_t           mode,
                           int             *err)
{
    struct aw_littlefs_volume *p_vol = (struct aw_littlefs_volume *)p_dev;
    struct aw_littlefs_file   *p_fil = NULL;
    int flag = 0;
    int ret;
    uint8_t path_buf[LFS_NAME_MAX];
    struct lfs_info info;

    __littlefs_path_build((char *)path_buf, name);

    p_fil = __littlefs_file_alloc();
    if (p_fil == NULL) {
        return NULL;
    }

    memcpy((char *)&p_fil->path, path_buf, LFS_NAME_MAX);
    p_fil->volume = p_vol;

    if (!(oflag & O_CREAT)) {
        ret = lfs_stat(&p_vol->lfs, (const char *)path_buf, &info);
        if (ret < 0) {
            AW_ERRF(("littlefs: lfs_stat failed: %d\n", ret));
            goto __free_exit;
        }
        if (info.type == LFS_TYPE_DIR) {
            oflag |= O_DIRECTORY;
        }
    } else {
        struct aw_utimbuf  create_time;
        aw_tm_t tv;

        aw_tm_get(&tv);
        aw_tm_to_time(&tv, &create_time.actime);
        __littlefs_utime(p_fil, &create_time);
    }

    if (oflag & O_DIRECTORY) {
        if (oflag & O_CREAT) {
            ret = lfs_mkdir(&p_vol->lfs, (const char *)path_buf);
        } else {
            ret = lfs_dir_open(&p_vol->lfs, &p_fil->dir, (const char *)path_buf);
        }
        p_fil->flags = LFS_TYPE_DIR;
    } else {
        flag |= (oflag & O_RDONLY) ? LFS_O_RDONLY : 0;
        flag |= (oflag & O_WRONLY) ? LFS_O_WRONLY : 0;
        flag |= (oflag & O_RDWR)   ? LFS_O_RDWR   : 0;
        flag |= (oflag & O_APPEND) ? LFS_O_APPEND : 0;
        flag |= (oflag & O_EXCL)   ? LFS_O_EXCL   : 0;
        flag |= (oflag & O_CREAT)  ? LFS_O_CREAT  : 0;
        flag |= (oflag & O_TRUNC)  ? LFS_O_TRUNC  : 0;
        ret = lfs_file_open(&p_vol->lfs, &p_fil->fil, (const char *)path_buf, flag);
        p_fil->flags = LFS_TYPE_REG;
    }

    if (ret < 0) {
        AW_ERRF(("littlefs: lfs_file_open failed: %d\n", ret));
        goto __free_exit;
    }

    return p_fil;
__free_exit:
    __littlefs_file_free(p_fil);
    return NULL;
}

static int __littlefs_write(void        *pfd,
                            const void  *p_buf,
                            size_t       nbyte)
{
    struct aw_littlefs_file   *p_fil = pfd;

    if (p_fil->flags == LFS_TYPE_DIR) {
        return -AW_EISDIR;
    }

    return lfs_file_write(&p_fil->volume->lfs, &p_fil->fil, p_buf, nbyte);
}

static int __littlefs_read(void   *pfd,
                           void   *p_buf,
                           size_t  nbyte)
{
    struct aw_littlefs_file *p_fil = pfd;

    if (p_fil->flags == LFS_TYPE_DIR) {
        return -AW_EISDIR;
    }

    return lfs_file_read(&p_fil->volume->lfs, &p_fil->fil, p_buf, nbyte);
}

static int __littlefs_ioctrl(void    *pfd,
                             int      cmd,
                             void    *p_arg)
{
    struct aw_littlefs_file *fil = pfd;
    int ret;

    switch(cmd) {
    case AW_FIOFLUSH:
        ret = __littlefs_flush(fil);
        break;
    case AW_FIOWHERE:
        break;
#if 0
    case AW_FIONREAD:
        break;
#endif
    case AW_FIOSEEK:
        ret = __littlefs_lseek(fil, (struct aw_seek_struct *)p_arg);
        break;
    case AW_FIOTRUNC:
        ret = __littlefs_truncate(fil, *((off_t *)p_arg));
        break;
    case AW_FIOUTIME:
        ret = __littlefs_utime(fil, (struct aw_utimbuf*)p_arg);
        break;
    case AW_FIOCHMOD:
        ret = __littlefs_chmod(fil, *(mode_t *)p_arg);
        break;
    case AW_FIOFSTATFSGET:
        ret = __littlefs_statvfs(fil, (struct aw_statvfs*)p_arg);
        break;
    case AW_FIOREADDIR:
        ret = __littlefs_readdir(fil, (struct aw_dir*)p_arg);
        break;
    case AW_FIORENAME:
        ret = __littlefs_rename(fil, (struct rename_struct*)p_arg);
        break;
    case AW_FIOSTATGET:
        ret = __littlefs_stat(fil, (struct aw_stat*)p_arg);
        break;
    default:
        ret = -AW_ENOSYS;
        break;
    }

    return ret;
}

static int __littlefs_close(void *pfd)
{
    struct aw_littlefs_file *p_fil = pfd;
    int ret;

    if (p_fil->flags == LFS_TYPE_DIR) {
        ret = lfs_dir_close(&p_fil->volume->lfs, &p_fil->dir);
    } else {
        ret = lfs_file_close(&p_fil->volume->lfs, &p_fil->fil);
    }
    if (ret != LFS_ERR_OK) {
        AW_ERRF(("littlefs: close %s failed\n", p_fil->path));
    }

    return __littlefs_file_free(p_fil);
}

static int __littlefs_remove(struct aw_iodev *p_dev,
                             const char      *name)
{
    struct aw_littlefs_volume *p_vol = (struct aw_littlefs_volume *)p_dev;
    uint8_t path_buf[LFS_NAME_MAX];

    __littlefs_path_build((char *)path_buf, name);

    return lfs_remove(&p_vol->lfs, (const char *)path_buf);
}


static void __littlefs_release(struct aw_iodev *p_dev)
{
    struct aw_littlefs_volume *p_vol = (struct aw_littlefs_volume *)p_dev;

    if (__littlefs_vol_detach(p_vol) != NULL ){
        AW_ERRF(("littlefs: volume detach failed\n"));
    }
}


/*****************************************************************************/


static void *__littlefs_vol_attach(const char *vol_path, const char *dev_path)
{
    struct aw_littlefs_volume *p_vol;

    /* get a lettlefs volume descriptor and register the volume */
    p_vol = __littlefs_volume_alloc();
    if (p_vol == NULL) {
        AW_ERRF(("littlefs: alloc volume for %s failed\n", vol_path));
        goto __exit;
    }

    /* find the MTD device and do initializing */
    if ((p_vol->blk_dev = aw_mtd_dev_find(dev_path)) != NULL) {
        aw_littlefs_mtd_init(p_vol);
    } else {
        AW_ERRF(("littlefs: %s not find\n", dev_path));
        goto __vol_free;
    }

    memcpy(p_vol->mpoint, vol_path, strlen(vol_path) + 1);
    goto __exit;

__vol_free:
    p_vol = __littlefs_vol_detach(p_vol);
__exit:
    return p_vol;
}

static void *__littlefs_vol_detach(struct aw_littlefs_volume *p_vol)
{
    aw_littlefs_mtd_destory(p_vol);

    return __littlefs_volume_free(p_vol);
}

static void *__littlefs_dev_attach (const char *vol_path, const char *dev_path)
{
    struct aw_littlefs_volume *p_vol;
    int ret;

    if((p_vol = __littlefs_vol_attach(vol_path, dev_path)) == NULL) {
        goto __exit;
    }

    /* mount lettlefs to I/O system */
    if (lfs_mount(&p_vol->lfs, &p_vol->config) != LFS_ERR_OK){
        AW_ERRF(("littlefs: mount %s failed\n", vol_path));
        goto __vol_detach;
    }

    /* add the littlefs volume to I/O system */
    ret = aw_iodev_create(&p_vol->io_dev, vol_path);
    if (ret != AW_OK) {
        AW_ERRF(("awlffs: add volume %s to I/O failed\n", vol_path));
        goto __unmount;
    }

    p_vol->io_dev.pfn_ent_open    = __littlefs_open;
    p_vol->io_dev.pfn_ent_write   = __littlefs_write;
    p_vol->io_dev.pfn_ent_read    = __littlefs_read;
    p_vol->io_dev.pfn_ent_ioctrl  = __littlefs_ioctrl;
    p_vol->io_dev.pfn_ent_close   = __littlefs_close;
    p_vol->io_dev.pfn_ent_delete  = __littlefs_remove;
    p_vol->io_dev.pfn_dev_release = __littlefs_release;
    goto __exit;

__unmount:
    lfs_unmount(&p_vol->lfs);
__vol_detach:
    p_vol = __littlefs_vol_detach(p_vol);
__exit:
    return p_vol;
}

static int __littlefs_dev_detach (void *volume)
{
    struct aw_littlefs_volume *p_vol = volume;
    int ret;

    /* unmount lettlefs to I/O system */
    if (lfs_unmount(&p_vol->lfs) != LFS_ERR_OK){
        AW_ERRF(("littlefs: unmount failed\n"));
        return AW_ERROR;
    }

    ret = aw_iodev_destroy(&p_vol->io_dev);
    if (ret != AW_OK) {
        AW_ERRF(("delete volume from I/O system failed %d.\n", ret));
    }

    return ret;
}

static int __littlefs_mkfs (const char                    *dev_path,
                            const struct aw_fs_format_arg *fmt_arg)
{
    struct aw_littlefs_volume *p_vol;
    int ret;

    if((p_vol = __littlefs_vol_attach("/littlefs", dev_path)) == NULL) {
        goto __exit;
    }

//    __littlefs_vol_erase(p_vol);

    /* mount lettlefs to I/O system */
    if ((ret = lfs_format(&p_vol->lfs, &p_vol->config)) != LFS_ERR_OK) {
        AW_ERRF(("littlefs: mount %s failed\n", fmt_arg->vol_name));
    }

    __littlefs_dev_detach(p_vol);
__exit:
    return ret;
}

aw_err_t aw_littlefs_init(void         *p_vol_mem,
                          unsigned int  vmem_size,
                          void         *p_fil_mem,
                          unsigned int  fmem_size)
{
    static struct aw_fs_type fs;

    if (aw_pool_init(&__g_vol_pool,
                     p_vol_mem,
                     vmem_size,
                     sizeof(struct aw_littlefs_volume)) == NULL) {
        return -AW_ENOENT;
    }

    if (aw_pool_init(&__g_fil_pool,
                     p_fil_mem,
                     fmem_size,
                     sizeof(struct aw_littlefs_file)) == NULL) {
        return -AW_ENOENT;
    }

    /* register littlefs file system */
    fs.fs_dev_attach = __littlefs_dev_attach;
    fs.fs_dev_detach = __littlefs_dev_detach;
    fs.fs_make_fs    = __littlefs_mkfs;
    fs.fs_name       = "littlefs";

    return aw_fs_type_register(&fs);
}

