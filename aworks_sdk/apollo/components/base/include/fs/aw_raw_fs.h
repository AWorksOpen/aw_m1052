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
 * \brief raw block device file system.
 *
 * \internal
 * \par modification history:
 * - 1.00 17-02-28  deo, first implementation
 * \endinternal
 */

#ifndef __FS_RAW_FS_H /* { */
#define __FS_RAW_FS_H

/**
 * \addtogroup grp_raw_fs
 * \copydoc raw_fs.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


#include "io/aw_io_device.h"
#include "fs/aw_blk_dev.h"
#include "aw_sem.h"

/** \brief raw file system volume descriptor */
struct aw_raw_fs_volume {
    struct aw_iodev      iodev;     /**< \brief I/O system device instance */
    struct aw_blk_dev   *p_bd;      /**< \brief backing block device */
    uint_t               bsize;     /**< \brief blocks size */
    uint_t               nblks;
};




/** \brief raw device file descriptor */
struct aw_raw_fs_file {
    struct aw_raw_fs_volume *p_vol;     /**< \brief pointer to volume descriptor */
    int                      oflags;    /**< \brief flags */
    uint_t                   offset;    /**< \brief file offset for read/write */
    AW_MUTEX_DECL(           lock);     /**< \brief mutex lock of the file */
    AW_SEMB_DECL(            semb);
    aw_semb_id_t             semb_id;
    void                    *p_buf;
};




/** \brief initialize ram file system */
int aw_raw_fs_init (void         *p_vol_mem,
                    unsigned int  vmem_size,
                    void         *p_fil_mem,
                    unsigned int  fmem_size);



#ifdef __cplusplus
}
#endif

/** @} grp_raw_fs */

#endif /* } __FS_RAW_FS_H */

/* end of file */
