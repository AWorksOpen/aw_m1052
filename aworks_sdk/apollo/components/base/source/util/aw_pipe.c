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
 * \brief 管道操作
 *
 * \internal
 * \par modification history
 * - 1.00 15-05-14  dav, create file
 * \endinternal
 */

#include "aw_pipe.h"
#include "aw_int.h"

aw_err_t aw_pipe_init(aw_pipe_t pb, void *p_buf, size_t nbytes)
{
    if (pb == NULL || p_buf == NULL || nbytes <= 0) {
        return -AW_EINVAL;
    }

    pb->wr = 0;
    pb->rd = 0;
    pb->unread = 0;

    pb->buf = p_buf;
    pb->len = nbytes;

    AW_SEMC_INIT( pb->sem_rd, 0, AW_SEM_Q_PRIORITY );
    AW_SEMC_INIT( pb->sem_wr, pb->len, AW_SEM_Q_PRIORITY );

    AW_MUTEX_INIT( pb->mutex, AW_SEM_Q_PRIORITY );

    return 0;
}

aw_err_t aw_pipe_terminate(aw_pipe_t pb)
{
    if (pb == NULL) {
        return -AW_EINVAL;
    }

    AW_SEMC_TERMINATE( pb->sem_rd );
    AW_SEMC_TERMINATE( pb->sem_wr );

    AW_MUTEX_TERMINATE( pb->mutex );

    return 0;
}

aw_err_t aw_pipe_read(aw_pipe_t pb, void *p_buf, size_t nbytes, size_t *p_readbytes, int timeout)
{
    if (pb == NULL || p_buf == NULL || nbytes <= 0) {
        return -AW_EINVAL;
    }

    if (0 >= pb->unread) {
        return -AW_ENODATA;
    }

    int ret = 0;

    size_t remain = nbytes;

    *p_readbytes = 0;

    char *p = (char*)p_buf;

    while ((remain) && (0 < pb->unread)) {

        ret = AW_SEMC_TAKE( pb->sem_rd, timeout );
        if ( ret ) {
            return ret;
        }

        ret = AW_MUTEX_LOCK( pb->mutex, timeout );
        if ( ret ) {
            AW_SEMC_GIVE( pb->sem_rd );
            return ret;
        }


        *p++ = pb->buf[pb->rd];
        pb->unread--;
        pb->rd++;

        *p_readbytes = *p_readbytes + 1;

        if (pb->rd >= pb->len) {
            pb->rd = 0;
        }

        remain--;

        ret = AW_MUTEX_UNLOCK( pb->mutex );
        if ( ret ) {
            // 互斥锁释放失败，但读数据操作已经完成了
            AW_SEMC_GIVE( pb->sem_wr );
            return ret;
        }

        ret = AW_SEMC_GIVE( pb->sem_wr );
        if ( ret ) {
            return ret;
        }
    }

    return nbytes - remain;
}

aw_err_t aw_pipe_write(aw_pipe_t   pb, 
                       const void *p_buf, 
                       size_t      nbytes, 
                       size_t     *p_writebytes, 
                       int         timeout)
{
    if (pb == NULL || p_buf == NULL || nbytes <= 0) {
        return -AW_EINVAL;
    }

    if (pb->len <= pb->unread) {
        return -AW_ENOBUFS;
    }

    int ret = 0;

    size_t remain = nbytes;

    *p_writebytes = 0;

    const uint32_t tmp = (uint32_t) p_buf;
    const char *p = (const char*)tmp;

    while ((remain) && (pb->len > pb->unread)) {

        ret = AW_SEMC_TAKE( pb->sem_wr, timeout );
        if ( ret ) {
            return ret;
        }

        ret = AW_MUTEX_LOCK( pb->mutex, timeout );
        if ( ret ) {
            AW_SEMC_GIVE( pb->sem_wr );
            return ret;
        }

        pb->buf[pb->wr] = *p++;

        pb->unread++;
        pb->wr++;

        (*p_writebytes) = (*p_writebytes) + 1;

        if (pb->wr >= pb->len) {
            pb->wr = 0;
        }

        remain--;

        ret = AW_MUTEX_UNLOCK( pb->mutex );
        if ( ret ) {
            // 互斥锁释放失败，但数据已经写成功了。
            AW_SEMC_GIVE( pb->sem_rd );
            return ret;
        }

        ret = AW_SEMC_GIVE( pb->sem_rd );
        if ( ret ) {
            return ret;
        }
    }

    return 0;
}

aw_err_t aw_pipe_getbyte(aw_pipe_t pb, uint8_t *p_data, int timeout)
{
    if (pb == NULL || p_data == NULL) {
        return -AW_EINVAL;
    }

    if (0 >= pb->unread) {
        return -AW_ENODATA;
    }

    int ret = 0;

    ret = AW_SEMC_TAKE( pb->sem_rd, timeout );
    if ( ret ) {
        return ret;
    }

    ret = AW_MUTEX_LOCK( pb->mutex, timeout );
    if ( ret ) {
        AW_SEMC_GIVE( pb->sem_rd );
        return ret;
    }

    *p_data = pb->buf[pb->rd];
    pb->unread--;
    pb->rd++;

    if (pb->rd >= pb->len) {
        pb->rd = 0;
    }

    ret = AW_MUTEX_UNLOCK( pb->mutex );
    if ( ret ) {
        /* 互斥锁释放失败，但读数据操作已经完成了*/
        AW_SEMC_GIVE( pb->sem_wr );
        return ret;
    }

    ret = AW_SEMC_GIVE( pb->sem_wr );
    if ( ret ) {
        return ret;
    }

    return 0;
}

aw_err_t aw_pipe_putbyte(aw_pipe_t pb, const uint8_t data, int timeout)
{
    if (pb == NULL) {
        return -AW_EINVAL;
    }

    if (pb->len <= pb->unread) {
        return -AW_ENOBUFS;
    }

    int ret = 0;

    ret = AW_SEMC_TAKE( pb->sem_wr, timeout );
    if ( ret ) {
        return ret;
    }

    ret = AW_MUTEX_LOCK( pb->mutex, timeout );
    if ( ret ) {
        AW_SEMC_GIVE( pb->sem_wr );
        return ret;
    }

    pb->buf[pb->wr] = data;

    pb->unread++;
    pb->wr++;

    if (pb->wr >= pb->len) {
        pb->wr = 0;
    }

    ret = AW_MUTEX_UNLOCK( pb->mutex );
    if ( ret ) {
        /* 互斥锁释放失败，但数据已经写成功了。*/
        AW_SEMC_GIVE( pb->sem_rd );
        return ret;
    }

    ret = AW_SEMC_GIVE( pb->sem_rd );
    if ( ret ) {
        return ret;
    }

    return 0;
}

