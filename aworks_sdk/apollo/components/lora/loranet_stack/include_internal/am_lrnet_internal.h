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
 * \brief   LoRaNet Internal Common Interface
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_INTERNAL_H
#define __AM_LRNET_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* LoRaNet */
#include "am_lrnet.h"

/* SMF */
#include "am_sm.h"
#include "am_sm_active.h"

/* Lib C */
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* lora-utilities */
#include "am_lora_utilities.h"
#include "am_lora_aes.h"
#include "am_lora_cmac.h"
#include "am_lora_data.h"
#include "am_lpsoftimer.h"
#include "am_sx127x.h"

#include "am_lrnet_srcfg.h"

#define __GET_UINT8(buf, n)       (buf[n])

#define __GET_UINT16(buf, n)      ((((uint16_t)buf[n])         & 0x00ff) |     \
                                   (((uint16_t)buf[n+1] << 8)  & 0xff00) )

#define __GET_UINT32(buf, n)    ((((uint32_t)buf[n])         & 0x000000ff) |   \
                                 (((uint32_t)buf[n+1] << 8)  & 0x0000ff00) |   \
                                 (((uint32_t)buf[n+2] << 16) & 0x00ff0000) |   \
                                 (((uint32_t)buf[n+3] << 24) & 0xff000000) )

#define __PUT_UINT8(buf, var, n)   (buf[n] = var)

#define __PUT_UINT16(buf, var, n)  \
                               ((buf[n]   = (uint8_t) (( var) & 0x00ff)),  \
                               (buf[n+1] = (uint8_t) (((var) & 0xff00) >> 8)))

#define __PUT_UINT32(buf, var, n)  \
            ((buf[n]   = (uint8_t) ( (var) & 0x000000ff)),       \
             (buf[n+1] = (uint8_t) (((var) & 0x0000ff00) >> 8)), \
             (buf[n+2] = (uint8_t) (((var) & 0x00ff0000) >> 16)),\
             (buf[n+3] = (uint8_t) (((var) & 0xff000000) >> 24)))

#define __BIT(n)                             (1 << n)
#define __BITS_MASK(start_bit, len)          ((~((~0u) << (len))) << start_bit)
#define __VALUE_TO_BITS(value, start_bit, len)   \
            (((value) << (start_bit)) & __BITS_MASK(start_bit, len))
#define __VALUE_FROM_BITS(value, start_bit, len) \
            (((value) & __BITS_MASK((start_bit), (len))) >> (start_bit))

#define __ADDR_MSB(addr)            __VALUE_FROM_BITS((addr), 8, 8)
#define __ADDR_LSB(addr)            __VALUE_FROM_BITS((addr), 0, 8)

/**
 * \name Alignment macros
 * @{
 */
#ifndef __ALIGN_DOWN
#define __ALIGN_DOWN(x, a) ((x) & -(a))
#endif

#ifndef __ALIGN_UP
#define __ALIGN_UP(x, a) (-(-(x) & -(a)))
#endif
/** @} */

#define __VTOS(n)     #n          /**< \brief Convert n to string */
#define __SVAL(m)     __VTOS(m)   /**< \brief Get string value of macro m */

#define AM_LRNET_CALL_REPEATEDLY_AGAINST_CODE       \
            do {                                    \
                static uint8_t static_init = 0;     \
                if (!static_init) {                 \
                    static_init = 1;                \
                } else {                            \
                    return;                         \
                }                                   \
            } while (0);

/**
 * \name LoRaNet Error Code Definition (internal)
 * @{
 */
#define AM_LRNET_RC_MIC_ERROR                3003

/** \brief Miscellaneous Functions(internal) */
#define AM_LRNET_NELEMENT(array) (sizeof(array) / sizeof(array[0]))

/**
 * \brief  num = 2^num_lv
 */
#define AM_LRNET_NUM_LV_TO_NUM(num_lv)    (1 << (num_lv))

extern struct am_lrnet_global {

    /* Buffer */
    uint8_t rf_txbuf[244];

    /* Config */
    am_lrnet_net_cfg_t         net_cfg;
    am_lrnet_role_common_cfg_t role_cfg;
    am_lrnet_common_evt_cb_t   common_evt_cb;

    /* pre-calculate const */
    uint16_t wakeup_packet_ms;
    uint16_t typical_packet_ms;
    uint16_t full_packet_ms;
    uint16_t timepiece_rx_timeout;
    uint16_t symbol_time_us;    /**< Ts = 1/Rs, unit: us */
    
    /* time of sending the biggest packet between router */
    uint32_t p2p_ms_max;        

    /* Var */
    uint16_t detect_id;
    time_t   last_wakeup_send_rtc;         /* 上次唤醒发送时间 */
    time_t   net_active_rtc;               /* 上次网络活跃时间 */

    am_lora_bool_t init_flg;
} g_lrnet_global;

#include "am_lrnet_hal.h"

/** \brief tools functions */
am_lora_static_inline
am_lora_bool_t am_lrnet_addr_is_broadcast (uint16_t addr)
{
    return (AM_LRNET_ADDR_BROADCAST == addr) || 
            AM_LRNET_ADDR_IS_GROUP_CAST(addr);
}

am_lora_static_inline
uint32_t am_lrnet_ttl_to_ms (uint16_t ttl)
{
    return ttl << 6;  /* 1 TTL = 64ms = 2^6ms */
}

am_lora_static_inline
uint16_t am_lrnet_ms_to_ttl (uint32_t ms)
{
    return (ms >> 6) + 1;  /* 1 TTL = 64ms = 2^6ms */
}

am_lora_static_inline
uint16_t am_lrnet_self_addr_get (void)
{
    return g_lrnet_global.role_cfg.dev_addr;
}

/** \brief 典型包在空时间 */
am_lora_static_inline
uint16_t am_lrnet_typical_packet_ms_get (void)
{
    return g_lrnet_global.typical_packet_ms;
}

/** \brief 典型包超时时间 */
am_lora_static_inline
uint16_t am_lrnet_typical_packet_timeout_get (void)
{
    return g_lrnet_global.typical_packet_ms
         + g_lrnet_global.net_cfg.packet_proc_time;
}

/** \brief 时间片计算 */
uint16_t am_lrnet_timepiece_calc (uint16_t answer_size);

am_lora_static_inline
am_lrnet_mode_t am_lrnet_role_mode_get (void)
{
    return g_lrnet_global.role_cfg.mode;
}

/** \brief 判断是否允许发送短唤醒 */
am_lora_bool_t am_lrnet_short_wakeup_allowable (void);

/** \brief 获取深度睡眠周期 */
uint32_t am_lrnet_deepsleep_interval_ms_get (void);

void am_lrnet_active_execute (void);

uint32_t am_lrnet_log2 (uint32_t n);

void am_lrnet_gmtime_hms (struct tm *p_time, time_t sec);

/** \brief Hal LoRa RF */
void am_lrnet_lora_tx (uint8_t *p_buf, uint8_t size);
void am_lrnet_lora_tx_repeat (uint8_t *p_data, uint8_t size, uint8_t offset);
void am_lrnet_lora_rx (uint32_t timeout_ms, uint8_t is_continuous);
void am_lrnet_lora_cad (void);
void am_lrnet_lora_sleep (void);

/* internal modules */
#include "am_lrnet_wait.h"
#include "am_lrnet_bitmap.h"
#include "am_lrnet_random.h"
#include "am_lrnet_delay_timer.h"
#include "am_lrnet_packet.h"
#include "am_lrnet_lcmd.h"
#include "am_lrnet_sm_waitrx.h"
#include "am_lrnet_sm_bctimepiece.h"
#include "am_lrnet_bctimepiece_flow.h"
#include "am_lrnet_active.h"
#include "am_lrnet_pool.h"
#include "am_lrnet_debug.h"

void am_lrnet_evt_callback_fetch_request (
        int          (*pfn_fetch_request) (am_lrnet_fetch_request_t *p_request),
        am_lrnet_packet_t *p_rx,
        am_lrnet_packet_t *p_tx);

typedef struct {
    am_sm_state_t         *p_top_state;
    am_sm_event_handler_t  p_top_handler;
    am_sm_state_t         *p_idle_state;
    am_sm_event_handler_t  p_idle_handler;
    am_sm_state_t         *p_busy_state;
    am_sm_event_handler_t  p_busy_handler;
} am_lrnet_role_t;

am_lora_static_inline
am_lora_bool_t am_lrnet_already_init (void)
{
    return g_lrnet_global.init_flg;
}

int am_lrnet_init (const am_lrnet_net_cfg_t         *p_lrnet_cfg,
                   const am_lrnet_role_common_cfg_t *p_role_cfg,
                   const am_lrnet_role_t            *p_role,
                   const am_lrnet_common_evt_cb_t   *p_common_evt);

int am_lrnet_init_lora_rf (const am_lrnet_rf_cfg_t *p_rf_cfg);

int am_lrnet_selftest (void);

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_INTERNAL_H */

/* end of file */
