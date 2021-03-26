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
 * \brief  ep24cxx eeprom driver
 *
 * this driver is the AWBus driver implementation of 24cxx EEPROM chips
 *
 * \internal
 * \par modification history:
 * - 1.02 13-01-17  zen, fix bug: the argument OFFSET was not used in
 *                  __ep24cxx_nvram_set() and __ep24cxx_nvram_get().
 * - 1.01 12-12-03  zen, change return value type from int to aw_err_t
 * - 1.00 12-10-22  zen, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include <string.h>

#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_delay.h"
#include "aw_sem.h"

#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "awbl_nvram.h"

#include "driver/nvram/awbl_ep24cxx.h"

/*******************************************************************************
  defines
*******************************************************************************/
#define __EP24CXX_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_ep24cxx_devinfo *p_devinfo = \
        (struct awbl_ep24cxx_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __EP24CXX_DEV_LOCK_INIT(p_dev) \
    do { \
        AW_MUTEX_INIT(((struct awbl_ep24cxx_dev *)\
            (p_dev))->devlock, AW_SEM_Q_PRIORITY ); \
    } while(0)

#define __EP24CXX_DEV_LOCK(p_dev) \
    do { \
        AW_MUTEX_LOCK(((struct awbl_ep24cxx_dev *)(p_dev))->devlock, \
                      AW_SEM_WAIT_FOREVER); \
    } while(0)

#define __EP24CXX_DEV_UNLOCK(p_dev) \
    do { \
        AW_MUTEX_UNLOCK(((struct awbl_ep24cxx_dev *)(p_dev))->devlock); \
    } while(0)

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __ep24cxx_inst_init1(awbl_dev_t *p_dev);
aw_local void __ep24cxx_inst_init2(awbl_dev_t *p_dev);
aw_local void __ep24cxx_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __ep24cxx_nvram_get(struct awbl_dev *p_dev,
                                      char            *p_name,
                                      int              unit,
                                      char            *p_buf,
                                      int              offset,
                                      int              len);

aw_local aw_err_t __ep24cxx_nvram_set(struct awbl_dev *p_dev,
                                      char            *p_name,
                                      int              unit,
                                      char            *p_buf,
                                      int              offset,
                                      int              len);

/*******************************************************************************
  locals
*******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_ep24cxx_drvfuncs = {
    __ep24cxx_inst_init1,
    __ep24cxx_inst_init2,
    __ep24cxx_inst_connect
};

/** \brief driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_nvram_get);
AWBL_METHOD_IMPORT(awbl_nvram_set);

aw_local aw_const struct awbl_dev_method __g_ep24cxx_dev_methods[] = {
    AWBL_METHOD(awbl_nvram_get, __ep24cxx_nvram_get),
    AWBL_METHOD(awbl_nvram_set, __ep24cxx_nvram_set),
    AWBL_METHOD_END
};

/** \brief driver registration (must defined as aw_const) */
aw_local aw_const awbl_i2c_drvinfo_t __g_ep24cxx_drv_registration = {
    {
        AWBL_VER_1,                     /**< \brief awb_ver */
        AWBL_BUSID_I2C,                 /**< \brief bus_id */
        AWBL_EP24CXX_NAME,              /**< \brief p_drvname */
        &__g_ep24cxx_drvfuncs,          /**< \brief p_busfuncs */
        &__g_ep24cxx_dev_methods[0],    /**< \brief p_methods */
        NULL                            /**< \brief pfunc_drv_probe */
    }
};



/** \brief get type's max sizes (byte) */
#define __EP24CXX_TP_MAXSIZE_GET(tpid) \
    ((1 << AW_BITS_GET(tpid, 20, 4)) << 7)

/** \brief get type's page size (byte) */
#define __EP24CXX_TP_PGSIZE_GET(tpid) \
    AW_BITS_GET(tpid, 0, 15)

/** \brief get type's data address bits number in slave address (bit) */
#define __EP24CXX_TP_REG_BITLEN_OV_GET(tpid) \
    AW_BITS_GET(tpid, 16, 3)

/** \brief get type's register address length (byte) */
#define __EP24CXX_TP_REG_BYTELEN_GET(tpid) \
    (AW_BIT_ISSET(tpid, 19) ? 2 : 1)

/** \brief get type's write time */
#define __EP24CXX_TP_WRITE_TIME_GET(tpid) \
    AW_BITS_GET(tpid, 24, 8)

/** \brief are there page (aw_bool_t) */
#define __EP24CXX_TP_HAS_PAGE(tpid) \
        (AW_BIT_ISSET(tpid, 15))

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __ep24cxx_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __ep24cxx_inst_init2(awbl_dev_t *p_dev)
{
    __EP24CXX_DEV_LOCK_INIT(p_dev);

    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __ep24cxx_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief program ep24cxx
 */
aw_local aw_err_t __ep24cxx_program_data (struct awbl_ep24cxx_dev *p_dev,
                                          uint32_t                 subaddr,
                                          uint8_t                 *p_buf,
                                          uint32_t                 len,
                                          aw_bool_t                is_read)
{
    __EP24CXX_DEVINFO_DECL(p_devinfo, p_dev);
    aw_err_t ret;

    uint32_t slave_addr    = p_devinfo->addr;
    unsigned reg_bitlen_ov = __EP24CXX_TP_REG_BITLEN_OV_GET(p_devinfo->type);
    unsigned reg_bytelen   = __EP24CXX_TP_REG_BYTELEN_GET(p_devinfo->type);
    unsigned reg_bitlen    = reg_bytelen << 3;
    unsigned char twr      = __EP24CXX_TP_WRITE_TIME_GET(p_devinfo->type);

    /*
     * calculate slave address and register address,
     * some register bits are embeded in slave address
     */
    if (reg_bitlen_ov) {
        unsigned ovbits = AW_BITS_GET(subaddr, reg_bitlen, reg_bitlen_ov);
        AW_BITS_SET(slave_addr,
                    0,
                    reg_bitlen_ov,
                    ovbits);
    }

    /* read/write eeprom */
    ret = is_read  ?
          awbl_i2c_read((struct awbl_i2c_device *)p_dev,
                        AW_I2C_ADDR_7BIT | (reg_bytelen == 1 ?
                            AW_I2C_SUBADDR_1BYTE : AW_I2C_SUBADDR_2BYTE),
                        slave_addr,
                        subaddr,
                        p_buf,
                        len)
       : awbl_i2c_write((struct awbl_i2c_device *)p_dev,
                        AW_I2C_ADDR_7BIT | (reg_bytelen == 1 ?
                            AW_I2C_SUBADDR_1BYTE : AW_I2C_SUBADDR_2BYTE),
                        slave_addr,
                        subaddr,
                        p_buf,
                        len);
    if (ret != AW_OK) {
        return ret;
    }

    /* waiting for program done */
    /* todo: this delay should add a systick time ? */
    if (( !is_read ) && (twr)) {
#if 0
        aw_mdelay(p_devinfo->twr);
#else
        /* add one more tick to make sure has enough time to program */
        aw_task_delay(twr * aw_sys_clkrate_get() / 1000 + 1);
#endif
    }

    return AW_OK;
}


/**
 * \brief awbl_nvram_set method handler
 */
aw_local aw_err_t __ep24cxx_rw (struct awbl_ep24cxx_dev *p_dev,
                                int                      start,
                                uint8_t                 *p_buf,
                                size_t                   len,
                                aw_bool_t                is_read)
{
    __EP24CXX_DEVINFO_DECL(p_devinfo, p_dev);
#if 0     
    __EP24CXX_TP_CAP_GET(p_devinfo->type) << 7;  /* byte */
#endif     
    uint32_t maxsize = __EP24CXX_TP_MAXSIZE_GET(p_devinfo->type);
    uint32_t maxaddr = maxsize - 1;
    uint32_t page    = __EP24CXX_TP_PGSIZE_GET(p_devinfo->type);
    aw_bool_t     has_page = __EP24CXX_TP_HAS_PAGE(p_devinfo->type) ? 
                             AW_TRUE : AW_FALSE;
    uint32_t len_tmp;
    aw_err_t ret;

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
        len = maxsize - start;
    }

    /* if page == 0 or read op. , this means there is no page limit */
    if ((!has_page ) ||
        (page == 0)) {

        page = maxsize;
    }

    /* write the unaligned data of the start */

    len_tmp =  AW_ROUND_DOWN(start + page, page) - start;
    if (len < len_tmp) {
        len_tmp = len;
    }

    /* lock the device */
    __EP24CXX_DEV_LOCK(p_dev);

    ret = AW_OK;
    ret = __ep24cxx_program_data(p_dev, start, p_buf, len_tmp, is_read);
    if (ret != AW_OK) {
        ret = -AW_EIO;
        goto exit;
    }
    len   -= len_tmp;
    start += len_tmp;
    p_buf += len_tmp;

    /* write the rest data */

    while (len) {
        len_tmp = len > page ? page : len;
        ret = __ep24cxx_program_data(p_dev, start, p_buf, len_tmp, is_read);
        if (ret < 0) {
            ret = -AW_EIO;
            goto exit;
        }
        len   -= len_tmp;
        start += len_tmp;
        p_buf += len_tmp;
    }

exit:
    __EP24CXX_DEV_UNLOCK(p_dev);

    return ret;
}

/**
 * \brief awbl_nvram_get method handler
 */
aw_local aw_err_t __ep24cxx_nvram_get (struct awbl_dev *p_dev,
                                       char            *p_name,
                                       int              unit,
                                       char            *p_buf,
                                       int              offset,
                                       int              len)
{
    __EP24CXX_DEVINFO_DECL(p_devinfo, p_dev);
    const struct awbl_nvram_segment *p_seg = p_devinfo->p_seglst;

    int      i;
    int      copylen;
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

            ret = __ep24cxx_rw((struct awbl_ep24cxx_dev *)p_dev,
                                p_seg->seg_addr + offset,
                                (uint8_t *)p_buf,
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
aw_local aw_err_t __ep24cxx_nvram_set (struct awbl_dev *p_dev,
                                       char            *p_name,
                                       int              unit,
                                       char            *p_buf,
                                       int              offset,
                                       int              len)
{
    __EP24CXX_DEVINFO_DECL(p_devinfo, p_dev);
    const struct awbl_nvram_segment *p_seg = p_devinfo->p_seglst;

    int      i;
    int      copylen;
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

            ret = __ep24cxx_rw((struct awbl_ep24cxx_dev *)p_dev,
                                p_seg->seg_addr + offset,
                                (uint8_t *)p_buf,
                                copylen,
                                AW_FALSE);
            return ret != AW_OK ? -AW_EIO : AW_OK;
        }
        p_seg++;
    }

    return -AW_ENXIO;
}

/******************************************************************************/
void awbl_ep24cxx_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_ep24cxx_drv_registration);
}

/* end of file */
