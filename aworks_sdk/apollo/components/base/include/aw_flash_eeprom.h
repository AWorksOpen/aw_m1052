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
 * \brief flash-eeprom �����ӿ�
 *
 * define ʹ�ñ�ģ�����aw_flash_eeprom.h
 *
 * \par ��ʾ��
 * \code
 *
 *   aw_flash_eeprom_mkinfo(&g_eeprom_info,
 *                          "/sflash0",  //�豸��
 *                           0,          flash ƫ��
 *                           4096,       flash ���С
 *                           2,          flash ������
 *                           256);       flash ҳ��С
 *
 *   aw_flash_eeprom_init(&g_eeprom_dev, flash eeprom�豸��ʼ��
 *                        &g_eeprom_info);
 *
 *    ���eeprom�е���������
 *   aw_flash_eeprom_allclean(&g_eeprom_dev);
 *
 *   д�����ʼ��
 *   for (i = 0; i < DATA_SIZE; i++) {
 *       g_data_wbuf[i] = i % 256;
 *   }
 *
 *   д������
 *   aw_flash_eeprom_write (&g_eeprom_dev,
 *                           TEST_ADDR,
 *                          (void *)g_data_wbuf,
 *                           DATA_SIZE);
 *
 *    ��ȡ����
 *   aw_flash_eeprom_read (&g_eeprom_dev,
 *                          EST_ADDR,
 *                         (void *)g_data_rbuf,
 *                          DATA_SIZE);
 *
 * \endcode
 *
 * // �������ݴ���ӡ�����
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
 * \brief EEPROM�豸���ݿ�ȣ���ʾһ����ַ�ռ���Դ��AW_FLASH_EEPROM_DATA_SIZE
 *        ���ֽڵ�����
 */
#define AW_FLASH_EEPROM_DATA_SIZE   2

/** \brief EEPROM�豸��ַ��ȣ��ֽڣ�������Ϊ2���ֽ� */
#define AW_FLASH_EEPROM_ADDR_SIZE   2

/**
 * \brief eeprom flash�豸������Ϣ
 */
typedef struct awbl_flash_eeprom_devinfo {

    const char                    *name;        /**< \brief FLASH�豸�� */
    int32_t                        offset;      /**< \brief FLASHƫ���� */
    uint32_t                       block_size;  /**< \brief FLASH���С */
    uint16_t                       nblocks;     /**< \brief FLASH������ */
    uint16_t                       page_size;   /**< \brief ҳ��С */
}awbl_flash_eeprom_devinfo_t;

/**
 * \brief spi flash�豸�ṹ��
 */
typedef struct awbl_flash_eeprom_dev {
    struct aw_mtd_info          *p_mtd_dev;   /**< \brief eeprom flash�豸 */
    awbl_flash_eeprom_devinfo_t *p_info;      /**< \brief eeprom flash�豸������Ϣ */
    int32_t                      cur_addr;    /**< \brief ��ǰָ�������flash��ַ  */
    uint16_t                     w_block_num; /**< \brief ��ǰָ�������flash���  */

    AW_MUTEX_DECL(mutex);                     /**< \brief ������������FLASH EEPROM */
    aw_mutex_id_t mutex_id;                   /**< \brief ������id */
} awbl_flash_eeprom_dev_t;


/**
 * \brief ��EEPROM�ж�ȡ����
 *
 * - ����ӿڴ�FLASH EEPROM�豸�ж�ȡlen���ֽ�
 *
 * \param[in]  p_dev   FLASH_EEPROM�豸
 * \param[in]  addr    EEPROM�豸�Ĵ�����ַ
 * \param[in]  p_buf   �����ݵĻ�����
 * \param[in]  len     ��ȡ�����������ֽ�Ϊ��λ��
 *
 * \retval AW_OK    ��ȡ�ɹ�
 * \retval AW_ERROR ��ȡʧ�ܣ��ռ䲻����
 * \retval -EINVAL   ��������
 */
aw_err_t aw_flash_eeprom_read (awbl_flash_eeprom_dev_t *p_dev,
                               uint32_t                 addr,
                               void                    *p_buf,
                               uint32_t                 len);

/**
 * \brief ��EEPROM�ж�ȡ����
 *
 * - ����ӿڴ�FLASH EEPROM�豸��д��len���ֽ�
 *
 * \param[in]  p_dev   FLASH_EEPROM�豸
 * \param[in]  addr    EEPROM�豸�Ĵ�����ַ
 * \param[in]  p_buf   д�����ݵĻ�����
 * \param[in]  len     д������������ֽ�Ϊ��λ��
 *
 * \retval AW_OK       д��ɹ�
 * \retval AW_ERROR    д��ʧ�ܣ�������ĳ����ַ�����ݲ����ڣ�
 * \retval -AW_EINVAL  ��������
 */
aw_err_t aw_flash_eeprom_write (awbl_flash_eeprom_dev_t *p_dev,
                                uint32_t                 addr,
                                void                    *p_buf,
                                uint32_t                 len);

/**
 * \brief ����FLASH EEPROM�豸���ò���
 *
 * \param[in]  p_info      ָ��FLASH EEPROM����
 * \param[in]  name        FLASH EEPROM��FLASH��
 * \param[in]  offset      ƫ������һ�������Ϊ0��
 * \param[in]  block_size  ���С
 * \param[in]  nblocks     ��������������Խ�࣬ʹ�õ�ʱ��Խ����
 * \param[in]  page_size   FLASH��ҳ��С
 *
 * \return ��
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
 * \brief ���EEPROM�������ݣ�������������FLASH�豸�е��������ݡ�
 *
 * \param[in]  p_dev   ָ��FLASH EEPROM�豸
 *
 * \return ��
 */
void aw_flash_eeprom_allclean (awbl_flash_eeprom_dev_t   *p_dev);

/**
 * \brief FLASH EEPROM�豸��ʼ��
 *
 * - ����ӿ�
 *
 * \param[in]  p_dev    ָ��FLASH EEPROM�豸�ṹ��
 * \param[out] p_info   FLASH EEPROM�豸����
 *
 * \retval AW_OK     ��ʼ���ɹ�
 * \retval -EINVAL   ��������
 * \retval -ENXIO    FLASH�豸������
 * \retval AW_ERRPR  ��ʼ��ʧ��
 */
aw_err_t aw_flash_eeprom_init (awbl_flash_eeprom_dev_t     *p_dev,
                               awbl_flash_eeprom_devinfo_t *p_info);

/**
 * \brief FLASH EEPROM�豸ȥ��ʼ��
 *
 * \param[in]  p_dev   ָ��FLASH EEPROM�豸
 *
 * \return ��
 */
void aw_flash_eeprom_uninit (awbl_flash_eeprom_dev_t   *p_dev);


#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif /* __AWBL_SPI_FLASH_H */
