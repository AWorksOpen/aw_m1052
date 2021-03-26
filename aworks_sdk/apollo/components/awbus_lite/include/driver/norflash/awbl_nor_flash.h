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
 * \brief Ethernet AX88796 Driver.
 *
 * \internal
 * \par modification history:
 * - 1.00 15-5-20, ehf, first implementation.
 * \endinternal
 */

#ifndef AWBL_NOR_FLASH_H_
#define AWBL_NOR_FLASH_H_

#include "aw_spi.h"
#include "awbl_nvram.h"
#include "aw_sem.h"

#define AWBL_NOR_FLASH_NAME   "awbl_nor_flash"

typedef struct awbl_nor_flash_devinfo{
    aw_const struct awbl_nvram_segment *p_seg_list;
    uint32_t   seg_count;

    uint32_t   base_addr;              /**< \brief norflash在系统中起始地址 */
    uint8_t    byte_mode;              /**< \brief 数据宽度 8/16*/
    uint32_t  sector_size;             /**< \brief 扇区大小 */
    uint32_t  sector_cnt;              /**< \brief 扇区数量 */
    uint32_t  reserved_sectors;        /**< \brief 保留扇区数 */

    const char *name;
    uint8_t    *p_bulk_buff;
    void (*pfunc_plfm_init)(void);
}awbl_nor_flash_devinfo_t;

/**
 * \brief nor flash 设备实例
 */
typedef struct awbl_nor_flash_dev {
    struct awbl_dev nor_dev;
    AW_MUTEX_DECL(devlock);             /**< \brief 设备锁 */
} awbl_nor_flash_dev_t;

/**
 * \brief nor flash driver register
 */
void awbl_nor_flash_drv_register (void);

#endif /* AWBL_NOR_FLASH_H_ */

/* end of file */


