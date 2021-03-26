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
 * \brief util
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

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#endif

#include "mbedtls_util.h"
#include "aw_mem.h"

#if defined(MBEDTLS_PLATFORM_MEMORY)
#if defined(MBEDTLS_PLATFORM_FREE_MACRO) && \
    defined(MBEDTLS_PLATFORM_CALLOC_MACRO)

#else
void *mbedtls_mem_calloc (size_t n, size_t size)
{
    int *p_mem = calloc (n, size);
    if (NULL == p_mem) {
        mbedtls_printf( "mbedtls calloc failed!\r\n" );
    }
    return p_mem;
}

void  mbedtls_mem_free (void *pstr)
{
    if (NULL == pstr) {
        return;
    }
    free(pstr);
}

void mbedtls_calloc_free_prepare (void)
{
    mbedtls_platform_set_calloc_free( mbedtls_mem_calloc, mbedtls_mem_free);
}
#endif /* MBEDTLS_PLATFORM_FREE_MACRO && MBEDTLS_PLATFORM_CALLOC_MACRO */
#endif /* MBEDTLS_PLATFORM_MEMORY && !MBEDTLS_PLATFORM_{FREE,CALLOC}_MACRO */

/* end of file */

