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
 * \brief mbedtls entropy ALT
 *
 * \internal
 * \par Modification history
 *
 * - 16-11-02, axn, The first version.
 * \endinternal
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

//#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#include "aw_system.h"

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

/**
 * 线性同余法：Xi = ( X(i-1) * A + C ) Mod M
 *
 * A = 6, C = 7, M = 255
 */
#define __A    6
#define __C    7
#define __M    255

static unsigned char __r_seed (void)
{
    uint32_t ticks = aw_sys_tick_get();
    return (unsigned char)(ticks % __M);
}

int mbedtls_hardware_poll( void          *data,
                           unsigned char *output, 
                           size_t         len, 
                           size_t        *olen )
{
    int i;
    unsigned char x = __r_seed();

    for (i = 0; i < len; i++) {
        output[i] = x;
        x = (x * __A + __C) % __M;
    }
    *olen = len;

    return 0;
}

#endif

/* end of file */
