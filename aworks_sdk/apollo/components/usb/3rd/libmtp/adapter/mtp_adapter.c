#include "aworks.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "mtp_adapter.h"
#include "string.h"
#include "stdio.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"

/* �����ڴ����亯��*/
void *mtp_malloc(unsigned int size){
    return aw_mem_alloc(size);
}

/* �ͷ��ڴ����亯��*/
void mtp_free(void *ptr){
    aw_mem_free(ptr);
}

/* ���·����ڴ����亯��*/
void *mtp_realloc(void *ptr, size_t newsize){
    return aw_mem_realloc(ptr, newsize);
}

/* �����ڴ沢��ʼ�����亯��*/
void *mtp_calloc(size_t nelem, size_t size){
    return aw_mem_calloc(nelem, size);
}
/* ��ȡhome·��*/
char *mtp_get_home_path(void){
    return NULL;
}

/* ΢����ʱ���亯��*/
void mtp_usleep(uint32_t us){
    aw_udelay(us);
}


/* ��ָ�������ж�ȡ���ݣ�ÿ�ζ�ȡһ��*/
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

/* ���ļ�*/
int mtp_open (const char *path, int oflag, mode_t mode){
	return aw_open(path, oflag, mode);
}

/* �ر��ļ�*/
int mtp_close (int index){
	return aw_close(index);
}

/* ��ȡ�ļ�����*/
ssize_t mtp_read (int index, void *p_buf, size_t nbyte){
	return aw_read(index, p_buf, nbyte);
}

/* ���ļ���д����*/
ssize_t mtp_write (int index, void *p_buf, size_t nbyte){
	return aw_write(index, p_buf, nbyte);
}

/* ɾ���ļ�*/
int mtp_unlink (const char *path){
	return aw_unlink(path);
}

int mtp_lseek (int fildes, int offset, int whence){
	return aw_lseek(fildes, offset, whence);
}
