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
 * \brief       AMetal SMF internal function & definition
 * \note        DO NOT include this header file directly.
 *
 * \internal
 * \par modification history:
 * - 1.00 16-10-12  ebi, first implementation
 * \endinternal
 */

#ifndef __AM_SM_INTERNAL_H
#define __AM_SM_INTERNAL_H

#include "am_sm.h"
#include "am_sm_active.h"
#include "am_sm_srcfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (AM_SRCFG_SM_UNITY_TESTING)
#define am_sm_local
#else
#define am_sm_local     static
#endif

/**
 * \name static inline∂®“Â
 * @{
 */
#if defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION)
#define am_sm_inline         __inline
#define am_sm_static_inline  static  __inline /**< \brief ARM±‡“Î∆˜inlineπÿº¸◊÷ */

#elif defined (__ICCARM__)
#define am_lora_inline         inline
#define am_sm_static_inline   static inline   /**< \brief IAR±‡“Î∆˜inlineπÿº¸◊÷ */

#elif defined   (__GNUC__)
#define am_sm_inline         inline
#define am_sm_static_inline  static  inline   /**< \brief GNU±‡“Î∆˜inlineπÿº¸◊÷ */

#elif defined   (__TASKING__)
#define am_sm_inline         inline
#define am_sm_static_inline  static inline    /**< \brief TASKING±‡“Î∆˜inlineπÿº¸◊÷ */

#elif defined   (WIN32)
#define am_sm_inline         __inline
#define am_sm_static_inline  static __inline

#endif /* __CC_ARM */
/** @} */

/**
 * \name define bool value
 * @{
 */
#define  AM_SM_FALSE   0    /**< define AW_FALSE */
#define  AM_SM_TRUE    1    /**< define AW_TRUE */
/* @} */

/**
 * \name am_sm_status
 * @{
 */
#define AM_SM_STATUS_NOT_AT_WORK     (0u) /**< \brief state machine uninitialized */
#define AM_SM_STATUS_WORKING         (1u) /**< \brief state machine working */
#define AM_SM_STATUS_EXITED          (2u) /**< \brief state machine on final state */
/* @} */

/**
 * \brief    process a event directly
 * \details  import event via p_this->event and process it directly
 *              e.p.
 *              AM_SM_CUR_STATE(p_this) = event;
 *              am_sm_event_process(p_this);
 * \note     internal function
 */
void am_sm_event_process (am_sm_t *p_this);

/**
 * \brief    Obtain state machine's status
 * \details  Did not check NULL input for speed optimize.
 * \return   state machine status, AM_SM_STATUS_*
 * \note     internal function
 */
uint8_t am_sm_status_get (am_sm_t *p_this);


/******************************************************************************/
/** \brief initialize a FIFO */
am_sm_static_inline
void am_sm_event_fifo_init (am_sm_event_fifo_t *p_fifo,
                            am_sm_event_t      *p_mem,
                            uint8_t             len)
{
    p_fifo->p_mem = p_mem;
    p_fifo->len   = len;
    p_fifo->head  = 0;
    p_fifo->tail  = 0;
    p_fifo->used  = 0;
}

/******************************************************************************/
/** \brief return value not equal to 0 if FIFO empty */
am_sm_static_inline
uint8_t am_sm_event_fifo_isempty (am_sm_event_fifo_t *p_fifo)
{
    return (p_fifo->used == 0);
}

/******************************************************************************/
/** \brief return value not equal to 0 if FIFO full */
am_sm_static_inline
uint8_t am_sm_event_fifo_isfull (am_sm_event_fifo_t *p_fifo)
{
    return (p_fifo->used == p_fifo->len);
}

/******************************************************************************/
/**
 * \brief  put event into fifo
 * \note   check fifo not full before calling!
 */
am_sm_static_inline
void am_sm_event_fifo_put (am_sm_event_fifo_t  *p_fifo,
                           const am_sm_event_t *p_event)
{
    p_fifo->p_mem[p_fifo->tail] = *p_event;

    p_fifo->tail++;
    if (p_fifo->tail == p_fifo->len) {
        p_fifo->tail = 0;
    }

    p_fifo->used++;
}

/******************************************************************************/
/**
 * \brief  get event from fifo
 * \note   check fifo is not empty before calling!
 */
am_sm_static_inline
void am_sm_event_fifo_get (am_sm_event_fifo_t *p_fifo, am_sm_event_t *p_event)
{
    *p_event = p_fifo->p_mem[p_fifo->head];

    p_fifo->head++;
    if (p_fifo->head == p_fifo->len) {
        p_fifo->head = 0;
    }

    p_fifo->used--;
}

/******************************************************************************/
/**
 * \brief  empty the fifo
 */
am_sm_static_inline
void am_sm_event_fifo_empty (am_sm_event_fifo_t *p_fifo)
{
    p_fifo->used = 0;
    p_fifo->head = 0;
    p_fifo->tail = 0;
}

#ifdef __cplusplus
}
#endif

#endif /* __AM_SM_INTERNAL_H */

/* end of file */
