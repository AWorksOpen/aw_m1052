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
 
#include <string.h>
#include "aw_mem.h"

/* need private header */
#define __PTHREAD_PRIVATE__
#include "pthread.h"
#include "aw_system.h"
#include "aw_vdebug.h"
#include "aw_delay.h"

#undef timersub
#define timersub(a, b, result)                              \
    do {                                                    \
        (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;       \
        (result)->tv_nsec = (a)->tv_nsec - (b)->tv_nsec;    \
        if ( (int)(result)->tv_sec > 0 )                         \
        {                                                   \
            if ((int)(result)->tv_nsec < 0) {                    \
                --(result)->tv_sec;                         \
                (result)->tv_nsec += 1000000000LL;          \
            }                                               \
        } else if ( (int)(result)->tv_sec < 0 ) {                \
            if ((int)(result)->tv_nsec > 0) {                    \
                ++(result)->tv_sec;                         \
                (result)->tv_nsec -= 1000000000LL;          \
            }                                               \
        }                                                   \
    } while (0)

#define __PTHREAD_MAGIC (~0xdeedbeef)
#define __IS_PTHREAD_VALID(p) ((p)->magic == __PTHREAD_MAGIC)
#define __MAKE_INVALID(p) ((p)->magic = 0)
#define __MAKE_VALID(p) ((p)->magic = __PTHREAD_MAGIC)

struct pthread
{
    pthread_attr_t attr;
    aw_task_id_t tid;
    char name[16];

    uint32_t magic;             /**< magic word  */

    void *(*start_routine)( void *);
    void *arg;

    int is_joinable;

    void *return_value;

    struct _pthread_cleanup_buffer *cleanup;

    AW_SEMB_DECL(join);
    aw_task_id_t __task_id;

    void **thread_local_storage; /* thread local storage area */
};

/* increase always */
static uint32_t g_pthread_thread_index;

/* default attr */
static pthread_attr_t g_default_pthread_attr_t = {
    {
        32*1024,/* __stack_size */
        0,      /* set by pthread_system_init() */
        PTHREAD_CREATE_JOINABLE,
    }
};


/* declare */
static void __pthread_cleanup( struct pthread *thiz );
static void pthread_system_init( void );
static void __pthread_lib_check_init( void );
static int pthread_kill( pthread_t pthread );



static void __pthread_lib_check_init( void )
{
    static int inited;
    if ( !inited )
    {
        pthread_system_init();
    }
    inited = 1;
}

/* call this first to init this lib */
static void pthread_system_init( void )
{
    g_default_pthread_attr_t.__data.__priority = aw_task_lowest_priority() - 2;
    g_default_pthread_attr_t.__data.__stack_size = 32*1024;
}

static void __pthread_entry( struct pthread *p_this )
{
    if ( p_this->start_routine ) {
        p_this->return_value = (*p_this->start_routine)( p_this->arg );
        while (1) {
            aw_mdelay( 1000 );      /* wait for delete */
        }
    }
}

/**
 *  \brief pthread create
 *
 *  Create a new thread, starting with execution of START-ROUTINE
 *  getting passed ARG.  Creation attributed come from ATTR.  The new
 *  handle is stored in *NEWTHREAD.
 */
int pthread_create (pthread_t *__restrict __newthread,
    const pthread_attr_t *__restrict __attr,
    void *(*__start_routine) (void *),
    void *__restrict __arg)
{
    struct pthread *p;

    __pthread_lib_check_init();
    if ( __newthread )
    {
        *__newthread =  0;
    }

    if ( __attr == NULL )
    {
        __attr = &g_default_pthread_attr_t;
    }

    p = aw_mem_calloc( 1, sizeof(*p) );
    if (p)
    {
        memset(p,0,sizeof(*p));
        memcpy( &p->attr, __attr, sizeof(p->attr) );
        aw_snprintf(p->name, sizeof(p->name), "pt%d", g_pthread_thread_index++ );
        p->start_routine = __start_routine;
        p->arg = __arg;
        AW_SEMB_INIT( p->join, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY );
        __MAKE_VALID( p );
        if ( __newthread )
        {
            *__newthread = (pthread_t)p;
        }
        if ( __attr->__data.__state == PTHREAD_CREATE_JOINABLE )
        {
            p->is_joinable = 1;
        } else {
            aw_kprintf("pthread warning: not support PTHREAD_CREATE_DETACHED now. Forced to PTHREAD_CREATE_JOINABLE.\r\n");
            p->is_joinable = 1; /*  */
        }
        p->tid = aw_task_create( p->name,
                __attr->__data.__priority,
                __attr->__data.__stack_size,
                (aw_pfuncvoid_t)__pthread_entry, p );
        aw_task_set_tls( p->tid, p );
        aw_task_startup( p->tid );

        return 0;
    }
    return -1;
}

static int pthread_kill( pthread_t pthread )
{
    __pthread_cleanup( pthread );
    AW_SEMB_GIVE( pthread->join );
    if ( !pthread->is_joinable )
    {
        pthread_detach( pthread );
    }
    return 0;
}

/**
 * \brief Terminate calling thread.
 *
 * The registered cleanup handlers are called via exception handling
 * so we cannot mark this function with __THROW.
 */
void pthread_exit (void *__retval)
{
    pthread_t thiz = pthread_self();
    thiz->return_value = __retval;
    pthread_kill( thiz );
}

/**
 * \brief
 *
 * Make calling thread wait for termination of the thread TH.  The
 *  exit status of the thread is stored in *THREAD_RETURN, if THREAD_RETURN
 *  is not NULL.
 *
 *  This function is a cancellation point and therefore not marked with
 *  __THROW.
 */
int pthread_join (pthread_t __th, void **__thread_return)
{
    int err;
    if ( __th == pthread_self())
    {
        return AW_EDEADLK;
    }
    if ( !__th->is_joinable )
    {
        return AW_EINVAL;
    }
    err = -AW_SEMB_TAKE( __th->join, AW_SEM_WAIT_FOREVER );
    if ( __thread_return )
    {
        *__thread_return = __th->return_value;
    }
    pthread_detach( __th );
    return err;
}

#ifdef __USE_GNU
/**
 * \brief Check whether thread TH has terminated
 *
 * Check whether thread TH has terminated.  If yes return the status of
 * the thread in *THREAD_RETURN, if THREAD_RETURN is not NULL.
 */
int pthread_tryjoin_np (pthread_t __th, void **__thread_return)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* NOT supported */

/**
 * \brief Check whether thread TH has terminated
 *
 *  Make calling thread wait for termination of the thread TH, but only
 *  until TIMEOUT.  The exit status of the thread is stored in
 *  *THREAD_RETURN, if THREAD_RETURN is not NULL.
 *
 *  This function is a cancellation point and therefore not marked with
 *  __THROW.
 */
int pthread_timedjoin_np (pthread_t __th, void **__thread_return,
    const struct timespec *__abstime)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */
#endif

/**
 * \brief pthread detach
 *
 *  Indicate that the thread TH is never to be joined with PTHREAD_JOIN.
 *  The resources of TH will therefore be freed immediately when it
 *  terminates, instead of waiting for another thread to perform PTHREAD_JOIN
 *  on it.
 */
int pthread_detach( pthread_t thiz )
{
    if ( !__IS_PTHREAD_VALID(thiz) )
    {
        return AW_ESRCH;
    }

    if ( thiz->cleanup )
    {
        pthread_kill( thiz );
    }

    aw_task_delete( thiz->tid );
    AW_SEMB_TERMINATE( thiz->join );
    __MAKE_INVALID(thiz);

    aw_mem_free( thiz );
    return 0;
}


/**
 *  \brief  Obtain the identifier of the current thread.
 *
 *  Obtain the identifier of the current thread.
 */
pthread_t pthread_self (void)
{
    return (pthread_t)aw_task_get_tls(aw_task_id_self());
}

/**
 *  \brief Compare two thread identifiers
 *   Compare two thread identifiers.
 */
int pthread_equal (pthread_t __thread1, pthread_t __thread2)
{
    return __thread1 == __thread2;
}


/* Thread attribute handling.  */

/**
 * \brief  Initialize thread attribute *ATTR with default attributes
 *
 * Initialize thread attribute *ATTR with default attributes
 * (detachstate is PTHREAD_JOINABLE, scheduling policy is SCHED_OTHER,
 * no user-provided stack).
 */
int pthread_attr_init (pthread_attr_t *__attr)
{
    __pthread_lib_check_init();

    if ( __attr )
    {
        *__attr = g_default_pthread_attr_t;
    }
    return 0;
}

/**
 *  \brief Destroy thread attribute *ATTR.
 *
 * Destroy thread attribute *ATTR.
 */
int pthread_attr_destroy (pthread_attr_t *__attr)
{
    (void)__attr;
    return 0;
}



/* Return the currently used minimal stack size.  */
int pthread_attr_getstacksize (const pthread_attr_t *__restrict
    __attr, size_t *__restrict __stacksize)
{
    if ( __attr && __stacksize )
    {
        *__stacksize = __attr->__data.__stack_size;
        return 0;
    }
    return AW_EINVAL;
}

/* Add information about the minimum stack size needed for the thread
   to be started.  This size must never be less than PTHREAD_STACK_MIN
   and must also not exceed the system limits.  */
int pthread_attr_setstacksize (pthread_attr_t *__attr,
    size_t __stacksize)
{
    if ( __attr )
    {
        __attr->__data.__stack_size = __stacksize;
        return 0;
    }
    return AW_EINVAL;
}/* apollo NOT supported now */


/**
 * \brief  Get detach state attribute.
 *
 * Get detach state attribute.
 */
int pthread_attr_getdetachstate (const pthread_attr_t *__attr,
    int *__detachstate)
{
    if ( __detachstate && __attr )
    {
        *__detachstate = __attr->__data.__state;
        return 0;
    }
    return AW_EINVAL;
}

/**
 * \brief  Set detach state attribute.
 *
 * Set detach state attribute.
 */
int pthread_attr_setdetachstate (pthread_attr_t *__attr,
    int __detachstate)
{
    if ( __detachstate && __attr )
    {
        __attr->__data.__state = __detachstate;
        return 0;
    }
    return AW_EINVAL;
}

#if 0

/**
 *   \brief Get the size of the guard area created for stack overflow protection.
 */
int pthread_attr_getguardsize (const pthread_attr_t *__attr, size_t *__guardsize)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/**
 *   Set the size of the guard area created for stack overflow protection.
 */
int pthread_attr_setguardsize (pthread_attr_t *__attr, size_t __guardsize)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */


/**
 *   Return in *PARAM the scheduling parameters of *ATTR.
 */
int pthread_attr_getschedparam (const pthread_attr_t *__restrict __attr,
    struct sched_param *__restrict __param)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Set scheduling parameters (priority, etc) in *ATTR according to PARAM.  */
int pthread_attr_setschedparam (pthread_attr_t *__restrict __attr,
    const struct sched_param *__restrict __param)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Return in *POLICY the scheduling policy of *ATTR.  */
int pthread_attr_getschedpolicy (const pthread_attr_t *__restrict
    __attr, int *__restrict __policy)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Set scheduling policy in *ATTR according to POLICY.  */
int pthread_attr_setschedpolicy (pthread_attr_t *__attr, int __policy)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Return in *INHERIT the scheduling inheritance mode of *ATTR.  */
int pthread_attr_getinheritsched (const pthread_attr_t *__restrict
    __attr, int *__restrict __inherit)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Set scheduling inheritance mode in *ATTR according to INHERIT.  */
int pthread_attr_setinheritsched (pthread_attr_t *__attr,
    int __inherit)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */


/* Return in *SCOPE the scheduling contention scope of *ATTR.  */
int pthread_attr_getscope (const pthread_attr_t *__restrict __attr,
    int *__restrict __scope)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Set scheduling contention scope in *ATTR according to SCOPE.  */
int pthread_attr_setscope (pthread_attr_t *__attr, int __scope)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Return the previously set address for the stack.  */
int pthread_attr_getstackaddr (const pthread_attr_t *__restrict
    __attr, void **__restrict __stackaddr)
{

    return 0;
}

/* Set the starting address of the stack of the thread to be created.
   Depending on whether the stack grows up or down the value must either
   be higher or lower than all the address in the memory block.  The
   minimal size of the block must be PTHREAD_STACK_MIN.  */
int pthread_attr_setstackaddr (pthread_attr_t *__attr,
    void *__stackaddr)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */



#ifdef __USE_XOPEN2K
/* Return the previously set address for the stack.  */
int pthread_attr_getstack (const pthread_attr_t *__restrict __attr,
    void **__restrict __stackaddr,
    size_t *__restrict __stacksize)
{

}

/* The following two interfaces are intended to replace the last two.  They
 * require setting the address as well as the size since only setting the
 * address will make the implementation on some architectures impossible.  
 */
int pthread_attr_setstack (pthread_attr_t *__attr, void *__stackaddr,
    size_t __stacksize)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */
#endif

#ifdef __USE_GNU
/* Thread created with attribute ATTR will be limited to run only on
 * the processors represented in CPUSET.  
 */
int pthread_attr_setaffinity_np (pthread_attr_t *__attr,
    size_t __cpusetsize,
    const cpu_set_t *__cpuset)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Get bit set in CPUSET representing the processors threads created with
 * ATTR can run on.  
 */
int pthread_attr_getaffinity_np (const pthread_attr_t *__attr,
    size_t __cpusetsize,
    cpu_set_t *__cpuset)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Get the default attributes used by pthread_create in this process.  */
int pthread_getattr_default_np (pthread_attr_t *__attr)
{

}

/* Set the default attributes to be used by pthread_create in this
 * process.  
 */
int pthread_setattr_default_np (const pthread_attr_t *__attr)
{

}

/* Initialize thread attribute *ATTR with attributes corresponding to the
   already running thread TH.  It shall be called on uninitialized ATTR
   and destroyed with pthread_attr_destroy when no longer needed.  */
int pthread_getattr_np (pthread_t __th, pthread_attr_t *__attr)
{

}
#endif


/* Functions for scheduling control.  */

/* Set the scheduling parameters for TARGET_THREAD according to POLICY
   and *PARAM.  */
int pthread_setschedparam (pthread_t __target_thread, int __policy,
    const struct sched_param *__param)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Return in *POLICY and *PARAM the scheduling parameters for TARGET_THREAD. */
int pthread_getschedparam (pthread_t __target_thread,
    int *__restrict __policy,
    struct sched_param *__restrict __param)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Set the scheduling priority for TARGET_THREAD.  */
int pthread_setschedprio (pthread_t __target_thread, int __prio)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */


#ifdef __USE_GNU
/* Get thread name visible in the kernel and its interfaces.  */
int pthread_getname_np (pthread_t __target_thread, char *__buf,
    size_t __buflen)
{

}

/* Set thread name visible in the kernel and its interfaces.  */
int pthread_setname_np (pthread_t __target_thread, const char *__name)
{

}
#endif

#ifdef __USE_UNIX98
/* Determine level of concurrency.  */
int pthread_getconcurrency (void)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Set new concurrency level to LEVEL.  */
int pthread_setconcurrency (int __level)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */
#endif



#ifdef __USE_GNU
/* Yield the processor to another thread or process.
 * This function is similar to the POSIX `sched_yield' function but
 * might be differently implemented in the case of a m-on-n thread
 * implementation.  
 */
int pthread_yield (void)
{
    return 0;
}
#if 0
/* Limit specified thread TH to run only on the processors represented
 * in CPUSET.  
 */
int pthread_setaffinity_np (pthread_t __th, size_t __cpusetsize,
    const cpu_set_t *__cpuset)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Get bit set in CPUSET representing the processors TH can run on.  */
int pthread_getaffinity_np (pthread_t __th, size_t __cpusetsize,
    cpu_set_t *__cpuset)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */
#endif
#endif


/* Functions for handling initialization.  */

int pthread_once (pthread_once_t *__once_control,
    void (*__init_routine) (void))
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */


/* Set cancelability state of current thread to STATE, returning old
 * state in *OLDSTATE if OLDSTATE is not NULL.  
 */
int pthread_setcancelstate (int __state, int *__oldstate)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Set cancellation state of current thread to TYPE, returning the old
   type in *OLDTYPE if OLDTYPE is not NULL.  */
int pthread_setcanceltype (int __type, int *__oldtype)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Cancel THREAD immediately or at the next possibility.  */
int pthread_cancel (pthread_t __th)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

/* Test for pending cancellation for the current thread and terminate
 * the thread as per pthread_exit(PTHREAD_CANCELED) if it has been
 * cancelled.  
 */
void pthread_testcancel (void)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    while (1);
}/* apollo NOT supported now */

#endif

/* Mutex handling.  */
typedef struct {
    AW_MUTEX_DECL(lock);
}pthread_mutex_private_t;

/* Initialize a mutex.  */
int pthread_mutex_init (pthread_mutex_t *__mutex,
    const pthread_mutexattr_t *__mutexattr)
{
    pthread_mutex_private_t *lock;

    lock = (pthread_mutex_private_t *)aw_mem_calloc(1, sizeof( pthread_mutex_private_t) );
    if ( !lock )
    {
        return -1;
    }
    memset(lock,0,sizeof(*lock));
    __mutex->__data.__mutex = lock;
    AW_MUTEX_INIT(lock->lock, AW_SEM_INVERSION_SAFE);
    return 0;
}

/* Destroy a mutex.  */
int pthread_mutex_destroy (pthread_mutex_t *__mutex)
{
    pthread_mutex_private_t *lock;
    if ( (!__mutex)
            || (0 == (lock = (pthread_mutex_private_t *)__mutex->__data.__mutex)))
    {
        return AW_EINVAL;
    }
    AW_MUTEX_TERMINATE( lock->lock );
    aw_mem_free( lock );
    __mutex->__data.__mutex = 0;
    return 0;
}


/* Try locking a mutex.  */
int pthread_mutex_trylock (pthread_mutex_t *__mutex)
{
    pthread_mutex_private_t *lock;
    aw_err_t err;

    if ( !__mutex || NULL == (lock = (pthread_mutex_private_t *)__mutex->__data.__mutex) )
    {
        return AW_EINVAL;
    }

    err = AW_MUTEX_LOCK( lock->lock, AW_SEM_WAIT_FOREVER );

    switch (err)
    {
        case AW_OK:return 0;
        case -AW_ETIME:return AW_ETIMEDOUT;
        case -AW_EAGAIN:return AW_EAGAIN;
        case -AW_EINVAL:return AW_EINVAL;
        case -AW_EDEADLK:return AW_EDEADLK;
        default: return AW_EPERM;
    }
}

/* Lock a mutex.  */
int pthread_mutex_lock (pthread_mutex_t *__mutex)
{
    pthread_mutex_private_t *lock;
    aw_err_t err;

    if ( !__mutex || NULL == (lock = (pthread_mutex_private_t *)__mutex->__data.__mutex) )
    {
        return AW_EINVAL;
    }
    err = AW_MUTEX_LOCK( lock->lock, AW_SEM_WAIT_FOREVER );

    switch (err)
    {
        case AW_OK:return 0;
        case -AW_ETIME:return AW_ETIMEDOUT;
        case -AW_EAGAIN:return AW_EAGAIN;
        case -AW_EINVAL:return AW_EINVAL;
        case -AW_EDEADLK:return AW_EDEADLK;
        default: return AW_EPERM;
    }
}

int __pthread_timespec_to_ms( const struct aw_timespec *abstime )
{
    struct aw_timespec currenttime;
    struct aw_timespec deltatime;
    unsigned int ms;

    if ( abstime )
    {
        if ( AW_OK != aw_timespec_get( &currenttime ) )
        {
            return AW_EINVAL;
        }
        timersub( abstime, &currenttime, &deltatime );

        if ( (int)deltatime.tv_sec < 0 || (int)deltatime.tv_nsec < 0 )
        {
            ms = 0;
        } else {
            ms = deltatime.tv_sec*1000LL + deltatime.tv_nsec/1000000LL;
        }
    } else {
        ms = 0;
    }
    return ms;
}

/* Wait until lock becomes available, or specified time passes. */
int pthread_mutex_timedlock (pthread_mutex_t *__restrict __mutex,
    const struct timespec *__restrict
    __abstime)
{
    pthread_mutex_private_t *lock;
    aw_err_t err = AW_ERROR;
    aw_tick_t ticks;
    unsigned int ms;

    if ( !__mutex || NULL == (lock = (pthread_mutex_private_t *)__mutex->__data.__mutex) )
    {
        return AW_EINVAL;
    }
    ms = __pthread_timespec_to_ms( (const struct aw_timespec*)__abstime );
    if ( ms == 0 )
    {
        ticks = 0;
    } else {
        ticks = aw_ms_to_ticks( ms );
    }
    err = AW_MUTEX_LOCK( lock->lock, ticks );
    switch (err)
    {
        case AW_OK:return 0;
        case -AW_ETIME:return AW_ETIMEDOUT;
        case -AW_EAGAIN:return AW_EAGAIN;
        case -AW_EINVAL:return AW_EINVAL;
        case -AW_EDEADLK:return AW_EDEADLK;
        default: return AW_EPERM;
    }
}

/* Unlock a mutex.  */
int pthread_mutex_unlock (pthread_mutex_t *__mutex)
{
    pthread_mutex_private_t *lock;
    aw_err_t err;

    if ( !__mutex || NULL == (lock = (pthread_mutex_private_t *)__mutex->__data.__mutex) )
    {
        return AW_EINVAL;
    }
    err = AW_MUTEX_UNLOCK( lock->lock );
    switch (err)
    {
        case AW_OK:return 0;
        case -AW_EINVAL:return AW_EINVAL;
        default: return AW_EPERM;
    }
}


/* Functions for handling mutex attributes.  */

/* Initialize mutex attribute object ATTR with default attributes
   (kind is PTHREAD_MUTEX_TIMED_NP).  */
int pthread_mutexattr_init (pthread_mutexattr_t *__attr)
{
    if ( __attr )
    {
        __attr->__data.__type = PTHREAD_MUTEX_RECURSIVE;
    }
    return 0;
}

/* Destroy mutex attribute object ATTR.  */
int pthread_mutexattr_destroy (pthread_mutexattr_t *__attr)
{
    __attr->__data.__type = 0;
    return 0;
}

#if 0
/* Get the process-shared flag of the mutex attribute ATTR.  */
int pthread_mutexattr_getpshared (const pthread_mutexattr_t *
    __restrict __attr,
    int *__restrict __pshared)
{
    if ( __pshared )
    {
        *__pshared = PTHREAD_PROCESS_PRIVATE;
    }
    return 0;
}/* apollo NOT supported now */

/* Set the process-shared flag of the mutex attribute ATTR.  */
int pthread_mutexattr_setpshared (pthread_mutexattr_t *__attr,
    int __pshared)
{
    aw_kprintf("%s: not supported\r\n", __FUNCTION__ );
    /* while (1); */
}/* apollo NOT supported now */
#endif

#if defined __USE_UNIX98 || defined __USE_XOPEN2K8
/* Return in *KIND the mutex kind attribute in *ATTR.  */
int pthread_mutexattr_gettype (const pthread_mutexattr_t *__restrict
    __attr, int *__restrict __kind)
{
    (void)__attr;
    if ( __kind && __attr )
    {
        *__kind = __attr->__data.__type;
    } else {
        return AW_EINVAL;
    }
    return 0;
}

/* Set the mutex kind attribute in *ATTR to KIND (either PTHREAD_MUTEX_NORMAL,
 * PTHREAD_MUTEX_RECURSIVE, PTHREAD_MUTEX_ERRORCHECK, or
 * PTHREAD_MUTEX_DEFAULT).  
 */
int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind)
{
    if ( __attr )
    {
        __attr->__data.__type = __kind;
    } else {
        return AW_EINVAL;
    }
    return 0;
}
#endif

static void __pthread_cleanup( struct pthread *thiz )
{
    struct _pthread_cleanup_buffer *clean;
    clean = thiz->cleanup;
    while ( clean )
    {
        if ( clean->__routine )
        {
            if ( clean->__canceltype == PTHREAD_CANCEL_DEFERRED
                || clean->__canceltype == PTHREAD_CANCEL_ASYNCHRONOUS )
            {
                aw_isr_defer_job_init( &clean->__data.defer, clean->__routine, clean->__arg );
                aw_isr_defer_job_add( &clean->__data.defer );
            } else {
                (*clean->__routine)( clean->__arg );
            }
        }
        clean = clean->__prev;
    }
    thiz->cleanup = NULL;
}
