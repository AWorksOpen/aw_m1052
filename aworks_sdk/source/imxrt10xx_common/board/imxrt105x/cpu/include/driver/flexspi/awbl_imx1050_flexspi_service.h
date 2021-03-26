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

    /* flexspi nor flash读数据 */
    aw_err_t (*pfunc_flexspi_nor_flash_read) (void      *p_dev,
                                              uint32_t   addr,
                                              uint8_t   *p_buf,
                                              uint32_t   bytes);

    /* flexspi nor flash读状态寄存器 */
    aw_err_t (*pfunc_flexspi_nor_flash_read_status) (void      *p_dev,
                                                     uint8_t   *p_buf,
                                                     uint32_t   bytes);


    /* flexspi nor flash 写状态寄存器 */
    aw_err_t (*pfunc_flexspi_nor_flash_write_status) (void      *p_dev,
                                                      uint8_t    cfg);
                                                     

    /* flexspi nor flash写使能 */
    aw_err_t (*pfunc_flexspi_nor_flash_write_enable) (void      *p_dev,
                                                      uint32_t   addr);

    /* flexspi nor flash擦除扇区 */
    aw_err_t (*pfunc_flexspi_nor_flash_erase) (void      *p_dev,
                                               uint32_t   addr);

    /* flexspi nor flash编程 */
    aw_err_t (*pfunc_flexspi_nor_flash_program) (void      *p_dev,
                                                 uint32_t   addr,
                                                 uint8_t   *p_buf,
                                                 uint32_t   bytes);
                                                 
    /* flexspi nor flash 读function寄存器 */
    aw_err_t (*pfunc_flexspi_nor_flash_read_function) (void      *p_dev,
                                                       uint8_t   *p_buf,
                                                       uint32_t   bytes);

    /* flexspi nor flash 写function寄存器 */
    aw_err_t (*pfunc_flexspi_nor_flash_write_function) (void      *p_dev,
                                                        uint8_t    cfg);

    /* flexspi nor flash 读保留扇区 */
    aw_err_t (*pfunc_flexspi_nor_flash_read_ir) (void       *p_dev,
                                                 uint32_t    addr,
                                                 uint8_t    *p_buf,
                                                 uint32_t    bytes);

    /* flexspi nor flash 写保留扇区 */
    aw_err_t (*pfunc_flexspi_nor_flash_write_ir) (void       *p_dev,
                                                  uint32_t    addr,
                                                  uint8_t    *p_buf,
                                                  uint32_t    bytes);

    /* flexspi nor flash 擦除保留扇区 */
    aw_err_t (*pfunc_flexspi_nor_flash_erase_ir) (void      *p_dev,
                                                  uint32_t   addr);

} ;

/**
 * \brief Flexspi 服务
 */
typedef struct awbl_imx1050_flexspi_service {

    struct  awbl_dev                    *p_dev;
    struct  awbl_flexspi_servfuncs      *p_func;
} awbl_imx1050_flexspi_service_t;



void awbl_imx1050_flexspi_init (void);


#ifdef __cplusplus
}
#endif



#endif /* __AWBL_IMX1050_FLEXSPI_SERVICE_H */


/* end of file */
