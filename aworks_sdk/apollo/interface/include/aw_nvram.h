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
 * \brief ����ʧ�Դ洢���ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ����ͷ�ļ� aw_nvram.h
 *
 * \par ��ʾ��
 * \code
 *  #include "aw_nvram.h"
 *
 *  char ip[4] = {1, 1, 1, 1};
 *  char data[4];
 *
 *  aw_nvram_set("ip", 0, &ip[0], 0, 4);    // ���÷���ʧ������"ip"
 *  aw_nvram_get("ip", 0, &data[0], 0, 4);  // ��ȡ����ʧ������"ip"
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-30  zen, first implementation
 * \endinternal
 */

#ifndef __AW_NVRAM_H
#define __AW_NVRAM_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_nvram
 * \copydoc aw_nvram.h
 * @{
 */

/**
 * \brief ��ȡһ������ʧ�Դ洢��Ϣ��
 *
 * �������ӷ���ʧ�Դ洢�豸�ж�ȡ����ʧ����Ϣ�����浽�������ṩ�Ļ������С��κ�
 * ��Ҫʹ��NVRAM�����ģ�鶼����ʹ�ñ�������
 *
 * ������ʹ��\a p_name ��\a unit ����ʶһ������ʧ����Ϣ��
 *
 * ��������ʼλ����\a offset ָ������������������\a len ָ�������ָ���Ĵ�С����
 * �˸÷���ʧ����Ϣ�Ĵ�С��������Ϊδ���塣
 *
 * \param[in]  p_name   ����ʧ�Դ洢��Ϣ������
 * \param[in]  unit     ����ʧ�Դ洢��Ϣ�ĵ�Ԫ�� 
 *                      (��ͬ���ֵķ���ʧ����Ϣ�õ�Ԫ������)
 * \param[out] p_buf    ���ݷ��صĻ�����
 * \param[in]  offset   �ڷ���ʧ�Դ洢��Ϣ�е�ƫ��
 * \param[in]  len      Ҫ��ȡ�ĳ���
 *
 * \retval  AW_OK     : �ɹ�
 * \retval -AW_EFAULT : \a p_name �� \a p_buf ΪNULL
 * \retval -AW_EXIO   : ָ���ķ���ʧ�Դ洢��Ϣ�����ڻ����ʧ��
 *
 * \par ʾ��
 * \code
 *  #include "aw_nvram.h"
 *
 *  char ip[4];
 *  aw_nvram_get("ip", 0, &ip[0], 0, 4);    // ��ȡ����ʧ������"ip"
 * \endcode
 *
 * \sa aw_nvram_set()
 */
aw_err_t aw_nvram_get (char *p_name, int unit, char *p_buf, int offset, int len);

/**
 * \brief д��һ������ʧ�Դ洢��Ϣ��
 *
 * �������ӵ������ṩ�Ļ������ж�ȡ��Ϣ��������洢������ʧ�Դ洢�豸�С��κ�
 * ��Ҫʹ��NVRAM�����ģ�鶼����ʹ�ñ�������
 *
 * ������ʹ��\a p_name ��\a unit ����ʶһ������ʧ����Ϣ��
 *
 * �洢����ʼλ����\a offset ָ�����洢����������\a len ָ�������ָ���Ĵ�С����
 * �˸÷���ʧ����Ϣ�Ĵ�С��������Ϊδ���塣
 *
 * \param[in] p_name   ����ʧ�Դ洢��Ϣ������
 * \param[in] unit     ����ʧ�Դ洢��Ϣ�ĵ�Ԫ�� 
 *                     (��ͬ���ֵķ���ʧ����Ϣ�õ�Ԫ������)
 * \param[in] p_buf    д�����ݻ�����
 * \param[in] offset   �ڷ���ʧ�Դ洢��Ϣ�е�ƫ��
 * \param[in] len      Ҫ�洢�ĳ���
 *
 * \retval  AW_OK     : �ɹ�
 * \retval -AW_EFAULT : \a p_name �� \a p_buf ΪNULL
 * \retval -AW_EXIO   : ָ���ķ���ʧ�Դ洢��Ϣ�����ڻ����ʧ��
 *
 * \par ʾ��
 * \code
 *  #include "aw_nvram.h"
 *
 *  char ip[4] = {1, 1, 1, 1};
 *  aw_nvram_set("ip", 0, &ip[0], 0, 4);    // ���÷���ʧ������"ip"
 * \endcode
 *
 * \sa aw_nvram_get()
 */
aw_err_t aw_nvram_set (char *p_name, int unit, char *p_buf, int offset, int len);


/** @} grp_aw_if_nvram */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_NVRAM_H */

/* end of file */
