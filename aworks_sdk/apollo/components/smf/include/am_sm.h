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
 * \brief       SMF implement
 *
 * \internal
 * \par modification history:
 * - 1.00 16-10-21  ebi, first first implementation
 * \endinternal
 */

#ifndef __AM_SM_H
#define __AM_SM_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name am_sm_handler_ret
 * @{
 */

/**
 * \brief    event unhandled
 * \details  A return value of handler, event did not be handled in the
 *           current handler and may be dispatched to the super-state's handler.
 */
#define AM_SM_RET_UNHANDLED     (0u)

/**
 * \brief    event be handled
 * \details  A return value of handler, event have been handled in the
 *           current state's handler.
 */
#define AM_SM_RET_HANDLED       (1u)

/**
 * \brief    transit to target state
 * \details  A return value of handler, event have been handled in the
 *           current state's handler and request to transit to specified state.
 * \note     do not use this macros directly, use AM_SM_TRAN() instead.
 */
#define AM_SM_RET_TRAN          (2u)

/**
 * \brief    transit to final state
 * \details  A return value of handler, event have been handled in the
 *           current state's handler and request to transit to final state.
 *           Current state will be set to AM_SM_STATE_FINAL.
 */
#define AM_SM_RET_EXIT          (3u)

/* @} */

/**
 * \brief  handler return value type definition,
 *         return value is handling result, based on AM_SM_RET_*
 */
typedef uint8_t am_sm_handler_ret_t;

/** \brief am_sm_t type definition */
typedef struct am_sm am_sm_t;

/** \brief event handler function pointer type definition */
typedef am_sm_handler_ret_t (*am_sm_event_handler_t)(am_sm_t *p_this);

/** \brief state of state machine type definition */
typedef struct am_sm_state {

    /** \brief state's super state */
    const struct am_sm_state    *p_super;

    /** \brief event handler */
    am_sm_handler_ret_t (*pfn_event_handler)(am_sm_t *p_this);

} am_sm_state_t;

/** \brief event structure definition */
typedef struct {
    uint32_t   num;     /**< \brief event number */
    void      *p_arg;   /**< \brief extra event argument */
} am_sm_event_t;

/**
 * \name am_sm_event_internal
 * @{
 */
#define AM_SM_EVENT_INIT   (0u) /**< \brief init action occurred event num */
#define AM_SM_EVENT_ENTRY  (1u) /**< \brief entry action occurred event num */
#define AM_SM_EVENT_EXIT   (2u) /**< \brief exit action occurred event num */
#define AM_SM_EVENT_USER   (3u) /**< \brief minimum value of custom event num */
/* @} */

/**
 * \name smf error number
 * @{
 */
#define AM_SM_RET_OK            0
#define AM_SM_RET_ERROR         -1
#define AM_SM_RET_EINVAL        22
/* @} */

/** \brief state machine definition */
struct am_sm {
    const am_sm_state_t       *p_cur_state;    /**< \brief current state */

    /**
     * \brief temporary var for tran_traget/pfn_action ...
     */
    union {
        const am_sm_state_t   *p_state;
        am_sm_event_handler_t  pfn_handler;
    } tmp;

    am_sm_event_t event;        /**< \brief event being processed */
};

/**
 * \brief   macros to represent initial pseudo state
 * \details State will be set when you call am_sm_init() function.
 */
#define AM_SM_STATE_INITIAL     (&g_sm_state_pseudo_initial)

/**
 * \brief macros to represent final pseudo state
 * \details state will be set when you return AM_SM_RET_EXIT in event handler
 */
#define AM_SM_STATE_FINAL       (&g_sm_state_pseudo_final)

/**
 * \brief   macros to make state machine transit to target state
 * \retval  AM_SM_RET_TRAN
 * \note    This macros can only be used in event handler function.
 */
#define AM_SM_TRAN(p_sm, p_target_state) (                  \
            ((p_sm)->tmp.p_state = (p_target_state)),       \
            AM_SM_RET_TRAN                                  \
        )

/** \brief macros to point up the sign is a state */
#define AM_SM_STATE(state)           (&(state))

/** \brief macros to get&set current state */
#define AM_SM_CUR_STATE(p_sm)        ((p_sm)->p_cur_state)

/** \brief macros to get&set super-state of current state */
#define AM_SM_SUPER_STATE(p_sm)      ((p_sm)->p_cur_state->p_super)

/** \brief macros to get&set event being processed */
#define AM_SM_CUR_EVENT(p_sm)        ((p_sm)->event)

/** \brief macros to get&set event num being processed */
#define AM_SM_CUR_EVENT_NUM(p_sm)    ((p_sm)->event.num)

/** \brief macros to get&set event argument being processed */
#define AM_SM_CUR_EVENT_ARG(p_sm)    ((p_sm)->event.p_arg)

/** \brief helper macros to define a state */
#define AM_SM_STATE_DECL(state)      static const am_sm_state_t state =

/**
 * \brief    initialize am_sm_t structure
 * \details  Current state will be set to AM_SM_STATE_INITIAL
 *
 * \param[in, out]  p_this         am_sm_t instance
 * \param[in]       pfn_initial    initial transition action function
 *
 * \retval  AM_SM_RET_OK       initialize succeed
 * \retval  -AM_SM_RET_EINVAL  parameter error
 *
 * \note    Do not operat a am_sm_t instance before initializing.
 */
int am_sm_init (am_sm_t *p_this, am_sm_event_handler_t pfn_initial);

/**
 * \brief    execute initial transition of state machine
 *
 * \param[in, out]  p_this    am_sm_t instance
 *
 * \retval  AM_SM_RET_OK   execute succeed
 */
int am_sm_start (am_sm_t *p_this);

/**
 * \brief  import a event and process
 *
 * \param[in, out]  p_this      am_sm_t instance
 * \param[in]       eventnum    event num
 * \param[in]       p_arg       event extend parameter
 *
 * \return  None
 */
void am_sm_event_in (am_sm_t *p_this, uint32_t event_num, void *p_arg);

/**
 * \brief    get shallow history state
 *
 * \details  For implementing a shallow history state, you need to record the
 *           shallow history state in the exit action.
 *           "hallow history state" is the current sub-state of shallow history
 *           state's super state.
 *
 * \param[in]   p_this          am_sm_t instance
 * \param[in]   p_super_state   shallow history state's super state
 *
 * \return   specified super-state's current sub-state
 */
const am_sm_state_t *am_sm_shallow_history_get (
        const am_sm_t *p_this, const am_sm_state_t *p_super_state);


/** \brief initial pseudo state for every am_sm_t */
extern const am_sm_state_t g_sm_state_pseudo_initial;

/** \brief final pseudo state for every am_sm_t */
extern const am_sm_state_t g_sm_state_pseudo_final;

/**
 * \brief  lock cpu to protect context
 *
 * \param[in]  None
 *
 * \return  cpu lock flag
 */
uint32_t am_sm_cpu_lock (void);

/**
 * \brief  unlock cpu
 *
 * \param[in]  lock:   the am_sm_cpu_lock return value
 *
 * \return  None
 */
void am_sm_cpu_unlock (uint32_t  lock);


#ifdef __cplusplus
}
#endif

#endif /* __AM_SM_H */

/* end of file */
