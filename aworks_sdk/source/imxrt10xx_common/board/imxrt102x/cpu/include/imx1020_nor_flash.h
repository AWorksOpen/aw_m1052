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
 * \brief IMX1020 Flexspi NOR Flashͨ�ýӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
 * \code
 * #include imx1020_nor_flash.h
 * \endcode
 *
 * \par ��ʾ��
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
 * \brief Flexspi NOR Flash�ӿɱ�������ж�ȡ����
 *
 * \param[in] p_buf  �����ݻ���
 * \param[in] offset �ڿɱ��flash�����е�ƫ��
 * \param[in] bytes  ��Ҫ��ȡ���ֽ���
 *
 * \retval   AW_OK      �����ɹ�
 * \retval   AW_ENXIO   �豸������
 * \retval   AW_EINVAL  ��Ч����
 */
aw_err_t  imx1020_nor_flash_read (uint8_t *p_buf,
                                  uint32_t offset,
                                  uint32_t nbytes);

/**
 * \brief Flexspi NOR Flash����һ������
 *
 * \param[in] offset �ڿɱ��flash�����е�ƫ��
 *
 * \retval   AW_OK     �����ɹ�
 * \retval   AW_ERROR  ����ʧ��
 * \retval   AW_ENXIO  �豸������
 * \retval   AW_EINVAL ��Ч����
 */
aw_err_t imx1020_nor_flash_sector_erase (uint32_t offset);


/**
 * \brief Flexspi NOR Flash���
 *
 * \param[in] p_buf  ��д�������
 * \param[in] offset �ڿɱ��flash�����е�ƫ��
 * \param[in] bytes  ��д��������ֽ�����
 *
 * \retval   AW_OK     �����ɹ�
 * \retval   AW_ERROR  ����ʧ��
 * \retval   AW_ENXIO  �豸������
 * \retval   AW_EINVAL ��Ч����
 *
 * \note �����Զ�flash��һҳ���б�̣���nbytes����ܳ���256
 *       �ڶ�flash���б��ǰ�������imx1020_nor_flash_sector_erase������ָ��������в���
 */
aw_err_t imx1020_nor_flash_page_program (uint8_t *p_buf,
                                         uint32_t offset,
                                         uint16_t nbytes);


/**
 * \brief Flexspi NOR Flash ��ȡinformation���е�����
 *
 * \param[in] p_buf  �����ݻ���
 * \param[in] row    ����ȡ��information�к�
 * \param[in] offset ����ȡ��information���е�ƫ��
 * \param[in] nbytes ����ȡ���ֽ���
 *
 * \retval    AW_OK     �����ɹ�
 * \retval    AW_ERROR  ����ʧ��
 * \retval    AW_ENXIO  �豸������
 * \retval    AW_EINVAL ��Ч����
 */
aw_err_t imx1020_nor_flash_ir_read(uint8_t *p_buf,
                                   uint8_t  row,
                                   uint32_t offset,
                                   uint32_t nbytes);


/**
 * \brief Flexspi NOR FLash ����һ��information��
 *
 * \param[in] row ��������information�к�
 *
 * \retval    AW_OK     �����ɹ�
 * \retval    AW_ERROR  ����ʧ��
 * \retval    AW_ENXIO  �豸������
 * \retval    AW_EINVAL ��Ч����
 */
aw_err_t imx1020_nor_flash_ir_erase(uint8_t row);


/**
 * \brief Flexspi NOR flash ��information�б��
 *
 * \param[in] p_buf  ��д�������
 * \param[in] offset �ڿɱ��flash�����е�ƫ��
 * \param[in] bytes  ��д��������ֽ�����
 *
 * \retval   AW_OK     �����ɹ�
 * \retval   AW_ERROR  ����ʧ��
 * \retval   AW_ENXIO  �豸������
 * \retval   AW_EINVAL ��Ч����
 *
 * \note information�д�СΪ256bytes�� ����һ��������256�ֽڵı��
 *       ��ʹ��֮ǰ�����imx1020_nor_flash_ir_erase�Դ�д����н��в���
 */
aw_err_t imx1020_nor_flash_ir_program (uint8_t *p_buf,
                                       uint8_t  row,
                                       uint32_t offset,
                                       uint16_t nbytes);


/**
 * \brief Flexspi NOR Flash ��status�Ĵ���
 *
 * \param[in] p_buf  �����ݻ���
 *
 * \retval   AW_OK     �����ɹ�
 * \retval   AW_ERROR  ����ʧ��
 * \retval   AW_ENXIO  �豸������
 * \retval   AW_EINVAL ��Ч����
 */
aw_err_t imx1020_nor_flash_read_sr(uint8_t *p_buf);


/**
 * \brief Flexspi NOR FLash дstatus�Ĵ���
 *
 * \param[in] p_buf  ��д�������
 *
 * \retval   AW_OK     �����ɹ�
 * \retval   AW_ERROR  ����ʧ��
 * \retval   AW_ENXIO  �豸������
 * \retval   AW_EINVAL ��Ч����
 */
aw_err_t imx1020_nor_flash_write_sr(uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flash ��function�Ĵ���
 *
 * \param[in] p_buf  �����ݻ���
 *
 * \retval   AW_OK     �����ɹ�
 * \retval   AW_ERROR  ����ʧ��
 * \retval   AW_ENXIO  �豸������
 * \retval   AW_EINVAL ��Ч����
 */
aw_err_t imx1020_nor_flash_read_fr(uint8_t *p_buf);


/**
 * \brief Flexspi NOR FLash дfunction�Ĵ���
 *
 * \param[in] p_buf  ��д�������
 *
 * \retval   AW_OK     �����ɹ�
 * \retval   AW_ERROR  ����ʧ��
 * \retval   AW_ENXIO  �豸������
 * \retval   AW_EINVAL ��Ч����
 */
aw_err_t imx1020_nor_flash_write_fr(uint8_t *p_buf);


#ifdef __cplusplus
}
#endif

#endif  /* __IMX1020_NOR_FLASH_H */

/* end of file */
