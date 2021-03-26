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
 * \brief Transaction File System Utility interfaces
 *
 * \internal
 * \par modification history:
 * - 1.00 17-07-06  mkr, first implementation
 * \endinternal
 */

#ifndef __AW_TXFS_UTILITY_H
#define __AW_TXFS_UTILITY_H

#include "aw_txfs_types.h"
#include "aw_txfs_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (AW_TXFS_EXEC_TICKS_DEBUG == 0)

#include "aw_system.h"

/** \brief execute ticks start */
#define AW_TXFS_EXEC_TICKS_START(ticks)   \
    do {                             \
        ticks = aw_sys_tick_get();   \
    } while (0)                      \

/** \brief get execute ticks result */
#define AW_TXFS_EXEC_TICKS_RESULT(ticks)   \
    do {                                   \
        ticks = aw_sys_tick_get() - ticks; \
    } while(0)                             \

#else
#define AW_TXFS_EXEC_TICKS_START(ticks)
#define AW_TXFS_EXEC_TICKS_RESULT(ticks)
#endif

/**
 * \brief find first setting bit in most significant bit
 *
 * \param[in] i  argument to find first set bit in
 *
 * \return most significant bit set
 *
 * \example
 *
 * i = 0x800,
 * 1000 0000 0000(b)
 * the return value is 12
 */
int aw_txfs_ffs_msb(uint32_t i);

/**
 * \brief find first setting bit in least significant bit
 *
 * \param[in] i argument to find first set bit in
 *
 * \return least significant bit set
 */
int aw_txfs_ffs_lsb(uint32_t i);

/**
 * \brief fill a buffer with a specified character
 *
 * \param[in] buf     pointer to buffer
 * \param[in] nbytes  number of bytes to fill
 * \param[in] ch      char with which to fill buffer
 */
void aw_txfs_bfill (FAST char *buf, int nbytes, FAST int ch);

/**
 * \brief calculate a CRC signature for a memory area
 *
 * \detail This routine calculates a 32-bit signature for a block, using a CRC.
 *
 * \param[in] data   pointer to data on which to perform CRC
 * \param[in] crcIn  Starting CRC value
 * \param[in] size   size of the <data> buffer
 *
 * \return 32-bit checksum
 */
uint32_t aw_txfs_cksum (char *data, uint32_t crcIn, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __HRFS_UTILITY_H */

/* end of file */

