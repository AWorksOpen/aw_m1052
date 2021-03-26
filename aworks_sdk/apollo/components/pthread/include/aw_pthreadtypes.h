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
 * \file   pthread_private.h
 * \brief
 *
 * \internal
 * \par modification history:
 * - 1.00 15-11-11  dcf, first implementation.
 * \endinternal
 */

#ifndef __PTHREAD_PRIVATE_H
#define __PTHREAD_PRIVATE_H
#include <stdint.h>
#include "apollo.h"
#ifdef __PTHREAD_PRIVATE__
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_types.h"
#include "aw_isr_defer.h"
#else
#include <stdlib.h>
#endif
#include "aw_time.h"

#define __USE_UNIX98 1
#define __USE_XOPEN2K8 1
#define __USE_GNU

#ifndef __GNU__
#define __restrict
#endif

# define __SIZEOF_PTHREAD_ATTR_T 36
# define __SIZEOF_PTHREAD_MUTEX_T 24
# define __SIZEOF_PTHREAD_MUTEXATTR_T 4
# define __SIZEOF_PTHREAD_COND_T 48
# define __SIZEOF_PTHREAD_CONDATTR_T 4
# define __SIZEOF_PTHREAD_RWLOCK_T 32
# define __SIZEOF_PTHREAD_RWLOCKATTR_T 8
# define __SIZEOF_PTHREAD_BARRIER_T 20
# define __SIZEOF_PTHREAD_BARRIERATTR_T 4
# define __SIZEOF_PTHREAD_CLEANUP_EXTRA_T 24
# define __SIZEOF_PTHREAD_CPUSET_T      8



struct pthread;
typedef struct pthread *pthread_t;

/* typedef int32_t clockid_t; */
typedef int32_t key_t;       /* Used for interprocess communication. */
/* typedef int32_t pid_t; */ /* Used for process IDs and process group IDs. */

typedef union
{
    char __size[__SIZEOF_PTHREAD_CONDATTR_T];
    int __align;
} pthread_condattr_t;

typedef uint32_t pthread_cond_attr_t;
struct sched_param;

typedef union
{
#ifdef __PTHREAD_PRIVATE__
    struct
    {
        uint32_t __type;        /*  */
    }__data;
#endif
    char __size[__SIZEOF_PTHREAD_MUTEXATTR_T];
    int __align;
} pthread_mutexattr_t;

typedef union
{
#ifdef __PTHREAD_PRIVATE__
    struct {
        pthread_mutexattr_t __attr;
        void *__mutex;
        pthread_t __owner;
    }__data;
#endif
    char __size[__SIZEOF_PTHREAD_MUTEX_T];
    long int __align;
}pthread_mutex_t;

typedef union
{
#ifdef __PTHREAD_PRIVATE__
    struct
    {
        AW_SEMC_DECL(__semc);
        pthread_mutex_t __lock;
        pthread_cond_attr_t __attr;
    } __data;
#endif
    char __size[__SIZEOF_PTHREAD_COND_T];
    long long int __align;
} pthread_cond_t;


typedef union
{
#ifdef __PTHREAD_PRIVATE__
    struct
    {
        uint32_t __stack_size;
        uint32_t __priority;
        uint32_t __state;/* PTHREAD_CREATE_JOINABLE or PTHREAD_CREATE_DETACHED */
    }__data;
#endif
    char __size[__SIZEOF_PTHREAD_ATTR_T];
    long int __align;
}pthread_attr_t;

typedef union
{
#ifdef __PTHREAD_PRIVATE__
    struct {
    }__data;
#endif
    char __size[__SIZEOF_PTHREAD_RWLOCKATTR_T];
    long int __align;
}pthread_rwlockattr_t;

typedef union
{
#ifdef __PTHREAD_PRIVATE__
    struct {
    }__data;
#endif
    char __size[__SIZEOF_PTHREAD_RWLOCK_T];
    long int __align;
}pthread_rwlock_t;


typedef union
{
#ifdef __PTHREAD_PRIVATE__
    struct {
    }__data;
#endif
    char __size[__SIZEOF_PTHREAD_CPUSET_T];
    long int __align;
}cpu_set_t;

/* Keys for thread-specific data */
typedef unsigned int pthread_key_t;
/* Once-only execution */
typedef int pthread_once_t;


#ifdef __cplusplus
extern "C"{
#endif



#ifdef __cplusplus
}
#endif


#endif /* __PTHREAD_PRIVATE_H */
