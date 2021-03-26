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
 * \brief SPIFI标准接口头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-31  mex, first implementation
 * \endinternal
 */

//#include <awbl_imx1050_flexspi_nand_service.h>
#include "aworks.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_assert.h"

#include "awbl_nand.h"
#include "awbl_nand_bus.h"
#include "awbl_nand_pf.h"

#include "driver/flexspi_nand/awbl_imx1050_flexspi_nand_service.h"

static awbl_imx1050_flexspi_service_t __gp_flexspi_serv ;

AWBL_METHOD_IMPORT(awbl_imx1050_flexspi_serv_func_get);
AWBL_METHOD_DEF(awbl_imx1050_flexspi_serv_func_get,
                "awbl_imx1050_flexspi_serv_func_get");


/**
 * \brief allocate imx1050 flexspi service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __imx1050_flexspi_serv_helper ( struct awbl_dev *p_dev, void *p_arg)
{
    if( NULL != __gp_flexspi_serv.p_func ) {
        return AW_OK;
    }

    awbl_method_handler_t   pfn_imx1050_flexspi_get_serv = NULL;
    pfn_imx1050_flexspi_get_serv = awbl_dev_method_get(p_dev,
            AWBL_METHOD_CALL(awbl_imx1050_flexspi_serv_func_get));

    __gp_flexspi_serv.p_dev = (void *)p_dev;

    if(NULL != pfn_imx1050_flexspi_get_serv) {
        pfn_imx1050_flexspi_get_serv(p_dev, &(__gp_flexspi_serv.p_func));
    }
    return AW_OK;
}

/**
 * \brief Flexspi initial
 */
void awbl_imx1050_flexspi_nand_init (void)
{
    awbl_dev_iterate(__imx1050_flexspi_serv_helper,
                     NULL,
                     AWBL_ITERATE_INSTANCES);
}


/**
 * \brief Flexspi NAND Flash Read ID
 */
aw_err_t  awbl_flexspi_nand_flash_read_id (uint8_t   *p_buf,
                                           uint32_t   bytes)
{
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nand_flash_read_id(
                                                __gp_flexspi_serv.p_dev,
                                                p_buf,
                                                bytes);
}

/**
 * \brief Flexspi NAND Flash Read Status
 */
aw_err_t  awbl_flexspi_nand_flash_read_status (uint8_t    status_addr,
                                               uint8_t   *status)
{
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nand_flash_read_status(
                                                    __gp_flexspi_serv.p_dev,
                                                    status_addr,
                                                    status);
}


/**
 * \brief Flexspi NAND Flash Write Status
 */
aw_err_t  awbl_flexspi_nand_flash_write_status (uint8_t    status_addr,
                                                uint8_t   *status)
{
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nand_flash_write_status(
                                                    __gp_flexspi_serv.p_dev,
                                                    status_addr,
                                                    status);
}


/**
 * \brief Flexspi NAND Flash Write Enable
 */
aw_err_t  awbl_flexspi_nand_flash_write_enable (void)
{
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nand_flash_write_enable(
                                                        __gp_flexspi_serv.p_dev);
}


/**
 * \brief Flexspi NAND Flash Write Disenable
 */
aw_err_t  awbl_flexspi_nand_flash_write_disable (void)
{
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nand_flash_write_disable(
                                                        __gp_flexspi_serv.p_dev);
}


/**
 * \brief Flexspi NAND Flash Block Erase
 */
aw_err_t  awbl_flexspi_nand_flash_block_erase (uint32_t addr)
{
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nand_flash_block_erase(
                                                        __gp_flexspi_serv.p_dev,
                                                        addr);
}



/**
 * \brief Flexspi NAND Flash Page Write
 */
aw_err_t  awbl_flexspi_nand_flash_page_write (uint32_t   p_addr,
                                              uint32_t   offset,
                                              uint8_t   *buf,
                                              uint32_t   bytes)
{
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nand_flash_page_write(
                                                        __gp_flexspi_serv.p_dev,
                                                        p_addr,
                                                        offset,
                                                        buf,
                                                        bytes);
}


/**
 * \brief Flexspi NAND Flash Page Read
 */
aw_err_t  awbl_flexspi_nand_flash_page_read (uint32_t   p_addr,
                                             uint32_t   offset,
                                             uint8_t   *buf,
                                             uint32_t   bytes)
{
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nand_flash_page_read(
                                                        __gp_flexspi_serv.p_dev,
                                                        p_addr,
                                                        offset,
                                                        buf,
                                                        bytes);
}



/******************************************************************************/
/**
 * \brief Flexspi NAND Flash Write Buf
 */
aw_err_t __flexspi_nandflash_write_buf(awbl_nand_platform_serv_t *p_this,
                                       uint32_t                   page_addr,
                                       uint32_t                   col,
                                       uint8_t                   *buf,
                                       int                        len,
                                       uint8_t                   *oob_buf,
                                       uint16_t                   oob_buf_size,
                                       aw_bool_t                  oob_required)
{
    aw_err_t    ret = -1;
    uint32_t    i = 0;
    uint32_t    oob_len = 0;
    uint32_t    oob_offs = 0;
    uint32_t    oob_layout = p_this->p_info->p_nand_ooblayout->cnt_freepos;
    uint32_t    max_pages = p_this->p_info->attr.chip_size /
                                p_this->p_info->attr.page_size;
    uint32_t    max_col = p_this->p_info->attr.page_size + p_this->p_info->attr.spare_size;
    uint8_t     *page_data = (uint8_t *)p_this->p_info->p_platform_info;

    memset((void *)page_data, 0, max_col);

    if (page_addr >= max_pages) {
        aw_kprintf("nand write: page addr error, max_pages: %d, write_addr: %d.\r\n",
                    max_pages,
                    page_addr);
        return AW_ERROR;
    }

    if ( (col + len) > max_col) {
        aw_kprintf("nand write: out of page range. col_addr: %d, write_len: %d.\r\n",
                   col,
                   len);
        return AW_ERROR;
    }

    /* 本次只写数据 */
    if ( (col == 0) && (oob_required == AW_FALSE) ) {
        ret = awbl_flexspi_nand_flash_page_write (page_addr,
                                                  col,
                                                  buf,
                                                  len);

    /* 本次只写oob */
    } else if ( (col == p_this->p_info->attr.page_size) && (oob_required == AW_FALSE) ) {
        ret = awbl_flexspi_nand_flash_page_read (page_addr,
                                                 0,
                                                 page_data,
                                                 max_col);
        if (ret != AW_OK) {
            return ret;
        }

        for (i = 0; i < oob_layout; i++) {
            oob_offs = p_this->p_info->p_nand_ooblayout->ptr_freepos[i].offset +
                           p_this->p_info->attr.page_size;
            oob_len = p_this->p_info->p_nand_ooblayout->ptr_freepos[i].length;
            memcpy((void *)(page_data + oob_offs),
                   (void *)(buf + p_this->p_info->p_nand_ooblayout->ptr_freepos[i].offset),
                   oob_len);
        }

        ret = awbl_flexspi_nand_flash_page_write (page_addr,
                                                  0,
                                                  page_data,
                                                  max_col);

    /* 本次数据和oob一起写 */
    } else if ( (col == 0) && (oob_required == AW_TRUE) ) {

        ret = awbl_flexspi_nand_flash_page_write (page_addr,
                                                  col,
                                                  buf,
                                                  len);

        ret = awbl_flexspi_nand_flash_page_read (page_addr,
                                                 0,
                                                 page_data,
                                                 max_col);
        if (ret != AW_OK) {
            return ret;
        }

        for (i = 0; i < oob_layout; i++) {
            oob_offs = p_this->p_info->p_nand_ooblayout->ptr_freepos[i].offset +
                           p_this->p_info->attr.page_size;
            oob_len = p_this->p_info->p_nand_ooblayout->ptr_freepos[i].length;
            memcpy((void *)(page_data + oob_offs),
                   (void *)(oob_buf + p_this->p_info->p_nand_ooblayout->ptr_freepos[i].offset),
                   oob_len);
        }

        ret = awbl_flexspi_nand_flash_page_write (page_addr,
                                                  0,
                                                  page_data,
                                                  max_col);
    }

    return ret;
}

aw_err_t __flexspi_nandflash_read_buf(awbl_nand_platform_serv_t *p_this,
                                      uint32_t                   page_addr,
                                      uint32_t                   col,
                                      uint8_t                   *buf,
                                      int                        len,
                                      uint8_t                   *oob_buf,
                                      uint16_t                   oob_buf_size,
                                      aw_bool_t                  oob_required)
{
    aw_err_t    ret = -1;
    uint32_t    max_pages = p_this->p_info->attr.chip_size /
                            p_this->p_info->attr.page_size;
    uint32_t    max_col = p_this->p_info->attr.page_size + p_this->p_info->attr.spare_size;
    uint8_t     *page_data = (uint8_t *)p_this->p_info->p_platform_info;

    memset((void *)page_data, 0, max_col);

    if (page_addr >= max_pages) {
        aw_kprintf("nand read: page addr error, max_pages: %d, read_addr: %d.\r\n",
                    max_pages,
                    page_addr);
        return AW_ERROR;
    }

    if ( (col + len) > max_col) {
        aw_kprintf("nand read: out of page range. col_addr: %d, read_len: %d.\r\n",
                   col,
                   len);
        return AW_ERROR;
    }

    ret = awbl_flexspi_nand_flash_page_read (page_addr,
                                             0,
                                             page_data,
                                             max_col);

    /* 如果本次是只读数据区域，将当前页中的数据复制到buf中 */
    if ( (col == 0) && (oob_required == AW_FALSE) ) {
        memcpy((void *)buf,
               (void *)page_data,
               len);

    /* 如果本次是只读oob区域，将当前页中的oob数据复制到buf中 */
    } else if ( (col == p_this->p_info->attr.page_size) && (oob_required == AW_FALSE) ) {
        memcpy((void *)buf,
               (void *)(page_data + p_this->p_info->attr.page_size),
               len);

    /* 如果本次是数据和oob一起读，则。。。buf中 */
    } else if ( (col == 0) && (oob_required == AW_TRUE) ) {
        memcpy((void *)buf,
               (void *)page_data,
               p_this->p_info->attr.page_size);
        memcpy((void *)oob_buf,
               (void *)(page_data + p_this->p_info->attr.page_size),
               oob_buf_size);
    }

    return ret;
}


/**
 * 是坏块的话返回：AW_TURE
 */
aw_bool_t __flexspi_nandflash_block_is_bad(awbl_nand_platform_serv_t  *p_this,
                                           uint32_t                    page_addr)
{
    aw_err_t        ret = -1;
    uint32_t        i = 0;
    uint32_t        max_col = p_this->p_info->attr.page_size + p_this->p_info->attr.spare_size;
    uint8_t         *page_data = (uint8_t *)p_this->p_info->p_platform_info;
    uint32_t        badblk_byte_offs = p_this->p_info->attr.page_size +
                                           p_this->p_info->attr.bad_blk_byte_offs;

    if ( (page_addr % p_this->p_info->attr.pages_per_blk) != 0) {
        aw_kprintf("nand: block is bad page_addr not a block_addr.\r\n");
        return AW_TRUE;
    }

    for (i = 0; i < 2; i++) {
        memset((void *)page_data, 0, max_col);
        ret = awbl_flexspi_nand_flash_page_read (page_addr + i,
                                                 0,
                                                 page_data,
                                                 max_col);
        if ((ret == AW_OK) && (*(page_data + badblk_byte_offs) != 0)) {
            return AW_FALSE;
        }
    }
    /* 当前块的第一页和第二页的保留区的第一个字节均为0，是坏块 */
    return AW_TRUE;
}


int __flexspi_nandflash_block_markbad(awbl_nand_platform_serv_t *p_this,
                                      uint32_t                   page_addr)
{
    aw_err_t        ret = -1;
    uint32_t        i = 0;
    uint32_t        max_col = p_this->p_info->attr.page_size + p_this->p_info->attr.spare_size;
    uint8_t         *page_data = (uint8_t *)p_this->p_info->p_platform_info;
    uint32_t        badblk_byte_offs = p_this->p_info->attr.page_size +
                                           p_this->p_info->attr.bad_blk_byte_offs;

    if ( (page_addr % p_this->p_info->attr.pages_per_blk) != 0) {
        aw_kprintf("nand: block is bad page_addr not a block_addr.\r\n");
        return AW_ERROR;
    }

    for (i = 0; i < 2; i++) {
        memset((void *)page_data, 0, max_col);
        ret = awbl_flexspi_nand_flash_page_read (page_addr + i,
                                                 0,
                                                 page_data,
                                                 max_col);
        if (ret != AW_OK) {
            return AW_ERROR;
        }
        /* 填写坏块标记的字节 */
        *(page_data + badblk_byte_offs) = 0;
        ret = awbl_flexspi_nand_flash_page_write (page_addr + i,
                                                  0,
                                                  page_data,
                                                  max_col);
        if (ret != AW_OK) {
            return AW_ERROR;
        }
    }

    return AW_OK;
}


int __flexspi_nandflash_erase (awbl_nand_platform_serv_t *p_this,
                               uint32_t                   page_addr)
{
    uint32_t    max_pages = p_this->p_info->attr.chip_size /
                               p_this->p_info->attr.page_size;

    if (page_addr > max_pages) {
        aw_kprintf("nand: erase page addr error, max_pages: %d, erase_addr: %d.\r\n",
                    max_pages,
                    page_addr);
        return AW_ERROR;
    }

    if ( (page_addr % p_this->p_info->attr.pages_per_blk) != 0) {
        aw_kprintf("nand: block erase addr not a block_addr.\r\n");
        return AW_ERROR;
    }

    return awbl_flexspi_nand_flash_block_erase (page_addr);
}


int __flexspi_nandflash_reset(awbl_nand_platform_serv_t *p_this)
{
    return AW_OK;
}



aw_err_t __flexspi_nandflash_read_id (awbl_nand_platform_serv_t *p_this,
                                      uint8_t                   *p_ids,
                                      uint8_t                    id_len)
{
     return awbl_flexspi_nand_flash_read_id (p_ids, id_len);
}


aw_bool_t __flexspi_nand_drv_probe (awbl_nand_platform_serv_t *p_this)
{
    awbl_nand_info_t   *p_info = p_this->p_info;
    int                 err;
    uint8_t             ids[10] = {0};

    if (p_this->reset != NULL) {
        err = p_this->reset(p_this);
        if (err != AW_OK) {
            AW_ERRF(("nand: reset failed\n"));
            return AW_FALSE;
        }
    }

    err = p_this->read_id(p_this, ids, p_info->id_len);
    if (err != AW_OK) {
        AW_ERRF(("nand: read ID failed\n"));
        return AW_FALSE;
    }

    AW_INFOF(("nand: chip find, id is %02xH(maker ID), %02xH(device ID).\r\n",
                ids[0], ids[1]));

    /* check IDs */
    if ((p_info->attr.maker_id != ids[0]) || (p_info->attr.device_id != ids[1])) {
        AW_ERRF(("nand error: probe failed, expect maker ID %02xH, device ID %02xH\n",
                 p_info->attr.maker_id, p_info->attr.device_id));
        return AW_FALSE;
    }

    return AW_TRUE;
}


awbl_nand_platform_serv_t *awbl_imx1050_flexspi_nand_platform_serv_create (
                                awbl_nand_info_t    *p_info,
                                struct awbl_nandbus *p_bus)
{

    void     *p_buf = NULL;
    awbl_nand_platform_serv_t *p_pf_serv;
    aw_assert(p_bus);

    p_pf_serv = malloc(sizeof(*p_pf_serv));
    aw_assert(p_pf_serv);
    memset(p_pf_serv, 0, sizeof(*p_pf_serv));

    p_pf_serv->p_info = p_info;
    p_pf_serv->p_bus  = p_bus;

    p_pf_serv->write_buf =      __flexspi_nandflash_write_buf;
    p_pf_serv->read_buf =       __flexspi_nandflash_read_buf;
    p_pf_serv->block_is_bad =   __flexspi_nandflash_block_is_bad;
    p_pf_serv->block_markbad =  __flexspi_nandflash_block_markbad;
    p_pf_serv->erase =          __flexspi_nandflash_erase;
    p_pf_serv->reset  =         __flexspi_nandflash_reset;
    p_pf_serv->read_id =        __flexspi_nandflash_read_id;
    p_pf_serv->probe  =         __flexspi_nand_drv_probe;

    p_buf = (void *)aw_mem_align(p_pf_serv->p_info->attr.page_size +
                                    p_pf_serv->p_info->attr.spare_size,
                                    sizeof(void *));
    if (p_buf != NULL) {
        p_pf_serv->p_info->p_platform_info = (void *)p_buf;
    } else {
        p_pf_serv = NULL;
    }

    return p_pf_serv;
}

void awbl_imx1050_flexspi_nand_platform_serv_destroy (awbl_nand_platform_serv_t *p_this)
{
    aw_assert(p_this);
    free(p_this);
    aw_mem_free(p_this->p_info->p_platform_info);
}

void awbl_imx1050_flexspi_nand_platform_register (void)
{
    (void)awbl_nand_platform_serv_register(awbl_imx1050_flexspi_nand_platform_serv_create,\
                                           awbl_imx1050_flexspi_nand_platform_serv_destroy);
}


/* end of file */
