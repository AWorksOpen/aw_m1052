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
 * \brief   LoRaNet SM-BcTimePiece Interface
 *          bctimepiece state machine, a sub state machine of active_proc
 *          function: broadcast a packet, then do timepiece receive
 *          usage:
 *              1. init by am_lrnet_sm_bctimepiece_init
 *              2. while following event join, 
 *                 call am_lrnet_sm_bctimepiece_evt_in  
 *                   AM_LRNET_EVT_RX_FINISH
 *                   AM_LRNET_EVT_TX_FINISH
 *                   AM_LRNET_EVT_DELAY_FINISH
 *              3. until evt_in return AM_LRNET_SM_BCTIMEPIECE_DONE
 *
 * \internal
 * \par modification history:
 * - 2017-07-04 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_SM_BCTIMEPIECE_H
#define __AM_LRNET_SM_BCTIMEPIECE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_internal.h"

/**
 * \brief workflow interface
 */
typedef struct am_lrnet_sm_bctimepiece_flow am_lrnet_sm_bctimepiece_flow_t;

struct am_lrnet_sm_bctimepiece_flow {

    /* step0. workflow init */
    void (*pfn_init) (am_lrnet_sm_bctimepiece_flow_t *p_flow,
                      uint16_t                       *p_piece_width,
                      uint16_t                       *p_piece_num);

    /* step1. bc */
    am_lrnet_packet_t* (*pfn_bc) (am_lrnet_sm_bctimepiece_flow_t *p_flow);

    /* step2. timepiece rx */
    void (*pfn_timepiece_rx) (am_lrnet_sm_bctimepiece_flow_t *p_flow,
                              am_lrnet_packet_t              *p_rx,
                              uint16_t                     cur_piece);

    /* step3. finish */
    void (*pfn_finish) (am_lrnet_sm_bctimepiece_flow_t *p_flow, int rx_err_cnt);
};

typedef enum {
    AM_LRNET_SM_BCTIMEPIECE_BUSY,    /**< \brief state machine working */
    AM_LRNET_SM_BCTIMEPIECE_DONE,    /**< \brief state machine on final state */
} am_lrnet_sm_bctimepiece_status_t;

/**
 * \brief       init the bctimepiece state machinek
 * \param[in]   p_workflow : implement of workflow interface
 */
void am_lrnet_sm_bctimepiece_init (am_lrnet_sm_bctimepiece_flow_t *p_workflow);

/**
 * \brief       event input
 * \return      current status of state machine
 */
am_lrnet_sm_bctimepiece_status_t am_lrnet_sm_bctimepiece_evt_in (
                                     uint32_t  evt,
                                     void     *p_arg);

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_SM_BCTIMEPIECE_H */

/* end of file */
