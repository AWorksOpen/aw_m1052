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
 * \brief Apolloͨ�ù��ߺ�
 *
 * �������ṩ����õĹ��ߺ꣬���� min() max() offsetof() �ȣ�����������ο�
 * \ref grp_aw_if_common ��
 *
 * ��������Ҫʹ��Apollo���κη����ģ��(*.<b>c</b>�ļ�)�У�������ʹ�ñ�����
 * ���� apollo.h ͷ�ļ��Ϳ���ʹ�ñ�����:
 *
 * // �������ݴ���ӡ�����
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
#include "aw_psp_common.h"     /* ƽ̨��ر�׼����ͷ�ļ� */
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/******************************************************************************/

/**
 * \name ͨ�ó�������
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

#define AW_LITTLE_ENDIAN 1234   /**< \brief Ŀ�����С�� */
#define AW_BIG_ENDIAN    3412   /**< \brief Ŀ�������� */

/** @} */

/******************************************************************************/

/**
 * \name ���ú궨��
 * @{
 */

/**
 * \brief ��ṹ���Ա��ƫ��
 * \attention ��ͬƽ̨�ϣ����ڳ�Ա��С���ڴ�����ԭ��
 *            ͬһ�ṹ���Ա��ƫ�ƿ����ǲ�һ����
 *
 * \par ʾ��
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

/** \brief ��ṹ���Ա��ƫ�ƣ�ͬ \ref AW_OFFSET һ�� */
#ifndef offsetof
#define offsetof(type, member)          AW_OFFSET(type, member)
#endif

/**
 * \brief ͨ���ṹ���Աָ���ȡ�����ýṹ���Ա�Ľṹ��
 *
 * \param ptr    ָ��ṹ���Ա��ָ��
 * \param type   �ṹ������
 * \param member �ṹ���иó�Ա������
 *
 * \par ʾ��
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
 * \brief ͨ���ṹ���Աָ���ȡ�����ýṹ���Ա�Ľṹ�壬
 *        ͬ \ref AW_CONTAINER_OF һ��
 */
#ifndef container_of
#define container_of(ptr, type, member)     AW_CONTAINER_OF(ptr, type, member)
#endif

/** \brief prefetch */
#ifndef prefetch
#define prefetch(x) (void)0
#endif

/**
 * \brief ����ṹ���Ա�Ĵ�С
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
 * \brief ��������Ԫ�ظ���
 *
 * \code
 *  int a[] = {0, 1, 2, 3};
 *  int element_a = AW_NELEMENTS(a);    // element_a = 4
 * \endcode
 */
#define AW_NELEMENTS(array) (sizeof (array) / sizeof ((array) [0]))

/**
 * \brief ����ѭ��
 *
 * д��Զѭ��������ʱ�󲿷ֳ���Աϲ����while(1)��������䣬
 * ����Щϲ����for(;;)��������䡣while(1)��ĳЩ�ϵĻ��ϸ�ı�������
 * ���ܻ���־��棬��Ϊ1�����ֶ������߼����ʽ
 *
 * \code
 * AW_FOREVER {
 *     ; // ѭ����һЩ���������ĳЩ����������
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
 * \brief ��ȡ2�����еĽϴ����ֵ
 * \param x ����1
 * \param y ����2
 * \return 2�����еĽϴ����ֵ
 * \note ��������ʹ��++��--����
 */
#ifndef max
#define max(x, y)   (((x) < (y)) ? (y) : (x))
#endif

/**
 * \brief ��ȡ2�����еĽ�С����ֵ
 * \param x ����1
 * \param y ����2
 * \return 2�����еĽ�С��ֵ
 * \note ��������ʹ��++��--����
 */
#ifndef min
#define min(x, y)   (((x) < (y)) ? (x) : (y))
#endif

#endif /* __cplusplus */

/**
 * \brief ��������
 *
 * \param x     ���������
 * \param align �������أ�����Ϊ2�ĳ˷�
 *
 * \code
 * int size = AW_ROUND_UP(15, 4);   // size = 16
 * \endcode
 */
#define AW_ROUND_UP(x, align)   (((int) (x) + (align - 1)) & ~(align - 1))

/**
 * \brief ��������
 *
 * \param x     ���������
 * \param align �������أ�����Ϊ2�ĳ˷�
 *
 * \code
 * int size = AW_ROUND_DOWN(15, 4);   // size = 12
 * \endcode
 */
#define AW_ROUND_DOWN(x, align) ((int)(x) & ~(align - 1))

/** \brief ������������ */
#define AW_DIV_ROUND_UP(n, d)   (((n) + (d) - 1) / (d))

/**
 * \brief �����Ƿ����
 *
 * \param x     ���������
 * \param align �������أ�����Ϊ2�ĳ˷�
 *
 * \code
 * if (AW_ALIGNED(x, 4) {
 *     ; // x����
 * } else {
 *     ; // x������
 * }
 * \endcode
 */
#define AW_ALIGNED(x, align)    (((int)(x) & (align - 1)) == 0)

/******************************************************************************/

/** \brief �Ѻ�չ����Ľ��ת��Ϊ�ַ��� */
#define AW_XSTR(s) AW_STR(s)

/** \brief �ѷ���ת��Ϊ�ַ��� */
#define AW_STR(s) #s

/******************************************************************************/

/** \brief ��1�ֽ�BCD����ת��Ϊ16�������� */
#define AW_BCD_TO_HEX(val)  (((val) & 0x0f) + ((val) >> 4) * 10)

/** \brief ��1�ֽ�16��������ת��ΪBCD���� */
#define AW_HEX_TO_BCD(val)  ((((val) / 10) << 4) + (val) % 10)

/** @} */

/*******************************************************************************
    ������ͨ��ͷ�ļ�
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
