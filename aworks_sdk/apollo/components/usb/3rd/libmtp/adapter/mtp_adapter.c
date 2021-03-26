#include "aworks.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "mtp_adapter.h"
#include "string.h"
#include "stdio.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"

/* 分配内存适配函数*/
void *mtp_malloc(unsigned int size){
    return aw_mem_alloc(size);
}

/* 释放内存适配函数*/
void mtp_free(void *ptr){
    aw_mem_free(ptr);
}

/* 重新分配内存适配函数*/
void *mtp_realloc(void *ptr, size_t newsize){
    return aw_mem_realloc(ptr, newsize);
}

/* 分配内存并初始化适配函数*/
void *mtp_calloc(size_t nelem, size_t size){
    return aw_mem_calloc(nelem, size);
}
/* 获取home路径*/
char *mtp_get_home_path(void){
    return NULL;
}

/* 微秒延时适配函数*/
void mtp_usleep(uint32_t us){
    aw_udelay(us);
}


/* 从指定的流中读取数据，每次读取一行*/
char *mtp_fgets(char *str, int n, MTP_FILE stream){
    int i;

    if(str == NULL){
        return NULL;
    }
    if(aw_read(stream, str, n) <= 0){
        return NULL;
    }
    for(i = 0; i < n; i++){
        if(str[i] == '\n'){
            break;
        }
    }
    if(i == n){
        return str;
    }
    memset(&str[i + 1], 0, n-(i+1));
    return str;

}

/* 打开文件*/
int mtp_open (const char *path, int oflag, mode_t mode){
	return aw_open(path, oflag, mode);
}

/* 关闭文件*/
int mtp_close (int index){
	return aw_close(index);
}

/* 读取文件数据*/
ssize_t mtp_read (int index, void *p_buf, size_t nbyte){
	return aw_read(index, p_buf, nbyte);
}

/* 往文件里写数据*/
ssize_t mtp_write (int index, void *p_buf, size_t nbyte){
	return aw_write(index, p_buf, nbyte);
}

/* 删除文件*/
int mtp_unlink (const char *path){
	return aw_unlink(path);
}

int mtp_lseek (int fildes, int offset, int whence){
	return aw_lseek(fildes, offset, whence);
}
