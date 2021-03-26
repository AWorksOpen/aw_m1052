/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
**** ***************************************************************************/

/**
 * \file
 * \brief Flexspi NOR Flash NVRAM 接口适配
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-31  mex, first implementation
 * \endinternal
 */

#include "string.h"
#include "driver/flexspi/awbl_imx1020_flexspi_nvram.h"
#include "driver/flexspi/awbl_imx1020_flexspi.h"


#define __FLEXSPI_FLASH_DEVINFO_DECL(p_info, p_nor_dev) \
        awbl_imx1020_flexspi_devinfo_t *p_info = \
        (awbl_imx1020_flexspi_devinfo_t *)AWBL_DEVINFO_GET(p_nor_dev)


aw_err_t awbl_flexspi_nor_flash_nvram_get (struct awbl_dev  *p_dev,
                                           char             *p_name,
                                           int               unit,
                                           void             *p_buf,
                                           int               offset,
                                           int               len)
{
    __FLEXSPI_FLASH_DEVINFO_DECL(p_devinfo, p_dev);
    const struct awbl_nvram_segment *p_seg = p_devinfo->nvram_info.p_seglst_list;

    int      i       = 0;
    int      copylen = 0;
    aw_err_t ret     = AW_OK;
    uint32_t sector_size = p_devinfo->p_cfg->sector_size;
    uint32_t sector_cnt  = p_devinfo->p_cfg->flash_size / sector_size;

    for(i = 0; i < p_devinfo->nvram_info.seglst_count; i++) {
        if ( ((p_seg->unit) == unit) && (strcmp(p_seg->p_name, p_name) == 0) ) {
            copylen = len;
            if (offset + len > p_seg->seg_size) {
                copylen = p_seg->seg_size - offset;
                if (copylen < 0) {
                    return -AW_EIO;
                }
            }
            if (p_seg->seg_addr + offset + copylen >
                (sector_size * sector_cnt)) {
                return -AW_ENXIO;
            }

            /* 读取数据 */
            ret = imx1020_nor_flash_read((uint8_t *)p_buf, offset, copylen);
            return ret != AW_OK ? -AW_EIO : AW_OK;
        }
        p_seg++;
    }

    return -AW_ENXIO;
}


aw_err_t awbl_flexspi_nor_flash_nvram_set (struct awbl_dev *p_dev,
                                           char            *p_name,
                                           int              unit,
                                           char            *p_buf,
                                           int              offset,
                                           int              len)
{

    __FLEXSPI_FLASH_DEVINFO_DECL(p_devinfo, p_dev);
    const struct awbl_nvram_segment *p_seg = p_devinfo->nvram_info.p_seglst_list;

    uint32_t  i, j, n;
    int      copylen;
    aw_err_t ret;

    uint32_t  sector_addr_start = 0;
    uint32_t  sector_addr_end = 0;
    uint32_t  current_addr = 0;
    uint32_t  copy_size = 0;
    uint32_t  page_size   = p_devinfo->p_cfg->page_size;
    uint32_t  sector_size = p_devinfo->p_cfg->sector_size;
    uint32_t  sector_cnt  = p_devinfo->p_cfg->flash_size / sector_size;
    uint32_t  sector_page_cnt = sector_size / page_size;

    for (i = 0; i < p_devinfo->nvram_info.seglst_count; i++) {
        if ((p_seg->unit == unit) && (strcmp(p_seg->p_name, p_name) == 0)) {
            copylen = len;
            if (offset + len > p_seg->seg_size) {
                copylen = p_seg->seg_size - offset;
                if (copylen < 0) {
                    return -AW_EIO;
                }
            }
            if (p_seg->seg_addr + offset + copylen  >
                (sector_size * sector_cnt)) {
                return  -AW_ENXIO;
            }

            for (j = 0; j < copylen;) {
                /* 计算扇区地址范围 */
                current_addr = j + offset;
                sector_addr_start = current_addr / sector_size * sector_size;
                sector_addr_end = sector_addr_start + sector_size;

                /* 读取一扇区数据 */
                ret = imx1020_nor_flash_read((uint8_t *)(p_devinfo->nvram_info.p_blk_buf),
                                             sector_addr_start,
                                             sector_size);

                /* 合并数据 */
                copy_size = (copylen - j) < (sector_addr_end - current_addr)? \
                            (copylen - j): \
                            (sector_addr_end - current_addr);

                memcpy(&p_devinfo->nvram_info.p_blk_buf[current_addr % sector_size],
                       &p_buf[j], copy_size);

                j += copy_size;

                /* 擦除扇区 */
                imx1020_nor_flash_sector_erase(sector_addr_start);

                if (ret != 0) {
                    return AW_ERROR;
                }

                /* 将扇区的数据重新写入 */
                for (n = 0; n < sector_page_cnt; n++) {

                    ret = imx1020_nor_flash_page_program((uint8_t *)(p_devinfo->nvram_info.p_blk_buf + n * page_size),
                                                         sector_addr_start + n * page_size,
                                                         page_size);
                    if (ret != AW_OK) {
                        return  ret;
                    }
                }

            }
            return  AW_OK;
        }
        p_seg++;
    }

    return -AW_ENXIO;
}


/* end of file */
