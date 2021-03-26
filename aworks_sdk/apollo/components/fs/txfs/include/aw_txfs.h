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
 * \brief TXFS AWorks adapter layer
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23 mkr, first implementation.
 * \endinternal
 */
#ifndef __AW_HRFS_H
#define __AW_HRFS_H

#include "fs/aw_blk_dev.h"
#include "io/aw_io_device.h"
#include "aw_txfs_config.h"
#include "aw_txfs_publish.h"
/******************************************************************************/
/** \brief TXFS volume structure */
struct aw_txfs_vol {
    struct aw_iodev    io_dev;     /**< \brief base on I/O device to AWorks */
    struct aw_blk_dev *p_blkdev;   /**< \brief mount to block device */
    aw_txfs_dev_id     txfs_dev;   /**< \brief txfs device id */
};

/** \brief TXFS device file descriptor */
struct aw_txfs_file {
    struct aw_txfs_vol *p_txfs; /**< \brief TXFS volume structure */
    struct vfile       *p_file; /**< \brief file table pointer */
    int                 oflags; /**< \brief flags */
    /** \brief integral path name of the file */
    char                path[AW_TXFS_PATH_MAX + 1];
};

/**
 * \brief TXFS library initial
 *
 * \param[in] buffers  buffers number
 *
 * \return AW_OK
 */
int aw_txfs_buffer_config (uint32_t buffers);

/**
 * \brief TXFS initial
 * \param[in] p_vol_mem  hrfs volume memory
 * \param[in] vmem_size  the size of hrfs volume memory
 * \param[in] p_fil_mem  hrfs file descriptor memory
 * \param[in] fmem_size  the size of hrfs file descriptor
 *
 * \return errno
 */
int aw_txfs_init (void *p_vol_mem, unsigned int vmem_size,
                  void *p_fil_mem, unsigned int fmem_size);


#endif

/* end of file */
