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
 * \brief ��������ض���ͷ�ļ�
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-28  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_COMPILER_H /* { */
#define __AW_PSP_COMPILER_H

/**
 * \name inline����
 * @{
 */
#if defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION)
#define aw_psp_inline   __inline    /**< \brief ARM������inline�ؼ��� */

#elif defined (__ICCARM__)
#define aw_psp_inline   inline      /**< \brief IAR������inline�ؼ��� */

#elif defined   (__GNUC__)
#define aw_psp_inline   inline      /**< \brief GNU������inline�ؼ��� */

#elif defined   (__TASKING__)
#define aw_psp_inline   inline      /**< \brief TASKING������inline�ؼ��� */

#elif defined   (WIN32)
#define aw_psp_inline   __inline

/** \brief �������������inline�ؼ��ֶ��� */

#endif /* __CC_ARM */
/** @} */

/**
 * \name static inline����
 * @{
 */
#if defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION)
#define aw_psp_inline         __inline
#define aw_psp_static_inline  static  __inline  /**< \brief ARM������inline�ؼ��� */

#elif defined (__ICCARM__)
#define aw_psp_inline         inline
#define aw_psp_tatic_inline   static inline     /**< \brief IAR������inline�ؼ��� */

#elif defined   (__GNUC__)
#define aw_psp_inline         inline
#define aw_psp_static_inline  static  inline    /**< \brief GNU������inline�ؼ��� */

#elif defined   (__TASKING__)
#define aw_psp_inline         inline
#define aw_psp_static_inline  static inline     /**< \brief TASKING������inline�ؼ��� */

#elif defined   (WIN32)
#define aw_psp_inline         __inline
#define aw_psp_static_inline  static __inline

#endif /* __CC_ARM */
/** @} */


/**
 * \name section����
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

/** \brief ���ݱ���������ѡ�� static �ؼ���*/
#define aw_psp_local    static

/** \brief ���ݱ���������ѡ�� const �ؼ���*/
#define aw_psp_const    const

/** \brief ���ݱ���������ѡ�� extern �ؼ���*/
#define aw_psp_import   extern

/** \brief ���ݱ���������ѡ�� extern �ؼ���*/
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
