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
 * \brief RAM simulated MTD device.
 *
 * \internal
 * \par modification history:
 * - 150310 orz, first implementation.
 * \endinternal
 */

#ifndef __AW_MTD_RAM_H /* { */
#define __AW_MTD_RAM_H

#include "apollo.h"
#include "mtd/aw_mtd.h"

/**
 * \addtogroup grp_aw_mtd
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

/** \brief MTD RAM device structure */
struct aw_mtd_ram {
    struct aw_mtd_info             mtd;        /**< \brief master MTD instance */
    void                          *mem;        /**< \brief memory of MTD dev */
    unsigned int                   parts;      /**< \brief number of partitions */
    struct aw_mtd_part_info       *part_info;  /**< \brief partition instance */
    const struct aw_mtd_partition *partitions; /**< \brief partitions */
};

/******************************************************************************/
aw_err_t aw_mtd_ram_add (struct aw_mtd_ram *ram,
                         const char        *name,
                         size_t             size,
                         unsigned int       erase_size);

aw_err_t aw_mtd_ram_del (struct aw_mtd_ram *ram);


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* } __AW_MTD_RAM_H */

/* end of file */
