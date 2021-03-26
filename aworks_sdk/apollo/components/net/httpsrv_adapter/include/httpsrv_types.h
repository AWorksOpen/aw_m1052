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
 * \brief Compiler defines for lwip.
 *
 * \internal
 * \par History
 * - 130604, orz, modify for anywhere platform.
 * - 110506, orz, First implementation.
 * \endinternal
 */

#ifndef __HTTPSRV_TYPES_H__
#define __HTTPSRV_TYPES_H__

#include "aw_types.h"

#define bool  aw_bool_t
#define false AW_FALSE
#define true  AW_TRUE

#ifndef LWIP_IPV4
#define LWIP_IPV4      (1)
#endif

#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? \
        ipaddr_ntoa_r((const ip_addr_t*)(src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? ipaddr_aton((src),(ip_addr_t*)(dst)) : 0)

#define taskYIELD()       aw_task_yield()
#define vTaskDelete(a)    httpsrv_thread_delete(a)



#endif /* __HTTPSRV_ARCH_H__ */

/* end of file */
