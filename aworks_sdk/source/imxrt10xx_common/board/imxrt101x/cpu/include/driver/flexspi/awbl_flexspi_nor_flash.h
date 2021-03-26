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
 * - 1.00 20-09-24 licl, first implementation
 * \endinternal
 */

#ifndef __AWBL_FLEXSPI_NOR_FLASH_H
#define __AWBL_FLEXSPI_NOR_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"


/**
 * \brief Flexspi NOR Flash读状态寄存器
 *
 * \param[in] p_buf  数据读缓存
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 */
aw_err_t imx1010_nor_flash_read_sr (uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flash写status寄存器
 *
 * \param[in] p_buf  待写入的数据
 *
 * \retval   AW_OK      写入成功
 * \retval   AW_ERROR   写入失败
 */
aw_err_t imx1010_nor_flash_write_sr (uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flash读function寄存器
 *
 * \param[in] p_buf  数据读缓存
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 */
aw_err_t imx1010_nor_flash_read_fr (uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flash写function寄存器
 *
 * \param[in] p_buf  待写入的数据
 *
 * \retval   AW_OK      写入成功
 * \retval   AW_ERROR   写入失败
 */
aw_err_t imx1010_nor_flash_write_fr (uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flash从指定地址读数据
 *
 * \param[in] p_buf     数据读缓存
 * \param[in] offset    偏移地址
 * \param[in] nbytes    需要读取的数据字节数
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 */
aw_err_t imx1010_nor_flash_read (uint8_t *p_buf, uint32_t offset, uint32_t nbytes);


/**
 * \brief Flexspi NOR Flash擦除一个扇区
 *
 * \param[in] offset    扇区偏移地址
 *
 * \retval   AW_OK      操作成功
 * \retval   AW_ERROR   操作失败
 */
aw_err_t imx1010_nor_flash_sector_erase (uint32_t offset);


/**
 * \brief Flexspi NOR Flash编程
 *
 * \param[in] p_buf     待写入的数据
 * \param[in] offset    偏移地址
 * \param[in] nbytes    待写入的数据字节数
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 */
aw_err_t imx1010_nor_flash_page_program (uint8_t *p_buf,
                                         uint32_t offset,
                                         uint16_t nbytes);


/**
 * \brief Flexspi NOR Flash 擦除一个保留扇区
 *
 * \param[in] row  扇区编号（0-3），扇区大小256byte
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 */
aw_err_t imx1010_nor_flash_ir_erase (uint8_t row);


/**
 * \brief Flexspi NOR Flash 写保留扇区
 *
 * \param[in] p_buf  待写入的数据
 * \param[in] row    扇区编号（0-3），扇区大小256byte
 * \param[in] offset 偏移地址
 * \param[in] nbytes 待写入的数据字节数
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 */
aw_err_t imx1010_nor_flash_ir_program (uint8_t *p_buf,
                                       uint8_t  row,
                                       uint32_t offset,
                                       uint16_t nbytes);


/**
 * \brief Flexspi NOR Flash 读保留扇区
 *
 * \param[in] p_buf     数据读缓存
 * \param[in] row       扇区编号（0-3），扇区大小256byte
 * \param[in] offset    偏移地址
 * \param[in] nbytes    需要读取的数据字节数
 *
 * \retval   AW_OK      读取成功
 * \retval   AW_ERROR   读取失败
 */
aw_err_t imx1010_nor_flash_ir_read (uint8_t *p_buf,
                                    uint8_t  row,
                                    uint32_t offset,
                                    uint32_t nbytes);


#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_FLEXSPI_NOR_FLASH_H */

/* end of file */
