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
 * \brief 网络接口适配文件
 *
 * 使用本服务需要包含头文件
 * \code
 * #include "aw_netif_adapter.h"
 * \endcode
 *
 * \internal
 * \par Modification History
 * - 1.00 18-04-02  xdn, first implementation
 * \endinternal
 */

#ifndef __AW_NETIF_ADAPTER_H
#define __AW_NETIF_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \addtogroup grp_aw_if_xx
 * \copydoc aw_netif_adapter.h
 * @{
 */

#include "aworks.h"

#include "lwip/opt.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/dhcp.h"
#ifdef LWIP_2_X
#include "lwip/dhcp6.h"
#elif defined LWIP_1_4_X
#else
#error "no lwipopts.h"
#endif

typedef struct pbuf aw_net_buf_t;

#if ETH_PAD_SIZE
    #define PBUF_HEADER_DROP_PAD(pbuf)      pbuf_header(pbuf, -ETH_PAD_SIZE)
    #define PBUF_HEADER_RECLAIM_PAD(pbuf)   pbuf_header(pbuf, ETH_PAD_SIZE)
#else
    #define PBUF_HEADER_DROP_PAD(pbuf)
    #define PBUF_HEADER_RECLAIM_PAD(pbuf)
#endif

/** @} grp_aw_if_xx */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __AW_NETIF_ADAPTER_H */

/* end of file */
