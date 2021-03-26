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
 * \brief YAFFS OS glue
 *
 * \internal
 * \par modification history:
 * - 1.00 2016-04-26  cyl, first implementation.
 * \endinternal
 */

#include "yaffs_osglue.h"
#include "yaffsfs.h"
#include "yaffs_guts.h"

#include "aw_errno.h"
#include "aw_mem.h"
#include "aw_sem.h"
#include "aw_task.h"
#include "aw_time.h"
#include "aw_delay.h"
#include "aw_vdebug.h"


/** \brief yaffs errno */
static int __yaffs_last_errno = YAFFS_OK;

/** \brief mutex */
AW_MUTEX_DECL_STATIC(__yaffs_mutex);

/** \brief the stack size of background garbage collection task */
#define __YAFFS_BG_GC_TASK_STACKSIZE 2048

/** \brief instance of background garbage collection task */
AW_TASK_DECL_STATIC(__yaffs_bg_gc_task, __YAFFS_BG_GC_TASK_STACKSIZE);


/**
 * \brief lock
 * \param[in] none
 * \return none
 */
void yaffsfs_Lock(void)
{
    AW_MUTEX_LOCK(__yaffs_mutex, AW_SEM_WAIT_FOREVER);
}

/**
 * \brief unlock
 * \param[in] none
 * \return none
 */
void yaffsfs_Unlock(void)
{
    AW_MUTEX_UNLOCK(__yaffs_mutex);
}

/**
 * \brief get current system time
 * \param[in] none
 * \return the seconds from 1990-1-1 00:00:00
 */
u32 yaffsfs_CurrentTime(void)
{
    return aw_time(NULL);
}

/**
 * \brief errno set
 * param[in] err : errno
 * \return   none
 */
void yaffsfs_SetError(int err)
{
    __yaffs_last_errno = err;
    AW_ERRNO_SET(err);
}

/**
 * \brief errno get
 * \param[in] none
 * \return errno
 */
int yaffsfs_GetLastError(void)
{
    return __yaffs_last_errno;
}

/**
 * \brief memory alloc
 * \param[in] size : the size of memory you wanna
 * \return the pointer of memory
 */
void *yaffsfs_malloc(size_t size)
{
    return aw_mem_alloc(size);
}

/**
 * \brief memory free
 * \param[in] the pointer of memory
 * \return none
 */
void yaffsfs_free(void *ptr)
{
    aw_mem_free(ptr);
}

/**
 * \brief memory region check
 *
 * \param[in] addr : the addr of memory
 * \param[in] size : the size of memory
 * \param[in] write_request : is write requeset?
 *
 * \retval -1 : the addr of memory is NULL
 * \reurn  0  : everything gonna right
 */
int yaffsfs_CheckMemRegion(const void *addr, size_t size, int write_request)
{
    return ((NULL == addr) ? -1 : 0);
}


/**
 * \brief the entry of background garbage collection task
 * \param[in] dummy : this entry doesn't need this argument, give me a NULL 
 *                    it's ok
 *
 */
static void __yaffs_bg_gc_entry (void *dummy)
{
    struct yaffs_dev *dev;
    int urgent = 0;
    int result;
    int next_urgent;

    /* Sleep for a bit to allow start up */
    aw_mdelay(2000);


    while (1) {
        yaffs_dev_rewind();
        next_urgent = 0;

        while ((dev = yaffs_next_dev()) != NULL) {
            result = yaffs_do_background_gc_reldev(dev, urgent);
            if (result > 0) {
                next_urgent = 1;
            }
        }

        urgent = next_urgent;

        if (next_urgent) {
            aw_mdelay(1000);
        } else {
            aw_mdelay(5000);
        }
    }
}


/**
 * \brief bug printer function
 *
 * \param[in] file_name : the name of file where the bug occur
 * \param[in] line_no   : the line where the bug occur
 *
 * \return none
 */
void yaffs_bug_fn (const char *file_name, int line_no)
{
    AW_INFOF(("yaffs bug occur at file %s line %d\r\n", file_name, line_no));
}


/**
 * \brief OS initialisation
 * \param[in] none
 * \return none
 */
void yaffsfs_OSInitialisation(void)
{
    static int __yaffs_bggc_task_prio;

    __yaffs_bggc_task_prio = aw_task_lowest_priority() - 2;

    AW_MUTEX_INIT(__yaffs_mutex, AW_SEM_Q_FIFO);

    AW_TASK_INIT(__yaffs_bg_gc_task,
                 "yaffs_backgroud_garbage_collection",
                 __yaffs_bggc_task_prio,
                 __YAFFS_BG_GC_TASK_STACKSIZE,
                 __yaffs_bg_gc_entry,
                 NULL);

    AW_TASK_STARTUP(__yaffs_bg_gc_task);
}

/* end of file */
