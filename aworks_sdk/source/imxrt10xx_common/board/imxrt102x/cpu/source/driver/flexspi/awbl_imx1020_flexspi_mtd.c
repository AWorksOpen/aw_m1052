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
 * \brief Flexspi NOR Flash MTD 接口适配
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-31  mex, first implementation
 * \endinternal
 */

#include "driver/flexspi/awbl_imx1020_flexspi_mtd.h"

#define __FLEXSPI_FLASH_DEVINFO_DECL(p_info, p_nor_dev) \
        awbl_imx1020_flexspi_devinfo_t *p_info = \
        (awbl_imx1020_flexspi_devinfo_t *)AWBL_DEVINFO_GET(p_nor_dev)

aw_local int __mtd_erase (struct aw_mtd_info        *p_mtd,
                          struct aw_mtd_erase_info  *instr);

aw_local int __mtd_read (struct aw_mtd_info  *p_mtd,
                         off_t                from,
                         size_t               len,
                         size_t              *retlen,
                         void                *p_buf);

aw_local int __mtd_write (struct aw_mtd_info   *p_mtd,
                          off_t                 to,
                          size_t                len,
                          size_t               *retlen,
                          const void           *p_buf);


aw_local const struct aw_mtd_ops __g_mtd_ops = {
    __mtd_erase,                /* mtd_erase */
    NULL,                       /* mtd_sync */
    __mtd_read,                 /* mtd_read */
    __mtd_write,                /* mtd_write */
    NULL,                       /* mtd_panic_write */
    NULL,                       /* mtd_oob_read */
    NULL,                       /* mtd_oob_write */
    NULL,                       /* mtd_block_is_bad */
    NULL,                       /* mtd_block_mark_bad */
    NULL,                       /* mtd_is_locked */
    NULL,                       /* mtd_lock */
    NULL,                       /* mtd_unlock */
};


aw_local int __check_offs_len (struct awbl_flexspi_flash_mtd *p_fmtd,
                               off_t                          offs,
                               size_t                         len)
{
    /* Start address must align on block boundary */
    if (offs & p_fmtd->erase_align_mask) {
        return -AW_EINVAL;
    }

    /* Length must align on block boundary */
    if (len & p_fmtd->erase_align_mask) {
        return -AW_EINVAL;
    }

    /* Do not allow past end of device */
    if (offs + len > p_fmtd->mtd.size) {
        return -AW_EINVAL;
    }

    return AW_OK;
}

aw_local int __mtd_erase (struct aw_mtd_info        *p_mtd,
                          struct aw_mtd_erase_info  *instr)
{
    struct awbl_flexspi_flash_mtd  *p_fmtd = NULL;
    awbl_imx1020_flexspi_devinfo_t *p_flash_devinfo = NULL;

    aw_err_t ret;
    size_t   len;
    int      addr;

    p_fmtd  = AW_CONTAINER_OF(p_mtd, struct awbl_flexspi_flash_mtd, mtd);
    p_flash_devinfo = (awbl_imx1020_flexspi_devinfo_t *)AWBL_DEVINFO_GET(&p_fmtd->p_flash->super);

    if (p_mtd == NULL || instr == NULL) {
        return -AW_EINVAL;
    }

    if (__check_offs_len(p_fmtd, instr->addr, instr->len)) {
        return -AW_EINVAL;
    }

    instr->fail_addr = AW_MTD_ERASE_FAIL_ADDR_UNKNOWN;
    instr->state     = AW_MTD_ERASE_PROCESSING;

    addr =  (int)(instr->addr);
    len  = instr->len;

    while (len) {

        /* 擦除扇区 */
        ret = imx1020_nor_flash_sector_erase(addr);

        /* See if block erase succeeded */
        if (ret != AW_OK) {
            instr->state     = AW_MTD_ERASE_FAILED;
            instr->fail_addr = (off_t)(addr - p_fmtd->addr_offset);
            goto __exit;
        }

        /* Increment page address and decrement length */
        len  -= p_flash_devinfo->p_cfg->block_size;
        addr += p_flash_devinfo->p_cfg->block_size;
    }

    instr->state = AW_MTD_ERASE_DONE;
    aw_mtd_erase_callback(instr);

__exit:
    ret = (instr->state == AW_MTD_ERASE_DONE) ? 0 : -AW_EIO;
    return ret;
}


aw_local int __mtd_read (struct aw_mtd_info  *p_mtd,
                         off_t                from,
                         size_t               len,
                         size_t              *retlen,
                         void                *p_buf)
{
    struct awbl_flexspi_flash_mtd *p_fmtd;
    aw_err_t                       ret;

    if (p_mtd == NULL || p_buf == NULL || len == 0) {
        return -AW_EINVAL;
    }

    p_fmtd  = AW_CONTAINER_OF(p_mtd, struct awbl_flexspi_flash_mtd, mtd);

    /* Do not allow reads past end of device */
    if ((from + len) > p_mtd->size) {
        return -AW_EINVAL;
    }

    if (!len) {
        return AW_OK;
    }

    /* 读取数据 */
    ret = imx1020_nor_flash_read(p_buf, from, len);
    if ((ret == AW_OK) && (retlen != NULL)) {
        *retlen = len;
    }

    return ret;
}


aw_local int __mtd_write (struct aw_mtd_info   *p_mtd,
                          off_t                 to,
                          size_t                len,
                          size_t               *retlen,
                          const void           *p_buf)
{
    struct awbl_flexspi_flash_mtd  *p_fmtd;
    aw_err_t                        ret;
    int                             addr;
    int                             offs;
    int                             length = len;
    uint32_t                        bytes;
    uint8_t                        *ptr = (uint8_t *)p_buf;

    if (p_mtd == NULL || p_buf == NULL || len == 0) {
        return -AW_EINVAL;
    }

    p_fmtd  = AW_CONTAINER_OF(p_mtd, struct awbl_flexspi_flash_mtd, mtd);

    /* Do not allow write past end of device */
    if ((to + len) > p_mtd->size) {
        return -AW_EINVAL;
    }
    if (!length) {
        return AW_OK;
    }

    addr = (int)(to);
    offs = (int)(addr & (p_mtd->write_size - 1));

    while (length) {
        bytes = (uint32_t)min(p_mtd->write_size - offs, length);
        ret = imx1020_nor_flash_page_program(ptr, addr, bytes);
        if (ret != AW_OK) {
            goto __exit;
        }

        ptr    += bytes;
        length -= bytes;
        offs    = 0;
        addr   += bytes;
    }

__exit:
    if ((ret == AW_OK) && (retlen != NULL)) {
        *retlen = len - length;
    }

    return ret;
}


aw_err_t awbl_flexspi_flash_mtd_init (struct awbl_imx1020_flexspi_dev *p_flash,
                                      void                            *p_obj,
                                      const void                      *p_info)
{

    __FLEXSPI_FLASH_DEVINFO_DECL(p_flash_devinfo, &p_flash->super);
    struct awbl_flexspi_flash_mtd               *p_fmtd  = NULL;
    const struct awbl_flexspi_flash_mtd_info    *p_finfo = NULL;

    aw_err_t                       ret;

    p_fmtd  = (struct awbl_flexspi_flash_mtd *)p_obj;
    p_finfo = (struct awbl_flexspi_flash_mtd_info *)p_info;

    p_fmtd->p_info  = p_finfo;
    p_fmtd->p_flash = p_flash;

    /* MTD device */
    p_fmtd->mtd.size              = p_finfo->nblks * p_flash_devinfo->p_cfg->block_size;
    p_fmtd->mtd.type              = AW_MTD_TYPE_NOR_FLASH;
    p_fmtd->mtd.flags             = AW_MTD_FLAGS_NOR_FLASH;
    p_fmtd->mtd.erase_size        = p_flash_devinfo->p_cfg->block_size;
    p_fmtd->mtd.write_size        = p_flash_devinfo->p_cfg->page_size;
    p_fmtd->mtd.write_buf_size    = 0;
    p_fmtd->mtd.oob_size          = 0;
    p_fmtd->mtd.oob_avail         = 0;
    p_fmtd->mtd.bitflip_threshold = 0;
    p_fmtd->mtd.ecc_strength      = 0;
    p_fmtd->mtd.oob_layout        = NULL;
    p_fmtd->mtd.ops               = &__g_mtd_ops;

    p_fmtd->erase_align_mask = (~0) % p_flash_devinfo->p_cfg->block_size;
    p_fmtd->addr_offset      = p_finfo->start_blk * p_flash_devinfo->p_cfg->block_size;

    ret = aw_mtd_dev_add(&p_fmtd->mtd, p_finfo->name, NULL);
    if (ret != AW_OK) {
        aw_kprintf("adding MTD device failed %d.\n", ret);
    }

    return ret;
}

/* end of file */
