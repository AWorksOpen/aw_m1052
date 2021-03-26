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
 * \brief modbus 常用功能头文件.
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
 * \brief 该函数读取缓冲区一个字节的位
 *
 * \note  该函数最高可读8位数据
 *
 * \param[in] p_bytebuf   一个字节（8位）的缓冲区
 * \param[in] bit_offset  读取位的偏移数
 * \param[in] nbits       读取位数，须小于8位
 *
 * \return 欲读取的位域值
 *
 * \par 示例：
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
 * \brief 该函数可设置某几位到一个字节的缓冲区
 *
 * \note 该函数一次操作最高可设置8bit，若偏移位数横跨2个字节， 要求缓冲区必须至少2个字节，防止偏移位数过长而越界。
 *
 * \param[in] p_bytebuf   写入缓冲区。
 * \param[in] bit_offset  偏移位数
 * \param[in] nbits       设置位数
 * \param[in] value       设置位数的值。该值的最低位为bit_offset设置的位值
 *
 * \return 无
 *
 * \par 示例：
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
 * \brief 读取p_bitsbuf区域中指定位域值到p_bytesbuf缓冲区
 *
 * \note  该函数不考虑内存重叠
 *
 * \param[in,out] p_bytesbuf  数据缓冲区
 * \param[in]     p_bitsbuf   位值缓冲区，存放位值
 * \param[in]     bit_offset  偏移位数
 * \param[in]     nbits       写入位数
 *
 * \return 无
 */
void aw_mb_bits_cp_to_buf (uint8_t       *p_bytesbuf,
                           const uint8_t *p_bitsbuf,
                           int            bit_offset,
                           int            nbits);

/**
 * \brief 从缓冲区设置bits的值，根据p_bytesbuf缓冲区中保存的位域值设置p_bitsbuf缓冲区的对应位域
 *
 * \note  该函数不考虑内存重叠
 *
 * \param[in,out] p_bitsbuf   设置数据缓冲区
 * \param[in]     p_bytesbuf  位值缓冲区，存放位值
 * \param[in]     bit_offset  偏移位数，需要设置的起始位偏移值
 * \param[in]     nbits       设置位数
 *
 * \return 无
 */
void aw_mb_bits_set_from_buf (uint8_t       *p_bitsbuf,
                              const uint8_t *p_bytesbuf,
                              int            bit_offset,
                              int            nbits);
/**
 * \brief 复制16bits寄存器数据
 *
 * \note  Modbus PDU大端模式数据转换为默认小端模式的数据
 *
 * \param[in,out] p_dst     输出缓冲区
 * \param[in]     p_src     输入数据缓冲区
 * \param[in]     num_reg   复制的寄存器数，2的倍数
 *
 * \return 无
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
