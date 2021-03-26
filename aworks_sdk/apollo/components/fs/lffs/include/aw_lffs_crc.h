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
 * \brief simple CRC functions
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */

#ifndef _AW_LFFS_CRC_H_
#define _AW_LFFS_CRC_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_lffs_core.h"

uint16_t aw_lffs_crc16update(const void *data, int length, uint16_t crc);
uint16_t aw_lffs_crc16sum(const void *data, int length);

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif

/* end of file */