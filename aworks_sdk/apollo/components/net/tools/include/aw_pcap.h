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
 * \brief generate a formated stream to save captured network data
 * which is fully supported by Wireshark/TShark
 *
 * \internal
 * \par Modification history
 *
 * - 1.00 18-03-23, phd, The first version.
 * \endinternal
 */

#ifndef __AW_PCAP_H
#define __AW_PCAP_H

#include <inttypes.h>

#include "aw_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_aw_pcap
 * \copydoc aw_pcap.h
 * @{
 */
typedef void (*aw_pcap_out_fn) (const void *, size_t);

void aw_pcap_start (aw_pcap_out_fn writer);

/* write packet */
void aw_pcap_dump (aw_pcap_out_fn  writer,
                   const void     *dump_buf,
                   size_t          nbytes,
                   uint32_t        ts_sec,
                   uint32_t        ts_usec);

void aw_pcap_dump_tv (aw_pcap_out_fn  writer,
                     const void      *dump_buf,
                     size_t           nbytes,
                     aw_timespec_t   *p_tv);

void aw_pcap_pkt_start (aw_pcap_out_fn writer,
                        size_t         nbytes,
                        uint32_t       ts_sec,
                        uint32_t       ts_usec);

void aw_pcap_pkt_start_tv (aw_pcap_out_fn  writer,
                           size_t          nbytes,
                           aw_timespec_t  *p_tv);

void aw_pcap_pkt_data (aw_pcap_out_fn  writer,
                       const void     *dump_buf,
                       size_t          nbytes);

/* @} grp_aw_if_aw_pcap */

#ifdef __cplusplus
}
#endif

#endif

/* end of file */
