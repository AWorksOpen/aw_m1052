
#include "mbedtls/config.h"

#include "apollo.h"
#include "io/aw_unistd.h"
#include "io/aw_fcntl.h"
#include "io/sys/aw_stat.h"

int mbedtls_io_fflush(FILE *fp)
{
    return aw_fsync((int)fp-1);
}

int mbedtls_io_flieno(FILE *fp)
{
    return (int)fp-1;
}

int mbedtls_io_setvbuf(FILE *fp, char *buf, int flag, size_t size)
{
    return 0;
}

int mbedtls_io_fputc(int c, FILE *fp)                            
{                                               
    if (fp == stdout) {                          
    } 
    return 0; 
}                                               

int mbedtls_io_fputs(char *str, FILE *fp)                           
{                                               
    if (fp == stdout) {                          
    }        
    return 0;          
}        
                                       
int mbedtls_io_fgetc(FILE *fp)                             
{                                               
    if (fp == stdout) {                          
    }            
    return 0;
}                                               

char* mbedtls_io_fgets(char *str, int n, FILE *fp)                           
{                                               
    if (fp == stdout) {                          
    }   
    return NULL;
}       
