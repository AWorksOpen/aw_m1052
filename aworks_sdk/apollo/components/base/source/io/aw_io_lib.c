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
 * \brief I/O API for application.
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-08  deo, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_vdebug.h"
#include "io/aw_io_device.h"
#include "io/aw_io_entry.h"
#include "io/aw_fcntl.h"
#include "io/aw_stropts.h"
#include "io/aw_unistd.h"
#include "aw_assert.h"

#include "string.h"

#define NAME_MAX        255

extern aw_err_t aw_io_full_path (char *full, const char *path);



/******************************************************************************/
static int __ent_create (const char *path, mode_t mode)
{
    struct aw_iodev    *p_dev;
    int                 ret;

    /* get IO device */
    p_dev = aw_iodev_get(path, &path);
    if (p_dev == NULL) {
        return -AW_ENODEV;
    }

    /* create a new */
    ret = aw_iodev_ent_create(p_dev, path, mode);
    aw_iodev_put(p_dev);
    return ret;
}




/******************************************************************************/
static int __ent_open (const char *path, int flag, mode_t mode)
{
    struct aw_iodev    *p_dev = NULL;
    struct aw_io_entry *p_entry = NULL;
    void               *p_ent;
    int                 ret = -AW_ENOENT;

    (void)p_ent;
    /* get IO device */
    p_dev = aw_iodev_get(path, &path);
    if (p_dev == NULL) {
        return -AW_ENODEV;
    }

    /* allocate a entry */
    p_entry = aw_io_entry_alloc();
    if (p_entry == NULL) {
        goto __failed;
    }

    /* open it */
    p_ent = aw_iodev_ent_open(p_dev, path, flag, mode, &ret);
    if (p_ent == NULL) {
        goto __failed;
    }
    /*
     * they do not allow the same value, because p_entry->p_ent is the
     * value to determine whether it has been assigned.
     */
    aw_assert(p_entry->p_ent != p_ent);

    p_entry->p_ent = p_ent;
    p_entry->p_dev = p_dev;

    /* get index */
    ret = aw_io_entry_to_index(p_entry);
    if (ret < 0) {
        goto __failed;
    }
    return ret;

__failed:
    if (p_dev) {
        aw_iodev_put(p_dev);
    }
    if (p_entry) {
        aw_io_entry_free(p_entry);
    }
    return ret;
}




/******************************************************************************/
int aw_create (const char *path,
               mode_t      mode)
{
    int  ret, oflag;
    char full[NAME_MAX];

    if ((path == NULL) || (path[0] == EOS)) {
        return -AW_EINVAL;
    }

    ret = aw_io_full_path(full, path);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __ent_create(full, mode);
    if (ret == -AW_ENOTSUP) {
        /* not support, go to try open */
        oflag = O_WRONLY | O_CREAT | O_TRUNC;

        ret = __ent_open(full, oflag, mode);
        if (ret > 0) {

        }
    }

    return ret;
}




/******************************************************************************/
int aw_delete (const char *path)
{
    struct aw_iodev *p_dev;
    int              ret;
    char             full[NAME_MAX];
    const char      *p_path = NULL;

    if ((path == NULL) || (path[0] == EOS)) {
        return -AW_EINVAL;
    }

    ret = aw_io_full_path(full, path);
    if (ret != AW_OK) {
        return ret;
    }

    /* get IO device */
    p_dev = aw_iodev_get(full, &p_path);
    if (p_dev == NULL) {
        return -AW_ENODEV;
    }
    ret = aw_iodev_ent_delete(p_dev, p_path);
    aw_iodev_put(p_dev);

    return ret;
}




/******************************************************************************/
int aw_open (const char *path, int oflag, mode_t mode)
{
    int              ret;
    char             full[NAME_MAX];

    if ((path == NULL) || (path[0] == EOS)) {
        return -AW_EINVAL;
    }

    ret = aw_io_full_path(full, path);
    if (ret != AW_OK) {
        return ret;
    }

    return __ent_open(full, oflag, mode);
}




/******************************************************************************/
int aw_close (int index)
{
    struct aw_io_entry *p_entry;
    int                 ret;

    if (!aw_io_entry_valid(index)) {
        return -AW_EBADF;
    }

    p_entry = aw_io_index_to_entry(index);
    if (NULL == p_entry) {
        return -AW_EBADF;
    }

    ret = aw_iodev_ent_close(p_entry->p_dev, p_entry->p_ent);
    if (aw_iodev_put(p_entry->p_dev) != AW_OK) {
        AW_WARNF(("%s: IO device put failed.\n", __FUNCTION__));
    }
    aw_io_entry_free(p_entry);

    return ret;
}




/******************************************************************************/
ssize_t aw_read (int index, void *p_buf, size_t nbyte)
{
    struct aw_io_entry *p_entry;

    if (!aw_io_entry_valid(index)) {
        return -AW_EBADF;
    }

    p_entry = aw_io_index_to_entry(index);
    if (NULL == p_entry) {
        return -AW_EBADF;
    }

    return aw_iodev_ent_read(p_entry->p_dev,
                             p_entry->p_ent,
                             p_buf,
                             nbyte);
}




/******************************************************************************/
ssize_t aw_write (int index, const void *p_buf, size_t nbyte)
{
    struct aw_io_entry *p_entry;

    if (!aw_io_entry_valid(index)) {
        return -AW_EBADF;
    }

    p_entry = aw_io_index_to_entry(index);
    if (NULL == p_entry) {
        return -AW_EBADF;
    }

    return aw_iodev_ent_write(p_entry->p_dev,
                             p_entry->p_ent,
                             p_buf,
                             nbyte);
}




/******************************************************************************/
int aw_ioctl (int index, int cmd, void *arg)
{
    struct aw_io_entry *p_entry;

    if (!aw_io_entry_valid(index)) {
        return -AW_EBADF;
    }

    p_entry = aw_io_index_to_entry(index);
    if (NULL == p_entry) {
        return -AW_EBADF;
    }

    return aw_iodev_ent_ioctrl(p_entry->p_dev,
                               p_entry->p_ent,
                               cmd,
                               arg);
}
