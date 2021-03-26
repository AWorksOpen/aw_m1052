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
 * \brief header file for the TXFS component
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23  mkr, first implementation.
 * \endinternal
 */

#ifndef __AW_TXFS_MONITER_H
#define __AW_TXFS_MONITER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief The AW_TXFS_PBR_xxx defines mark the minimum set of
 *        info required for a partition boot record.
 */
/** \brief x86 jump instruction (3 bytes) */
#define AW_TXFS_PBR_JMP             (0x00)

/** \brief system ID string     (8 bytes) */
#define AW_TXFS_PBR_SYS_ID          (0x03)

/** \brief bytes per block      (2 bytes) */
#define AW_TXFS_PBR_BYTES_PER_BLK   (0x0b)

/** \brief # of blocks on vol   (4 bytes) */
#define AW_TXFS_PBR_NUM_BLKS        (0x20)

#ifdef __cplusplus
}
#endif

#endif /* __AW_TXFS_MONITER_H */

/* end of file */




