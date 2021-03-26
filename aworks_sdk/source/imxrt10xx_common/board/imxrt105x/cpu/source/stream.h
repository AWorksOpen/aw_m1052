#ifndef         __STREAM_H__
#define         __STREAM_H__

#include "unistd.h"
#include "string.h"

typedef enum {
    STREAM_TYPE_FILE = 0,
    STREAM_TYPE_MEMORY = 1,
}stream_type_t;

typedef struct {
    stream_type_t       type;
    uint8_t            *p_buf;
    int                 pos;
    int                 mem_len;
    int                 fd;
}stream_t;

int stream_init_mem(stream_t *p_stream,const uint8_t*p_buf,int len);
int stream_init_file(stream_t *p_stream,const char *file);

ssize_t stream_read(stream_t *p_stream, void *buf, size_t nbyte);
off_t stream_lseek(stream_t *p_stream, off_t offset, int whence);
void stream_close(stream_t *p_stream);


#endif
