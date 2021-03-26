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
 * \brief task 平台相关标准定义头文件
 *
 * \internal
 * \par modification history:
 * - 1.12 15-11-02  deo, fixes aw_semb_valid and
 *                       add aw_semc_valid aw_semc_set_invalid aw_semb_set_invalid
 * - 1.11 14-08-29  orz, do fixes to fit new RTK kernel due to API changed
 * - 1.10 13-03-04  zen, refine the description
 * - 1.00 12-09-14  orz, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_SEM_H
#define __AW_PSP_SEM_H

#include "rtk.h"
#include "aw_common.h"

/******************************************************************************/
#define AW_PSP_SEM_WAIT_FOREVER        AW_WAIT_FOREVER
#define AW_PSP_SEM_NO_WAIT             0

/******************************************************************************/
#define AW_PSP_SEM_EMPTY               0
#define AW_PSP_SEM_FULL                1

/******************************************************************************/
#define AW_PSP_SEM_Q_FIFO              0x00
#define AW_PSP_SEM_Q_PRIORITY          0x01
#define AW_PSP_SEM_DELETE_SAFE         0x02
#define AW_PSP_SEM_INVERSION_SAFE      0x04

/******************************************************************************/
typedef struct rtk_semb      *aw_psp_semb_id_t;
typedef struct rtk_semc      *aw_psp_semc_id_t;
typedef struct rtk_mutex     *aw_psp_mutex_id_t;

/******************************************************************************/
#define AW_PSP_SEMB_DECL(sem)          struct rtk_semb sem
#define AW_PSP_SEMB_DECL_STATIC(sem)   static AW_PSP_SEMB_DECL(sem)
#define AW_PSP_SEMB_IMPORT(sem)        extern AW_PSP_SEMB_DECL(sem)

aw_static_inline aw_psp_semb_id_t aw_psp_semb_init (
        struct rtk_semb *p_semb,
        int count)
{
    return rtk_semb_init(p_semb, count);
}

#define AW_PSP_SEMB_INIT(sem, initial_state, options)  \
            aw_psp_semb_init(&(sem), initial_state)

#define AW_PSP_SEMB_TAKE(sem, timeout)      aw_semb_take(&(sem), timeout)
#define AW_PSP_SEMB_GIVE(sem)               aw_semb_give(&(sem))
#define AW_PSP_SEMB_RESET(sem, reset_value) aw_semb_reset(&(sem), reset_value)
#define AW_PSP_SEMB_TERMINATE(sem)          aw_semb_terminate(&(sem))

/******************************************************************************/
static aw_inline aw_err_t aw_semb_take(aw_psp_semb_id_t sem_id, aw_tick_t timeout)
{
    return rtk_semb_take(sem_id, timeout);
}

/******************************************************************************/
static aw_inline aw_err_t aw_semb_give(aw_psp_semb_id_t sem_id)
{
    return rtk_semb_give(sem_id);
}

/******************************************************************************/
static aw_inline aw_err_t aw_semb_reset(
        aw_psp_semb_id_t sem_id,
        unsigned int reset_value )
{
    return rtk_semb_reset(sem_id, reset_value );
}


/******************************************************************************/
static aw_inline aw_err_t aw_semb_terminate(aw_psp_semb_id_t sem_id)
{
    return rtk_semb_terminate(sem_id);
}

/******************************************************************************/
#define AW_PSP_SEMC_DECL(sem)          RTK_SEMC_DECL(sem)
#define AW_PSP_SEMC_DECL_STATIC(sem)   static RTK_SEMC_DECL(sem)
#define AW_PSP_SEMC_IMPORT(sem)        extern RTK_SEMC_DECL(sem)

aw_static_inline aw_psp_semc_id_t aw_psp_semc_init (
        struct rtk_semc *p_semc,
        unsigned int count)
{
    return rtk_semc_init(p_semc, count);
}

#define AW_PSP_SEMC_INIT(sem, initial_count, options) \
            aw_psp_semc_init(&(sem), initial_count)

#define AW_PSP_SEMC_TAKE(sem, timeout)     aw_semc_take(&(sem), timeout)
#define AW_PSP_SEMC_GIVE(sem)              aw_semc_give(&(sem))
#define AW_PSP_SEMC_RESET(sem, reset_value) aw_semc_reset(&(sem), reset_value)
#define AW_PSP_SEMC_TERMINATE(sem)         rtk_semc_terminate(&(sem))




/******************************************************************************/
static aw_inline aw_err_t aw_semc_take(
        aw_psp_semc_id_t sem_id,
        unsigned int timeout)
{
    return rtk_semc_take(sem_id, timeout);
}

/******************************************************************************/
static aw_inline aw_err_t aw_semc_give(aw_psp_semc_id_t sem_id)
{
    return rtk_semc_give(sem_id);
}

/******************************************************************************/
static aw_inline aw_err_t aw_semc_reset(
        aw_psp_semc_id_t sem_id, unsigned int reset_value)
{
    return rtk_semc_reset(sem_id, reset_value);
}

/******************************************************************************/
static aw_inline aw_err_t aw_semc_terminate(aw_psp_semc_id_t sem_id)
{
    return rtk_semc_terminate(sem_id);
}

/******************************************************************************/
aw_static_inline aw_psp_mutex_id_t aw_psp_mutex_init (struct rtk_mutex *p_mutex)
{
    int             err;
    err = rtk_mutex_init(p_mutex);
    if (AW_OK == err) {
        return p_mutex;
    }
    return NULL;
}

#define AW_PSP_MUTEX_DECL(mutex)            RTK_MUTEX_DECL(mutex)
#define AW_PSP_MUTEX_DECL_STATIC(mutex)     static RTK_MUTEX_DECL(mutex)
#define AW_PSP_MUTEX_IMPORT(mutex)          extern RTK_MUTEX_DECL(mutex)
#define AW_PSP_MUTEX_INIT(mutex, options)   aw_psp_mutex_init(&(mutex))
#define AW_PSP_MUTEX_LOCK(mutex, timeout)   aw_mutex_lock(&(mutex), timeout)
#define AW_PSP_MUTEX_UNLOCK(mutex)          aw_mutex_unlock(&(mutex))
#define AW_PSP_MUTEX_TERMINATE(mutex)       aw_mutex_terminate(&(mutex))

/******************************************************************************/
aw_static_inline aw_err_t aw_mutex_lock(
        aw_psp_mutex_id_t p_mutex,
        unsigned int timeout)
{
    return rtk_mutex_lock(p_mutex, timeout);
}

/******************************************************************************/
aw_static_inline aw_err_t aw_mutex_unlock(aw_psp_mutex_id_t p_mutex)
{
    return rtk_mutex_unlock(p_mutex);
}

/******************************************************************************/
aw_static_inline aw_err_t aw_mutex_terminate(aw_psp_mutex_id_t p_mutex)
{
    return rtk_mutex_terminate(p_mutex);
}


#endif  /* __AW_PSP_SEM_H */

/* end of file */
