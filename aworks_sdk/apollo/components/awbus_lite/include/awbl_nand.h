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
 * \brief Nand head file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-3-22  vih, first implementation
 * - 2.00 20-04-14 vih, refactoring
 * \endinternal
 */

#ifndef __AWBL_NAND_H
#define __AWBL_NAND_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "awbus_lite.h"
#include "mtd/aw_mtd.h"
#include "aw_sem.h"
#include "awbl_nand_pf.h"
#include "awbl_nand_ecc.h"


/** \brief NandFlash�������� */
#define AWBL_NAND_DRV_NAME        "awbl_nand_drv_name"


/* standard command */
#define AWBL_NANDFLASH_CMD_READ0             0
#define AWBL_NANDFLASH_CMD_READ1             1
#define AWBL_NANDFLASH_CMD_RNDOUT            5
#define AWBL_NANDFLASH_CMD_PAGEPROG          0x10
#define AWBL_NANDFLASH_CMD_READOOB           0x50
#define AWBL_NANDFLASH_CMD_ERASE1            0x60
#define AWBL_NANDFLASH_CMD_STATUS            0x70
#define AWBL_NANDFLASH_CMD_SEQIN             0x80
#define AWBL_NANDFLASH_CMD_RNDIN             0x85
#define AWBL_NANDFLASH_CMD_READID            0x90
#define AWBL_NANDFLASH_CMD_ERASE2            0xd0
#define AWBL_NANDFLASH_CMD_PARAM             0xec
#define AWBL_NANDFLASH_CMD_GET_FEATURES      0xee
#define AWBL_NANDFLASH_CMD_SET_FEATURES      0xef
#define AWBL_NANDFLASH_CMD_RESET             0xff
#define AWBL_NANDFLASH_CMD_LOCK              0x2a
#define AWBL_NANDFLASH_CMD_UNLOCK1           0x23
#define AWBL_NANDFLASH_CMD_UNLOCK2           0x24
/* expand command for large page */
#define AWBL_NANDFLASH_CMD_READSTART         0x30
#define AWBL_NANDFLASH_CMD_RNDOUTSTART       0xE0
#define AWBL_NANDFLASH_CMD_CACHEDPROG        0x15


#define AWBL_NANDFLASH_ECC_NO_ERR           0x00
#define AWBL_NANDFLASH_ECC_ERR_CORRECTED    0x01
#define AWBL_NANDFLASH_ECC_ERR_UNCORRECTED  0x02

typedef struct awbl_nand_chip   awbl_nand_t;

/**
 * \brief  ioctl������ѡ��
 */
typedef enum awbl_nand_ioctl_req {

    /** \brief ��ȡҳ��С */
    AWBL_NAND_PAGE_SIZE,

    /** \brief ��ȡÿ����ҳ������ */
    AWBL_NAND_PAGE_NUM_PER_BLK,

    /** \brief ��ȡ���С */
    AWBL_NAND_BLK_SIZE,

    /** \brief ��ȡ��������� */
    AWBL_NAND_BLK_NUM,

    /** \brief ��ȡ���õ�oob��С */
    AWBL_NAND_OOB_SIZE,

    /** \brief ��ȡƬ��С  */
    AWBL_NAND_CHIP_SIZE,

} awbl_nand_ioctl_req_t;



typedef void   (*awbl_nand_log_t) (void         *p_color,
                                   const char   *func_name,
                                   int           line,
                                   const char   *fmt, ...);

typedef struct awbl_nand_init {
    awbl_nand_info_t            *p_info;
    awbl_nand_log_t              pfn_log;
    awbl_nand_ecc_t             *p_ecc;
    awbl_nand_platform_serv_t   *p_pf_serv;
} awbl_nand_init_t;

struct awbl_nand_chip {

    /** \brief �����豸 */
    struct awbl_dev              awdev;
    awbl_nand_init_t             init;
    awbl_nand_info_t            *p_info;  /**< \brief �� hwconf �ļ��ṩ  */
    struct aw_mtd_info           mtd;
    struct aw_mtd_part_info     *p_mtd_part;
    uint8_t                      mtd_part_cnt;
    AW_MUTEX_DECL(               op_mutex);
    awbl_nand_platform_serv_t   *p_pf_serv;

    /** \brief log2(page_size) */
    int                          page_shift;
    int                          phys_erase_shift;
    int                          erase_align_mask;
    int                          pagemask;

    /** \brief ���ݻ��棬page+oobsize */
    uint8_t                     *page_buf;
    uint8_t                     *oob_buf;

    awbl_nand_ecc_t             *p_ecc;

    /** \brief oob���� */
    struct aw_mtd_oob_layout     oob_layout;

    void                        *priv;
};

aw_err_t awbl_nand_platform_serv_register (
        awbl_nand_platform_serv_t *(*pfn_platform_serv_create) (
                awbl_nand_info_t    *,
                struct awbl_nandbus *),
        void (*pfn_platform_serv_destroy) (awbl_nand_platform_serv_t *));

void awbl_nand_init (awbl_nand_init_t *p_init, awbl_nand_t *p_nand);

void awbl_nand_uninit (awbl_nand_t *p_nand);

/**
 * \brief nand�����ݣ���ecc���飬���ܿ�ҳ
 *
 * \param[in] p_chip           nand�豸�ṹ��
 * \param[in] page             Ҫ����ҳ��ַ
 * \param[in] col              Ҫ����ҳ�ڵ�ַ
 * \param[in] buf              ������ݵ�buf
 * \param[in] len              ������
 *
 * \retval �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t awbl_nand_read (struct awbl_nand_chip *p_chip,
                         uint32_t               page,
                         uint32_t               col,
                         uint8_t               *buf,
                         int                    len);

/**
 * \brief nandд���ݣ���ecc���飬���ܿ�ҳ
 *
 * \param[in] p_chip           nand�豸�ṹ��
 * \param[in] page             ҳ��ַ
 * \param[in] col              ҳ�ڵ�ַ
 * \param[in] buf              ������ݵ�buf
 * \param[in] len              �����ĳ���
 *
 * \retval �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t awbl_nand_write (struct awbl_nand_chip *p_chip,
                          uint32_t               page,
                          uint32_t               col,
                          uint8_t               *buf,
                          int                    len);

/**
 * \brief nandҳ�����ݣ���ecc���飬һ�β�������Ϊһ��ҳ
 *
 * \param[in] p_chip           nand�豸�ṹ��
 * \param[in] ipage            ҳ��ַ
 * \param[out] buf             ������ݵ�buf
 *
 * \retval �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t awbl_nand_page_read (struct awbl_nand_chip *p_chip,
                              uint8_t               *buf,
                              uint32_t               ipage);

/**
 * \brief nandҳд���ݣ���ecc���飬һ�β�������Ϊһ��ҳ
 *
 * \param[in] p_chip           nand�豸�ṹ��
 * \param[in] ipage            ҳ��ַ
 * \param[in] buf              ������ݵ�buf
 *
 * \retval �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t awbl_nand_page_write (struct awbl_nand_chip *p_chip,
                               uint8_t               *buf,
                               uint32_t               ipage);

/**
 * \brief дoob����free��
 *
 * \param[in] p_chip           nand�豸�ṹ��
 * \param[in] ipage            ҳ��ַ
 * \param[in] offs             ��free����ƫ��
 * \param[in] oob_data         ����buff
 * \param[in] data_len         д�볤��
 *
 * \retval  �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 *
 * \note    ��oob��д��ʱ��Ӧ��֪��free���Ĵ�С����д���Ȳ��ɳ���free���Ĵ�С��
 *          ����awbl_nand_ioctl(p_chip, AWBL_NAND_OOB_SIZE, &free_szie);
 *          �ɵõ�free���Ĵ�С
 */
aw_err_t awbl_nand_oob_write (struct awbl_nand_chip    *p_chip,
                              uint32_t                  ipage,
                              uint32_t                  offs,
                              uint8_t                  *oob_data,
                              uint16_t                  data_len);

/**
  * \brief ��oob����free��
 *
 * \param[in] p_chip           nand�豸�ṹ��
 * \param[in] ipage            ҳ��ַ
 * \param[in] offs             ��free����ƫ��
 * \param[in] oob_data         ����buff
 * \param[in] data_len         ������
 *
 * \retval �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 *
 * \note    ��oob����дʱ��Ӧ��֪��free���Ĵ�С����д���Ȳ��ɳ���free���Ĵ�С��
 *          ����awbl_nand_ioctl(p_chip, AWBL_NAND_OOB_SIZE, &free_szie);
 *          �ɵõ�free���Ĵ�С
 */
aw_err_t awbl_nand_oob_read (struct awbl_nand_chip    *p_chip,
                             uint32_t                  ipage,
                             uint32_t                  offs,
                             uint8_t                  *oob_data,
                             uint16_t                  data_len);

/**
 * \brief �����
 *
 * \param[in] p_chip           nand�豸�ṹ��
 * \param[in] iblk             ���ַ
 *
 * \retval �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t awbl_nand_blk_erase (struct awbl_nand_chip *p_chip,
                              uint32_t               iblk);

/**
 * \brief �жϻ���
 *
 * \param[in] p_chip           nand�豸�ṹ��
 * \param[in] iblk             ���ַ
 *
 * \retval �����жϽ��
 */
aw_bool_t awbl_nand_blk_is_bad (struct awbl_nand_chip *p_chip,
                                uint32_t               iblk);

/**
 * \brief ��ǻ���
 *
 * \param[in] p_chip           nand�豸�ṹ��
 * \param[in] iblk             ���ַ
 *
 * \retval �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t awbl_nand_blk_markbad (struct awbl_nand_chip *p_chip,
                                uint32_t               iblk);

/**
 * \brief IOCTL
 *
 * \param[in]     p_chip           nand�豸�ṹ��
 * \param[in]     req              ����
 * \param[in/out] dat              ���ݵ�ַ
 *
 * \retval �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 *
 * \par ����
 * \code
 *
 *  uint32_t dat = 0;
 *
 *  //��ȡ��nandflash��ҳ��С
 *  awbl_nand_ioctl(p_chip, AWBL_NAND_PAGE_SIZE, &dat);
 */
aw_err_t awbl_nand_ioctl (struct awbl_nand_chip *p_chip,
                          awbl_nand_ioctl_req_t  req,
                          void                  *dat);



struct awbl_nand_chip *awbl_mtd_get_nand(struct aw_mtd_info *mtd);

struct awbl_nand_chip *awbl_mtd_set_nand(struct aw_mtd_info *mtd, void *p_nand);


void *awbl_nand_get_data(struct awbl_nand_chip *p_nand);

void awbl_nand_set_data(struct awbl_nand_chip *p_nand, void *priv);

/*
 * Check if it is a SLC nand.
 * The !nand_is_slc() can be used to check the MLC/TLC nand chips.
 * We do not distinguish the MLC and TLC now.
 */
aw_bool_t awbl_nand_is_slc(struct awbl_nand_chip *chip);

aw_err_t awbl_nand_read_id (struct awbl_nand_chip *p_nand,
                            uint8_t               *p_ids,
                            uint8_t                id_len);

void awbl_nand_init_default (awbl_nand_info_t *p_info,
                             awbl_nand_t      *p_nand);

void awbl_nand_init_with_hw_ecc (awbl_nand_init_t *p_init,
                                 awbl_nand_t      *p_nand,
                                 awbl_nand_info_t *p_info);


#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_NAND_H */

/* end of file */
