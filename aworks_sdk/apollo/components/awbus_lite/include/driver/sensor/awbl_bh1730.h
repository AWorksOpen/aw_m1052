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
 * \brief BH1730 (����ǿ�ȼ������봫����) ͷ�ļ�
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-18  vir, first implementation
 * \endinternal
 */

#ifndef __AWBL_BH1730_H
#define __AWBL_BH1730_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "awbl_sensor.h"


#define AWBL_BH1730_NAME      "awbl_bh1730"

/*
 * \name bh1730 gain
 */
#define BH1730_GAIN_1X        0X00    /**< \brief 100k lx */
#define BH1730_GAIN_2X        0X01    /**< \brief 50k lx */
#define BH1730_GAIN_64X       0X02    /**< \brief 1.5k lx */
#define BH1730_GAIN_128X      0X03    /**< \brief 780 lx */
/**
 * \brief BH1730 �豸��Ϣ�ṹ��
 */
typedef struct awbl_bh1730_devinfo {
    /**
     *  \brief �ô��������նȲɼ���ͨ����ʾid
     *
     *  BH1730 ����2·���նȲɼ�ͨ������start_id��ʼ(����start_id)��
     *  ���� start_id ��Ӧģ���ͨ��1 ��start_id + 1 ��Ӧģ���ͨ��2.
     *  eg�� ��start_id ����Ϊ  0 , �� ID = 0 ��Ӧģ���ͨ��1�� ID = 1��
     *  ��Ӧģ���ͨ�� 2.
     */
    int     start_id;

    /** \brief �������� */
    int     alert_pin;     

    /*
     * \brief I2C 7λ �豸��ַ���̶�Ϊ0x23(ռ��7λ)
     */
    uint8_t i2c_addr;

    /** \brief ���棬Ӱ����նȵļ���ֵ */
    uint8_t                         gain;
} awbl_bh1730_devinfo_t;


/**
 * \brief TPS02R �豸�ṹ��
 */
typedef struct awbl_bh1730_dev {
    /** \brief �̳��� AWBus �豸 */
    struct awbl_i2c_device          dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t           sensor_serv;

    struct aw_isr_defer_job         defer_job;

    /** \brief ���޴����ص�����*/
    aw_sensor_trigger_cb_t          pfn_trigger_cb;

    /** \brief ���޴����ص��������� */
    void                           *p_arg;

    /** \brief ���޴�������ֵ */
    aw_sensor_val_t                 thres_low;

    /** \brief ���޴�������ֵ */
    aw_sensor_val_t                 thres_up;

    /** \brief ����ʱ�䣬Ӱ����նȵļ���ֵ */
    int32_t                         itime_ms;

    /** \brief ��ǰģʽ  */
    uint8_t                         mode;
} awbl_bh1730_dev_t;

/*
 * \brief ע�� BH1730 ����
 */
void awbl_bh1730_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_BH1730_H */

/* end of file */


