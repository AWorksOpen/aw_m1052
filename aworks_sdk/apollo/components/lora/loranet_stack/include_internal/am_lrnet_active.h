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
 * \brief   LoRaNet Active Module Interface
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */

#ifndef __LRNET_ACTIVE_H
#define __LRNET_ACTIVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_internal.h"

enum {
    AM_LRNET_EVT_DELAY_FINISH = AM_SM_EVENT_USER,

    /** \brief events for active_rxtx */
    AM_LRNET_EVT_LORA_RX_FINISH,    /**< \brief LoRa RF Rx Finish */
    AM_LRNET_EVT_LORA_TX_FINISH,    /**< \brief LoRa RF Tx Finish */
    AM_LRNET_EVT_LORA_CAD_FINISH,   /**< \brief LoRa RF CAD Finish */
    AM_LRNET_EVT_CMD_WAKEUPRX,
    AM_LRNET_EVT_CMD_RX,
    AM_LRNET_EVT_CMD_TX,
    AM_LRNET_EVT_CMD_TX_LBT,
    
    /** \brief RXTX return to idle state without finish message */
    AM_LRNET_EVT_CMD_FORCE_IDLE,    

    /** \brief Common API */
    AM_LRNET_EVT_API_ADDR_SET,
    AM_LRNET_EVT_API_MODE_SET,

    /** \brief Center API */
    AM_LRNET_EVT_API_TX,
    AM_LRNET_EVT_API_FETCH,
    AM_LRNET_EVT_API_PING,
    AM_LRNET_EVT_API_UPSTREAM_BC,
    AM_LRNET_EVT_API_TIMEBC,
    AM_LRNET_EVT_API_GFETCH,
    AM_LRNET_EVT_API_GFETCH_S,
    AM_LRNET_EVT_API_DETECT,

    /** \brief Raw API */
    AM_LRNET_EVT_API_RAW_TX,

    /** \brief Terminal API */
    AM_LRNET_EVT_API_EREPORT,
    AM_LRNET_EVT_API_BURST_REPORT,

    /** \brief events from active_rxtx to active_proc  */
    
    /** \brief Packet Rx Finish, p_arg is ((am_lrnet_packet_t*) p_packet) */
    AM_LRNET_EVT_RX_FINISH,

    /** \brief Packet Tx Finish, p_arg is ((int) error_no) */
    AM_LRNET_EVT_TX_FINISH,    

    /** \brief events to active_proc */
    /** \brief mount a state to proc and transform state to it */
    AM_LRNET_EVT_MOUNT,
    AM_LRNET_EVT_UNMOUNT,
    AM_LRNET_EVT_LOOP_WAKEUP,
    AM_LRNET_EVT_GO_TOP,
    AM_LRNET_EVT_RX_WINDOW_OPEN,       /**< \brief p_arg为开启接收的毫秒时间 */

    AM_LRNET_EVT_SM_WAITRX_EXIT,
};

/** \brief 不进行唤醒操作 */
#define AM_LRNET_TX_CTRL_NO_WAKEUP          __BIT(0)

/* 
 * \brief 仅发送一次且不进行唤醒，
 *        若需要ack，则尝试接收ack后返回，此时若返回值不为AM_OK，表示未收到ack；
 *        若无需ack，则发送完毕后直接返回AM_OK
 */
#define AM_LRNET_TX_CTRL_NO_RETRY           __BIT(1)

/* 禁止短唤醒，保证唤醒可靠 */
#define AM_LRNET_TX_CTRL_NO_SHORT_WAKEUP    __BIT(2)

extern am_sm_active_t *gp_lrnet_active_rxtx;

const char* am_lrnet_active_evt_to_str (uint32_t evt);

void am_lrnet_active_rxtx_init (void);

void am_lrnet_active_rxtx_post (uint32_t evt, void *p_arg);

void am_lrnet_active_rxtx_packet_tx (am_lrnet_packet_t *p_packet,
                                     uint32_t           tx_ctrl,
                                     uint32_t           delay_ms);

void am_lrnet_active_rxtx_packet_tx_lbt (am_lrnet_packet_t *p_packet,
                                         uint32_t           timeout);

void am_lrnet_active_rxtx_packet_rx (uint32_t       timeout_ms, 
                                     am_lora_bool_t is_continuous);

am_lora_bool_t am_lrnet_active_rxtx_busy (void);

extern am_sm_active_t *gp_lrnet_active_proc;

void am_lrnet_active_proc_init (
        am_sm_state_t         *p_top_state,
        am_sm_event_handler_t  p_top_handler,
        am_sm_state_t         *p_idle_state,
        am_sm_event_handler_t  p_idle_handler,
        am_sm_state_t         *p_busy_state,
        am_sm_event_handler_t  p_busy_handler);

void am_lrnet_active_proc_post (uint32_t evt, void *p_arg);

void am_lrnet_active_proc_mount (am_sm_event_handler_t p_handler);

void am_lrnet_active_proc_unmount (void);

void am_lrnet_active_proc_delay_timer_set (uint32_t value_ms, uint32_t msg);

void am_lrnet_active_proc_delay_timer_cancel (void);

/*
 * 退出proc的当前状态并hook一个rxwin状态在idle_state上
 * rxwin以LoRa Single Packet Rx模式接收 接收完毕后：
 *  1. 若未收到包则转换到上层状态
 *  2. 若收到包则交给超状态处理
 */
void am_lrnet_active_proc_rxwin (uint32_t ms);

am_lora_bool_t am_lrnet_active_proc_busy (void);

am_lora_static_inline
void am_lrnet_active_proc_go_top (void)
{
    am_lrnet_active_proc_post(AM_LRNET_EVT_GO_TOP, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* __LRNET_ACTIVE_H */

/* end of file */
