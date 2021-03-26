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
 * \brief 通用环形缓冲区
 *
 * \internal
 * \par modification history
 * - 1.02 12-12-08  orz, change type of size and nbytes to size_t.
 * - 1.01 12-10-22  orz, add prefix "aw_" to everything
 * - 1.00 12-10-15  orz, add implementation from VxWorks
 * \endinternal
 */

#include "aw_rngbuf.h"
#include "aw_common.h"          /* for min() */
#include <string.h>             /* for memcpy() */

/**
 * \addtogroup grp_aw_rngbuf
 * @{
 */

/******************************************************************************/
void aw_rngbuf_init (struct aw_rngbuf *p_rb, char *p_buf, size_t size)
{
    p_rb->in   = 0;
    p_rb->out  = 0;
    p_rb->buf  = p_buf;
    p_rb->size = size > 0 ? size : 1;
}

/******************************************************************************/
int aw_rngbuf_putchar (aw_rngbuf_t rb, const char data)
{
    uint32_t    in;
    uint32_t    out;
    int         size;

    in = READ_ONCE32(rb->in);
    out = READ_ONCE32(rb->out);
    size = rb->size;

    out = (out + size - 1) ;
    if (out >= size) {
        out -= size;
    }

    if (in != out) {
        rb->buf[in] = data;
        in = in + 1;
        if (in >= size) {
            in -= size;
        }
        aw_smp_wmb();
        WRITE_ONCE32(rb->in,in);
        return 1;
    }

    return 0;
}

/******************************************************************************/
int aw_rngbuf_getchar (aw_rngbuf_t rb, char *p_data)
{
    uint32_t    in;
    uint32_t    out;
    int         size;

    in = READ_ONCE32(rb->in);
    out = READ_ONCE32(rb->out);
    size = rb->size;

    if (out != in) {
        aw_smp_rmb();
        *p_data = rb->buf[out];
        out ++;
        if (out >= size) {
            out = 0;
        }
        WRITE_ONCE32(rb->out,out);
        return 1;
    }

    return 0;
}

/******************************************************************************/
size_t aw_rngbuf_put (aw_rngbuf_t rb, const char *p_buf, size_t nbytes)
{
    uint32_t    in;
    uint32_t    out;
    int         size;
    int         bytes_put = 0;
    int         n;

    in = READ_ONCE32(rb->in);
    out = READ_ONCE32(rb->out);
    size = rb->size;

    if (in < out) {
        n = out - in - 1;
        if (n > nbytes) {
            n = nbytes;
        }
        memcpy(&rb->buf[in],p_buf,n);
        bytes_put += n;
    }
    else {
        /* 此种情况，数据不能一次性复制完成*/
        /* 第一次复制*/
        n = out + size -in - 1;
        if (n > nbytes) {
            n = nbytes;
        }
        if (n + in > size) {
            n = size - in ;
        }
        memcpy(&rb->buf[in],p_buf,n);
        bytes_put += n;

        /* 第二次复制*/
        n = nbytes - bytes_put;
        if (out == 0) {
            n = 0;
        }
        else if (n > out - 1) {
            n = out - 1;
        }
        if (n > 0) {
            memcpy(&rb->buf[0],p_buf + bytes_put,n);
            bytes_put += n;
        }
    }

    in = (in + bytes_put);
    if (in >= size) {
        in -= size;
    }
    aw_smp_wmb();
    WRITE_ONCE32(rb->in,in);

    return (bytes_put);
}

/******************************************************************************/
size_t aw_rngbuf_get (aw_rngbuf_t rb, char *p_buf, size_t nbytes)
{
    uint32_t    in;
    uint32_t    out;
    int         size;
    int         bytes_got = 0;
    int         n;

    in = READ_ONCE32(rb->in);
    out = READ_ONCE32(rb->out);
    size = rb->size;

    /* 确保如果当前cpu看到了in的改变，一定会看到buf内容的改变*/
    aw_smp_rmb();
    if (in >= out) {
        /* in has not wrapped around */
        n = in - out;
        if (n > nbytes) {
            n = nbytes;
        }
        memcpy(p_buf, &rb->buf[out], n);
        bytes_got += n;
    } else {
        /*
         * in has wrapped around.  Grab chars up to the size of the
         * buffer, then wrap around if we need to.
         */
        n = size - out;
        if (n > nbytes) {
            n = nbytes;
        }

        memcpy(p_buf, &rb->buf[out], n);
        bytes_got += n;

        n = in;
        if (n > (nbytes - bytes_got)) {
            n = nbytes - bytes_got;
        }
        if (n > 0) {
            memcpy(p_buf + bytes_got, &rb->buf[0], n);
            bytes_got += n;
        }
    }
    out = out + bytes_got;
    if (out >= size) {
        out -= size;
    }

    WRITE_ONCE32(rb->out,out);

    return (bytes_got);
}

/******************************************************************************/
void aw_rngbuf_flush (aw_rngbuf_t rb)
{
    uint32_t        in;
    in = READ_ONCE32(rb->in);
    WRITE_ONCE32(rb->out,in);
}

/******************************************************************************/
aw_bool_t aw_rngbuf_isempty (aw_rngbuf_t rb)
{
    uint32_t    in;
    uint32_t    out;

    in = READ_ONCE32(rb->in);
    out = READ_ONCE32(rb->out);

    return (in == out);
}

/******************************************************************************/
aw_bool_t aw_rngbuf_isfull (aw_rngbuf_t rb)
{
    uint32_t    in;
    uint32_t    out;

    in = READ_ONCE32(rb->in);
    out = READ_ONCE32(rb->out);

    in = in - out + 1;

    return ((in == 0) || (in == rb->size));
}

/******************************************************************************/
size_t aw_rngbuf_freebytes (aw_rngbuf_t rb)
{
    uint32_t    in;
    uint32_t    out;

    in = READ_ONCE32(rb->in);
    out = READ_ONCE32(rb->out);

    out = out + rb->size - in - 1;
    if (out >=rb->size) {
        out -= rb->size;
    }

    return out;
}

/******************************************************************************/
size_t aw_rngbuf_nbytes (aw_rngbuf_t rb)
{
    uint32_t    in;
    uint32_t    out;

    in = READ_ONCE32(rb->in);
    out = READ_ONCE32(rb->out);

    out = in + rb->size - out;
    if (out >=rb->size) {
        out -= rb->size;
    }
    return out;
}

/******************************************************************************/
void aw_rngbuf_put_ahead (aw_rngbuf_t rb, char byte, size_t offset)
{
    int n = rb->in + offset;

    if (n >= rb->size) {
        n -= rb->size;
    }

    *(rb->buf + n) = byte;
}

/******************************************************************************/
void aw_rngbuf_move_ahead (aw_rngbuf_t rb, size_t n)
{
    n += rb->in;

    if (n >= rb->size) {
        n -= rb->size;
    }

    rb->in = n;
}

/** @} */

/* end of file */
