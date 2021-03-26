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
 * \brief Apollo通用工具宏
 *
 * 本服务提供了最常用的工具宏，例如 min() max() offsetof() 等，更多内容请参考
 * \ref grp_aw_if_common 。
 *
 * 在所有需要使用Apollo的任何服务的模块(*.<b>c</b>文件)中，都必须使用本服务，
 * 包含 apollo.h 头文件就可以使用本服务:
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.02 13-01-15  orz, move some defines to separate files.
 * - 1.01 12-09-05  orz, fix some defines.
 * - 1.00 12-08-23  zen, first implementation.
 * \endinternal
 */

#ifndef __AW_COMMON_H
#define __AW_COMMON_H

/**
 * \addtogroup grp_aw_if_common
 * \copydoc aw_common.h
 * @{
 */
#include "aw_types.h"
#include "aw_compiler.h"
#include "aw_psp_common.h"     /* 平台相关标准定义头文件 */
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/******************************************************************************/

/**
 * \name 通用常量定义
 * @{
 */

#define AW_OK               0   /**< \brief OK */
#define AW_ERROR          (-1)  /**< \brief generic error */

#define AW_NO_WAIT          0                   /**< \brief timeout: no wait */
#define AW_WAIT_FOREVER     (unsigned long)(-1) /**< \brief timeout: wait forever */

#ifndef EOF
#define EOF               (-1)  /** @brief end of file */
#endif

#define NONE              (-1)  /**< \brief for times when NULL won't do */
#define EOS               '\0'  /**< \brief C string terminator */

#ifndef NULL
#define NULL       ((void *)0)  /**< NULL pointer */
#endif

#define AW_LITTLE_ENDIAN 1234   /**< \brief 目标机器小端 */
#define AW_BIG_ENDIAN    3412   /**< \brief 目标机器大端 */

/** @} */

/******************************************************************************/

/**
 * \name 常用宏定义
 * @{
 */

/**
 * \brief 求结构体成员的偏移
 * \attention 不同平台上，由于成员大小和内存对齐等原因，
 *            同一结构体成员的偏移可能是不一样的
 *
 * \par 示例
 * \code
 *  struct my_struct = {
 *      int  m1;
 *      char m2;
 *  };
 *  int offset_m2;
 *
 *  offset_m2 = AW_OFFSET(struct my_struct, m2);
 * \endcode
 */
#define AW_OFFSET(structure, member)    ((size_t)(&(((structure *)0)->member)))

/** \brief 求结构体成员的偏移，同 \ref AW_OFFSET 一样 */
#ifndef offsetof
#define offsetof(type, member)          AW_OFFSET(type, member)
#endif

/**
 * \brief 通过结构体成员指针获取包含该结构体成员的结构体
 *
 * \param ptr    指向结构体成员的指针
 * \param type   结构体类型
 * \param member 结构体中该成员的名称
 *
 * \par 示例
 * \code
 *  struct my_struct = {
 *      int  m1;
 *      char m2;
 *  };
 *  struct my_struct  my_st;
 *  char             *p_m2 = &my_st.m2;
 *  struct my_struct *p_st = AW_CONTAINER_OF(p_m2, struct my_struct, m2);
 * \endcode
 */
#define AW_CONTAINER_OF(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type,member)))

/**
 * \brief 通过结构体成员指针获取包含该结构体成员的结构体，
 *        同 \ref AW_CONTAINER_OF 一样
 */
#ifndef container_of
#define container_of(ptr, type, member)     AW_CONTAINER_OF(ptr, type, member)
#endif

/** \brief prefetch */
#ifndef prefetch
#define prefetch(x) (void)0
#endif

/**
 * \brief 计算结构体成员的大小
 *
 * \code
 *  struct a = {
 *      uint32_t m1;
 *      uint32_t m2;
 *  };
 *  int size_m2;
 *
 *  size_m2 = AW_MEMBER_SIZE(a, m2);   //size_m2 = 4
 * \endcode
 */
#define AW_MEMBER_SIZE(structure, member)  (sizeof(((structure *)0)->member))

/**
 * \brief 计算数组元素个数
 *
 * \code
 *  int a[] = {0, 1, 2, 3};
 *  int element_a = AW_NELEMENTS(a);    // element_a = 4
 * \endcode
 */
#define AW_NELEMENTS(array) (sizeof (array) / sizeof ((array) [0]))

/**
 * \brief 永久循环
 *
 * 写永远循环的语句块时大部分程序员喜欢用while(1)这样的语句，
 * 而有些喜欢用for(;;)这样的语句。while(1)在某些老的或严格的编译器上
 * 可能会出现警告，因为1是数字而不是逻辑表达式
 *
 * \code
 * AW_FOREVER {
 *     ; // 循环做一些事情或满足某些条件后跳出
 * }
 * \endcode
 */
#define AW_FOREVER  for (;;)

/******************************************************************************/

/*
 * min & max are C++ standard functions which are provided by the user
 * side C++ libraries
 */
#ifndef __cplusplus

/**
 * \brief 获取2个数中的较大的数值
 * \param x 数字1
 * \param y 数字2
 * \return 2个数中的较大的数值
 * \note 参数不能使用++或--操作
 */
#ifndef max
#define max(x, y)   (((x) < (y)) ? (y) : (x))
#endif

/**
 * \brief 获取2个数中的较小的数值
 * \param x 数字1
 * \param y 数字2
 * \return 2个数中的较小数值
 * \note 参数不能使用++或--操作
 */
#ifndef min
#define min(x, y)   (((x) < (y)) ? (x) : (y))
#endif

#endif /* __cplusplus */

/**
 * \brief 向上舍入
 *
 * \param x     被运算的数
 * \param align 对齐因素，必须为2的乘方
 *
 * \code
 * int size = AW_ROUND_UP(15, 4);   // size = 16
 * \endcode
 */
#define AW_ROUND_UP(x, align)   (((int) (x) + (align - 1)) & ~(align - 1))

/**
 * \brief 向下舍入
 *
 * \param x     被运算的数
 * \param align 对齐因素，必须为2的乘方
 *
 * \code
 * int size = AW_ROUND_DOWN(15, 4);   // size = 12
 * \endcode
 */
#define AW_ROUND_DOWN(x, align) ((int)(x) & ~(align - 1))

/** \brief 倍数向上舍入 */
#define AW_DIV_ROUND_UP(n, d)   (((n) + (d) - 1) / (d))

/**
 * \brief 测试是否对齐
 *
 * \param x     被运算的数
 * \param align 对齐因素，必须为2的乘方
 *
 * \code
 * if (AW_ALIGNED(x, 4) {
 *     ; // x对齐
 * } else {
 *     ; // x不对齐
 * }
 * \endcode
 */
#define AW_ALIGNED(x, align)    (((int)(x) & (align - 1)) == 0)

/******************************************************************************/

/** \brief 把宏展开后的结果转换为字符串 */
#define AW_XSTR(s) AW_STR(s)

/** \brief 把符号转换为字符串 */
#define AW_STR(s) #s

/******************************************************************************/

/** \brief 将1字节BCD数据转换为16进制数据 */
#define AW_BCD_TO_HEX(val)  (((val) & 0x0f) + ((val) >> 4) * 10)

/** \brief 将1字节16进制数据转换为BCD数据 */
#define AW_HEX_TO_BCD(val)  ((((val) / 10) << 4) + (val) % 10)

/** @} */

/*******************************************************************************
    引出的通用头文件
*******************************************************************************/
#include "aw_io.h"
#include "aw_bitops.h"
#include "aw_byteops.h"
#include "aw_system.h"


#ifdef __cplusplus
}
#endif    /* __cplusplus     */

/** @} grp_aw_if_common */

#endif    /* __AW_COMMON_H    */

/* end of file */
