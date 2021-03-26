/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

#ifndef USER_CODE_AW_LITTLEFS_H_
#define USER_CODE_AW_LITTLEFS_H_

#include "AWorks.h"
#include "lfs.h"
#include "io/aw_io_device.h"

struct aw_littlefs_volume {
    struct aw_iodev     io_dev;     /**< \brief base on I/O device to AWorks */
    void               *blk_dev;    /**< \brief backing MTD device */

    lfs_t               lfs;
    struct lfs_config   config;

    char                mpoint[16];
};

struct aw_littlefs_file {
    struct aw_littlefs_volume *volume;   /**< \brief pointer to volume descriptor */
    int                        flags;    /**< \brief flags */

    lfs_file_t                 fil;
    lfs_dir_t                  dir;

    char path[LFS_NAME_MAX];
};

struct aw_littlefs_init_struct {

};

void aw_littlefs_mtd_init(struct aw_littlefs_volume *p_vol);
void aw_littlefs_mtd_destory(struct aw_littlefs_volume *p_vol);
void aw_littlefs_disk_init(struct aw_littlefs_volume *p_vol);
void aw_littlefs_disk_destory(struct aw_littlefs_volume *p_vol);

aw_err_t aw_littlefs_init(void         *p_vol_mem,
                          unsigned int  vmem_size,
                          void         *p_fil_mem,
                          unsigned int  fmem_size);

#endif /* USER_CODE_AW_LITTLEFS_H_ */
