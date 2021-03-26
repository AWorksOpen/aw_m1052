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
 * \brief SATA cdrom subclass head file
 *
 * define SATA cdrom subclass data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 17-08-07  anu, first implementation
 * \endinternal
 */


#ifndef __AW_SATA_CDROM_SUBCLASS_H
#define __AW_SATA_CDROM_SUBCLASS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/* ¿˚”√ATA√¸¡ÓºØ */
#include "driver/ata/awbl_ata.h"
#include "driver/cdrom/awbl_cdrom.h"
#include "driver/cdrom/awbl_sata_cdrom.h"

aw_err_t aw_sata_cdrom_atapi_pwr_set (struct awbl_sata_cdrom_dev *p_this,
                                      awbl_ata_pwr_type_t         type,
                                      awbl_ata_pwr_time_t         time);

aw_bool_t aw_sata_cdrom_atapi_device_diagnostic (
        struct awbl_sata_cdrom_dev *p_this);

aw_err_t aw_sata_cdrom_atapi_identify_device_get (
        struct awbl_sata_cdrom_dev *p_this,
        awbl_ata_identify_data_t   *p_identify_data);

aw_err_t aw_sata_cdrom_atapi_cdrom_identify_get (
                struct awbl_sata_cdrom_dev *p_this,
                awbl_sata_cdrom_identify_t *p_disk_identify);

aw_err_t aw_sata_cdrom_atapi_sense_request (
        struct awbl_sata_cdrom_dev *p_this,
        awbl_cdrom_sense_request_t *p_sense);

aw_err_t aw_sata_cdrom_atapi_read (struct awbl_sata_cdrom_dev *p_this,
                                   uint32_t                    block_addr,
                                   uint8_t                    *p_buf,
                                   uint32_t                    len);

aw_err_t aw_sata_cdrom_atapi_write (struct awbl_sata_cdrom_dev *p_this,
                                    uint32_t                    block_addr,
                                    uint8_t                    *p_buf,
                                    uint32_t                    len);

aw_err_t aw_sata_cdrom_atapi_eject (
        struct awbl_sata_cdrom_dev *p_this, aw_bool_t is_eject);

aw_err_t aw_sata_cdrom_atapi_door_lock (
        struct awbl_sata_cdrom_dev *p_this, aw_bool_t is_lock);

aw_err_t aw_sata_cdrom_atapi_disk_capacity_read (
        struct awbl_sata_cdrom_dev *p_this,
        uint32_t                   *p_lba,
        uint32_t                   *p_blk_size,
        uint32_t                   *p_sectors_per_frame);

aw_err_t aw_sata_cdrom_atapi_speed_set (
        struct awbl_sata_cdrom_dev *p_this,
        awbl_cdrom_data_rates_t     read_speed_khz,
        awbl_cdrom_data_rates_t     write_speed_khz);

aw_err_t awbl_sata_cdrom_mode_sense (
        struct awbl_sata_cdrom_dev  *p_this,
        awbl_cdrom_mode_page_code_t  page_code,
        uint32_t                     page_control,
        uint8_t                     *p_buf,
        uint32_t                     buf_len);

aw_err_t awbl_sata_cdrom_mode_select (
        struct awbl_sata_cdrom_dev *p_this,
        uint32_t                    page_code,
        uint32_t                    page_control,
        uint8_t                    *p_buf,
        uint32_t                    buf_len);

aw_err_t aw_sata_cdrom_atapi_capabilities_and_status_read (
        struct awbl_sata_cdrom_dev           *p_this,
        awbl_cdrom_capabilities_and_status_t *p_cap_status);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_SATA_CDROM_SUBCLASS_H */

/* end of file*/
