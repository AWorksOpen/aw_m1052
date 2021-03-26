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
 * \brief modbus ���ù���ͷ�ļ�.
 *
 * \internal
 * \par modification history
 * - 1.02 2015-05-18  cod, fix code style and comments, a little improve.
 * - 1.01 2013-03-29  liuweiyun, fix code style and comments, a little improve.
 * - 1.00 2012-05-03  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_UTILS_H /* { */
#define __AW_MB_UTILS_H

#include "modbus/aw_mb_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_utils
 * \copydoc aw_mb_utils.h
 * @{
 */

/**
 * \brief �ú�����ȡ������һ���ֽڵ�λ
 *
 * \note  �ú�����߿ɶ�8λ����
 *
 * \param[in] p_bytebuf   һ���ֽڣ�8λ���Ļ�����
 * \param[in] bit_offset  ��ȡλ��ƫ����
 * \param[in] nbits       ��ȡλ������С��8λ
 *
 * \return ����ȡ��λ��ֵ
 *
 * \par ʾ����
 * \code
 * uint8_t bits_buf[2] = {0, 0};
 * uint8_t value;
 *
 * // Extract the bits 3 - 10.
 * value = aw_mb_bits_get(bits_buf, 3, 8);
 * \endcode
 */
uint8_t aw_mb_bits_get (const uint8_t *p_bytebuf, int bit_offset, int nbits);

/**
 * \brief �ú���������ĳ��λ��һ���ֽڵĻ�����
 *
 * \note �ú���һ�β�����߿�����8bit����ƫ��λ�����2���ֽڣ� Ҫ�󻺳�����������2���ֽڣ���ֹƫ��λ��������Խ�硣
 *
 * \param[in] p_bytebuf   д�뻺������
 * \param[in] bit_offset  ƫ��λ��
 * \param[in] nbits       ����λ��
 * \param[in] value       ����λ����ֵ����ֵ�����λΪbit_offset���õ�λֵ
 *
 * \return ��
 *
 * \par ʾ����
 * \code
 * uint8_t bits_buf[2] = {0, 0};
 *
 * // Set bit 4 to 1 (set 1 bit starting at bit offset 4 to value 1)
 * aw_mb_bits_set(bits_buf, 4, 1, 1);
 *
 * // Set bit 7 to 1 and bit 8 to 0.
 * aw_mb_bits_set(bits_buf, 7, 2, 0x01);
 *
 * // Set bits 8 - 11 to 0x05 and bits 12 - 15 to 0x0A;
 * aw_mb_bits_set(bits_buf, 8, 8, 0x5A);
 * \endcode
 */
void aw_mb_bits_set (uint8_t *p_bytebuf,
                     int      bit_offset,
                     int      nbits,
                     uint8_t  value);

/**
 * \brief ��ȡp_bitsbuf������ָ��λ��ֵ��p_bytesbuf������
 *
 * \note  �ú����������ڴ��ص�
 *
 * \param[in,out] p_bytesbuf  ���ݻ�����
 * \param[in]     p_bitsbuf   λֵ�����������λֵ
 * \param[in]     bit_offset  ƫ��λ��
 * \param[in]     nbits       д��λ��
 *
 * \return ��
 */
void aw_mb_bits_cp_to_buf (uint8_t       *p_bytesbuf,
                           const uint8_t *p_bitsbuf,
                           int            bit_offset,
                           int            nbits);

/**
 * \brief �ӻ���������bits��ֵ������p_bytesbuf�������б����λ��ֵ����p_bitsbuf�������Ķ�Ӧλ��
 *
 * \note  �ú����������ڴ��ص�
 *
 * \param[in,out] p_bitsbuf   �������ݻ�����
 * \param[in]     p_bytesbuf  λֵ�����������λֵ
 * \param[in]     bit_offset  ƫ��λ������Ҫ���õ���ʼλƫ��ֵ
 * \param[in]     nbits       ����λ��
 *
 * \return ��
 */
void aw_mb_bits_set_from_buf (uint8_t       *p_bitsbuf,
                              const uint8_t *p_bytesbuf,
                              int            bit_offset,
                              int            nbits);
/**
 * \brief ����16bits�Ĵ�������
 *
 * \note  Modbus PDU���ģʽ����ת��ΪĬ��С��ģʽ������
 *
 * \param[in,out] p_dst     ���������
 * \param[in]     p_src     �������ݻ�����
 * \param[in]     num_reg   ���ƵļĴ�������2�ı���
 *
 * \return ��
 */
void aw_mb_regcpy (void *p_dst, const void *p_src, uint16_t num_reg);

/**
 * @} grp_aw_if_mb_utils
 */

#ifdef __cplusplus
}
#endif

#endif /* } __AW_MB_UTILS_H */

/* end of file */
