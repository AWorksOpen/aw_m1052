/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief ���������Ǵ����� BMG160 ͷ�ļ�
 *
 * \internal
 * \par modification history
 * - 1.00 18-12-11  ipk, first implementation.
 * \endinternal
 */
 
#ifndef __AW_SENSOR_BMG160_H
#define __AW_SENSOR_BMG160_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_i2c.h"
#include "awbl_sensor.h"
#include "aw_isr_defer.h"
    

#define AWBL_BMG160_NAME      "awbl_bmg160"

/**
 * \brief ������ BMG160 �豸��Ϣ�ṹ��
 */
typedef struct awbl_bmg160_devinfo {
    /**
     *  \brief �ô��������նȲɼ���ͨ����ʾid
     *
     *  BH1730 ����3·���ٶȲɼ�ͨ������start_id��ʼ(����start_id)��
     *  ���� start_id ��Ӧģ���ͨ��1.
     *  eg�� ��start_id ����Ϊ  0 , �� ID = 0 ��Ӧģ���ͨ��1.
     *
     *  ÿ��ͨ����ȡ�������ݵ�λ��: ��/S
     */
    int     start_id;

    /**
     * \brief ����׼�������������� INT1
     */
    int     trigger_pin;

    /*
     * \brief I2C 7λ �豸��ַ
     *
     * ���豸7λ��ַ�̶�Ϊ 0x68
     */
    uint8_t i2c_addr;

} awbl_bmg160_devinfo_t;

/****************************** У׼ֵ������************************************/

/**
 * \breif ������ BMG160 �豸�ṹ�嶨��
 */
typedef struct awbl_bmg160_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev                bmg160_dev;

    /** \brief I2C�ӻ��豸  */
    aw_i2c_device_t                i2c_dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t          sensor_serv;

    void                          *p_arg[3];     /**< \brief �����ص��������� */
    uint8_t                        flags[3];     /**< \brief ������־λ       */
    struct aw_isr_defer_job        g_myjob;      /**< \brief �ж���ʱ�������� */
    aw_sensor_val_t                range;        /**< \brief ����         */
    uint8_t                        trigger;      /**< \brief ��¼��־λ       */
    aw_sensor_val_t                data[3];      /**< \brief �ڲ����ݻ���     */
    /** \brief ����׼�����������ص����� */
    aw_sensor_trigger_cb_t         pfn_trigger_cb[3];

} awbl_bmg160_dev_t;

/*
 * \brief ע�� BMG160 ����
 */
void awbl_bmg160_drv_register (void);


#ifdef __cplusplus
}
#endif

#endif /* __AW_SENSOR_BMG160_H */

/* end of file */
