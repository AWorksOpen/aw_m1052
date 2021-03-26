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
 * \brief I/O entry manage
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-08  deo, first implementation
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "apollo.h"
#include "io/aw_io_device.h"
#include "io/aw_io_entry.h"
#include "aw_sem.h"
#include "aw_errno.h"



/*******************************************************************************
    define
*******************************************************************************/
#define __IO_ENTRY_RESERVED  (void *)(-1)



/*******************************************************************************
    local variables
*******************************************************************************/
AW_MUTEX_DECL_STATIC(            __g_entry_mutex);
static struct aw_io_entry       *__gp_entry_tab = NULL;
static int                       __g_entry_cnt = 0;
static struct aw_iodev           __g_def_stdio;




/******************************************************************************/
static void __entry_tab_lock (void)
{
    AW_MUTEX_LOCK(__g_entry_mutex, AW_WAIT_FOREVER);
}




/******************************************************************************/
static void __entry_tab_unlock (void)
{
    AW_MUTEX_UNLOCK(__g_entry_mutex);
}




/******************************************************************************/
int aw_io_entry_init (struct aw_io_entry *p_ents, int count)
{
    int  ret;

    if ((p_ents == NULL) || (count == 0)) {
        return -AW_EINVAL;
    }

    if (AW_MUTEX_INIT(__g_entry_mutex, AW_SEM_Q_PRIORITY) == NULL) {
        return -AW_ENOMEM;
    }

    __gp_entry_tab = p_ents;
    __g_entry_cnt  = count;

    for (ret = 0; ret < count; ret++) {
        p_ents[ret].p_dev = NULL;
        p_ents[ret].p_ent = NULL;
    }

    ret = aw_iodev_create(&__g_def_stdio, "aw-stdio");
    if (ret != AW_OK) {
        AW_MUTEX_TERMINATE(__g_entry_mutex);
        return ret;
    }
    /* TODO implement STDIO functions*/

    __gp_entry_tab[0].p_dev = &__g_def_stdio;
    __gp_entry_tab[1].p_dev = &__g_def_stdio;
    __gp_entry_tab[2].p_dev = &__g_def_stdio;

    return AW_OK;
}




/******************************************************************************/
int aw_io_entry_to_index (struct aw_io_entry *p_entry)
{
    int  index;

    if ((p_entry < &__gp_entry_tab[0]) ||
        (p_entry > &__gp_entry_tab[__g_entry_cnt - 1])) {
        return -AW_EINVAL;
    }

    index = (int)(p_entry - &__gp_entry_tab[0]);
    return index;
}




/******************************************************************************/
struct aw_io_entry *aw_io_index_to_entry (int index)
{
    if ((index < 0) || (index >= __g_entry_cnt)) {
        return NULL;
    }

    return &__gp_entry_tab[index];
}




/******************************************************************************/
struct aw_io_entry *aw_io_entry_alloc (void)
{
    int   i;

    __entry_tab_lock();
    for (i = 3; i < (int)__g_entry_cnt; i++) {
        if (__gp_entry_tab[i].p_ent == NULL) {
            __gp_entry_tab[i].p_ent = __IO_ENTRY_RESERVED;
            __entry_tab_unlock();
            return &__gp_entry_tab[i];
        }
    }
    __entry_tab_unlock();
    return NULL;
}




/******************************************************************************/
int aw_io_entry_free (struct aw_io_entry *p_entry)
{
    int index = 0;

    if (p_entry == NULL) {
        return -AW_EINVAL;
    }

    __entry_tab_lock();

    index = aw_io_entry_to_index(p_entry);
    if (index < 0) {
        __entry_tab_unlock();
        return -AW_EINVAL;
    }
    __gp_entry_tab[index].p_dev = NULL;
    __gp_entry_tab[index].p_ent = NULL;

    __entry_tab_unlock();

    return AW_OK;
}




/******************************************************************************/
aw_bool_t aw_io_entry_valid (int index)
{
    struct aw_io_entry *p_entry = aw_io_index_to_entry(index);

    if ((p_entry == NULL) ||
        (p_entry->p_ent == NULL) ||
        (p_entry->p_ent == __IO_ENTRY_RESERVED)) {
        return AW_FALSE;
    }
    return AW_TRUE;
}
