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
 * \brief virtual root file system.
 *
 * \internal
 * \par modification history:
 * - 140820 orz, first implementation.
 * \endinternal
 */

/******************************************************************************/
#include "apollo.h"
#include "fs/aw_root_fs.h"
#include "io/aw_io_device.h"
#include "io/sys/aw_stat.h"
#include "io/aw_fcntl.h"
#include "io/aw_dirent.h"
#include "aw_errno.h"
#include "aw_ioctl.h"
#include "aw_time.h"
#include <string.h>

/******************************************************************************/
/** \brief helper data structure for scanning root directory */
struct __root_fs_dir {
    int              rfd_index; /**< \brief current index of directory stream*/
    struct aw_dir   *rfd_dir;   /**< \brief directory reading stream */
    struct aw_iodev *rfd_dev;   /**< \brief the I/O device of the directory */
};

/******************************************************************************/
/** \brief root volume data structure */
static struct __root_fs_volume {
    struct aw_iodev ios_dev;   /**< \brief device instance for io system */
    time_t          init_sec;  /**< \brief volume initializing time */
} __g_root_vol;

/** \brief root file data structure */
static struct __root_fs_file {
    struct __root_fs_volume *vol; /**< \brief the volume this file is on */
} __g_root_fil;

/******************************************************************************/
static int __root_fs_size_get_call_back(struct aw_iodev *dev, size_t *total_sz)
{
    *total_sz += (sizeof(*dev) + strlen(dev->name));
    return 0;
}

static size_t __root_fs_size_get (void)
{
    size_t total_size = 0;

    (void)aw_iodev_foreach((void *)__root_fs_size_get_call_back, &total_size);
    return total_size;
}

/******************************************************************************/
/**
 * \brief test that a device name is valid for root file system.
 *
 * this routine test the name of a device to ensure that it meets the rules for
 * names in the root file system.
 *
 * \retval 0            the name is a valid root file name.
 * \retval -AW_EPERM    the name is a invalid root file name.
 */
static int __root_fs_dev_name_test (struct aw_iodev *dev)
{
    int i;

    /* make sure device name begins with a slash and exclude the name "/" */
    if ((dev->name[0] != '/') || (strcmp(dev->name, "/") == 0)) {
        return -AW_EPERM;
    }
    /* exclude things that have embedded slashes */
    for (i = 1; dev->name[i] != '\0'; i++) {
        if ('/' == dev->name[i]) {
            return -AW_EPERM;
        }
    }
    return 0;
}

/******************************************************************************/
static int __root_fs_scandir (struct aw_iodev *dev, struct __root_fs_dir *d)
{
    if (d->rfd_index++ >= d->rfd_dir->d_index) {
        if (__root_fs_dev_name_test(dev) == 0) {
            d->rfd_dev = dev;
            return 1;
        }
    }
    return 0;
}

static int __root_fs_readdir (struct aw_dir *d)
{
    struct __root_fs_dir rfdir;
    int                  err   = 0;
    size_t               name_size;

    rfdir.rfd_dev	= NULL;
    rfdir.rfd_dir	= d;
    rfdir.rfd_index	= 0;

    (void)aw_iodev_foreach((void *)__root_fs_scandir, &rfdir);
    if (NULL != rfdir.rfd_dev) {
        /* find a valid directory entry meets the naming criteria and index */
        name_size = sizeof(d->d_dirent.d_name) - 1;
        (void)strncpy(d->d_dirent.d_name, rfdir.rfd_dev->name + 1, name_size);
        d->d_dirent.d_name[name_size] = '\0';
        d->d_dirent.d_ino             = (ino_t)(rfdir.rfd_index - 1);
        d->d_index                    = rfdir.rfd_index; /* save next index */
    } else {
        err = -AW_ENOENT;
    }

    return err;
}

/******************************************************************************/
/**
 * \brief open the root directory of the root-fs.
 *
 * this routine opens the file named "" on the device "/" only. attempts to
 * open any other file will fail.
 *
 * \return a pointer to a file descriptor or NULL on failure.
 */
static void *__root_fs_open(struct aw_iodev *dev,
                            const char      *path,
                            int              oflags,
                            mode_t           mode,
                            int             *err)
{
    (void)dev;
    (void)mode;

    /* test the we are opening the root-fs directory */
    if (strcmp(path, "") && strcmp(path, "/") && strcmp(path, ".") &&
        strcmp(path, "./")) {
        *err = -AW_ENOENT;
        return NULL;
    }
    /* test that our flags do not include write */
    if ((oflags & O_ACCMODE) != O_RDONLY) {
        *err = -AW_EROFS;
        return NULL;
    }
    /* test that our flags do not include O_CREAT or O_TRUNC */
    if (oflags & (O_CREAT | O_TRUNC)) {
        *err = -AW_EROFS;
        return NULL;
    }
    /*
     * if we get this far, then we return the token which represents every open
     * file, as there is only one file which can be opened ("/")
     */
    __g_root_fil.vol = &__g_root_vol;
    return (&__g_root_fil);
}

/******************************************************************************/
int __root_fs_close (void *pfd)
{
    return (pfd != (void *)(&__g_root_fil)) ? -AW_EBADF : 0;
}

/******************************************************************************/
ssize_t __root_fs_read (void *pfd, void *buf, size_t nbyte)
{
    (void)pfd;
    (void)buf;
    (void)nbyte;

    return -AW_EISDIR;
}

/******************************************************************************/
ssize_t __root_fs_write (void *pfd, const void *buf, size_t nbyte)
{
    (void)pfd;
    (void)buf;
    (void)nbyte;

    return -AW_EISDIR;
}

/******************************************************************************/
int __root_fs_ioctl (void *pfd, int cmd, void *arg)
{
    struct aw_stat *st;
    size_t          size;
    int             err;

    switch(cmd) {
    case AW_FIOSTATGET:
        st   = (struct aw_stat *)arg;
        size = __root_fs_size_get();

        st->st_dev   = 0;
        st->st_ino   = 0;
        st->st_mode  = 0444 | S_IFDIR;
        st->st_nlink = 1;
        st->st_gid   = 0;
        st->st_rdev  = 0;
        st->st_size  = size;

        st->st_atim.tv_sec  = __g_root_vol.init_sec;
        st->st_atim.tv_nsec = 0;
        st->st_mtim.tv_sec  = __g_root_vol.init_sec;
        st->st_mtim.tv_nsec = 0;
        st->st_ctim.tv_sec  = __g_root_vol.init_sec;
        st->st_ctim.tv_nsec = 0;

        st->st_blksize = 512;
        st->st_blocks  = (size + 511) / 512;

        err = 0;
        break;

    case AW_FIOREADDIR:
        err = __root_fs_readdir((struct aw_dir *)arg);
        break;

    default:
        err = -AW_ENOTSUP;
        break;
    }
    return err;
}




/******************************************************************************/
int aw_root_fs_init (void)
{
    int  ret;

    (void)aw_time(&__g_root_vol.init_sec);

    ret = aw_iodev_create(&__g_root_vol.ios_dev, "/");
    if (ret == AW_OK) {
        __g_root_vol.ios_dev.pfn_ent_open    = __root_fs_open;
        __g_root_vol.ios_dev.pfn_ent_close   = __root_fs_close;
        __g_root_vol.ios_dev.pfn_ent_read    = __root_fs_read;
        __g_root_vol.ios_dev.pfn_ent_write   = __root_fs_write;
        __g_root_vol.ios_dev.pfn_ent_ioctrl  = __root_fs_ioctl;
    }

    return ret;
}

/* end of file */
