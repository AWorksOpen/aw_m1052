/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief LTR-553ALS-01 (����ǿ�ȼ������봫����) ͷ�ļ�
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-18  vir, first implementation
 * \endinternal
 */

#ifndef __AWBL_LTR553_H
#define __AWBL_LTR553_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "awbl_sensor.h"


#define AWBL_LTR553_NAME      "awbl_ltr553"

/*
 * \name ltr553_integration_time
 */
#define LTR553_INTEGRATION_TIME_100MS   0X00    /*< \brief 100ms */
#define LTR553_INTEGRATION_TIME_50MS    0X01    /*< \brief 50ms */
#define LTR553_INTEGRATION_TIME_200MS   0X02    /*< \brief 200ms */
#define LTR553_INTEGRATION_TIME_400MS   0X03    /*< \brief 400ms */
#define LTR553_INTEGRATION_TIME_150MS   0X04    /*< \brief 150ms */
#define LTR553_INTEGRATION_TIME_250MS   0X05    /*< \brief 250ms */
#define LTR553_INTEGRATION_TIME_300MS   0X06    /*< \brief 300ms */
#define LTR553_INTEGRATION_TIME_350MS   0X07    /*< \brief 350ms */

/*
 * \name ltr553_gain
 */
#define LTR553_GAIN_1X                  0X00    /*< \brief 1     ~ 64k lx*/
#define LTR553_GAIN_2X                  0X01    /*< \brief 0.5   ~ 32k lx*/
#define LTR553_GAIN_4X                  0X02    /*< \brief 0.25  ~ 16k lx*/
#define LTR553_GAIN_8X                  0X03    /*< \brief 0.125 ~ 8k  lx*/
#define LTR553_GAIN_48X                 0X06    /*< \brief 0.02  ~ 1.3klx*/
#define LTR553_GAIN_96X                 0X07    /*< \brief 0.01  ~ 600 lx*/

/**
 * \brief LTR553 �豸��Ϣ�ṹ��
 */
typedef struct awbl_ltr553_devinfo {
    /**
     *  \brief �ô��������նȲɼ���ͨ����ʾid
     *
     *  LTR553 ����2·���նȲɼ�ͨ������start_id��ʼ(����start_id)��
     *  ���� start_id ��Ӧģ���ͨ��1 ��  start_id + 1 ��Ӧģ��� ͨ��2.
     *  eg�� ��start_id ����Ϊ  0 , �� ID = 0 ��Ӧģ���ͨ��1�� ID = 1��
     *  ��Ӧģ���ͨ�� 2.
     */
    int32_t     start_id;

    /** \brief �������� */
    int32_t     alert_pin;

    /** \brief ȡ������ʱ��  */
    int32_t     als_integration_time;

    /** \brief I2C 7λ �豸��ַ���̶�Ϊ0x23(ռ��7λ) */
    uint8_t     i2c_addr;
    
    /** \brief ALS���� */
    uint8_t     als_gain;               
} awbl_ltr553_devinfo_t;


/**
 * \brief TPS02R �豸�ṹ��
 */
typedef struct awbl_ltr553_dev {
    /** \brief �̳��� AWBus �豸 */
    struct awbl_i2c_device          dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t           sensor_serv;

    /** \brief ALS���޴����ص�����*/
    aw_sensor_trigger_cb_t          pfn_als_trigger_cb;

    /** \brief ALS���޴����ص��������� */
    void                           *p_als_arg;

    /** \brief ALS���� */
    int32_t                         als_meas_time;

    /** \brief ALS�����ޱ���ֵ */
    int32_t                         als_thres_low;

    /** \brief ALS�����ޱ���ֵ */
    int32_t                         als_thres_up;

    /** \brief PS�����ޱ���ֵ */
    int32_t                         ps_thres_low;

    /** \brief ALS�����ޱ���ֵ */
    int32_t                         ps_thres_up;

    /** \brief PS���޴����ص�����*/
    aw_sensor_trigger_cb_t          pfn_ps_trigger_cb;

    /** \brief PS���޴����ص��������� */
    void                           *p_ps_arg;

    /** \brief ALS PSģʽ */
    uint8_t                         als_ps_mode;

    /** \brief ����ͨ�� */
    uint8_t                         alert_chan;
} awbl_ltr553_dev_t;

/*
 * \brief ע�� LTR553 ����
 */
void awbl_ltr553_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LTR553_H */

/* end of file */

