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
 * \brief Flexspi NOR Flashͨ�ýӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
 * \code
 * #include aw_flexspi_nor_flash.h
 * \endcode
 *
 * \par ��ʾ��
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
 * \brief Flexspi NOR Flash��״̬�Ĵ���
 *
 * \param[in] p_buf  ���ݶ�����
 *
 * \retval   AW_OK      ��ȡ�ɹ�
 * \retval   AW_ERROR   ��ȡʧ��
 */
aw_err_t imx1010_nor_flash_read_sr (uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flashдstatus�Ĵ���
 *
 * \param[in] p_buf  ��д�������
 *
 * \retval   AW_OK      д��ɹ�
 * \retval   AW_ERROR   д��ʧ��
 */
aw_err_t imx1010_nor_flash_write_sr (uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flash��function�Ĵ���
 *
 * \param[in] p_buf  ���ݶ�����
 *
 * \retval   AW_OK      ��ȡ�ɹ�
 * \retval   AW_ERROR   ��ȡʧ��
 */
aw_err_t imx1010_nor_flash_read_fr (uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flashдfunction�Ĵ���
 *
 * \param[in] p_buf  ��д�������
 *
 * \retval   AW_OK      д��ɹ�
 * \retval   AW_ERROR   д��ʧ��
 */
aw_err_t imx1010_nor_flash_write_fr (uint8_t *p_buf);


/**
 * \brief Flexspi NOR Flash��ָ����ַ������
 *
 * \param[in] p_buf     ���ݶ�����
 * \param[in] offset    ƫ�Ƶ�ַ
 * \param[in] nbytes    ��Ҫ��ȡ�������ֽ���
 *
 * \retval   AW_OK      ��ȡ�ɹ�
 * \retval   AW_ERROR   ��ȡʧ��
 */
aw_err_t imx1010_nor_flash_read (uint8_t *p_buf, uint32_t offset, uint32_t nbytes);


/**
 * \brief Flexspi NOR Flash����һ������
 *
 * \param[in] offset    ����ƫ�Ƶ�ַ
 *
 * \retval   AW_OK      �����ɹ�
 * \retval   AW_ERROR   ����ʧ��
 */
aw_err_t imx1010_nor_flash_sector_erase (uint32_t offset);


/**
 * \brief Flexspi NOR Flash���
 *
 * \param[in] p_buf     ��д�������
 * \param[in] offset    ƫ�Ƶ�ַ
 * \param[in] nbytes    ��д��������ֽ���
 *
 * \retval   AW_OK     �����ɹ�
 * \retval   AW_ERROR  ����ʧ��
 */
aw_err_t imx1010_nor_flash_page_program (uint8_t *p_buf,
                                         uint32_t offset,
                                         uint16_t nbytes);


/**
 * \brief Flexspi NOR Flash ����һ����������
 *
 * \param[in] row  ������ţ�0-3����������С256byte
 *
 * \retval   AW_OK     �����ɹ�
 * \retval   AW_ERROR  ����ʧ��
 */
aw_err_t imx1010_nor_flash_ir_erase (uint8_t row);


/**
 * \brief Flexspi NOR Flash д��������
 *
 * \param[in] p_buf  ��д�������
 * \param[in] row    ������ţ�0-3����������С256byte
 * \param[in] offset ƫ�Ƶ�ַ
 * \param[in] nbytes ��д��������ֽ���
 *
 * \retval   AW_OK     �����ɹ�
 * \retval   AW_ERROR  ����ʧ��
 */
aw_err_t imx1010_nor_flash_ir_program (uint8_t *p_buf,
                                       uint8_t  row,
                                       uint32_t offset,
                                       uint16_t nbytes);


/**
 * \brief Flexspi NOR Flash ����������
 *
 * \param[in] p_buf     ���ݶ�����
 * \param[in] row       ������ţ�0-3����������С256byte
 * \param[in] offset    ƫ�Ƶ�ַ
 * \param[in] nbytes    ��Ҫ��ȡ�������ֽ���
 *
 * \retval   AW_OK      ��ȡ�ɹ�
 * \retval   AW_ERROR   ��ȡʧ��
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
