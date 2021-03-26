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
 * \brief mtd driver for YAFFS
 *
 * \internal
 * \par modification history:
 * - 1.00 2016-05-05  cyl, first implementation.
 * \endinternal
 */


#include "apollo.h"
#include "awbl_nand.h"
#include "mtd/aw_mtd.h"
#include "aw_mem.h"
#include "string.h"
#include "aw_vdebug.h"
#include "aw_shell.h"

#include "aw_yaffs.h"

//#include "driver/nand/awbl_nandflash.h"

#include "yaffsfs.h"
#include "yaffs_guts.h"
#include "yaffs_trace.h"
#include "yaffs_osglue.h"


/* 调试信息的开关 */
//#define __DEBUG_OPEN

#ifdef  __DEBUG_OPEN
#define  pr_info(fmt, ...)    \
     aw_shell_printf(aw_dbg_shell_user(), fmt, ##__VA_ARGS__)
#else
#define pr_info
#endif


/**
 * \brief mask of debug and trace
 */
unsigned yaffs_trace_mask = YAFFS_TRACE_SCAN       |
                            YAFFS_TRACE_SCAN_DEBUG |
                            YAFFS_TRACE_GC         |
                            YAFFS_TRACE_ERASE      |
                            YAFFS_TRACE_ERROR      |
                            YAFFS_TRACE_TRACING    |
                            YAFFS_TRACE_ALLOCATE   |
                            YAFFS_TRACE_BAD_BLOCKS |
                            YAFFS_TRACE_VERIFY     |
                            0;


/******************************************************************************/

/**
 * \brief get mtd device from yaffs device
 * \param[in] dev : the pointer of yaffs device
 * \return the pointer of mtd device
 */
aw_static_inline struct aw_mtd_info *__yaffs_dev_to_mtd (struct yaffs_dev *dev)
{
    return (struct aw_mtd_info *)(dev->driver_context);
}


/**
 * \brief get the beginning and endding block number from mtd device
 *
 * \param[in]  mtd     : the pointer of mtd device
 * \param[out] beg_blk : the beginning of block number
 * \param[out] end_blk : the endding of block number
 *
 * \return none
 */
static void __yaffs_get_blkno_from_mtd (struct aw_mtd_info *mtd,
                                        int                *beg_blk,
                                        int                *end_blk)
{
    int i;

    struct awbl_nand_chip *nand = NULL;

    *beg_blk = 0;
    *end_blk = 0;

    nand = (struct awbl_nand_chip *)mtd->data;
#if 0 
    for (i = 0; i < nand->npartition; i++) {
        if (mtd->name == nand->p_partition_tab[i].name) {
            *beg_blk = nand->p_partition_tab[i].start_blk_num;
            *end_blk = nand->p_partition_tab[i].end_blk_num;
            break;
        }
    }
#endif 
    *beg_blk = 0;
    *end_blk = mtd->size / mtd->erase_size;
}

/******************************************************************************/

/**
 * \brief write yaffs chunk using mtd interface
 *
 * \param[in] dev        : the pointer of yaffs device
 * \param[in] nand_chunk : the chunk number
 * \param[in] data       : the pointer of data you write
 * \param[in] data_len   : the length of data you write
 * \param[in] oob        : the pointer of oob data you write
 * \param[in] oob_len    : the length of oob data you write
 *
 * \retval YAFFS_OK   : operate successful
 * \retval YAFFS_FAIL : operate fail
 */
static int __yaffs_mtd_write_chunk (struct yaffs_dev *dev,
                                    int               nand_chunk,
                                    const uint8_t    *data,
                                    int               data_len,
                                    const uint8_t    *oob,
                                    int               oob_len)
{
    int ret = AW_OK;

    struct aw_mtd_oob_ops ops;

    struct aw_mtd_info *mtd = __yaffs_dev_to_mtd(dev);

    ops.mode     = AW_MTD_OOB_MODE_AUTO;
    ops.data_buf = (void *)data;
    ops.len      = data_len;
    ops.oob_buf  = (void *)oob;
    ops.oob_len  = oob_len;
    ops.oob_offs = 0;

    ret = aw_mtd_oob_write(mtd, nand_chunk * mtd->write_size, &ops);

    return (ret ? YAFFS_FAIL : YAFFS_OK);
}


/**
 * \brief read yaffs chunk using mtd interface
 *
 * \param[in]  dev            : the pointer of yaffs device
 * \param[in]  nand_chunk     : the chunk number
 * \param[out] data           : the pointer of data you read
 * \param[in]  data_len       : the length of data you read
 * \param[out] oob            : the pointer of oob data you read
 * \param[in]  oob_len        : the length of oob data you read
 * \param[out] ecc_result_out : the result of ecc verify
 *
 * \retval YAFFS_OK   : operate successful
 * \retval YAFFS_FAIL : operate fail
 */
static int __yaffs_mtd_read_chunk (struct yaffs_dev      *dev,
                                   int                    nand_chunk,
                                   uint8_t               *data,
                                   int                    data_len,
                                   uint8_t               *oob,
                                   int                    oob_len,
                                   enum yaffs_ecc_result *ecc_result_out)
{
    int ret = AW_OK;

    struct aw_mtd_oob_ops ops;

    struct aw_mtd_info *mtd = __yaffs_dev_to_mtd(dev);

    ops.mode     = AW_MTD_OOB_MODE_AUTO;
    ops.data_buf = data;
    ops.len      = data_len;
    ops.oob_buf  = oob;
    ops.oob_len  = oob_len;
    ops.oob_offs = 0;

    ret = aw_mtd_oob_read(mtd, nand_chunk * mtd->write_size, &ops);

    *ecc_result_out = ret ? YAFFS_ECC_RESULT_UNFIXED : YAFFS_ECC_RESULT_NO_ERROR;

    return (ret ? YAFFS_FAIL : YAFFS_OK);
}


/**
 * \brief erase block
 *
 * \param[in] dev      : the pointer of yaffs device
 * \param[in] block_no : the block number
 *
 * \retval YAFFS_OK   : operate successful
 * \retval YAFFS_FAIL : operate fail
 */
static int __yaffs_mtd_erase_block (struct yaffs_dev *dev, int block_no)
{
    struct aw_mtd_erase_info ei;

    struct aw_mtd_info *mtd = __yaffs_dev_to_mtd(dev);

    ei.mtd      = mtd;
    ei.addr     = block_no * mtd->erase_size;
    ei.len      = mtd->erase_size;
    ei.retries  = 2;
    ei.callback = NULL;
    ei.next     = NULL;
    ei.priv     = NULL;

    if (AW_OK == aw_mtd_erase(mtd, &ei)) {

        pr_info("\nerase %d block. \n", block_no);

        return YAFFS_OK;
    } else {
        return YAFFS_FAIL;
    }
}


/**
 * \brief mark block bad
 *
 * \param[in] dev      : the pointer of yaffs device
 * \param[in] block_no : the block number
 *
 * \retval YAFFS_OK   : operate successful
 * \retval YAFFS_FAIL : operate fail
 */
static int __yaffs_mtd_mark_bad (struct yaffs_dev *dev, int block_no)
{
    struct aw_mtd_info *mtd = __yaffs_dev_to_mtd(dev);

    if (AW_OK == aw_mtd_block_mark_bad(mtd, block_no * mtd->erase_size)) {

        pr_info("\nmark %d block is bad. \n", block_no);

        return YAFFS_OK;
    } else {
        return YAFFS_FAIL;
    }
}


/**
 * \brief check the block is bad or not
 *
 * \param[in] dev      : the pointer of yaffs device
 * \param[in] block_no : the block number
 *
 * \retval YAFFS_OK   : operate successful
 * \retval YAFFS_FAIL : operate fail
 */
static int __yaffs_mtd_check_bad (struct yaffs_dev *dev, int block_no)
{
    struct aw_mtd_info *mtd = __yaffs_dev_to_mtd(dev);

    /* mtd->erase_size = 131072(2048*64), 那oob区不需要擦除吗  */
    if (aw_mtd_block_is_bad(mtd, block_no * mtd->erase_size)) {
        pr_info("\nthe %d block is bad. \n", block_no);
        return YAFFS_FAIL;
    } else {
        pr_info("\nthe %d block is normal. \n", block_no);
        return YAFFS_OK;
    }
}


/**
 * \brief initalise the mtd interface
 * \param[in] dev : the pointer of yaffs device
 * \return YAFFS_OK : operate successful
 */
static int __yaffs_mtd_initialise (struct yaffs_dev *dev)
{
    (void)dev;
    return YAFFS_OK;
}


/**
 * \brief deinitalise the mtd interface
 * \param[in] dev : the pointer of yaffs device
 * \return YAFFS_OK : operate successful
 */
static int __yaffs_mtd_deinitialise (struct yaffs_dev *dev)
{
    (void)dev;
    return YAFFS_OK;
}


/******************************************************************************/

/**
 * \brief mtd init
 * \param[in] vol : the pointer of yaffs volume
 * \return none
 */
void aw_yaffs_mtd_init (struct aw_yaffs_volume *vol)
{
    int beg_blk, end_blk;

    struct aw_mtd_info  *mtd     = NULL;
    struct yaffs_dev    *yfs_dev = NULL;
    struct yaffs_driver *drv     = NULL;

    if (NULL == vol) {
        return;
    }

    mtd     = vol->mtd;
    yfs_dev = &vol->yfs_dev;

/*    yfs_dev->os_context = mtd; */
    yfs_dev->driver_context = mtd;

    __yaffs_get_blkno_from_mtd(mtd, &beg_blk, &end_blk);

    yfs_dev->param.start_block = beg_blk;
    yfs_dev->param.end_block   = end_blk;

    yfs_dev->param.chunks_per_block      = mtd->erase_size / mtd->write_size;
    yfs_dev->param.total_bytes_per_chunk = mtd->write_size;
/* yfs_dev->param.spare_bytes_per_chunk = mtd->oob_layout->oob_free->length; */
    yfs_dev->param.spare_bytes_per_chunk = mtd->oob_avail;
    yfs_dev->param.is_yaffs2             = 1;
    yfs_dev->param.n_reserved_blocks     = 5;
    yfs_dev->param.always_check_erased   = 1;
    yfs_dev->param.no_tags_ecc           = 1;

    drv = &yfs_dev->drv;
    drv->drv_write_chunk_fn  = __yaffs_mtd_write_chunk;
    drv->drv_read_chunk_fn   = __yaffs_mtd_read_chunk;
    drv->drv_erase_fn        = __yaffs_mtd_erase_block;
    drv->drv_mark_bad_fn     = __yaffs_mtd_mark_bad;
    drv->drv_check_bad_fn    = __yaffs_mtd_check_bad;
    drv->drv_initialise_fn   = __yaffs_mtd_initialise;
    drv->drv_deinitialise_fn = __yaffs_mtd_deinitialise;
}

/**
 * \brief mtd device erase all blocks
 * \param[in] vol : the pointer of yaffs volume
 * \return none
 */
void aw_yaffs_mtd_erase (struct aw_yaffs_volume *vol)
{
    int i;
    int beg_blk, end_blk;

    struct aw_mtd_info *mtd = NULL;

    if (NULL == vol) {
        return;
    }

    mtd = vol->mtd;

    __yaffs_get_blkno_from_mtd(mtd, &beg_blk, &end_blk);

    pr_info("\nerase all block(%d~%d) start: \n", beg_blk, end_blk);

    for (i = beg_blk; i < end_blk; i++) {
        if (YAFFS_FAIL == __yaffs_mtd_check_bad(&vol->yfs_dev, i)) {
            continue;
        }

        if (YAFFS_OK != __yaffs_mtd_erase_block(&vol->yfs_dev, i)) {
            AW_ERRF(("yaffs: fail to erase block %d\n", i));
        }
    }
}


/**
 * \brief yaffs startup
 * \param[in] none
 * \return 0 : operate successful
 */
int yaffs_start_up (void)
{
    static int __start_up_called = 0;

    if (__start_up_called) {
        return 0;
    }

    __start_up_called = 1;

    yaffsfs_OSInitialisation();

    return 0;
}

/* end of file */
