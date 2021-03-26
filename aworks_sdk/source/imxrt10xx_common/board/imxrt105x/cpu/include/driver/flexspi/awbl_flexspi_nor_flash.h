/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2014 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief Flexspi NOR Flash通用接口
 *
 * 使用本服务需要包含头文件
 * \code
 * #include aw_flexspi_nor_flash.h
 * \endcode
 *
 * \par 简单示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-31 mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_FLEXSPI_NOR_FLASH_H
#define __AWBL_FLEXSPI_NOR_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"


/**
 * \brief Flexspi NOR Flash从指定地址读数据
 *
 * \param[in] addr  地址
 * \param[in] p_buf 数据读缓存
 * \param[in] bytes 需要读取的数据字节数
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 */
aw_err_t  awbl_flexspi_nor_flash_read (uint32_t   addr,
                                       uint8_t   *p_buf,
                                       uint32_t   bytes);

/**
 * \brief Flexspi NOR Flash读状态寄存器
 *
 * \param[in] p_buf  数据读缓存
 * \param[in] bytes  需要读取的数据字节数
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 */
aw_err_t  awbl_flexspi_nor_flash_read_status(uint8_t   *p_buf,
                                             uint32_t   bytes);


/**
 * \brief Flexspi NOR Flash写status寄存器
 *
 * \param[in] p_buf  待写入的数据
 *
 * \retval   AW_OK      写入成功
 * \retval   AW_ERROR   写入失败
 */
aw_err_t  awbl_flexspi_nor_flash_write_status(uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flash读function寄存器
 *
 * \param[in] p_buf  数据读缓存
 * \param[in] bytes  需要读取的数据字节数
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 */
aw_err_t  awbl_flexspi_nor_flash_read_function(uint8_t   *p_buf,
                                               uint32_t   bytes);


/**
 * \brief Flexspi NOR Flash写function寄存器
 *
 * \param[in] p_buf  待写入的数据
 *
 * \retval   AW_OK      写入成功
 * \retval   AW_ERROR   写入失败
 */
aw_err_t  awbl_flexspi_nor_flash_write_function(uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flash写使能
 *
 * \param[in] addr  写数据的地址
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 */
aw_err_t  awbl_flexspi_nor_flash_write_enable(uint32_t  addr);


/**
 * \brief Flexspi NOR Flash擦除一个扇区
 *
 * \param[in] addr  扇区所在的地址
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 */
aw_err_t awbl_flexspi_nor_flash_erase (uint32_t  addr);


/**
 * \brief Flexspi NOR Flash编程
 *
 * \param[in] addr   数据写入的起始地址
 * \param[in] p_buf  待写入的数据
 * \param[in] bytes  待写入的数据字节数
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 */
aw_err_t awbl_flexspi_nor_flash_program (uint32_t   addr,
                                         uint8_t   *p_buf,
                                         uint32_t   bytes);

/**
 * \brief Flexspi NOR Flash 擦除一个保留扇区
 *
 * \param[in] index  扇区编号（0-3），扇区大小256byte
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 */
aw_err_t awbl_flexspi_nor_flash_info_erase (uint8_t index);


/**
 * \brief Flexspi NOR Flash 写保留扇区
 *
 * \param[in] index  扇区编号（0-3），扇区大小256byte
 * \param[in] addr   数据写入的起始地址
 * \param[in] p_buf  待写入的数据
 * \param[in] bytes  待写入的数据字节数
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 */
aw_err_t awbl_flexspi_nor_flash_info_program (uint8_t  index,
                                              uint8_t  addr,
                                              uint8_t *p_buf,
                                              uint32_t bytes);

/**
 * \brief Flexspi NOR Flash 读保留扇区
 *
 * \param[in] index 扇区编号（0-3），扇区大小256byte
 * \param[in] addr  地址
 * \param[in] p_buf 数据读缓存
 * \param[in] bytes 需要读取的数据字节数
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 */
aw_err_t  awbl_flexspi_nor_flash_info_read (uint8_t  index,
                                            uint8_t  addr,
                                            uint8_t *p_buf,
                                            uint32_t bytes);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_FLEXSPI_NOR_FLASH_H */

/* end of file */
