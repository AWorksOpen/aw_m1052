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
 * \file   aw_qrcode_decode.h
 * \brief  qrcode
 *
 * \internal
 * \par modification history:
 * - 1.00 15-11-10  dcf, first implementation.
 * \endinternal
 */
 
#ifndef __AW_QRCODE_DECODE_H__
#define __AW_QRCODE_DECODE_H__
/**
* \brief  QR����뺯��
*
* \param[in]    data            ͼƬ����
* \param[in]    width           ͼƬ�Ŀ�
* \param[in]    height          ͼƬ�ĸ�
* \param[out]   msg             ������Ϣ
* \return                       ���ؽ����ʶ��:-4~0.
* \note:
* 1.�����ͼƬ����dataΪ��ͨ����ʽ���ݣ��ڽ�������У�������ȡ��һ��
*   ͨ�������ݽ��н��룬����decode_data[k++] = data[i+=4]��
* 2.�����ʶ������Ϣ�ֱ��ʾ��
*     0�������ɹ���
*    -1�������ʧ�ܣ�δ��⵽��ά�룻
*    -2�������ʧ�ܣ�ͼƬ�ĳ�����Ҫ����0��
*    -3�������ʧ�ܣ��������
*    -4�������ʧ�ܣ��������
*/
#ifdef __cplusplus
extern "C" {
#endif

//int aw_qrcode_decode(const unsigned char *data, int width, int height, std::string *msg);
int aw_qrcode_decode(const unsigned char *data, int width, int height, char *p_buf, int size);

#ifdef __cplusplus
}
#endif
#endif
