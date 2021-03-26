#ifndef __IOT_SDK_AWORKS_PLATFORM_COMMON_H
#define __IOT_SDK_AWORKS_PLATFORM_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file aworks_platform_common.h
 */

#ifndef ENDL
    #define ENDL		"\r\n"
#endif

#define AW_PRINT_IP4ADDR_FMT \
    "%lu.%lu.%lu.%lu"

#define AW_PRINT_IP4ADDR_VAL(addr) \
    addr & 0x000000ff, \
   (addr & 0x0000ff00) >> 8, \
   (addr & 0x00ff0000) >> 16, \
    addr >> 24

#ifdef __cplusplus
}
#endif

#endif
