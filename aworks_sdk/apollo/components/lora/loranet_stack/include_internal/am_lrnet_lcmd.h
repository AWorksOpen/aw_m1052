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
 * \brief   LoRaNet LCMD
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_LCMD_H
#define __AM_LRNET_LCMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_internal.h"

#define AM_LRNET_LCMD_APP                    0
#define AM_LRNET_LCMD_NONE                   1
#define AM_LRNET_LCMD_DETECTEXEREQ           2
#define AM_LRNET_LCMD_DETECTEXEACK           3
#define AM_LRNET_LCMD_DETECT                 4
#define AM_LRNET_LCMD_DETECTACK              5
#define AM_LRNET_LCMD_ECHOREQ                6
#define AM_LRNET_LCMD_ECHOACK                7
#define AM_LRNET_LCMD_GFETCHEXEREQ           8
#define AM_LRNET_LCMD_GFETCHEXEACK           9
#define AM_LRNET_LCMD_GFETCH                 10
#define AM_LRNET_LCMD_GFETCHACK              11
#define AM_LRNET_LCMD_TIMEBC                 12
#define AM_LRNET_LCMD_TIMEBCEXEREQ           13
#define AM_LRNET_LCMD_TIMEBCEXEACK           14
#define AM_LRNET_LCMD_EREPORT                15
#define AM_LRNET_LCMD_EREPORTACK             16
#define AM_LRNET_LCMD_UPSTREAMBC             17
#define AM_LRNET_LCMD_UPSTREAMDATA           18


void am_lrnet_lcmd_app_packet_set (am_lrnet_packet_t *p_packet,
                                   const uint8_t     *p_data,
                                   uint8_t            data_size);


typedef struct {
    uint16_t time_piece_width;

    /**
     * \brief
     *      timepiece_num = LORANET_NUM_LV_TO_NUM(timepiece_num_lv)
     */
    uint8_t  time_piece_lv;

    uint16_t start_addr;
    uint16_t end_addr;
    uint8_t  detect_id;
} am_lrnet_lcmd_detect_t;

void am_lrnet_lcmd_detect_init (am_lrnet_lcmd_detect_t *p_detect,
                                uint16_t                start_addr,
                                uint16_t                end_addr,
                                uint8_t                 time_piece_lv);

int am_lrnet_lcmd_detect_packet_set (am_lrnet_packet_t            *p_packet,
                                     const am_lrnet_lcmd_detect_t *p_detect);

int am_lrnet_lcmd_detect_parser (am_lrnet_packet_t      *p_packet,
                                 am_lrnet_lcmd_detect_t *p_detect);

int am_lrnet_lcmd_detectexereq_packet_set (
        am_lrnet_packet_t            *p_packet,
        const am_lrnet_lcmd_detect_t *p_detect);

int am_lrnet_lcmd_detectexereq_parser (am_lrnet_packet_t      *p_packet,
                                       am_lrnet_lcmd_detect_t *p_detect);

typedef struct {
    uint16_t   ack_count;
    uint16_t   error_count;
    uint8_t   *p_addr_list;       /**< \brief 要求分配lpayload最大大小的空间 */
    uint8_t    addr_list_item_num;

    /** \brief private */
    am_lrnet_packet_t *_p_packet;
} am_lrnet_lcmd_detectexeack_t;

/** \brief 使用p_packet的lpayload内存 */
void am_lrnet_lcmd_detectexeack_init (am_lrnet_lcmd_detectexeack_t *p_exeack);

void am_lrnet_lcmd_detectexeack_free (am_lrnet_lcmd_detectexeack_t *p_exeack);

int am_lrnet_lcmd_detectexeack_parser (
        am_lrnet_packet_t            *p_packet,
        am_lrnet_lcmd_detectexeack_t *p_detectexeack);

int am_lrnet_lcmd_detectexeack_packet_set (
        am_lrnet_packet_t                  *p_packet,
        const am_lrnet_lcmd_detectexeack_t *p_detectexeack);

int am_lrnet_lcmd_detectexeack_addr_set (am_lrnet_lcmd_detectexeack_t *p_ack,
                                         uint16_t                      addr);

void am_lrnet_lcmd_detectexeack_foreach_addr (
        am_lrnet_lcmd_detectexeack_t *p_ack,
        void                          pfn_cb (uint16_t  addr,
                                              uint16_t  idx,
                                              void     *p_arg),
        void                         *p_cb_arg);

int am_lrnet_lcmd_detectack_packet_set (am_lrnet_packet_t *p_packet);

int am_lrnet_lcmd_echoreq_set (am_lrnet_packet_t *p_packet,
                               uint16_t           payload_size);

int am_lrnet_lcmd_echoack_set (am_lrnet_packet_t *p_packet,
                               am_lrnet_packet_t *p_echoreq);

typedef struct {
    uint16_t  time_piece_width;     /**< \brief TimePieceWidth */
    uint8_t   fetch_data_size;      /**< \brief FetchDataSize */
    uint8_t   data_size;            /**< \brief size of data */

    /** \brief DevAddrs[0], DevAddrs[1..32]  */
    am_lrnet_bitmap_addr_t dev_addr;

    uint8_t  *p_data;               /**< \brief Data */
} am_lrnet_lcmd_gfetch_t;

void am_lrnet_lcmd_gfetch_init (am_lrnet_lcmd_gfetch_t  *p_gfetch,
                                am_lrnet_bitmap_addr_t  *p_bitmap_addr,
                                uint8_t                  fetch_data_size,
                                const uint8_t           *p_data_tx,
                                uint8_t                  data_tx_size);

int am_lrnet_lcmd_gfetch_parser (am_lrnet_packet_t      *p_packet,
                                 am_lrnet_lcmd_gfetch_t *p_gfetch);

int am_lrnet_lcmd_gfetch_packet_set (am_lrnet_packet_t            *p_packet,
                                     const am_lrnet_lcmd_gfetch_t *p_gfetch);

am_lora_static_inline
int am_lrnet_lcmd_gfetchexereq_parser (am_lrnet_packet_t      *p_packet,
                                       am_lrnet_lcmd_gfetch_t *p_gfetch)
{
    return am_lrnet_lcmd_gfetch_parser(p_packet, p_gfetch);
}

int am_lrnet_lcmd_gfetchexereq_packet_set (
            am_lrnet_packet_t            *p_packet,
            const am_lrnet_lcmd_gfetch_t *p_gfetch);

int am_lrnet_lcmd_gfetchack_packet_set (am_lrnet_packet_t *p_packet);

typedef struct {
    uint8_t   fetch_data_size;
    uint8_t  *p_fetch_data_list;
    uint8_t   fetch_data_num;

    am_lrnet_bitmap64_t fetch_data_valid;

    /** \brief private */
    am_lrnet_packet_t *_p_packet;
} am_lrnet_lcmd_gfetchexeack_t;

void am_lrnet_lcmd_gfetchexeack_init (am_lrnet_lcmd_gfetchexeack_t *p_exeack,
                                      const am_lrnet_lcmd_gfetch_t *p_gfetch);

void am_lrnet_lcmd_gfetchexeack_free (am_lrnet_lcmd_gfetchexeack_t *p_exeack);

void am_lrnet_lcmd_gfetchexeack_data_set (
         am_lrnet_lcmd_gfetchexeack_t *p_gfetchexeack,
         uint16_t                      piece_cur,
         uint8_t                      *p_data,
         uint8_t                       data_size);

int am_lrnet_lcmd_gfetchexeack_packet_parser (
            am_lrnet_packet_t            *p_packet,
            am_lrnet_lcmd_gfetchexeack_t *p_gfetchexeack);

int am_lrnet_lcmd_gfetchexeack_packet_set (
            am_lrnet_packet_t                  *p_packet,
            const am_lrnet_lcmd_gfetchexeack_t *p_gfetchexeack);

typedef struct {
    uint32_t tick;
    time_t   utc;
    uint16_t initial_ttl;   /**< \brief auto fill by __lcmd_timedata_set() */
} am_lrnet_lcmd_timedata_t;

int am_lrnet_lcmd_timebc_packet_set (am_lrnet_packet_t *p_packet,
                                     uint32_t           tick,
                                     time_t             utc,
                                     uint16_t           ttl);

int am_lrnet_lcmd_timebc_packet_parser (am_lrnet_packet_t *p_packet,
                                        uint32_t          *p_tick,
                                        time_t            *p_utc);

void am_lrnet_lcmd_timedata_init (am_lrnet_lcmd_timedata_t       *p_timedata,
                                  const am_lrnet_multihop_info_t *p_multihop);

time_t am_lrnet_lcmd_timedata_calib (const am_lrnet_multihop_info_t *p_multihop,
                                     const am_lrnet_lcmd_timedata_t *p_timedata);

int am_lrnet_lcmd_timebcexereq_packet_set (am_lrnet_packet_t *p_packet,
                                           uint32_t           tick,
                                           time_t             utc);

int am_lrnet_lcmd_timebcexereq_packet_parser (
        am_lrnet_packet_t  *p_packet,
        am_lrnet_lcmd_timedata_t *p_timedata);

int am_lrnet_lcmd_timebcexeack_packet_set (am_lrnet_packet_t *p_packet);

typedef struct {
    am_lrnet_lcmd_timedata_t timedata;
    uint8_t                  timepiece;
    am_lrnet_bitmap144_t     utime_bitmap;
} am_lrnet_lcmd_upstreambc_t;

int am_lrnet_lcmd_upstreambc_packet_parser (
            am_lrnet_packet_t          *p_packet,
            am_lrnet_lcmd_upstreambc_t *p_ubc);

int am_lrnet_lcmd_upstreambc_packet_set (
        am_lrnet_packet_t                *p_packet,
        const am_lrnet_lcmd_upstreambc_t *p_ubc);

am_lora_static_inline
am_lora_bool_t am_lrnet_lcmd_is_timebc (am_lrnet_packet_t *p_packet)
{
    return (p_packet->lcmd == AM_LRNET_LCMD_TIMEBC);
}

am_lora_static_inline
am_lora_bool_t am_lrnet_lcmd_is_timebcexereq (am_lrnet_packet_t *p_packet)
{
    return (p_packet->lcmd == AM_LRNET_LCMD_TIMEBCEXEREQ);
}

typedef struct {
    uint16_t         addr;
    const uint8_t   *p_data;
    uint16_t         size;
} am_lrnet_lcmd_ereport_t;

am_lora_static_inline
void am_lrnet_lcmd_ereport_init (am_lrnet_lcmd_ereport_t *p_ereport,
                                 uint16_t                 addr,
                                 const uint8_t           *p_data,
                                 uint16_t                 size)
{
    p_ereport->addr   = addr;
    p_ereport->p_data = p_data;
    p_ereport->size   = size;
}

int am_lrnet_lcmd_ereport_packet_set (
    am_lrnet_packet_t             *p_packet,
    const am_lrnet_lcmd_ereport_t *p_ereport);

int am_lrnet_lcmd_ereport_packet_parser (
    am_lrnet_packet_t       *p_packet,
    am_lrnet_lcmd_ereport_t *p_ereport);

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_LCMD_H */

/* end of file */
