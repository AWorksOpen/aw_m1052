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
 

#include "pthread.h"
#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_assert.h"

pthread_mutex_t lock;
pthread_mutex_t lock_timedout;

static void *_threada( void *a)
{
    int index = 0;
    while (1)
    {
        int err;
        err = pthread_mutex_lock( &lock );
        aw_kprintf("thread %s ", a);
        aw_task_delay( 100 );
        aw_kprintf(" %s running %d err=%d\r\n", a, index++, err );
        err = pthread_mutex_unlock( &lock );
        aw_assert(err == 0 );
        aw_task_delay( 2 );
        if ( index > 3 )
        {
            pthread_exit( (void*)1234567 );
        }
    }
    return (void*)5656;
}

static void *_threadb( void *a)
{
    int index = 0;
    while (1)
    {
        int err;
        err = pthread_mutex_lock( &lock );
        aw_kprintf("thread %s ", a);
        aw_task_delay( 100 );
        aw_kprintf(" %s running %d err=%d\r\n", a, index++, err );
        err = pthread_mutex_unlock( &lock );
        aw_assert(err == 0 );
        aw_task_delay( 2 );
        if ( index > 3 )
        {
            pthread_exit( (void*)12345678 );
        }
    }
    return (void*)5656;
}

static void *_thread2( void *a)
{
    int index = 0;
    while (1)
    {
        int err;
        err = pthread_mutex_lock( &lock );
        aw_kprintf("thread %s ", a);
        aw_task_delay( 100 );
        aw_kprintf(" %s running %d err=%d\r\n", a, index++, err );
        err = pthread_mutex_unlock( &lock );
        aw_assert(err == 0 );
        aw_task_delay( 2 );
        if ( index > 3 )
        {
            break;
        }
    }
    return (void*)5656;
}


static void *_thread_mutex_timedout_test( void *a)
{
    int index = 0;
    while (1)
    {
        aw_err_t err;
        struct timespec tv;

        aw_timespec_get( (struct aw_timespec*)&tv );

        tv.tv_sec += 5;/* after 5 seconds. */

        err = pthread_mutex_timedlock( &lock_timedout, &tv );

        if ( err == AW_ETIMEDOUT )
        {
            aw_kprintf("pthread mutex timeout test\r\n");
        }
        if ( ++index > 3 )
        {
            break;
        }
    }
    return (void*)5656;
}


void test_pthread( void )
{
    pthread_t thread_a = 0;
    pthread_t thread_b = 0;
    pthread_t thread_c = 0;
    pthread_attr_t attr;
    void *ret;

    pthread_attr_init( &attr );
    pthread_attr_setstacksize( &attr, 8888 );

    pthread_mutex_init( &lock, NULL );
    pthread_mutex_init( &lock_timedout, NULL );
    pthread_mutex_lock( &lock_timedout ); /* lock by this thread, so other thread will not get the lock. */

    pthread_create( &thread_a, 0, _threada, "aaaa" );
    pthread_create( &thread_b, &attr, _threadb, "bbbb" );
    pthread_create( &thread_c, &attr, _thread2, "cccc" );

    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );/* this feature is not supported by apollo, ignored */
    pthread_create( 0, &attr, _thread_mutex_timedout_test, "dddd" );

    pthread_attr_destroy( &attr );

    if ( 0 == pthread_join( thread_a, &ret ) )
    {
        aw_kprintf("thread a join ok, ret=%d\r\n", (int)ret );
        aw_assert( (int)ret == 1234567 );
    }
    if ( 0 == pthread_join( thread_b, &ret ) )
    {
        aw_kprintf("thread b join ok, ret=%d\r\n", (int)ret );
        aw_assert( (int)ret == 12345678 );
    }
    if ( 0 == pthread_join( thread_c, &ret ) )
    {
        aw_kprintf("thread c join ok, ret=%d\r\n", (int)ret );
        aw_assert( (int)ret == 5656 );
    }

}


