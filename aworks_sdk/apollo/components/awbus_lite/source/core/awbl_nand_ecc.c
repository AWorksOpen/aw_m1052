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
 * \brief nand controler software ecc driver source file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-3-22  vih, first implementation
 * - 2.00 20-04-14 vih, refactoring
 * \endinternal
 */

#include "aworks.h"
#include "aw_soft_bch.h"
#include "awbl_nand_ecc.h"
#include "aw_vdebug.h"
#include "aw_assert.h"
#include "string.h"
#include "aw_mem.h"


/*******************************************************************************
  macro operate
*******************************************************************************/
#define __LOG_DEV_DEF(p_dev)

#define __LOG_FCOLOR_RED        "\x1B[31m"
#define __LOG_FCOLOR_GREEN      "\x1B[32m"

#define __LOG(mask, color, fmt, ...)   \
    if ((mask)) {\
         __log(color, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);\
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
/**
 * \brief NandFlash BCH控制结构体
 */
struct nand_bch_control {

    /** \brief BCH控制结构 */
    struct aw_soft_bch_ctrl *bch;

    /** \brief 错误定位数组 */
    unsigned int            *errloc;

    /** \brief 异或ECC掩码，使得那些已经擦除的页在decode时也是合法的 */
    unsigned char           *eccmask;
};
/******************************************************************************/
typedef int (*__ecc_calculate_t) (awbl_nand_ecc_t         *p_this,
                                  const  uint8_t          *p_buf,
                                  uint8_t                 *ecc_buf);
typedef int (*__ecc_correct_t) (awbl_nand_ecc_t           *p_this,
                                uint8_t                    *dat,
                                uint8_t                    *read_ecc,
                                uint8_t                    *calc_ecc);

/******************************************************************************/

typedef struct __nand_ecc_reg{
    struct __nand_ecc_reg     *p_next;
    enum awbl_nand_ecc_mode    ecc_mode;
    awbl_nand_ecc_t         *(*pfn_custom_create) (awbl_nand_ecc_init_t *p);
} __nand_ecc_reg_t;

aw_local __nand_ecc_reg_t *__gp_reg_list = NULL;


/******************************************************************************/
static void __log (void         *p_color,
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
aw_local void __oob_free_get (awbl_nand_info_t  *p_info,
                              uint8_t           *oob_buf_src,
                              uint16_t           free_ofs,
                              uint8_t           *oob_free_buf,
                              uint16_t           free_buf_size)
{
    const struct aw_mtd_oob_space *free = p_info->p_nand_ooblayout->ptr_freepos;
    uint32_t                boffs = 0;
    size_t                  bytes = 0;
    int                     free_cnt = p_info->p_nand_ooblayout->cnt_freepos;
    uint16_t                len   = free_buf_size;

    memset(oob_free_buf, 0xFF, free_buf_size);
    /* 提取free区的数据 */
    for (; free->length && len && free_cnt; free++, len -= bytes, free_cnt--) {
        /* Read request not from offset 0? */
        if (free_ofs) {
            if (free_ofs >= free->length) {
                free_ofs -= free->length;
                continue;
            }
            boffs = free->offset + free_ofs;
            bytes = min(len, (free->length - free_ofs));
            free_ofs = 0;
        } else {
            bytes = min(len, free->length);
            boffs = free->offset;
        }
        memcpy(oob_free_buf, oob_buf_src + boffs, bytes);
        oob_free_buf += bytes;
    }
    __ASSERT_D(len, ==, 0);
}

aw_local void __oob_free_put (awbl_nand_info_t  *p_info,
                              uint8_t           *oob_free_src,
                              uint16_t           free_size,
                              uint16_t           free_ofs,
                              uint8_t           *oob_buf)
{
    const struct aw_mtd_oob_space *free = p_info->p_nand_ooblayout->ptr_freepos;
    uint32_t boffs = 0;
    uint32_t woffs = free_ofs;  /* oob_buf 的存放的起始偏移 */
    size_t   bytes = 0;
    int      free_cnt = p_info->p_nand_ooblayout->cnt_freepos;
    uint16_t len = free_size;

    for (; free->length && len && free_cnt; free++, len -= bytes, free_cnt--) {
        /* Write request not from offset 0? */
        if (unlikely(woffs)) {
            if (woffs >= free->length) {
                woffs -= free->length;
                continue;
            }
            boffs = free->offset + woffs;
            bytes = min(len, (free->length - woffs));
            woffs = 0;
        } else {
            bytes = min(len, free->length);
            boffs = free->offset;
        }
        memcpy(oob_buf + boffs, oob_free_src, bytes);
        oob_free_src += bytes;
    }
    __ASSERT_D(len, ==, 0);
}

aw_local void __oob_ecc_put (awbl_nand_info_t  *p_info,
                             uint8_t           *oob_ecc_src,
                             uint16_t           ecc_size,
                             uint8_t           *oob_buf)
{
    int  i = 0, offs = 0, neccs = 0, total = 0;

    total = ecc_size;
    for (i = 0; i < p_info->p_nand_ooblayout->cnt_eccpos; i++) {
        offs = p_info->p_nand_ooblayout->ptr_eccpos[i].offset;

        if (total > p_info->p_nand_ooblayout->ptr_eccpos[i].length) {
            memcpy(oob_buf + offs,
                   oob_ecc_src + neccs,
                   p_info->p_nand_ooblayout->ptr_eccpos[i].length);

            total -= p_info->p_nand_ooblayout->ptr_eccpos[i].length;
        } else {
            memcpy(oob_buf + offs,
                   oob_ecc_src + neccs,
                   total);

            total = 0;
            break;
        }

        neccs += p_info->p_nand_ooblayout->ptr_eccpos[i].length;
        __ASSERT_D(neccs, <=, ecc_size);
    }
}
aw_local void __oob_ecc_get (awbl_nand_info_t  *p_info,
                             uint8_t           *oob_buf_src,
                             uint8_t           *oob_ecc_buf,
                             uint16_t           ecc_size)
{
    int i = 0, offs = 0, neccs = 0;

    memset(oob_ecc_buf, 0xFF, ecc_size);
    for (i = 0; i < p_info->p_nand_ooblayout->cnt_eccpos; i++) {
        offs = p_info->p_nand_ooblayout->ptr_eccpos[i].offset;

        memcpy(oob_ecc_buf + neccs,
               oob_buf_src + offs,
               p_info->p_nand_ooblayout->ptr_eccpos[i].length);

        neccs += p_info->p_nand_ooblayout->ptr_eccpos[i].length;
        __ASSERT_D(neccs, <=, ecc_size);
    }
}
/******************************************************************************/
/*
 *  读取整个 oob 区(大小为oob_size)，
 */
aw_local int __platform_oob_read (awbl_nand_ecc_t    *p_this,
                                uint32_t              page,
                                uint8_t              *oob_buf,
                                uint16_t              oob_buf_size)
{
    aw_assert(p_this->p_pf_serv->read_buf);
    aw_assert(oob_buf);
    __ASSERT_D(oob_buf_size, >=, p_this->p_info->attr.spare_size);
    return p_this->p_pf_serv->read_buf(p_this->p_pf_serv,
                                       page,
                                       p_this->p_info->attr.page_size,
                                       oob_buf,
                                       oob_buf_size,
                                       NULL,
                                       0,
                                       AW_FALSE);
}

/*
 *  写整个 oob 区(大小为oob_size)，
 */
aw_local int __platform_oob_write (awbl_nand_ecc_t      *p_this,
                                   uint8_t              *oob_buf,
                                   uint16_t              oob_buf_size,
                                   uint32_t              page)
{
    aw_assert(p_this->p_pf_serv->write_buf);
    aw_assert(oob_buf);
    __ASSERT_D(oob_buf_size, >=, p_this->p_info->attr.spare_size);
    return p_this->p_pf_serv->write_buf(p_this->p_pf_serv,
                                        page,
                                        p_this->p_info->attr.page_size,
                                        oob_buf,
                                        oob_buf_size,
                                        NULL,
                                        0,
                                        AW_FALSE);
}
/******************************************************************************
 HAM
******************************************************************************/


/**
 * \brief
 * invparity is a 256 byte table that contains the odd parity
 * for each byte. So if the number of bits in a byte is even,
 * the array element is 1, and when the number of bits is odd
 * the array eleemnt is 0.
 */
static const char invparity[256] = {
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};

/**
 * \brief
 * bitsperbyte contains the number of bits per byte
 * this is only used for testing and repairing parity
 * (a precalculated value slightly improves performance)
 */
static const char bitsperbyte[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
};

/**
 * \brief
 * addressbits is a lookup table to filter out the bits from the xor-ed
 * ECC data that identify the faulty location.
 * this is only used for repairing parity
 * see the comments in nand_correct_data for more details
 */
static const char addressbits[256] = {
    0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01,
    0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03,
    0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01,
    0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03,
    0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x05,
    0x06, 0x06, 0x07, 0x07, 0x06, 0x06, 0x07, 0x07,
    0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x05,
    0x06, 0x06, 0x07, 0x07, 0x06, 0x06, 0x07, 0x07,
    0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01,
    0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03,
    0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01,
    0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03,
    0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x05,
    0x06, 0x06, 0x07, 0x07, 0x06, 0x06, 0x07, 0x07,
    0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x05,
    0x06, 0x06, 0x07, 0x07, 0x06, 0x06, 0x07, 0x07,
    0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09,
    0x0a, 0x0a, 0x0b, 0x0b, 0x0a, 0x0a, 0x0b, 0x0b,
    0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09,
    0x0a, 0x0a, 0x0b, 0x0b, 0x0a, 0x0a, 0x0b, 0x0b,
    0x0c, 0x0c, 0x0d, 0x0d, 0x0c, 0x0c, 0x0d, 0x0d,
    0x0e, 0x0e, 0x0f, 0x0f, 0x0e, 0x0e, 0x0f, 0x0f,
    0x0c, 0x0c, 0x0d, 0x0d, 0x0c, 0x0c, 0x0d, 0x0d,
    0x0e, 0x0e, 0x0f, 0x0f, 0x0e, 0x0e, 0x0f, 0x0f,
    0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09,
    0x0a, 0x0a, 0x0b, 0x0b, 0x0a, 0x0a, 0x0b, 0x0b,
    0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09,
    0x0a, 0x0a, 0x0b, 0x0b, 0x0a, 0x0a, 0x0b, 0x0b,
    0x0c, 0x0c, 0x0d, 0x0d, 0x0c, 0x0c, 0x0d, 0x0d,
    0x0e, 0x0e, 0x0f, 0x0f, 0x0e, 0x0e, 0x0f, 0x0f,
    0x0c, 0x0c, 0x0d, 0x0d, 0x0c, 0x0c, 0x0d, 0x0d,
    0x0e, 0x0e, 0x0f, 0x0f, 0x0e, 0x0e, 0x0f, 0x0f
};


/**
 * \brief ECC计算
 */
aw_local int __ham_ecc_calculate (awbl_nand_ecc_t            *p_this,
                                  const  uint8_t             *buf,
                                  uint8_t                    *code)
{
    int i;
    unsigned int   eccsize = p_this->size;
    const uint32_t *bp = (uint32_t *)buf;
    /* 256 or 512 bytes/ecc  */
    const uint32_t eccsize_mult = eccsize >> 8;
    uint32_t cur;       /* current value in buffer */
    /* rp0..rp15..rp17 are the various accumulated parities (per byte) */
    uint32_t rp0, rp1, rp2, rp3, rp4, rp5, rp6, rp7;
    uint32_t rp8, rp9, rp10, rp11, rp12, rp13, rp14, rp15, rp16;
    uint32_t rp17;      /* to make compiler happy */
    uint32_t par;       /* the cumulative parity for all data */
    uint32_t tmppar;    /* the cumulative parity for this iteration;
                         * for rp12, rp14 and rp16 at the end of the loop
                         */

    par = 0;
    rp4 = 0;
    rp6 = 0;
    rp8 = 0;
    rp10 = 0;
    rp12 = 0;
    rp14 = 0;
    rp16 = 0;

    /*
     * The loop is unrolled a number of times;
     * This avoids if statements to decide on which rp value to update
     * Also we process the data by longwords.
     * Note: passing unaligned data might give a performance penalty.
     * It is assumed that the buffers are aligned.
     * tmppar is the cumulative sum of this iteration.
     * needed for calculating rp12, rp14, rp16 and par
     * also used as a performance improvement for rp6, rp8 and rp10
     */
    for (i = 0; i < eccsize_mult << 2; i++) {
        cur = *bp++;
        tmppar = cur;
        rp4 ^= cur;
        cur = *bp++;
        tmppar ^= cur;
        rp6 ^= tmppar;
        cur = *bp++;
        tmppar ^= cur;
        rp4 ^= cur;
        cur = *bp++;
        tmppar ^= cur;
        rp8 ^= tmppar;

        cur = *bp++;
        tmppar ^= cur;
        rp4 ^= cur;
        rp6 ^= cur;
        cur = *bp++;
        tmppar ^= cur;
        rp6 ^= cur;
        cur = *bp++;
        tmppar ^= cur;
        rp4 ^= cur;
        cur = *bp++;
        tmppar ^= cur;
        rp10 ^= tmppar;

        cur = *bp++;
        tmppar ^= cur;
        rp4 ^= cur;
        rp6 ^= cur;
        rp8 ^= cur;
        cur = *bp++;
        tmppar ^= cur;
        rp6 ^= cur;
        rp8 ^= cur;
        cur = *bp++;
        tmppar ^= cur;
        rp4 ^= cur;
        rp8 ^= cur;
        cur = *bp++;
        tmppar ^= cur;
        rp8 ^= cur;

        cur = *bp++;
        tmppar ^= cur;
        rp4 ^= cur;
        rp6 ^= cur;
        cur = *bp++;
        tmppar ^= cur;
        rp6 ^= cur;
        cur = *bp++;
        tmppar ^= cur;
        rp4 ^= cur;
        cur = *bp++;
        tmppar ^= cur;

        par ^= tmppar;
        if ((i & 0x1) == 0)
            rp12 ^= tmppar;
        if ((i & 0x2) == 0)
            rp14 ^= tmppar;
        if (eccsize_mult == 2 && (i & 0x4) == 0)
            rp16 ^= tmppar;
    }

    /*
     * handle the fact that we use longword operations
     * we'll bring rp4..rp14..rp16 back to single byte entities by
     * shifting and xoring first fold the upper and lower 16 bits,
     * then the upper and lower 8 bits.
     */
    rp4 ^= (rp4 >> 16);
    rp4 ^= (rp4 >> 8);
    rp4 &= 0xff;
    rp6 ^= (rp6 >> 16);
    rp6 ^= (rp6 >> 8);
    rp6 &= 0xff;
    rp8 ^= (rp8 >> 16);
    rp8 ^= (rp8 >> 8);
    rp8 &= 0xff;
    rp10 ^= (rp10 >> 16);
    rp10 ^= (rp10 >> 8);
    rp10 &= 0xff;
    rp12 ^= (rp12 >> 16);
    rp12 ^= (rp12 >> 8);
    rp12 &= 0xff;
    rp14 ^= (rp14 >> 16);
    rp14 ^= (rp14 >> 8);
    rp14 &= 0xff;
    if (eccsize_mult == 2) {
        rp16 ^= (rp16 >> 16);
        rp16 ^= (rp16 >> 8);
        rp16 &= 0xff;
    }

    /*
     * we also need to calculate the row parity for rp0..rp3
     * This is present in par, because par is now
     * rp3 rp3 rp2 rp2 in little endian and
     * rp2 rp2 rp3 rp3 in big endian
     * as well as
     * rp1 rp0 rp1 rp0 in little endian and
     * rp0 rp1 rp0 rp1 in big endian
     * First calculate rp2 and rp3
     */
#ifdef __BIG_ENDIAN
    rp2 = (par >> 16);
    rp2 ^= (rp2 >> 8);
    rp2 &= 0xff;
    rp3 = par & 0xffff;
    rp3 ^= (rp3 >> 8);
    rp3 &= 0xff;
#else
    rp3 = (par >> 16);
    rp3 ^= (rp3 >> 8);
    rp3 &= 0xff;
    rp2 = par & 0xffff;
    rp2 ^= (rp2 >> 8);
    rp2 &= 0xff;
#endif

    /* reduce par to 16 bits then calculate rp1 and rp0 */
    par ^= (par >> 16);
#ifdef __BIG_ENDIAN
    rp0 = (par >> 8) & 0xff;
    rp1 = (par & 0xff);
#else
    rp1 = (par >> 8) & 0xff;
    rp0 = (par & 0xff);
#endif

    /* finally reduce par to 8 bits */
    par ^= (par >> 8);
    par &= 0xff;

    /*
     * and calculate rp5..rp15..rp17
     * note that par = rp4 ^ rp5 and due to the commutative property
     * of the ^ operator we can say:
     * rp5 = (par ^ rp4);
     * The & 0xff seems superfluous, but benchmarking learned that
     * leaving it out gives slightly worse results. No idea why, probably
     * it has to do with the way the pipeline in pentium is organized.
     */
    rp5 = (par ^ rp4) & 0xff;
    rp7 = (par ^ rp6) & 0xff;
    rp9 = (par ^ rp8) & 0xff;
    rp11 = (par ^ rp10) & 0xff;
    rp13 = (par ^ rp12) & 0xff;
    rp15 = (par ^ rp14) & 0xff;
    if (eccsize_mult == 2)
        rp17 = (par ^ rp16) & 0xff;

    /*
     * Finally calculate the ECC bits.
     * Again here it might seem that there are performance optimisations
     * possible, but benchmarks showed that on the system this is developed
     * the code below is the fastest
     */
#ifdef CONFIG_MTD_NAND_ECC_SMC
    code[0] =
        (invparity[rp7] << 7) |
        (invparity[rp6] << 6) |
        (invparity[rp5] << 5) |
        (invparity[rp4] << 4) |
        (invparity[rp3] << 3) |
        (invparity[rp2] << 2) |
        (invparity[rp1] << 1) |
        (invparity[rp0]);
    code[1] =
        (invparity[rp15] << 7) |
        (invparity[rp14] << 6) |
        (invparity[rp13] << 5) |
        (invparity[rp12] << 4) |
        (invparity[rp11] << 3) |
        (invparity[rp10] << 2) |
        (invparity[rp9] << 1)  |
        (invparity[rp8]);
#else
    code[1] =
        (invparity[rp7] << 7) |
        (invparity[rp6] << 6) |
        (invparity[rp5] << 5) |
        (invparity[rp4] << 4) |
        (invparity[rp3] << 3) |
        (invparity[rp2] << 2) |
        (invparity[rp1] << 1) |
        (invparity[rp0]);
    code[0] =
        (invparity[rp15] << 7) |
        (invparity[rp14] << 6) |
        (invparity[rp13] << 5) |
        (invparity[rp12] << 4) |
        (invparity[rp11] << 3) |
        (invparity[rp10] << 2) |
        (invparity[rp9] << 1)  |
        (invparity[rp8]);
#endif
    if (eccsize_mult == 1)
        code[2] =
            (invparity[par & 0xf0] << 7) |
            (invparity[par & 0x0f] << 6) |
            (invparity[par & 0xcc] << 5) |
            (invparity[par & 0x33] << 4) |
            (invparity[par & 0xaa] << 3) |
            (invparity[par & 0x55] << 2) |
            3;
    else
        code[2] =
            (invparity[par & 0xf0] << 7) |
            (invparity[par & 0x0f] << 6) |
            (invparity[par & 0xcc] << 5) |
            (invparity[par & 0x33] << 4) |
            (invparity[par & 0xaa] << 3) |
            (invparity[par & 0x55] << 2) |
            (invparity[rp17] << 1) |
            (invparity[rp16] << 0);

    return 0;
}



/**
 * 校验数据(检测并纠正1位错误)
 */
aw_local int __ham_ecc_correct (awbl_nand_ecc_t                  *p_this,
                                uint8_t                          *buf,
                                uint8_t                          *read_ecc,
                                uint8_t                          *calc_ecc)
{
    unsigned int  eccsize = p_this->size;
    unsigned char b0, b1, b2, bit_addr;
    unsigned int byte_addr;
    /* 256 or 512 bytes/ecc  */
    const uint32_t eccsize_mult = eccsize >> 8;

    /*
     * b0 to b2 indicate which bit is faulty (if any)
     * we might need the xor result  more than once,
     * so keep them in a local var
    */
#ifdef CONFIG_MTD_NAND_ECC_SMC
    b0 = read_ecc[0] ^ calc_ecc[0];
    b1 = read_ecc[1] ^ calc_ecc[1];
#else
    b0 = read_ecc[1] ^ calc_ecc[1];
    b1 = read_ecc[0] ^ calc_ecc[0];
#endif
    b2 = read_ecc[2] ^ calc_ecc[2];

    /* check if there are any bitfaults */

    /* repeated if statements are slightly more efficient than switch ... */
    /* ordered in order of likelihood */

    if ((b0 | b1 | b2) == 0)
        return 0;   /* no error */

    if ((((b0 ^ (b0 >> 1)) & 0x55) == 0x55) &&
        (((b1 ^ (b1 >> 1)) & 0x55) == 0x55) &&
        ((eccsize_mult == 1 && ((b2 ^ (b2 >> 1)) & 0x54) == 0x54) ||
         (eccsize_mult == 2 && ((b2 ^ (b2 >> 1)) & 0x55) == 0x55))) {
    /* single bit error */
        /*
         * rp17/rp15/13/11/9/7/5/3/1 indicate which byte is the faulty
         * byte, cp 5/3/1 indicate the faulty bit.
         * A lookup table (called addressbits) is used to filter
         * the bits from the byte they are in.
         * A marginal optimisation is possible by having three
         * different lookup tables.
         * One as we have now (for b0), one for b2
         * (that would avoid the >> 1), and one for b1 (with all values
         * << 4). However it was felt that introducing two more tables
         * hardly justify the gain.
         *
         * The b2 shift is there to get rid of the lowest two bits.
         * We could also do addressbits[b2] >> 1 but for the
         * performance it does not make any difference
         */
        if (eccsize_mult == 1)
            byte_addr = (addressbits[b1] << 4) + addressbits[b0];
        else
            byte_addr = (addressbits[b2 & 0x3] << 8) +
                    (addressbits[b1] << 4) + addressbits[b0];
        bit_addr = addressbits[b2 >> 2];
        /* flip the bit */
        buf[byte_addr] ^= (1 << bit_addr);
        return 1;

    }
    /* count nr of bits; use table lookup, faster than calculating it */
    if ((bitsperbyte[b0] + bitsperbyte[b1] + bitsperbyte[b2]) == 1)
        return 1;   /* error in ECC data; no action needed */

    AW_DBGF(("%s: uncorrectable ECC error\n", __func__));
    return -1;
}

/******************************************************************************
 BCH
******************************************************************************/
/**
 * \brief 通过页大小、oob大小以及校验强度来计算出
 *        在使用软件BCH时m的大小以及每步校验的数据块大小
 *        2^m-1 > eccsize*8  ;  m*t <= eccbytes*8
 */
aw_local void __nandflash_bch_size_calculate (
    awbl_nand_ecc_t         *p_this,
    uint8_t                  ecc_strength,
    uint8_t                 *m,
    uint16_t                *data_size)
{
    static uint16_t __m_data_size[11] = {
                        2,    /**< m = 5  */
                        4,    /**< m = 6  */
                        8,    /**< m = 7  */
                        16,   /**< m = 8  */
                        32,   /**< m = 9  */
                        64,   /**< m = 10 */
                        128,  /**< m = 11 */
                        256,  /**< m = 12 */
                        512,  /**< m = 13 */
                        1024, /**< m = 14 */
                        2048  /**< m = 15 */
                    };

    int      i;
    uint16_t steps;
    uint16_t temp_ds, temp_es;
    uint16_t best_es = 0xFFFF;
    int16_t  best_ds = 0xFFFF;
    int8_t   best_m  = 0xFF;

    for (i = 0; i < AW_NELEMENTS(__m_data_size); i++) {

        temp_ds = __m_data_size[i];
        steps   = p_this->p_info->attr.page_size / temp_ds;
        temp_es = (steps * ecc_strength * (i+5) + 8 - 1) / 8; /* 向上取整除 */

        if (temp_es > p_this->p_info->attr.spare_size) {
            continue;
        }

        if (temp_es <= best_es) {
            best_es = temp_es;
            best_ds = temp_ds;
            best_m  = i+5;
        }

    }

    *data_size = (uint16_t)best_ds;
    *m         = (uint8_t) best_m;
}


/******************************************************************************/
aw_local aw_err_t __swecc_bch_init (awbl_nand_ecc_t *p_this)
{
    struct nand_bch_control *nbc = NULL;
    uint16_t  data_size;
    int       i = 0;
    uint8_t   m;
    uint8_t  *erase_page = NULL;

    if ((!p_this) || (!p_this->p_info->ecc_strength)) {
        return AW_ERROR;
    }

    nbc = (struct nand_bch_control *)aw_mem_alloc(
                                     sizeof(struct nand_bch_control));
    aw_assert(nbc);
    memset(nbc, 0, sizeof(*nbc));
    p_this->priv = nbc;

    __nandflash_bch_size_calculate(p_this,
                                   p_this->p_info->ecc_strength,
                                   &m,
                                   &data_size);

    if ((0xFF == m) && (0xFFFF == data_size)) {
        __ASSERT(0, "nandflash: maybe oob size is not big enough\r\n");
        return AW_ERROR;
    }

    nbc->bch = aw_soft_bch_init(m,
                                p_this->p_info->ecc_strength,
                                0);

    nbc->errloc = (unsigned int *)aw_mem_alloc(p_this->p_info->ecc_strength *
                                               sizeof(unsigned int));
    aw_assert(nbc->errloc);

    nbc->eccmask = (unsigned char *)aw_mem_alloc(nbc->bch->ecc_bytes *
                                                 sizeof(unsigned char));
    aw_assert(nbc->eccmask);
    memset((void *)nbc->eccmask, 0, nbc->bch->ecc_bytes * sizeof(unsigned char));

    erase_page = (uint8_t *)aw_mem_alloc(data_size);
    aw_assert(erase_page);
    memset(erase_page, 0xff, data_size);

    aw_soft_bch_encode(nbc->bch, erase_page, data_size, nbc->eccmask);
    for (i = 0; i < nbc->bch->ecc_bytes; i++) {
        nbc->eccmask[i] ^= 0xff;
    }

    aw_mem_free(erase_page);

    p_this->size         = data_size;
    p_this->bytes        = nbc->bch->ecc_bytes;
    p_this->steps        = p_this->p_info->attr.page_size / p_this->size;
    p_this->strength     = p_this->p_info->ecc_strength;
    p_this->total_bytes  = p_this->bytes * p_this->steps;

    if (!p_this->size && (p_this->p_info->attr.spare_size >= 64)) {
        p_this->size = 512;
        p_this->strength = 4;
    }

    return AW_OK;
}


/**
 * \brief ECC计算接口实现(BCH)
 */
aw_local int __bch_ecc_calculate (awbl_nand_ecc_t            *p_this,
                                  const  uint8_t             *p_buf,
                                  uint8_t                    *ecc_buf)
{
    unsigned int i;
    struct nand_bch_control  *nbc  = NULL;

    nbc = p_this->priv;

    memset((void *)ecc_buf, 0, p_this->bytes);
    aw_soft_bch_encode(nbc->bch, p_buf, p_this->size, ecc_buf);

    /* 使用掩码使得擦除页的ECC也是合法的 */
    for (i = 0; i < p_this->bytes; i++) {
        ecc_buf[i] ^= nbc->eccmask[i];
    }

    return 0;
}


/**
 * \brief ECC校验接口实现(BCH)
 */
aw_local int __bch_ecc_correct (awbl_nand_ecc_t            *p_this,
                                uint8_t                    *p_buf,
                                uint8_t                    *read_ecc,
                                uint8_t                    *calc_ecc)
{
    int i, count;
    unsigned int               *errloc      = NULL;
    struct nand_bch_control    *nbc         = NULL;

    nbc = p_this->priv;

    errloc = nbc->errloc;

    count = aw_soft_bch_decode(nbc->bch,
                               NULL,
                               p_this->size,
                               read_ecc,
                               calc_ecc,
                               NULL,
                               errloc);

    if (count > 0) {
        for (i = 0; i < count; i++) {
            if (errloc[i] < (p_this->size * 8)) {
                p_buf[errloc[i] >> 3] ^= (1 << (errloc[i] & 7));
            }
            AW_DBGF(("%s: corrected bitflip %u\n", __func__, errloc[i]));
        }

    } else if (count < 0) {
        /* ecc unrecoverable error  */
        count = -AW_EBADMSG;
    }
    return count;
}


/******************************************************************************/
/**
 * nand_read_page_raw - [INTERN] read raw page data without ecc
 * @mtd: mtd info structure
 * @p_this: nand p_this info structure
 * @buf: buffer to store read data
 * @oob_required: caller requires OOB data read to p_this->oob_poi
 * @page: page number to read
 *
 * Not for syndrome calculating ECC controllers, which use a special oob layout.
 */
aw_local int __nand_read_page_raw(awbl_nand_ecc_t           *p_this,
                                  uint8_t                   *data_buf,
                                  uint8_t                   *oob_free,
                                  uint16_t                   oob_free_size,
                                  aw_bool_t                  oob_required,
                                  uint32_t                   page)
{
    aw_err_t    ret;
    aw_assert(p_this->p_pf_serv->read_buf);
    ret = p_this->p_pf_serv->read_buf(p_this->p_pf_serv,
                                      page,
                                      0,
                                      data_buf,
                                      p_this->p_info->attr.page_size,
                                      p_this->buffers->oob_buf,
                                      p_this->p_info->attr.spare_size,
                                      oob_required);
    __CHECK_D(ret, ==, AW_OK, return ret);
    if (oob_free) {
        __oob_free_get(p_this->p_info,
                       p_this->buffers->oob_buf,
                       0,
                       oob_free,
                       oob_free_size);
    }
    return AW_OK;
}

aw_local int __nand_write_page_raw (awbl_nand_ecc_t           *p_this,
                                    uint8_t                   *data_buf,
                                    uint8_t                   *oob_free,
                                    uint16_t                   oob_free_size,
                                    aw_bool_t                  oob_required,
                                    uint32_t                   page)
{
    aw_err_t    ret;
    aw_assert(p_this->p_pf_serv->write_buf);

#if 0
    /* 默认全为 0xFF，保护 坏块标记和未使用区域 */
    memset(p_this->buffers->oob_buf, 0xff, p_this->p_info->attr.spare_size);
#endif

    if (oob_free) {
        __oob_free_put(p_this->p_info,
                       oob_free,
                       oob_free_size,
                       0,
                       p_this->buffers->oob_buf);
    }
    ret = p_this->p_pf_serv->write_buf(p_this->p_pf_serv,
                                       page,
                                       0,
                                       data_buf,
                                       p_this->p_info->attr.page_size,
                                       p_this->buffers->oob_buf,
                                       p_this->p_info->attr.spare_size,
                                       oob_required);
    __CHECK_D(ret, ==, AW_OK, return ret);
    return AW_OK;
}


/**
 * __nand_read_oob_free - [REPLACEABLE] the most common OOB data read function
 * @mtd: mtd info structure
 * @p_this: nand p_this info structure
 * @page: page number to read
 */
aw_local int __nand_read_oob_free (awbl_nand_ecc_t    *p_this,
                                   uint32_t            page,
                                   uint32_t            offs,
                                   uint8_t            *oob_free,
                                   uint16_t            len)
{
    int ret;

    ret = __platform_oob_read(p_this,
                              page,
                              p_this->buffers->oob_buf,
                              p_this->p_info->attr.spare_size);
    __CHECK_D(ret, ==, AW_OK, return ret);
    __oob_free_get(p_this->p_info, p_this->buffers->oob_buf, offs, oob_free, len);

    return AW_OK;
}

/**
 * __nand_write_oob_free - [REPLACEABLE] the most common OOB data write function
 * @mtd: mtd info structure
 * @p_this: nand p_this info structure
 * @page: page number to write
 */
aw_local int __nand_write_oob_free (awbl_nand_ecc_t    *p_this,
                                    uint32_t            page,
                                    uint32_t            offs,
                                    uint8_t            *oob_free,
                                    uint16_t            len)
{
    int ret;

    /* 默认全为 0xFF，保护 坏块标记和 ecc 区域 */
    memset(p_this->buffers->oob_buf, 0xff, p_this->p_info->attr.spare_size);
    __oob_free_put(p_this->p_info, oob_free, len, offs, p_this->buffers->oob_buf);

    ret = __platform_oob_write (p_this,
                                p_this->buffers->oob_buf,
                                p_this->p_info->attr.spare_size,
                                page);

    __CHECK_D(ret, ==, AW_OK, return ret);
    return AW_OK;
}

/******************************************************************************/

void awbl_nand_ecc_itf_def_set (awbl_nand_ecc_t *p_ecc)
{

    if (!p_ecc->itf.read_page_raw) {
        p_ecc->itf.read_page_raw = __nand_read_page_raw;
    }

    if (!p_ecc->itf.write_page_raw) {
        p_ecc->itf.write_page_raw = __nand_write_page_raw;
    }
    if (!p_ecc->itf.read_page) {
        p_ecc->itf.read_page = p_ecc->itf.read_page_raw;
    }

    if (!p_ecc->itf.write_page) {
        p_ecc->itf.write_page = p_ecc->itf.write_page_raw;
    }

    if (!p_ecc->itf.read_oob_free) {
        p_ecc->itf.read_oob_free = __nand_read_oob_free;
    }

    if (!p_ecc->itf.write_oob_free) {
        p_ecc->itf.write_oob_free = __nand_write_oob_free;
    }
}

/******************************************************************************/
/**
 * nand_read_page_swecc - [REPLACEABLE] software ECC based page read function,
 * will call requires OOB data read to p_this->oob_poi
 *
 * @mtd: mtd info structure
 * @p_this: nand p_this info structure
 * @buf: buffer to store read data
 * @oob_required:
 * @page: page number to read
 */
aw_local int __nand_read_page_swecc (awbl_nand_ecc_t  *p_this,
                                     uint8_t          *buf,
                                     uint8_t          *oob_free,
                                     uint16_t          oob_free_size,
                                     aw_bool_t         oob_required,
                                     uint32_t          page,
                                     __ecc_calculate_t pfn_calculate,
                                     __ecc_correct_t   pfn_correct)
{
    uint8_t     *p = buf;
    uint8_t     *read_ecc = NULL;
    uint8_t     *calc_ecc = NULL;
    uint32_t     i;

    int eccsize  = p_this->bytes;
    int datasize = p_this->size;
    int eccsteps = p_this->steps;
    int stat;
    int ret;

    /* 读页数据, oob 数据读到 p_this->oob_buf 中 */
    ret = p_this->itf.read_page_raw(p_this,
                                    buf,
                                    oob_free,
                                    oob_free_size,
                                    oob_required,
                                    page);
    __CHECK_D(ret, ==, AW_OK, return ret);

    /* 读出 ecc */
    __oob_ecc_get(p_this->p_info,
                  p_this->buffers->oob_buf,
                  p_this->ecc_buf,
                  p_this->ecc_buf_size);

    calc_ecc = p_this->buffers->calc_ecc;
    read_ecc = p_this->ecc_buf;

    for (i = 0; eccsteps; eccsteps--, i += eccsize, p += datasize) {
        pfn_calculate(p_this, p, &calc_ecc[i]);
    }

    eccsteps = p_this->steps;

    p   = buf;
    ret = 0;
    for (i = 0 ; eccsteps; eccsteps--, i += eccsize, p += datasize) {
        stat = pfn_correct(p_this, p, &read_ecc[i], &calc_ecc[i]);
        if (stat < 0) {
            __LOG_PFX(1, "[ ERROR ] uncorrectable ecc error(page:%d)", page);

            /* 发生无法恢复的位翻转 */
            return -AW_EBADMSG;
        } else if (stat > 0){
            /* 翻转位数 */
            ret += stat;
        }
    }

    return ret;
}


aw_local int __nand_read_page_swecc_bch (awbl_nand_ecc_t    *p_this,
                                         uint8_t            *buf,
                                         uint8_t            *oob_free,
                                         uint16_t            oob_free_size,
                                         aw_bool_t           oob_required,
                                         uint32_t            page)
{
    return __nand_read_page_swecc(p_this,
                                  buf,
                                  oob_free,
                                  oob_free_size,
                                  oob_required,
                                  page,
                                  __bch_ecc_calculate,
                                  __bch_ecc_correct);
}
aw_local int __nand_read_page_swecc_ham (awbl_nand_ecc_t    *p_this,
                                         uint8_t            *buf,
                                         uint8_t            *oob_free,
                                         uint16_t            oob_free_size,
                                         aw_bool_t           oob_required,
                                         uint32_t            page)
{
    return __nand_read_page_swecc(p_this,
                                  buf,
                                  oob_free,
                                  oob_free_size,
                                  oob_required,
                                  page,
                                  __ham_ecc_calculate,
                                  __ham_ecc_correct);
}

aw_local int __nand_write_page_swecc (awbl_nand_ecc_t       *p_this,
                                      uint8_t               *p_buf,
                                      uint32_t               page,
                                      uint8_t               *oob_free,
                                      uint16_t               oob_free_size,
                                      aw_bool_t              oob_required,
                                      __ecc_calculate_t      pfn_calculate)
{
    int i;
    int ret;

    int eccsize  = p_this->bytes;
    int datasize = p_this->size;
    int eccsteps = p_this->steps;

    uint8_t  *p = p_buf, *calc_ecc = p_this->buffers->calc_ecc;

    /* 计算 ecc */
    for (i = 0; eccsteps; eccsteps--, i += eccsize, p += datasize) {
        pfn_calculate(p_this, p, &calc_ecc[i]);
    }

    /* 默认全为 0xFF，保护 坏块标记和未使用区域 */
    memset(p_this->buffers->oob_buf, 0xff, p_this->p_info->attr.spare_size);

    __oob_ecc_put(p_this->p_info,
                  calc_ecc,
                  p_this->total_bytes,
                  p_this->buffers->oob_buf);

    ret = p_this->itf.write_page_raw(p_this,
                                     p_buf,
                                     oob_free,
                                     oob_free_size,
                                     oob_required,
                                     page);
    __CHECK_D(ret, ==, AW_OK, return ret);

    return ret;
}


/** \brief 写入页数据 */
aw_local int __nand_write_page_swecc_ham (awbl_nand_ecc_t   *p_this,
                                          uint8_t           *p_buf,
                                          uint8_t           *oob_free,
                                          uint16_t           oob_free_size,
                                          aw_bool_t          oob_required,
                                          uint32_t           page)
{
    return __nand_write_page_swecc(p_this,
                                   p_buf,
                                   page,
                                   oob_free,
                                   oob_free_size,
                                   oob_required,
                                   __ham_ecc_calculate);
}
aw_local int __nand_write_page_swecc_bch (awbl_nand_ecc_t       *p_this,
                                          uint8_t               *p_buf,
                                          uint8_t               *oob_free,
                                          uint16_t               oob_free_size,
                                          aw_bool_t              oob_required,
                                          uint32_t               page)
{
    return __nand_write_page_swecc(p_this,
                                   p_buf,
                                   page,
                                   oob_free,
                                   oob_free_size,
                                   oob_required,
                                   __bch_ecc_calculate);

}

/******************************************************************************/
aw_local awbl_nand_ecc_t *__none_ecc_create (awbl_nand_ecc_init_t *p_init)
{
    awbl_nand_ecc_t             *p_ecc;
    awbl_nand_platform_serv_t   *p_pf_serv  = p_init->p_pf_serv;
    awbl_nand_info_t            *p_info     = p_init->p_info;

    aw_assert(p_init && p_init->p_info && p_init->p_pf_serv);

    p_ecc = malloc(sizeof(*p_ecc));
    aw_assert(p_ecc);
    memset(p_ecc, 0, sizeof(*p_ecc));

    p_ecc->p_info    = p_init->p_info;
    p_ecc->p_pf_serv = p_init->p_pf_serv;

    /* 初始化 buffer */
    {
        awbl_nand_ecc_buffers_t    *nbuf;

        nbuf = (awbl_nand_ecc_buffers_t *)malloc(
                    sizeof(*nbuf) + p_info->attr.spare_size * 2);
        aw_assert(nbuf);
        nbuf->oob_buf   = (uint8_t *)(nbuf + 1);
        nbuf->calc_ecc  = nbuf->oob_buf  + p_info->attr.spare_size;
        p_ecc->buffers  = nbuf;
    }

    aw_assert(p_pf_serv->p_bus);

    {
        p_ecc->size          = p_ecc->p_info->attr.page_size;
        p_ecc->bytes         = 0;
        p_ecc->steps         = 1;
        p_ecc->strength      = 0;
        p_ecc->total_bytes   = 0;
    }

    /* 设置 ecc 读写回调 */
    awbl_nand_ecc_itf_def_set(p_ecc);

    return p_ecc;
}

aw_local awbl_nand_ecc_t *__bch_swecc_create (awbl_nand_ecc_init_t *p_init)
{
    awbl_nand_ecc_t             *p_ecc;
    awbl_nand_platform_serv_t   *p_pf_serv  = p_init->p_pf_serv;
    awbl_nand_info_t            *p_info     = p_init->p_info;

    aw_assert(p_init && p_init->p_info && p_init->p_pf_serv);

    p_ecc = malloc(sizeof(*p_ecc));
    aw_assert(p_ecc);
    memset(p_ecc, 0, sizeof(*p_ecc));

    p_ecc->p_info    = p_init->p_info;
    p_ecc->p_pf_serv = p_init->p_pf_serv;

    /* 初始化 buffer */
    {
        awbl_nand_ecc_buffers_t    *nbuf;

        nbuf = (awbl_nand_ecc_buffers_t *)malloc(
                    sizeof(*nbuf) + p_info->attr.spare_size * 2);
        aw_assert(nbuf);
        nbuf->oob_buf   = (uint8_t *)(nbuf + 1);
        nbuf->calc_ecc  = nbuf->oob_buf  + p_info->attr.spare_size;
        p_ecc->buffers  = nbuf;
    }

    aw_assert(p_pf_serv->p_bus);

    /* 初始化 bch 参数 */
    __swecc_bch_init(p_ecc);

    /* 设置硬件 ecc 读写回调 */
    {
        awbl_nand_ecc_itf_def_set(p_ecc);
        p_ecc->itf.read_page      = __nand_read_page_swecc_bch;
        p_ecc->itf.write_page     = __nand_write_page_swecc_bch;
    }

    /* 检测总的 ecc bytes 区够不够 */
    if (p_info->ecc_mode != AWBL_NAND_ECC_MODE_NONE) {
        int                          i;
        uint16_t                     ecc_space = 0;

        __ASSERT(p_ecc->total_bytes != 0, "");
        for (i = 0; i < p_info->p_nand_ooblayout->cnt_eccpos; i++) {
            ecc_space += p_info->p_nand_ooblayout->ptr_eccpos[i].length;
        }

        __ASSERT(ecc_space >= p_ecc->total_bytes,
                 "ecc_space = %d, p_this->total_bytes = %d",
                 ecc_space,
                 p_ecc->total_bytes);

        if (ecc_space) {
            p_ecc->ecc_buf = (uint8_t *)malloc(ecc_space);
            p_ecc->ecc_buf_size = ecc_space;
            __ASSERT(p_ecc->ecc_buf, "malloc() = NULL");
            memset(p_ecc->ecc_buf, 0, ecc_space);
        }
    }

    return p_ecc;
}
aw_local awbl_nand_ecc_t *__ham_swecc_create (awbl_nand_ecc_init_t *p_init)
{
    awbl_nand_ecc_t             *p_ecc;
    awbl_nand_platform_serv_t   *p_pf_serv  = p_init->p_pf_serv;
    awbl_nand_info_t            *p_info     = p_init->p_info;

    aw_assert(p_init && p_init->p_info && p_init->p_pf_serv);

    p_ecc = malloc(sizeof(*p_ecc));
    aw_assert(p_ecc);
    memset(p_ecc, 0, sizeof(*p_ecc));

    p_ecc->p_info    = p_init->p_info;
    p_ecc->p_pf_serv = p_init->p_pf_serv;
    /* 初始化 buffer */
    {
        awbl_nand_ecc_buffers_t    *nbuf;

        nbuf = (awbl_nand_ecc_buffers_t *)malloc(
                    sizeof(*nbuf) + p_info->attr.spare_size * 2);
        aw_assert(nbuf);
        nbuf->oob_buf   = (uint8_t *)(nbuf + 1);
        nbuf->calc_ecc  = nbuf->oob_buf  + p_info->attr.spare_size;
        p_ecc->buffers  = nbuf;
    }

    aw_assert(p_pf_serv->p_bus);

    /* 设置 ecc 参数和 读写回调 */
    {
        p_ecc->size          = 256;
        p_ecc->bytes         = 3;
        p_ecc->steps         = p_ecc->p_info->attr.page_size / 256;
        p_ecc->strength      = 1;
        p_ecc->total_bytes   = p_ecc->bytes * p_ecc->steps;

        awbl_nand_ecc_itf_def_set(p_ecc);
        p_ecc->itf.read_page      = __nand_read_page_swecc_ham;
        p_ecc->itf.write_page     = __nand_write_page_swecc_ham;
    }

    /* 检测总的 ecc bytes 区够不够 */
    if (p_info->ecc_mode != AWBL_NAND_ECC_MODE_NONE) {
        int                          i;
        uint16_t                     ecc_space = 0;

        __ASSERT(p_ecc->total_bytes != 0, "");
        for (i = 0; i < p_info->p_nand_ooblayout->cnt_eccpos; i++) {
            ecc_space += p_info->p_nand_ooblayout->ptr_eccpos[i].length;
        }

        __ASSERT(ecc_space >= p_ecc->total_bytes,
                 "ecc_space = %d, p_this->total_bytes = %d",
                 ecc_space,
                 p_ecc->total_bytes);

        if (ecc_space) {
            p_ecc->ecc_buf      = (uint8_t *)malloc(ecc_space);
            p_ecc->ecc_buf_size = ecc_space;
            __ASSERT(p_ecc->ecc_buf, "malloc() = NULL");
            memset(p_ecc->ecc_buf, 0, ecc_space);
        }
    }

    return p_ecc;
}

/******************************************************************************/
aw_err_t awbl_nand_ecc_register (enum awbl_nand_ecc_mode ecc_mode,
                                 awbl_nand_ecc_t *(*pfn_custom_create) (
                                         awbl_nand_ecc_init_t *p))
{
    __nand_ecc_reg_t *p_reg;
    __ASSERT(pfn_custom_create, "");

    p_reg = malloc(sizeof(__nand_ecc_reg_t));
    __ASSERT(p_reg, "");

    p_reg->p_next            = NULL;
    p_reg->ecc_mode          = ecc_mode;
    p_reg->pfn_custom_create = pfn_custom_create;

    if (__gp_reg_list == NULL) {
        __gp_reg_list            = p_reg;
    } else {
        p_reg->p_next            = __gp_reg_list;
        __gp_reg_list            = p_reg;
    }
    return AW_OK;
}

/******************************************************************************/
awbl_nand_ecc_t *awbl_nand_ecc_create (awbl_nand_ecc_init_t *p_init)
{
    awbl_nand_ecc_t     *p_ecc = NULL;
    __nand_ecc_reg_t    *p_reg = __gp_reg_list;

    aw_assert(p_init && p_init->p_info && p_init->p_pf_serv);
    while (p_reg != NULL) {
        if (p_reg->ecc_mode == p_init->p_info->ecc_mode) {
            __ASSERT(p_reg->pfn_custom_create, "");
            p_ecc = p_reg->pfn_custom_create(p_init);
            return p_ecc;
        }
        p_reg = p_reg->p_next;
    }

    switch (p_init->p_info->ecc_mode) {
        case AWBL_NAND_ECC_MODE_NONE: {
            p_ecc = __none_ecc_create(p_init);
        } break;

        case AWBL_NAND_ECC_MODE_SW_BCH: {
            p_ecc = __bch_swecc_create(p_init);
        } break;

        case AWBL_NAND_ECC_MODE_SW_HAM: {
            p_ecc = __ham_swecc_create(p_init);
        } break;

        default:break;
    }

    return p_ecc;
}

/* end of file */
