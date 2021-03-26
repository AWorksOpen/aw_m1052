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
 * \brief   LoRaNet SM-WaitRx Interface
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_SM_WAITRX_H
#define __AM_LRNET_SM_WAITRX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_internal.h"

/* 
 * note:
 * event listen:
 *   AM_LRNET_EVT_RX_FINISH:
 *   AM_LRNET_EVT_TX_FINISH:
 */

typedef enum {
    AM_LRNET_SM_WAITRX_BUSY,
    AM_LRNET_SM_WAITRX_TIMEOUT,
    AM_LRNET_SM_WAITRX_DONE,
} am_lrnet_sm_waitrx_status_t;

/**
 * \brief       Initialize a template state machine:
 *                  send a packet and wait for reply until timeout
 * \param[in]	p_tx        tx packet, NULL if only wait
 * \param[in]	tx_ctrl     tx control, AM_LRNET_TX_CTRL_*
 * \param[in]   timeout     rx timeout
 * \param[in]   pfn_accept  callback function to check if the received packet
 *                          is the one you are wating.
 *
 * \return	    0 on succuss, negative error code on failed.
 *
 * \note        sm_waitrx would not set free the p_tx packet
 */
int am_lrnet_sm_waitrx_init (
            am_lrnet_packet_t *p_tx,
            uint32_t           tx_ctrl,
            uint32_t           timeout,
            am_lora_bool_t   (*pfn_accept) (am_lrnet_packet_t *p_packet));


/**
 * \brief	sm_waitrx set free, call this function to transform its state to
 *          finnal state while it is working.
 */
void am_lrnet_sm_waitrx_free (void);

/**
 * \brief       Post event to sm_waitrx, return current status
 *
 * \details     if return value is not BUSY, means work is finished;
 *              if return value is DONE, please call function
 *              am_lrnet_sm_waitrx_rx_packet_get to() to fetch the packet.
 *
 * \param[in]	evt         event
 * \param[out]	p_arg       argument
 *
 * \return	    current status of sm_waitrx;
 */
am_lrnet_sm_waitrx_status_t am_lrnet_sm_waitrx_evt_in (uint32_t  evt,
                                                       void     *p_arg);

am_lrnet_packet_t* am_lrnet_sm_waitrx_rx_packet_get (void);

#ifdef __cplusplus
}
#endif

#endif  /* __AM_LRNET_SM_WAITRX_H */

/* end of file */
