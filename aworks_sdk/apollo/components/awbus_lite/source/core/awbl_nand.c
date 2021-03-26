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
 * \brief Nand source file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-3-22  vih, first implementation
 * - 2.00 20-04-14 vih, refactoring
 * \endinternal
 */

#include "aworks.h"
#include "awbl_nand.h"
#include "aw_bitops.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_mem.h"
#include "aw_assert.h"
#include "aw_delay.h"
#include "awbl_nand_ecc.h"
/******************************************************************************/
#define __LOG_DEV_DEF(p_ctl)  \
        awbl_nand_log_t _pfn_log_out_ = \
            (p_ctl ? ((awbl_nand_t *)p_ctl)->init.pfn_log : NULL )

#define __LOG_FCOLOR_RED        "\x1B[31m"
#define __LOG_FCOLOR_GREEN      "\x1B[32m"

#define __LOG(mask, color, fmt, ...)   \
    if ((mask) && _pfn_log_out_) {\
         _pfn_log_out_(color, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG(mask, NULL, "[%s:%d] " fmt "\n", \
               __func__, __LINE__, ##__VA_ARGS__)

#define __LOG_RES(is_true, mask, fmt, ...) \
    if (is_true) {\
        __LOG(mask, __LOG_FCOLOR_GREEN, "[ OK ] " fmt "\n", \
                     ##__VA_ARGS__);\
    } else {\
        __LOG(1, __LOG_FCOLOR_RED,\
                     "[ ER ] [%s:%d] " fmt "\n", \
                     __func__, __LINE__, ##__VA_ARGS__);\
    }

#define __CHECK(cond, exe, fmt, ...) \
    if (!(cond)) {\
        __LOG(1, __LOG_FCOLOR_RED, \
                   "[ CHECK Failed ][%s:%d] " #cond "\n",\
                   __func__, __LINE__); \
        __LOG(1, NULL,\
                   "    [ DETAIL ] " fmt " \n", ##__VA_ARGS__);\
        exe; \
    }

#define __ASSERT(cond, fmt, ...) \
    if (!(cond)) {\
        __LOG(1, __LOG_FCOLOR_RED,\
               "[ ASSERT Failed ] \n- file: %s : %d \n- func: %s() \n- cond: "\
               #cond "\n", __FILE__, __LINE__, __func__); \
        if (strlen((char *)fmt) != 0) {\
            __LOG(1, __LOG_FCOLOR_RED,"- \"" fmt "\"\n", ##__VA_ARGS__);\
        } \
        aw_assert(cond); \
    }

#define __CHECK_D(a, cond, b, exe)    __CHECK((a cond b), exe, "%d %d", a, b)
#define __ASSERT_D(a, cond, b)        __ASSERT((a cond b), "%d %d", a, b)

/******************************************************************************/
#define __LOG_RW             1
#define __LOG_MTD            0


/******************************************************************************/
/* The maximum expected count of bytes in the NAND ID sequence */
#define NAND_MAX_ID_LEN  8

/* status */
#define __NANDFLASH_STATUS_FAIL           0x01

/******************************************************************************/
typedef struct __nand_platform_serv_reg {
    awbl_nand_platform_serv_t *(*pfn_platform_serv_create) (
                    awbl_nand_info_t    *,
                    struct awbl_nandbus *);
    void (*pfn_platform_serv_destroy) (awbl_nand_platform_serv_t *);
} __nand_platform_serv_reg_t;

__nand_platform_serv_reg_t __g_platform_serv_reg = { NULL, NULL };
/******************************************************************************/
static void *__mem_alloc (size_t size)
{
    void *p = aw_mem_alloc(size);
    aw_assert(p);
    return p;
}

static void __mem_free (void *p)
{
    if (p != NULL) {
        aw_mem_free(p);
    }
}

static void __nand_mem_free (awbl_nand_t *p_nand)
{
    __mem_free(p_nand->p_mtd_part);
    p_nand->p_mtd_part = NULL;
}

/******************************************************************************/
aw_err_t awbl_nand_read_id (awbl_nand_t             *p_nand,
                            uint8_t                 *p_ids,
                            uint8_t                  id_len)
{
    aw_err_t ret;
    aw_assert(p_nand->p_pf_serv->read_id);
    ret = p_nand->p_pf_serv->read_id(p_nand->p_pf_serv, p_ids, id_len);
    return ret;
}
/******************************************************************************/
/**
 * \brief 找到一个二进制数中最低位1所在索引位置
 */
aw_local int __nand_ffs (int v)
{
    int i;
    for (i=0; i<(sizeof(int) * 8); i++) {
        if ((1<<i) & v) {
            return (i + 1);
        }
    }
    return 0;
}

/******************************************************************************/
aw_local int __nandflash_check_offs_len_blk (struct aw_mtd_info *mtd,
                                             off_t               offs,
                                             size_t              len)
{
    awbl_nand_t           *p_nand;

    if (mtd == NULL) {
        return -AW_EINVAL;
    }

    p_nand = awbl_mtd_get_nand(mtd);

    /* Start address must align on block boundary */
/*    if (offs & p_nand->phys_erase_shift) { */
    if (offs & ((1ULL << p_nand->phys_erase_shift) - 1)) {
        return -AW_EINVAL;
    }

    /* Length must align on block boundary */
    /*    if (len & p_partition->erase_align_mask) {*/
    if (len & ((1ULL << p_nand->phys_erase_shift) - 1)) {
        return -AW_EINVAL;
    }

    /* Do not allow past end of device */
/*
 *     if (offs + len > mtd->size) {
 *       return -AW_EINVAL;
 *   }
 *
 */

    return AW_OK;
}

/******************************************************************************/
aw_local int __nand_read_page (struct aw_mtd_info    *mtd,
                               off_t                  from,
                               struct aw_mtd_oob_ops *ops)
{
    int       err;
    uint32_t  page;
    uint16_t  col;
    uint32_t  bytes;
    uint32_t  length, oob_length, max_oobsize, oob_readlen;
    uint8_t  *ptr = NULL, *buf = NULL, *oob_buf = NULL;
    aw_bool_t is_align, is_use_buf, is_raw_data, oob_required;

    awbl_nand_t             *p_nand = awbl_mtd_get_nand(mtd);
    awbl_nand_ecc_t         *p_ecc = p_nand->p_ecc;
    __LOG_DEV_DEF(p_nand);
    (void)is_raw_data;

    __ASSERT(p_nand && p_ecc && ops->len, "");

    page          = from >> p_nand->page_shift;
    col           = from & (mtd->write_size - 1);
    length        = ops->len;
    ptr           = ops->data_buf;
    oob_length    = ops->oob_len;
    oob_buf       = ops->oob_buf;
    oob_required  = ops->oob_buf ? AW_TRUE : AW_FALSE;
    is_raw_data   = (AW_MTD_OOB_MODE_RAW  == ops->mode) ? AW_TRUE : AW_FALSE;
    max_oobsize   = mtd->oob_avail;

    while (length) {

        bytes = min(mtd->write_size - col, length);

        is_align = (bytes == mtd->write_size);

        is_use_buf = ((is_align) ? AW_FALSE : AW_TRUE);

        buf = (is_use_buf ? p_nand->page_buf : ptr);
        if (is_use_buf) {
            memset(buf, 0, mtd->write_size);
        }

        if (unlikely(ops->mode == AW_MTD_OOB_MODE_RAW)) {
            err = p_ecc->itf.read_page_raw(p_ecc,
                                           buf,
                                           oob_buf,
                                           oob_length,
                                           oob_required,
                                           page);
        } else {
            oob_required = 1;
            err = p_ecc->itf.read_page(p_ecc,
                                       buf,
                                       oob_buf,
                                       oob_length,
                                       oob_required,
                                       page);
        }

        if (is_use_buf) {
            memcpy(ptr, buf + col, bytes);
        }

        if (oob_required) {
            oob_readlen = min(oob_length, max_oobsize);
            if (oob_readlen) {
                oob_buf += oob_readlen;
                oob_length -= oob_readlen;
            }
        }

        col     = 0;
        ptr    += bytes;
        length -= bytes;
        page++;

        /* 如果是ecc错误，数据和oob仍然读出来 */
        if (AW_OK != err) {
            break;
        }
    }

    ops->ret_len     = ops->len - length;
    ops->oob_ret_len = oob_readlen;

    return err;
}

static int __nand_write_page (struct aw_mtd_info    *mtd,
                              off_t                  to,
                              struct aw_mtd_oob_ops *ops)
{
    int       err;
    uint32_t  page;
    uint16_t  col;
    uint32_t  bytes;
    uint32_t  length, oob_length, max_oobsize, oob_writelen;
    uint8_t  *ptr = NULL, *oob_buf = NULL;
    aw_bool_t            is_raw_data, oob_required;
    aw_bool_t            is_align, is_use_buf;
    uint8_t             *buf = NULL;
    awbl_nand_t         *p_nand = awbl_mtd_get_nand(mtd);
    awbl_nand_ecc_t     *p_ecc = p_nand->p_ecc;
    __LOG_DEV_DEF(p_nand);

    (void)is_raw_data;

    __ASSERT(p_nand && p_ecc, "");
    if (ops->len == 0) {
        return AW_OK;
    }

    if (0 != (to & (mtd->write_size - 1)) ||
        0 != (to & (mtd->write_size - 1))) {
        return -AW_EINVAL;
    }

    /* get page address and column address */
    page          = to >> p_nand->page_shift;
    col           = to & (mtd->write_size - 1);
    length        = ops->len;
    ptr           = ops->data_buf;
    oob_length    = ops->oob_len;
    oob_buf       = ops->oob_buf;
    oob_required  = ops->oob_buf ? AW_TRUE : AW_FALSE;
    is_raw_data   = (AW_MTD_OOB_MODE_RAW  == ops->mode) ? AW_TRUE : AW_FALSE;
    max_oobsize   = mtd->oob_avail;

    if (ops->oob_buf  &&
        ops->oob_offs &&
       (ops->oob_offs + ops->oob_len > max_oobsize)) {
        return -AW_EINVAL;
    }

    while (length) {
        bytes       = min(mtd->write_size - col, length);
        is_align    = (bytes == mtd->write_size);
        is_use_buf  = ((is_align) ? AW_FALSE : AW_TRUE);
        buf         = (is_use_buf ? p_nand->page_buf : ptr);

        if (is_use_buf) {
            memset(buf, 0, mtd->write_size);
            memcpy(buf + col, ptr, bytes);
        }

        oob_writelen = min(oob_length, max_oobsize);
        if (unlikely(ops->mode == AW_MTD_OOB_MODE_RAW)) {
            err = p_nand->p_ecc->itf.write_page_raw(p_ecc,
                                                    buf,
                                                    oob_buf,
                                                    oob_writelen,
                                                    oob_required,
                                                    page);
        } else {
            oob_required = 1;
            err = p_nand->p_ecc->itf.write_page(p_ecc,
                                                buf,
                                                oob_buf,
                                                oob_writelen,
                                                oob_required,
                                                page);
        }

        if (oob_required && oob_writelen) {
            oob_buf    += oob_writelen;
            oob_length -= oob_writelen;
        }

        if (AW_OK != err) {
            break;
        }

        ptr    += bytes;
        length -= bytes;
        col     =  0;
        page++;
    }

    ops->ret_len     = ops->len - length;
    ops->oob_ret_len = oob_writelen;
    return err;
}

static int __nand_read_oob (struct aw_mtd_info    *mtd,
                            off_t                  from,
                            struct aw_mtd_oob_ops *ops)
{
    int                      ret = 0;
    awbl_nand_t             *p_nand = awbl_mtd_get_nand(mtd);
    awbl_nand_ecc_t         *p_ecc = p_nand->p_ecc;
    __LOG_DEV_DEF(p_nand);

    __ASSERT_D(p_nand, !=, NULL);
    __ASSERT_D(p_ecc, !=, NULL);

    __CHECK_D(ops->oob_len + ops->oob_offs, <=, mtd->oob_avail,
              return -AW_EINVAL);

    /* 起始地址须为页的首地址 */
    __ASSERT_D((from & (p_nand->p_info->attr.page_size - 1)), ==, 0);

    /* 无论哪种模式，应用只能读取到 oob free 区的数据  */
    ret = p_nand->p_ecc->itf.read_oob_free(p_ecc,
                                           (from >> p_nand->page_shift),
                                           ops->oob_offs,
                                           ops->oob_buf,
                                           ops->oob_len);
    __CHECK_D(ret, ==, AW_OK, return ret);
    return ret;
}

static int __nand_write_oob (struct aw_mtd_info    *mtd,
                             off_t                  to,
                             struct aw_mtd_oob_ops *ops)
{
    int                      ret = 0;
    awbl_nand_t             *p_nand = awbl_mtd_get_nand(mtd);
    awbl_nand_ecc_t         *p_ecc = p_nand->p_ecc;
    __LOG_DEV_DEF(p_nand);

    __ASSERT_D(p_nand, !=, NULL);
    __ASSERT_D(p_ecc, !=, NULL);

    __CHECK_D(ops->oob_len + ops->oob_offs, <=, mtd->oob_avail,
              return -AW_EINVAL);

    /* 起始地址须为页的首地址 */
    __ASSERT_D((to & (p_nand->p_info->attr.page_size - 1)), ==, 0);

    /* 无论哪种模式，应用只能读取到 oob free 区的数据  */
    ret = p_nand->p_ecc->itf.write_oob_free(p_ecc,
                                           (to >> p_nand->page_shift),
                                           ops->oob_offs,
                                           ops->oob_buf,
                                           ops->oob_len);
    __CHECK_D(ret, ==, AW_OK, return ret);
    return ret;
}

/******************************************************************************/
/**
 * \retval  AW_OK    : 擦除成功
 * \retval  -AW_EBADMSG : 是坏块，不能擦除
 * \retval  -AW_EIO     : IO错误
 * \retval  -AW_EINVAL  : 参数错误
 */
aw_local int __nand_mtd_erase (struct aw_mtd_info       *mtd,
                               struct aw_mtd_erase_info *instr)
{
    awbl_nand_t           *p_nand;
    int                    err = AW_OK;
    size_t                 len;
    size_t                 page;

    if (mtd == NULL || instr == NULL) {
        return -AW_EINVAL;
    }

    p_nand = awbl_mtd_get_nand(mtd);
    if (p_nand == NULL) {
        return -AW_EINVAL;
    }

    err = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(err == AW_OK);

    if (__nandflash_check_offs_len_blk(mtd, instr->addr, instr->len)) {
        err = -AW_EINVAL;
        goto _exit;
    }

    instr->fail_addr = AW_MTD_ERASE_FAIL_ADDR_UNKNOWN;
    instr->state     = AW_MTD_ERASE_PROCESSING;

    /* Shift to get first page */
    page =  instr->addr >> p_nand->page_shift;

    /* Loop through the pages */
    len = instr->len;
    while (len) {

        if (p_nand->p_pf_serv->block_is_bad(p_nand->p_pf_serv, page)) {
            AW_INFOF(("\n%s: attempt to erase a bad block at page 0x%08x\n",
                    __func__, page));
            instr->state = AW_MTD_ERASE_FAILED;
            err = -AW_EBADMSG;
            goto _exit;
        }

        err = p_nand->p_pf_serv->erase(p_nand->p_pf_serv, page);
        /* See if block erase succeeded */
        if (err != AW_OK) {
            instr->state     = AW_MTD_ERASE_FAILED;
            instr->fail_addr = ((off_t)page << p_nand->page_shift);
            err = -AW_EIO;
            goto _exit;
        }

        /* Increment page address and decrement length */
        len -= mtd->erase_size;

        /* Calculate pages in each block */
        page += 1 << (p_nand->phys_erase_shift - p_nand->page_shift);
    }

    instr->state = AW_MTD_ERASE_DONE;
    err = AW_OK;
    aw_mtd_erase_callback(instr);

_exit:
/*    err = (instr->state == AW_MTD_ERASE_DONE) ? AW_OK : err; */
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return err;
}

aw_local int __nand_mtd_read (struct aw_mtd_info *mtd,
                              off_t               from,
                              size_t              len,
                              size_t             *retlen,
                              void               *buf)
{
    awbl_nand_t          *p_nand;
    int                   err;
    struct aw_mtd_oob_ops ops;

    if (NULL == mtd || NULL == buf || 0 == len) {
        return -AW_EINVAL;
    }

    p_nand = awbl_mtd_get_nand(mtd);
    memset(&ops, 0, sizeof(ops));

    err = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(err == AW_OK);

    ops.data_buf = buf;
    ops.len      = len;
    ops.mode     = AW_MTD_OOB_MODE_AUTO;
    err = __nand_read_page(mtd, from, &ops);

    *retlen = ops.ret_len;

    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return err;
}

aw_local int __nand_mtd_write (struct aw_mtd_info *mtd,
                               off_t               to,
                               size_t              len,
                               size_t             *retlen,
                               const void         *buf)
{
    awbl_nand_t          *p_nand;
    int                   err;
    struct aw_mtd_oob_ops ops;


    if (NULL == mtd || NULL == buf || 0 == len) {
        return -AW_EINVAL;
    }

    p_nand = awbl_mtd_get_nand(mtd);
    memset(&ops, 0, sizeof(ops));

    err = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(err == AW_OK);
    ops.data_buf = (void *)buf;
    ops.len      = len;
    ops.mode     = AW_MTD_OOB_MODE_AUTO;
    err = __nand_write_page(mtd, to, &ops);

    *retlen = ops.ret_len;

    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return err;
}


aw_local int __nand_mtd_oob_read  (struct aw_mtd_info    *mtd,
                                   off_t                  from,
                                   struct aw_mtd_oob_ops *ops)
{
    awbl_nand_t *p_nand;
    int          err = AW_OK;

    p_nand = awbl_mtd_get_nand(mtd);

    switch(ops->mode) {
    case AW_MTD_OOB_MODE_PLACE:
    case AW_MTD_OOB_MODE_AUTO:
    case AW_MTD_OOB_MODE_RAW:
        break;

    default:
        return -AW_ENOTSUP;
    }

    err = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(err == AW_OK);
    if (!ops->data_buf) {
        err = __nand_read_oob(mtd, from, ops);
    } else {
        err = __nand_read_page(mtd, from, ops);
    }
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return err;
}

aw_local int __nand_mtd_oob_write (struct aw_mtd_info    *mtd,
                                   off_t                  to,
                                   struct aw_mtd_oob_ops *ops)
{
    awbl_nand_t *p_nand;
    int          err = AW_OK;

    p_nand = awbl_mtd_get_nand(mtd);

    switch(ops->mode) {
    case AW_MTD_OOB_MODE_PLACE:
    case AW_MTD_OOB_MODE_AUTO:
    case AW_MTD_OOB_MODE_RAW:
        break;

    default:
        return -AW_ENOTSUP;
    }
    err = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(err == AW_OK);
    if (!ops->data_buf) {
        err = __nand_write_oob(mtd, to, ops);
    } else {
        err = __nand_write_page(mtd, to, ops);
    }
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return err;
}

/*
 * \retval  AW_OK : 不是坏块
 * \retval  1     : 是坏块
 * \retval  -AW_EIO  : 有IO错误
 * \retval  -EIVAL: 参数无效
 */
aw_local int __nand_mtd_block_is_bad  (struct aw_mtd_info *mtd, off_t ofs)
{
    awbl_nand_t       *p_nand;
    aw_bool_t          is_bad;
    aw_err_t           ret;

    p_nand = awbl_mtd_get_nand(mtd);

    if (p_nand == NULL) {
        return -AW_EINVAL;
    }

    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);
    is_bad = p_nand->p_pf_serv->block_is_bad(p_nand->p_pf_serv,
                                             (ofs >> p_nand->page_shift));
    if (is_bad) {
        /* 是坏块 */
        ret =  1;
        goto __exit;
    }
    /* 不是坏块 */
    ret = AW_OK;

__exit:
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}


aw_local int __nand_mtd_block_mark_bad (struct aw_mtd_info *mtd, off_t ofs)
{
    awbl_nand_t                 *p_nand;
    aw_err_t                     ret;

    p_nand = awbl_mtd_get_nand(mtd);

    if (p_nand == NULL) {
        return -AW_EINVAL;
    }

    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);
    ret = p_nand->p_pf_serv->block_markbad(p_nand->p_pf_serv,
                                     (ofs >> p_nand->page_shift));
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

const struct aw_mtd_ops __g_mtd_ops = {
    .mtd_block_is_bad   = __nand_mtd_block_is_bad,
    .mtd_erase          = __nand_mtd_erase,
    .mtd_read           = __nand_mtd_read,
    .mtd_write          = __nand_mtd_write,
    .mtd_panic_write    = __nand_mtd_write,
    .mtd_oob_read       = __nand_mtd_oob_read,
    .mtd_oob_write      = __nand_mtd_oob_write,
    .mtd_block_mark_bad = __nand_mtd_block_mark_bad,
};

/******************************************************************************/

/******************************************************************************/


aw_err_t awbl_nand_ioctl (awbl_nand_t *p_nand,
                          awbl_nand_ioctl_req_t  req,
                          void                  *dat)
{
    __LOG_DEV_DEF(p_nand);
    aw_err_t ret;
    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);

    switch (req) {

    case AWBL_NAND_PAGE_SIZE:
        *(uint32_t *)dat = (uint32_t)(1 << p_nand->page_shift);
        break;

    case AWBL_NAND_BLK_SIZE:
        *(uint32_t *)dat = (uint32_t)(1 << p_nand->phys_erase_shift);
        break;

    case AWBL_NAND_OOB_SIZE:
        *(uint32_t *)dat = (uint32_t)(p_nand->oob_layout.oob_avail);
        break;

    case AWBL_NAND_BLK_NUM:
        *(uint32_t *)dat = (uint32_t)(
                          (p_nand->p_info->attr.chip_size) /
                          (1 << p_nand->phys_erase_shift));
        break;

    case AWBL_NAND_PAGE_NUM_PER_BLK:
        *(uint32_t *)dat = (uint32_t)(
                           (1 << p_nand->phys_erase_shift) /
                           (1 << p_nand->page_shift));
        break;

    case AWBL_NAND_CHIP_SIZE:
        *(uint32_t *)dat = (uint32_t)(p_nand->p_info->attr.chip_size);
        break;

    default:
        __LOG_PFX(1, "not support the request(%d).", req);
        ret = -AW_ENOTSUP;
    }

    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

aw_err_t awbl_nand_read (awbl_nand_t           *p_nand,
                         uint32_t               page,
                         uint32_t               col,
                         uint8_t               *buf,
                         int                    len)
{
    aw_err_t ret;
    awbl_nand_platform_serv_t *p_pf_serv = p_nand->p_pf_serv;
    aw_assert(p_nand->p_pf_serv->read_buf);

    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);
    ret = p_pf_serv->read_buf(p_pf_serv, page, col, buf, len, NULL, 0, 0);
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

aw_err_t awbl_nand_write (awbl_nand_t           *p_nand,
                          uint32_t               page,
                          uint32_t               col,
                          uint8_t               *buf,
                          int                    len)
{
    aw_err_t                     ret;
    awbl_nand_platform_serv_t   *p_pf_serv = p_nand->p_pf_serv;
    aw_assert(p_nand->p_pf_serv->write_buf);
    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);
    ret = p_pf_serv->write_buf(p_pf_serv, page, col, buf, len, NULL, 0, 0);
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

aw_err_t awbl_nand_page_read (awbl_nand_t           *p_nand,
                              uint8_t               *buf,
                              uint32_t               ipage)
{
    aw_err_t ret;
    aw_assert(p_nand->p_ecc->itf.read_page);

    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);
    ret = p_nand->p_ecc->itf.read_page(p_nand->p_ecc,
                                       buf,
                                       NULL,
                                       0,
                                       AW_FALSE,
                                       ipage);
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

aw_err_t awbl_nand_page_write (awbl_nand_t           *p_nand,
                               uint8_t               *buf,
                               uint32_t               ipage)
{
    aw_err_t ret;
    aw_assert(p_nand->p_ecc->itf.write_page);

    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);
    ret = p_nand->p_ecc->itf.write_page(p_nand->p_ecc,
                                        buf,
                                        NULL,
                                        0,
                                        AW_FALSE,
                                        ipage);
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

aw_err_t awbl_nand_oob_write (awbl_nand_t              *p_nand,
                              uint32_t                  ipage,
                              uint32_t                  offs,
                              uint8_t                  *oob_data,
                              uint16_t                  data_len)
{
    aw_err_t         ret;
    awbl_nand_ecc_t *p_ecc = p_nand->p_ecc;
    aw_assert(p_nand->p_ecc->itf.write_oob_free);

    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);
    ret = p_ecc->itf.write_oob_free(p_ecc, ipage, offs, oob_data, data_len);
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

aw_err_t awbl_nand_oob_read (awbl_nand_t              *p_nand,
                             uint32_t                  ipage,
                             uint32_t                  offs,
                             uint8_t                  *oob_data,
                             uint16_t                  data_len)
{
    aw_err_t         ret;
    awbl_nand_ecc_t *p_ecc = p_nand->p_ecc;
    aw_assert(p_nand->p_ecc->itf.read_oob_free);
    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);
    ret = p_ecc->itf.read_oob_free(p_ecc, ipage, offs, oob_data, data_len);
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

aw_err_t awbl_nand_blk_erase (awbl_nand_t *p_nand,
                              uint32_t               iblk)
{
    aw_err_t ret;
    aw_assert(p_nand->p_pf_serv->erase);
    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);
    ret = p_nand->p_pf_serv->erase(p_nand->p_pf_serv,
                                   iblk * p_nand->mtd.erase_size /
                                       p_nand->mtd.write_size);
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

aw_bool_t awbl_nand_blk_is_bad (awbl_nand_t *p_nand,
                                uint32_t               iblk)
{
    aw_err_t ret;
    aw_assert(p_nand->p_pf_serv->block_is_bad);
    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);
    ret = p_nand->p_pf_serv->block_is_bad(p_nand->p_pf_serv,
                                          iblk * p_nand->mtd.erase_size /
                                              p_nand->mtd.write_size);
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

aw_err_t awbl_nand_blk_markbad (awbl_nand_t *p_nand,
                                uint32_t               iblk)
{
    aw_err_t ret;
    aw_assert(p_nand->p_pf_serv->block_markbad);
    ret = AW_MUTEX_LOCK(p_nand->op_mutex, aw_ms_to_ticks(5000));
    aw_assert(ret == AW_OK);

    ret = p_nand->p_pf_serv->block_markbad(p_nand->p_pf_serv,
                                           iblk * p_nand->mtd.erase_size /
                                               p_nand->mtd.write_size);
    AW_MUTEX_UNLOCK(p_nand->op_mutex);
    return ret;
}

awbl_nand_t *awbl_mtd_get_nand (struct aw_mtd_info *mtd)
{
    return mtd->data;
}

awbl_nand_t *awbl_mtd_set_nand (struct aw_mtd_info *mtd, void *p_nand)
{
    return mtd->data = p_nand;
}

void *awbl_nand_get_data (awbl_nand_t *p_nand)
{
    return p_nand->priv;
}

void awbl_nand_set_data (awbl_nand_t *p_nand, void *priv)
{
    p_nand->priv = priv;
}

aw_bool_t awbl_nand_is_slc (awbl_nand_t *p_nand)
{
    return p_nand->p_info->attr.bits_per_cell == 1;
}

/******************************************************************************/

aw_local void __log_cb (void         *p_color,
                        const char   *func_name,
                        int           line,
                        const char   *fmt, ...)
{
    char        log_buf[256 + 1];
    uint16_t    len;
    va_list     ap;

    va_start( ap, fmt );
    len = aw_vsnprintf((char *)log_buf, sizeof(log_buf), fmt, ap);
    va_end( ap );
    if (len >= sizeof(log_buf)) {
        aw_assert(len < sizeof(log_buf) - 1);
    }

#ifdef __VIH_DEBUG
    aw_shell_printf_with_color(aw_default_shell_get(),
                               p_color,
                               func_name,
                               log_buf);
#else
    AW_INFOF((log_buf));
#endif
}
/******************************************************************************/

void awbl_nand_init (awbl_nand_init_t *p_init, awbl_nand_t *p_nand)
{
    __LOG_DEV_DEF(NULL);
    awbl_nand_info_t        *p_info = NULL;

    __ASSERT_D(p_init, !=, NULL);
    __ASSERT_D(p_init->p_info, !=, NULL);
    __ASSERT_D(p_init->p_pf_serv, !=, NULL);
    __ASSERT_D(p_nand, !=, NULL);
    p_nand->init = *p_init;
    p_info = p_init->p_info;

    AW_MUTEX_INIT(p_nand->op_mutex, AW_SEM_Q_FIFO);
    p_nand->p_info    = p_info;
    p_nand->p_ecc     = p_init->p_ecc;
    p_nand->p_pf_serv = p_init->p_pf_serv;

    /*
     * 初始化 oob 的部分参数
     */
    {
        uint32_t                 temp = 0;
        int                      i;

        p_nand->oob_layout.bbm_pos   = p_info->attr.bad_blk_byte_offs;
        p_nand->oob_layout.ecc_pos   = p_info->p_nand_ooblayout->ptr_eccpos;
        p_nand->oob_layout.oob_free  = p_info->p_nand_ooblayout->ptr_freepos;

        /* 计算ecc字节 */
        for (temp = 0, i = 0; i < p_info->p_nand_ooblayout->cnt_eccpos; i++) {
            temp += p_info->p_nand_ooblayout->ptr_eccpos[i].length;
        }
        p_nand->oob_layout.ecc_bytes = (uint8_t)temp;

        /* 计算oob可用字节 */
        for (temp = 0, i = 0; i < p_info->p_nand_ooblayout->cnt_freepos; i++) {
            temp += p_info->p_nand_ooblayout->ptr_freepos[i].length;
        }
        p_nand->oob_layout.oob_avail = (uint8_t)temp;
    }

    /* 初始化 buf */
    {
        p_nand->page_buf = malloc(p_info->attr.page_size);
        p_nand->oob_buf  = malloc(p_info->attr.spare_size);
        aw_assert(p_nand->page_buf && p_nand->oob_buf);
    }

    /* 初始化 mtd 的部分参数 */
    {
        struct aw_mtd_info      *p_mtd  = 0;

        p_nand->mtd_part_cnt         = p_info->npartition;

        /* 有多少个分区就申请多少个mtd大小的内存 */
        if (p_nand->mtd_part_cnt > 0) {
            p_nand->p_mtd_part =
                    (struct aw_mtd_part_info *)__mem_alloc(
                         sizeof(struct aw_mtd_part_info) * p_info->npartition);

            __ASSERT(p_nand->p_mtd_part, "mtd memory allocate failed.");
        } else {
            p_nand->p_mtd_part = NULL;
        }

        p_mtd                    = &p_nand->mtd;
        p_mtd->type              = AW_MTD_TYPE_NAND_FLASH;
        p_mtd->flags             = AW_MTD_FLAGS_NAND_FLASH;
        p_mtd->oob_size          = p_info->attr.spare_size;
        p_mtd->oob_avail         = p_nand->oob_layout.oob_avail;
        p_mtd->erase_size        = p_info->attr.blk_size;
        p_mtd->write_size        = p_info->attr.page_size;
        p_mtd->write_buf_size    = p_info->attr.page_size;
        p_mtd->size              = p_info->attr.chip_size;
    }

    {
        struct aw_mtd_info          *mtd = &p_nand->mtd;
        p_nand->phys_erase_shift = __nand_ffs(mtd->erase_size) - 1;
        p_nand->page_shift       = __nand_ffs(mtd->write_size) - 1;
        p_nand->erase_align_mask = (~0) % mtd->erase_size;
    /*    p_nand->pagemask       = (p_nand->chipsize >> p_nand->page_shift) - 1; */
        p_nand->pagemask         = (1 << p_nand->page_shift) - 1;
    }

    {
        struct aw_mtd_info          *mtd = &p_nand->mtd;
        int                          i;
        int                          offset;
        int                          size;
        int                          ret;

        /* Fill in remaining MTD driver data */
        mtd->ecc_strength       = p_nand->p_info->ecc_strength;
        mtd->bitflip_threshold  = p_nand->p_info->ecc_strength;
        mtd->oob_layout         =
                (const struct aw_mtd_oob_layout *)&p_nand->oob_layout;
        mtd->ops                = &__g_mtd_ops;
        mtd->data               = (void *)p_nand;

        for (i = 0; i < p_nand->p_info->npartition; i++) {
            offset = p_nand->p_info->p_partition_tab[i].start_blk_num *
                         mtd->erase_size +
                     p_nand->p_info->p_partition_tab[i].page_offset *
                         mtd->write_buf_size;

            size   = (p_nand->p_info->p_partition_tab[i].end_blk_num -
                    p_nand->p_info->p_partition_tab[i].start_blk_num + 1) *
                            mtd->erase_size -
                    (p_nand->p_info->p_partition_tab[i].page_offset *
                            mtd->write_buf_size);

            __LOG_PFX(__LOG_MTD,
                     "add mtd_part dev(%s: blk_offs:%d,blk_num:%d).",
                      p_nand->p_info->p_partition_tab[i].name,
                      offset/mtd->erase_size,
                      size/mtd->erase_size);
            ret = aw_mtd_part_add(&p_nand->p_mtd_part[i],
                                  mtd,
                                  p_nand->p_info->p_partition_tab[i].name,
                                  offset,
                                  size,
                                  p_nand);

            __ASSERT_D(ret, ==, AW_OK);
            if (ret != AW_OK) {
                goto __exit;
            }
        }
    }
    return;

__exit:
    __nand_mem_free(p_nand);
}

/******************************************************************************/
void awbl_nand_uninit (awbl_nand_t *p_nand)
{
    int          i;
    aw_err_t     ret;
    aw_assert(p_nand);
    for (i = 0; i < p_nand->p_info->npartition; i++) {
        ret = aw_mtd_part_del(&p_nand->p_mtd_part[i]);
        aw_assert(ret == AW_OK);
    }
    __nand_mem_free(p_nand);
}


/******************************************************************************/
aw_err_t awbl_nand_platform_serv_register (
        awbl_nand_platform_serv_t *(*pfn_platform_serv_create) (
                awbl_nand_info_t    *,
                struct awbl_nandbus *),
        void (*pfn_platform_serv_destroy) (awbl_nand_platform_serv_t *))
{
    aw_assert(pfn_platform_serv_create);
    aw_assert(pfn_platform_serv_destroy);

    __g_platform_serv_reg.pfn_platform_serv_create  = pfn_platform_serv_create;
    __g_platform_serv_reg.pfn_platform_serv_destroy = pfn_platform_serv_destroy;
    return AW_OK;
}

awbl_nand_platform_serv_t *awbl_nand_platform_serv_create (
        awbl_nand_info_t    *p_info,
        struct awbl_nandbus *p_bus)
{
    awbl_nand_platform_serv_t *p_pf_serv;
    aw_assert(p_bus);
    if (__g_platform_serv_reg.pfn_platform_serv_create) {
        p_pf_serv = __g_platform_serv_reg.pfn_platform_serv_create(p_info,
                                                                   p_bus);
    } else {
        /* 默认处理 */
        p_pf_serv                 = malloc(sizeof(*p_pf_serv));
        aw_assert(p_pf_serv);
        memset(p_pf_serv, 0, sizeof(*p_pf_serv));

        awbl_nand_serv_default_set(p_pf_serv);
        p_pf_serv->p_info = p_info;
        p_pf_serv->p_bus  = p_bus;
    }
    return p_pf_serv;
}

void awbl_nand_platform_serv_destroy (awbl_nand_platform_serv_t *p_this)
{
    if (__g_platform_serv_reg.pfn_platform_serv_destroy) {
        __g_platform_serv_reg.pfn_platform_serv_destroy(p_this);
    } else {
        aw_assert(p_this);
        free(p_this);
    }
}


/******************************************************************************/
#define __NAND_DEV_DECL(p_nand, p_awdev) \
        awbl_nand_t *p_nand = \
            AW_CONTAINER_OF(p_awdev, awbl_nand_t, awdev)

#define __NAND_DEVINFO_ARR_DECL(p_info_arr, p_awdev) \
        awbl_nand_info_t **p_info_arr = \
            (awbl_nand_info_t **)AWBL_DEVINFO_GET(p_awdev)


/* tag init */

aw_local void __nandflash_inst_connect (struct awbl_dev *p_awdev)
{
    __NAND_DEV_DECL(                 p_nand, p_awdev);
    __NAND_DEVINFO_ARR_DECL(         p_info_arr, p_awdev);
    __LOG_DEV_DEF(NULL);
    int                              i;
    struct awbl_nandbus             *p_bus;
    awbl_nand_info_t                *p_info;
    awbl_nand_ecc_t                 *p_ecc;
    awbl_nand_platform_serv_t       *p_pf_serv;

    /* 获取 bus */
    p_bus  = (struct awbl_nandbus  *)p_nand->awdev.p_parentbus->p_ctlr;

    for (i = 0; p_info_arr[i] != NULL; i++) {
        p_info = p_info_arr[i];

        /* 平台初始化，提供对 nandflash 的直接读写接口  */
        p_pf_serv = awbl_nand_platform_serv_create(p_info, p_bus);
        __ASSERT(p_pf_serv, "");

        /* 测试设备 */
        if (p_pf_serv->probe(p_pf_serv) == AW_TRUE) {
            break;
        }
        awbl_nand_platform_serv_destroy(p_pf_serv);
        p_pf_serv = NULL;
    }
    __ASSERT(p_pf_serv != NULL, "nandflash probe failed.!!!");

    /* ecc 初始化 */
    {
        awbl_nand_ecc_init_t init;
        memset(&init, 0, sizeof(init));

        init.p_info     = p_info;
        init.p_pf_serv  = p_pf_serv;
        p_ecc = awbl_nand_ecc_create(&init);
        __ASSERT(p_ecc, "");
    }

    /* nand 初始化 */
    {
        awbl_nand_init_t   init;
        memset(&init, 0, sizeof(init));
        init.p_info     = p_info;
        init.pfn_log    = __log_cb;
        init.p_ecc      = p_ecc;
        init.p_pf_serv  = p_pf_serv;

        awbl_nand_init(&init, p_nand);
    }
}
/******************************************************************************/
/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_nandflash_drvfuncs = {
        NULL,
        __nandflash_inst_connect,
        NULL
};

/******************************************************************************/
/* driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_nandflash_drv_registration = {
    AWBL_VER_1,                             /* awb_ver */
    AWBL_BUSID_NAND,                        /* bus_id */
    AWBL_NAND_DRV_NAME,                     /* p_drvname */
    &__g_nandflash_drvfuncs,                /* p_busfuncs */
    NULL,                                   /* p_methods */
    NULL                                    /* pfunc_drv_probe */
};

void awbl_nand_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register((struct awbl_drvinfo *)&__g_nandflash_drv_registration);
    aw_assert(err == AW_OK);
    return;
}


/* end of file */
