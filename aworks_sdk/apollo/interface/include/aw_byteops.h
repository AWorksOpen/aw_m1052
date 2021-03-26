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
 * \brief �ֽڲ�����ض���
 *
 * �������ṩ����õ�λ�������ߺ꣬���� AW_LSB() AW_LONGSWAP() �ȣ���������
 * ��ο� \ref grp_aw_if_byteops ��
 *
 * ��������Ҫʹ��Apollo���κη����ģ��(*.<b>c</b>�ļ�)�У�������ʹ�ñ�����
 * ���� apollo.h ͷ�ļ��Ϳ���ʹ�ñ�����
 *
 * // �������ݴ���ӡ�����
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
 * \brief ȡ2-byte�����ĸ�λbyte
 *
 * \par ʾ��
 * \code
 *  uint16_t a = 0x1234;
 *  uint16_t b;
 *
 *  b = AW_MSB(a);  //b=0x12
 * \endcode
 */
#define AW_MSB(x)   (((x) >> 8) & 0xff)

/**
 * \brief ȡ2-byte�����ĵ�λbyte
 *
 * \par ʾ��
 * \code
 *  uint16_t a = 0x1234;
 *  uint16_t b;
 *
 *  b = AW_LSB(a);  //b=0x34
 * \endcode
 */
#define AW_LSB(x)   ((x) & 0xff)

/**
 * \brief ȡ2-word�����ĸ�λword
 *
 * \par ʾ��
 * \code
 *  uint32_t a = 0x12345678;
 *  uint32_t b;
 *
 *  b = AW_MSW(a);  //b=0x1234
 * \endcode
 */
#define AW_MSW(x)   (((x) >> 16) & 0xffff)

/**
 * \brief ȡ2-word�����ĵ�λword
 *
 * \par ʾ��
 * \code
 *  uint32_t a = 0x12345678;
 *  uint32_t b;
 *
 *  b = AW_LSW(a);  //b=0x5678
 * \endcode
 */
#define AW_LSW(x)   ((x) & 0xffff)

/**
 * \brief ����32-bit�����ĸ�λword�͵�λword
 *
 * \par ʾ��
 * \code
 *  uint32_t a = 0x12345678;
 *  uint32_t b;
 *
 *  b = AW_WORDSWAP(a); //b=0x56781234
 * \endcode
 */
#define AW_WORDSWAP(x)  (AW_MSW(x) | (AW_LSW(x) << 16))

/**
 * \brief ����32-bit�������ֽ�˳��
 *
 * \par ʾ��
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

#define AW_LLSB(x)  ((x) & 0xff)            /**< \brief ȡ32bit������1���ֽ� */
#define AW_LNLSB(x) (((x) >> 8) & 0xff)     /**< \brief ȡ32bit������2���ֽ� */
#define AW_LNMSB(x) (((x) >> 16) & 0xff)    /**< \brief ȡ32bit������3���ֽ� */
#define AW_LMSB(x)  (((x) >> 24) & 0xff)    /**< \brief ȡ32bit������4���ֽ� */

/** @} grp_aw_if_byteops */

#endif /* __AW_BYTEOPS_H */

/* end of file */
