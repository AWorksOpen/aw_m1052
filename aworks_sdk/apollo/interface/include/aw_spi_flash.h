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
 * \brief SPI Flash 接口头文件
 *
 * 使用本模块需要包含头文件 aw_spi_flash.h
 *
 * \par 简单示例
 * \code
 *  #include "aw_spi_flash.h"
 *
 *  aw_spi_flash_t  pflsh;
 *  char            buf[] = {"this is a test."};
 *
 *
 *  pflsh = aw_spi_flash_open("/sflash0");
 *  if (pflsh != NULL) {
 *      aw_spi_flash_erase(pflsh, 0);                       //擦出0块
 *
 *      aw_spi_flash_write(pflsh, 0, buf, sizeof(buf));     //写数据到0块
 *
 *      aw_spi_flash_read(pflsh, 0, buf, sizeof(buf));      //读回数据
 *
 *      aw_spi_flash_close(pflsh);
 *  }
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-09-04  deo, first implementation
 * \endinternal
 */



#ifndef __AW_SPI_FLASH_H
#define __AW_SPI_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_spi_flash
 * \copydoc aw_spi_flash.h
 * @{
 */

/** \brief SPI Flash 设备句柄类型 */
typedef void * aw_spi_flash_t;

/** \brief SPI Flash 设备信息 */
typedef struct aw_spi_flash_info {
    uint_t     block_size;     /**< \brief 块大小 */
    uint_t     nblocks;        /**< \brief 块数量 */
    uint_t     page_size;      /**< \brief 页大小 */
} aw_spi_flash_info_t;


/**
 * \brief 打开一个SPI Flash设备，获得设备句柄
 *
 * \param[in]  name  设备名称
 *
 * \retval  SPI Flash设备句柄 或 NULL
 */
aw_spi_flash_t aw_spi_flash_open (const char *name);




/**
 * \brief 关闭一个SPI Flash设备
 *
 * \param[in]  p_flash  设备句柄
 *
 * \retval  AW_OK       成功
 * \retval  -AW_EINVAL  p_flash无效
 */
aw_err_t aw_spi_flash_close (aw_spi_flash_t p_flash);




/**
 * \brief 从SPI Flash设备读取指定长度数据
 *
 * \param[in]  p_flash  设备句柄
 * \param[in]  addr     目标数据在Flash中的地址
 * \param[in]  p_buf    接收数据的内存地址
 * \param[in]  len      读取数据长度
 * \param[out] rlen     实际读取数据长度
 *
 * \retval  AW_OK       成功
 * \retval  -AW_EINVAL  p_flash 或 p_buf 无效
 * \retval  -AW_EBUSY   设备忙
 * \retval  -AW_EIO     设备通信错误
 */
aw_err_t aw_spi_flash_read (aw_spi_flash_t   p_flash,
                            uint32_t         addr,
                            void            *p_buf,
                            uint32_t         len,
                            uint32_t        *rlen);




/**
 * \brief 从SPI Flash设备读取指定长度数据
 *
 * \param[in]  p_flash  设备句柄
 * \param[in]  addr     目标地址
 * \param[out] p_data   待写入数据在内存中的地址
 * \param[in]  len      写入数据的长度
 * \param[out] rlen     实际写入数据长度
 *
 * \retval  AW_OK       成功
 * \retval  -AW_EINVAL  p_flash 或  p_data 无效
 * \retval  -AW_EBUSY   设备忙
 * \retval  -AW_EIO     设备通信错误
 */
aw_err_t aw_spi_flash_write (aw_spi_flash_t   p_flash,
                             uint32_t         addr,
                             void            *p_data,
                             uint32_t         len,
                             uint32_t        *rlen);




/**
 * \brief 擦出SPI Flash 指定地址所在的块
 *
 * \param[in]  p_flash  设备句柄
 * \param[in]  addr     目标地址
 *
 * \retval  AW_OK       成功
 * \retval  -AW_EINVAL  p_flash 或  p_data 无效
 * \retval  -AW_EBUSY   设备忙
 * \retval  -AW_EIO     设备通信错误
 */
aw_err_t aw_spi_flash_erase (aw_spi_flash_t  p_flash,
                             uint32_t        addr);




/**
 * \brief 获取SPI Flash 信息
 *
 * \param[in]  p_flash  设备句柄
 * \param[in]  p_info   存放信息数据结构
 *
 * \retval  AW_OK       成功
 * \retval  -AW_EINVAL  p_flash 或  p_data 无效
 */
aw_err_t aw_spi_flash_info_get (aw_spi_flash_t       p_flash,
                                aw_spi_flash_info_t *p_info);


/** @} */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_SPI_FLASH_H */

/* end of file */

