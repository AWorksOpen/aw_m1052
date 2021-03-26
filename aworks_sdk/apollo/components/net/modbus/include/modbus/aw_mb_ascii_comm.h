/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief modbus ASCIIģʽ�����ӿ�.
 *
 * \internal
 * \par modification history
 * - 1.00 2016-03-03 cod, first implementation
 * \endinternal
 */
#ifndef __AW_MB_ASCII_COMM_H
#define __AW_MB_ASCII_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_ascii_comm
 * \copydoc aw_mb_ascii_comm.h
 * @{
 */

/******************************************************************************/
/**
 * \brief ��ASCII 0~9�� A~Fת����ʵ������
 *
 * \param[in] x  0~9�� A~F
 *
 * \return ת�����ֵ
 */
uint8_t aw_mb_xdigit2bin (uint8_t x);
/******************************************************************************/
/**
 * \brief ��0~9��0xA~0xFת����ASCII
 *
 * \param[in] x  0~0xF
 *
 * \return ת�����ֵ
 */
uint8_t aw_mb_bin2xdigit (uint8_t x);
/******************************************************************************/
/**
 * \brief �жϴ������x�Ƿ�Ϊ0~9�� A~F֮���ASCII��
 *
 * \param[in] x  �ж�ֵ
 * \retval    1  ����ֵΪ0~9�� A~F֮���ASCIIֵ
 * \retval    0  ����ֵ�Ĳ���ASCII��0~9�� A~F֮���ֵ
 */
uint8_t aw_mb_isxhdigit (uint8_t x);
/******************************************************************************/
/**
 * \brief ��ȡPDU����֡LRCУ��ֵ
 *
 * \param[in] p_frame  ֡���ݻ�����
 * \param[in] len      ���ݳ���
 *
 * \return ��������Ϊ�������ݳ��ȴ���0,����LRCУ��ֵ�� ���򣬷���0
 */
uint8_t aw_mb_lrc (uint8_t *p_frame, uint16_t len);

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_ascii_comm */

#endif

/* end of file */
