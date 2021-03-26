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
 * \brief ping
 *
 * \internal
 * \par Modification history
 *
 * - 2.00 18-06-01, phd, rework
 * - 1.00 15-12-02, axn, The first version.
 * \endinternal
 */

#ifndef __AW_PING_H
#define __AW_PING_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_ping
 * \copydoc aw_ping.h
 * @{
 */
#include "aworks.h"
#include "lwip/opt.h"
#include "aw_time.h"
#include "aw_sockets.h"

typedef struct aw_ping_s {
#ifdef LWIP_2_X
    struct sockaddr_storage to;

    void *pkt_send;
    void *pkt_recv;
#elif defined LWIP_1_4_X
    struct in_addr dest_addr;
    struct in_addr src_addr;
    struct icmp_echo_hdr *pkt;
#else
#error "no lwipopts.h"
#endif

    aw_timespec_t time_send;
    aw_timespec_t time_recv;

    uint16_t id;
    uint16_t seqno;

    uint8_t nextchar;
} aw_ping_t;


/**
 * \brief ping function
 *
 * \param[in]   ctx         An aw_ping_t instance.
 * \param[in]   dest_addr   Destination IP address or domain name.
 * \param[in]   src_addr    Source IP address in outgoing packet.
 * \param[in]   ttl         Set TTL(Time To Live) in the IP header for outgoing 
 *                          packet,
 *                          Get TTL in the IP header from incoming packet.
 * \param[in]   data_size   Ping send data size.
 * \param[in]   timout_ms   Ping receive timeout in milliseconds.
 *
 * \retval  >=0             Round-Trip Time in milliseconds
 * \retval  AW_ERROR        socket alloc failed
 * \retval  -AW_EAGAIN      send data failed
 * \retval  -AW_ETIMEDOUT   timeout
 *
 * \par Ê¾Àý
 * \code
 * #include "aw_ping.h"
 *
 * aw_ping_t ping;
 * int time_ms;
 * struct in_addr ipaddr;
 * inet_aton("192.168.28.70", &ipaddr);
 * time_ms = aw_ping(&ping, &ipaddr, NULL, 32, NULL, 1472, timeout_ms);
 * \endcode
 */
#ifdef LWIP_2_X
int aw_ping (aw_ping_t               *ctx,
             struct sockaddr_storage *to,
             struct sockaddr_storage *from,
             uint8_t                 *ttl,
             size_t                   data_size,
             int                      timout_ms);
#elif defined LWIP_1_4_X
int aw_ping (aw_ping_t      *ctx,
             struct in_addr *dest_addr,
             struct in_addr *src_addr,
             uint8_t         ttl_send,
             uint8_t        *ttl_recv,
             size_t          data_size,
             int             timout_ms);
#else
#error "no lwipopts.h"
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* __AW_PING_H */

/* end of file */

