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
 * \file   pthread.h
 * \brief  subset of pthread
 *
 * \internal
 * \par modification history:
 * - 1.00 15-11-10  dcf, first implementation.
 * \endinternal
 */
 
#undef __AW_SEM_H
#define __PTHREAD_PRIVATE__
#include "pthread.h"


int pthread_cond_init (pthread_cond_t *__restrict thiz,
        const pthread_condattr_t *__restrict __cond_attr)
{
    if ( 0 == thiz )
    {
        return AW_EINVAL;
    }
    AW_SEMC_INIT( thiz->__data.__semc, 0,  AW_SEM_Q_PRIORITY );
    return 0;
}


int pthread_cond_destroy(pthread_cond_t *thiz)
{
    if ( 0 == thiz )
    {
        return AW_EINVAL;
    }
    AW_SEMC_TERMINATE( thiz->__data.__semc );
    return 0;
}


int pthread_cond_signal(pthread_cond_t *thiz)
{
    aw_err_t err;

    err = AW_SEMC_GIVE( thiz->__data.__semc );
    if (err == AW_OK )
        return 0;

    return -err;
}

#if defined(__RTK_H)
int pthread_cond_broadcast(pthread_cond_t *thiz)
{
    semc_broadcast( &thiz->__data.__semc );/* this is rtk API */
    return 0;
}
#else
#warning not support pthread_cond_broadcast
#endif

extern int __pthread_timespec_to_ms( const struct aw_timespec *abstime );

int pthread_cond_timedwait(pthread_cond_t        *thiz, 
                           pthread_mutex_t       *mutex, 
                           const struct timespec *abstime)
{
    int ms = __pthread_timespec_to_ms( (const struct aw_timespec *)abstime );
    aw_tick_t ticks;
    aw_err_t err;

    if ( !thiz )
    {
        return AW_EINVAL;
    }
    if ( ms <= 0 )
    {
        ticks = 0;
    } else {
        ticks = aw_ms_to_ticks( ms );
    }
    if ( 0 != pthread_mutex_unlock( mutex ) )
    {
        return AW_EDEADLK;
    }
    err = AW_SEMC_TAKE( thiz->__data.__semc, ticks );
    pthread_mutex_lock( mutex );
    switch (err)
    {
    case AW_OK:return 0;
    case -AW_ETIMEDOUT:return AW_ETIMEDOUT;
    default: return -err;
    }
}


int pthread_cond_wait(pthread_cond_t *thiz, pthread_mutex_t *mutex)
{
    aw_err_t err;

    if ( !thiz )
    {
        return AW_EINVAL;
    }
    if ( 0 != pthread_mutex_unlock( mutex ) )
    {
        return AW_EDEADLK;
    }
    err = AW_SEMC_TAKE( thiz->__data.__semc, AW_SEM_WAIT_FOREVER );
    pthread_mutex_lock( mutex );
    switch (err)
    {
    case AW_OK:return 0;
    default: return -err;
    }
}



