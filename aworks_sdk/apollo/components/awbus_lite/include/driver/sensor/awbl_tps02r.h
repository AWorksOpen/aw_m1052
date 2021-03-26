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
 * \brief TPS02R (����ģ��) ͷ�ļ�
 *
 * \internal
 * \par Modification History
 * - 1.00 18-05-11  vir, first implementation
 * \endinternal
 */

#ifndef __AWBL_TPS02R_H
#define __AWBL_TPS02R_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "awbl_sensor.h"


#define AWBL_TPS02R_NAME      "awbl_tps02r"

/*
 * \name grp_tps02r_chan
 */
#define AW_TPS02R_CHAN_1      0    /**< \brief TPS02R ��ͨ��1 */
#define AW_TPS02R_CHAN_2      1    /**< \brief TPS02R ��ͨ��2 */

/**
 * \brief TPS02R �豸��Ϣ�ṹ��
 */
typedef struct awbl_tps02r_devinfo {
    /**
     *  \brief �ô�����ͨ������ʼid
     *
     *  TPS02R ����2·�¶Ȳɼ�ͨ������start_id��ʼ(����start_id)��������ID�����
     *  ��ģ�������ͨ�������� start_id ��Ӧģ���ͨ��1 ��  start_id + 1 ��Ӧ
     *  ģ��� ͨ�� 2
     *  eg�� ��start_id ����Ϊ  0 , �� ID = 0 ��Ӧģ���ͨ��1�� ID = 1����Ӧģ��
     *  ��ͨ�� 2.
     */
    int     start_id;

    int     alert_pin;     /**< \brief �������� */

    /*
     * \brief I2C 7λ �豸��ַ
     *
     *  A0���� �ӵ�(0x48)���� VDD ��������(0x49)
     */
    uint8_t i2c_addr;

}awbl_tps02r_devinfo_t;


/**
 * \brief TPS02R �豸�ṹ��
 */
typedef struct awbl_tps02r_dev {
    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev                      dev;

    /** \brief I2C �ӻ��豸 */
    aw_i2c_device_t                      i2c_dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t                sensor_serv;

    /** \brief ���޴����ص� ����*/
    aw_sensor_trigger_cb_t               pfn_trigger_cb;

    /** \brief ���޴����ص��������� */
    void                                 *p_arg;

    /** \brief �����¶� */
    aw_sensor_val_t                     temp_low[2];

    /** \brief �����¶� */
    aw_sensor_val_t                     temp_high[2];

    /** \brief ������������ */
    uint8_t                             count[2];

    /** \brief ����ͨ�� */
    uint8_t                              alert_chan;

    /** \brief ����Ƶ�ʣ�ÿ������Ĵ��� */
    uint8_t                              sampling_rate;
    ;

}awbl_tps02r_dev_t;

/*
 * \brief ע�� TPS02R ����
 */
void awbl_tps02r_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_TPS02R_H */

/* end of file */
