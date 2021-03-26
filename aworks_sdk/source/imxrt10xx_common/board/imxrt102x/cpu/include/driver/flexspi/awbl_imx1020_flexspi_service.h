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
 * - 1.00 19-05-21  ral, first implementation
 * \endinternal
 */


#ifndef __AWBL_IMX1020_FLEXSPI_SERVICE_H
#define __AWBL_IMX1020_FLEXSPI_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"
#include "aw_errno.h"


/**
 * \brief Flexspi 服务函数
 */
struct awbl_flexspi_servfuncs {

    /* flexspi nor flash read */
    aw_err_t (*pfunc_flexspi_nor_flash_read) (void      *p_dev,
                                              uint32_t   addr,
                                              uint8_t   *p_buf,
                                              uint32_t   bytes);

    /* flexspi nor flash erase section */
    aw_err_t (*pfunc_flexspi_nor_flash_erase) (void      *p_dev,
                                               uint32_t   addr);

    /* flexspi nor flash page program */
    aw_err_t (*pfunc_flexspi_nor_flash_program) (void      *p_dev,
                                                 uint32_t   addr,
                                                 uint8_t   *p_buf,
                                                 uint32_t   bytes);

    /* flexspi nor flash read function register */
    aw_err_t (*pfunc_flexspi_nor_flash_rdfr) (void      *p_dev,
                                              uint8_t   *p_buf);

    /* flexspi nor flash write function register */
    aw_err_t (*pfunc_flexspi_nor_flash_wrfr) (void      *p_dev,
                                              uint8_t   *p_buf);

    /* flexspi nor flash read information row */
    aw_err_t (*pfunc_flexspi_nor_flash_irrd) (void      *p_dev,
                                              uint32_t   addr,
                                              uint8_t   *p_buf,
                                              uint32_t   bytes);

    /* flexspi nor flash erase information row */
    aw_err_t (*pfunc_flexspi_nor_flash_irer) (void     *p_dev,
                                              uint32_t  addr);

    /* flexspi nor flash program information row */
    aw_err_t (*pfunc_flexspi_nor_flash_irp) (void      *p_dev,
                                             uint32_t   addr,
                                             uint8_t   *p_buf,
                                             uint32_t   bytes);

    /* flexspi nor flash read status register */
    aw_err_t (*pfunc_flexspi_nor_flash_rdsr) (void      *p_dev,
                                              uint8_t   *p_buf);

    /* flexspi nor flash write status register */
    aw_err_t (*pfunc_flexspi_nor_flash_wrsr) (void      *p_dev,
                                              uint8_t   *p_buf);
} ;

/**
 * \brief Flexspi 服务
 */
typedef struct awbl_imx1020_flexspi_service {

    struct  awbl_dev                    *p_dev;
    struct  awbl_flexspi_servfuncs      *p_func;
} awbl_imx1020_flexspi_service_t;



void awbl_imx1020_flexspi_init (void);


#ifdef __cplusplus
}
#endif



#endif /* __AWBL_IMX1020_FLEXSPI_SERVICE_H */


/* end of file */
