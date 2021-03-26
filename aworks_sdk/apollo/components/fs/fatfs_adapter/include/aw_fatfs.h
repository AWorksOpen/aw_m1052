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
 * \brief FatFs AWorks adapter layer
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-07  deo, first implementation
 * \endinternal
 */

#ifndef __AW_FATFS_H
#define __AW_FATFS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_fat_ios
 * @{
 */
#include "aw_sem.h"
#include "io/aw_io_device.h"
#include "fs/aw_blk_dev.h"
#include "ff.h"



/** \brief FatFs volume descriptor */
struct aw_fatfs_volume {
    struct aw_iodev      io_dev;    /**< \brief base on I/O device to AWorks */
    struct aw_blk_dev   *blk_dev;   /**< \brief mount to block device */
    FATFS                ff_volume; /**< \brief base on FatFs volume */
    int                  disk_idx;  /**< \brief disk index */
};




/** \brief FatFs device file descriptor */
struct aw_fatfs_file {
    struct aw_fatfs_volume *volume;   /**< \brief pointer to volume descriptor */
    int                     oflags;   /**< \brief flags */
    union {
        FIL                 fil;      /**< \brief the FAT file descriptor */
        DIR                 dir;      /**< \brief the directory descriptor */
    } u;
    char                    path[FF_MAX_LFN + 1];
};




/**
 * \brief FAT FS initializing data
 * \note (bio_pool_size / bio_buf_size) should be equal to
 *       (vol_pool_size / sizeof(struct fat_fs_volume))
 */
struct aw_fatfs_init_struct {
    struct aw_fatfs_volume *vols;     /**< \brief volume memory */
    unsigned int            nvol;     /**< \brief n volumes */
    struct aw_fatfs_file   *fils;     /**< \brief file memory */
    unsigned int            nfil;     /**< \brief n files */
};



/**
 * \brief AWorks FatFs initialize
 *
 * \param[in]  p_vol_mem    : memory for volumes
 * \param[in]  vmem_size    : size of p_vol_mem
 * \param[in]  p_fil_mem    : memory for files
 * \param[in]  fmem_size    : size of p_fil_mem
 */
int aw_fatfs_init (void         *p_vol_mem,
                   unsigned int  vmem_size,
                   void         *p_fil_mem,
                   unsigned int  fmem_size);

int aw_fatfs_disk_lib_init (void);
int aw_fatfs_disk_num_get (struct aw_blk_dev *p_bd);
int aw_fatfs_disk_num_put (int disk);

time_t aw_fattime_to_time (DWORD   fattime);
DWORD  aw_time_to_fattime (time_t *t);


#ifdef __cplusplus
}
#endif


#endif /* __AW_FATFS_H */

/* end of file */

