/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Nandflash default service
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-14  vih, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "awbl_nand_pf.h"
#include "awbl_nand.h"
#include "awbl_nand_bus.h"
#include "aw_bitops.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_mem.h"
#include "aw_assert.h"
#include "aw_delay.h"

/******************************************************************************/
#define __LOG_DEV_DEF(p_ctl)


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
#define __LOG_RW             0

/******************************************************************************/
/* status */
#define __NANDFLASH_STATUS_FAIL           0x01

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

#if  defined(__VIH_DEBUG) && 0
static void __detail_data_print (awbl_nand_platform_serv_t  *p_this,
                                 void                   *p_buf,
                                 int                     page,
                                 int                     offset,
                                 int                     len)
{
    int blk;
    int page_start;
    int i;
    int addr = 0;
    __LOG_DEV_DEF(NULL);

    if (__LOG_RW == 0) {
        return;
    }

    blk        = page / (p_this->p_info->attr.blk_size / p_this->p_info->attr.page_size);
    page_start = page % (p_this->p_info->attr.blk_size / p_this->p_info->attr.page_size);

    __LOG(__LOG_RW, NULL, "block:%d, page:%d\naddr: ", \
              blk, page_start);
    for (i = 0; i < 32; i++) {
        __LOG(__LOG_RW, NULL, "%02d ", i);
    }
    for (i = 0; i < 102; i++) {
        if (i == 0) {
            __LOG(__LOG_RW, NULL, "\n");
        }
        __LOG(__LOG_RW, NULL, "=");
    }

    if ((offset % 32) != 0) {
        addr = offset;
        addr = AW_ROUND_DOWN(32, addr);
        __LOG(__LOG_RW, NULL, "\n%04d: ", addr);
        i = offset - addr;
        for (; i > 0; i--) {
            __LOG(__LOG_RW, NULL, "   ");
        }
    }

    for (i = 0; i < len; i++, offset++) {
        if ((offset % 32) == 0) {
            __LOG(__LOG_RW, NULL, "\n%04d: ", offset);
        }
        __LOG(__LOG_RW, NULL, "%02x ", ((uint8_t *)p_buf)[i]);
    }
    __LOG(__LOG_RW, NULL, "\n");

//    aw_mdelay(2000);
}
#else
#define __detail_data_print(a1, a2, a3, a4, a5)
#endif /* __VIH_DEBUG */
/******************************************************************************/
/**
 * \brief OPT结构体配置
 */
aw_local void __nandbus_opt_make     (struct awbl_nandbus_opt *p_opt,
                                      uint8_t               type,
                                      aw_bool_t                lock_ce,
                                      const void           *ptr,
                                      uint32_t              len)
{
    p_opt->type     = type;
    p_opt->lock_ce  = lock_ce;
    p_opt->ptr      = ptr;
    p_opt->len      = len;
}

aw_local aw_err_t __read_id (awbl_nand_platform_serv_t *p_this,
                             uint8_t                   *p_ids,
                             uint8_t                    id_len)
{
    struct awbl_nandbus      *p_bus = p_this->p_bus;

    struct awbl_nandbus_opt  opt;
    aw_err_t              err;
    uint8_t               buf[2];

    awbl_nandbus_lock(p_bus);

    /* select chip. */
    err = awbl_nandbus_select(p_bus, p_this->p_info);
    if (err != AW_OK) {
        goto _exit;
    }

    /* command phase. */
    buf[0] = AWBL_NANDFLASH_CMD_READID;

    __nandbus_opt_make(&opt, NAND_CMD, AW_TRUE, buf, 1);

    err = awbl_nandbus_opt(p_bus, &opt);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* address phase. */
    buf[0] = 0;

    __nandbus_opt_make(&opt, NAND_ADDR, AW_TRUE, buf, 1);

    err = awbl_nandbus_opt(p_bus, &opt);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* data phase, read ID */
    __nandbus_opt_make(&opt, NAND_RD_DATA, AW_FALSE, p_ids, id_len);

    err = awbl_nandbus_opt(p_bus, &opt);
    if (err != AW_OK) {
        goto _reset_host;
    }

    goto _exit;

_reset_host:
    awbl_nandbus_reset(p_bus);

_exit:
    awbl_nandbus_unlock(p_bus);
#if 0
    if (err == AW_OK) {
        __flash_id_parse(p_ids, id_len);
    }
#endif
    return err;
}

aw_local aw_bool_t __probe (awbl_nand_platform_serv_t *p_this)
{
    int                          err;
    uint8_t                      ids[10];
    aw_bool_t                    found = AW_FALSE;

    awbl_nandbus_lock(p_this->p_bus);

    if (1) {

        /* reset the flash */
        err = p_this->reset(p_this);
        if (err != AW_OK) {
            AW_ERRF(("[ ERROR ] nand: reset failed\n"));
            goto __exit;
        }

        err = p_this->read_id(p_this, ids, 2);
        if (err != AW_OK) {
            AW_ERRF(("[ ERROR ] nand: read ID failed\n"));
            goto __exit;
        }

        /* check IDs */
        if ((p_this->p_info->attr.maker_id == ids[0]) &&
                (p_this->p_info->attr.device_id == ids[1])) {
            found = AW_TRUE;
        }
    }

    if (!found) {
        goto __exit;
    }

    AW_INFOF(("\n"
              "nand: found a new nandflash device (VID:0x%02X  PID:0x%02X)\n",
              p_this->p_info->attr.maker_id,   p_this->p_info->attr.device_id));

    AW_INFOF((" - nblks:%d \n", p_this->p_info->attr.nblks));
    AW_INFOF((" - blk_size:%d \n", p_this->p_info->attr.blk_size));
    AW_INFOF((" - pages_per_blk:%d \n", p_this->p_info->attr.pages_per_blk));
    AW_INFOF((" - page_size:%d \n", p_this->p_info->attr.page_size));
    AW_INFOF((" - spare_size:%d \n", p_this->p_info->attr.spare_size));
    AW_INFOF((" - ecc_mode:%d \n", p_this->p_info->ecc_mode));
    AW_INFOF((" - ecc_strength:%d \n", p_this->p_info->ecc_strength));

#if 0
    for (i = 2; i < p_info->id_len; i++) {
        AW_INFOF((", %02xH", ids[i]));
    }
    AW_INFOF(("\" \n"));
#endif

    awbl_nandbus_unlock(p_this->p_bus);
    return AW_TRUE;
__exit:
    awbl_nandbus_unlock(p_this->p_bus);
    return AW_FALSE;
}


/**
 *  \brief 写入数据，oob_required为真则会将p_oob_poi的数据写入到oob
 */
static aw_err_t __buf_write          (awbl_nand_platform_serv_t  *p_this,
                                      uint32_t                    page_addr,
                                      uint32_t                    col_addr,
                                      uint8_t                    *p_buf,
                                      int                         len,
                                      uint8_t                    *oob_buf,
                                      uint16_t                    oob_buf_size,
                                      aw_bool_t                   oob_required)
{
    __LOG_DEV_DEF(p_this);
    struct awbl_nandbus_opt   ctrl;
    aw_err_t                  err;
    uint8_t                   buf[5];
    uint8_t                   i;
    uint32_t                  timeout;
    uint8_t                   retries = 10;
    struct awbl_nandbus      *p_bus          = p_this->p_bus;

    uint16_t                  page_size = p_this->p_info->attr.page_size;

    awbl_nandbus_lock(p_bus);

    /* select chip. */
    err = awbl_nandbus_select(p_bus, p_this->p_info);
    if (err != AW_OK) {
        goto _exit;
    }

    /* wait for complete. */
    timeout = 5000000;
    while (!awbl_nandbus_is_ready(p_bus)) {
        timeout--;
        if (timeout == 0) {
            AW_INFOF(("[ ERROR ] [%s:%d]:!awbl_nandbus_is_ready(p_bus)", \
                     __func__, __LINE__));
            err = -AW_ETIME;
            goto _reset_host;
        }
    }

_retry:
    /* command phase. */
    i = 0;
    if (page_size <= 512) {
        if (col_addr < 256) {
            buf[i++] = AWBL_NANDFLASH_CMD_READ0;
        } else if (col_addr >= 512) {
            buf[i++] = AWBL_NANDFLASH_CMD_READOOB;
        } else {
            buf[i++] = AWBL_NANDFLASH_CMD_READ1;
        }
    }

    buf[i++] = AWBL_NANDFLASH_CMD_SEQIN;

    __nandbus_opt_make(&ctrl, NAND_CMD, AW_TRUE, buf, i);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    i=0;
    /* column address. */
    buf[i++] = col_addr & 0xFF;
    if (page_size > 512) {
        buf[i++] = (col_addr>>8) & 0xFF;
    }

    /* page address.*/
    buf[i++] = page_addr & 0xFF;
    buf[i++] = (page_addr>>8) & 0xFF;
    /* if need to send the third byte. */
    if ( ((page_size > 512) &&
          (p_this->p_info->attr.chip_size > (128<<20))) ||
         ((page_size <= 512) &&
          (p_this->p_info->attr.chip_size > (32<<20)))
        ) {
        buf[i++] = (page_addr>>16) & 0xFF;
    }

    __nandbus_opt_make(&ctrl, NAND_ADDR, AW_TRUE, buf, i);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* data phase, write data. */
    __nandbus_opt_make(&ctrl, NAND_WR_DATA, AW_FALSE, p_buf, len);
    err = awbl_nandbus_opt(p_bus, &ctrl);

    if (col_addr < 2048) {
    	if (__LOG_RW) {
            __LOG_PFX(__LOG_RW, "write page :");
            __detail_data_print(p_this, p_buf, page_addr, col_addr, len);
    	}
    } else {
    	if (__LOG_RW) {
    		__LOG_PFX(__LOG_RW, "write oob :");
    		__detail_data_print(p_this, p_buf, page_addr, col_addr, len);
    	}
    }

    if (AW_OK == err && oob_required) {
        __ASSERT(oob_buf && oob_buf_size, "");
        /* 写oob */
        __nandbus_opt_make(&ctrl,
                            NAND_WR_DATA,
                            AW_FALSE,
                            oob_buf,
                            oob_buf_size);

        err = awbl_nandbus_opt(p_bus, &ctrl);
    }

    if (AW_OK != err) {
        goto _reset_host;
    }

    /* second command phase. */
    buf[0] = AWBL_NANDFLASH_CMD_PAGEPROG;

    __nandbus_opt_make(&ctrl, NAND_CMD, AW_TRUE, buf, 1);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* wait for complete. */
    timeout = 5000000;
    while (!awbl_nandbus_is_ready(p_bus)) {
        timeout--;
        aw_udelay(1);
        if (timeout == 0) {
            AW_INFOF(("[ ERROR ] [%s:%d]:!awbl_nandbus_is_ready(p_bus)", \
                     __func__, __LINE__));
            err = -AW_ETIME;
            goto _reset_host;
        }
    }

    /* third command phase. */
    buf[0] = AWBL_NANDFLASH_CMD_STATUS;

    __nandbus_opt_make(&ctrl, NAND_CMD, AW_TRUE, buf, 1);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* read status. */
    __nandbus_opt_make(&ctrl, NAND_RD_DATA, AW_FALSE, buf, 1);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    if (buf[0] & __NANDFLASH_STATUS_FAIL) {
        if (retries--) {
            goto _retry;
        }
        err = AW_ERROR;
    } else {
        err = AW_OK;
    }

    goto _exit;

_reset_host:
    awbl_nandbus_reset(p_bus);

_exit:
    awbl_nandbus_unlock(p_bus);

    return err;
}

/**
 * \brief 读取数据，如果oob_required为真则会将oob数据读到p_oob_poi
 */
static aw_err_t __buf_read   (awbl_nand_platform_serv_t  *p_this,
                              uint32_t                    page_addr,
                              uint32_t                    col_addr,
                              uint8_t                    *p_buf,
                              int                         len,
                              uint8_t                    *oob_buf,
                              uint16_t                    oob_buf_size,
                              aw_bool_t                   oob_required)
{
    struct awbl_nandbus         *p_bus     = p_this->p_bus;
    __LOG_DEV_DEF(p_this);
    struct awbl_nandbus_opt      ctrl;
    aw_err_t                     err;
    uint8_t                      buf[5];
    uint8_t                      i;
    uint32_t                     timeout;
    uint16_t                     page_size = p_this->p_info->attr.page_size;

    awbl_nandbus_lock(p_bus);

    /* select chip. */
    err = awbl_nandbus_select(p_bus, p_this->p_info);
    if (err != AW_OK) {
        goto _exit;
    }

    /* wait for complete. */
    timeout = 5000000;
    while (!awbl_nandbus_is_ready(p_bus)) {
        timeout--;
        if (timeout == 0) {
            AW_INFOF(("[ ERROR ] [%s:%d]:!awbl_nandbus_is_ready(p_bus)", \
                     __func__, __LINE__));
            err = -AW_ETIME;
            goto _reset_host;
        }
    }

    /* command phase. */
    if (page_size <= 512) {
        if (col_addr < 256) {
            buf[0] = AWBL_NANDFLASH_CMD_READ0;
        } else if (col_addr >= 512) {
            buf[0] = AWBL_NANDFLASH_CMD_READOOB;
        } else {
            buf[0] = AWBL_NANDFLASH_CMD_READ1;
        }
    } else {
        buf[0] = AWBL_NANDFLASH_CMD_READ0;
    }

    __nandbus_opt_make(&ctrl, NAND_CMD, AW_TRUE, buf, 1);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    i=0;
    /* column address. */
    buf[i++] = col_addr & 0xFF;
    if (page_size > 512) {
        buf[i++] = (col_addr>>8) & 0xFF;
    }

    /* page address.*/
    buf[i++] = page_addr & 0xFF;
    buf[i++] = (page_addr>>8) & 0xFF;
    if ( ((page_size > 512) &&
          (p_this->p_info->attr.chip_size > (128<<20))) ||
         ((page_size <= 512) &&
          (p_this->p_info->attr.chip_size > (32<<20)))
        ) {
        buf[i++] = (page_addr>>16) & 0xFF;
    }

    __nandbus_opt_make(&ctrl, NAND_ADDR, AW_TRUE, buf, i);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }


    if (page_size > 512) {
        /* the other command phase. */
        buf[0] = AWBL_NANDFLASH_CMD_READSTART;
        __nandbus_opt_make(&ctrl, NAND_CMD, AW_TRUE, buf, 1);
        err = awbl_nandbus_opt(p_bus, &ctrl);
        if (err != AW_OK) {
            goto _reset_host;
        }
    }

    /* wait for complete. */
    timeout = 5000000;
    while (!awbl_nandbus_is_ready(p_bus)) {
        timeout--;
        aw_udelay(1);
        if (timeout == 0) {
            AW_INFOF(("[ ERROR ] [%s:%d]:!awbl_nandbus_is_ready(p_bus)", \
                     __func__, __LINE__));
            err = -AW_ETIME;
            goto _reset_host;
        }
    }

    /* read data. */
    __nandbus_opt_make(&ctrl, NAND_RD_DATA, AW_FALSE, p_buf, len);
    err = awbl_nandbus_opt(p_bus, &ctrl);

    if (col_addr < 2048) {
    	if (__LOG_RW) {
			__LOG_PFX(__LOG_RW, "read page :");
			__detail_data_print(p_this, p_buf, page_addr, col_addr, len);
    	}
    } else {
    	if (__LOG_RW) {
			__LOG_PFX(__LOG_RW, "read oob :");
			__detail_data_print(p_this, p_buf, page_addr, col_addr, len);
    	}
    }

    if (AW_OK == err && oob_required) {
        __ASSERT(oob_buf && oob_buf_size, "");

        /* 将oob数据读到p_oob_poi */
        __nandbus_opt_make(&ctrl,
                           NAND_RD_DATA,
                           AW_FALSE,
                           oob_buf,
                           oob_buf_size);

        err = awbl_nandbus_opt(p_bus, &ctrl);
    }

    if (AW_OK != err) {
        goto _reset_host;
    }

    goto _exit;

_reset_host:
    awbl_nandbus_reset(p_bus);

_exit:

    awbl_nandbus_unlock(p_bus);
    return err;
}

/* 写坏块标记，是直接写到oob区的，标记坏块前应该先擦除块 */
aw_local int __block_markbad (awbl_nand_platform_serv_t *p_this, uint32_t page)
{
    uint8_t  val = 0;

    return __buf_write(p_this,
                       page,
                       p_this->p_info->attr.page_size +
                           p_this->p_info->attr.bad_blk_byte_offs,
                       &val,
                       1,
                       NULL,
                       0,
                       AW_FALSE);
}

aw_local int __block_erase (awbl_nand_platform_serv_t *p_this, uint32_t page_addr)
{
    struct awbl_nandbus         *p_bus  = p_this->p_bus;
    struct awbl_nandbus_opt      ctrl;
    aw_err_t                     err;
    uint8_t                      buf[3];
    uint32_t                     timeout;
    uint8_t                      len;
    uint8_t                      retries = 10;
    uint16_t                     page_size = p_this->p_info->attr.page_size;

    __CHECK_D(p_this, !=, NULL, return -AW_EINVAL);
    __CHECK_D(page_addr, <,
              (p_this->p_info->attr.chip_size / p_this->p_info->attr.page_size),
              return -AW_EINVAL);

    awbl_nandbus_lock(p_bus);

    /* select chip. */
    err = awbl_nandbus_select(p_bus, p_this->p_info);
    if (err != AW_OK) {
        goto _exit;
    }
    /* wait for complete. */
    timeout = 5000000;
    while (!awbl_nandbus_is_ready(p_bus)) {
        timeout--;
        if (timeout == 0) {
            AW_INFOF(("[ ERROR ] [%s:%d]:!awbl_nandbus_is_ready(p_bus)", \
                     __func__, __LINE__));
            err = -AW_ETIME;
            goto _reset_host;
        }
    }

_retry:
    /* command phase. */
    buf[0] = AWBL_NANDFLASH_CMD_ERASE1;

    __nandbus_opt_make(&ctrl, NAND_CMD, AW_TRUE, buf, 1);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* address phase. */
    buf[0] = page_addr & 0xFF;
    buf[1] = (page_addr>>8) & 0xFF;

    /* if need to send the third byte. */
    if ( ((page_size > 512) &&
          (p_this->p_info->attr.chip_size > (128<<20))) ||
         ((page_size <= 512) &&
          (p_this->p_info->attr.chip_size > (32<<20)))
        ) {
        buf[2] = (page_addr>>16) & 0xFF;
        len = 3;
    } else {
        len = 2;
    }
    __nandbus_opt_make(&ctrl, NAND_ADDR, AW_TRUE, buf, len);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* command phase, second command. */
    buf[0] = AWBL_NANDFLASH_CMD_ERASE2;

    __nandbus_opt_make(&ctrl, NAND_CMD, AW_TRUE, buf, 1);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* status phase, wait for complete. */
    timeout = 5000000;
    while (!awbl_nandbus_is_ready(p_bus)) {
        timeout--;
        aw_udelay(1);
        if (timeout == 0) {
            AW_INFOF(("[ ERROR ] [%s:%d]:!awbl_nandbus_is_ready(p_bus)", \
                     __func__, __LINE__));
            err = -AW_ETIME;
            goto _reset_host;
        }
    }


    /* command phase. */
    buf[0] = AWBL_NANDFLASH_CMD_STATUS;

    __nandbus_opt_make(&ctrl, NAND_CMD, AW_TRUE, buf, 1);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* data phase, read status. */
    __nandbus_opt_make(&ctrl, NAND_RD_DATA, AW_FALSE, buf, 1);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    if (buf[0] & __NANDFLASH_STATUS_FAIL) {
        if (retries--) {
            goto _retry;
        }
        err = -AW_EPERM;
    } else {
        err = AW_OK;
    }
    goto _exit;

_reset_host:
    awbl_nandbus_reset(p_bus);

_exit:
    awbl_nandbus_unlock(p_bus);
    return err;
}

/* 查看坏块标记，是直接查oob区的，查看的块应该是被擦除过还没写入过数据的 */
aw_local aw_bool_t __block_is_bad (awbl_nand_platform_serv_t *p_this,
                                   uint32_t                   page)
{
    uint8_t     val = 0;
    aw_err_t    err;

    err = __buf_read(p_this,
                     page,
                     p_this->p_info->attr.page_size +
                         p_this->p_info->attr.bad_blk_byte_offs,
                     &val,
                     1,
                     NULL,
                     0,
                     AW_FALSE);

    /* 操作失败直接返回为坏块，禁止后续操作 */
    __CHECK_D(err, ==, AW_OK, return AW_TRUE);

    return (val == 0xFF) ? AW_FALSE : AW_TRUE;
}

aw_local int __reset (awbl_nand_platform_serv_t *p_this)
{
    struct awbl_nandbus     *p_bus  = p_this->p_bus;
    struct awbl_nandbus_opt ctrl;
    aw_err_t                err;
    int                     temp;
    __LOG_DEV_DEF(p_this);

    __ASSERT_D(p_this, !=, NULL);
    __ASSERT_D(p_this->p_info, !=, NULL);
    __ASSERT_D(p_this->p_bus, !=, NULL);
    awbl_nandbus_lock(p_bus);

    /* select chip. */
    err = awbl_nandbus_select(p_bus, p_this->p_info);
    if (err != AW_OK) {
        goto _exit;
    }
    /* wait for complete. */
    temp = 5000000;
    while (!awbl_nandbus_is_ready(p_bus)) {
        temp--;
        if (temp == 0) {
            AW_INFOF(("[ ERROR ] [%s:%d]:!awbl_nandbus_is_ready(p_bus)", \
                     __func__, __LINE__));
            err = -AW_ETIME;
            goto _reset_host;
        }
    }

    /* command phase. */
    temp = AWBL_NANDFLASH_CMD_RESET;

    __nandbus_opt_make(&ctrl, NAND_CMD, AW_FALSE, &temp, 1);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* status phase, wait for complete. */
    temp = 5000000;
    while (!awbl_nandbus_is_ready(p_bus)) {
        temp--;
        aw_udelay(1);
        if (temp == 0) {
            AW_INFOF(("[ ERROR ] [%s:%d]:!awbl_nandbus_is_ready(p_bus)", \
                     __func__, __LINE__));
            err = -AW_ETIME;
            goto _reset_host;
        }
    }

    err = AW_OK;
    goto _exit;

_reset_host:
    awbl_nandbus_reset(p_bus);

_exit:
    awbl_nandbus_unlock(p_bus);
    return err;
}


void awbl_nand_serv_default_set (awbl_nand_platform_serv_t *p_pf_serv)
{
    __LOG_DEV_DEF(NULL);
    __ASSERT_D(p_pf_serv, !=, NULL);
    p_pf_serv->probe         = __probe;
    p_pf_serv->read_id       = __read_id;
    p_pf_serv->read_buf      = __buf_read;
    p_pf_serv->write_buf     = __buf_write;
    p_pf_serv->block_markbad = __block_markbad;
    p_pf_serv->block_is_bad  = __block_is_bad;
    p_pf_serv->reset         = __reset;
    p_pf_serv->erase         = __block_erase;
}


/* end of file */
