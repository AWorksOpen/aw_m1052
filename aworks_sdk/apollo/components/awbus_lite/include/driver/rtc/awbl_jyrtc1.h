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
 * \brief AWBus-lite RTC JYRTC1 ����
 *
 * JYRTC-1 ������ 32.768kHZ �����¶Ȳ�������������DTCXO������ I2C ���߽ӿ�Ϊ
 * ͨ�ŷ�ʽ�ĸ߾���ʵʱʱ��оƬ�� 
 * ����ʱ�Ӻ��Զ�����У���������ܣ����б������̶����ڶ�ʱ�жϡ�ʱ���
 * ���жϺ�ʹ�� OE �� 37.768kHz Ƶ������ȶ��ֹ��ܡ� 
 *
 * ����:
 * - ���ø��ȶ��ȵ� 32.768kHz �� DTCXO�������¶Ȳ������������� 
 * - ֧�� I2C���߸���ģʽ��400K���� 
 * - ��ʱ�������ܣ����趨���죬���ڣ�Сʱ�����ӣ� 
 * - �̶����ڶ�ʱ�жϹ��ܡ� 
 * - ʱ������жϹ��� 
 * - 32.768kHz Ƶ�����������ʹ�� OE ���ܣ� 
 * - �����Զ��������ܡ���2000 �� 2099�� 
 * - ��Χ�ӿڵ�ѹ��2.2V �� 5.5V 
 * - ��Χʱ�䱣�ֵ�ѹ��1.8V �� 5.5V 
 * - �͵������ģ�<1.8uA/3V��Type�� 
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-22  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_JYRTC1_H
#define __AWBL_JYRTC1_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#include "awbl_rtc.h"
#include "awbl_i2cbus.h"

/** \brief JYRTC1 ������ */
#define AWBL_JYRTC1_NAME   "jyrtc1"

/**
 * \brief JYRTC1 �豸��Ϣ
 */
typedef struct awbl_jyrtc1_devinfo {

    /** \brief RTC ����������Ϣ */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief �豸�ӻ���ַ */
    uint8_t addr;

} awbl_jyrtc1_devinfo_t;

/**
 * \brief JYRTC1 �豸ʵ��
 */
typedef struct awbl_jyrtc1_dev {

    /** \brief �̳��� AWBus I2C device �豸 */
    struct awbl_i2c_device super;

    /** \brief RTC ���� */
    struct awbl_rtc_service rtc_serv;

    /** \brief �豸�� */
    AW_MUTEX_DECL(devlock);
} awbl_jyrtc1_dev_t;

/**
 * \brief JYRTC1 driver register
 */
void awbl_jyrtc1_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_JYRTC1_H */

/* end of file */

