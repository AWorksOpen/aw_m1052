
#include "curl_setup.h"

#include "apollo.h"
#include "io/aw_unistd.h"
#include "io/aw_fcntl.h"
#include "io/sys/aw_stat.h"

void* curl_io_open(const char *path, char *str)
{                                               
    int fd;                                     
    int flag=0;                                 
    flag |= (strchr(str,'r') ? O_RDONLY :0);    
    flag |= (strchr(str,'w') ? O_WRONLY :0);    
    flag |= (strchr(str,'a') ? O_APPEND :0);    
    fd = aw_open(path, flag, 777);              
    if (fd < 0) {                               
        return NULL;                              
    }                                           
    return (void*)fd;                       
}

int curl_io_write(const char *buf, size_t size,size_t n, void *fp)                        
{                                               
    if (0 == (int)fp) {
        aw_kprintf("%s", buf);
        return size*n;
    } else if (-1 == (int)fp) {
        return 0;
    }
    return aw_write(((int)fp), buf, size*n);      
}                                               

int curl_io_read(char *buf, size_t size,size_t n, void *fp)
{                                               
    if (fp == stdin) {                           
    } else if (-1 == (int)fp) {
        return 0;
    }
    return aw_read(((int)fp), buf, size*n);       
} 

int curl_io_lseek(void *fp, size_t off, int flag)
{
    return aw_lseek((int)fp, off, flag);
}

int curl_io_close(void *fp)
{
    return aw_close((int)fp);
}

int curl_io_fflush(FILE *fp)
{
    return aw_fsync((int)fp-1);
}

int curl_io_flieno(FILE *fp)
{
    return (int)fp-1;
}

int curl_io_setvbuf(FILE *fp, char *buf, int flag, size_t size)
{
    return 0;
}

int curl_io_fputc(int c, FILE *fp)                            
{                                               
    if (fp == stdout) {                          
    } 
    return 0; 
}                                               

int curl_io_fputs(char *str, FILE *fp)                           
{                                               
    if (fp == stdout) {                          
    }        
    return 0;          
}        
                                       
int curl_io_fgetc(FILE *fp)                             
{                                               
    if (fp == stdout) {                          
    }            
    return 0;
}                                               

char* curl_io_fgets(char *str, int n, FILE *fp)                           
{                                               
    if (fp == stdout) {                          
    }   
    return NULL;
}       
