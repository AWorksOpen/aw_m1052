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
 * \brief 编译器相关定义头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-28  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_COMPILER_H /* { */
#define __AW_PSP_COMPILER_H

/**
 * \name inline定义
 * @{
 */
#if defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION)
#define aw_psp_inline   __inline    /**< \brief ARM编译器inline关键字 */

#elif defined (__ICCARM__)
#define aw_psp_inline   inline      /**< \brief IAR编译器inline关键字 */

#elif defined   (__GNUC__)
#define aw_psp_inline   inline      /**< \brief GNU编译器inline关键字 */

#elif defined   (__TASKING__)
#define aw_psp_inline   inline      /**< \brief TASKING编译器inline关键字 */

#elif defined   (WIN32)
#define aw_psp_inline   __inline

/** \brief 添加其它编译器inline关键字定义 */

#endif /* __CC_ARM */
/** @} */

/**
 * \name static inline定义
 * @{
 */
#if defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION)
#define aw_psp_inline         __inline
#define aw_psp_static_inline  static  __inline  /**< \brief ARM编译器inline关键字 */

#elif defined (__ICCARM__)
#define aw_psp_inline         inline
#define aw_psp_tatic_inline   static inline     /**< \brief IAR编译器inline关键字 */

#elif defined   (__GNUC__)
#define aw_psp_inline         inline
#define aw_psp_static_inline  static  inline    /**< \brief GNU编译器inline关键字 */

#elif defined   (__TASKING__)
#define aw_psp_inline         inline
#define aw_psp_static_inline  static inline     /**< \brief TASKING编译器inline关键字 */

#elif defined   (WIN32)
#define aw_psp_inline         __inline
#define aw_psp_static_inline  static __inline

#endif /* __CC_ARM */
/** @} */


/**
 * \name section定义
 * @{
 */
#if defined(__CC_ARM) || defined(__ARMCOMPILER_VERSION) /* RealView Compiler */
#define aw_psp_section(x)    __attribute__((section(x)))

#elif defined (__ICCARM__)  /* IAR Compiler */
#define aw_psp_section(x)    \ x

#elif defined (__GNUC__)    /* GNU GCC Compiler */
#define aw_psp_section(x)    __attribute__((section(x)))

#elif defined   (WIN32)
#define aw_psp_section(x)

#endif
/** @} */

/** \brief 根据编译器类型选择 static 关键字*/
#define aw_psp_local    static

/** \brief 根据编译器类型选择 const 关键字*/
#define aw_psp_const    const

/** \brief 根据编译器类型选择 extern 关键字*/
#define aw_psp_import   extern

/** \brief 根据编译器类型选择 extern 关键字*/
#define aw_psp_export   extern

#if !(defined(__GNUC__)) || defined(__ARMCOMPILER_VERSION)
#define restrict
#endif

#if defined(__GNUC__) && !(defined(__ARMCOMPILER_VERSION))
# define likely(x)      __builtin_expect(!!(x), 1)
# define unlikely(x)    __builtin_expect(!!(x), 0)
#else
# define likely(x)        x
# define unlikely(x)      x
#endif



#endif /* } __AW_PSP_COMPILER_H */

/* end of file */
