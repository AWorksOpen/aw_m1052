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
 * \brief Buzzer driver.
 *
 * \internal
 * \par modification history
 * - 1.01 14-11-01  ops, redefine the abstract buzzer by using new specification.
 * - 1.00 13-03-08  orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_BUZZER_H
#define __AWBL_BUZZER_H

#include "aw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup awbl_buzzer
 * @{
 */

/** \brief buzzer device */
struct awbl_buzzer_dev {

    /**
     * \brief �򿪻�رշ���������
     *
     * \param[in] on �� ΪTRUEʱ���򿪷�������ΪFALSEʱ���رշ�����
     * 
     * \return ��
     */
    void (*beep) (struct awbl_buzzer_dev *p_dev, aw_bool_t on);

    /**
     * \brief ������ú���
     *
     * ����֧�֣����Խ���ֵ����ΪNULL
     *
     * \param[in] p_dev : �������豸
     * \param[in] level : ��ȣ�0 ~ 100�����Ϊ0ʱ��ʾ������100ʱ������
     *
     * \retval AW_OK        ���óɹ�
     * \retval -AW_ENOTSUP  �豸��֧�ָò���
     */
    aw_err_t (*loud_set) (struct awbl_buzzer_dev *p_dev, int level);
};

/**
 * \brief ע��������豸
 *
 * \param[in] p_dev : �������豸
 *
 * \return ��
 *
 * \note һ��ϵͳ��ֻ��ע��һ��������
 */
void awbl_buzzer_register_device (struct awbl_buzzer_dev *p_dev);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_BUZZER_H */

/* end of file */
