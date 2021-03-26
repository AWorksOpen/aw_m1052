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
 * \brief LoRa����
 *
 * \internal
 * \par Modification history
 * - 1.0.0 18-04-01  sky, first implementation.
 * \endinternal
 */

#ifndef __AM_LORA_TYPE_H
#define __AM_LORA_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * @addtogroup am_if_lora_type
 * @copydoc am_lora_type.h
 * @{
 */

/**
 * \name static inline����
 * @{
 */
#if defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION)
#define am_lora_inline         __inline

/** \brief ARM������inline�ؼ��� */
#define am_lora_static_inline  static  __inline

#elif defined (__ICCARM__)
#define am_lora_inline         inline

/** \brief IAR������inline�ؼ��� */
#define am_lora_static_inline   static inline

#elif defined   (__GNUC__)
#define am_lora_inline         inline

/** \brief GNU������inline�ؼ��� */
#define am_lora_static_inline  static  inline

#elif defined   (__TASKING__)
#define am_lora_inline         inline

/** \brief TASKING������inline�ؼ��� */
#define am_lora_static_inline  static inline

#elif defined   (WIN32)
#define am_lora_inline         __inline
#define am_lora_static_inline  static __inline

#endif /* __CC_ARM */
/** @} */

/**
 * \name �������붨��
 * @{
 */
#if defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION)
#define am_lora_var_aligned(n) __align(n)
#elif defined (__ICCARM__)
#define am_lora_var_aligned(n) _Pragma data_alignment = n
#elif defined (__GNUC__)
#define am_lora_var_aligned(n) __attribute__ ((aligned(n)))
#endif

/** @} */

/**
 * \name attribute weak and alias
 * @{
 */
#if defined(__CC_ARM) || \
    defined(__ARMCOMPILER_VERSION) || \
    defined(__GNUC__)
#define AM_LORA_WEAK_ALIAS(name)  __attribute__((weak, alias(name)))
#endif

/** @} */


/**
 * \name LoRa error number
 * @{
 */
#define AM_LORA_RET_OK          0       /**< \brief ��ȷ */
#define AM_LORA_RET_ERROR       -1      /**< \brief ���� */
#define AM_LORA_ESRCH           3       /**< \brief ���̲����� */
#define AM_LORA_RET_EAGAIN      11      /**< \brief ��Դ�����ã������� */
#define AM_LORA_ENOMEM          12      /**< \brief �ռ䣨�ڴ棩���� */
#define AM_LORA_RET_EBUSY       16      /**< \brief �豸����Դæ */
#define AM_LORA_RET_EINVAL      22      /**< \brief ��Ч���� */
#define AM_LORA_ENOTSUP         35      /**< \brief ��֧�� */
#define AM_LORA_ENOBUFS         55      /**< \brief ����ռ䲻�� */
/** @} */

/**
 * \name LoRa bool type
 * @{
 */
typedef uint8_t am_lora_bool_t;

#ifndef AM_LORA_TRUE
#define AM_LORA_TRUE    1
#endif

#ifndef AM_LORA_FALSE
#define AM_LORA_FALSE   0
#endif

/** @} */

/** \brief void (*pfn) (void *) */
typedef void (*am_lora_pfnvoid_t) (void *);

/**
 * \brief Returns the minimum value between a and b
 * \param [in] a : 1st value
 * \param [in] b : 2nd value
 * \retval minValue Minimum value
 */
#define AM_LORA_MIN(a, b) (((a) < (b)) ? (a) : (b))

/**
 * \brief Returns the maximum value between a and b
 * \param [in] a : 1st value
 * \param [in] b : 2nd value
 * \retval maxValue Maximum value
 */
#define AM_LORA_MAX(a, b) (((a) > (b)) ? (a) : (b))

/**
 * \brief Returns 2 raised to the power of n
 * \param [in] n : power value
 * \retval result of raising 2 to the power n
 */
#define AM_LORA_POW2(n) (1 << n)

/** \brief ��ṹ���Ա��ƫ��*/
#define AM_LORA_OFFSET(structure, member)   \
           ((size_t)(&(((structure *)0)->member)))

/**
 * \brief ͨ���ṹ���Աָ���ȡ�����ýṹ���Ա�Ľṹ��
 */
#define AM_LORA_CONTAINER_OF(ptr, type, member) \
            ((type *)((char *)(ptr) - AM_LORA_OFFSET(type,member)))

/**
 * \brief ��������Ԫ�ظ���
 */
#define AM_LORA_NELEMENTS(array)     (sizeof (array) / sizeof ((array) [0]))




/** \brief bit��λ */
#define AM_LORA_BIT(bit)                  (1u << (bit))

/** \brief bit���� */
#define AM_LORA_BIT_CLR(data, bit)        ((data) &= ~AM_LORA_BIT(bit))

/** \brief bit��λ */
#define AM_LORA_BIT_SET(data, bit)        ((data) |= AM_LORA_BIT(bit))

/**
 * \brief ����һ�����ʽ
 *
 * �����ʽΪ��ʱ����겻���κ����飬Ϊ��ʱ�����am_lora_assert_msg()�������
 * ��Ϣ��������Ϣ��ʽΪ��
 * �ļ������кţ����ʽ
 * \hideinitializer
 */
//#define AM_LORA_DEBUG
#ifdef AM_LORA_DEBUG

/** \brief help macro */
#define _AM_LORA_VTOS(n)     #n              /**< \brief Convert n to string */

/** \brief Get string value of macro m */
#define _AM_LORA_SVAL(m)     _AM_LORA_VTOS(m)

#define am_lora_assert(e) \
    ((e) ? (void)0 : \
        am_lora_assert_msg(__FILE__":"_AM_LORA_SVAL(__LINE__)":("#e")\n"))
extern void am_lora_assert_msg (const char *msg);
#else
#define am_lora_assert(e)    ((void)0)
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LORA_TYPE_H */

/* end of file */
