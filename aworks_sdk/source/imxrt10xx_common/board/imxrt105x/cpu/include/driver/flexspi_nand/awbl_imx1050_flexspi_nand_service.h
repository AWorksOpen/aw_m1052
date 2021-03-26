/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
**** ***************************************************************************/

/**
 * \file
 * \brief SPIFI标准接口头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-31  mex, first implementation
 * \endinternal
 */


#ifndef __AWBL_IMX1050_FLEXSPI_SERVICE_H
#define __AWBL_IMX1050_FLEXSPI_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"
#include "aw_errno.h"


/**
 * \brief Flexspi 服务函数
 */
struct awbl_flexspi_servfuncs {

    /* Flexspi NAND Flash 读ID */
    aw_err_t (*pfunc_flexspi_nand_flash_read_id) (void      *p_dev,
                                                  uint8_t   *p_buf,
                                                  uint32_t   bytes);

    /* Flexspi NAND Flash 写使能 */
    aw_err_t (*pfunc_flexspi_nand_flash_write_enable) (void   *p_dev);

    /* Flexspi NAND Flash 写禁能 */
    aw_err_t (*pfunc_flexspi_nand_flash_write_disable) (void  *p_dev);

    /* Flexspi NAND Flash 读状态寄存器 */
    aw_err_t (*pfunc_flexspi_nand_flash_read_status) (void      *p_dev,
                                                      uint8_t    status_addr,
                                                      uint8_t   *status);

    /* Flexspi NAND Flash 写状态寄存器 */
    aw_err_t (*pfunc_flexspi_nand_flash_write_status) (void      *p_dev,
                                                       uint8_t    status_addr,
                                                       uint8_t   *status);

    /* Flexspi NAND Flash 擦除扇区 */
    aw_err_t (*pfunc_flexspi_nand_flash_block_erase) (void      *p_dev,
                                                      uint32_t   addr);

    /* Flexspi NAND Flash 读页数据 */
    aw_err_t (*pfunc_flexspi_nand_flash_page_read) (void      *p_dev,
                                                    uint32_t   p_addr,
                                                    uint32_t   offset,
                                                    uint8_t   *p_buf,
                                                    uint32_t   bytes);

    /* Flexspi NAND Flash 写页数据 */
    aw_err_t (*pfunc_flexspi_nand_flash_page_write) (void      *p_dev,
                                                    uint32_t    p_addr,
                                                    uint32_t    offset,
                                                    uint8_t    *buf,
                                                    uint32_t    bytes);
} ;

/**
 * \brief Flexspi 服务
 */
typedef struct awbl_imx1050_flexspi_service {

    struct  awbl_dev                    *p_dev;
    struct  awbl_flexspi_servfuncs      *p_func;
} awbl_imx1050_flexspi_service_t;


void awbl_imx1050_flexspi_nand_init (void);


#ifdef __cplusplus
}
#endif



#endif /* __AWBL_IMX1050_FLEXSPI_SERVICE_H */


/* end of file */
