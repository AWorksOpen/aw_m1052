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
 * \brief 字节操作相关定义
 *
 * 本服务提供了最常用的位操作工具宏，例如 AW_LSB() AW_LONGSWAP() 等，更多内容
 * 请参考 \ref grp_aw_if_byteops 。
 *
 * 在所有需要使用Apollo的任何服务的模块(*.<b>c</b>文件)中，都必须使用本服务，
 * 包含 apollo.h 头文件就可以使用本服务
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 13-01-15  orz, move defines from aw_common.h
 * \endinternal
 */

#ifndef __AW_BYTEOPS_H
#define __AW_BYTEOPS_H

/**
 * \addtogroup grp_aw_if_byteops
 * \copydoc aw_byteops.h
 * @{
 */

/**
 * \brief 取2-byte整数的高位byte
 *
 * \par 示例
 * \code
 *  uint16_t a = 0x1234;
 *  uint16_t b;
 *
 *  b = AW_MSB(a);  //b=0x12
 * \endcode
 */
#define AW_MSB(x)   (((x) >> 8) & 0xff)

/**
 * \brief 取2-byte整数的低位byte
 *
 * \par 示例
 * \code
 *  uint16_t a = 0x1234;
 *  uint16_t b;
 *
 *  b = AW_LSB(a);  //b=0x34
 * \endcode
 */
#define AW_LSB(x)   ((x) & 0xff)

/**
 * \brief 取2-word整数的高位word
 *
 * \par 示例
 * \code
 *  uint32_t a = 0x12345678;
 *  uint32_t b;
 *
 *  b = AW_MSW(a);  //b=0x1234
 * \endcode
 */
#define AW_MSW(x)   (((x) >> 16) & 0xffff)

/**
 * \brief 取2-word整数的低位word
 *
 * \par 示例
 * \code
 *  uint32_t a = 0x12345678;
 *  uint32_t b;
 *
 *  b = AW_LSW(a);  //b=0x5678
 * \endcode
 */
#define AW_LSW(x)   ((x) & 0xffff)

/**
 * \brief 交换32-bit整数的高位word和低位word
 *
 * \par 示例
 * \code
 *  uint32_t a = 0x12345678;
 *  uint32_t b;
 *
 *  b = AW_WORDSWAP(a); //b=0x56781234
 * \endcode
 */
#define AW_WORDSWAP(x)  (AW_MSW(x) | (AW_LSW(x) << 16))

/**
 * \brief 交换32-bit整数的字节顺序
 *
 * \par 示例
 * \code
 *  uint32_t a = 0x12345678;
 *  uint32_t b;
 *
 *  b = AW_LONGSWAP(a); //b=0x78563412
 * \endcode
 */
#define AW_LONGSWAP(x)  ((AW_LLSB(x) << 24) | \
                        (AW_LNLSB(x) << 16) | \
                        (AW_LNMSB(x) << 8)  | \
                        (AW_LMSB(x)))

#define AW_LLSB(x)  ((x) & 0xff)            /**< \brief 取32bit整数第1个字节 */
#define AW_LNLSB(x) (((x) >> 8) & 0xff)     /**< \brief 取32bit整数第2个字节 */
#define AW_LNMSB(x) (((x) >> 16) & 0xff)    /**< \brief 取32bit整数第3个字节 */
#define AW_LMSB(x)  (((x) >> 24) & 0xff)    /**< \brief 取32bit整数第4个字节 */

/** @} grp_aw_if_byteops */

#endif /* __AW_BYTEOPS_H */

/* end of file */
