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
 * \brief standard symbolic constants and types.
 *
 * \internal
 * \par modification history:
 * 1.04 17-12-27 deo, 使用aw_io_pwd 和 aw_io_pwd_set
 * 1.03 16-01-07 deo, add aw_getcwd aw_chdir
 * 1.02 15-04-30 deo, 增加aw_ftruncate,aw_fsync
 * 1.01 15-04-17 deo, 增加aw_truncate
 * 1.00 14-10-20 orz, first implementation.
 * \endinternal
 */
#include "apollo.h"
#include "aw_sem.h"
#include "io/aw_io_device.h"
#include "io/aw_io_lib.h"
#include "io/sys/aw_stat.h"
#include "io/aw_fcntl.h"
#include "io/aw_stropts.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"
#include "aw_ioctl.h"
#include "string.h"

#define NAME_MAX 255

extern char *aw_io_pwd (void);
extern void aw_io_pwd_set (char *new);




/******************************************************************************/
off_t aw_lseek (int fildes, off_t offset, int whence)
{
    struct aw_seek_struct par;
    aw_err_t              ret;

    par.offset = offset;
    par.whence = whence;

    if ((ret = aw_ioctl(fildes, AW_FIOSEEK, &par)) < 0) {
        return ret;
    }
    return par.offset;
}




/******************************************************************************/
int aw_rmdir (const char *path)
{
    int  fd, ret, len;

    if ((fd = aw_open(path, O_RDONLY | O_DIRECTORY, 0777)) < 0) {
        return fd;
    }

    ret = aw_ioctl(fd, AW_FIORMDIR, NULL);
    (void)aw_close(fd);
    if ((0 != ret) && ((-AW_ENOTSUP == ret) || (-AW_ENOSYS == ret))) {
        return aw_delete(path);
    }
    return ret;
}




/******************************************************************************/
int aw_unlink (const char *path)
{
    int fd, err;

    if ((fd = aw_open(path, O_RDONLY, 0777)) < 0) {
        return fd;
    }
    err = aw_ioctl(fd, AW_FIOUNLINK, NULL);
    (void)aw_close(fd);
    if ((0 != err) && ((-AW_ENOSYS == err) || (AW_ENOTSUP == err))) {
        return aw_delete(path);
    }
    return err;
}

int aw_rename (const char *oldpath, const char *newpath)
{
    char old[NAME_MAX], new[NAME_MAX];
    struct rename_struct rs;
    struct aw_iodev     *p_old = NULL, *p_new = NULL;
    int                  fd, ret;

    ret = aw_io_full_path(old, oldpath);
    if (ret != AW_OK) {
        return ret;
    }

    ret = aw_io_full_path(new, newpath);
    if (ret != AW_OK) {
        return ret;
    }

    /* convert to underling file system path */
    if ((p_old = aw_iodev_get(old, &rs.old_name)) == NULL) {
        AW_ERRNO_SET(AW_ENODEV);
        ret = -AW_ENODEV;
        goto __exit;
    }
    if ((p_new = aw_iodev_get(new, &rs.new_name)) == NULL) {
        AW_ERRNO_SET(AW_ENODEV);
        ret = -AW_ENODEV;
        goto __exit;
    }
    if (p_old != p_new) {
        /* currently we don't support rename files on different devices */
        ret = -AW_ENOTSUP;
        goto __exit;
    }
    if ((fd = aw_open(old, O_RDONLY, 0777)) < 0) {
        ret = fd;
        goto __exit;
    }
    ret = aw_ioctl(fd, AW_FIORENAME, &rs);
    (void)aw_close(fd);
__exit:
    if (p_old) {
        aw_iodev_put(p_old);
    }

    if (p_new) {
        aw_iodev_put(p_new);
    }
    return ret;
}

int aw_truncate (const char *path, off_t length)
{
    int fd,err;

    if ((fd = aw_open(path, O_RDWR, 0777)) < 0) {
        return fd;
    }
    err = aw_ioctl(fd, AW_FIOTRUNC, &length);
    (void)aw_close(fd);
    return err;
}

int aw_ftruncate(int fildes, off_t length)
{
    return aw_ioctl(fildes, AW_FIOTRUNC, &length);
}

char *aw_getcwd (char *buf, size_t size)
{
    size_t      len;
    const char *pwd;

    if ((size == 0) || (buf == NULL)) {
        AW_ERRNO_SET(AW_EINVAL);
        return NULL;
    }

    pwd = aw_io_pwd();
    len = strlen(pwd);

    if ((size < len) || (size > NAME_MAX)) {
        AW_ERRNO_SET(AW_ERANGE);
        return NULL;
    }

    AW_ERRNO_SET(0);

    return strcpy(buf, pwd);
}

int aw_chdir (const char *path)
{
    struct aw_stat  sbuf;
    char            buf[NAME_MAX];

    aw_io_full_path(buf, path);

    if (aw_stat(buf, &sbuf) < 0) {
        AW_ERRNO_SET(AW_ENOENT);
        return -1;
    } else if (!S_ISDIR(sbuf.st_mode)) {
        AW_ERRNO_SET(AW_ENOTDIR);
        return -1;
    } else {
        aw_io_pwd_set(buf);
    }

    return 0;
}


int aw_fsync(int fildes)
{
    return aw_ioctl(fildes, AW_FIOFLUSH, NULL);
}

/* end of file */
