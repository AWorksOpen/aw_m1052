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

#ifndef __PTHREAD_H
#define __PTHREAD_H


#include "aw_pthreadtypes.h"

/* pthread provide these type */

/* time */
struct timespec;

/* Cleanup buffers */
struct _pthread_cleanup_buffer
{
    void (*__routine) (void *);             /* Function to call.  */
    void *__arg;                            /* Its argument.  */
    int __canceltype;                       /* Saved cancellation type. */
    struct _pthread_cleanup_buffer *__prev; /* Chaining of cleanup functions.  */
    struct unoin
    {
#ifdef __PTHREAD_PRIVATE__
        struct aw_isr_defer_job defer;
#endif
        char __size[__SIZEOF_PTHREAD_CLEANUP_EXTRA_T];
        long int __align;
    }__data;
};

/* Detach state.  */
enum {
        PTHREAD_CREATE_JOINABLE,
#define PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_JOINABLE
        PTHREAD_CREATE_DETACHED
#define PTHREAD_CREATE_DETACHED PTHREAD_CREATE_DETACHED
    };

/* Mutex types.  */
enum{
        PTHREAD_MUTEX_TIMED_NP,
        PTHREAD_MUTEX_RECURSIVE_NP,
        PTHREAD_MUTEX_ERRORCHECK_NP,
        PTHREAD_MUTEX_ADAPTIVE_NP
#if defined __USE_UNIX98 || defined __USE_XOPEN2K8
        ,
        PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_TIMED_NP,
        PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
        PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
        PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
#endif
#ifdef __USE_GNU
        /* For compatibility.  */
        , PTHREAD_MUTEX_FAST_NP = PTHREAD_MUTEX_TIMED_NP
#endif
    };


#ifdef __USE_XOPEN2K
/* Robust mutex or not flags.  */
enum
    {
        PTHREAD_MUTEX_STALLED,
        PTHREAD_MUTEX_STALLED_NP = PTHREAD_MUTEX_STALLED,
        PTHREAD_MUTEX_ROBUST,
        PTHREAD_MUTEX_ROBUST_NP = PTHREAD_MUTEX_ROBUST
    };
#endif


#if defined __USE_POSIX199506 || defined __USE_UNIX98
/* Mutex protocols.  */
enum
    {
        PTHREAD_PRIO_NONE,
        PTHREAD_PRIO_INHERIT,
        PTHREAD_PRIO_PROTECT
    };
#endif


#ifdef __PTHREAD_MUTEX_HAVE_PREV
# define PTHREAD_MUTEX_INITIALIZER                      \
    { { 0, 0, 0, 0, 0, __PTHREAD_SPINS, { 0, 0 } } }
# ifdef __USE_GNU
#  define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP                        \
    { { 0, 0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, __PTHREAD_SPINS, { 0, 0 } } }
#  define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP                       \
    { { 0, 0, 0, 0, PTHREAD_MUTEX_ERRORCHECK_NP, __PTHREAD_SPINS, { 0, 0 } } }
#  define PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP                         \
    { { 0, 0, 0, 0, PTHREAD_MUTEX_ADAPTIVE_NP, __PTHREAD_SPINS, { 0, 0 } } }
#  define PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP                         \
    { { 0, 0, 0, 0, PTHREAD_MUTEX_ADAPTIVE_NP, __PTHREAD_SPINS, { 0, 0 } } }

# endif
#else
# define PTHREAD_MUTEX_INITIALIZER              \
    { { 0, 0, 0, 0, 0, { __PTHREAD_SPINS } } }
# ifdef __USE_GNU
#  define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP                        \
    { { 0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, 0, { __PTHREAD_SPINS } } }
#  define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP                       \
    { { 0, 0, 0, PTHREAD_MUTEX_ERRORCHECK_NP, 0, { __PTHREAD_SPINS } } }
#  define PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP                         \
    { { 0, 0, 0, PTHREAD_MUTEX_ADAPTIVE_NP, 0, { __PTHREAD_SPINS } } }

# endif
#endif


/* Read-write lock types.  */
#if defined __USE_UNIX98 || defined __USE_XOPEN2K
enum
    {
        PTHREAD_RWLOCK_PREFER_READER_NP,
        PTHREAD_RWLOCK_PREFER_WRITER_NP,
        PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
        PTHREAD_RWLOCK_DEFAULT_NP = PTHREAD_RWLOCK_PREFER_READER_NP
    };

/* Define __PTHREAD_RWLOCK_INT_FLAGS_SHARED to 1 if pthread_rwlock_t
   has the shared field.  All 64-bit architectures have the shared field
   in pthread_rwlock_t.  */
#ifndef __PTHREAD_RWLOCK_INT_FLAGS_SHARED
# if __WORDSIZE == 64
#  define __PTHREAD_RWLOCK_INT_FLAGS_SHARED 1
# endif
#endif

/* Read-write lock initializers.  */
# define PTHREAD_RWLOCK_INITIALIZER                                     \
    { { 0, 0, 0, 0, 0, 0, 0, 0, __PTHREAD_RWLOCK_ELISION_EXTRA, 0, 0 } }
# ifdef __USE_GNU
#  ifdef __PTHREAD_RWLOCK_INT_FLAGS_SHARED
#   define PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP        \
    { { 0, 0, 0, 0, 0, 0, 0, 0, __PTHREAD_RWLOCK_ELISION_EXTRA, 0,  \
                PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP } }
#  else
#   if __BYTE_ORDER == __LITTLE_ENDIAN
#    define PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP           \
    { { 0, 0, 0, 0, 0, 0, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP, \
                0, __PTHREAD_RWLOCK_ELISION_EXTRA, 0, 0 } }
#   else
#    define PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP           \
    { { 0, 0, 0, 0, 0, 0, 0, 0, 0, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP, \
                0 } }
#   endif
#  endif
# endif
#endif  /* Unix98 or XOpen2K */


/* Scheduler inheritance.  */
enum {
    PTHREAD_INHERIT_SCHED,
#define PTHREAD_INHERIT_SCHED   PTHREAD_INHERIT_SCHED
    PTHREAD_EXPLICIT_SCHED
#define PTHREAD_EXPLICIT_SCHED  PTHREAD_EXPLICIT_SCHED
};


/* Scope handling.  */
enum{
    PTHREAD_SCOPE_SYSTEM,
#define PTHREAD_SCOPE_SYSTEM    PTHREAD_SCOPE_SYSTEM
    PTHREAD_SCOPE_PROCESS
#define PTHREAD_SCOPE_PROCESS   PTHREAD_SCOPE_PROCESS
};


/* Process shared or private flag.  */
enum{
    PTHREAD_PROCESS_PRIVATE,
#define PTHREAD_PROCESS_PRIVATE PTHREAD_PROCESS_PRIVATE
    PTHREAD_PROCESS_SHARED
#define PTHREAD_PROCESS_SHARED  PTHREAD_PROCESS_SHARED
};



/* Conditional variable handling.  */
#define PTHREAD_COND_INITIALIZER { { 0, 0, 0, 0, 0, (void *) 0, 0, 0 } }

/* Cancellation */
enum{
    PTHREAD_CANCEL_ENABLE,
#define PTHREAD_CANCEL_ENABLE   PTHREAD_CANCEL_ENABLE
    PTHREAD_CANCEL_DISABLE
#define PTHREAD_CANCEL_DISABLE  PTHREAD_CANCEL_DISABLE
};
enum{
    PTHREAD_CANCEL_DEFERRED,
#define PTHREAD_CANCEL_DEFERRED PTHREAD_CANCEL_DEFERRED
    PTHREAD_CANCEL_ASYNCHRONOUS
#define PTHREAD_CANCEL_ASYNCHRONOUS PTHREAD_CANCEL_ASYNCHRONOUS
};
#define PTHREAD_CANCELED ((void *) -1)


/* Single execution handling.  */
#define PTHREAD_ONCE_INIT 0



#ifdef __cplusplus
extern "C" {
#endif

    /**
     *  \brief pthread create
     *
     *  Create a new thread, starting with execution of START-ROUTINE
     *  getting passed ARG.  Creation attributed come from ATTR.  The new
     *  handle is stored in *NEWTHREAD.
     */
    extern int pthread_create (pthread_t *__restrict __newthread,
        const pthread_attr_t *__restrict __attr,
        void *(*__start_routine) (void *),
        void *__restrict __arg);

    /**
     * \brief Terminate calling thread.
     *
     * The registered cleanup handlers are called via exception handling
     * so we cannot mark this function with __THROW.
     */
    extern void pthread_exit (void *__retval);

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
    extern int pthread_join (pthread_t __th, void **__thread_return);

#ifdef __USE_GNU
    /**
     * \brief Check whether thread TH has terminated
     *
     * Check whether thread TH has terminated.  If yes return the status of
     * the thread in *THREAD_RETURN, if THREAD_RETURN is not NULL.
     */
    extern int pthread_tryjoin_np (pthread_t __th, void **__thread_return); 

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
    extern int pthread_timedjoin_np (pthread_t __th, void **__thread_return,
        const struct timespec *__abstime); /* apollo NOT supported now */
#endif

    /**
     * \brief pthread detach
     *
     *  Indicate that the thread TH is never to be joined with PTHREAD_JOIN.
     *  The resources of TH will therefore be freed immediately when it
     *  terminates, instead of waiting for another thread to perform PTHREAD_JOIN
     *  on it.
     */
    extern int pthread_detach (pthread_t __th);


    /**
     *  \brief  Obtain the identifier of the current thread.
     *
     *  Obtain the identifier of the current thread.
     */
    extern pthread_t pthread_self (void);

    /**
     *  \brief Compare two thread identifiers
     *   Compare two thread identifiers.
     */
    extern int pthread_equal (pthread_t __thread1, pthread_t __thread2);


    /* Thread attribute handling.  */

    /**
     * \brief  Initialize thread attribute *ATTR with default attributes
     *
     * Initialize thread attribute *ATTR with default attributes
     * (detachstate is PTHREAD_JOINABLE, scheduling policy is SCHED_OTHER,
     * no user-provided stack).
     */
    extern int pthread_attr_init (pthread_attr_t *__attr);

    /**
     *  \brief Destroy thread attribute *ATTR.
     *
     * Destroy thread attribute *ATTR.
     */
    extern int pthread_attr_destroy (pthread_attr_t *__attr);

    /**
     * \brief  Get detach state attribute.
     *
     * Get detach state attribute.
     */
    extern int pthread_attr_getdetachstate (const pthread_attr_t *__attr,
        int *__detachstate);

    /**
     * \brief  Set detach state attribute.
     *
     * Set detach state attribute.
     */
    extern int pthread_attr_setdetachstate (pthread_attr_t *__attr,
        int __detachstate);


    /**
     *   \brief Get the size of the guard area created for stack overflow protection.
     */
    /* apollo NOT supported now */
    extern int pthread_attr_getguardsize (const pthread_attr_t *__attr, size_t *__guardsize); 

    /**
     *   Set the size of the guard area created for stack overflow protection.
     */
    /* apollo NOT supported now */
    extern int pthread_attr_setguardsize (pthread_attr_t *__attr, size_t __guardsize); 


    /**
     *   Return in *PARAM the scheduling parameters of *ATTR.
     */
    extern int pthread_attr_getschedparam (const pthread_attr_t *__restrict __attr,
        struct sched_param *__restrict __param); /* apollo NOT supported now */

    /* Set scheduling parameters (priority, etc) in *ATTR according to PARAM.  */
    extern int pthread_attr_setschedparam (pthread_attr_t *__restrict __attr,
        const struct sched_param *__restrict __param); /* apollo NOT supported now */

    /* Return in *POLICY the scheduling policy of *ATTR.  */
    extern int pthread_attr_getschedpolicy (const pthread_attr_t *__restrict
        __attr, int *__restrict __policy); /* apollo NOT supported now */

    /* Set scheduling policy in *ATTR according to POLICY.  */
    /* apollo NOT supported now */
    extern int pthread_attr_setschedpolicy (pthread_attr_t *__attr, int __policy); 

    /* Return in *INHERIT the scheduling inheritance mode of *ATTR.  */
    extern int pthread_attr_getinheritsched (const pthread_attr_t *__restrict
        __attr, int *__restrict __inherit); /* apollo NOT supported now */

    /* Set scheduling inheritance mode in *ATTR according to INHERIT.  */
    extern int pthread_attr_setinheritsched (pthread_attr_t *__attr,
        int __inherit); /* apollo NOT supported now */


    /* Return in *SCOPE the scheduling contention scope of *ATTR.  */
    extern int pthread_attr_getscope (const pthread_attr_t *__restrict __attr,
        int *__restrict __scope); /* apollo NOT supported now */

    /* Set scheduling contention scope in *ATTR according to SCOPE.  */
    /* apollo NOT supported now */
    extern int pthread_attr_setscope (pthread_attr_t *__attr, int __scope); 

    /* Return the previously set address for the stack.  */
    extern int pthread_attr_getstackaddr (const pthread_attr_t *__restrict
        __attr, void **__restrict __stackaddr);

    /* Set the starting address of the stack of the thread to be created.
       Depending on whether the stack grows up or down the value must either
       be higher or lower than all the address in the memory block.  The
       minimal size of the block must be PTHREAD_STACK_MIN.  */
    extern int pthread_attr_setstackaddr (pthread_attr_t *__attr,
        void *__stackaddr); /* apollo NOT supported now */

    /* Return the currently used minimal stack size.  */
    extern int pthread_attr_getstacksize (const pthread_attr_t *__restrict
        __attr, size_t *__restrict __stacksize);

    /* Add information about the minimum stack size needed for the thread
       to be started.  This size must never be less than PTHREAD_STACK_MIN
       and must also not exceed the system limits.  */
    extern int pthread_attr_setstacksize (pthread_attr_t *__attr,
        size_t __stacksize);

#ifdef __USE_XOPEN2K
    /* Return the previously set address for the stack.  */
    extern int pthread_attr_getstack (const pthread_attr_t *__restrict __attr,
        void **__restrict __stackaddr,
        size_t *__restrict __stacksize);

    /* The following two interfaces are intended to replace the last two.  They
       require setting the address as well as the size since only setting the
       address will make the implementation on some architectures impossible.  */
    extern int pthread_attr_setstack (pthread_attr_t *__attr, void *__stackaddr,
        size_t __stacksize); /* apollo NOT supported now */
#endif

#ifdef __USE_GNU
    /* Thread created with attribute ATTR will be limited to run only on
       the processors represented in CPUSET.  */
    extern int pthread_attr_setaffinity_np (pthread_attr_t *__attr,
        size_t __cpusetsize,
        const cpu_set_t *__cpuset); /* apollo NOT supported now */

    /* Get bit set in CPUSET representing the processors threads created with
       ATTR can run on.  */
    extern int pthread_attr_getaffinity_np (const pthread_attr_t *__attr,
        size_t __cpusetsize,
        cpu_set_t *__cpuset); /* apollo NOT supported now */

    /* Get the default attributes used by pthread_create in this process.  */
    extern int pthread_getattr_default_np (pthread_attr_t *__attr);

    /* Set the default attributes to be used by pthread_create in this
       process.  */
    extern int pthread_setattr_default_np (const pthread_attr_t *__attr);

    /* Initialize thread attribute *ATTR with attributes corresponding to the
       already running thread TH.  It shall be called on uninitialized ATTR
       and destroyed with pthread_attr_destroy when no longer needed.  */
    extern int pthread_getattr_np (pthread_t __th, pthread_attr_t *__attr);
#endif


    /* Functions for scheduling control.  */

    /* Set the scheduling parameters for TARGET_THREAD according to POLICY
       and *PARAM.  */
    extern int pthread_setschedparam (pthread_t __target_thread, int __policy,
        const struct sched_param *__param); /* apollo NOT supported now */

    /* Return in *POLICY and *PARAM the scheduling parameters for TARGET_THREAD. */
    extern int pthread_getschedparam (pthread_t __target_thread,
        int *__restrict __policy,
        struct sched_param *__restrict __param); /* apollo NOT supported now */

    /* Set the scheduling priority for TARGET_THREAD.  */
    /* apollo NOT supported now */
    extern int pthread_setschedprio (pthread_t __target_thread, int __prio); 


#ifdef __USE_GNU
    /* Get thread name visible in the kernel and its interfaces.  */
    extern int pthread_getname_np (pthread_t __target_thread, char *__buf,
        size_t __buflen);

    /* Set thread name visible in the kernel and its interfaces.  */
    extern int pthread_setname_np (pthread_t __target_thread, const char *__name);
#endif


#ifdef __USE_UNIX98
    /* Determine level of concurrency.  */
    extern int pthread_getconcurrency (void); /* apollo NOT supported now */

    /* Set new concurrency level to LEVEL.  */
    extern int pthread_setconcurrency (int __level); /* apollo NOT supported now */
#endif

#ifdef __USE_GNU
    /* Yield the processor to another thread or process.
       This function is similar to the POSIX `sched_yield' function but
       might be differently implemented in the case of a m-on-n thread
       implementation.  */
    extern int pthread_yield (void);


    /* Limit specified thread TH to run only on the processors represented
       in CPUSET.  */
    extern int pthread_setaffinity_np (pthread_t __th, size_t __cpusetsize,
        const cpu_set_t *__cpuset); /* apollo NOT supported now */

    /* Get bit set in CPUSET representing the processors TH can run on.  */
    extern int pthread_getaffinity_np (pthread_t __th, size_t __cpusetsize,
        cpu_set_t *__cpuset); /* apollo NOT supported now */
#endif


    /* Functions for handling initialization.  */

    /* Guarantee that the initialization function INIT_ROUTINE will be called
       only once, even if pthread_once is executed several times with the
       same ONCE_CONTROL argument. ONCE_CONTROL must point to a static or
       extern variable initialized to PTHREAD_ONCE_INIT.

       The initialization functions might throw exception which is why
       this function is not marked with __THROW.  */
    extern int pthread_once (pthread_once_t *__once_control,
        void (*__init_routine) (void)); /* apollo NOT supported now */


    /* Functions for handling cancellation.

       Note that these functions are explicitly not marked to not throw an
       exception in C++ code.  If cancellation is implemented by unwinding
       this is necessary to have the compiler generate the unwind information.  */

    /* Set cancelability state of current thread to STATE, returning old
       state in *OLDSTATE if OLDSTATE is not NULL.  */
    /* apollo NOT supported now */
    extern int pthread_setcancelstate (int __state, int *__oldstate); 

    /* Set cancellation state of current thread to TYPE, returning the old
       type in *OLDTYPE if OLDTYPE is not NULL.  */
    /* apollo NOT supported now */
    extern int pthread_setcanceltype (int __type, int *__oldtype); 

    /* Cancel THREAD immediately or at the next possibility.  */
    extern int pthread_cancel (pthread_t __th); /* apollo NOT supported now */

    /* Test for pending cancellation for the current thread and terminate
       the thread as per pthread_exit(PTHREAD_CANCELED) if it has been
       cancelled.  */
    extern void pthread_testcancel (void); /* apollo NOT supported now */

    /* Mutex handling.  */

    /* Initialize a mutex.  */
    extern int pthread_mutex_init (pthread_mutex_t *__mutex,
        const pthread_mutexattr_t *__mutexattr);

    /* Destroy a mutex.  */
    extern int pthread_mutex_destroy (pthread_mutex_t *__mutex);


    /* Try locking a mutex.  */
    extern int pthread_mutex_trylock (pthread_mutex_t *__mutex);

    /* Lock a mutex.  */
    extern int pthread_mutex_lock (pthread_mutex_t *__mutex);

/* #ifdef __USE_XOPEN2K */
    /* Wait until lock becomes available, or specified time passes. */
    extern int pthread_mutex_timedlock (pthread_mutex_t *__restrict __mutex,
        const struct timespec *__restrict
        __abstime);
/*#endif*/

    /* Unlock a mutex.  */
    extern int pthread_mutex_unlock (pthread_mutex_t *__mutex);


    /* Get the priority ceiling of MUTEX.  */
    extern int pthread_mutex_getprioceiling (const pthread_mutex_t *
        __restrict __mutex,
        int *__restrict __prioceiling); /* apollo NOT supported now */

    /* Set the priority ceiling of MUTEX to PRIOCEILING, return old
       priority ceiling value in *OLD_CEILING.  */
    extern int pthread_mutex_setprioceiling (pthread_mutex_t *__restrict __mutex,
        int __prioceiling,
        int *__restrict __old_ceiling); /* apollo NOT supported now */


#ifdef __USE_XOPEN2K8
    /* Declare the state protected by MUTEX as consistent.  */
    /* apollo NOT supported now */
    extern int pthread_mutex_consistent (pthread_mutex_t *__mutex); 
# ifdef __USE_GNU
    /* apollo NOT supported now */
    extern int pthread_mutex_consistent_np (pthread_mutex_t *__mutex); 
# endif
#endif

    /* Functions for handling mutex attributes.  */

    /* Initialize mutex attribute object ATTR with default attributes
       (kind is PTHREAD_MUTEX_TIMED_NP).  */
    extern int pthread_mutexattr_init (pthread_mutexattr_t *__attr);

    /* Destroy mutex attribute object ATTR.  */
    extern int pthread_mutexattr_destroy (pthread_mutexattr_t *__attr);

    /* Get the process-shared flag of the mutex attribute ATTR.  */
    extern int pthread_mutexattr_getpshared (const pthread_mutexattr_t *
        __restrict __attr,
        int *__restrict __pshared); /* apollo NOT supported now */

    /* Set the process-shared flag of the mutex attribute ATTR.  */
    extern int pthread_mutexattr_setpshared (pthread_mutexattr_t *__attr,
        int __pshared); /* apollo NOT supported now */

#if defined __USE_UNIX98 || defined __USE_XOPEN2K8
    /* Return in *KIND the mutex kind attribute in *ATTR.  */
    extern int pthread_mutexattr_gettype (const pthread_mutexattr_t *__restrict
        __attr, int *__restrict __kind);

    /* Set the mutex kind attribute in *ATTR to KIND (either PTHREAD_MUTEX_NORMAL,
       PTHREAD_MUTEX_RECURSIVE, PTHREAD_MUTEX_ERRORCHECK, or
       PTHREAD_MUTEX_DEFAULT).  */
    extern int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind);
#endif

    /* Return in *PROTOCOL the mutex protocol attribute in *ATTR.  */
    extern int pthread_mutexattr_getprotocol (const pthread_mutexattr_t *
        __restrict __attr,
        int *__restrict __protocol);

    /* Set the mutex protocol attribute in *ATTR to PROTOCOL (either
       PTHREAD_PRIO_NONE, PTHREAD_PRIO_INHERIT, or PTHREAD_PRIO_PROTECT).  */
    extern int pthread_mutexattr_setprotocol (pthread_mutexattr_t *__attr,
        int __protocol);

    /* Return in *PRIOCEILING the mutex prioceiling attribute in *ATTR.  */
    extern int pthread_mutexattr_getprioceiling (const pthread_mutexattr_t *
        __restrict __attr,
        int *__restrict __prioceiling); /* apollo NOT supported now */

    /* Set the mutex prioceiling attribute in *ATTR to PRIOCEILING.  */
    extern int pthread_mutexattr_setprioceiling (pthread_mutexattr_t *__attr,
        int __prioceiling); /* apollo NOT supported now */

#ifdef __USE_XOPEN2K
    /* Get the robustness flag of the mutex attribute ATTR.  */
    extern int pthread_mutexattr_getrobust (const pthread_mutexattr_t *__attr,
        int *__robustness); /* apollo NOT supported now */

# ifdef __USE_GNU
    extern int pthread_mutexattr_getrobust_np (const pthread_mutexattr_t *__attr,
        int *__robustness); /* apollo NOT supported now */
# endif

    /* Set the robustness flag of the mutex attribute ATTR.  */
    extern int pthread_mutexattr_setrobust (pthread_mutexattr_t *__attr,
        int __robustness); /* apollo NOT supported now */
# ifdef __USE_GNU
    extern int pthread_mutexattr_setrobust_np (pthread_mutexattr_t *__attr,
        int __robustness); /* apollo NOT supported now */
# endif
#endif


#if defined __USE_UNIX98 || defined __USE_XOPEN2K
    /* Functions for handling read-write locks.  */

    /* Initialize read-write lock RWLOCK using attributes ATTR, or use
       the default values if later is NULL.  */
    extern int pthread_rwlock_init (pthread_rwlock_t *__restrict __rwlock,
        const pthread_rwlockattr_t *__restrict
        __attr);

    /* Destroy read-write lock RWLOCK.  */
    extern int pthread_rwlock_destroy (pthread_rwlock_t *__rwlock);

    /* Acquire read lock for RWLOCK.  */
    extern int pthread_rwlock_rdlock (pthread_rwlock_t *__rwlock);

    /* Try to acquire read lock for RWLOCK.  */
    extern int pthread_rwlock_tryrdlock (pthread_rwlock_t *__rwlock);

# ifdef __USE_XOPEN2K
    /* Try to acquire read lock for RWLOCK or return after specfied time.  */
    extern int pthread_rwlock_timedrdlock (pthread_rwlock_t *__restrict __rwlock,
        const struct timespec *__restrict
        __abstime);
# endif

    /* Acquire write lock for RWLOCK.  */
    extern int pthread_rwlock_wrlock (pthread_rwlock_t *__rwlock);

    /* Try to acquire write lock for RWLOCK.  */
    extern int pthread_rwlock_trywrlock (pthread_rwlock_t *__rwlock);

# ifdef __USE_XOPEN2K
    /* Try to acquire write lock for RWLOCK or return after specfied time.  */
    extern int pthread_rwlock_timedwrlock (pthread_rwlock_t *__restrict __rwlock,
        const struct timespec *__restrict
        __abstime);
# endif

    /* Unlock RWLOCK.  */
    extern int pthread_rwlock_unlock (pthread_rwlock_t *__rwlock);


    /* Functions for handling read-write lock attributes.  */

    /* Initialize attribute object ATTR with default values.  */
    extern int pthread_rwlockattr_init (pthread_rwlockattr_t *__attr);

    /* Destroy attribute object ATTR.  */
    extern int pthread_rwlockattr_destroy (pthread_rwlockattr_t *__attr);

    /* Return current setting of process-shared attribute of ATTR in PSHARED.  */
    extern int pthread_rwlockattr_getpshared (const pthread_rwlockattr_t *
        __restrict __attr,
        int *__restrict __pshared); /* apollo NOT supported now */

    /* Set process-shared attribute of ATTR to PSHARED.  */
    extern int pthread_rwlockattr_setpshared (pthread_rwlockattr_t *__attr,
        int __pshared); /* apollo NOT supported now */

    /* Return current setting of reader/writer preference.  */
    extern int pthread_rwlockattr_getkind_np (const pthread_rwlockattr_t *
        __restrict __attr,
        int *__restrict __pref); /* apollo NOT supported now */

    /* Set reader/write preference.  */
    extern int pthread_rwlockattr_setkind_np (pthread_rwlockattr_t *__attr,
        int __pref); /* apollo NOT supported now */
#endif


    /* Functions for handling conditional variables.  */

    /* Initialize condition variable COND using attributes ATTR, or use
       the default values if later is NULL.  */
    extern int pthread_cond_init (pthread_cond_t *__restrict __cond,
        const pthread_condattr_t *__restrict __cond_attr);

    /* Destroy condition variable COND.  */
    extern int pthread_cond_destroy (pthread_cond_t *__cond);

    /* Wake up one thread waiting for condition variable COND.  */
    extern int pthread_cond_signal (pthread_cond_t *__cond);

    /* Wake up all threads waiting for condition variables COND.  */
    extern int pthread_cond_broadcast (pthread_cond_t *__cond);

    /* Wait for condition variable COND to be signaled or broadcast.
       MUTEX is assumed to be locked before.

       This function is a cancellation point and therefore not marked with
       __THROW.  */
    extern int pthread_cond_wait (pthread_cond_t *__restrict __cond,
        pthread_mutex_t *__restrict __mutex);

    /* Wait for condition variable COND to be signaled or broadcast until
       ABSTIME.  MUTEX is assumed to be locked before.  ABSTIME is an
       absolute time specification; zero is the beginning of the epoch
       (00:00:00 GMT, January 1, 1970).

       This function is a cancellation point and therefore not marked with
       __THROW.  */
    extern int pthread_cond_timedwait (pthread_cond_t *__restrict __cond,
        pthread_mutex_t *__restrict __mutex,
        const struct timespec *__restrict __abstime);

    /* Functions for handling condition variable attributes.  */

    /* Initialize condition variable attribute ATTR.  */
    extern int pthread_condattr_init (pthread_condattr_t *__attr);

    /* Destroy condition variable attribute ATTR.  */
    extern int pthread_condattr_destroy (pthread_condattr_t *__attr);

    /* Get the process-shared flag of the condition variable attribute ATTR.  */
    extern int pthread_condattr_getpshared (const pthread_condattr_t *
        __restrict __attr,
        int *__restrict __pshared); /* apollo NOT supported now */

    /* Set the process-shared flag of the condition variable attribute ATTR.  */
    extern int pthread_condattr_setpshared (pthread_condattr_t *__attr,
        int __pshared); /* apollo NOT supported now */

#ifdef __USE_XOPEN2K
    /* Get the clock selected for the condition variable attribute ATTR.  */
    extern int pthread_condattr_getclock (const pthread_condattr_t *
        __restrict __attr,
        __clockid_t *__restrict __clock_id); /* apollo NOT supported now */

    /* Set the clock selected for the condition variable attribute ATTR.  */
    extern int pthread_condattr_setclock (pthread_condattr_t *__attr,
        __clockid_t __clock_id); /* apollo NOT supported now */
#endif


#ifdef __USE_XOPEN2K
    /* Functions to handle spinlocks.  */

    /* Initialize the spinlock LOCK.  If PSHARED is nonzero the spinlock can
       be shared between different processes.  */
    extern int pthread_spin_init (pthread_spinlock_t *__lock, int __pshared);

    /* Destroy the spinlock LOCK.  */
    extern int pthread_spin_destroy (pthread_spinlock_t *__lock);

    /* Wait until spinlock LOCK is retrieved.  */
    extern int pthread_spin_lock (pthread_spinlock_t *__lock);

    /* Try to lock spinlock LOCK.  */
    extern int pthread_spin_trylock (pthread_spinlock_t *__lock);

    /* Release spinlock LOCK.  */
    extern int pthread_spin_unlock (pthread_spinlock_t *__lock);


    /* Functions to handle barriers.  */

    /* Initialize BARRIER with the attributes in ATTR.  The barrier is
       opened when COUNT waiters arrived.  */
    extern int pthread_barrier_init (pthread_barrier_t *__restrict __barrier,
        const pthread_barrierattr_t *__restrict
        __attr, unsigned int __count); /* apollo NOT supported now */

    /* Destroy a previously dynamically initialized barrier BARRIER.  */
    /* apollo NOT supported now */
    extern int pthread_barrier_destroy (pthread_barrier_t *__barrier); 

    /* Wait on barrier BARRIER.  */
    /* apollo NOT supported now */
    extern int pthread_barrier_wait (pthread_barrier_t *__barrier); 


    /* Initialize barrier attribute ATTR.  */
    /* apollo NOT supported now */
    extern int pthread_barrierattr_init (pthread_barrierattr_t *__attr);

    /* Destroy previously dynamically initialized barrier attribute ATTR.  */
    /* apollo NOT supported now */
    extern int pthread_barrierattr_destroy (pthread_barrierattr_t *__attr); 

    /* Get the process-shared flag of the barrier attribute ATTR.  */
    extern int pthread_barrierattr_getpshared (const pthread_barrierattr_t *
        __restrict __attr,
        int *__restrict __pshared); /* apollo NOT supported now */

    /* Set the process-shared flag of the barrier attribute ATTR.  */
    extern int pthread_barrierattr_setpshared (pthread_barrierattr_t *__attr,
        int __pshared); /* apollo NOT supported now */
#endif


    /* Functions for handling thread-specific data.  */

    /* Create a key value identifying a location in the thread-specific
       data area.  Each thread maintains a distinct thread-specific data
       area.  DESTR_FUNCTION, if non-NULL, is called with the value
       associated to that key when the key is destroyed.
       DESTR_FUNCTION is not called if the value associated is NULL when
       the key is destroyed.  */
    extern int pthread_key_create (pthread_key_t *__key,
        void (*__destr_function) (void *)); /* apollo NOT supported now */

    /* Destroy KEY.  */
    /* apollo NOT supported now */
    extern int pthread_key_delete (pthread_key_t __key); 

    /* Return current value of the thread-specific data slot identified by KEY.  */
    /* apollo NOT supported now */
    extern void *pthread_getspecific (pthread_key_t __key); 
    /* Store POINTER in the thread-specific data slot identified by KEY. */
    extern int pthread_setspecific (pthread_key_t __key,
        const void *__pointer); /* apollo NOT supported now */


#ifdef __USE_XOPEN2K
    /* Get ID of CPU-time clock for thread THREAD_ID.  */
    extern int pthread_getcpuclockid (pthread_t __thread_id,
        __clockid_t *__clock_id); /* apollo NOT supported now */
#endif


    /* Install handlers to be called when a new process is created with FORK.
       The PREPARE handler is called in the parent process just before performing
       FORK. The PARENT handler is called in the parent process just after FORK.
       The CHILD handler is called in the child process.  Each of the three
       handlers can be NULL, meaning that no handler needs to be called at that
       point.
       PTHREAD_ATFORK can be called several times, in which case the PREPARE
       handlers are called in LIFO order (last added with PTHREAD_ATFORK,
       first called before FORK), and the PARENT and CHILD handlers are called
       in FIFO (first added, first called).  */

    extern int pthread_atfork (void (*__prepare) (void),
        void (*__parent) (void),
        void (*__child) (void)); /* apollo NOT supported now */


#ifdef __cplusplus
}
#endif


#endif /* __PTHREAD_H */
