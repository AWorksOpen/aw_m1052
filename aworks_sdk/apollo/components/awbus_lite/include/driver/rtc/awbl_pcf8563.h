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
 * \brief AWBus-lite RTC PCF8563 ����
 *
 * PCF8563 �ǵ͹��ĵ� CMOS ʵʱʱ�ӣ�����оƬ�����ṩһ���ɱ��ʱ�������һ��
 * �ж�����͵������������еĵ�ַ������ͨ�� I 2 C ���߽ӿڴ��д��ݡ�
 * ��������ٶ�Ϊ400Kbits/s��ÿ�ζ�д���ݺ���Ƕ���ֵ�ַ�Ĵ������Զ�����������
 *
 * ����:
 * - �͹�������������ֵΪ 0.25��A��VDD=3.0V��Tamb=25��ʱ����
 * - ���ͱ�־
 * - ������ѹ��Χ��1.0��5.5
 * - �����ߵ���������ֵΪ 0.25��A(V DD =3.0V,T amb =25��)
 * - 400KHz �� I 2 C  ���߽ӿڣ�VDD=1.8��5.5V  ʱ����
 * - �ɱ��ʱ�����Ƶ��Ϊ��32.768KHz��1024Hz��32Hz��1Hz��
 * - �����Ͷ�ʱ����
 * - ����������
 * - �ڲ����ɵ��������ݡ�
 * - Ƭ�ڵ�Դ��λ���ܡ�
 * - I2C���ߴӵ�ַ��0x50
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "pcf8563"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_pcf8563_devinfo
 *
 * \par 2.�����豸
 *
 *  - PCF8563 �����豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_pcf8563_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_rtc
 *
 * \par 5.ʵ�����
 *
 *  - \ref grp_awbl_pcf8563_impl
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-05  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_PCF8563_H
#define __AWBL_PCF8563_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_pcf8563
 * \copydetails awbl_pcf8563.h
 * @{
 */

/**
 * \defgroup  grp_awbl_pcf8563_impl ʵ�����
 * \todo
 *
 */

/**
 * \defgroup  grp_awbl_pcf8563_hwconf �豸����/����
 * \todo
 */

/** @} grp_awbl_drv_pcf8563 */

#include "awbl_rtc.h"
#include "awbl_i2cbus.h"


/** \brief PCF8563 ������ */
#define AWBL_PCF8563_NAME   "pcf8563"

/**
 * \brief PCF8563 �豸��Ϣ
 */
typedef struct awbl_pcf8563_devinfo {

    /** \brief RTC ����������Ϣ */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief �豸�ӻ���ַ */
    uint8_t addr;

} awbl_pcf8563_devinfo_t;

/**
 * \brief PCF8563 �豸ʵ��
 */
typedef struct awbl_pcf8563_dev {

    /** \brief �̳��� AWBus I2C device �豸 */
    struct awbl_i2c_device super;

    /** \brief RTC ���� */
    struct awbl_rtc_service rtc_serv;

    /** \brief �豸�� */
    AW_MUTEX_DECL(devlock);
} awbl_pcf8563_dev_t;

/**
 * \brief PCF8563 driver register
 */
void awbl_pcf8563_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_PCF8563_H */

/* end of file */

