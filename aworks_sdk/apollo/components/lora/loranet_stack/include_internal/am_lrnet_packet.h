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
 * \brief   LoRaNet Packet
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */


#ifndef __AM_LRNET_PACKET_H
#define __AM_LRNET_PACKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_internal.h"

/** \brief wakeup packet payload size */
#define AM_LRNET_PACKET_WAKEUP_PL_SIZE       3

/** \brief typical packet payload size */
#define AM_LRNET_PACKET_TYPICAL_PL_SIZE      16  

/** \brief packet msgtype: normal */
#define AM_LRNET_PACKET_MSGTYPE_NORMAL       0u  

/** \brief packet msgtype: multi-hop */
#define AM_LRNET_PACKET_MSGTYPE_MULTIHOP     1u  

/** \brief packet msgtype: wakeup */
#define AM_LRNET_PACKET_MSGTYPE_WAKEUP       2u  

/** \brief packet msgtype: ack */
#define AM_LRNET_PACKET_MSGTYPE_ACK          3u  

#define AM_LRNET_PACKET_FIX_PART_SIZE        6
#define AM_LRNET_PACKET_LOPT_FIX_PART_SIZE   6
#define AM_LRNET_PACKET_LOPT_SIZE_MAX        10
#define AM_LRNET_PACKET_MIC_MAX              1
#define AM_LRNET_PACKET_CRYPT_MAX            14
#define AM_LRNET_PACKET_EXTRA_MAX           (AM_LRNET_PACKET_FIX_PART_SIZE  +  \
                                             AM_LRNET_PACKET_LOPT_SIZE_MAX  +  \
                                             AM_LRNET_PACKET_MIC_MAX        +  \
                                             AM_LRNET_PACKET_CRYPT_MAX)

#define AM_LRNET_PACKET_PAYLOAD_BUF_SIZE     AM_LRNET_PACKET_PAYLOAD_SIZE_MAX

#define AM_LRNET_PACKET_BUF_SIZE            (AM_LRNET_PACKET_EXTRA_MAX      +  \
                                             AM_LRNET_PACKET_PAYLOAD_SIZE_MAX)

typedef struct {
    uint16_t tar_addr;
    uint16_t src_addr;
    uint16_t ttl_origin;           /**< \brief LOpt.TTL */
    uint8_t  route_list_len;
    uint8_t  route_list[4];
} am_lrnet_multihop_info_t;

typedef struct {
    uint8_t    net_id;
    uint8_t    msgtype;           /**< \brief LHeader.MsgType */
    uint8_t    lcmd;              /**< \brief LHeader.LCmd */
    uint8_t    no_ack;            /**< \brief LHeader.NoAckFlag */
    uint16_t   rx_addr;           /**< \brief RxAddr */
    uint16_t   tx_addr;           /**< \brief TxAddr */

    am_lrnet_multihop_info_t multihop;

    uint32_t   timestamp;         /**< \brief systick_ms when packet created */
    int16_t    rssi;              /**< \brief RSSI */

    uint16_t   lpayload_size;     /**< \brief size of LPayload */

    /** \brief LPayload */
    uint8_t    lpayload[AM_LRNET_PACKET_PAYLOAD_SIZE_MAX +
                        AM_LRNET_PACKET_CRYPT_MAX];
} am_lrnet_packet_t;

/*******************************************************************************
  Tools Function
*******************************************************************************/
am_lora_static_inline
uint16_t am_lrnet_packet_sender_get (am_lrnet_packet_t *p_packet)
{
    return p_packet->tx_addr;
}

am_lora_static_inline
uint16_t am_lrnet_packet_receiver_get (am_lrnet_packet_t *p_packet)
{
    return p_packet->rx_addr;
}

am_lora_static_inline
uint16_t am_lrnet_packet_src_get (am_lrnet_packet_t *p_packet)
{
    if (AM_LRNET_PACKET_MSGTYPE_MULTIHOP == p_packet->msgtype) {
        return p_packet->multihop.src_addr;
    } else {
        return p_packet->tx_addr;
    }
}

am_lora_static_inline
uint16_t am_lrnet_packet_tar_get (am_lrnet_packet_t *p_packet)
{
    if (AM_LRNET_PACKET_MSGTYPE_MULTIHOP == p_packet->msgtype) {
        return p_packet->multihop.tar_addr;
    } else {
        return p_packet->rx_addr;
    }
}

am_lora_static_inline
am_lora_bool_t am_lrnet_packet_is_broadcast (am_lrnet_packet_t *p_packet)
{
    return am_lrnet_addr_is_broadcast(am_lrnet_packet_receiver_get(p_packet));
}

uint16_t am_lrnet_packet_size_get_arg (uint16_t lpayload_size,
                                       uint8_t  msgtype,
                                       uint8_t  route_list_len);

uint16_t am_lrnet_packet_size_get (const am_lrnet_packet_t *p_packet);

am_lora_static_inline
uint32_t am_lrnet_packet_tx_ms_get (const am_lrnet_packet_t *p_packet)
{
    uint16_t rf_payload_size = am_lrnet_packet_size_get(p_packet);

    return am_lrnet_hal_lora_time_on_air(rf_payload_size);
}

am_lora_static_inline
uint32_t am_lrnet_packet_tx_ms_get_arg (uint8_t msgtype,
                                        uint8_t lpayload_size,
                                        uint8_t router_list_len)
{
    uint16_t rf_payload_size = am_lrnet_packet_size_get_arg(lpayload_size,
                                                            msgtype,
                                                            router_list_len);

    return am_lrnet_hal_lora_time_on_air(rf_payload_size);
}


am_lora_static_inline
void am_lrnet_packet_lpayload_set (am_lrnet_packet_t *p_packet,
                                   const uint8_t  *p_data,
                                   uint8_t         data_size)
{
    p_packet->lpayload_size = data_size;
    am_lora_memcpy1(p_packet->lpayload, p_data, data_size);
}

void am_lrnet_packet_module_init (void);

/* 构建packet_t 需要自行调用 am_lrnet_packet_free释放 */
am_lrnet_packet_t* am_lrnet_packet_normal_init (uint16_t tar_addr);
am_lrnet_packet_t* am_lrnet_packet_multihop_init (
                       am_lrnet_multihop_info_t *p_multi);
am_lrnet_packet_t* am_lrnet_packet_ack_init (
                       const am_lrnet_packet_t *p_rx_packet);
am_lrnet_packet_t* am_lrnet_packet_wakeup_init (uint16_t tar_addr,
                                                uint8_t  data_size,
                                                uint16_t leave_ms);
am_lrnet_packet_t* am_lrnet_packet_reply_init (
                       const am_lrnet_packet_t *p_packet_rx);

/**
 * \brief       Packet set free
 * \param[in]   pp_packet   pointer to a packet pointer
 *                          if *pp_packet is NULL, do nothing
 */
void am_lrnet_packet_free (am_lrnet_packet_t **pp_packet);

/* rx */
void am_lrnet_packet_rx (uint32_t timeout_ms, uint8_t is_continuous);
void am_lrnet_packet_wakeup_rx (void);
am_lrnet_packet_t* am_lrnet_packet_rx_get (void);

/* tx */
int am_lrnet_packet_tx (const am_lrnet_packet_t *p_packet);
uint16_t am_lrnet_packet_rf_payload_generate (const am_lrnet_packet_t *p_packet,
                                              uint8_t                 *p_buffer,
                                              uint8_t                  buf_size);

am_lora_bool_t am_lrnet_packet_tx_is_done (void);

typedef struct {
    uint16_t sleep_ms;
    uint16_t rx_win_ms;
} am_lrnet_packet_wakeup_info_t;

int am_lrnet_packet_wakeup_info_get (const am_lrnet_packet_t       *p_packet,
                                     am_lrnet_packet_wakeup_info_t *p_info);


void am_lrnet_packet_wakeup_set_rf_payload (uint8_t  *p_rf_payload,
                                            uint8_t   data_size,
                                            uint16_t  leave_ms);

/** some tools function */
am_lora_bool_t am_lrnet_packet_need_ack (am_lrnet_packet_t *p_packet);

/**
 * \brief        Calculate the remain TTL of a packet (unit: ms)
 * \details     If packet is not multihop packet, always return the max value.
 * \param[in]    p_packet:   packet
 * \return      remain TTL (unit: ms)
 */
uint32_t am_lrnet_packet_remain_ttl_ms_get (const am_lrnet_packet_t *p_packet);


void am_lrnet_multihop_info_upend_raw (uint8_t *p_list, uint8_t list_len);

void am_lrnet_multihop_info_upend (am_lrnet_multihop_info_t *p_info);

void am_lrnet_multihop_info_init (am_lrnet_multihop_info_t *p_info,
                                  uint16_t                  tar_addr,
                                  uint8_t                  *p_route_list,
                                  uint8_t                   router_list_len,
                                  uint32_t                  extra_ttl_ms,
                                  uint8_t                   is_backtrack);

uint32_t am_lrnet_packet_timeout_get (am_lrnet_packet_t *p_packet);

am_lora_bool_t am_lrnet_packet_payload_compare (const am_lrnet_packet_t *p_a,
                                                const am_lrnet_packet_t *p_b);

uint16_t am_lrnet_multihop_up_addr_get (am_lrnet_multihop_info_t *p_info);

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_PACKET_H */

/* end of file */
