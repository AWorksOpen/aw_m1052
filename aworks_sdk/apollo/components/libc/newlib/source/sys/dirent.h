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
 * \brief format of directory entries.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-26 orz, first implementation.
 * \endinternal
 */
 
#ifndef __RTT_DIRENT_H__
#define __RTT_DIRENT_H__

#include <rtthread.h>


/* File types */
#define FT_REGULAR        0    /* regular file */
#define FT_SOCKET         1    /* socket file  */
#define FT_DIRECTORY      2    /* directory    */
#define FT_USER           3    /* user defined */

#define DT_UNKNOWN        0x00
#define DT_REG            0x01
#define DT_DIR            0x02

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_DIR_STRUCTURE
typedef struct 
{
    int fd;                            /* directory file */
    char buf[512];
    int num;
    int cur;
} DIR;
#endif

#ifndef HAVE_DIRENT_STRUCTURE
struct dirent
{
    /** \brief The type of the file */
    rt_uint8_t  d_type;    
    /**
     * \brief The length of the not including the terminating null file name
     *
     */
    rt_uint8_t  d_namlen; 
    /** \brief length of this record */
    rt_uint16_t d_reclen;  
    /** \brief The null-terminated file name */
    char d_name[256];      
};
#endif

int            closedir(DIR *);
DIR           *opendir(const char *);
struct dirent *readdir(DIR *);
int            readdir_r(DIR *, struct dirent *, struct dirent **);
void           rewinddir(DIR *);
void           seekdir(DIR *, long int);
long           telldir(DIR *);

#ifdef __cplusplus
}
#endif

#endif
