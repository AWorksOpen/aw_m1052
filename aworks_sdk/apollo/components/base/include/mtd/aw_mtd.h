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
 * \brief AWorks MTD (Memory Technology Device) driver.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-24 orz, first implementation.
 * \endinternal
 */

#ifndef __AW_MTD_H /* { */
#define __AW_MTD_H

#include "apollo.h"
#include "aw_types.h"
#include "aw_list.h"
#include "aw_errno.h"

/**
 * \addtogroup grp_aw_mtd
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/** \brief MTD device types */
enum aw_mtd_type {
    AW_MTD_TYPE_ABSENT          = 0,
    AW_MTD_TYPE_RAM             = 1,
    AW_MTD_TYPE_ROM             = 2,
    AW_MTD_TYPE_NOR_FLASH       = 3,
    AW_MTD_TYPE_NAND_FLASH      = 4,
    AW_MTD_TYPE_DATA_FLASH      = 5,
    AW_MTD_TYPE_UBI_VOLUME      = 6,
    AW_MTD_TYPE_MLC_NAND_FLASH  = 7
};

/**
 * \name capabilities flags
 * @{
 */
#define AW_MTD_FLAG_WRITEABLE     0x400  /**< \brief device is writeable */
#define AW_MTD_FLAG_BIT_WRITEABLE 0x800  /**< \brief single bits can be flipped */
#define AW_MTD_FLAG_NO_ERASE      0x1000 /**< \brief no erase necessary */
#define AW_MTD_FLAG_POWERUP_LOCK  0x2000 /**< \brief always locked after reset */
/** @} */

/**
 * \name some common devices / combinations of capabilities
 * @{
 */
#define AW_MTD_FLAGS_ROM            0
#define AW_MTD_FLAGS_RAM            (AW_MTD_FLAG_WRITEABLE     | \
                                     AW_MTD_FLAG_BIT_WRITEABLE | \
                                     AW_MTD_FLAG_NO_ERASE)
#define AW_MTD_FLAGS_NOR_FLASH      (AW_MTD_FLAG_WRITEABLE     | \
                                     AW_MTD_FLAG_BIT_WRITEABLE)
#define AW_MTD_FLAGS_NAND_FLASH     (AW_MTD_FLAG_WRITEABLE)

#define AW_MTD_FLAGS_SDCARD         (AW_MTD_FLAG_WRITEABLE)
/** @} */

#define AW_MTD_PART_SIZ_FULL        0

/** \brief MTD NAND device ECC mode */
enum aw_mtd_nand_ecc_mode {
    AW_MTD_NAND_ECC_OFF        = 0, /**< switch off ECC (Not recommended) */
    AW_MTD_NAND_ECC_PLACE      = 1, /**< use the given placement by hardware */
    AW_MTD_NAND_ECC_AUTO_PLACE = 2, /**< use the default placement scheme */

    /**
     * \brief Use the given placement by hardware.
     *  (Do not store ecc result on read)
     */
    AW_MTD_NAND_ECC_PLACE_ONLY = 3,

    /**
     * \brief Use the given auto placement scheme rather than
     * using the default
     */
    AW_MTD_NAND_ECC_AUTO_PLACE_USER = 4
};

/** \brief MTD device erase status */
enum aw_mtd_erase_status {
    AW_MTD_ERASE_PENDING    = 0x01,
    AW_MTD_ERASE_PROCESSING = 0x02,
    AW_MTD_ERASE_SUSPEND    = 0x04,
    AW_MTD_ERASE_DONE       = 0x08,
    AW_MTD_ERASE_FAILED     = 0x10,
};

/** \brief MTD operation modes */
enum aw_mtd_oob_mode {
    /** OOB data are placed at the given offset (default) */
    AW_MTD_OOB_MODE_PLACE = 0,
    /**
     * \brief OOB data are automatically placed at the free areas which are defined
     * by the internal ECC layout.
     */
    AW_MTD_OOB_MODE_AUTO = 1,
    /**
     * \brief OOB data are transferred as-is, with no error correction,
     * this mode implies \a AW_MTD_OOB_MODE_PLACE.
     */
    AW_MTD_OOB_MODE_RAW  = 2,
};

/******************************************************************************/
#define AW_MTD_ERASE_FAIL_ADDR_UNKNOWN  -1LL

/******************************************************************************/
struct aw_mtd_info;

/******************************************************************************/
struct aw_mtd_erase_region_info {
    /** brief At which this region starts, from the beginning of the MTD */
    off_t offset;
    /** \brief erase size for this region */
    uint_t erase_size;
    /** \brief Number of blocks of \a erase_size in this region */
    uint_t num_blocks;
};

/**
 * \brief If the erase fails, \a fail_addr might indicate exactly which block 
 * failed.If \a fail_addr = AW_MTD_ERASE_FAIL_ADDR_UNKNOWN, the failure was not
 * at the device level or was not specific to any particular block.
 */
struct aw_mtd_erase_info {
    struct aw_mtd_info         *mtd;
    off_t                       addr;
    size_t                      len;
    off_t                       fail_addr;
    uint_t                      retries;
    void                      (*callback) (struct aw_mtd_erase_info *self);
    void                       *priv;
    enum aw_mtd_erase_status    state;
    struct aw_mtd_erase_info   *next;
};

/**
 * \brief OOB operation operands
 * \note it is allowed to read more than one OOB area at one go, but not write.
 * The interface assumes that the OOB write requests program only one page's
 * OOB area.
 */
struct aw_mtd_oob_ops {
    enum aw_mtd_oob_mode mode;  /**< operation mode */

    size_t len;         /**< \brief number of data bytes to write/read */
    size_t ret_len;     /**< \brief number of data bytes written/read */
    size_t oob_len;     /**< \brief number of OOB bytes to write/read */
    size_t oob_ret_len; /**< \brief number of OOB bytes written/read */

    /**
     * \brief offset of OOB data in the OOB area (only relevant
     * when mode = AW_MTD_OPS_PLACE_OOB or AW_MTD_OPS_RAW)
     */
    off_t oob_offs;

    void *data_buf; /**< \brief data buffer, if NULL only OOB data are read/written */
    void *oob_buf;  /**< \brief OOB data buffer */
};

/** \brief MTD OOB space descriptor */
struct aw_mtd_oob_space {
    uint8_t offset; /**< \brief offset into OOB of this free area */
    uint8_t length; /**< \brief length of this free area */
};

/** \brief MTD OOB layout */
struct aw_mtd_oob_layout {
    uint8_t  oob_avail;      /**< \brief available OOB bytes */
    uint8_t  bbm_pos;        /**< \brief bad block mark positions */
    uint8_t  ecc_bytes;      /**< \brief number of ECC bytes */
    const struct aw_mtd_oob_space *ecc_pos;      /**< \brief ECC positions */
    const struct aw_mtd_oob_space *oob_free;     /**< \brief free OOB positions */
};

/** \brief MTD driver operations */
struct aw_mtd_ops {
    int  (*mtd_erase) (struct aw_mtd_info *mtd, struct aw_mtd_erase_info *i);
    void (*mtd_sync)  (struct aw_mtd_info *mtd);

    int (*mtd_read)  (struct aw_mtd_info *mtd,
                      off_t               from,
                      size_t              len,
                      size_t             *retlen,
                      void               *buf);
    int (*mtd_write) (struct aw_mtd_info *mtd,
                      off_t               to,
                      size_t              len,
                      size_t             *retlen,
                      const void         *buf);

    int (*mtd_panic_write) (struct aw_mtd_info *mtd,
                            off_t               to,
                            size_t              len,
                            size_t             *retlen,
                            const void         *buf);

    int (*mtd_oob_read)  (struct aw_mtd_info    *mtd,
                          off_t                  from,
                          struct aw_mtd_oob_ops *ops);
    int (*mtd_oob_write) (struct aw_mtd_info    *mtd,
                          off_t                  to,
                          struct aw_mtd_oob_ops *ops);

    int (*mtd_block_is_bad)   (struct aw_mtd_info *mtd, off_t ofs);
    int (*mtd_block_mark_bad) (struct aw_mtd_info *mtd, off_t ofs);

    int (*mtd_is_locked) (struct aw_mtd_info *mtd, off_t ofs, size_t len);
    int (*mtd_lock)      (struct aw_mtd_info *mtd, off_t ofs, size_t len);
    int (*mtd_unlock)    (struct aw_mtd_info *mtd, off_t ofs, size_t len);
};

/** \brief MTD information */
struct aw_mtd_info {
    struct aw_list_head list;
    const char         *name;
    void               *data; /**< \brief device private data */

    enum aw_mtd_type    type;
    uint_t              flags;
    size_t              size;       /**< \brief total size of the MTD */
    uint_t              erase_size; /**< \brief "Major" erase size for the device */
    /**
     * \brief  Minimal writable unit size. In case of NOR flash it is 1 (even
     * though individual bits can be cleared), in case of NAND flash it is
     * one NAND page (or half, or one-fourth of it), in case of ECC-ed NOR
     * it is of ECC block size, etc. It is illegal to have \a writesize = 0.
     * Any driver registering a struct aw_mtd_info must ensure a \a writesize of
     * 1 or larger.
     */
    uint_t write_size;
    /**
     * \brief  Size of the write buffer used by the MTD. MTD devices having a write
     * buffer can write multiple \a writesize chunks at a time. E.g. while
     * writing 4 * \a writesize bytes to a device with 2 * \a writesize bytes
     * buffer the MTD driver can (but doesn't have to) do 2 \a writesize
     * operations, but not 4. Currently, all NANDs have \a write_buf_size
     * equivalent to \a write_size (NAND page size). Some NOR flashes do have
     * \a write_buf_size greater than \a write_size.
     */
    uint_t write_buf_size;

    uint_t oob_size;   /**< \brief Amount of OOB data per block (e.g. 16) */
    uint_t oob_avail;  /**< \brief Available OOB bytes per block */

    /**
     * \brief Read ops return -EUCLEAN if max number of bit flips corrected 
     * on any one region comprising an ECC step equals or exceeds this value.
     * Settable by driver, else defaults to ecc_strength.
     */
    uint_t bitflip_threshold;

    /** \brief  max number of correctable bit errors per ECC step */
    uint_t ecc_strength;

    /** \brief  OOB layout description data */
    const struct aw_mtd_oob_layout *oob_layout;

    const struct aw_mtd_ops *ops;
};

/** \brief MTD partition info structure */
struct aw_mtd_part_info {
    struct aw_mtd_info  mtd;    /**< \brief MTD device of the partition */
    struct aw_mtd_info *master; /**< \brief Master MTD device */
    off_t               offset; /**< \brief offset into master MTD device */
    struct aw_list_head list;   /**< \brief list node in partition list */
};

/** \brief MTD partition */
struct aw_mtd_partition {
    const char *name;       /**< \brief identifier string */
    size_t      offset;     /**< \brief offset within the master MTD space */
    size_t      size;       /**< \brief partition size */
    unsigned    mask_flags; /**< \brief master MTD flags to mask out */
};

/******************************************************************************/
void aw_mtd_lib_init (void);

/******************************************************************************/
struct aw_mtd_info *aw_mtd_dev_find (const char *name);

aw_err_t aw_mtd_dev_add (struct aw_mtd_info *mtd, const char *name, void *data);
aw_err_t aw_mtd_dev_del (struct aw_mtd_info *mtd);

/******************************************************************************/
int  aw_mtd_erase (struct aw_mtd_info *mtd, struct aw_mtd_erase_info *ei);
void aw_mtd_sync  (struct aw_mtd_info *mtd);

int aw_mtd_read (struct aw_mtd_info *mtd,
                 off_t               from,
                 size_t              len,
                 size_t             *retlen,
                 void               *buf);

int aw_mtd_write (struct aw_mtd_info *mtd,
                  off_t               to,
                  size_t              len,
                  size_t             *retlen,
                  const void         *buf);

int aw_mtd_panic_write (struct aw_mtd_info *mtd,
                        off_t               to,
                        size_t              len,
                        size_t             *retlen,
                        const void         *buf);

int aw_mtd_oob_read (struct aw_mtd_info    *mtd,
                     off_t                  from,
                     struct aw_mtd_oob_ops *ops);

int aw_mtd_oob_write (struct aw_mtd_info    *mtd,
                      off_t                  to,
                      struct aw_mtd_oob_ops *ops);

int aw_mtd_block_is_bad   (struct aw_mtd_info *mtd, off_t ofs);
int aw_mtd_block_mark_bad (struct aw_mtd_info *mtd, off_t ofs);

int aw_mtd_is_locked (struct aw_mtd_info *mtd, off_t ofs, size_t len);
int aw_mtd_lock      (struct aw_mtd_info *mtd, off_t ofs, size_t len);
int aw_mtd_unlock    (struct aw_mtd_info *mtd, off_t ofs, size_t len);

/******************************************************************************/
void aw_mtd_erase_callback (struct aw_mtd_erase_info *ei);

/******************************************************************************/
void aw_mtd_part_lib_init (void);

aw_bool_t aw_mtd_is_partition (struct aw_mtd_info *mtd);

aw_err_t aw_mtd_part_add (struct aw_mtd_part_info *part,
                          struct aw_mtd_info      *master,
                          const char              *name,
                          off_t                    offset,
                          size_t                   size,
                          void                    *data);

aw_err_t aw_mtd_part_del (struct aw_mtd_part_info *part);


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* } __AW_MTD_H */

/* end of file */
