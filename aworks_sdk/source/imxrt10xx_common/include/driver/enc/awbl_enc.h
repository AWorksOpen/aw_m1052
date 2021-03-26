/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2014 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx Quadrature Decoderͨ�ýӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
 * \code
 * #include awbl_enc.h
 * \endcode
 *
 * \par ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 18-03-21 mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_ENC_H
#define __AWBL_ENC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"


/**
 * \brief ��ʼ��������
 *
 * \param[in] chan   ͨ��
 * \param[in] p_func �жϻص�����
 * \param[in] p_arg  �жϻص������Ĳ���
 *
 * \retval   AW_OK      ��ʼ���ɹ�
 * \retval   AW_ERROR   ��ʼ��ʧ��
 * \retval   -AW_EINVAL ��������
 */
aw_err_t  awbl_enc_init(uint32_t         chan,
                        aw_pfuncvoid_t   p_func,
                        void            *p_arg);



/**
 * \brief ��ñ�������λ��
 *
 * \param[in] chan  ͨ��
 * \param[in] p_val ���ݶ�����
 *
 * \retval   AW_OK      ��ȡ�ɹ�
 * \retval   AW_ERROR   ��ȡʧ��
 * \retval   -AW_EINVAL ��������
 */
aw_err_t  awbl_enc_get_position(uint32_t   chan,
                                int       *p_val);



/**
 * \brief ��ñ�������λ�ò�ֵ(����λ�ú��ϴ�λ�õĲ�ֵ)
 *
 * \param[in] chan  ͨ��
 * \param[in] p_val ���ݶ�����
 *
 * \retval   AW_OK      ��ȡ�ɹ�
 * \retval   AW_ERROR   ��ȡʧ��
 * \retval   -AW_EINVAL ��������
 */
aw_err_t  awbl_enc_get_position_difference(uint32_t   chan,
                                           int16_t   *p_val);



/**
 * \brief ��ñ���������תȦ��
 *
 * \param[in] chan  ͨ��
 * \param[in] p_val ���ݶ�����
 *
 * \retval   AW_OK      ��ȡ�ɹ�
 * \retval   AW_ERROR   ��ȡʧ��
 * \retval   -AW_EINVAL ��������
 */
aw_err_t  awbl_enc_get_revolution(uint32_t   chan,
                                  int16_t   *p_val);



#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_ENC_H */

/* end of file */
