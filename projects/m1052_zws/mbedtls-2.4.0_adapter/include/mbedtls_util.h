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
#ifndef __MBEDTLS_UTIL_H
#define __MBEDTLS_UTIL_H

#include <stdint.h>
#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_time.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define MBEDTLS_DEBUG_PRINT

#ifdef MBEDTLS_DEBUG_PRINT
#define MBEDTLS_DEBUG_INFOF(x)                   AW_INFOF(x)
#define MBEDTLS_DEBUG_WARNF(x)                   AW_WARNF(x)
#define MBEDTLS_DEBUG_ERRF(x)                    AW_ERRF(x)
#else
#define MBEDTLS_DEBUG_INFOF(x)
#define MBEDTLS_DEBUG_WARNF(x)
#define MBEDTLS_DEBUG_ERRF(x)
#endif /* MBEDTLS_DEBUG_PRINT */

void *mbedtls_mem_calloc (size_t n, size_t size);
void  mbedtls_mem_free (void *pstr);
void  mbedtls_calloc_free_prepare (void);

#ifdef __cplusplus
}
#endif

#endif /* __MBEDTLS_UTIL_H */

/* end of file */

