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
 * \brief YAFFS IO adapter
 *
 * \internal
 * \par modification history:
 * - 1.00 2016-05-05  cyl, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_pool.h"
#include "aw_assert.h"

#include "string.h"

#include "aw_ioctl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_statvfs.h"
#include "fs/aw_fs_type.h"
#include "io/aw_dirent.h"

#include "aw_yaffs.h"
#include "yaffs_guts.h"



/**
 * \brief pool management structure
 */
static struct {
    struct aw_pool vol; /**< \brief volume */
    struct aw_pool fil; /**< \brief file descriptor */
} __g_yaffs_pool;

/*************** yaffs volume pool manage interfaces **************************/


/**
 * \brief yaffs volume memory alloc
 * \param[in] none
 * \return the pointer of yaffs volume memory
 */
static struct aw_yaffs_volume *__yaffs_vol_alloc (void)
{
    struct aw_yaffs_volume *vol = aw_pool_item_get(&__g_yaffs_pool.vol);

    if (NULL != vol) {
        (void)memset(vol, 0, sizeof(*vol));
    }

    return vol;
}


/**
 * \brief yaffs volume memory free
 * \param[in] vol : the pointer of yaffs volume memory
 * \retval     NULL : operate successful
 * \retval not NULL : operate fail
 */
aw_static_inline void *__yaffs_vol_free (struct aw_yaffs_volume *vol)
{
    int err;

    err = aw_pool_item_return(&__g_yaffs_pool.vol, vol);
    aw_assert(0 == err);

    return 0 == err ? NULL : vol;
}


/**
 * \brief yaffs file descriptor alloc
 * \param[in] none
 * \return the pointer of yaffs file descriptor
 */
static struct aw_yaffs_file *__yaffs_fil_alloc (void)
{
    struct aw_yaffs_file *fil = aw_pool_item_get(&__g_yaffs_pool.fil);

    if (NULL != fil) {
        (void)memset(fil, 0, sizeof(*fil));
    }

    return fil;
}

/**
 * \brief yaffs file descriptor free
 * \param[in] fil : the pointer of file descriptor
 * \retval     NULL : operate successful
 * \retval not NULL : operate fail
 */
static void *__yaffs_fil_free (struct aw_yaffs_file *fil)
{
    int err = aw_pool_item_return(&__g_yaffs_pool.fil, fil);

    aw_assert(0 == err);

    return 0 == err ? NULL : fil;
}


/**
 * \brief get yaffs volume from io device
 * \param[in] io_dev : the pointer of io device
 * \return the pointer of yaffs volume
 */
static struct aw_yaffs_volume *__yaffs_vol_get_from_io_dev (
                  struct aw_iodev *io_dev)
{
    return AW_CONTAINER_OF(io_dev, struct aw_yaffs_volume, io_dev);
}


/*********************************IO dirver************************************/

/**
 * \brief build yaffs path
 *
 * \param[out] buf    : for saving the yaffs path built
 * \param[in]  mpoint : the mount point
 * \param[in]  path   : the path
 */
static void __yaffs_path_build (char *buf, const char *mpoint, const char *path)
{
    unsigned int n = strlen(mpoint);

    (void)strcpy(buf, mpoint);
    if ('\0' != *path) {
        (void)strncpy(&buf[n], path, YAFFS_MAX_NAME_LENGTH - n);
    }
}


/**
 * \brief remove file or directory
 *
 * \param[in] dev  : the pointer of io device
 * \param[in] path : the path
 *
 * \retval AW_OK    : operate successful
 * \retval AW_ERROR : operate fail
 * \retval else     : the errno of io operation
 */
static int __yaffs_remove (struct aw_iodev *dev, const char *path)
{
    int ret = YAFFS_OK;
    struct yaffs_stat s;

    if (YAFFS_OK == yaffs_lstat(path, &s)) {
        if (S_IFREG == (s.st_mode & S_IFMT)) {
            yaffs_unlink(path);
        } else if (S_IFDIR == (s.st_mode & S_IFMT)) {
            yaffs_rmdir(path);
        }
    }

    ret = yaffs_get_error();

    if (YAFFS_OK == ret) {
        ret = AW_OK;
    } else if (YAFFS_FAIL == ret) {
        ret = AW_ERROR;
    } else {
        ret = -ret;
    }

    return ret;
}

/**
 * \brief open file or directory
 *
 * \param[in] dev   : the pointer of io device
 * \param[in] path  : the path
 * \param[in] oflag : the flag of openning
 * \param[in] mode  : the mode of openning
 *
 * \return the pointer of file descriptor
 */
static void *__yaffs_open (struct aw_iodev  *dev,
                           const char       *path,
                           int               oflag,
                           mode_t            mode,
                           int              *err)

{
    struct aw_yaffs_file   *fil = NULL;
    struct aw_yaffs_volume *vol = NULL;
    int                     ret;

    char ypath[YAFFS_MAX_NAME_LENGTH + 1];

    fil = __yaffs_fil_alloc();

    vol = __yaffs_vol_get_from_io_dev(dev);

    if (NULL != fil) {
        __yaffs_path_build(ypath, vol->yfs_dev.param.name, path);

        if ('\0' == *path) {
            oflag |= O_DIRECTORY;
        }

        fil->volume = vol;
        fil->oflags = oflag;

        if (O_DIRECTORY & oflag) {
            fil->dir = yaffs_opendir(ypath);
            if (!fil->dir) {
                int ret = yaffs_mkdir(ypath, mode);
                if (0 == ret) {
                    fil->dir = yaffs_opendir(ypath);
                }
            }
        } else {
            fil->fdesc = yaffs_open(ypath, oflag, mode);
            if (fil->fdesc < 0) {
                *err = yaffsfs_GetLastError();
                return 0;
            }
        }

        strcpy(fil->path, ypath);
    }

    *err = AW_OK;
    return fil;
}

/**
 * \brief close file or directory
 * \param[in] pfd : the pointer of file descriptor
 * \retval AW_OK    : operate successful
 * \retval AW_ERROR : operate fail
 * \retval else     : the errno of io operation
 */
static int __yaffs_close (void *pfd)
{
    int ret = YAFFS_OK;
    struct aw_yaffs_file *fil = (struct aw_yaffs_file *)pfd;

    if (O_DIRECTORY & fil->oflags) {
        ret = yaffs_closedir(fil->dir);
    } else {
        ret = yaffs_close(fil->fdesc);
    }

    if (ret > 0) {
        ret = -ret;
    }

    fil = __yaffs_fil_free(fil);
    if (NULL != fil) {
        AW_ERRF(("yaffsio: fail to free file descriptor\n"));
        ret = AW_ERROR;
    }

    return ret;
}

/**
 * \brief read file
 *
 * \param[in]  pfd    : the pointer of file descriptor
 * \param[out] buf    : for saving the data you wanna read
 * \param[in]  nbytes : the bytes you wanna read
 *
 * \return the length you actually read
 */
static ssize_t __yaffs_read (void *pfd, void *buf, size_t nbyte)
{
    int ret = YAFFS_OK;
    ssize_t                len;
    struct aw_yaffs_file *fil = (struct aw_yaffs_file *)pfd;

    if (fil->oflags & O_DIRECTORY) {
        return -EISDIR; /* does not accesses directory as regular file */
    }

    if ((len = yaffs_read(fil->fdesc, buf, nbyte)) < 0) {
        ret = yaffs_get_error();
        if (YAFFS_OK == ret) {
            ret = AW_OK;
        } else if (YAFFS_FAIL == ret) {
            ret = AW_ERROR;
        } else {
            ret = -ret;
        }
        return ret;
    }

    return len;
}

/**
 * \brief write file
 *
 * \param[in]  pfd    : the pointer of file descriptor
 * \param[out] buf    : the pointer of data you wanna write
 * \param[in]  nbytes : the bytes you wanna write
 *
 * \return the length you actually write
 */
static ssize_t __yaffs_write (void *pfd, const void *buf, size_t nbyte)
{
    int ret = YAFFS_OK;
    ssize_t                len;
    struct aw_yaffs_file *fil = (struct aw_yaffs_file *)pfd;

    if (fil->oflags & O_DIRECTORY) {
        return -EISDIR; /* does not accesses directory as regular file */
    }

    if ((len = yaffs_write(fil->fdesc, buf, nbyte)) < 0) {
        ret = yaffs_get_error();
        if (YAFFS_OK == ret) {
            ret = AW_OK;
        } else if (YAFFS_FAIL == ret) {
            ret = AW_ERROR;
        } else {
            ret = -ret;
        }
        return ret;
    }

    return len;
}

/**
 * \brief io ctrl
 *
 * \param[in] pfd : the pointer of file descriptor
 * \param[in] cmd : command
 * \param[in] arg : the argument of command
 *
 * \return errno
 */
static int __yaffs_ioctl (void *pfd, int cmd, void *arg)
{
    int ret = YAFFS_OK;
    struct aw_yaffs_file *fil = (struct aw_yaffs_file *)pfd;

    switch (cmd) {

    case AW_FIOFLUSH: {
        ret = yaffs_flush(fil->fdesc);
        break;
    }

    case AW_FIODISKFORMAT: {
        ret = yaffs_format(fil->volume->yfs_dev.param.name, 0, 0, 0);
        break;
    }

    case AW_FIOSEEK: {
        struct aw_seek_struct *param = (struct aw_seek_struct *)arg;

        ret = yaffs_lseek(fil->fdesc, param->offset, param->whence);
        if (ret >= 0) {
            return ret;
        } else {
            ret = YAFFS_FAIL;
        }
        break;
    }

    case AW_FIORENAME: {
        struct rename_struct *param = (struct rename_struct *)arg;
        char old_name[100];
        char new_name[100];

        sprintf(old_name, "%s%s", fil->volume->io_dev.name, param->old_name);
        sprintf(new_name, "%s%s", fil->volume->io_dev.name, param->new_name);

        if (yaffs_rename(old_name, new_name) == 0) {
/* if (yaffs_rename(param->old_name, param->new_name) == 0) { */
            ret = YAFFS_OK;
        } else {
            ret = YAFFS_FAIL;
        }

        break;
    }

    case AW_FIOSYNC: {
        ret = yaffs_fsync(fil->fdesc);
        break;
    }

    case AW_FIONFREE: {
        *(Y_LOFF_T *)arg = yaffs_freespace(fil->volume->yfs_dev.param.name);
        ret = YAFFS_OK;
        break;
    }

    case AW_FIOMKDIR: {
        break;
    }

    case AW_FIORMDIR: {
        ret = yaffs_rmdir((const char *)arg);
        break;
    }

    case AW_FIOTRUNC: {
        ret = yaffs_ftruncate(fil->fdesc, *(Y_LOFF_T *)arg);
        break;
    }

    case AW_FIOUTIME: {
        struct yaffs_utimbuf *param = (struct yaffs_utimbuf *)arg;
        ret = yaffs_utime(fil->path, param);
        break;
    }

    case AW_FIOCHMOD: {
        ret = yaffs_chmod(fil->path, (mode_t)arg);
        break;
    }

    case AW_FIOFSTATFSGET: {
        Y_LOFF_T frees = 0;
        struct aw_statvfs *param = (struct aw_statvfs *)arg;

        param->f_bsize = fil->volume->yfs_dev.param.chunks_per_block *
                         fil->volume->yfs_dev.param.total_bytes_per_chunk;
        param->f_frsize = param->f_bsize;
        param->f_blocks = fil->volume->yfs_dev.param.end_block -
                          fil->volume->yfs_dev.param.start_block + 1;

        frees = yaffs_freespace(fil->volume->yfs_dev.param.name);
        param->f_bfree = frees / param->f_bsize;
        param->f_bavail = param->f_blocks - param->f_bfree;

        param->f_files   = 0;
        param->f_ffree   = 0;
        param->f_favail  = 0;
        param->f_flag    = 0;
        param->f_fsid    = 0;
        param->f_namemax = YAFFS_MAX_NAME_LENGTH;

        ret = YAFFS_OK;
        break;
    }

    case AW_FIOLINK: {
        break;
    }

    case AW_FIOUNLINK: {
        ret = yaffs_unlink(fil->path);
        if (0 == ret) {
            ret = YAFFS_OK;
        } else {
            ret = YAFFS_FAIL;
        }
        break;
    }

    case AW_FIOSTATGET: {
        struct aw_stat *param = (struct aw_stat *)arg;
        struct yaffs_stat s;
        ret = yaffs_stat(fil->path, &s);
        if (0 == ret) {
            param->st_dev     = s.st_dev;
            param->st_ino     = s.st_ino;
            param->st_mode    = s.st_mode;
            param->st_nlink   = s.st_nlink;
            param->st_uid     = s.st_uid;
            param->st_gid     = s.st_gid;
            param->st_rdev    = s.st_rdev;
            param->st_size    = s.st_size;
            param->st_blksize = s.st_blksize;
            param->st_blocks  = s.st_blocks;
            param->st_atim.tv_sec  = s.yst_atime;
            param->st_atim.tv_nsec = 0;
            param->st_mtim.tv_sec  = s.yst_mtime;
            param->st_mtim.tv_nsec = 0;
            param->st_ctim.tv_sec  = s.yst_ctime;
            param->st_ctim.tv_nsec = 0;
            ret = YAFFS_OK;
        }
        break;
    }

    case AW_FIOFSTATGET: {
        struct aw_stat *param = (struct aw_stat *)arg;
        struct yaffs_stat s;
        ret = yaffs_fstat(fil->fdesc, &s);
        if (YAFFS_OK == ret) {
            param->st_dev     = s.st_dev;
            param->st_ino     = s.st_ino;
            param->st_mode    = s.st_mode;
            param->st_nlink   = s.st_nlink;
            param->st_uid     = s.st_uid;
            param->st_gid     = s.st_gid;
            param->st_rdev    = s.st_rdev;
            param->st_size    = s.st_size;
            param->st_blksize = s.st_blksize;
            param->st_blocks  = s.st_blocks;
            param->st_atim.tv_sec  = s.yst_atime;
            param->st_atim.tv_nsec = 0;
            param->st_mtim.tv_sec = s.yst_mtime;
            param->st_mtim.tv_nsec = 0;
            param->st_ctim.tv_sec = s.yst_ctime;
            param->st_ctim.tv_nsec = 0;
        }
        break;
    }

    case AW_FIOREADDIR: {
        if (fil->oflags & O_DIRECTORY) {
            int len = 0;
            struct yaffs_dirent *ydir = NULL;
            struct aw_dir          *pdir = (struct aw_dir *)arg;
            ydir = yaffs_readdir(fil->dir);
            if (ydir) {
                pdir->d_dirent.d_ino = ydir->d_ino;
                len = strlen(ydir->d_name);
                if (len > (sizeof(pdir->d_dirent.d_name) - 1)) {
                    len = sizeof(pdir->d_dirent.d_name) -1;
                }
                memcpy(pdir->d_dirent.d_name, ydir->d_name, len + 1);
                ret = YAFFS_OK;
            } else {
                ret = ENOENT;
            }
        } else {
            ret = ENOTDIR;
        }

        break;
    }

    default :
        ret = ENOSYS;

    }

    if (YAFFS_OK == ret) {
        ret = AW_OK;
    } else if (YAFFS_FAIL == ret) {
        ret = AW_ERROR;
    } else {
        ret = -ret;
    }

    return ret;
}

#if !(defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION))
static void __yaffs_release (struct aw_iodev *p_dev)
{
    struct aw_yaffs_volume *p_vol = AW_CONTAINER_OF(p_dev,
                                                    struct aw_yaffs_volume,
                                                    io_dev);
    (void)__volume_free(p_vol);
}

#endif
/******************************************************************************/

/**
 * \brief add yaffs device
 *
 * \param[in] vol     : the pointer of yaffs volume
 * \param[in] mountpt : the mount point
 *
 * \return none
 */
static void __yaffs_add_device (struct aw_yaffs_volume *vol, const char *mountpt)
{
    vol->yfs_dev.param.name = mountpt;
    yaffs_add_device(&vol->yfs_dev);
}

/**
 * \brief del yaffs device
 * \param[in] vol : the pointer of yaffs volume
 * \return none
 */
static void __yaffs_del_device (struct aw_yaffs_volume *vol)
{
    void yaffs_remove_device(struct yaffs_dev *dev);
    yaffs_remove_device(&vol->yfs_dev);
}

/**
 * \brief volume attach
 *
 * \param[in] vol_path : volume path
 * \param[in] dev_path : device path
 *
 * \return the pointer of volume
 */
static void *__yaffs_vol_attach (const char *vol_path, const char *dev_path)
{
    struct aw_yaffs_volume *vol = NULL;

    vol = __yaffs_vol_alloc();
    if (NULL == vol) {
        AW_ERRF(("yaffsvol: alloc volume for %s failed\n", dev_path));
        return NULL;
    }

    vol->mtd = aw_mtd_dev_find(dev_path);
    if (NULL == vol->mtd) {
        AW_ERRF(("yaffsvol: fail to find mtd device %s\n", dev_path));
        __yaffs_vol_free(vol);
        return NULL;
    }

    yaffs_start_up();

    aw_yaffs_mtd_init(vol);

    if (vol_path) {
        __yaffs_add_device(vol, vol_path);
    } else {
        __yaffs_add_device(vol, dev_path);
    }

    return vol;
}


/**
 * \brief volume detach
 *
 * \param[in] vol_path : volume path
 * \param[in] dev_path : device path
 *
 * \return same as the return code of __yaffs_vol_free()
 */
static struct aw_yaffs_volume *__yaffs_vol_detach (struct aw_yaffs_volume *vol)
{
    __yaffs_del_device(vol);

    return __yaffs_vol_free(vol);
}

/**
 * \brief attach to io device
 *
 * \param[in] vol_path : volume path
 * \param[in] dev_path : device path
 *
 * \return the pointer of volume
 */
static void *__yaffs_dev_attach (const char *vol_path, const char *dev_path)
{
    struct aw_yaffs_volume *vol = NULL;
    int ret;

    vol = __yaffs_vol_attach(vol_path, dev_path);
    if (NULL == vol) {
        return NULL;
    }

    if (-1 == yaffs_mount(vol_path)) {
        AW_ERRF(("yaffsvol: fail to mount yaffs at %s\n", vol_path));
        __yaffs_vol_detach(vol);
        return NULL;
    }

    /* add the FAT volume to I/O system */
    ret = aw_iodev_create(&vol->io_dev, vol_path);
    if (ret != AW_OK) {
        AW_ERRF(("yaffsvol: fail to add volume %s to IO system\n", vol_path));
        yaffs_unmount(vol_path);
        __yaffs_vol_detach(vol);
        return NULL;
    }

    vol->io_dev.pfn_ent_delete  = __yaffs_remove;
    vol->io_dev.pfn_ent_open    = __yaffs_open;
    vol->io_dev.pfn_ent_close   = __yaffs_close;
    vol->io_dev.pfn_ent_read    = __yaffs_read;
    vol->io_dev.pfn_ent_write   = __yaffs_write;
    vol->io_dev.pfn_ent_ioctrl  = __yaffs_ioctl;
/* vol->io_dev.pfn_dev_release = __yaffs_release; */

    return vol;
}

/**
 * \brief detach from io device
 * \param[in] param : the pointer of yaffs volume
 * \retval AW_OK    : operate successful
 * \retval AW_ERROR : operate fail
 */
static int __yaffs_dev_detach (void *param)
{
    struct aw_yaffs_volume *vol = NULL;

    vol = (struct aw_yaffs_volume *)param;

    if (-1 == yaffs_unmount(vol->yfs_dev.param.name)) {
        AW_ERRF(("yaffsvol: volume %s are busy, fail to unmount %s\n",
                 vol->yfs_dev.param.name,
                 vol->yfs_dev.param.name));

        return AW_ERROR;
    }

    /* remove the volume from I/O system */
    if (aw_iodev_destroy(&vol->io_dev) != AW_OK) {
        AW_ERRF(("error: aw_io_dev_delete(&vol->io_dev) != AW_OK !\n"));
        return AW_ERROR;
    }
#if 0
    if (AW_OK != aw_io_dev_delete(&vol->io_dev)) {
        AW_ERRF(("error: aw_io_dev_delete(&vol->io_dev) != AW_OK !\n"));
        return AW_ERROR;
    }
#endif

    __yaffs_vol_detach(vol);

    return AW_OK;
}

/**
 * \brief make file system
 *
 * \param[in] dev_path : mtd device path
 * \param[in] fmt_arg  : format argument
 *
 * \retval AW_OK    : operate successful
 * \retval AW_ERROR : operate fail
 */
static int __yaffs_make_fs (const char                    *dev_path,
                            const struct aw_fs_format_arg *fmt_arg)
{
    int ret = AW_OK;

    struct aw_yaffs_volume *vol = NULL;

    if (NULL != fmt_arg && (fmt_arg->flags & AW_FS_FMT_FLAG_LOW_LEVEL)) {
        /* low-level formatting */
        vol = __yaffs_vol_attach(NULL, dev_path);
        if (NULL == vol) {
            return AW_ERROR;
        }

        aw_yaffs_mtd_erase(vol);

        (void)__yaffs_vol_detach(vol);
    }

    vol = __yaffs_vol_attach(NULL, dev_path);
    if (NULL == vol) {
        return AW_ERROR;
    }

    if (0 != yaffs_mount(dev_path)) {
        if (0 != yaffs_format(dev_path, 0, 0, 0)) {
            AW_ERRF(("yaffsvol: fail to mkfs yaffs at %s\n", dev_path));
            ret = AW_ERROR;
        }
    }

    __yaffs_vol_detach(vol);

    return ret;
}


/******************************************************************************/

/**
 * \brief yaffs init
 * \param[in] init : the initialise param
 * \return same as the return code of aw_fs_type_register()
 */
int aw_yaffs_init (const struct aw_yaffs_init_param *init)
{
    static struct aw_fs_type fs;

    aw_pool_init(&__g_yaffs_pool.vol,
                 init->vol_pool,
                 init->vol_pool_size,
                 sizeof(*init->vol_pool));

    aw_pool_init(&__g_yaffs_pool.fil,
                 init->fil_pool,
                 init->fil_pool_size,
                 sizeof(*init->fil_pool));

    /* 注册YAFFS文件系统 */
    fs.fs_dev_attach = __yaffs_dev_attach;
    fs.fs_dev_detach = __yaffs_dev_detach;
    fs.fs_make_fs    = __yaffs_make_fs;
    fs.fs_name       = "yaffs";

    return aw_fs_type_register(&fs);
}

/* end of file */

