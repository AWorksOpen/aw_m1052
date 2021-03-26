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
 * \brief SPI-Flash MTD ½Ó¿Ú
 *
 * \internal
 * \par modification history:
 * - 1.00 17-08-31  deo, first implementation
 * \endinternal
 */


/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "aw_vdebug.h"
#include "mtd/aw_mtd.h"

#include "driver/norflash/awbl_spi_flash.h"
#include "driver/norflash/awbl_spi_flash_mtd.h"



#define __SMTD_TRACE(info)  AW_WARNF(info)

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



/******************************************************************************/
aw_local const struct aw_mtd_ops __g_mtd_ops = {
    __mtd_erase,                /**< \brief mtd_erase */
    NULL,                       /**< \brief mtd_sync */
    __mtd_read,                 /**< \brief mtd_read */
    __mtd_write,                /**< \brief mtd_write */
    NULL,                       /**< \brief mtd_panic_write */
    NULL,                       /**< \brief mtd_oob_read */
    NULL,                       /**< \brief mtd_oob_write */
    NULL,                       /**< \brief mtd_block_is_bad */
    NULL,                       /**< \brief mtd_block_mark_bad */
    NULL,                       /**< \brief mtd_is_locked */
    NULL,                       /**< \brief mtd_lock */
    NULL,                       /**< \brief mtd_unlock */
};




/******************************************************************************/
aw_local int __check_offs_len (struct awbl_spi_flash_mtd *p_fmtd,
                               off_t                      offs,
                               size_t                     len)
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




/******************************************************************************/
aw_local int __mtd_erase (struct aw_mtd_info        *p_mtd,
                          struct aw_mtd_erase_info  *instr)
{
    struct awbl_spi_flash_mtd  *p_fmtd;
    aw_err_t                    ret;
    size_t                      len;
    int                         addr;

    p_fmtd  = AW_CONTAINER_OF(p_mtd, struct awbl_spi_flash_mtd, mtd);

    if (p_mtd == NULL || instr == NULL) {
        return -AW_EINVAL;
    }

    if (__check_offs_len(p_fmtd, instr->addr, instr->len)) {
        return -AW_EINVAL;
    }

    instr->fail_addr = AW_MTD_ERASE_FAIL_ADDR_UNKNOWN;
    instr->state     = AW_MTD_ERASE_PROCESSING;

    /* get the start address */
    addr =  (int)(instr->addr + p_fmtd->addr_offset);

    len = instr->len;

    while (len) {

        ret = awbl_spi_flash_erase(p_fmtd->p_flash, addr);
        /* See if block erase succeeded */
        if (ret != AW_OK) {
            instr->state     = AW_MTD_ERASE_FAILED;
            instr->fail_addr = (off_t)(addr - p_fmtd->addr_offset);
            goto __exit;
        }

        /* Increment page address and decrement length */
        len  -= p_fmtd->p_flash->p_info->block_size;
        addr += p_fmtd->p_flash->p_info->block_size;
    }

    instr->state = AW_MTD_ERASE_DONE;
    aw_mtd_erase_callback(instr);

__exit:
    ret = (instr->state == AW_MTD_ERASE_DONE) ? 0 : -AW_EIO;
    return ret;
}




/******************************************************************************/
aw_local int __mtd_read (struct aw_mtd_info  *p_mtd,
                         off_t                from,
                         size_t               len,
                         size_t              *retlen,
                         void                *p_buf)
{
    struct awbl_spi_flash_mtd  *p_fmtd;
    aw_err_t                    ret;
    int                         addr;
    uint32_t                    rlen;

    if (p_mtd == NULL || p_buf == NULL || len == 0) {
        return -AW_EINVAL;
    }

    p_fmtd  = AW_CONTAINER_OF(p_mtd, struct awbl_spi_flash_mtd, mtd);

    /* Do not allow reads past end of device */
    if ((from + len) > p_mtd->size) {
        return -AW_EINVAL;
    }

    if (!len) {
        return AW_OK;
    }

    addr = from + p_fmtd->addr_offset;

    ret = awbl_spi_flash_read(p_fmtd->p_flash, addr, p_buf, len, &rlen);
    if ((ret == AW_OK) && (retlen)) {
        *retlen = rlen;
    }

    return ret;
}




/******************************************************************************/
aw_local int __mtd_write (struct aw_mtd_info   *p_mtd,
                          off_t                 to,
                          size_t                len,
                          size_t               *retlen,
                          const void           *p_buf)
{
    struct awbl_spi_flash_mtd  *p_fmtd;
    aw_err_t                    ret;
    int                         addr;
    int                         offs;
    int                         length = len;
    uint32_t                    bytes;
    uint8_t                    *ptr = (uint8_t *)p_buf;

    if (p_mtd == NULL || p_buf == NULL || len == 0) {
        return -AW_EINVAL;
    }

    p_fmtd  = AW_CONTAINER_OF(p_mtd, struct awbl_spi_flash_mtd, mtd);

    /* Do not allow write past end of device */
    if ((to + len) > p_mtd->size) {
        return -AW_EINVAL;
    }
    if (!length) {
        return AW_OK;
    }

    /* get the start address */
    addr = (int)(to + p_fmtd->addr_offset);
    offs = (int)(addr & (p_mtd->write_size - 1));

    while (length) {
        bytes = (uint32_t)min(p_mtd->write_size - offs, length);

        ret = awbl_spi_flash_write(p_fmtd->p_flash, addr, ptr, bytes, &bytes);
        if (ret != AW_OK) {
            goto __exit;
        }

        ptr    += bytes;
        length -= bytes;
        offs    = 0;
        addr   += bytes;
    }

__exit:
    if ((ret == AW_OK) && (retlen)) {
        *retlen = len - length;
    }

    return ret;
}




/******************************************************************************/
aw_err_t awbl_spi_flash_mtd_init (struct awbl_spi_flash_dev *p_flash,
                                  void                      *p_obj,
                                  const void                *p_info)
{
    struct awbl_spi_flash_mtd            *p_fmtd;
    const struct awbl_spi_flash_mtd_info *p_finfo;
    aw_err_t                              ret;

    p_fmtd  = (struct awbl_spi_flash_mtd *)p_obj;
    p_finfo = (struct awbl_spi_flash_mtd_info *)p_info;

    p_fmtd->p_info  = p_finfo;
    p_fmtd->p_flash = p_flash;

    /* MTD device */
    p_fmtd->mtd.size              = p_finfo->nblks * p_flash->p_info->block_size;
    p_fmtd->mtd.type              = AW_MTD_TYPE_NOR_FLASH;
    p_fmtd->mtd.flags             = AW_MTD_FLAGS_NOR_FLASH;
    p_fmtd->mtd.erase_size        = p_flash->p_info->block_size;
    p_fmtd->mtd.write_size        = p_flash->p_info->page_size;
    p_fmtd->mtd.write_buf_size    = 0;
    p_fmtd->mtd.oob_size          = 0;
    p_fmtd->mtd.oob_avail         = 0;
    p_fmtd->mtd.bitflip_threshold = 0;
    p_fmtd->mtd.ecc_strength      = 0;
    p_fmtd->mtd.oob_layout        = NULL;
    p_fmtd->mtd.ops               = &__g_mtd_ops;

    p_fmtd->erase_align_mask = (~0) % p_flash->p_info->block_size;
    p_fmtd->addr_offset      = p_finfo->start_blk * p_flash->p_info->block_size;

    ret = aw_mtd_dev_add(&p_fmtd->mtd,
                         p_flash->p_info->name,
                         NULL);
    if (ret != AW_OK) {
        __SMTD_TRACE(("adding MTD device failed %d.\n", ret));
    }

    return ret;
}

/* end of file */
