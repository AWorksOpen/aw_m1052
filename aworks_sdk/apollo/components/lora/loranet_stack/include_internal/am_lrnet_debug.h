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
 * \brief   LoRaNet Debug Moudle Interface
 * \details
 *            Debug info divide to 8 levels, low level is more basal in system.
 *            Set AM_LRNET_SRCFG_DEBUG_INFO_LV to conceal the lower level debug 
 *                info.
 *            Set AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK to conceal the specified 
 *                level info.
 *
 *              Level       Description
 *              --------------------------
 *              0           Driver: HAL Layer, RAW data
 *              1           Packet: calc details
 *              2           Packet: timestamp & ACK, etc.
 *              3           Packet: RX TX Info           <- SDK User Debug Level
 *              4           Behavior: LCMD common
 *              5           Behavior: ROLE
 *              6           Main Info
 *              7           -
 *
 * \internal
 * \par modification history:
 * - 2017-02-28 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_DEBUG_H
#define __AM_LRNET_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_srcfg.h"

/*******************************************************************************
  Debug Info Format Ctrl
*******************************************************************************/
#define __FMT_HEAD                   "\033["

#if AM_LRNET_SRCFG_DEBUG
#define AM_LORA_DBG_INFO(...)    (void)am_lora_kprintf(__VA_ARGS__)
#else
#define AM_LORA_DBG_INFO(...)
#endif

#define __DEBUG_PRINTF               AM_LORA_DBG_INFO

#define AM_LRNET_DEBUG_FMT_NONE       __FMT_HEAD "0m"
#define AM_LRNET_DEBUG_FMT_CLR        __FMT_HEAD "2J"
#define AM_LRNET_DEBUG_FMT_BLACK      __FMT_HEAD "30m"
#define AM_LRNET_DEBUG_FMT_RED        __FMT_HEAD "31m"
#define AM_LRNET_DEBUG_FMT_GREEN      __FMT_HEAD "32m"
#define AM_LRNET_DEBUG_FMT_YELLOW     __FMT_HEAD "33m"
#define AM_LRNET_DEBUG_FMT_BLUE       __FMT_HEAD "34m"
#define AM_LRNET_DEBUG_FMT_PURPLE     __FMT_HEAD "35m"
#define AM_LRNET_DEBUG_FMT_CYAN       __FMT_HEAD "36m"
#define AM_LRNET_DEBUG_FMT_WHITE      __FMT_HEAD "37m"

#define AM_LRNET_DEBUG_FMT_SET(__n)   __DEBUG_PRINTF(__n)

/*******************************************************************************
  Debug Info Level
*******************************************************************************/
#define __UNUSED_MACRO_PRINTF(__NAME) \
            am_lora_static_inline           \
            void __NAME (const char *unused, ...) { (void) unused; }

#ifndef AM_LRNET_SRCFG_DEBUG_INFO_LV
#if AM_LRNET_SRCFG_DEBUG
#define AM_LRNET_SRCFG_DEBUG_INFO_LV   0    /**< \brief SDK User Debug Level */
#else
#define AM_LRNET_SRCFG_DEBUG_INFO_LV   255
#endif
#endif

#ifndef AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK
#define AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK      0xff
#endif

#define AM_LRNET_DEBUG_INFO  AM_LRNET_DEBUG_INFO_L7

#if (AM_LRNET_SRCFG_DEBUG && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV <= 0) && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK & (1 << 0)))
#define AM_LRNET_DEBUG_INFO_L0   __DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AM_LRNET_DEBUG_INFO_L0)
#endif

#if (AM_LRNET_SRCFG_DEBUG && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV <= 1) && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK & (1 << 1)))
#define AM_LRNET_DEBUG_INFO_L1   __DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AM_LRNET_DEBUG_INFO_L1)
#endif

#if (AM_LRNET_SRCFG_DEBUG && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV <= 2) && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK & (1 << 2)))
#define AM_LRNET_DEBUG_INFO_L2   __DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AM_LRNET_DEBUG_INFO_L2)
#endif

#if (AM_LRNET_SRCFG_DEBUG && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV <= 3) && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK & (1 << 3)))
#define AM_LRNET_DEBUG_INFO_L3   __DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AM_LRNET_DEBUG_INFO_L3)
#endif

#if (AM_LRNET_SRCFG_DEBUG && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV <= 4) && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK & (1 << 4)))
#define AM_LRNET_DEBUG_INFO_L4   __DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AM_LRNET_DEBUG_INFO_L4)
#endif

#if (AM_LRNET_SRCFG_DEBUG && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV <= 5) && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK & (1 << 5)))
#define AM_LRNET_DEBUG_INFO_L5   __DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AM_LRNET_DEBUG_INFO_L5)
#endif

#if (AM_LRNET_SRCFG_DEBUG && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV <= 6) && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK & (1 << 6)))
#define AM_LRNET_DEBUG_INFO_L6   __DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AM_LRNET_DEBUG_INFO_L6)
#endif

#if (AM_LRNET_SRCFG_DEBUG && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV <= 7) && \
    (AM_LRNET_SRCFG_DEBUG_INFO_LV_MASK & (1 << 7)))
#define AM_LRNET_DEBUG_INFO_L7   __DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AM_LRNET_DEBUG_INFO_L7)
#endif

/*******************************************************************************
  Assert
*******************************************************************************/
#if (AM_LRNET_SRCFG_DEBUG && AM_LRNET_SRCFG_DEBUG_ASSERT)
    
/** \brief Convert n to string */
#define __ASSERT_VTOS(n)     #n          

/** \brief Get string value of macro m */
#define __ASSERT_SVAL(m)     __ASSERT_VTOS(m)   
#define am_lrnet_assert(e_)    do {                                            \
    if (!(e_)) {                                                              \
        __DEBUG_PRINTF(__FILE__":"__ASSERT_SVAL(__LINE__)":("#e_")\n");   \
        while(1);                                                              \
    }                                                                          \
} while(0)

#else
am_lora_static_inline
void am_lrnet_assert (int unused)
{
    (void) unused;
}
#endif

/*******************************************************************************
  Common Debug Output Tools Functions
*******************************************************************************/

#if (AM_LRNET_SRCFG_DEBUG)
void am_lrnet_debug_rx_tx_info_print (const am_lrnet_packet_t *p_packet,
                                      uint8_t                  is_tx);

void am_lrnet_debug_data_print (const uint8_t *p_data, uint32_t size);

void am_lrnet_debug_addr_bitmap_print (const am_lrnet_bitmap_addr_t *p_addr);

void am_lrnet_debug_dev_addr_print (uint16_t dev_addr);

#else

am_lora_static_inline
void am_lrnet_debug_rx_tx_info_print (const am_lrnet_packet_t *p_packet,
                                      uint8_t                  is_tx)
{
    (void) p_packet;
    (void) is_tx;
}

am_lora_static_inline
void am_lrnet_debug_data_print (const uint8_t *p_data, uint32_t size)
{
    (void) p_data;
    (void) size;
}

am_lora_static_inline
void am_lrnet_debug_addr_bitmap_print (const am_lrnet_bitmap_addr_t *p_addr)
{
    (void) p_addr;
}

am_lora_static_inline
void am_lrnet_debug_dev_addr_print (uint16_t dev_addr)
{
    (void) dev_addr;
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_DEBUG_H */

/* end of file */
