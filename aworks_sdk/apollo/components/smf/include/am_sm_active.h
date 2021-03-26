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
 * \brief       SMF Active Object Header File
 * \details     Active Object supply a method for posting event to state
 *              machine, it works well no matter in a ISR, event handler, even
 *              in different task(in multitask environment).
 *
 * \internal
 * \par modification history:
 * - 1.00 16-10-25  ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_SM_ACTIVE_H
#define __AM_SM_ACTIVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_sm.h"

struct am_sm_event_fifo;

/**
 * \name am_sm_active
 * @{
 */

/** \brief event ring queue FIFO structure */
typedef struct {
    am_sm_event_t   *p_mem;     /**< \brief queue memory field */
    uint8_t          len;       /**< \brief queue length */
    uint8_t          head;      /**< \brief ring queue, head */
    uint8_t          tail;      /**< \brief ring queue, tail */
    uint8_t          used;      /**< \brief current element be used */
} am_sm_event_fifo_t;

/** \brief active object structure definition */
typedef struct am_sm_active {
    am_sm_t                 super;
    am_sm_event_fifo_t      fifo;
    uint8_t                 prio;
    uint8_t                 status;
    struct am_sm_active    *p_next;
} am_sm_active_t;

/** \brief state machine instance up-cast to active object */
#define AM_SM_TO_ACTIVE(p_sm)         (AM_CONTAINER_OF((p_sm), am_sm_active_t, super))

/** \brief active object instance downcasting to state machine */
#define AM_SM_FROM_ACTIVE(p_active)   (&(p_active)->super)

/**
 * \brief       active object initialize
 *
 * \param[in,out]   p_this          am_sm_active_t instance
 * \param[in]       pfn_initial     initial transition action function
 * \param[in]       p_event_queue   pointer to event queue
 * \param[in]       queue_length    event queue length
 * \param[in]       priority        priority of active object
 *                                  (lowest) 0 <= priority <= 31 (highest)
 *
 * \retval  AM_SM_RET_OK       initialize succeed
 * \retval  -AM_SM_RET_EINVAL
 *
 * \note    Do not operat a am_sm_t instance before initializing.
 */
int am_sm_active_init (am_sm_active_t        *p_this,
                       am_sm_event_handler_t  pfn_initial,
                       am_sm_event_t         *p_event_queue,
                       size_t                 queue_length,
                       uint8_t                priority);

/**
 * \brief       start a active object
 *
 * \details     Execute initial transition of state machine included.
 *              After start a active object, active object can receive event
 *              which be sent with am_sm_active_post().
 *
 * \param[in,out]   p_active   am_sm_active_t instance
 *
 * \retval      AM_SM_RET_OK       on succeed
 * \retval      -AM_SM_RET_EINVAL  parameter in vain
 * \retval      AM_SM_RET_ERROR    error eccur, maybe this active object is started
 */
int am_sm_active_start (am_sm_active_t *p_active);

/**
 * \brief       stop a active object immediately
 *
 * \details     Using this function to stop a active object immediately, once
 *              executing, active object's event queue will be empty so you had
 *              to treat it carefully.
 *              For security, stopping a active object with a return value
 *              AM_SM_RET_EXIT in a state machine event handler is recommanded.
 *
 * \param[in,out]   p_active   am_sm_active_t instance
 *
 * \return      AM_SM_RET_OK on success, negative errno code on fail.
 */
int am_sm_active_kill (am_sm_active_t *p_active);

/**
 * \brief       post a event to specified active object
 *
 * \param[in,out]   p_active    am_sm_active_t instance
 * \param[in]       event_num   user event number
 * \param[in]       p_arg       event extend parameter
 *
 * \retval      AM_SM_RET_OK on success, negative errno code on fail (FIFO full?).
 */
int am_sm_active_post (am_sm_active_t    *p_active,
                       uint32_t           event_num,
                       void              *p_arg);

/**
 * \brief       execute signal dispatch of active objects' fifo
 * \retval      AM_SM_RET_OK on idle
 */
int am_sm_active_exec (void);

/**
 * \brief       execute signal dispatch of active objects' fifo
 * \retval      AM_SM_RET_OK on idle
 */
int am_sm_active_exec_int (void);

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __AM_SM_ACTIVE_H */

/* end of file */
