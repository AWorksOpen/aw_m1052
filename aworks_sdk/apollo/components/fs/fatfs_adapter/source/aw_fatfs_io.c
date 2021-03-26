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
 * \brief FatFs AWorks adapter layer
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-07  deo, first implementation
 * \endinternal
 */


#include "apollo.h"
#include "aw_pool.h"
#include "aw_sem.h"
#include "aw_mem.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "aw_ioctl.h"
#include "fs/aw_fs_type.h"
#include "io/aw_fcntl.h"
#include "io/aw_dirent.h"
#include "io/aw_unistd.h"
#include "io/aw_utime.h"
#include "io/sys/aw_stat.h"
#include "io/sys/aw_statvfs.h"

#include "aw_fatfs.h"
#include "diskio.h"
#include "string.h"
#include "errno.h"



#define __FATFS_TRACE(info) \
            do { \
                AW_INFOF(("%s line %d: ", __FILE__, __LINE__)); \
                AW_INFOF(info); \
            } while (0)

#define __FATFS_TIMEOUT   AW_WAIT_FOREVER


static struct aw_pool    __g_vol_pool;
static struct aw_pool    __g_fil_pool;
struct aw_fatfs_volume **__g_volumes;

static struct aw_fatfs_volume *__volume_alloc (void);
static void __volume_free (struct aw_fatfs_volume *p_vol);
static struct aw_fatfs_file *__file_alloc (void);
static void __file_free (struct aw_fatfs_file *p_fil);

static int __fatfs_remove (struct aw_iodev *dev, const char *path);
static void *__fatfs_open (struct aw_iodev *dev,
                           const char      *path,
                           int              oflag,
                           mode_t           mode,
                           int             *err);
static int __fatfs_close (void *pfd);
static ssize_t __fatfs_read (void *pfd, void *buf, size_t nbyte);
static ssize_t __fatfs_write (void *pfd, const void *buf, size_t nbyte);
static int __fatfs_ioctl (void *pfd, int cmd, void *arg);
static void __fatfs_release (struct aw_iodev *p_dev);

/******************************************************************************/

/******************************************************************************/

/** @brief convert FAT FS res to error number */
static inline int __fatfs_errno_map (FRESULT res)
{
    static const uint8_t err_tbl[] = {
        0,         /* 0,  FR_OK */
        AW_EIO,    /* 1,  FR_DISK_ERR */
        AW_EINVAL, /* 2,  FR_INT_ERR */
        AW_EIO,    /* 3,  FR_NOT_READY */
        AW_ENOENT, /* 4,  FR_NO_FILE */
        AW_ENOENT, /* 5,  FR_NO_PATH */
        AW_EINVAL, /* 6,  FR_INVALID_NAME */
        AW_EROFS,  /* 7,  FR_DENIED */
        AW_EEXIST, /* 8,  FR_EXIST */
        AW_EEXIST, /* 9,  FR_INVALID_OBJECT */
        AW_EROFS,  /* 10, FR_WRITE_PROTECTED */
        AW_EIO,    /* 11, FR_INVALID_DRIVE */
        AW_EIO,    /* 12, FR_NOT_ENABLED */
        AW_ENOENT, /* 13, FR_NO_FILESYSTEM */
        AW_EINVAL, /* 14, FR_MKFS_ABORTED */
        AW_ETIME,  /* 15, FR_TIMEOUT */
        AW_EPERM,  /* 16, FR_LOCKED */
        AW_ENOSPC, /* 17, FR_NOT_ENOUGH_CORE */
        AW_EMFILE, /* 18, FR_TOO_MANY_OPEN_FILES */
        AW_EINVAL  /* 19, FR_INVALID_PARAMETER */
    };

    return  -(err_tbl[res]);
}







/******************************************************************************/
static int __fatfs_remove (struct aw_iodev *dev, const char *path)
{
#if !FF_FS_READONLY
    struct aw_fatfs_volume *p_vol;
    char                    fat_path[FF_MAX_LFN + 1];

    p_vol = AW_CONTAINER_OF(dev, struct aw_fatfs_volume, io_dev);

    (void)aw_snprintf(fat_path, FF_MAX_LFN, "%d:%s", p_vol->disk_idx, path);

    return __fatfs_errno_map(f_unlink(fat_path));
#else
    return -AW_ENOTSUP;
#endif
}




/******************************************************************************/
static void *__fatfs_open (struct aw_iodev *dev,
                           const char      *path,
                           int              oflag,
                           mode_t           mode,
                           int             *err)
{
    struct aw_fatfs_volume *p_vol;
    struct aw_fatfs_file   *p_fil;
    FILINFO                 finfo;
    char                    fat_path[FF_MAX_LFN + 1];
    int                     ret;

    p_vol = AW_CONTAINER_OF(dev, struct aw_fatfs_volume, io_dev);

    (void)aw_snprintf(fat_path, FF_MAX_LFN, "%d:%s", p_vol->disk_idx, path);

    /* check if is a directory */
    if (path[0] == '\0') {
        /* root directory */
        oflag |= O_DIRECTORY;
    } else {
        /* check path */
        ret = __fatfs_errno_map(f_stat(fat_path, &finfo));
        if (ret == AW_OK) {
            if (finfo.fattrib & AM_DIR) {
                oflag |= O_DIRECTORY;
            } else if (oflag & O_DIRECTORY) {
                /* request to open a directory,
                 * but this file is no a directory */
                *err = -AW_ENOTDIR;
                return NULL;
            }
        } else {
            if (!(oflag & O_CREAT)) {
                *err = ret;
                return NULL;
            }
        }
    }

    if (oflag & O_DIRECTORY) {
        /* create new directory */
        if (oflag & O_CREAT) {
#if !FF_FS_READONLY
            ret = __fatfs_errno_map(f_mkdir(fat_path));
#else
            ret = -AW_ENOTSUP;
#endif
            if (ret != AW_OK) {
                *err = ret;
                return NULL;
            }
        }

        /* allocate a file descriptor */
        p_fil = __file_alloc();
        if (p_fil == NULL) {
            *err = -AW_ENOMEM;
            return NULL;
        }

        p_fil->volume = p_vol;
        p_fil->oflags = oflag;
        strcpy(p_fil->path, fat_path);

        /* open directory */
        ret = __fatfs_errno_map(f_opendir(&p_fil->u.dir, fat_path));
        if (ret != AW_OK) {
            *err = ret;
            __file_free(p_fil);
            p_fil = NULL;
        }
    } else {
        BYTE  mode = FA_READ;

        /* write flag */
        if (oflag & O_WRONLY) {
            mode = FA_WRITE;
        }

        if ((oflag & O_ACCMODE) & O_RDWR) {
            mode |= FA_WRITE;
        }

        /* if it is not existing, create it. */
        if (oflag & O_CREAT) {
            mode |= FA_OPEN_ALWAYS;
        }

        /* creates a new file. If the file is existing,
         * it is truncated and overwritten. */
        if (oflag & O_TRUNC) {
            mode |= FA_CREATE_ALWAYS;
        }

        /* Creates a new file.
         * The function fails if the file is already existing */
        if (oflag & O_EXCL) {
            mode |= FA_CREATE_NEW;
        }

        if (oflag & O_APPEND) {
            mode |= FA_OPEN_APPEND;
        }

        /* allocate a file descriptor */
        p_fil = __file_alloc();
        if (p_fil == NULL) {
            *err = -AW_ENOMEM;
            return NULL;
        }

        p_fil->volume = p_vol;
        p_fil->oflags = oflag;
        strcpy(p_fil->path, fat_path);

        /* open file */
        ret = __fatfs_errno_map(f_open(&p_fil->u.fil, fat_path, mode));
        if (ret != AW_OK) {
            *err = ret;
            __file_free(p_fil);
            p_fil = NULL;
        }
    }

    return p_fil;
}




/******************************************************************************/
static int __fatfs_close (void *pfd)
{
    struct aw_fatfs_file *p_fil;
    FRESULT               res;

    p_fil = (struct aw_fatfs_file *)pfd;

    if (p_fil->oflags & O_DIRECTORY) {
        res = f_closedir(&p_fil->u.dir);
    } else {
        res = f_close(&p_fil->u.fil);
    }
    (void)__file_free(p_fil);

    return __fatfs_errno_map(res);
}




/******************************************************************************/
static ssize_t __fatfs_read (void *pfd, void *buf, size_t nbyte)
{
    struct aw_fatfs_file *p_fil;
    int                   ret;
    UINT                  bytes;

    p_fil = (struct aw_fatfs_file *)pfd;

    if (p_fil->oflags & O_DIRECTORY) {
        /* FatFs does not accesses directory as regular file */
        return -AW_EISDIR;
    }

    ret = __fatfs_errno_map(f_read(&p_fil->u.fil, buf, nbyte, &bytes));
    return (ret == AW_OK) ? bytes: ret;
}




/******************************************************************************/
static ssize_t __fatfs_write (void *pfd, const void *buf, size_t nbyte)
{
#if FF_FS_READONLY
    return -EPERM;
#else
    struct aw_fatfs_file *p_fil;
    int                   ret;
    UINT                  bytes;

    p_fil = (struct aw_fatfs_file *)pfd;

    if (p_fil->oflags & O_DIRECTORY) {
        return -AW_EISDIR;
    }

    ret = __fatfs_errno_map(f_write(&p_fil->u.fil, buf, nbyte, &bytes));
    return (ret == AW_OK) ? bytes: ret;
#endif
}




/******************************************************************************/
static int __fatfs_lseek (struct aw_fatfs_file  *p_fil, 
                          struct aw_seek_struct *par)
{
    off_t offset = par->offset;
    int   ret;

    switch (par->whence) {
    case SEEK_SET:
        break;
    case SEEK_CUR:
        offset += (off_t)f_tell(&p_fil->u.fil);
        break;
    case SEEK_END:
        offset += (off_t)f_size(&p_fil->u.fil);
        break;
    default:
        return -AW_EINVAL;
    }

    ret = __fatfs_errno_map(f_lseek(&p_fil->u.fil, offset));
    if (ret == AW_OK) {
        par->offset = (off_t)f_tell(&p_fil->u.fil);
    }

    return ret;
}




/******************************************************************************/
static int __fatfs_truncate (struct aw_fatfs_file *p_fil, off_t length)
{
    int   ret;

#if !FF_FS_READONLY
    ret = __fatfs_errno_map(f_lseek(&p_fil->u.fil, length));
    if (ret != 0) {
        return ret;
    }

    if (f_size(&p_fil->u.fil) > length) {
        ret = __fatfs_errno_map(f_truncate(&p_fil->u.fil));
    }
#else
        ret = -AW_ENOTSUP;
#endif
    return ret;
}




/******************************************************************************/
static int __fatfs_utime(struct aw_fatfs_file *p_fil,
                         struct aw_utimbuf    *p_utime)
{
#if !FF_FS_READONLY
    FRESULT     res;
    FILINFO     finfo;
    DWORD       fattime;
    time_t      time;

    time = p_utime->modtime;
    fattime = aw_time_to_fattime(&time);
    finfo.fdate = fattime >> 16;
    finfo.ftime = fattime & 0xFFFFU;
    res = f_utime(p_fil->path, &finfo);
    return __fatfs_errno_map(res);
#else
    return -AW_ENOTSUP;
#endif
}




/******************************************************************************/
static int __fatfs_chmod (struct aw_fatfs_file *p_fil, mode_t mode)
{
#if !FF_FS_READONLY
    FRESULT res;
    BYTE    value = 0;
    BYTE    mask = 0;

    if (mode & S_IFDIR) {
        value |= AM_DIR;
        mask |= AM_DIR;
    }

    if (!(mode & (S_IWUSR | S_IWGRP | S_IWOTH))) {
        value |= AM_RDO;
        mask |= AM_RDO;
    }

    res = f_chmod(p_fil->path, value, mask);
    return __fatfs_errno_map(res);
#else
    return -AW_ENOTSUP;
#endif
}




/******************************************************************************/
static int __fatfs_statvfs (struct aw_fatfs_volume *p_vol,
                            struct aw_statvfs      *buf)
{
    FATFS  *pfs;
    int     ret;
    char    drv[4];
    DWORD   fre_clust;
    WORD    bsize;

    (void)aw_snprintf(drv, sizeof(drv), "%d:", p_vol->disk_idx);

#if !FF_FS_READONLY
    ret = __fatfs_errno_map(f_getfree(drv, &fre_clust, &pfs));
    if (ret != AW_OK) {
        return ret;
    }
#else
    return -AW_ENOTSUP;
#endif

    if (disk_ioctl(p_vol->disk_idx, GET_SECTOR_SIZE, &bsize) != RES_OK) {
        return -AW_EIO;
    }

    (void)memset(buf, 0, sizeof(*buf));

    /* get total sectors and free sectors */
    buf->f_bsize  = pfs->csize * bsize;
    buf->f_frsize = bsize;
    buf->f_blocks = pfs->n_fatent - 2;
    buf->f_bfree  = fre_clust;

    return 0;
}




/******************************************************************************/
static int __fatfs_readdir (struct aw_fatfs_file *p_fil, struct aw_dir *d)
{
    struct aw_dirent *entry = &d->d_dirent;
    FILINFO           finfo;
    FRESULT           res;
    WORD              bsize;

    res = f_readdir(&p_fil->u.dir, &finfo);
    if ((res == FR_OK) && finfo.fname[0]) {
        entry->d_ino = d->d_index++;

        (void)strcpy(entry->d_name, finfo.fname);

#ifdef __CANDTU_EX
        (void)memset(&entry->d_stat, 0, sizeof(entry->d_stat));

        entry->d_stat.st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH |
                                S_IWUSR | S_IWGRP | S_IWOTH;
        if (finfo.fattrib & AM_DIR) {
            entry->d_stat.st_mode &= ~S_IFREG;
            entry->d_stat.st_mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
        }
        if (finfo.fattrib & AM_RDO) {
            entry->d_stat.st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
        }
        entry->d_stat.st_size        = finfo.fsize;
        entry->d_stat.st_mtim.tv_sec = 
            aw_fattime_to_time((finfo.fdate << 16) | finfo.ftime);
        entry->d_stat.st_mtim.tv_nsec= 0;
        entry->d_stat.st_atim        = entry->d_stat.st_mtim;
        entry->d_stat.st_ctim        = entry->d_stat.st_mtim;

        if (disk_ioctl(p_fil->volume->disk_idx, GET_SECTOR_SIZE, &bsize) != 
            RES_OK) {
            return -AW_EIO;
        }

        entry->d_stat.st_blksize = p_fil->volume->ff_volume.csize * bsize;
        entry->d_stat.st_blocks  = 
            AW_DIV_ROUND_UP(finfo.fsize, entry->d_stat.st_blksize);

#endif
    } else {
        res = FR_NO_FILE;
    }

    return __fatfs_errno_map(res);
}




/******************************************************************************/
static int __fatfs_rename (struct aw_fatfs_file   *p_fil,
                           int                     idx,
                           struct rename_struct   *p_r)
{
#if !FF_FS_READONLY
    FRESULT res;
    char    old_path[FF_MAX_LFN + 1];
    char    new_path[FF_MAX_LFN + 1];

    (void)aw_snprintf(old_path, FF_MAX_LFN, "%d:%s", idx, p_r->old_name);
    (void)aw_snprintf(new_path, FF_MAX_LFN, "%d:%s", idx, p_r->new_name);

    if (p_fil->oflags & O_DIRECTORY) {
        res = f_closedir(&p_fil->u.dir);
    } else {
        res = f_close(&p_fil->u.fil);
    }

    if (res == FR_OK) {
        res = f_rename(old_path, new_path);
        if (res == FR_OK) {
            if (p_fil->oflags & O_DIRECTORY) {
                res = f_opendir(&p_fil->u.dir, new_path);
            } else {
                BYTE  mode = FA_READ;

                if (p_fil->oflags & O_WRONLY) {
                    mode = FA_WRITE;
                }
                if ((p_fil->oflags & O_ACCMODE) & O_RDWR) {
                    mode |= FA_WRITE;
                }
                /* 
                 * Open the file, if it is existing. 
                 * If not, a new file is created. 
                 */
                if (p_fil->oflags & O_CREAT) {
                    mode |= FA_OPEN_ALWAYS;
                }
                /* 
                 * Creates a new file. If the file is existing,
                 * it is truncated and overwritten. 
                 */
                if (p_fil->oflags & O_TRUNC) {
                    mode |= FA_CREATE_ALWAYS;
                }
                /* 
                 * Creates a new file. The function fails if the file is 
                 * already existing 
                 */
                if (p_fil->oflags & O_EXCL) {
                    mode |= FA_CREATE_NEW;
                }
                res = f_open(&p_fil->u.fil, new_path, mode);
            }

            AW_DBGF(("rename (%s) to (%s) %s\n", old_path, new_path,
                    res == FR_OK ? "OK" : "failed"));
            if (res == FR_OK) {
                strcpy(p_fil->path, new_path);
            }
        }
    }

    return __fatfs_errno_map(res);
#else
    return -AW_ENOTSUP;
#endif
}




/******************************************************************************/
static int __fatfs_stat (struct aw_fatfs_file *p_fil,
                         struct aw_stat       *p_buf)
{
    FILINFO      finfo;
    FRESULT      res;
    WORD         bsize;

    if (p_fil->path[2] != '\0') {/* TODO */
        res = f_stat(p_fil->path, &finfo);
    } else {
        /* we are stat at root path, however, f_stat() can't do that */
        finfo.fsize   = 0;
        finfo.fdate   = 0;
        finfo.ftime   = 0;
        finfo.fattrib = AM_DIR;
        res = FR_OK;
    }

    if (res == FR_OK) {
        (void)memset(p_buf, 0, sizeof(*p_buf));
        p_buf->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH
                    | S_IWUSR | S_IWGRP | S_IWOTH;
        if (finfo.fattrib & AM_DIR) {
            p_buf->st_mode &= ~S_IFREG;
            p_buf->st_mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
        }
        if (finfo.fattrib & AM_RDO) {
            p_buf->st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
        }
        p_buf->st_size        = finfo.fsize;
        p_buf->st_mtim.tv_sec = 
            aw_fattime_to_time((finfo.fdate << 16) | finfo.ftime);
        p_buf->st_mtim.tv_nsec= 0;
        p_buf->st_atim        = p_buf->st_mtim;
        p_buf->st_ctim        = p_buf->st_mtim;

        if (disk_ioctl(p_fil->volume->disk_idx, GET_SECTOR_SIZE, &bsize) != 
            RES_OK) {
            return -AW_EIO;
        }

        p_buf->st_blksize = p_fil->volume->ff_volume.csize * bsize;
        p_buf->st_blocks  = AW_DIV_ROUND_UP(finfo.fsize, p_buf->st_blksize);
    }

    return __fatfs_errno_map(res);
}




/******************************************************************************/
static int __fatfs_ioctl (void *pfd, int cmd, void *arg)
{
    struct aw_fatfs_file   *p_fil;
    struct aw_fatfs_volume *p_vol;
    int                     ret = 0;

    p_fil = (struct aw_fatfs_file *)pfd;
    p_vol = p_fil->volume;

    switch (cmd) {
    case AW_FIOFLUSH:
#if !FF_FS_READONLY
        ret = __fatfs_errno_map(f_sync(&p_fil->u.fil));
#else
        ret = AW_OK;
#endif
        break;

    case AW_FIOWHERE:
        *((off_t *)arg) = f_tell(&p_fil->u.fil);
        break;

    case AW_FIONREAD:
        *((size_t *)arg) = f_size(&p_fil->u.fil) - f_tell(&p_fil->u.fil);
        break;

    case AW_FIOSEEK:
        ret = __fatfs_lseek(p_fil, arg);
        break;

    case AW_FIOTRUNC:
        ret = __fatfs_truncate(p_fil, *((off_t *)arg));
        break;

    case AW_FIOUTIME:
        ret = __fatfs_utime(p_fil, arg);
        break;

    case AW_FIOCHMOD:
        ret = __fatfs_chmod(p_fil, *(mode_t*)arg);
        break;

    case AW_FIOFSTATFSGET:
        ret = __fatfs_statvfs(p_vol, (struct aw_statvfs *)arg);
        break;

    case AW_FIOREADDIR:
        if (p_fil->oflags & O_DIRECTORY) {
            ret = __fatfs_readdir(p_fil, (struct aw_dir *)arg);
        } else {
            ret = -AW_ENOTDIR;
        }
        break;

    case AW_FIORENAME:
        ret = __fatfs_rename(p_fil,
                             p_vol->disk_idx,
                             (struct rename_struct *)arg);
        break;
    case AW_FIORMDIR:
#if !FF_FS_READONLY
        return __fatfs_errno_map(f_unlink(p_fil->path));
#else
        return -AW_ENOTSUP;
#endif
        break;
    case AW_FIOSTATGET:
        ret = __fatfs_stat(p_fil, (struct aw_stat*)arg);
        break;
    default:
        ret = -AW_ENOSYS;
        break;
    }
    return ret;
}




/******************************************************************************/
static void __fatfs_release (struct aw_iodev *p_dev)
{
    struct aw_fatfs_volume *p_vol = AW_CONTAINER_OF(p_dev,
                                                    struct aw_fatfs_volume,
                                                    io_dev);
    (void)__volume_free(p_vol);
}





/******************************************************************************/
static struct aw_fatfs_volume *__volume_alloc (void)
{
    struct aw_fatfs_volume *p_vol;

    p_vol = (struct aw_fatfs_volume *)aw_pool_item_get(&__g_vol_pool);
    if (p_vol) {
        (void)memset(p_vol, 0, sizeof(*p_vol));
    }
    return p_vol;
}




/******************************************************************************/
static void __volume_free (struct aw_fatfs_volume *p_vol)
{
    int  ret;
    ret = aw_pool_item_return(&__g_vol_pool, p_vol);
    aw_assert(0 == ret);
}




/******************************************************************************/
static struct aw_fatfs_file *__file_alloc (void)
{
    struct aw_fatfs_file *p_fil;

    p_fil = (struct aw_fatfs_file *)aw_pool_item_get(&__g_fil_pool);
    if (p_fil) {
        (void)memset(p_fil, 0, sizeof(*p_fil));
    }
    return p_fil;
}




/******************************************************************************/
static void __file_free (struct aw_fatfs_file *p_fil)
{
    int  ret;
    ret = aw_pool_item_return(&__g_fil_pool, p_fil);
    aw_assert(0 == ret);
}




/******************************************************************************/
static void *__fatfs_dev_attach (const char *vol, const char *dev)
{
    struct aw_fatfs_volume *p_vol = NULL;
    struct aw_blk_dev      *p_bd = NULL;
    struct aw_statvfs       finfo;
    char                    drv[4];
    int                     ret;

    /* get block device */
    p_bd = aw_blk_dev_get(dev);
    if (p_bd == NULL) {
        goto __failed;
    }

    /* get a volume and register it */
    p_vol = __volume_alloc();
    if (p_vol == NULL) {
        __FATFS_TRACE(("allocate volume failed.\n"));
        goto __failed;
    }

    p_vol->disk_idx = -1;
    p_vol->blk_dev  = p_bd;

    /* get a system disk number */
    p_vol->disk_idx = aw_fatfs_disk_num_get(p_bd);
    if (p_vol->disk_idx < 0) {
        __FATFS_TRACE(("get disk number failed.\n"));
        goto __failed;
    }

    /* mount the FAT volume */
    (void)aw_snprintf(drv, sizeof(drv), "%d:", p_vol->disk_idx);
    ret = __fatfs_errno_map(f_mount(&p_vol->ff_volume, drv, 1));
    if (ret != AW_OK) {
        (void)f_mount(NULL, drv, 1);
        __FATFS_TRACE(("%s mount failed.\n", vol));
        goto __failed;
    }

    /* get information */
    __fatfs_statvfs(p_vol, &finfo);

    /* add the FAT volume to I/O system */
    ret = aw_iodev_create(&p_vol->io_dev, vol);
    if (ret != AW_OK) {
        (void)f_mount(NULL, drv, 1);
        __FATFS_TRACE(("%s add to I/O failed.\n", vol));
        goto __failed;
    }

    p_vol->io_dev.pfn_ent_delete  = __fatfs_remove;
    p_vol->io_dev.pfn_ent_open    = __fatfs_open;
    p_vol->io_dev.pfn_ent_close   = __fatfs_close;
    p_vol->io_dev.pfn_ent_read    = __fatfs_read;
    p_vol->io_dev.pfn_ent_write   = __fatfs_write;
    p_vol->io_dev.pfn_ent_ioctrl  = __fatfs_ioctl;
    p_vol->io_dev.pfn_dev_release = __fatfs_release;

    goto __exit;

__failed:
    errno = ret;

    if (p_vol) {
        if (p_vol->disk_idx >= 0) {
            aw_fatfs_disk_num_put(p_vol->disk_idx);
        }
        __volume_free(p_vol);
    }
    if (p_bd) {
        /* put block device */
        ret = aw_blk_dev_put(p_bd);
        if (ret != AW_OK) {
            __FATFS_TRACE(("put block device failed: %d.\n", ret));
        }
    }
    p_vol = NULL;
__exit:
    return p_vol;
}




/******************************************************************************/
static int __fatfs_dev_detach (void *vol)
{
    struct aw_fatfs_volume *p_vol = (struct aw_fatfs_volume *)vol;
    char                    drv[4];
    int                     ret;

    /* unmount the FAT volume */
    (void)aw_snprintf(drv, sizeof(drv), "%d:", p_vol->disk_idx);

    ret = __fatfs_errno_map(f_mount(NULL, drv, 0));
    if (ret != AW_OK) {
        __FATFS_TRACE(("%s unmount volume %d failed %d.\n",
                       __FUNCTION__,
                       p_vol->disk_idx,
                       ret));
    }

    /* unregister the FAT volume */
    ret = aw_fatfs_disk_num_put(p_vol->disk_idx);
    if (ret != AW_OK) {
        __FATFS_TRACE(("%s put disk number failed: %d.\n",
                       __FUNCTION__,
                       ret));
    }
    p_vol->disk_idx = -1;

    /* put block device */
    ret = aw_blk_dev_put(p_vol->blk_dev);
    if (ret != AW_OK) {
        __FATFS_TRACE(("%s put block device failed: %d.\n",
                       __FUNCTION__,
                       ret));
    }
    p_vol->blk_dev = NULL;

    /* remove the volume from I/O system */
    ret = aw_iodev_destroy(&p_vol->io_dev);
    if (ret != AW_OK) {
        __FATFS_TRACE(("%s delete volume from I/O system failed %d.\n",
                       __FUNCTION__,
                       ret));
    }

    return AW_OK;
}




/******************************************************************************/
static int __fatfs_mkfs (const char                    *dev_path,
                         const struct aw_fs_format_arg *fmt_arg)
{
#if !FF_FS_READONLY
    struct aw_blk_dev      *p_bd = NULL;
    char                    drv[4];
    BYTE                   *work = NULL;
    int                     ret, disk = -AW_ENOENT;
    BYTE                    opt = FM_ANY;
    DWORD                   au  = 0;
    uint32_t                bszie = 0;
    uint32_t                blks = 0;
    uint32_t                dev_size = 0;

    work = aw_mem_alloc(FF_MAX_SS);
    if (work == NULL) {
        return -AW_ENOMEM;
    }

    /* get block device */
    p_bd = aw_blk_dev_get(dev_path);
    if (p_bd == NULL) {
        ret = -AW_ENODEV;
        goto __exit;
    }

    ret = aw_blk_dev_ioctl(p_bd, AW_BD_GET_BSIZE, &bszie);
    if (ret != AW_OK) {
        goto __exit;
    }

    ret = aw_blk_dev_ioctl(p_bd, AW_BD_GET_BLKS, &blks);
    if (ret != AW_OK) {
        goto __exit;
    }

    /* get block device size */
    dev_size = (uint64_t)bszie * blks / 1024 / 1024;

    if (dev_size > 32 * 1024) {         /* ÈÝÁ¿  > 32 * 1024 MB */
        opt = FM_EXFAT;
    } else if (dev_size > 32) {         /* ÈÝÁ¿  > 32 MB */
        opt = FM_FAT32;
    } else {                            /* ÈÝÁ¿  < 32 MB */
        opt = FM_FAT;
    }

    /* allocate disk number */
    disk = aw_fatfs_disk_num_get(p_bd);
    if (disk < 0) {
        goto __exit;
    }

    (void)aw_snprintf(drv, sizeof(drv), "%d:", disk);

    if (fmt_arg) {
        au = fmt_arg->unit_size;
    }

    ret = aw_blk_dev_lock(p_bd);
    if (ret != AW_OK) {
        goto __exit;
    }

    /* make it */
    ret = __fatfs_errno_map(f_mkfs(drv,
                                   opt,
                                   au,
                                   work,
                                   FF_MAX_SS));

    aw_blk_dev_unlock(p_bd);

__exit:
    if (work) {
        aw_mem_free(work);
    }

    if (p_bd) {
        /* put block device */
        aw_blk_dev_put(p_bd);
    }

    if (disk >= 0) {
        aw_fatfs_disk_num_put(disk);
    }

    return ret;
#else
    return -AW_ENOTSUP;
#endif
}




/******************************************************************************/

int aw_fatfs_init (void         *p_vol_mem,
                   unsigned int  vmem_size,
                   void         *p_fil_mem,
                   unsigned int  fmem_size)
{
    static struct aw_fs_type fs;

    if (aw_pool_init(&__g_vol_pool,
                     p_vol_mem,
                     vmem_size,
                     sizeof(struct aw_fatfs_volume)) == NULL) {
        return -AW_ENOENT;
    }

    if (aw_pool_init(&__g_fil_pool,
                     p_fil_mem,
                     fmem_size,
                     sizeof(struct aw_fatfs_file)) == NULL) {
        return -AW_ENOENT;
    }

    /* register FAT file system */
    fs.fs_dev_attach = __fatfs_dev_attach;
    fs.fs_dev_detach = __fatfs_dev_detach;
    fs.fs_make_fs    = __fatfs_mkfs;
    fs.fs_name       = "vfat";
    return aw_fs_type_register(&fs);

    return AW_OK;
}

/* end of file */
