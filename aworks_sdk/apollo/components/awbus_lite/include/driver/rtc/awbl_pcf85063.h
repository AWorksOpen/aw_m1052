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
 * \brief AWBus-lite RTC PCF85063 ����
 *
 * PCF85063 �ǵ͹��ĵ� CMOS ʵʱʱ�ӣ�����оƬ�����ṩһ���ɱ��ʱ�������һ��
 * ƫ�ƼĴ�������΢��ʱ�ӣ����еĵ�ַ������ͨ�� I2C ���߽ӿڴ��д��ݡ��������
 * �ٶ�Ϊ400Kbits/s��ÿ�ζ�д���ݺ���Ƕ���ֵ�ַ�Ĵ������Զ�����������
 *
 * ע��:
 * - ʱ���趨��ΧΪ1970~2070��������Χ�᷵�ش���
 *
 * ����:
 * - ����32.768 kHz��ʯӢ�����ṩ�ꡢ�¡��ա��ܡ�ʱ���֡����ʱ��
 * - �͹�������������ֵΪ 0.25��A��VDD=3.0V��Tamb=25��ʱ����
 * - �ɱ��ʱ�����Ƶ��Ϊ��32.768KHz��16.384 kHz, 8.192 kHz, 4.096 kHz, 
 *                         2.048 kHz, 1.024 kHz, 1 Hz��
 * - �������ܡ�
 * - �����Լ�������жϡ�
 * - Ƭ���ϵ縴λ���ܡ�
 * - ���´�����������-40�� to +105��
 * - ������ѹ��Χ��0.9��5.5V
 * - 400KHz �� I2C ���߽ӿڣ�VDD=1.8��5.5V ʱ����
 * - ��ѡ�ڲ������������� 7pF �� 12.5pF��
 * - ��������ʱ����
 * - ����ֹͣ��⹦�ܡ�
 * - �ɱ�̵�ʱ��ƫ��������
 * - I2C���ߴӵ�ַ��0x50��
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "pcf85063"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_pcf85063_devinfo
 *
 * \par 2.�����豸
 *
 *  - PCF85063 �����豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_pcf85063_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_rtc
 *
 * \par 5.ʵ�����
 *
 *  - \ref grp_awbl_pcf85063_impl
 *
 * \internal
 * \par modification history:
 * - 1.00 16-04-26  sky, first implementation
 * \endinternal
 */

#ifndef __AWBL_PCF85063_H
#define __AWBL_PCF85063_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_pcf85063
 * \copydetails awbl_pcf85063.h
 * @{
 */

/**
 * \defgroup  grp_awbl_pcf85063_impl ʵ�����
 * \todo
 *
 */

/**
 * \defgroup  grp_awbl_pcf85063_hwconf �豸����/����
 * \todo
 */

/** @} grp_awbl_drv_pcf85063 */

#include "awbl_rtc.h"
#include "awbl_i2cbus.h"

/** \brief PCF85063 ������ */
#define AWBL_PCF85063_NAME   "pcf85063"

/**
 * \brief PCF85063 �豸��Ϣ
 */
typedef struct awbl_pcf85063_devinfo {

    /** \brief RTC ����������Ϣ */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief �豸�ӻ���ַ */
    uint8_t addr;
	
	/** \brief ʱ��ƫ�õ���ѡ��0����7pf; 1����12.5pf */
	uint8_t cap_set;

} awbl_pcf85063_devinfo_t;

/**
 * \brief PCF85063 �豸ʵ��
 */
typedef struct awbl_pcf85063_dev {

    /** \brief �̳��� AWBus I2C device �豸 */
    struct awbl_i2c_device super;

    /** \brief RTC ���� */
    struct awbl_rtc_service rtc_serv;

    /** \brief �豸�� */
    AW_MUTEX_DECL(devlock);
    
} awbl_pcf85063_dev_t;

/**
 * \brief PCF85063 driver register
 */
void awbl_pcf85063_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_PCF85063_H */

/* end of file */
