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
 * \brief RAM disk implementation with block device interface.
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-03  deo, first implementation
 * \endinternal
 */

#ifndef __AW_RAM_DISK_H /* { */
#define __AW_RAM_DISK_H

#include "fs/aw_blk_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief RAM disk object */
struct aw_ram_disk {
    struct aw_blk_dev  bd;    /**< \brief block device instance */
    uint8_t           *p_mem; /**< \brief memory for ram disk */
    const char        *name;
    uint32_t           bsize; /**< \brief block size */
    uint32_t           nblks; /**< \brief number of blocks */
    void             (*pfn_release) (struct aw_ram_disk *p_disk);
};




/** \brief create a ram disk */
int aw_ram_disk_create (struct aw_ram_disk *p_disk,
                        const char         *name,
                        uint_t              bsize,
                        void               *p_mem,
                        uint_t              size,
                        void (*pfn_release) (struct aw_ram_disk *p_disk));




/** \brief destroy a ram disk */
int aw_ram_disk_destroy (struct aw_ram_disk *p_disk);




#ifdef __cplusplus
}
#endif

/** @} grp_awbl_ram_disk */

#endif /* } __AW_RAM_DISK_H */

/* end of file */
