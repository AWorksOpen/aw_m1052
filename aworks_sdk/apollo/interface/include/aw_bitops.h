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
 * \brief 位操作相关定义头文件
 *
 * 本服务提供了最常用的位操作工具宏，例如 AW_BIT_SET() AW_BIT_GET() 等，更多内容
 * 请参考 \ref grp_aw_if_bitops 。
 *
 * 在所有需要使用Apollo的任何服务的模块(*.<b>c</b>文件)中，都必须使用本服务，
 * 包含 apollo.h 头文件就可以使用本服务
 *
 * // 更多内容待添加。。。

 * \internal
 * \par modification history:
 * - 1.00 13-01-15  orz, move defines from aw_common.h
 * \endinternal
 */

#ifndef __AW_BITOPS_H
#define __AW_BITOPS_H

/**
 * \addtogroup grp_aw_if_bitops
 * \copydoc aw_bitops.h
 * @{
 */

#include "aw_io.h"
#include "aw_common.h"

/******************************************************************************/

/**
 * \name 通用位常数定义
 * @{
 */

/** \brief 长整数位数 */
#ifndef BITS_PER_LONG
#define BITS_PER_LONG       32
#endif

/** \brief 字节位数 */
#define BITS_PER_BYTE       8

/** @} */

/******************************************************************************/

/**
 * \name 通用位操作
 * @{
 */

/** \brief 计算n bits掩码 */
#define BIT_MASK(nr)        (1UL << ((nr) % BITS_PER_LONG))

/** \brief bits转换为字个数 */
#define BIT_WORD(nr)        ((nr) / BITS_PER_LONG)

/** \brief bit在long型bit map中的索引 */
#define BITS_TO_LONGS(nr)   AW_DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

/** \brief bit移位 */
#define AW_BIT(bit)              (1u << (bit))

/** \brief 值移位 */
#define AW_SBF(value, field)     ((value) << (field))

/** \brief bit置位 */
#define AW_BIT_SET(data, bit)    ((data) |= AW_BIT(bit))

/** \brief bit清零 */
#define AW_BIT_CLR(data, bit)    ((data) &= ~AW_BIT(bit))

/** \brief bit置位, 根据 mask 指定的位 */
#define AW_BIT_SET_MASK(data, mask)    ((data) |= (mask))

/** \brief bit清零, 根据 mask 指定的位 */
#define AW_BIT_CLR_MASK(data, mask)    ((data) &= ~(mask))

/** \brief bit翻转 */
#define AW_BIT_TOGGLE(data, bit) ((data) ^= AW_BIT(bit))

/** \brief bit修改 */
#define AW_BIT_MODIFY(data, bit, value) \
    ((value) ? AW_BIT_SET(data, bit) : AW_BIT_CLR(data, bit))

/** \brief 测试bit是否置位 */
#define AW_BIT_ISSET(data, bit)  ((data) & AW_BIT(bit))

/** \brief 获取bit值 */
#define AW_BIT_GET(data, bit)    (AW_BIT_ISSET(data, bit) ? 1 : 0)

/** \brief 获取 n bits 掩码值 */
#define AW_BITS_MASK(n)          (~((~0u) << (n)))

/** \brief 获取位段值 */
#define AW_BITS_GET(data, start, len)  \
    (((data) >> (start)) & AW_BITS_MASK(len))

/** \brief 设置位段值 */
#define AW_BITS_SET(data, start, len, value) \
    ((data) = (((data) & ~AW_SBF(AW_BITS_MASK(len), (start))) | \
               AW_SBF((value) & (AW_BITS_MASK(len)), (start))))


/**
 * \brief 查找置位的最高有效位
 *
 * 查找一个32-bit数置位的的最高有效位，返回该位的索引。
 *
* This routine finds the most significant bit set in the 32 bit argument
* passed to it and returns the index of that bit.  Bits are numbered starting
* at 1 from the least signifficant bit.  A return value of zero indicates that
* the value passed is zero.
*
* RETURNS: index of most significant bit set, or zero */
#define AW_FFS(a)   AW_PLFM_FFS(a)

#define AW_FLS(a)   AW_PFLM_FFS(a)

/** @} */

/******************************************************************************/

/**
 * \name 32位寄存器读写操作
 * @{
 */

#define AW_REG_READ32(addr)        readl((volatile void __iomem *)(addr))
#define AW_REG_WRITE32(addr, data) writel(data, (volatile void __iomem *)(addr))

#define AW_REG_BIT_SET32(addr, bit) \
    do { \
        uint32_t __val__ = readl((volatile void __iomem *)(addr)); \
        AW_BIT_SET(__val__, bit); \
        writel(__val__, (volatile void __iomem *)(addr)); \
    } while (0)


#define AW_REG_BIT_CLR32(addr, bit) \
    do { \
        uint32_t __val__ = readl((volatile void __iomem *)(addr)); \
        AW_BIT_CLR(__val__, bit); \
        writel(__val__, (volatile void __iomem *)(addr)); \
    } while (0)

#define AW_REG_BIT_SET_MASK32(addr, mask) \
    do { \
        uint32_t __val__ = readl((volatile void __iomem *)(addr)); \
        AW_BIT_SET_MASK(__val__, mask); \
        writel(__val__, (volatile void __iomem *)(addr)); \
    } while (0)


#define AW_REG_BIT_CLR_MASK32(addr, mask) \
    do { \
        uint32_t __val__ = readl((volatile void __iomem *)(addr)); \
        AW_BIT_CLR_MASK(__val__, mask); \
        writel(__val__, (volatile void __iomem *)(addr)); \
    } while (0)

#define AW_REG_BIT_ISSET32(addr, bit) \
    AW_BIT_ISSET(readl((volatile void __iomem *)(addr)), bit)

#define AW_REG_BIT_GET32(addr, bit) \
    AW_BIT_GET(readl((volatile void __iomem *)(addr)), bit)

#define AW_REG_BITS_GET32(addr, start, len) \
    AW_BITS_GET(readl((volatile void __iomem *)(addr)), start, len)

#define AW_REG_BIT_MODIFY32(addr, bit, value) \
    do { \
        uint32_t __val__ = readl((volatile void __iomem *)(addr)); \
        AW_BIT_MODIFY(__val__, bit, value); \
        writel(__val__, (volatile void __iomem *)(addr)); \
    } while (0)

#define AW_REG_BITS_SET32(addr, start, len, value) \
    do { \
        uint32_t __val__ = readl((volatile void __iomem *)(addr)); \
        AW_BITS_SET(__val__, start, len, value); \
        writel(__val__, (volatile void __iomem *)(addr)); \
    } while (0)

/** \todo 检查定义是否完整*/

/** @} */

/** @} grp_aw_if_bitops */

#endif /* __AW_BITOPS_H */

/* end of file */
