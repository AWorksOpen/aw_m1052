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
 * \brief LED�豸�����ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_led.h
 *
 * \par ʹ��ʾ��
 * \code
 * #include "aw_led.h"
 *
 * aw_led_set(0, true);     // ����LED0
 * aw_led_set(0, false);    // Ϩ��LED0
 *
 * aw_led_on(1);            // ����LED1
 * aw_led_off(1);           // Ϩ��LED1
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.01 14-11-25  ops, redefine the led framework by service.
 * - 1.00 13-03-11  orz, first implementation
 * \endinternal
 */

#ifndef __AW_LED_H
#define __AW_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_led
 * \copydoc aw_led.h
 * @{
 */

#include "aw_types.h"

/**
 * \brief ����LED״̬
 * \param[in] id   LED���, ���ñ���ɾ���ƽ̨����
 * \param[in] on   LED�ĵ���״̬, trueΪ����, falseΪϨ��
 *
 * \retval  AW_OK      �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_led_set (int id, aw_bool_t on);

/**
 * \brief ����LED
 * \param[in] id   LED���, ���ñ���ɾ���ƽ̨����.
 *
 * \retval  AW_OK      �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_led_on (int id);

/**
 * \brief Ϩ��LED
 * \param[in] id   LED���, ���ñ���ɾ���ƽ̨����
 *
 * \retval  AW_OK      �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_led_off (int id);

/**
 * \brief ��תLED
 * \param[in] id   LED���, ���ñ���ɾ���ƽ̨����
 *
 * \retval  AW_OK      �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_led_toggle (int id);

/** @} grp_aw_if_led */

#ifdef __cplusplus
}
#endif

#endif /* __AW_LED_H */

/* end of file */
