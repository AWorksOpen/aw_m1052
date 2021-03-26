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
 * \brief SATA disk subclass head file
 *
 * define SATA disk subclass data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 17-08-07  anu, first implementation
 * \endinternal
 */


#ifndef __AW_SATA_DISK_SUBCLASS_H
#define __AW_SATA_DISK_SUBCLASS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/* ¿˚”√ATA√¸¡ÓºØ */
#include "driver/ata/awbl_ata.h"
#include "driver/disk/awbl_sata_disk.h"

aw_bool_t aw_sata_disk_ata_device_diagnostic (
        struct awbl_sata_disk_dev *p_this);

aw_err_t aw_sata_disk_ata_identify_device_get (
        struct awbl_sata_disk_dev *p_this,
        awbl_ata_identify_data_t  *p_identify_data);

aw_err_t aw_sata_disk_ata_disk_identify_get (
                struct awbl_sata_disk_dev *p_this,
                awbl_sata_disk_identify_t *p_disk_identify);

aw_err_t aw_sata_disk_ata_read (struct awbl_sata_disk_dev *p_this,
                                uint32_t                   block_addr,
                                uint8_t                   *p_buf,
                                uint32_t                   len);

aw_err_t aw_sata_disk_ata_write (struct awbl_sata_disk_dev *p_this,
                                 uint32_t                   block_addr,
                                 uint8_t                   *p_buf,
                                 uint32_t                   len);

aw_err_t aw_sata_disk_ata_pwr_set (struct awbl_sata_disk_dev *p_this,
                                   awbl_ata_pwr_type_t        type,
                                   awbl_ata_pwr_time_t        time);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_SATA_DISK_SUBCLASS_H */

