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
 * \brief HTS221 (����ʪ��ģ��) ͷ�ļ�
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-12  wan, first implementation
 * \endinternal
 */

#ifndef __AWBL_HTS221_H
#define __AWBL_HTS221_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "awbl_sensor.h"
#include "aw_isr_defer.h"

#define AWBL_HTS221_NAME      "awbl_hts221"

/**
 * \brief HTS221 �豸��Ϣ�ṹ��
 */
typedef struct awbl_hts221_devinfo {
    /**
     * \brief �ô�����ͨ������ʼid
     *
     * HTS221 ����1·ʪ�Ȳɼ�ͨ����1·�¶Ȳɼ�ͨ��,��start_id��ʼ(����start_id)
     * ��������ID�������ģ�������ͨ�������� start_id ��Ӧģ���ʪ�ȴ�����ͨ����
     * start_id + 1 ��Ӧģ����¶ȴ�����ͨ��
     * eg����start_id ����Ϊ0, ��ID = 0,��Ӧģ���ͨ��1��ID = 1��Ӧģ���ͨ��2.
     */
    int     start_id;

    /**
     * \brief ����׼��������������
     *
     * ��������AWorks�ӿڵ�AW-Sensor�����Ѿ���Ϊ GPIO1_27,��ֱ�Ӵ�������ż���
     */
    int     trigger_pin;

    /*
     * \brief I2C 7λ �豸��ַ
     *
     * ���豸7λ��ַ�̶�Ϊ 0x5f
     */
    uint8_t i2c_addr;

    /** \brief ƽ̨��ʼ���ص��������ص�ע�ᴥ�������ѱ�ϵͳʹ�ã� */
    void (*pfunc_plfm_init)(void);

} awbl_hts221_devinfo_t;

/**
 * \brief HTS221 ʪ�Ⱥ��¶�У׼ֵ����
 */
typedef struct awbl_hts221_calibration_data {
    int16_t   x0;
    int8_t    y0;
    int16_t   x1;
    int8_t    y1;
} awbl_hts221_calibration_data_t;

/**
 * \brief HTS221 �豸�ṹ��
 */
typedef struct awbl_hts221_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev                      dev;

    /** \brief I2C �ӻ��豸 */
    aw_i2c_device_t                      i2c_dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t                sensor_serv;

    /** \brief �ֱ𱣴�ʪ�Ⱥ��¶ȵ�У׼ֵ */
    awbl_hts221_calibration_data_t       cal_val[2];

    /** \brief ����׼�����������ص����� */
    aw_sensor_trigger_cb_t               pfn_trigger_cb[2];

    /** \brief ����׼�����������ص��������� */
    void                                *p_arg[2];

    /** \brief ������־λ */
    uint8_t                              flags[2];

    /** \brief �ж���ʱ�������� */
    struct aw_isr_defer_job              g_myjob;

    /** \brief ����Ƶ�ʣ�ÿ������Ĵ��� */
    aw_sensor_val_t                      sampling_rate;

    /** \brief ��¼��־λ */
    uint8_t                              trigger;

    /** \brief ����׼��������ֵ���� */
    aw_sensor_val_t                      current_data[2];

} awbl_hts221_dev_t;

/*
 * \brief ע�� HTS221 ����
 */
void awbl_hts221_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_HTS221_H */

/* end of file */
