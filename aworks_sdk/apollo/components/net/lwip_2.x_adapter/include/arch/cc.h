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
 * @file
 * @brief Compiler defines for lwip.
 *
 * @internal
 * @par History
 * - 13-06-04, orz, Porting for anywhere platform, add TI DSP support.
 * - 13-03-18, orz, First implementation.
 * @endinternal
 */

#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#include "lwipopts.h"

#ifdef LWIP_DEBUG
#    ifndef AW_DEBUG
#        define AW_DEBUG
#    endif
#endif

#include "arch/perf.h"

#include "aw_types.h"
#include "aw_int.h"
#include "aw_assert.h"
#include "aw_vdebug.h"

#define LWIP_ERRNO_INCLUDE <errno.h>

#ifdef _TIMEVAL_DEFINED
#    define LWIP_TIMEVAL_PRIVATE 0
#endif

#if MEM_LIBC_MALLOC
#    include <stdlib.h>
#endif

#if 1 /* this flags are defined in "iron/types.h" */
#    define U16_F "hu"
#    define S16_F "hd"
#    define X16_F "hx"
#    define U32_F "u"
#    define S32_F "d"
#    define X32_F "x"
#    define SZT_F "d"
#endif

#ifndef BYTE_ORDER
#    define BYTE_ORDER LITTLE_ENDIAN
#endif

#if defined(__CC_ARM) || defined(__ARMCOMPILER_VERSION) /* ARMCC compiler */
#    define PACK_STRUCT_FIELD(x) x
#    define PACK_STRUCT_STRUCT __attribute__((__packed__))
#    define PACK_STRUCT_BEGIN
#    define PACK_STRUCT_END
#elif defined(__IAR_SYSTEMS_ICC__) /* IAR Compiler */
#    define PACK_STRUCT_BEGIN
#    define PACK_STRUCT_STRUCT
#    define PACK_STRUCT_END
#    define PACK_STRUCT_FIELD(x) x
#    define PACK_STRUCT_USE_INCLUDES
#elif defined(__TMS470__) || defined(_TMS320C6X)
#    define PACK_STRUCT_BEGIN
#    define PACK_STRUCT_STRUCT
#    define PACK_STRUCT_END
#    define PACK_STRUCT_FIELD(x) x
#elif defined(__GNUC__) /* GNU GCC Compiler */
#    define PACK_STRUCT_FIELD(x) x
#    define PACK_STRUCT_STRUCT __attribute__((packed))
#    define PACK_STRUCT_BEGIN
#    define PACK_STRUCT_END
#endif

#define LWIP_PLATFORM_DIAG(x)                                                 \
    do {                                                                      \
        aw_kprintf x;                                                         \
    } while (0)
#define LWIP_PLATFORM_ASSERT(x)                                               \
    do {                                                                      \
        aw_kprintf(x);                                                        \
        aw_assert(0);                                                         \
    } while (0)

#define SYS_ARCH_DECL_PROTECT(x) AW_INT_CPU_LOCK_DECL(x)
#define SYS_ARCH_PROTECT(x) AW_INT_CPU_LOCK(x)
#define SYS_ARCH_UNPROTECT(x) AW_INT_CPU_UNLOCK(x)

#ifndef LWIP_NORAND
extern int rand(void);
#    define LWIP_RAND() (rand())
#endif

#endif /* __ARCH_CC_H__ */

/* end of file */
