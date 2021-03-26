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
 * \brief SATA cdrom driver head file
 *
 * define SATA cdrom driver data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 17-07-28  anu, first implementation
 * \endinternal
 */


#ifndef __AWBL_SATA_CDROM_H
#define __AWBL_SATA_CDROM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "fs/aw_blk_dev.h"
#include "awbl_satabus.h"
#include "aw_sem.h"
#include "aw_delayed_work.h"

struct awbl_sata_cdrom_dev;

#define AWBL_SATA_CDROM_NAME    "awbl_sata_cdrom_driver"

typedef uint32_t awbl_sata_cdrom_flag_t;
#define AWBL_SATA_CDROM_FLAG_LBA            (0X0001) /**< \brief ���豸֧��LBAѰַ */
#define AWBL_SATA_CDROM_FLAG_LBA48          (0X0002) /**< \brief ���豸֧��LBA48Ѱַ */
#define AWBL_SATA_CDROM_FLAG_NCQ            (0X0004) /**< \brief ���豸֧��NCQ */
#define AWBL_SATA_CDROM_FLAG_DMA            (0X0008) /**< \brief ���豸֧��DMA */

/** \brief  ���̱�ʶ��Ϣ */
typedef struct awbl_sata_cdrom_identify {

    uint8_t  serial[20 + 1];        /**< \brief serial number */
    uint8_t  fw_rev[8 + 1];         /**< \brief firmware revision */
    uint8_t  model_num[40 + 1];     /**< \brief model number */

    uint32_t lba;                   /**< \brief �߼����ַ */
    uint32_t blk_size;              /**< \brief ��Ĵ�С Byte */
    uint32_t sectors_per_frame;     /**< \brief ÿ֡��ռ������ */
    uint32_t cdrom_capacity;        /**< \brief �������� MByte */

    awbl_sata_cdrom_flag_t flag;    /**< \brief ���豸֧�ֵĹ��� */

} awbl_sata_cdrom_identify_t;

/** \brief ���̲������������� */
typedef struct awbl_sata_cdrom_drv_funs {

    aw_err_t (*pfn_cdrom_type_probe) (
            struct awbl_sata_cdrom_dev *p_this);

    aw_err_t (*pfn_cdrom_identify_get) (
            struct awbl_sata_cdrom_dev *p_this,
            awbl_sata_cdrom_identify_t *p_identify_data);

    aw_err_t (*pfn_read) (struct awbl_sata_cdrom_dev *p_this,
                          uint32_t                   block_addr,
                          uint8_t                   *p_buf,
                          uint32_t                   len);

    aw_err_t (*pfn_write) (struct awbl_sata_cdrom_dev *p_this,
                           uint32_t                   block_addr,
                           uint8_t                   *p_buf,
                           uint32_t                   len);

      aw_err_t (*pfn_cdrom_atapi_capacity_read) (
              struct awbl_sata_cdrom_dev *p_this,
              uint32_t                   *p_lba,
              uint32_t                   *p_blk_size,
              uint32_t                   *p_sectors_per_frame);

} awbl_sata_cdrom_drv_funs_t;

/** \brief cdrom sata struct */
struct awbl_sata_cdrom_dev {

    struct awbl_satabus_dev *p_sata_dev;
    struct aw_blk_dev        bd;             /**< \brief block device */
    
    struct aw_delayed_work   dwork_detect;   /**< \brief period works for slot */

    awbl_sata_cdrom_drv_funs_t *p_drv_funs;
    awbl_sata_cdrom_identify_t  identify_data;

    aw_bool_t   is_found_disk; /**< \brief �Ƿ��⵽���� */
    aw_bool_t   is_removed;    /**< \brief �Ƿ��Ƴ� */

    AW_MUTEX_DECL(mutex);

};


/**
 * \brief cdrom sata register
 */
void awbl_sata_cdrom_drv_register (void);

struct awbl_sata_cdrom_dev *aw_sata_cdrom_dev_get (const char *p_name);

aw_err_t aw_sata_cdrom_dev_put (struct awbl_sata_cdrom_dev *p_this);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_SATA_CDROM_H */

