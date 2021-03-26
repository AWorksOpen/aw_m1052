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
 * \brief IMX1020 Flexspi NOR Flash通用接口
 *
 * 使用本服务需要包含头文件
 * \code
 * #include imx1020_nor_flash.h
 * \endcode
 *
 * \par 简单示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 19-05-21 ral, first implementation
 * \endinternal
 */

#ifndef __IMX1020_NOR_FLASH_H
#define __IMX1020_NOR_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"

/**
 * \brief Flexspi NOR Flash从可编程区域中读取数据
 *
 * \param[in] p_buf  读数据缓存
 * \param[in] offset 在可编程flash区域中的偏移
 * \param[in] bytes  需要读取的字节数
 *
 * \retval   AW_OK      操作成功
 * \retval   AW_ENXIO   设备不存在
 * \retval   AW_EINVAL  无效参数
 */
aw_err_t  imx1020_nor_flash_read (uint8_t *p_buf,
                                  uint32_t offset,
                                  uint32_t nbytes);

/**
 * \brief Flexspi NOR Flash擦除一个扇区
 *
 * \param[in] offset 在可编程flash区域中的偏移
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 * \retval   AW_ENXIO  设备不存在
 * \retval   AW_EINVAL 无效参数
 */
aw_err_t imx1020_nor_flash_sector_erase (uint32_t offset);


/**
 * \brief Flexspi NOR Flash编程
 *
 * \param[in] p_buf  待写入的数据
 * \param[in] offset 在可编程flash区域中的偏移
 * \param[in] bytes  待写入的数据字节数）
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 * \retval   AW_ENXIO  设备不存在
 * \retval   AW_EINVAL 无效参数
 *
 * \note 最多可以对flash的一页进行编程，即nbytes最大不能超过256
 *       在对flash进行编程前必须调用imx1020_nor_flash_sector_erase函数对指定区域进行擦除
 */
aw_err_t imx1020_nor_flash_page_program (uint8_t *p_buf,
                                         uint32_t offset,
                                         uint16_t nbytes);


/**
 * \brief Flexspi NOR Flash 读取information行中的数据
 *
 * \param[in] p_buf  读数据缓存
 * \param[in] row    待读取的information行号
 * \param[in] offset 待读取的information行中的偏移
 * \param[in] nbytes 待读取的字节数
 *
 * \retval    AW_OK     操作成功
 * \retval    AW_ERROR  操作失败
 * \retval    AW_ENXIO  设备不存在
 * \retval    AW_EINVAL 无效参数
 */
aw_err_t imx1020_nor_flash_ir_read(uint8_t *p_buf,
                                   uint8_t  row,
                                   uint32_t offset,
                                   uint32_t nbytes);


/**
 * \brief Flexspi NOR FLash 擦除一个information行
 *
 * \param[in] row 待擦除的information行号
 *
 * \retval    AW_OK     操作成功
 * \retval    AW_ERROR  操作失败
 * \retval    AW_ENXIO  设备不存在
 * \retval    AW_EINVAL 无效参数
 */
aw_err_t imx1020_nor_flash_ir_erase(uint8_t row);


/**
 * \brief Flexspi NOR flash 对information行编程
 *
 * \param[in] p_buf  待写入的数据
 * \param[in] offset 在可编程flash区域中的偏移
 * \param[in] bytes  待写入的数据字节数）
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 * \retval   AW_ENXIO  设备不存在
 * \retval   AW_EINVAL 无效参数
 *
 * \note information行大小为256bytes， 所以一次最多进行256字节的编程
 *       在使用之前需调用imx1020_nor_flash_ir_erase对待写入的行进行擦除
 */
aw_err_t imx1020_nor_flash_ir_program (uint8_t *p_buf,
                                       uint8_t  row,
                                       uint32_t offset,
                                       uint16_t nbytes);


/**
 * \brief Flexspi NOR Flash 读status寄存器
 *
 * \param[in] p_buf  读数据缓存
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 * \retval   AW_ENXIO  设备不存在
 * \retval   AW_EINVAL 无效参数
 */
aw_err_t imx1020_nor_flash_read_sr(uint8_t *p_buf);


/**
 * \brief Flexspi NOR FLash 写status寄存器
 *
 * \param[in] p_buf  待写入的数据
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 * \retval   AW_ENXIO  设备不存在
 * \retval   AW_EINVAL 无效参数
 */
aw_err_t imx1020_nor_flash_write_sr(uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flash 读function寄存器
 *
 * \param[in] p_buf  读数据缓存
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 * \retval   AW_ENXIO  设备不存在
 * \retval   AW_EINVAL 无效参数
 */
aw_err_t imx1020_nor_flash_read_fr(uint8_t *p_buf);


/**
 * \brief Flexspi NOR FLash 写function寄存器
 *
 * \param[in] p_buf  待写入的数据
 *
 * \retval   AW_OK     操作成功
 * \retval   AW_ERROR  操作失败
 * \retval   AW_ENXIO  设备不存在
 * \retval   AW_EINVAL 无效参数
 */
aw_err_t imx1020_nor_flash_write_fr(uint8_t *p_buf);


#ifdef __cplusplus
}
#endif

#endif  /* __IMX1020_NOR_FLASH_H */

/* end of file */
