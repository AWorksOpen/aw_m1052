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
 * - 1.00 15-03-10 orz, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_vdebug.h"

#include "mtd/aw_mtd_ram.h"
#include "string.h"

/******************************************************************************/
#define __mtd_to_mtdram(mtd)    ((struct aw_mtd_ram *)mtd)

/*******************************************************************************
  local variables
*******************************************************************************/

/******************************************************************************/
static int __aw_mtd_ram_erase (struct aw_mtd_info       *mtd,
                               struct aw_mtd_erase_info *ei)
{
    struct aw_mtd_ram *ram = __mtd_to_mtdram(mtd);

   (void)memset((char *)ram->mem + ei->addr, 0xff, ei->len);
    ei->state = AW_MTD_ERASE_DONE;
    aw_mtd_erase_callback(ei);
    return 0;
}

static int __aw_mtd_ram_read (struct aw_mtd_info *mtd,
                              off_t               from,
                              size_t              len,
                              size_t             *retlen,
                              void               *buf)
{
    struct aw_mtd_ram *ram = __mtd_to_mtdram(mtd);

   (void)memcpy(buf, (char *)ram->mem + from, len);
   *retlen = len;
    return 0;
}

static int __aw_mtd_ram_write (struct aw_mtd_info *mtd,
                               off_t               to,
                               size_t              len,
                               size_t             *retlen,
                               const void         *buf)
{
    struct aw_mtd_ram *ram = __mtd_to_mtdram(mtd);

   (void)memcpy((char *)ram->mem + to, buf, len);
   *retlen = len;
    return 0;
}

/******************************************************************************/
static const struct aw_mtd_ops __g_aw_mtd_ram_ops = {
    __aw_mtd_ram_erase,            /**< \brief mtd_erase */
      NULL,                        /**< \brief mtd_sync */
    __aw_mtd_ram_read,             /**< \brief mtd_read */
    __aw_mtd_ram_write,            /**< \brief mtd_write */
      NULL,                        /**< \brief mtd_panic_write */
      NULL,                        /**< \brief mtd_oob_read */
      NULL,                        /**< \brief mtd_oob_write */
      NULL,                        /**< \brief mtd_block_is_bad */
      NULL,                        /**< \brief mtd_block_mark_bad */
      NULL,                        /**< \brief mtd_is_locked */
      NULL,                        /**< \brief mtd_lock */
      NULL,                        /**< \brief mtd_unlock */
};

/*******************************************************************************
  global functions
*******************************************************************************/
aw_err_t aw_mtd_ram_add (struct aw_mtd_ram *ram,
                         const char        *name,
                         size_t             size,
                         unsigned int       erase_size)
{
    struct aw_mtd_info *mtd;
    aw_err_t            err;
    unsigned int        i;

    if ((NULL == ram)      ||
        (NULL == name)     ||
        (NULL == ram->mem) ||
        (size < erase_size)) {

        AW_ERRF(("mtdram: add MTD RAM device failed, parameter invalid\n"));
        return -AW_EINVAL;
    }
    AW_INFOF(("mtdram: adding %s\n", name));

    mtd = &ram->mtd;
   (void)memset(mtd, 0, sizeof(*mtd));

/*  mtd->name  = name; */
/*  mtd->data  = data; */
    mtd->size  = size;
    mtd->type  = AW_MTD_TYPE_RAM;
    mtd->flags = AW_MTD_FLAGS_RAM;

    mtd->oob_size  = 0;
    mtd->oob_avail = 0;

    mtd->erase_size     = erase_size;
    mtd->write_size     = 1;
    mtd->write_buf_size = erase_size;

    mtd->ecc_strength      = 0;
    mtd->bitflip_threshold = 0;

    mtd->ops = &__g_aw_mtd_ram_ops;

    if ((err = aw_mtd_dev_add(mtd, name, NULL)) != AW_OK) {
        return err;
    }

    /* add MTD partitions if any */
    if ((NULL != ram->part_info) && (NULL != ram->partitions)) {
        for (i = 0; i < ram->parts; i++) {
            err = aw_mtd_part_add(&ram->part_info[i],
                                  mtd,
                                  ram->partitions[i].name,
                                  ram->partitions[i].offset,
                                  ram->partitions[i].size,
                                  NULL);
            if (AW_OK != err) {
                return err;
            }
            /* mask out the flags of master flags */
            ram->mtd.flags &= ~ram->partitions[i].mask_flags;
        }
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_mtd_ram_del (struct aw_mtd_ram *ram)
{
    unsigned int i;

    /* Delete MTD partitions if any */
    if ((NULL != ram->part_info) && (NULL != ram->partitions)) {
        for (i = 0; i < ram->parts; i++) {
            (void)aw_mtd_part_del(&ram->part_info[i]);
        }
    }
    return aw_mtd_dev_del(&ram->mtd);
}

/* end of file */
