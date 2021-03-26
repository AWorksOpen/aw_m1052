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
 * \brief Apollo MTD (Memory Technology Device) partition lib.
 *
 * \internal
 * \par modification history:
 * - 150305 orz, first implementation.
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/

#include "apollo.h"
#include "aw_vdebug.h"

#include "mtd/aw_mtd.h"
#include "aw_int.h"
#include "aw_assert.h"
#include "aw_compiler.h"
#include "string.h"

/******************************************************************************/
#define __mtd_to_part(mtd)  ((struct aw_mtd_part_info *)mtd)

/*******************************************************************************
  local variables
*******************************************************************************/
static struct aw_list_head __g_mtd_partitions;

/******************************************************************************/
static int __aw_mtd_part_erase (struct aw_mtd_info       *mtd,
                                struct aw_mtd_erase_info *ei)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    ei->addr = ei->addr + part->offset;
    return part->master->ops->mtd_erase(mtd, ei);
}

static void __aw_mtd_part_sync (struct aw_mtd_info *mtd)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    if (part->master->ops->mtd_sync) {
        part->master->ops->mtd_sync(mtd);
    }
}

static int __aw_mtd_part_read (struct aw_mtd_info *mtd,
                               off_t               from,
                               size_t              len,
                               size_t             *retlen,
                               void               *buf)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    return part->master->ops->mtd_read(mtd,
                                       part->offset + from,
                                       len,
                                       retlen,
                                       buf);
}

static int __aw_mtd_part_write (struct aw_mtd_info *mtd,
                                off_t               to,
                                size_t              len,
                                size_t             *retlen,
                                const void         *buf)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    return part->master->ops->mtd_write(mtd,
                                        part->offset + to,
                                        len,
                                        retlen,
                                        buf);
}

static int __aw_mtd_part_panic_write (struct aw_mtd_info *mtd,
                                      off_t               to,
                                      size_t              len,
                                      size_t             *retlen,
                                      const void         *buf)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    if (!part->master->ops->mtd_panic_write) {
        return -AW_EOPNOTSUPP;
    }
    return part->master->ops->mtd_panic_write(mtd,
                                              part->offset + to,
                                              len,
                                              retlen,
                                              buf);
}

static int __aw_mtd_part_oob_read (struct aw_mtd_info    *mtd,
                                   off_t                  from,
                                   struct aw_mtd_oob_ops *ops)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    if (!part->master->ops->mtd_oob_read) {
        return -AW_EOPNOTSUPP;
    }
    return part->master->ops->mtd_oob_read(mtd,
                                           part->offset + from,
                                           ops);
}

static int __aw_mtd_part_oob_write (struct aw_mtd_info    *mtd,
                                    off_t                  to,
                                    struct aw_mtd_oob_ops *ops)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    if (!part->master->ops->mtd_oob_write) {
        return -AW_EOPNOTSUPP;
    }
    return part->master->ops->mtd_oob_write(mtd,
                                            part->offset + to,
                                            ops);
}

static int __aw_mtd_part_is_locked (struct aw_mtd_info *mtd,
                                    off_t               ofs,
                                    size_t              len)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    if (!part->master->ops->mtd_is_locked) {
        return -AW_EOPNOTSUPP;
    }
    return part->master->ops->mtd_is_locked(mtd,
                                            part->offset + ofs,
                                            len);
}

static int __aw_mtd_part_block_is_bad (struct aw_mtd_info *mtd, off_t ofs)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    if (!part->master->ops->mtd_block_is_bad) {
        return 0;
    }
    return part->master->ops->mtd_block_is_bad(mtd,
                                               part->offset + ofs);
}

static int __aw_mtd_part_block_mark_bad (struct aw_mtd_info *mtd, off_t ofs)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    if (!part->master->ops->mtd_block_mark_bad) {
        return -AW_EOPNOTSUPP;
    }
    return part->master->ops->mtd_block_mark_bad(mtd,
                                                 part->offset + ofs);
}

static int __aw_mtd_part_lock (struct aw_mtd_info *mtd, off_t ofs, size_t len)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    if (!part->master->ops->mtd_lock) {
        return -AW_EOPNOTSUPP;
    }
    return part->master->ops->mtd_lock(mtd, part->offset + ofs, len);
}

static int __aw_mtd_part_unlock (struct aw_mtd_info *mtd, off_t ofs, size_t len)
{
    struct aw_mtd_part_info *part = __mtd_to_part(mtd);

    if (!part->master->ops->mtd_unlock) {
        return -AW_EOPNOTSUPP;
    }
    return part->master->ops->mtd_unlock(mtd, part->offset + ofs, len);
}

/******************************************************************************/
static const struct aw_mtd_ops __g_aw_mtd_part_ops = {
    __aw_mtd_part_erase,            /**< \brief mtd_erase */
    __aw_mtd_part_sync,             /**< \brief mtd_sync */
    __aw_mtd_part_read,             /**< \brief mtd_read */
    __aw_mtd_part_write,            /**< \brief mtd_write */
    __aw_mtd_part_panic_write,      /**< \brief mtd_panic_write */
    __aw_mtd_part_oob_read,         /**< \brief mtd_oob_read */
    __aw_mtd_part_oob_write,        /**< \brief mtd_oob_write */
    __aw_mtd_part_block_is_bad,     /**< \brief mtd_block_is_bad */
    __aw_mtd_part_block_mark_bad,   /**< \brief mtd_block_mark_bad */
    __aw_mtd_part_is_locked,        /**< \brief mtd_is_locked */
    __aw_mtd_part_lock,             /**< \brief mtd_lock */
    __aw_mtd_part_unlock            /**< \brief mtd_unlock */
};

/*******************************************************************************
    global functions
*******************************************************************************/
void aw_mtd_part_lib_init (void)
{
    aw_list_head_init(&__g_mtd_partitions);
}

/******************************************************************************/
aw_bool_t aw_mtd_is_partition (struct aw_mtd_info *mtd)
{
    struct aw_mtd_part_info *part;
    AW_INT_CPU_LOCK_DECL(    flags);

    AW_INT_CPU_LOCK(flags);
    aw_list_for_each_entry(part,
                           &__g_mtd_partitions,
                           struct aw_mtd_part_info,
                           list) {
        if (mtd == &part->mtd) {
            AW_INT_CPU_UNLOCK(flags);
            return AW_TRUE;
        }
    }
    AW_INT_CPU_UNLOCK(flags);
    return AW_FALSE;
}

/******************************************************************************/
aw_err_t aw_mtd_part_add (struct aw_mtd_part_info *part,
                          struct aw_mtd_info      *master,
                          const char              *name,
                          off_t                    offset,
                          size_t                   size,
                          void                    *data)
{
    struct aw_mtd_info  *slave;
    AW_INT_CPU_LOCK_DECL(flags);
    aw_err_t             err;

    if ((NULL == part) || (NULL == master) || (NULL == name)) {
        AW_ERRF(("mtdpart: adding part with invalid parameters\n"));
        return -AW_EINVAL;
    }
    if (AW_MTD_PART_SIZ_FULL == size) {
        size = master->size - offset;
    }
    if (size <= 0) {
        AW_ERRF(("mtdpart: adding a zero size partition %s\n", name));
        return -AW_EINVAL;
    }
    if (offset >= master->size) {
        AW_ERRF(("mtdpart: partition %s is out of reach (%ul)\n", name, offset));
        return -AW_EINVAL;
    }
    AW_INFOF(("mtdpart: adding %s\n", name));

    part->master = master;
    part->offset = offset;

    /* set up the MTD object for this partition */

    slave = &part->mtd;
   (void)memset(slave, 0, sizeof(*slave));

/*  slave->name  = name; */
/*  slave->data  = data; */
    slave->size  = size;
    slave->type  = master->type;
    slave->flags = master->flags;

    slave->oob_size   = master->oob_size;
    slave->oob_avail  = master->oob_avail;
    slave->oob_layout = master->oob_layout;

    slave->erase_size     = master->erase_size;
    slave->write_size     = master->write_size;
    slave->write_buf_size = master->write_buf_size;

    slave->ecc_strength      = master->ecc_strength;
    slave->bitflip_threshold = master->bitflip_threshold;

    slave->ops = &__g_aw_mtd_part_ops;

    if ((err = aw_mtd_dev_add(slave, name, data)) != AW_OK) {
        return err;
    }

    /* add to partition list */
    aw_list_head_init(&part->list);
    AW_INT_CPU_LOCK(flags);
    aw_list_add_tail(&part->list, &__g_mtd_partitions);
    AW_INT_CPU_UNLOCK(flags);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_mtd_part_del (struct aw_mtd_part_info *part)
{
    AW_INT_CPU_LOCK_DECL(flags);
    aw_err_t             err;

    if ((err = aw_mtd_dev_del(&part->mtd)) != AW_OK) {
        return err;
    }

    AW_INT_CPU_LOCK(flags);
    aw_list_del_init(&part->list);
    AW_INT_CPU_UNLOCK(flags);

    return AW_OK;
}

/******************************************************************************/
void aw_mtd_erase_callback (struct aw_mtd_erase_info *ei)
{
    struct aw_mtd_part_info *part;

    if (__aw_mtd_part_erase == ei->mtd->ops->mtd_erase) {
        part = __mtd_to_part(ei->mtd);
        if (ei->fail_addr != AW_MTD_ERASE_FAIL_ADDR_UNKNOWN) {
            ei->fail_addr -= part->offset;
        }
        ei->addr -= part->offset;
    }
    if (NULL != ei->callback) {
        ei->callback(ei);
    }
}

/* end of file */
