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
 * \brief Apollo MTD (Memory Technology Device) driver.
 *
 * \internal
 * \par modification history:
 * - 1.01 15-08-20 deo, add AW_MTD_FLAG_NO_ERASE flag match in aw_mtd_erase.
 * - 1.00 14-12-24 orz, first implementation.
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

/*******************************************************************************
  local variables
*******************************************************************************/
static struct aw_list_head __g_mtd_table;

/*******************************************************************************
  global functions
*******************************************************************************/
void aw_mtd_lib_init (void)
{
    aw_list_head_init(&__g_mtd_table);
    aw_mtd_part_lib_init();
}

/******************************************************************************/
struct aw_mtd_info *aw_mtd_dev_find (const char *name)
{
    struct aw_mtd_info  *mtd;
    AW_INT_CPU_LOCK_DECL(flags);

    AW_INT_CPU_LOCK(flags);

    /* If NULL, the MTD library is not initialized. */
    if (__g_mtd_table.next == NULL) {
        AW_INFOF(("MTD: the MTD library is not initialized."));
        return NULL;
    }

    aw_list_for_each_entry(mtd, &__g_mtd_table, struct aw_mtd_info, list) {
        if (strcmp(name, mtd->name) == 0) {
            AW_INT_CPU_UNLOCK(flags);
            return mtd;
        }
    }
    AW_INT_CPU_UNLOCK(flags);
    return NULL;
}

/******************************************************************************/
aw_err_t aw_mtd_dev_add (struct aw_mtd_info *mtd, const char *name, void *data)
{
    AW_INT_CPU_LOCK_DECL(flags);

    if ((NULL == mtd) || (NULL == name) || ('\0' == *name)) {
        AW_ERRF(("mtd: adding device with invalid parameters\n"));
        return -AW_EINVAL;
    } else if (aw_mtd_dev_find(name) != NULL) {
        AW_ERRF(("mtd: adding %s error, device already exist\n", name));
        return -AW_EEXIST;
    }
    AW_INFOF(("mtd: adding %s\n", name));

    mtd->name = name;
    mtd->data = data;

    /* add to MTD device list */
    aw_list_head_init(&mtd->list);
    AW_INT_CPU_LOCK(flags);
    aw_list_add_tail(&mtd->list, &__g_mtd_table);
    AW_INT_CPU_UNLOCK(flags);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_mtd_dev_del (struct aw_mtd_info *mtd)
{
    AW_INT_CPU_LOCK_DECL(flags);

    AW_INT_CPU_LOCK(flags);
    aw_list_del_init(&mtd->list);
    AW_INT_CPU_UNLOCK(flags);

    return AW_OK;
}

/******************************************************************************/

/**
 * \brief Erase is an asynchronous operation.  Device drivers are supposed
 * to call ei->callback() whenever the operation completes, even
 * if it completes with a failure.
 * Callers are supposed to pass a callback function and wait for it
 * to be called before writing to the block.
 */
int aw_mtd_erase (struct aw_mtd_info *mtd, struct aw_mtd_erase_info *ei)
{
    if ((ei->addr > mtd->size) ||
        (ei->len > (mtd->size - ei->addr))) {
        return -AW_EINVAL;
    }
    if (!(mtd->flags & AW_MTD_FLAG_WRITEABLE)) {
        return -AW_EROFS;
    }
    if (mtd->flags & AW_MTD_FLAG_NO_ERASE) {
        ei->state = AW_MTD_ERASE_DONE;
        return AW_OK;
    }
    ei->fail_addr = AW_MTD_ERASE_FAIL_ADDR_UNKNOWN;
    if (!ei->len) {
        ei->state = AW_MTD_ERASE_DONE;
        aw_mtd_erase_callback(ei);
        return 0;
    }
    return mtd->ops->mtd_erase(mtd, ei);
}

void aw_mtd_sync (struct aw_mtd_info *mtd)
{
    if (mtd->ops->mtd_sync) {
        mtd->ops->mtd_sync(mtd);
    }
}

int aw_mtd_read (struct aw_mtd_info *mtd,
                 off_t               from,
                 size_t              len,
                 size_t             *retlen,
                 void               *buf)
{
    int ret_code;

   *retlen = 0;
    if ((from > mtd->size) ||
        (len > (mtd->size - from))) {
        return -AW_EINVAL;
    }
    if (!len) {
        return 0;
    }
    /*
     * In the absence of an error, drivers return a non-negative integer
     * representing the maximum number of bitflips that were corrected on
     * any one ecc region (if applicable; zero otherwise).
     */
    ret_code = mtd->ops->mtd_read(mtd, from, len, retlen, buf);
    if (unlikely(ret_code < 0)) {
        return ret_code;
    }
    if (mtd->ecc_strength == 0) {
        return 0;   /* device lacks ECC */
    }
    return ret_code >= mtd->bitflip_threshold ? -AW_EUCLEAN : 0;
}

int aw_mtd_write (struct aw_mtd_info *mtd,
                  off_t               to,
                  size_t              len,
                  size_t             *retlen,
                  const void         *buf)
{
    *retlen = 0;
    if ((to > mtd->size) ||
        (len > (mtd->size - to))) {
        return -AW_EINVAL;
    }
    if (!mtd->ops->mtd_write || !(mtd->flags & AW_MTD_FLAG_WRITEABLE)) {
        return -AW_EROFS;
    }
    if (!len) {
        return 0;
    }
    return mtd->ops->mtd_write(mtd, to, len, retlen, buf);
}

/**
 * \brief In blackbox flight recorder like scenarios we want to make successful 
 * writes in interrupt context. panic_write() is only intended to be called when
 * its known the kernel is about to panic and we need the write to succeed. Since
 * the kernel is not going to be running for much longer, this function can break 
 * locks and delay to ensure the write succeeds (but not sleep).
 */
int aw_mtd_panic_write (struct aw_mtd_info *mtd,
                        off_t               to,
                        size_t              len,
                        size_t             *retlen,
                        const void         *buf)
{
    *retlen = 0;
    if (!mtd->ops->mtd_panic_write) {
        return -AW_EOPNOTSUPP;
    }
    if ((to > mtd->size) ||
        (len > (mtd->size - to))) {
        return -AW_EINVAL;
    }
    if (!(mtd->flags & AW_MTD_FLAG_WRITEABLE)) {
        return -AW_EROFS;
    }
    if (!len) {
        return 0;
    }
    return mtd->ops->mtd_panic_write(mtd, to, len, retlen, buf);
}

int aw_mtd_oob_read (struct aw_mtd_info    *mtd,
                     off_t                  from,
                     struct aw_mtd_oob_ops *ops)
{
    int ret_code;

    ops->ret_len     = 0;
    ops->oob_ret_len = 0;

    if (!mtd->ops->mtd_oob_read) {
        return -AW_EOPNOTSUPP;
    }

    if ((from + ops->len) > mtd->size ||
        ops->oob_len > mtd->oob_avail) {
        return -AW_EINVAL;
    }

    /*
     * In cases where ops->data_buf != NULL, mtd->ops->mtd_oob_read() has
     * semantics similar to mtd->ops->mtd_read(), returning a non-negative
     * integer representing max bitflips. In other cases,
     * mtd->ops->mtd_read_oob() may return -AW_EUCLEAN. In all cases,
     * perform similar logic to mtd_read().
     */
    ret_code = mtd->ops->mtd_oob_read(mtd, from, ops);
    if (unlikely(ret_code < 0)) {
        return ret_code;
    }
    if (mtd->ecc_strength == 0) {
        return 0;   /* device lacks ECC */
    }
    return ret_code >= mtd->bitflip_threshold ? -AW_EUCLEAN : 0;
}

int aw_mtd_oob_write (struct aw_mtd_info    *mtd,
                      off_t                  to,
                      struct aw_mtd_oob_ops *ops)
{
    ops->ret_len     = 0;
    ops->oob_ret_len = 0;

    if (!mtd->ops->mtd_oob_write) {
        return -AW_EOPNOTSUPP;
    }
    if (!(mtd->flags & AW_MTD_FLAG_WRITEABLE)) {
        return -AW_EROFS;
    }

    if ((to + ops->len) > mtd->size ||
        ops->oob_len > mtd->oob_avail) {
        return -AW_EINVAL;
    }

    return mtd->ops->mtd_oob_write(mtd, to, ops);
}

int aw_mtd_block_is_bad (struct aw_mtd_info *mtd, off_t ofs)
{
    if (!mtd->ops->mtd_block_is_bad) {
        return 0;
    }
    if (ofs > mtd->size) {
        return -AW_EINVAL;
    }
    return mtd->ops->mtd_block_is_bad(mtd, ofs);
}

int aw_mtd_block_mark_bad (struct aw_mtd_info *mtd, off_t ofs)
{
    if (!mtd->ops->mtd_block_mark_bad) {
        return -AW_EOPNOTSUPP;
    }
    if (ofs > mtd->size) {
        return -AW_EINVAL;
    }
    if (!(mtd->flags & AW_MTD_FLAG_WRITEABLE)) {
        return -AW_EROFS;
    }
    return mtd->ops->mtd_block_mark_bad(mtd, ofs);
}

int aw_mtd_is_locked (struct aw_mtd_info *mtd, off_t ofs, size_t len)
{
    if (!mtd->ops->mtd_is_locked) {
        return -AW_EOPNOTSUPP;
    }
    if ((ofs > mtd->size) ||
        (len > (mtd->size - ofs))) {

        return -AW_EINVAL;
    }
    if (!len) {
        return 0;
    }
    return mtd->ops->mtd_is_locked(mtd, ofs, len);
}

/* Chip-supported device locking */
int aw_mtd_lock (struct aw_mtd_info *mtd, off_t ofs, size_t len)
{
    if (!mtd->ops->mtd_lock) {
        return -AW_EOPNOTSUPP;
    }
    if ((ofs > mtd->size) ||
        (len > (mtd->size - ofs))) {
        return -AW_EINVAL;
    }
    if (!len) {
        return 0;
    }
    return mtd->ops->mtd_lock(mtd, ofs, len);
}

int aw_mtd_unlock (struct aw_mtd_info *mtd, off_t ofs, size_t len)
{
    if (!mtd->ops->mtd_unlock) {
        return -AW_EOPNOTSUPP;
    }
    if ((ofs > mtd->size) ||
        (len > (mtd->size - ofs))) {
        return -AW_EINVAL;
    }
    if (!len) {
        return 0;
    }
    return mtd->ops->mtd_unlock(mtd, ofs, len);
}

/* end of file */
