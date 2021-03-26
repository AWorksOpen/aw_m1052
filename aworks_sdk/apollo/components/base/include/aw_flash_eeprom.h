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
 * \brief flash-eeprom 驱动接口
 *
 * define 使用本模块包含aw_flash_eeprom.h
 *
 * \par 简单示例
 * \code
 *
 *   aw_flash_eeprom_mkinfo(&g_eeprom_info,
 *                          "/sflash0",  //设备名
 *                           0,          flash 偏移
 *                           4096,       flash 块大小
 *                           2,          flash 块数量
 *                           256);       flash 页大小
 *
 *   aw_flash_eeprom_init(&g_eeprom_dev, flash eeprom设备初始化
 *                        &g_eeprom_info);
 *
 *    清除eeprom中的所有数据
 *   aw_flash_eeprom_allclean(&g_eeprom_dev);
 *
 *   写缓存初始化
 *   for (i = 0; i < DATA_SIZE; i++) {
 *       g_data_wbuf[i] = i % 256;
 *   }
 *
 *   写入数据
 *   aw_flash_eeprom_write (&g_eeprom_dev,
 *                           TEST_ADDR,
 *                          (void *)g_data_wbuf,
 *                           DATA_SIZE);
 *
 *    读取数据
 *   aw_flash_eeprom_read (&g_eeprom_dev,
 *                          EST_ADDR,
 *                         (void *)g_data_rbuf,
 *                          DATA_SIZE);
 *
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 16-01-25  bob, first implementation
 * \endinternal
 */

#ifndef __AWBL_FLASH_EEPROM_H
#define __AWBL_FLASH_EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aw_sem.h"

/** 
 * \brief EEPROM设备数据宽度，表示一个地址空间可以存放AW_FLASH_EEPROM_DATA_SIZE
 *        个字节的数据
 */
#define AW_FLASH_EEPROM_DATA_SIZE   2

/** \brief EEPROM设备地址宽度（字节），配置为2个字节 */
#define AW_FLASH_EEPROM_ADDR_SIZE   2

/**
 * \brief eeprom flash设备配置信息
 */
typedef struct awbl_flash_eeprom_devinfo {

    const char                    *name;        /**< \brief FLASH设备名 */
    int32_t                        offset;      /**< \brief FLASH偏移量 */
    uint32_t                       block_size;  /**< \brief FLASH块大小 */
    uint16_t                       nblocks;     /**< \brief FLASH块数量 */
    uint16_t                       page_size;   /**< \brief 页大小 */
}awbl_flash_eeprom_devinfo_t;

/**
 * \brief spi flash设备结构体
 */
typedef struct awbl_flash_eeprom_dev {
    struct aw_mtd_info          *p_mtd_dev;   /**< \brief eeprom flash设备 */
    awbl_flash_eeprom_devinfo_t *p_info;      /**< \brief eeprom flash设备配置信息 */
    int32_t                      cur_addr;    /**< \brief 当前指向操作的flash地址  */
    uint16_t                     w_block_num; /**< \brief 当前指向操作的flash块号  */

    AW_MUTEX_DECL(mutex);                     /**< \brief 互斥量，保护FLASH EEPROM */
    aw_mutex_id_t mutex_id;                   /**< \brief 互斥量id */
} awbl_flash_eeprom_dev_t;


/**
 * \brief 从EEPROM中读取数据
 *
 * - 这个接口从FLASH EEPROM设备中读取len个字节
 *
 * \param[in]  p_dev   FLASH_EEPROM设备
 * \param[in]  addr    EEPROM设备寄存器地址
 * \param[in]  p_buf   读数据的缓存区
 * \param[in]  len     读取的数据量（字节为单位）
 *
 * \retval AW_OK    读取成功
 * \retval AW_ERROR 读取失败（空间不够）
 * \retval -EINVAL   参数错误
 */
aw_err_t aw_flash_eeprom_read (awbl_flash_eeprom_dev_t *p_dev,
                               uint32_t                 addr,
                               void                    *p_buf,
                               uint32_t                 len);

/**
 * \brief 从EEPROM中读取数据
 *
 * - 这个接口从FLASH EEPROM设备中写入len个字节
 *
 * \param[in]  p_dev   FLASH_EEPROM设备
 * \param[in]  addr    EEPROM设备寄存器地址
 * \param[in]  p_buf   写入数据的缓存区
 * \param[in]  len     写入的数据量（字节为单位）
 *
 * \retval AW_OK       写入成功
 * \retval AW_ERROR    写入失败（可能是某个地址的数据不存在）
 * \retval -AW_EINVAL  参数错误
 */
aw_err_t aw_flash_eeprom_write (awbl_flash_eeprom_dev_t *p_dev,
                                uint32_t                 addr,
                                void                    *p_buf,
                                uint32_t                 len);

/**
 * \brief 设置FLASH EEPROM设备配置参数
 *
 * \param[in]  p_info      指向FLASH EEPROM配置
 * \param[in]  name        FLASH EEPROM的FLASH名
 * \param[in]  offset      偏移量（一般情况下为0）
 * \param[in]  block_size  块大小
 * \param[in]  nblocks     块数量（块数量越多，使用的时间越长）
 * \param[in]  page_size   FLASH的页大小
 *
 * \return 无
 */
aw_local aw_inline
void aw_flash_eeprom_mkinfo (awbl_flash_eeprom_devinfo_t *p_info,
                              const char                 *name,
                              int32_t                     offset,
                              uint32_t                    block_size,
                              uint16_t                    nblocks,
                              uint16_t                    page_size) {
    p_info->name       = name;
    p_info->offset     = offset;
    p_info->nblocks    = nblocks;
    p_info->block_size = block_size;
    p_info->page_size  = page_size;
}

/**
 * \brief 清除EEPROM所有数据，程序会清空整个FLASH设备中的所有数据。
 *
 * \param[in]  p_dev   指向FLASH EEPROM设备
 *
 * \return 无
 */
void aw_flash_eeprom_allclean (awbl_flash_eeprom_dev_t   *p_dev);

/**
 * \brief FLASH EEPROM设备初始化
 *
 * - 这个接口
 *
 * \param[in]  p_dev    指向FLASH EEPROM设备结构体
 * \param[out] p_info   FLASH EEPROM设备配置
 *
 * \retval AW_OK     初始化成功
 * \retval -EINVAL   参数错误
 * \retval -ENXIO    FLASH设备不存在
 * \retval AW_ERRPR  初始化失败
 */
aw_err_t aw_flash_eeprom_init (awbl_flash_eeprom_dev_t     *p_dev,
                               awbl_flash_eeprom_devinfo_t *p_info);

/**
 * \brief FLASH EEPROM设备去初始化
 *
 * \param[in]  p_dev   指向FLASH EEPROM设备
 *
 * \return 无
 */
void aw_flash_eeprom_uninit (awbl_flash_eeprom_dev_t   *p_dev);


#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif /* __AWBL_SPI_FLASH_H */
