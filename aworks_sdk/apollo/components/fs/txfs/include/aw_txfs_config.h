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
 * \brief Highly Reliable File System configuration file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-07-06  mkr, first implementation
 * \endinternal
 */

#ifndef __AW_TXFS_CONFIG_H
#define __AW_TXFS_CONFIG_H

/**
 * \brief Whether using defer working node for commit data
 *
 * AW_TRUE using defer working node commit; AW_FALSE using commit task
 *
 */
#define AW_TXFS_IS_DEFER_COMMIT       (AW_TRUE)

/** \brief TXFS Execute time testing for debug */
#define AW_TXFS_EXEC_TICKS_DEBUG      (1u)

/** \brief Enable Debug for print information */
#define AW_TXFS_DEBUG_ENABLE          (0u)

/** \brief debug for checking dynamic memory usage in TXFS */
#define AW_TXFS_DEBUG_MEMALLOC_USAGE  (0u)

/** \brief max pathname length excluding EOS */
#define AW_TXFS_PATH_MAX              (255u)

/** \brief The maximum chars in a file name, including EOS */
#define AW_TXFS_MAX_FILENAME_LEN      (251u)

#endif /* __AW_TXFS_CONFIG_H */

/* end of file */
