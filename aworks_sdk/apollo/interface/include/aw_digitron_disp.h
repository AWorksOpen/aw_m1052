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
 * \brief ֧�ֶ�ʵ���Ķ�ʽ����������ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_digitron_disp.h
 *
 * \par ʹ��ʾ��
 * \code
 * #include "aw_digitron_disp.h"
 *
 * // ��ʼ��������8��ASCII����
 * aw_digitron_disp_set_decode(0, aw_digitron_seg8_ascii_decode);
 *
 * aw_digitron_disp_set_blink(0, 0, TURE); // ���õ�0���������˸
 * aw_digitron_disp_at(0, 0, 0x54);        // ��0���������ʾ�Զ�����״0x54
 * aw_digitron_disp_char_at(0, 1, 'A');    // ��2���������ʾ�ַ�'A'
 * aw_digitron_disp_clr();                 // ��������������ʾ��������
 * aw_digitron_disp_str(0, 0, 4, "0123");  // �������ʾ�ַ���"0123"
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.10 17-09-07  tee, Change the return type from void to aw_err_t
 * - 1.03 15-03-17  ops, redefine the aw_digitron_disp_str.
 * - 1.02 14-11-24  ops, refactor by using list for abstract digitron device management.
 * - 1.01 14-11-06  ops, redefine the interface by using the concept of digitron matrix.
 * - 1.00 13-02-25  orz, first implementation.
 * \endinternal
 */

#ifndef __AW_DIGITRON_DISP_H
#define __AW_DIGITRON_DISP_H

#include "aw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_digitron_disp
 * \copydoc aw_digitron_disp.h
 * @{
 */

/**
 * \brief ����ASCII�ַ�Ϊ8������ܵĶ������
 *
 * \param ascii_char Ҫ�����ASCII�ַ�
 *
 * \return ASCII�ַ���Ӧ���������������, ����޷����룬���ؿհ��롣
 *
 * \note
 * �������һ����Ϊ aw_digitron_disp_set_decode() �Ĳ����� ֧�ֵ�ASCII�ַ��У�
 *  |  A  |  S  |  C  |  I  |  I  |
 *  |-----|-----|-----|-----|-----|
 *  | '0' | '1' | '2' | '3' | '4' |
 *  | '5' | '6' | '7' | '8' | '9' |
 *  | 'A' | 'B' | 'C' | 'D' | 'E' |
 *  | 'F' | 'H' | 'I' | 'J' | 'L' |
 *  | 'N' | 'O' | 'P' | 'R' | 'S' |
 *  | 'a' | 'b' | 'c' | 'd' | 'e' |
 *  | 'f' | 'h' | 'i' | 'j' | 'l' |
 *  | 'n' | 'o' | 'p' | 'r' | 's' |
 *  | '-' | '.' | ' ' |  -  |  -  |
 */
uint16_t aw_digitron_seg8_ascii_decode (uint16_t ascii_char);

/**
 * \brief �����������ʾ�����Ķ�������
 *
 * \param id           ��ʾ�����
 *
 * \param pfn_decode   �Զ�����뺯��
 */
aw_err_t aw_digitron_disp_decode_set (int id, uint16_t (*pfn_decode)(uint16_t code));

/**
 * \brief �����������ʾ��˸����
 *
 * \param id     ��ʾ�����
 * \param index  �����������
 * \param blink  ��˸���ԣ�TURE����˸��FALSE����ֹ��˸��
 */
aw_err_t aw_digitron_disp_blink_set (int id, int index, aw_bool_t blink);

/**
 * \brief ֱ���������д��������ʾ���ݡ�������һ��������ʾ�Զ����������Ż���״
 *
 * \param id     ��ʾ�����
 * \param index  ������Դ�����.
 * \param seg    д�뵽�Դ������(����ܵġ��Ρ��룬�ߵ�ƽ��Ч).
 */
aw_err_t aw_digitron_disp_at (int id, int index, uint16_t seg);

/**
 * \brief ��ָ����ŵ����������ʾASCII�ַ�
 *
 * \param id     ��ʾ�����
 * \param index  �����������
 * \param ch     Ҫ��ʾ��ASCII�ַ�
 *
 * \note ����ʾ���ַ��ɽ��뺯��ȷ��
 */
aw_err_t aw_digitron_disp_char_at (int id, int index, const char ch);

/**
 * \brief �����������ʾASCII�ַ���
 *
 * \param id    ��ʾ�����
 * \param index ��ʼ�����������
 * \param len   ��ʾ����
 * \param p_str Ҫ��ʾ��ASCII�ַ���
 *
 * \note ����ʾ���ַ��ɽ��뺯��ȷ��
 */
aw_err_t aw_digitron_disp_str (int id, int index, int len, const char *p_str);


/**
 * \brief ��������������ʾ
 *
 * \param id    ��ʾ�����
 */
aw_err_t aw_digitron_disp_clr (int id);

/**
 * \brief ʹ���������ʾ������ɨ�裨Ĭ��״̬�ǿ����ģ�
 *
 * \param id    ��ʾ�����
 */
aw_err_t aw_digitron_disp_enable (int id);

/**
 * \brief �����������ʾ,ֹͣɨ��
 *
 * \param id   ��ʾ�����
 */
aw_err_t aw_digitron_disp_disable (int id);

/** @} grp_aw_if_digitron_disp */

#ifdef __cplusplus
}
#endif

#endif /* __AW_DIGITRON_DISP_H */

/* end of file */
