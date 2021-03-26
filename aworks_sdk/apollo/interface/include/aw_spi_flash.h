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
 * \brief SPI Flash �ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ����ͷ�ļ� aw_spi_flash.h
 *
 * \par ��ʾ��
 * \code
 *  #include "aw_spi_flash.h"
 *
 *  aw_spi_flash_t  pflsh;
 *  char            buf[] = {"this is a test."};
 *
 *
 *  pflsh = aw_spi_flash_open("/sflash0");
 *  if (pflsh != NULL) {
 *      aw_spi_flash_erase(pflsh, 0);                       //����0��
 *
 *      aw_spi_flash_write(pflsh, 0, buf, sizeof(buf));     //д���ݵ�0��
 *
 *      aw_spi_flash_read(pflsh, 0, buf, sizeof(buf));      //��������
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

/** \brief SPI Flash �豸������� */
typedef void * aw_spi_flash_t;

/** \brief SPI Flash �豸��Ϣ */
typedef struct aw_spi_flash_info {
    uint_t     block_size;     /**< \brief ���С */
    uint_t     nblocks;        /**< \brief ������ */
    uint_t     page_size;      /**< \brief ҳ��С */
} aw_spi_flash_info_t;


/**
 * \brief ��һ��SPI Flash�豸������豸���
 *
 * \param[in]  name  �豸����
 *
 * \retval  SPI Flash�豸��� �� NULL
 */
aw_spi_flash_t aw_spi_flash_open (const char *name);




/**
 * \brief �ر�һ��SPI Flash�豸
 *
 * \param[in]  p_flash  �豸���
 *
 * \retval  AW_OK       �ɹ�
 * \retval  -AW_EINVAL  p_flash��Ч
 */
aw_err_t aw_spi_flash_close (aw_spi_flash_t p_flash);




/**
 * \brief ��SPI Flash�豸��ȡָ����������
 *
 * \param[in]  p_flash  �豸���
 * \param[in]  addr     Ŀ��������Flash�еĵ�ַ
 * \param[in]  p_buf    �������ݵ��ڴ��ַ
 * \param[in]  len      ��ȡ���ݳ���
 * \param[out] rlen     ʵ�ʶ�ȡ���ݳ���
 *
 * \retval  AW_OK       �ɹ�
 * \retval  -AW_EINVAL  p_flash �� p_buf ��Ч
 * \retval  -AW_EBUSY   �豸æ
 * \retval  -AW_EIO     �豸ͨ�Ŵ���
 */
aw_err_t aw_spi_flash_read (aw_spi_flash_t   p_flash,
                            uint32_t         addr,
                            void            *p_buf,
                            uint32_t         len,
                            uint32_t        *rlen);




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
 * \retval  -AW_EINVAL  p_flash ��  p_data ��Ч
 * \retval  -AW_EBUSY   �豸æ
 * \retval  -AW_EIO     �豸ͨ�Ŵ���
 */
aw_err_t aw_spi_flash_write (aw_spi_flash_t   p_flash,
                             uint32_t         addr,
                             void            *p_data,
                             uint32_t         len,
                             uint32_t        *rlen);




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
aw_err_t aw_spi_flash_erase (aw_spi_flash_t  p_flash,
                             uint32_t        addr);




/**
 * \brief ��ȡSPI Flash ��Ϣ
 *
 * \param[in]  p_flash  �豸���
 * \param[in]  p_info   �����Ϣ���ݽṹ
 *
 * \retval  AW_OK       �ɹ�
 * \retval  -AW_EINVAL  p_flash ��  p_data ��Ч
 */
aw_err_t aw_spi_flash_info_get (aw_spi_flash_t       p_flash,
                                aw_spi_flash_info_t *p_info);


/** @} */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_SPI_FLASH_H */

/* end of file */

