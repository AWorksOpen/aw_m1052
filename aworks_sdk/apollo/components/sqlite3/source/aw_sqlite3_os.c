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
 * \brief Sqlite3's memory and mutex subsystem implementation.(AWorks platform)
 *
 * \internal
 * \par modification history:
 * - 1.00 16-01-04  cyl, first implementation.
 * \endinternal
 */


#include "apollo.h"
#include "aw_sqlite3_config.h"
#include "sqlite3.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "string.h"

/******************************************************************************/
#ifdef SQLITE_AWORKS_MALLOC

#include "aw_mem.h"

/**
 * \brief Memory allocation function.
 */
static void *__sqlite3_mem_malloc (int nbytes)
{
    int *actual_ptr = (int *)aw_mem_alloc(nbytes+4);
    if (actual_ptr == NULL) return NULL;

    *actual_ptr = nbytes;
    return (void *)(actual_ptr+1);
}

/**
 * \brief Free memory.
 */
static void __sqlite3_mem_free (void *ptr)
{
	aw_mem_free((void *)((int)ptr-4));
    return;
}

/**
 * \brief Change the size of an existing memory allocation.
 */
static void *__sqlite3_mem_realloc (void *ptr, int nbytes)
{
    int *actual_ptr = aw_mem_realloc((void *)((int)ptr-4), nbytes+4);
    if (actual_ptr == NULL) return NULL;

    *actual_ptr = nbytes;
    return (void *)(actual_ptr+1);
}

/**
 * \brief Return the size of an outstanding allocation, in bytes.
 */
static int __sqlite3_mem_size_get (void *ptr)
{
    int size = *(int *)((int)ptr-4);
    return size;
}

/**
 * \brief Round up a request size to the next valid allocation size.
 */
static int __sqlite3_mem_roundup (int n)
{
//    return AW_ROUND_UP(n, sizeof(void *)) +
//                       AW_ROUND_UP(4, sizeof(void *)) + sizeof(void *);
    return n;
}

/**
 * \brief Memory module init.
 */
static int __sqlite3_mem_init (void *p_data)
{
    (void)p_data;
    return SQLITE_OK;
}

/**
 * \brief Memory module deinit.
 */
static void __sqlite3_mem_shutdown (void *p_data)
{
    (void)p_data;
    return;
}

/**
 * \brief Populate the low-level memory allocation function pointers in
 *        sqlite3GlobalConfig.m with pointers to the routines in this file. The
 *        arguments specify the block of memory to manage.
 */
void sqlite3MemSetDefault (void)
{
    static const sqlite3_mem_methods __mem_methods = {
        __sqlite3_mem_malloc,
        __sqlite3_mem_free,
        __sqlite3_mem_realloc,
        __sqlite3_mem_size_get,
        __sqlite3_mem_roundup,
        __sqlite3_mem_init,
        __sqlite3_mem_shutdown,
        NULL
    };

   sqlite3_config(SQLITE_CONFIG_MALLOC, &__mem_methods);
}

#endif /* SQLITE_AWORKS_MALLOC */



/******************************************************************************/
#ifdef SQLITE_MUTEX_AWORKS

#include "aw_sem.h"
#include "aw_task.h"

/**
 * \brief Each recursive mutex is an instance of the following structure.
 */
struct sqlite3_mutex {
    AW_MUTEX_DECL(        mutex); /**< \brief mutex controlling the lock */

#ifdef SQLITE_DEBUG
    int                   id;     /**< \brief mutex type */
    volatile int          nref;   /**< \brief number of enterances */
    volatile aw_task_id_t owner;  /**< \brief thread holding this mutex */
    int                   trace;  /**< \brief true to trace change */
#endif
};


/** \brief mutex initializer */
#define SQLITE_AWORKS_MUTEX_INITIALIZER {0}


#ifdef SQLITE_DEBUG
#define SQLITE3_MUTEX_INITIALIZER \
    { SQLITE_AWORKS_MUTEX_INITIALIZER, 0, 0, (aw_task_id_t)0, 0 }
#else
#define SQLITE3_MUTEX_INITIALIZER { SQLITE_AWORKS_MUTEX_INITIALIZER }
#endif


/**
 * \brief The sqlite3_mutex_held() and sqlite3_mutex_notheld() routine are
 *        intended for use only inside aw_assert() statements.
 */
#ifdef SQLITE_DEBUG
static int __sqlite3_mutex_held (sqlite3_mutex *p_mutex)
{
    return p_mutex->nref != 0 && p_mutex->owner == aw_task_id_self();
}

static int __sqlite3_mutex_notheld2 (sqlite3_mutex *p_mutex, aw_task_id_t id)
{
    return p_mutex->nref == 0 || p_mutex->owner != id;
}

static int __sqlite3_mutex_notheld (sqlite3_mutex *p_mutex)
{
    aw_task_id_t id = aw_task_id_self();
    return __sqlite3_mutex_notheld2(p_mutex, id);
}
#endif


/**
 * \brief initialize and deinitialize the mutex subsystem.
 */
static sqlite3_mutex __sqlite3_mutexs[] = {
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER,
    SQLITE3_MUTEX_INITIALIZER
};

static int aw_mutex_isinit = 0;

/** \brief mutex init */
static int __sqlite3_mutex_init (void)
{
    uint8_t i;

    for (i = 0; i < AW_NELEMENTS(__sqlite3_mutexs); i++) {
        AW_MUTEX_INIT(__sqlite3_mutexs[i].mutex, AW_SEM_Q_FIFO);
    }

    aw_mutex_isinit = 1;

    return SQLITE_OK;
}

/** \brief mutex deinit */
static int __sqlite3_mutex_end (void)
{
    uint8_t i;

    for (i = 0; i < AW_NELEMENTS(__sqlite3_mutexs); i++) {
        AW_MUTEX_TERMINATE(__sqlite3_mutexs[i].mutex);
    }

    aw_mutex_isinit = 0;

    return SQLITE_OK;
}


/**
 * \brief The sqlite3_mutex_alloc() routine allocates a new
 *        mutex and returns a pointer to it.  If it returns NULL
 *        that means that a mutex could not be allocated.  SQLite
 *        will unwind its stack and return an error.  The argument
 *        to sqlite3_mutex_alloc() is one of these integer constants:
 *
 *        <ul>
 *        <li>  SQLITE_MUTEX_FAST
 *        <li>  SQLITE_MUTEX_RECURSIVE
 *        <li>  SQLITE_MUTEX_STATIC_MASTER
 *        <li>  SQLITE_MUTEX_STATIC_MEM
 *        <li>  SQLITE_MUTEX_STATIC_MEM2
 *        <li>  SQLITE_MUTEX_STATIC_PRNG
 *        <li>  SQLITE_MUTEX_STATIC_LRU
 *        <li>  SQLITE_MUTEX_STATIC_PMEM
 *        </ul>
 *
 *        The first two constants cause sqlite3_mutex_alloc() to create
 *        a new mutex.  The new mutex is recursive when SQLITE_MUTEX_RECURSIVE
 *        is used but not necessarily so when SQLITE_MUTEX_FAST is used.
 *        The mutex implementation does not need to make a distinction
 *        between SQLITE_MUTEX_RECURSIVE and SQLITE_MUTEX_FAST if it does
 *        not want to.  But SQLite will only request a recursive mutex in
 *        cases where it really needs one.  If a faster non-recursive mutex
 *        implementation is available on the host platform, the mutex subsystem
 *        might return such a mutex in response to SQLITE_MUTEX_FAST.
 *
 *        The other allowed parameters to sqlite3_mutex_alloc() each return
 *        a pointer to a static preexisting mutex.  Six static mutexes are
 *        used by the current version of SQLite.  Future versions of SQLite
 *        may add additional static mutexes.  Static mutexes are for internal
 *        use by SQLite only.  Applications that use SQLite mutexes should
 *        use only the dynamic mutexes returned by SQLITE_MUTEX_FAST or
 *        SQLITE_MUTEX_RECURSIVE.
 *
 *        Note that if one of the dynamic mutex parameters (SQLITE_MUTEX_FAST
 *        or SQLITE_MUTEX_RECURSIVE) is used then sqlite3_mutex_alloc()
 *        returns a different mutex on every call.  But for the static
 *        mutex types, the same mutex is returned on every call that has
 *        the same type number.
 */
SQLITE_API void *sqlite3Malloc(sqlite_uint64);
static sqlite3_mutex *__sqlite3_mutex_alloc (int type)
{
    sqlite3_mutex *p_mutex = 0;

    switch (type) {

    case SQLITE_MUTEX_FAST:
    case SQLITE_MUTEX_RECURSIVE:
        //todo: maybe this will cause an error!(sqlite3_malloc)
//        p_mutex = sqlite3_malloc(sizeof(*p_mutex));
        p_mutex = sqlite3Malloc(sizeof(*p_mutex));
        memset(p_mutex, 0, sizeof(*p_mutex));
        
        if (p_mutex) {
#ifdef SQLITE_DEBUG
            p_mutex->id    = type;
            p_mutex->nref  = 0;
            p_mutex->owner = 0;
            p_mutex->trace = 1;
#endif
            AW_MUTEX_INIT(p_mutex->mutex, AW_SEM_Q_FIFO);
        }
        break;

    default:
#ifdef SQLITE_ENABLE_API_ARMOR
        if (type - 2 < 0 || type - 2 >= AW_NELEMENTS(__sqlite3_mutexs)) {
            (void)SQLITE_MISUSE_BKPT;
            return 0;
        }
#endif
        p_mutex = &__sqlite3_mutexs[type - 2];

        break;
    }

    return p_mutex;
}

/**
 * \brief This routine deallocates a previously
 *        allocated mutex.  SQLite is careful to deallocate every
 *        mutex that it allocates.
 */
static void __sqlite3_mutex_free (sqlite3_mutex *p_mutex)
{
    aw_assert(p_mutex);
#ifdef SQLITE_DEBUG
    aw_assert(p_mutex->nref  == 0 &&
              p_mutex->owner == 0);

    aw_assert(p_mutex->id == SQLITE_MUTEX_FAST ||
              p_mutex->id == SQLITE_MUTEX_RECURSIVE);
#endif
    AW_MUTEX_TERMINATE(p_mutex->mutex);
    sqlite3_free(p_mutex);
}

/**
 * \brief The sqlite3_mutex_enter() and sqlite3_mutex_try() routines attempt
 *        to enter a mutex.  If another thread is already within the mutex,
 *        sqlite3_mutex_enter() will block and sqlite3_mutex_try() will return
 *        SQLITE_BUSY.  The sqlite3_mutex_try() interface returns SQLITE_OK
 *        upon successful entry.  Mutexes created using SQLITE_MUTEX_RECURSIVE can
 *        be entered multiple times by the same thread.  In such cases the,
 *        mutex must be exited an equal number of times before another thread
 *        can enter.  If the same thread tries to enter any other kind of mutex
 *        more than once, the behavior is undefined.
 */
static void __sqlite3_mutex_enter (sqlite3_mutex *p_mutex)
{
#ifdef SQLITE_DEBUG
    aw_task_id_t id = aw_task_id_self();
    aw_assert(p_mutex->id == SQLITE_MUTEX_RECURSIVE ||
           __sqlite3_mutex_notheld2(p_mutex, id));
#endif
    aw_assert(p_mutex != 0);
    AW_MUTEX_LOCK(p_mutex->mutex, AW_SEM_WAIT_FOREVER);

#ifdef SQLITE_DEBUG
    aw_assert(p_mutex->nref   > 0 ||
              p_mutex->owner == 0);
    p_mutex->owner = id;
    p_mutex->nref++;
    if (p_mutex->trace) {
        AW_INFOF(("enter mutex %p (%d) with nRef=%d\n",
                  p_mutex,
                  p_mutex->trace,
                  p_mutex->nref));
    }
#endif
}

static int __sqlite3_mutex_try (sqlite3_mutex *p_mutex)
{
#ifdef SQLITE_DEBUG
    aw_task_id_t id = aw_task_id_self();
    int rc = SQLITE_BUSY;

    aw_assert(p_mutex->id == SQLITE_MUTEX_RECURSIVE ||
              __sqlite3_mutex_notheld2(p_mutex, id));

#endif

  /*
   * The sqlite3_mutex_try() routine is very rarely used, and when it
   * is used it is merely an optimization.  So it is OK for it to always
   * fail.
   */
#if 1
    if (AW_MUTEX_LOCK(p_mutex->mutex, AW_SEM_NO_WAIT)) {
#ifdef SQLITE_DEBUG
        p_mutex->owner = id;
        p_mutex->nref++;
        rc = SQLITE_OK;
#endif
    }
#else
    (void)p_mutex;
#endif

#ifdef SQLITE_DEBUG
    if (rc == SQLITE_OK && p_mutex->trace) {
        AW_INFOF(("try mutex %p (%d) with nRef=%d\n",
                  p_mutex,
                  p_mutex->trace,
                  p_mutex->nref));
    }
#endif

#ifdef SQLITE_DEBUG
    return rc;
#else
    return SQLITE_OK;
#endif
}

/**
 * \brief The sqlite3_mutex_leave() routine exits a mutex that was
 *        previously entered by the same thread.  The behavior
 *        is undefined if the mutex is not currently entered or
 *        is not currently allocated.  SQLite will never do either.
 */
static void __sqlite3_mutex_leave (sqlite3_mutex *p_mutex)
{
#ifdef SQLITE_DEBUG
    aw_task_id_t id = aw_task_id_self();
    aw_assert(p_mutex->nref > 0);
    aw_assert(p_mutex->owner == id);
    p_mutex->nref--;
    if (p_mutex->nref == 0) {
        p_mutex->owner = 0;
    }
    aw_assert(p_mutex->nref == 0 ||
              p_mutex->id   == SQLITE_MUTEX_RECURSIVE);
#endif

    aw_assert(p_mutex != 0);
    AW_MUTEX_UNLOCK(p_mutex->mutex);


#ifdef SQLITE_DEBUG
    if (p_mutex->trace) {
        AW_INFOF(("leave mutex %p (%d) with nRef=%d\n",
                  p_mutex,
                  p_mutex->trace,
                  p_mutex->nref));
    }
#endif
}

/**
 * \brief sqlite3 default mutex method
 */
sqlite3_mutex_methods const *sqlite3DefaultMutex (void)
{
    static const sqlite3_mutex_methods __mutexs_methods = {
        __sqlite3_mutex_init,
        __sqlite3_mutex_end,
        __sqlite3_mutex_alloc,
        __sqlite3_mutex_free,
        __sqlite3_mutex_enter,
        __sqlite3_mutex_try,
        __sqlite3_mutex_leave,
#ifdef SQLITE_DEBUG
        __sqlite3_mutex_held,
        __sqlite3_mutex_notheld
#else
        0,
        0
#endif
    };

    return &__mutexs_methods;
}

#endif /* SQLITE_MUTEX_AWORKS */

/* end of file */
