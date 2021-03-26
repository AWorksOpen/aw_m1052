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
 * \brief iMX RT10xx Analog Comparator ��׼�ӿ�
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
 * - 1.00 18-04-12 mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_ACMP_H
#define __AWBL_ACMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"


/**< \brief  VREF����ͨ��1 */
#define VREF_SOURCE_VIN1   1

/**< \brief  VREF����ͨ��2 */
#define VREF_SOURCE_VIN2   2

/**
 * \brief ACMP�ж�״̬��־
 */
typedef enum {
    kCMP_OutputRisingEventFlag = 0,   /**< \brief Rising-edge on the comparison output has occurred. */
    kCMP_OutputFallingEventFlag = 1,  /**< \brief Falling-edge on the comparison output has occurred. */
} cmp_status_flags_t;


/**
 * \brief ��ʼ��ACMP,��ע���жϷ�����
 *
 * \param[in] acmp_id  ACMPģ��ID (1 - 4)
 * \param[in] p_func   �жϻص�����
 *
 * \retval   AW_OK      ��ʼ���ɹ�
 * \retval   AW_ERROR   ��ʼ��ʧ��
 * \retval   -AW_EINVAL ��������
 */
aw_err_t  awbl_acmp_init(uint8_t          acmp_id,
                         aw_pfuncvoid_t   p_func);

/**
 * \brief ����DAC�����òο���ѹ
 *
 * \param[in] acmp_id  ACMPģ��ID (1 - 4)
 * \param[in] vin      �ο���ѹ������ͨ��
 * \param[in] vin_mv   �ο���ѹ������ֵ(mv)
 * \param[in] vref_mv  �ο���ѹ�����ֵ(��Χ��vin_mv/64 ~ vin_mv)��
 *                     �òο���ѹ�ɽ���Ƚ���
 *
 * \retval   AW_OK      �ɹ�
 * \retval   AW_ERROR   ʧ��
 * \retval   -AW_EINVAL ��������
 */
aw_err_t awbl_acmp_config_dac (uint8_t     acmp_id,
                               uint8_t     vin,
                               uint16_t    vin_mv,
                               uint16_t    vref_mv);

/**
 * \brief �Ƚ���������ͨ��ѡ��
 *
 * \param[in] acmp_id         ACMPģ��ID (1 - 4)
 * \param[in] positive_chan   ͬ�����������ͨ��
 * \param[in] negative_chan   �������������ͨ��
 *
 * \retval   AW_OK      �ɹ�
 * \retval   AW_ERROR   ʧ��
 * \retval   -AW_EINVAL ��������
 */
aw_err_t awbl_acmp_set_input_chan(uint8_t   acmp_id,
                                  uint32_t  positive_chan,
                                  uint16_t  negative_chan);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_ACMP_H */

/* end of file */
