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
 * \brief Nandflash service interface
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-14  vih, first implementation
 * \endinternal
 */


#ifndef __AWBL_NAND_PF_H
#define __AWBL_NAND_PF_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

typedef struct awbl_nand_platform_serv  awbl_nand_platform_serv_t;
typedef struct awbl_nand_info           awbl_nand_info_t;

/** \brief  NandFlash芯片控制器时序 */
struct awbl_nand_timing {

    /* ns for all */
    uint8_t        tcs;        /**< \brief CE setup */
    uint8_t        tch;        /**< \brief CE hold */
    uint8_t        tcls;       /**< \brief CLE setup */
    uint8_t        tclh;       /**< \brief CLE hold */
    uint8_t        tals;       /**< \brief ALE setup */
    uint8_t        talh;       /**< \brief ALE hold */
    uint8_t        tds;        /**< \brief data setup */
    uint8_t        tdh;        /**< \brief data hold */
    uint8_t        trp;        /**< \brief RE pulse width */
    uint8_t        trea;       /**< \brief RE access time */
    uint8_t        tcea;       /**< \brief CE access time */
    uint8_t        tchz;       /**< \brief CE high to output Hi-Z */
    uint8_t        trc;        /**< \brief read cycle time */
    uint8_t        trhz;       /**< \brief RE high to output Hi-Z */
    uint8_t        tclr;       /**< \brief CLE to RE delay */
    uint8_t        twp;        /**< \brief WE pulse width */
    uint8_t        twc;        /**< \brief WE high hold time */
};


struct awbl_nand_platform_serv {
    struct awbl_nandbus         *p_bus;
    awbl_nand_info_t            *p_info;

    int     (*write_buf)            (awbl_nand_platform_serv_t *p_this,
                                     uint32_t                   page_addr,
                                     uint32_t                   col,
                                     uint8_t                   *buf,
                                     int                        len,
                                     uint8_t                   *oob_buf,
                                     uint16_t                   oob_buf_size,
                                     aw_bool_t                  oob_required);

    int     (*read_buf)             (awbl_nand_platform_serv_t *p_this,
                                     uint32_t                   page_addr,
                                     uint32_t                   col,
                                     uint8_t                   *buf,
                                     int                        len,
                                     uint8_t                   *oob_buf,
                                     uint16_t                   oob_buf_size,
                                     aw_bool_t                  oob_required);

    aw_bool_t   (*block_is_bad)     (awbl_nand_platform_serv_t *p_this,
                                     uint32_t                   page_addr);

    int      (*block_markbad)       (awbl_nand_platform_serv_t *p_this,
                                     uint32_t                   page_addr);

    int      (*erase)               (awbl_nand_platform_serv_t *p_this,
                                     uint32_t                   page_addr);

    int      (*reset)               (awbl_nand_platform_serv_t *p_this);

    aw_err_t (*read_id)             (awbl_nand_platform_serv_t *p_this,
                                     uint8_t                   *p_ids,
                                     uint8_t                    id_len);

    aw_bool_t (*probe)              (awbl_nand_platform_serv_t *p_this);
};

typedef struct {
    /** \brief 页大小 */
    uint16_t                   page_size;

    /** \brief 每页空闲区域(oob)大小 */
    uint8_t                    spare_size;

    /** \brief 每块的页个数 */
    uint16_t                   pages_per_blk;

    /** \brief 块个数 */
    uint32_t                   nblks;

    /** \brief 坏块标记位置相对于空闲块起始位置的偏移 */
    uint8_t                    bad_blk_byte_offs;

    /** \brief 每cell的位个数 */
    uint8_t                    bits_per_cell;

    /** \brief 块大小 */
    uint32_t                   blk_size;

    /** \brief 片大小 */
    uint64_t                   chip_size;

    /** \brief 制造商ID */
    uint8_t                    maker_id;

    /** \brief 设备ID */
    uint8_t                    device_id;
} awbl_nand_attr_t;

/**
 * \brief OOB布局信息
 */
struct awbl_nand_ooblayout {

    /** \brief 指向ECC位置描述数组 */
    const struct aw_mtd_oob_space *ptr_eccpos;

    /** \brief ECC位置描述数组成员个数 */
    int                            cnt_eccpos;

    /** \brief 指向空闲位置描述数组 */
    const struct aw_mtd_oob_space *ptr_freepos;

    /** \brief 空闲位置描述数组成员个数 */
    int                            cnt_freepos;
};


/** \brief NandFlash分区信息 */
struct awbl_nand_partition {

    /** \brief 分区名称 */
    const char                  *name;

    /** \brief 分区起始块编号 */
    uint32_t                     start_blk_num;

    /** \brief 分区终止块编号 */
    uint32_t                     end_blk_num;

        /** \brief 起始页偏移 */
    size_t                       page_offset;
};

/**
 * \brief ECC模式
 */
typedef enum awbl_nand_ecc_mode {

    AWBL_NAND_ECC_MODE_NONE = 0,    /**< \brief 无ECC */
    AWBL_NAND_ECC_MODE_SW_HAM,      /**< \brief 使用软件ECC，汉明码 */
    AWBL_NAND_ECC_MODE_SW_BCH,      /**< \brief 使用软件ECC，BCH算法 */

    AWBL_NAND_ECC_MODE_HW,          /**< \brief 使用硬件ECC */
    AWBL_NAND_ECC_MODE_HW_OOBFIRST  /**< \brief 使用硬件ECC，先读取OOB数据 */

} awbl_nand_ecc_mode_t;

typedef struct awbl_nand_info {
    awbl_nand_attr_t                     attr;
    uint8_t                              ecc_strength;
    uint8_t                              id_len;        /**< \brief ID的长度 */

    const struct awbl_nand_partition    *p_partition_tab;
    int                                  npartition;    /**< \brief 分区个数 */

    enum awbl_nand_ecc_mode              ecc_mode;

    /** \brief ECC配置信息 */
    const struct awbl_nand_ooblayout    *p_nand_ooblayout;

    void                                *p_platform_info;
} awbl_nand_info_t;

awbl_nand_platform_serv_t *awbl_nand_platform_serv_create (
        awbl_nand_info_t    *p_info,
        struct awbl_nandbus *p_bus);

void awbl_nand_platform_serv_destroy (awbl_nand_platform_serv_t *p_this);

void awbl_nand_serv_default_set (awbl_nand_platform_serv_t *p_pf_serv);


#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_NAND_PF_H */

/* end of file */
