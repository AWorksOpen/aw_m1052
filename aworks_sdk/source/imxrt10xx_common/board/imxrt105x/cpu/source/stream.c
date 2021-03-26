#include "stream.h"
#include "aw_vdebug.h"
#include "aw_common.h"
#include "aw_delay.h"
#include "aw_mem.h"
#include "fs/aw_mount.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"

int stream_init_mem(stream_t *p_stream,const uint8_t*p_buf,int len)
{
    memset(p_stream,0,sizeof(*p_stream));
    p_stream->type = STREAM_TYPE_MEMORY;
    p_stream->p_buf = (uint8_t *)p_buf;
    p_stream->mem_len = len;

    return 0;
}

int stream_init_file(stream_t *p_stream,const char *file)
{
    int             fd;
    memset(p_stream,0,sizeof(*p_stream));
    p_stream->type = STREAM_TYPE_FILE;
    p_stream->fd = -1;

    fd = open(file,O_RDONLY,0);
    if ( 0 > fd) {
        return errno;
    }

    p_stream->fd = fd;
    return 0;
}

static ssize_t stream_read_mem(stream_t *p_stream, void *buf, size_t nbyte)
{
    int         len;
    len = p_stream->mem_len - p_stream->pos;
    if (len > nbyte) {
        len = nbyte;
    }
    memcpy(buf,p_stream->p_buf+p_stream->pos,len);
    p_stream->pos += len;
    return len;
}

static ssize_t stream_read_file(stream_t *p_stream, void *buf, size_t nbyte)
{
    int         len;
    len = read(p_stream->fd,buf,nbyte);
    return len;
}

ssize_t stream_read(stream_t *p_stream, void *buf, size_t nbyte)
{
    if (STREAM_TYPE_MEMORY == p_stream->type) {
        return stream_read_mem(p_stream,buf,nbyte);
    }
    return stream_read_file(p_stream,buf,nbyte);
}

off_t stream_lseek_mem(stream_t *p_stream, off_t offset, int whence)
{
    off_t               ret = -1;
    switch (whence) {
    case SEEK_SET:
        p_stream->pos = offset;
        break;
    case SEEK_CUR:
        p_stream->pos += offset;
        break;
    case SEEK_END:
        p_stream->pos = p_stream->mem_len +offset;
        break;
    default:
        return ret;
    }

    if (p_stream->pos > p_stream->mem_len) {
        p_stream->pos = p_stream->mem_len;
    }
    if (p_stream->pos < 0) {
        p_stream->pos = 0;
    }
    ret = p_stream->pos;
    return ret;
}

off_t stream_lseek_file(stream_t *p_stream, off_t offset, int whence)
{
    off_t               ret = -1;
    ret = lseek(p_stream->fd,offset,whence);
    return ret;
}

off_t stream_lseek(stream_t *p_stream, off_t offset, int whence)
{
    if (STREAM_TYPE_MEMORY == p_stream->type) {
        return stream_lseek_mem(p_stream,offset,whence);
    }
    return stream_lseek_file(p_stream,offset,whence);
}

void stream_close(stream_t *p_stream)
{
    if (STREAM_TYPE_FILE == p_stream->type) {
        close(p_stream->fd);
        p_stream->fd = -1;
    }
}
