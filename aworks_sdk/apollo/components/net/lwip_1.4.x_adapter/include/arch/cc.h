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
    #ifndef AW_DEBUG
        #define AW_DEBUG
    #endif
#endif
#include "aw_assert.h"

#include "aw_vdebug.h"		/* for printf() */
#include "arch/perf.h"
#include "aw_types.h"
#include "aw_int.h"

#if 1
#include "errno.h"
#define ERRNO
#define set_errno(err) AW_ERRNO_SET((err))
#else
#define LWIP_PROVIDE_ERRNO
#endif

#ifdef _TIMEVAL_DEFINED
#define LWIP_TIMEVAL_PRIVATE        0
#endif

#if MEM_LIBC_MALLOC
#include "stdlib.h"
#endif

typedef uint8_t		u8_t;
typedef int8_t		s8_t;
typedef uint16_t	u16_t;
typedef int16_t		s16_t;
typedef uint32_t	u32_t;
typedef int32_t		s32_t;
typedef uint32_t	mem_ptr_t;

#if 1 /* this flags are defined in "iron/types.h" */
#define U16_F       "hu"
#define S16_F       "hd"
#define X16_F       "hx"
#define U32_F       "u"
#define S32_F       "d"
#define X32_F       "x"
#define SZT_F       "d"
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER                  LITTLE_ENDIAN
#endif

/* Use LWIP error codes */
//#define LWIP_PROVIDE_ERRNO

#if defined(__CC_ARM) || defined(__ARMCOMPILER_VERSION) /* ARMCC compiler */
#define PACK_STRUCT_FIELD(x)        x
#define PACK_STRUCT_STRUCT        __attribute__ ((__packed__))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#elif defined(__IAR_SYSTEMS_ICC__)   /* IAR Compiler */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_USE_INCLUDES
#elif defined (__TMS470__) || defined(_TMS320C6X)
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#elif defined(__GNUC__)     /* GNU GCC Compiler */
#define PACK_STRUCT_FIELD(x)        x
#define PACK_STRUCT_STRUCT        __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#endif

#define LWIP_PLATFORM_DIAG(x)	    do {aw_kprintf x;} while(0)
#define LWIP_PLATFORM_ASSERT(x)     do {aw_kprintf(x); aw_assert(0);} while (0)

#define SYS_ARCH_DECL_PROTECT(x)    AW_INT_CPU_LOCK_DECL(x)
#define SYS_ARCH_PROTECT(x)         AW_INT_CPU_LOCK(x)
#define SYS_ARCH_UNPROTECT(x)       AW_INT_CPU_UNLOCK(x)

/**
 * 主机与网络字节转换
 * (为了速度这里使用宏, 但是不可以直接使用, 例如: htonl(x++) 就会错误.)
 */
#define LWIP_PLATFORM_BYTESWAP      1

#if BYTE_ORDER == LITTLE_ENDIAN

static __inline u16_t __cc_htons (u16_t  x)
{
    return  (u16_t)((((x) & 0x00ff) << 8) |
                    (((x) & 0xff00) >> 8));
}
static __inline u32_t __cc_htonl (u32_t  x)
{
    return  (u32_t)((((x) & 0x000000ff) << 24) |
                    (((x) & 0x0000ff00) <<  8) |
                    (((x) & 0x00ff0000) >>  8) |
                    (((x) & 0xff000000) >> 24));
}

#define LWIP_PLATFORM_HTONS(x)    __cc_htons(x)
#define LWIP_PLATFORM_HTONL(x)    __cc_htonl(x)
#else
#define LWIP_PLATFORM_HTONS(x)      x
#define LWIP_PLATFORM_HTONL(x)      x
#endif /*  BYTE_ORDER == LITTLE_ENDIAN */

#endif /* __ARCH_CC_H__ */

