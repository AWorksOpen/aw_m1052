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
 * \brief SPI-Flash ����ͷ�ļ�
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




/** \brief SPI Flash MTD ��չ�ӿ����ݽṹ */
struct awbl_spi_flash_mtd_ex {
    void        *p_mtd;         /**< \brief MTD������   */

    const void  *p_info;        /**< \brief MTD������Ϣ   */

    aw_err_t (*pfn_init) (struct awbl_spi_flash_dev *p_flash,
                          void                      *p_mtd,
                          const void                *p_info);
};




/** \brief SPI Flash �豸��Ϣ */
struct awbl_spi_flash_devinfo {
    const char       *name;           /**< \brief �豸���� */

    uint_t            block_size;     /**< \brief ���С */
    uint_t            nblocks;        /**< \brief ������ */
    uint_t            page_size;      /**< \brief ҳ��С */

    uint16_t          spi_mode;       /**< \brief SPI �ӿ�ͨ��ģʽ */
    int               spi_cs_pin;     /**< \brief SPI Ƭѡ���� */
    uint32_t          spi_speed;      /**< \brief SPI �ӿ��ٶ� */

    /** \brief MTD �ӿ���չ���� */
    const struct awbl_spi_flash_mtd_ex *p_mtd_ex;

    /** \brief NVRAM �ӿ���չ���� */
    const void                         *p_nvram_info;

    void (*pfunc_plfm_init)(void);
};




/** \brief SPI Flash �豸���ݽṹ  */
struct awbl_spi_flash_dev {
    struct awbl_spi_device                spi;      /** \brief SPI �豸���� */
    const struct awbl_spi_flash_devinfo  *p_info;
    AW_MUTEX_DECL(                        devlock);
    struct aw_list_head                   node;
    uint_t                                size;
};




/**
 * \brief ��ʼ�� SPI FLASH ������Դ
 */
void awbl_spi_flash_lib_init (void);




/**
 * \brief ע�� SPI FLASH ����
 */
void awbl_spi_flash_drv_register (void);




/**
 * \brief ��һ��SPI Flash�豸������豸���
 *
 * \param[in]  name  �豸����
 *
 * \retval  SPI Flash�豸��� �� NULL
 */
struct awbl_spi_flash_dev *awbl_spi_flash_open (const char *name);




/**
 * \brief �ر�һ��SPI Flash�豸
 *
 * \param[in]  p_flash  �豸���
 *
 * \retval  AW_OK       �ɹ�
 * \retval  -AW_EINVAL  p_flash��Ч
 */
aw_err_t awbl_spi_flash_close (struct awbl_spi_flash_dev *p_flash);




/**
 * \brief ��SPI Flash�豸��ȡָ����������
 *
 * \param[in]  p_flash  �豸���
 * \param[in]  addr     Ŀ��������Flash�еĵ�ַ
 * \param[in]  p_buf    �������ݵ��ڴ��ַ
 * \param[in]  len      ��ȡ���ݵĳ���
 * \param[out] rlen     ʵ�ʶ�ȡ���ݳ���
 *
 * \retval  AW_OK       �ɹ�
 * \retval  -AW_EINVAL  p_flash �� p_buf ��Ч
 * \retval  -AW_EBUSY   �豸æ
 * \retval  -AW_EIO     �豸ͨ�Ŵ���
 */
aw_err_t awbl_spi_flash_read (struct awbl_spi_flash_dev *p_flash,
                              uint32_t                   addr,
                              uint8_t                   *p_buf,
                              uint32_t                   len,
                              uint32_t                  *rlen);




/**
 * \brief ��SPI Flash�豸��ȡָ����������
 *
 * \param[in]  p_flash  �豸���
 * \param[in]  addr     Ŀ���ַ
 * \param[out] p_data   ��д���������ڴ��еĵ�ַ
 * \param[in]  len      д�����ݵĳ���
 * \param[out] rlen     ʵ��д�����ݳ���
 *
 * \retval  AW_OK       �ɹ�
 * \retval  -AW_EINVAL  p_flash �� p_buf ��Ч
 * \retval  -AW_EBUSY   �豸æ
 * \retval  -AW_EIO     �豸ͨ�Ŵ���
 */
aw_err_t awbl_spi_flash_write (struct awbl_spi_flash_dev *p_flash,
                               uint32_t                   addr,
                               uint8_t                   *p_data,
                               uint32_t                   len,
                               uint32_t                  *rlen);




/**
 * \brief ����SPI Flash ָ����ַ���ڵĿ�
 *
 * \param[in]  p_flash  �豸���
 * \param[in]  addr     Ŀ���ַ
 *
 * \retval  AW_OK       �ɹ�
 * \retval  -AW_EINVAL  p_flash ��  p_data ��Ч
 * \retval  -AW_EBUSY   �豸æ
 * \retval  -AW_EIO     �豸ͨ�Ŵ���
 */
aw_err_t awbl_spi_flash_erase (struct awbl_spi_flash_dev *p_flash,
                               uint32_t                   addr);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_SPI_FLASH_H */

/* end of file */
