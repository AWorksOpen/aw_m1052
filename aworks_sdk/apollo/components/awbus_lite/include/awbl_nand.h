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


/** \brief NandFlash驱动名称 */
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
 * \brief  ioctl的请求选项
 */
typedef enum awbl_nand_ioctl_req {

    /** \brief 获取页大小 */
    AWBL_NAND_PAGE_SIZE,

    /** \brief 获取每块中页的数量 */
    AWBL_NAND_PAGE_NUM_PER_BLK,

    /** \brief 获取块大小 */
    AWBL_NAND_BLK_SIZE,

    /** \brief 获取块的总数量 */
    AWBL_NAND_BLK_NUM,

    /** \brief 获取可用的oob大小 */
    AWBL_NAND_OOB_SIZE,

    /** \brief 获取片大小  */
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

    /** \brief 基类设备 */
    struct awbl_dev              awdev;
    awbl_nand_init_t             init;
    awbl_nand_info_t            *p_info;  /**< \brief 由 hwconf 文件提供  */
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

    /** \brief 数据缓存，page+oobsize */
    uint8_t                     *page_buf;
    uint8_t                     *oob_buf;

    awbl_nand_ecc_t             *p_ecc;

    /** \brief oob布局 */
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
 * \brief nand读数据，无ecc检验，不能跨页
 *
 * \param[in] p_chip           nand设备结构体
 * \param[in] page             要读的页地址
 * \param[in] col              要读的页内地址
 * \param[in] buf              存放数据的buf
 * \param[in] len              读长度
 *
 * \retval 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t awbl_nand_read (struct awbl_nand_chip *p_chip,
                         uint32_t               page,
                         uint32_t               col,
                         uint8_t               *buf,
                         int                    len);

/**
 * \brief nand写数据，无ecc检验，不能跨页
 *
 * \param[in] p_chip           nand设备结构体
 * \param[in] page             页地址
 * \param[in] col              页内地址
 * \param[in] buf              存放数据的buf
 * \param[in] len              操作的长度
 *
 * \retval 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t awbl_nand_write (struct awbl_nand_chip *p_chip,
                          uint32_t               page,
                          uint32_t               col,
                          uint8_t               *buf,
                          int                    len);

/**
 * \brief nand页读数据，带ecc检验，一次操作长度为一整页
 *
 * \param[in] p_chip           nand设备结构体
 * \param[in] ipage            页地址
 * \param[out] buf             存放数据的buf
 *
 * \retval 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t awbl_nand_page_read (struct awbl_nand_chip *p_chip,
                              uint8_t               *buf,
                              uint32_t               ipage);

/**
 * \brief nand页写数据，带ecc检验，一次操作长度为一整页
 *
 * \param[in] p_chip           nand设备结构体
 * \param[in] ipage            页地址
 * \param[in] buf              存放数据的buf
 *
 * \retval 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t awbl_nand_page_write (struct awbl_nand_chip *p_chip,
                               uint8_t               *buf,
                               uint32_t               ipage);

/**
 * \brief 写oob区的free区
 *
 * \param[in] p_chip           nand设备结构体
 * \param[in] ipage            页地址
 * \param[in] offs             在free区的偏移
 * \param[in] oob_data         数据buff
 * \param[in] data_len         写入长度
 *
 * \retval  操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 *
 * \note    对oob区写入时，应先知道free区的大小，读写长度不可超过free区的大小，
 *          调用awbl_nand_ioctl(p_chip, AWBL_NAND_OOB_SIZE, &free_szie);
 *          可得到free区的大小
 */
aw_err_t awbl_nand_oob_write (struct awbl_nand_chip    *p_chip,
                              uint32_t                  ipage,
                              uint32_t                  offs,
                              uint8_t                  *oob_data,
                              uint16_t                  data_len);

/**
  * \brief 读oob区的free区
 *
 * \param[in] p_chip           nand设备结构体
 * \param[in] ipage            页地址
 * \param[in] offs             在free区的偏移
 * \param[in] oob_data         数据buff
 * \param[in] data_len         读长度
 *
 * \retval 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 *
 * \note    对oob区读写时，应先知道free区的大小，读写长度不可超过free区的大小，
 *          调用awbl_nand_ioctl(p_chip, AWBL_NAND_OOB_SIZE, &free_szie);
 *          可得到free区的大小
 */
aw_err_t awbl_nand_oob_read (struct awbl_nand_chip    *p_chip,
                             uint32_t                  ipage,
                             uint32_t                  offs,
                             uint8_t                  *oob_data,
                             uint16_t                  data_len);

/**
 * \brief 块擦除
 *
 * \param[in] p_chip           nand设备结构体
 * \param[in] iblk             块地址
 *
 * \retval 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t awbl_nand_blk_erase (struct awbl_nand_chip *p_chip,
                              uint32_t               iblk);

/**
 * \brief 判断坏块
 *
 * \param[in] p_chip           nand设备结构体
 * \param[in] iblk             块地址
 *
 * \retval 返回判断结果
 */
aw_bool_t awbl_nand_blk_is_bad (struct awbl_nand_chip *p_chip,
                                uint32_t               iblk);

/**
 * \brief 标记坏块
 *
 * \param[in] p_chip           nand设备结构体
 * \param[in] iblk             块地址
 *
 * \retval 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t awbl_nand_blk_markbad (struct awbl_nand_chip *p_chip,
                                uint32_t               iblk);

/**
 * \brief IOCTL
 *
 * \param[in]     p_chip           nand设备结构体
 * \param[in]     req              请求
 * \param[in/out] dat              数据地址
 *
 * \retval 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 *
 * \par 范例
 * \code
 *
 *  uint32_t dat = 0;
 *
 *  //获取该nandflash的页大小
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
