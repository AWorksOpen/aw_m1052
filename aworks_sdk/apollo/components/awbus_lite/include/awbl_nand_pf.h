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

/** \brief  NandFlashоƬ������ʱ�� */
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
    /** \brief ҳ��С */
    uint16_t                   page_size;

    /** \brief ÿҳ��������(oob)��С */
    uint8_t                    spare_size;

    /** \brief ÿ���ҳ���� */
    uint16_t                   pages_per_blk;

    /** \brief ����� */
    uint32_t                   nblks;

    /** \brief ������λ������ڿ��п���ʼλ�õ�ƫ�� */
    uint8_t                    bad_blk_byte_offs;

    /** \brief ÿcell��λ���� */
    uint8_t                    bits_per_cell;

    /** \brief ���С */
    uint32_t                   blk_size;

    /** \brief Ƭ��С */
    uint64_t                   chip_size;

    /** \brief ������ID */
    uint8_t                    maker_id;

    /** \brief �豸ID */
    uint8_t                    device_id;
} awbl_nand_attr_t;

/**
 * \brief OOB������Ϣ
 */
struct awbl_nand_ooblayout {

    /** \brief ָ��ECCλ���������� */
    const struct aw_mtd_oob_space *ptr_eccpos;

    /** \brief ECCλ�����������Ա���� */
    int                            cnt_eccpos;

    /** \brief ָ�����λ���������� */
    const struct aw_mtd_oob_space *ptr_freepos;

    /** \brief ����λ�����������Ա���� */
    int                            cnt_freepos;
};


/** \brief NandFlash������Ϣ */
struct awbl_nand_partition {

    /** \brief �������� */
    const char                  *name;

    /** \brief ������ʼ���� */
    uint32_t                     start_blk_num;

    /** \brief ������ֹ���� */
    uint32_t                     end_blk_num;

        /** \brief ��ʼҳƫ�� */
    size_t                       page_offset;
};

/**
 * \brief ECCģʽ
 */
typedef enum awbl_nand_ecc_mode {

    AWBL_NAND_ECC_MODE_NONE = 0,    /**< \brief ��ECC */
    AWBL_NAND_ECC_MODE_SW_HAM,      /**< \brief ʹ�����ECC�������� */
    AWBL_NAND_ECC_MODE_SW_BCH,      /**< \brief ʹ�����ECC��BCH�㷨 */

    AWBL_NAND_ECC_MODE_HW,          /**< \brief ʹ��Ӳ��ECC */
    AWBL_NAND_ECC_MODE_HW_OOBFIRST  /**< \brief ʹ��Ӳ��ECC���ȶ�ȡOOB���� */

} awbl_nand_ecc_mode_t;

typedef struct awbl_nand_info {
    awbl_nand_attr_t                     attr;
    uint8_t                              ecc_strength;
    uint8_t                              id_len;        /**< \brief ID�ĳ��� */

    const struct awbl_nand_partition    *p_partition_tab;
    int                                  npartition;    /**< \brief �������� */

    enum awbl_nand_ecc_mode              ecc_mode;

    /** \brief ECC������Ϣ */
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
