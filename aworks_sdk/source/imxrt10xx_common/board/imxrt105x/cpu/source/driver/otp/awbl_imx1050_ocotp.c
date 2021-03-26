/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT1050 ocotp driver
 *
 * \internal
 * \par Modification History
 * - 1.00 2017-11-15  mex, first implementation.
 * \endinternal
 */


#include "aworks.h"
#include <string.h>

#include "aw_spinlock.h"
#include "aw_delay.h"
#include "aw_sem.h"
#include "aw_int.h"
#include "aw_bitops.h"
#include "awbus_lite.h"
#include "awbl_nvram.h"
#include "awbl_plb.h"
#include "driver/otp/awbl_imx1050_ocotp.h"
#include "driver/otp/imx1050_ocotp_regs.h"
#include "imx1050_reg_base.h"
#include "aw_clk.h"
#include "aw_assert.h"





/*******************************************************************************
  defines
*******************************************************************************/
#define __IMX6UL_OCOTP_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx1050_ocotp_devinfo *p_devinfo = \
        (struct awbl_imx1050_ocotp_devinfo *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
   OTP Controller Timing Register define
*******************************************************************************/

/* This count value specifies the strobe period in one time write OTP */
#define __OTP_TIMING_STROBE_PROG_SHIFT       0
#define __OTP_TIMING_STROBE_PROG(x)        (((x) & 0xFFFUL) << __OTP_TIMING_STROBE_PROG_SHIFT)

/* This count value specifies the time to add to all default timing parameters other than the Tpgm and Trd. */
#define __OTP_TIMING_RELAX_SHIFT            12
#define __OTP_TIMING_RELAX(x)              (((x) & 0xFUL) << __OTP_TIMING_RELAX_SHIFT)

/* This count value specifies the strobe period in one time write OTP */
#define __OTP_TIMING_STROBE_READ_SHIFT      16
#define __OTP_TIMING_STROBE_READ(x)        (((x) & 0x3FUL) << __OTP_TIMING_STROBE_READ_SHIFT)

/* This count value specifies time interval between auto read and write access in one time program. */
#define __OTP_TIMING_WAIT_SHIFT             22
#define __OTP_TIMING_WAIT(x)               (((x) & 0x3FUL) << __OTP_TIMING_WAIT_SHIFT)


/*******************************************************************************
   OTP Controller Control Register define
*******************************************************************************/

/* OTP write and read access address register */
#define __OTP_CTRL_ADDR_SHIFT               0
#define __OTP_CTRL_ADDR(x)                 (((x) & 0x3FUL) << __OTP_CTRL_ADDR_SHIFT)

/* OTP controller status bit */
#define __OTP_CTRL_BUSY_SHIFT               8
#define __OTP_CTRL_BUSY_MASK               (0X1UL << __OTP_CTRL_BUSY_SHIFT)

/* Set by the controller when an access to a locked region(OTP or shadow register) is requested */
#define __OTP_CTRL_ERROR_SHIFT              9
#define __OTP_CTRL_ERROR_MASK              (0X1UL << __OTP_CTRL_ERROR_SHIFT)

/* Set to force re-loading the shadow registers (HW/SW capability and LOCK) */
#define __OTP_CTRL_RELOAD_SHADOWS_SHIFT     10
#define __OTP_CTRL_RELOAD_SHADOWS_MASK     (0X1UL << __OTP_CTRL_RELOAD_SHADOWS_SHIFT)

/* Set by controller when calculated CRC value is not equal to appointed CRC fuse word */
#define __OTP_CTRL_CRC_FAIL_SHIFT           12
#define __OTP_CTRL_CRC_FAIL_MASK           (0X1UL << __OTP_CTRL_CRC_FAIL_SHIFT)

/* Write 0x3E77 to enable OTP write accesses. */
#define __OTP_CTRL_WR_UNLOCK_SHIFT          16
#define __OTP_CTRL_WR_UNLOCK(x)            (((x) & 0xFFFFUL) << __OTP_CTRL_WR_UNLOCK_SHIFT)


/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __imx1050_ocotp_inst_init1(awbl_dev_t *p_dev);
aw_local void __imx1050_ocotp_inst_init2(awbl_dev_t *p_dev);
aw_local void __imx1050_ocotp_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __imx1050_ocotp_nvram_get(struct awbl_dev *p_dev,
                                            char            *p_name,
                                            int              unit,
                                            char            *p_buf,
                                            int              offset,
                                            int              len);

aw_local aw_err_t __imx1050_ocotp_nvram_set(struct awbl_dev *p_dev,
                                            char            *p_name,
                                            int              unit,
                                            char            *p_buf,
                                            int              offset,
                                            int              len);

/*******************************************************************************
  locals
*******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_imx1050_ocotp_drvfuncs = {
	__imx1050_ocotp_inst_init1,
	__imx1050_ocotp_inst_init2,
	__imx1050_ocotp_inst_connect
};

/** \brief driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_nvram_get);
AWBL_METHOD_IMPORT(awbl_nvram_set);

aw_local aw_const struct awbl_dev_method __g_imx1050_ocotp_dev_methods[] = {
    AWBL_METHOD(awbl_nvram_get, __imx1050_ocotp_nvram_get),
    AWBL_METHOD(awbl_nvram_set, __imx1050_ocotp_nvram_set),
    AWBL_METHOD_END
};

/** \brief driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_imx1050_ocotp_drv_registration = {
    {
        AWBL_VER_1,                           /* awb_ver */
        AWBL_BUSID_PLB,                       /* bus_id */
        AWBL_IMX1050_OCOTP_NAME,              /* p_drvname */
        &__g_imx1050_ocotp_drvfuncs,          /* p_busfuncs */
        &__g_imx1050_ocotp_dev_methods[0],    /* p_methods */
        NULL                                  /* pfunc_drv_probe */
    }
};


/** \brief get type's max sizes (byte) */
#define __IMX1050_OCOTP_TP_MAXSIZE_GET()      512


/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __imx1050_ocotp_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __imx1050_ocotp_inst_init2(awbl_dev_t *p_dev)
{
    __IMX6UL_OCOTP_DEVINFO_DECL(p_devinfo, p_dev);
    aw_clk_enable(p_devinfo->clk_id);

    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __imx1050_ocotp_inst_connect(awbl_dev_t *p_dev)
{

    return;
}

static aw_err_t Delay4Busy(imx1050_ocotp_regs_t *p_regs)
{
    /* Wait until the busy flag clears */
    while (AW_REG_BITS_GET32(&p_regs->CTRL, 8, 1));

    /* process any errors */
    if (AW_REG_BITS_GET32(&p_regs->CTRL, 9, 1)) {

        /* clear the error bit so more writes can be done */
        writel(0x3FF, &p_regs->CTRL_CLR );
        return AW_ERROR;
    }
    return AW_OK;
}

aw_local aw_err_t  __imx1050_ocotp_fuse_burn(struct awbl_imx1050_ocotp_dev *p_dev,
                                             uint32_t                       addr,
                                             uint32_t                       data)
{
	__IMX6UL_OCOTP_DEVINFO_DECL(p_devinfo, p_dev);
    uint32_t            freq;
    uint32_t            STROBE_PROG,STROBE_READ,RELAX,WAIT;
    uint32_t            reg_val = 0;
    int                 result = AW_OK;
    imx1050_ocotp_regs_t *p_regs;

    p_regs = (imx1050_ocotp_regs_t *)p_devinfo->regbase;

    /* In order to avoid erroneous code performing erroneous writes to OTP,
     * a special unlocking sequence is required for writes.
     *
     * 1. Check that HW_OCOTP_CTRL_BUSY and HW_OCOTP_CTRL_ERROR are
     * clear. Overlapped accesses are not supported by the controller. Any pending
     * write must be completed before a write access can be requested.
     */
    Delay4Busy (p_regs);

    /* 2. Setup timing register - ipg_clk typically at 66.5MHz <=> 15ns
     * tRELAX = (OCOTP_TIMING[RELAX]+1)/ipg_frequency > 16.2ns
     *
     * tPGM = ((OCOTP_TIMING[STROBE_PROG]+1)- 2*(OCOTP_TIMING[RELAX]+1))/ipg_frequency
     * 9000ns < tPGM (typ 10000ns) < 11000ns
     *
     * tRD = ((OCOTP_TIMING[STROBE_READ]+1)- 2*(OCOTP_TIMING[RELAX]+1))/ipg_frequency > 36ns
     *
     * Default value of timing register is 0x01461299
     * => RELAX = 1 => tRELAX = 30ns
     * => STROBE_PROG = 299 => tPGM = 9930ns
     * => STROBE_READ = 6 => tRD = 45ns
     */

    freq = aw_clk_rate_get (p_devinfo->clk_id);
    freq /= 1000;

    RELAX = (30 * freq + 500 * 1000) / 1000 / 1000 ;
    STROBE_PROG = (9930 * freq + 500 * 1000) / 1000 / 1000;
    STROBE_PROG += 2 * RELAX;
    STROBE_READ = (45 * freq + 500 * 1000) / 1000 / 1000;
    STROBE_READ += 2 * RELAX;
    RELAX --;
    STROBE_READ --;
    STROBE_PROG --;

    WAIT = 5;

    reg_val = 0;
    reg_val = __OTP_TIMING_WAIT(WAIT)  |
              __OTP_TIMING_STROBE_READ(STROBE_READ) |
              __OTP_TIMING_RELAX(RELAX) |
              __OTP_TIMING_STROBE_PROG(STROBE_PROG);

    writel(reg_val, &p_regs->TIMING);

    result = Delay4Busy (p_regs);
    if (AW_OK != result){
        goto cleanup;
    }
    aw_udelay(100);


    /* 3. Write the requested address to HW_OCOTP_CTRL_ADDR and program the
     * unlock code into HW_OCOTP_CTRL_WR_UNLOCK. This must be programmed
     * for each write access. The lock code is documented in the XML register
     * description. Both the unlock code and address can be written in the same
     * operation.
     */

    reg_val = readl(&p_regs->CTRL);
    reg_val = __OTP_CTRL_WR_UNLOCK(BV_OCOTP_CTRL_WR_UNLOCK__KEY) |
               __OTP_CTRL_ADDR(addr);
    writel(reg_val, &p_regs->CTRL);

    /* 4. Write the data to HW_OCOTP_DATA. This will automatically set
     * HW_OCOTP_CTRL_BUSY and clear HW_OCOTP_CTRL_WR_UNLOCK. In
     * this case, the data is a programming mask. Bit fields with 1s will result in that
     * OTP bit being set. Only the controller can clear HW_OCOTP_CTRL_BUSY. The
     * controller will use the mask to program a 32-bit word in the OTP per the address
     * in ADDR. At the same time that the write is accepted, the controller makes an
     * internal copy of HW_OCOTP_CTRL_ADDR which cannot be updated until the
     * next write sequence is initiated. This copy guarantees that erroneous writes to
     * HW_OCOTP_CTRL_ADDR will not affect an active write operation. It should
     * also be noted that during the programming HW_OCOTP_DATA will shift right
     * (with zero fill). This shifting is required to program the OTP serially. During the
     * write operation, HW_OCOTP_DATA cannot be modified.
     */
    writel(data, &p_regs->DATA);

cleanup:
    Delay4Busy (p_regs);
    return result;

}



aw_local aw_err_t  __imx1050_ocotp_fuse_read(struct awbl_imx1050_ocotp_dev *p_dev,
                                               uint32_t                    addr,
                                               uint32_t                    *data)
{
    __IMX6UL_OCOTP_DEVINFO_DECL(p_devinfo, p_dev);
    imx1050_ocotp_regs_t      *p_regs = NULL;
    aw_err_t                   err = AW_OK;
    uint32_t                  reg_ctl = 0;

    p_regs = (imx1050_ocotp_regs_t *)p_devinfo->regbase;

    err = Delay4Busy(p_regs);
    if (err != AW_OK){
        goto cleanup;
    }

    /* Write the requested address into HW_OCOTP_CTRL register */
    reg_ctl = readl(&p_regs->CTRL);
    reg_ctl =  __OTP_CTRL_ADDR(addr);
    writel(reg_ctl, &p_regs->CTRL);

    /* Initiate a read of 4bytes at an OTP address */
    writel(1, &p_regs->READ_CTRL);

    /* Have to wait for busy bit to be cleared */
    Delay4Busy (p_regs);

    /* Finally, read the data */
    *data = readl(&p_regs->READ_FUSE_DATA);

cleanup:
    return err;
}

aw_local int8_t
__imx1050_ocotp_read_bytes_in_word (struct awbl_imx1050_ocotp_dev *p_dev,
                                    uint32_t                       addr,
                                    uint8_t                        start,
                                    uint8_t                        end,
                                    uint8_t                       *pbuf)
{
    uint8_t bits, nbytes;
    uint32_t data_reg;
    int ret;

    nbytes = end - start + 1;
    addr = addr / 4;

    ret = __imx1050_ocotp_fuse_read (p_dev, addr, &data_reg);
    if (ret < 0) {
        return ret;
    }

    while (1) {
        bits = start << 3;
        *pbuf = (data_reg >> bits) & 0x000000FF;
        pbuf++;
        if (end == start) {
            break;
        } else {
            start++;
        }
    }
    return nbytes;
}


aw_local int8_t
__imx1050_ocotp_burn_bytes_in_word (struct awbl_imx1050_ocotp_dev *p_dev,
                                    uint32_t                       addr,
                                    uint8_t                        start,
                                    uint8_t                        end,
                                    uint8_t                       *pbuf)
{
    uint8_t bits, nbytes;
    uint32_t data_reg = 0;
    int ret;

    nbytes = end - start + 1;
    addr = addr / 4;

    while (1) {
        bits = start << 3;
        AW_BITS_SET(data_reg, bits, 8, *pbuf);
        pbuf++;
        if (end == start) {
            break;
        }
        else {
            start++;
        }
    }
    ret = __imx1050_ocotp_fuse_burn (p_dev, addr, data_reg);
    if (ret < 0) {
        return ret;
    }

    return nbytes;
}



/**
 * \brief awbl_nvram_set method handler
 */
aw_local aw_err_t __imx1050_ocotp_rw (struct awbl_imx1050_ocotp_dev *p_dev,
                                      int                            start,
                                      uint8_t                       *p_buf,
                                      size_t                         len,
                                      aw_bool_t                      is_read)
{
    uint32_t maxsize = __IMX1050_OCOTP_TP_MAXSIZE_GET();
    uint32_t maxaddr = maxsize - 1;
    uint32_t len_tmp;
    aw_err_t err = AW_OK;

    uint8_t end;
    int nbytes;
    AW_INT_CPU_LOCK_DECL(lock);

    /* start address beyond this eeprom's capacity */
    if (start > maxaddr) {
        return -AW_ENXIO;
    }

    /* no data will be read or written */
    if (len == 0) {
        return AW_OK;
    }

    /* adjust len that will not beyond eeprom's capacity */
    if ((start + len) > maxsize) {
        len_tmp = maxsize - start;
    } else {
        len_tmp = len;
    }

    while (len_tmp) {

        end = (len_tmp > (4 - (start % 4))) ? 3 : ((start % 4) + (len_tmp - 1));

        /* lock the device */
        AW_INT_CPU_LOCK(lock);

        if (is_read) {

            nbytes = __imx1050_ocotp_read_bytes_in_word (p_dev, start, start % 4,
                                                         end, p_buf);

            AW_INT_CPU_UNLOCK(lock);

        }
        else {

            nbytes = __imx1050_ocotp_burn_bytes_in_word (p_dev, start, start % 4,
                                                         end, p_buf);
            AW_INT_CPU_UNLOCK(lock);
        }

        if (nbytes < 0) {
            goto cleanup;
            err =  nbytes;
        }
        p_buf += nbytes;
        start += nbytes;
        len_tmp -= nbytes;
    }
cleanup:
    return err;
}

/**
 * \brief awbl_nvram_get method handler
 */
aw_local aw_err_t __imx1050_ocotp_nvram_get (struct awbl_dev *p_dev,
                                             char            *p_name,
                                             int              unit,
                                             char            *p_buf,
                                             int              offset,
                                             int              len)
{
    __IMX6UL_OCOTP_DEVINFO_DECL(p_devinfo, p_dev);
    const struct awbl_nvram_segment *p_seg = p_devinfo->p_seglst;

    int i;
    int copylen;
    aw_err_t ret;

    for (i = 0; i < p_devinfo->seglst_count; i++) {
        if ((p_seg->unit == unit) && (strcmp (p_seg->p_name, p_name) == 0)) {
            copylen = len;
            if (offset + len > p_seg->seg_size) {
                copylen = p_seg->seg_size - offset;
                if (copylen < 0) {
                    return -AW_EIO;
                }
            }

            ret = __imx1050_ocotp_rw ((struct awbl_imx1050_ocotp_dev *) p_dev,
                                       p_seg->seg_addr + offset,
                                       (uint8_t *) p_buf,
                                       copylen,
                                       AW_TRUE);

            return ret != AW_OK ? -AW_EIO : AW_OK;
        }
        p_seg++;
    }

    return -AW_ENXIO;
}

/**
 * \brief awbl_nvram_set method handler
 */
aw_local aw_err_t __imx1050_ocotp_nvram_set (struct awbl_dev *p_dev,
                                             char            *p_name,
                                             int              unit,
                                             char            *p_buf,
                                             int              offset,
                                             int              len)
{
    __IMX6UL_OCOTP_DEVINFO_DECL(p_devinfo, p_dev);
    const struct awbl_nvram_segment *p_seg = p_devinfo->p_seglst;

    int i;
    int copylen;
    aw_err_t ret;

    for (i = 0; i < p_devinfo->seglst_count; i++) {
        if ((p_seg->unit == unit) && (strcmp(p_seg->p_name, p_name) == 0)) {
            copylen = len;
            if (offset + len > p_seg->seg_size) {
                copylen = p_seg->seg_size - offset;
                if (copylen < 0) {
                    return -AW_EIO;
                }
            }

            ret = __imx1050_ocotp_rw ((struct awbl_imx1050_ocotp_dev *) p_dev,
                                       p_seg->seg_addr + offset,
                                      (uint8_t *) p_buf,
                                       copylen,
                                       AW_FALSE);

            return ret != AW_OK ? -AW_EIO : AW_OK;
        }
        p_seg++;
    }

    return -AW_ENXIO;
}

/******************************************************************************/
void awbl_imx1050_ocotp_register (void)
{
    awbl_drv_register (
            (struct awbl_drvinfo *) &__g_imx1050_ocotp_drv_registration);
}

/* end of file */
