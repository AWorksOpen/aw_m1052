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
 * \brief RTK types 标准定义
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-28  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_TYPES_H
#define __AW_PSP_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */


#if defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION)
#include <stdint.h>  /* for standard int type */
#include <time.h>

#if defined (__CC_ARM) /* ARM Compiler V5 */
#pragma anon_unions
#endif

typedef unsigned int   size_t;
typedef long           ssize_t;


/* typedef long          time_t;*/
/* typedef unsigned long clock_t;*/


/* define in type.h */
typedef int            mode_t;
typedef int            off_t;
typedef short          dev_t;
typedef unsigned short ino_t;
typedef unsigned short nlink_t;
typedef unsigned short uid_t;
typedef unsigned short gid_t;
struct timespec {
  long    tv_sec;   /* Seconds */
  long    tv_nsec;  /* Nanoseconds */
};

#ifndef _BLKSIZE_T_DECLARED
typedef long blksize_t;
#define _BLKSIZE_T_DECLARED
#endif

#ifndef _BLKCNT_T_DECLARED
typedef long blkcnt_t;
#define _BLKCNT_T_DECLARED
#endif

#define aw_psp_bool_t  uint8_t
#define AW_PSP_TRUE    1
#define AW_PSP_FALSE   0

#elif defined (__ICCARM__)

#elif defined   (__GNUC__)
#include <sys/types.h>
/*
 *  高版本的newlib中,sys/types.h不再定义struct timespec
 *  需要包含time.h
 *  TODO: 与awlibc中的time.h有冲突。不能包含time.h
 */
/* #include <time.h> */
#include <stdint.h>  /* for standard int type */
#include <stddef.h>  /* for size_t */
#include <stdbool.h> /* for bool */

#define aw_psp_bool_t  _Bool
#define AW_PSP_TRUE    1
#define AW_PSP_FALSE   0

#ifndef _TIMEVAL_DEFINED
struct timeval {
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* and microseconds */
};
#define _TIMEVAL_DEFINED
#endif

#ifndef _BLKSIZE_T_DECLARED
typedef long blksize_t;
#define _BLKSIZE_T_DECLARED
#endif

#ifndef _BLKCNT_T_DECLARED
typedef long blkcnt_t;
#define _BLKCNT_T_DECLARED
#endif

#elif defined   (__TASKING__)

#elif defined   (WIN32)


#endif /* __CC_ARM */

typedef unsigned long   aw_psp_tick_t;
typedef uint32_t        aw_psp_timestamp_freq_t;

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AW_PSP_TYPES_H    */

/* end of file */
