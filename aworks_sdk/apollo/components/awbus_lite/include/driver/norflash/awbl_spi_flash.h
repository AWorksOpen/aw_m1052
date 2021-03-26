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
 * \brief SPI-Flash 驱动头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 14-10-31  deo, first implementation
 * \endinternal
 */

#ifndef __AWBL_SPI_FLASH_H
#define __AWBL_SPI_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_sem.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "awbl_spibus.h"


#define AWBL_SPI_FLASH_NAME   "awbl_spi_flash"


struct awbl_spi_flash_dev;




/** \brief SPI Flash MTD 扩展接口数据结构 */
struct awbl_spi_flash_mtd_ex {
    void        *p_mtd;         /**< \brief MTD对象句柄   */

    const void  *p_info;        /**< \brief MTD对象信息   */

    aw_err_t (*pfn_init) (struct awbl_spi_flash_dev *p_flash,
                          void                      *p_mtd,
                          const void                *p_info);
};




/** \brief SPI Flash 设备信息 */
struct awbl_spi_flash_devinfo {
    const char       *name;           /**< \brief 设备名称 */

    uint_t            block_size;     /**< \brief 块大小 */
    uint_t            nblocks;        /**< \brief 块数量 */
    uint_t            page_size;      /**< \brief 页大小 */

    uint16_t          spi_mode;       /**< \brief SPI 接口通信模式 */
    int               spi_cs_pin;     /**< \brief SPI 片选引脚 */
    uint32_t          spi_speed;      /**< \brief SPI 接口速度 */

    /** \brief MTD 接口扩展数据 */
    const struct awbl_spi_flash_mtd_ex *p_mtd_ex;

    /** \brief NVRAM 接口扩展数据 */
    const void                         *p_nvram_info;

    void (*pfunc_plfm_init)(void);
};




/** \brief SPI Flash 设备数据结构  */
struct awbl_spi_flash_dev {
    struct awbl_spi_device                spi;      /** \brief SPI 设备对象 */
    const struct awbl_spi_flash_devinfo  *p_info;
    AW_MUTEX_DECL(                        devlock);
    struct aw_list_head                   node;
    uint_t                                size;
};




/**
 * \brief 初始化 SPI FLASH 公共资源
 */
void awbl_spi_flash_lib_init (void);




/**
 * \brief 注册 SPI FLASH 驱动
 */
void awbl_spi_flash_drv_register (void);




/**
 * \brief 打开一个SPI Flash设备，获得设备句柄
 *
 * \param[in]  name  设备名称
 *
 * \retval  SPI Flash设备句柄 或 NULL
 */
struct awbl_spi_flash_dev *awbl_spi_flash_open (const char *name);




/**
 * \brief 关闭一个SPI Flash设备
 *
 * \param[in]  p_flash  设备句柄
 *
 * \retval  AW_OK       成功
 * \retval  -AW_EINVAL  p_flash无效
 */
aw_err_t awbl_spi_flash_close (struct awbl_spi_flash_dev *p_flash);




/**
 * \brief 从SPI Flash设备读取指定长度数据
 *
 * \param[in]  p_flash  设备句柄
 * \param[in]  addr     目标数据在Flash中的地址
 * \param[in]  p_buf    接收数据的内存地址
 * \param[in]  len      读取数据的长度
 * \param[out] rlen     实际读取数据长度
 *
 * \retval  AW_OK       成功
 * \retval  -AW_EINVAL  p_flash 或 p_buf 无效
 * \retval  -AW_EBUSY   设备忙
 * \retval  -AW_EIO     设备通信错误
 */
aw_err_t awbl_spi_flash_read (struct awbl_spi_flash_dev *p_flash,
                              uint32_t                   addr,
                              uint8_t                   *p_buf,
                              uint32_t                   len,
                              uint32_t                  *rlen);




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
 * \retval  -AW_EINVAL  p_flash 或 p_buf 无效
 * \retval  -AW_EBUSY   设备忙
 * \retval  -AW_EIO     设备通信错误
 */
aw_err_t awbl_spi_flash_write (struct awbl_spi_flash_dev *p_flash,
                               uint32_t                   addr,
                               uint8_t                   *p_data,
                               uint32_t                   len,
                               uint32_t                  *rlen);




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
aw_err_t awbl_spi_flash_erase (struct awbl_spi_flash_dev *p_flash,
                               uint32_t                   addr);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_SPI_FLASH_H */

/* end of file */
