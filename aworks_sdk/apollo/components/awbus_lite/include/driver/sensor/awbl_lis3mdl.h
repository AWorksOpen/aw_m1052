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
 * \brief LIS3MDL (����Ŵ�����) ͷ�ļ�
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-25  wan, first implementation
 * \endinternal
 */

#ifndef __AWBL_LIS3MDL_H
#define __AWBL_LIS3MDL_H

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

#define AWBL_LIS3MDL_NAME      "awbl_lis3mdl"

/**
 * \brief LIS3MDL �豸��Ϣ�ṹ��
 */
typedef struct awbl_lis3mdl_devinfo {

    /**
     * \brief �ô�����ͨ������ʼid
     *
     * LIS3MDL ����4·�ɼ�ͨ��,�ֱ�ΪX�ᣬY�ᣬZ��Ÿ�Ӧǿ�Ⱥ�һ·�¶����ݣ���
     * start_id��ʼ(����start_id)�����ĸ�ID�������ģ����ĸ�ͨ��������
     * start_id ��Ӧģ��X��Ÿ�Ӧǿ�ȣ�start_id + 1 ��Ӧģ���Y��Ÿ�Ӧǿ��
     * eg�� ��start_id����Ϊ0, ��ID = 0,��Ӧģ���ͨ��1��ID = 1��Ӧģ���ͨ��2.
     */
    int     start_id;

    int     drdy_pin;  /*< \brief ����׼�������������� */
    int     int_pin;   /*< \brief x,y,z��������ֵ����������ж����� */
    uint8_t i2c_addr;  /*< \brief I2C 7λ �豸��ַ(���豸7λ��ַ�̶�Ϊ 0x1C) */

    /** \brief ƽ̨��ʼ���ص��������ص�ע�ᴥ�������ѱ�ϵͳʹ�ã� */
    void    (*pfunc_plfm_init)(void);

} awbl_lis3mdl_devinfo_t;

/**
 * \brief LIS3MDL �豸�ṹ��
 */
typedef struct awbl_lis3mdl_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev                      dev;

    /** \brief I2C �ӻ��豸 */
    aw_i2c_device_t                      i2c_dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t                sensor_serv;

    /** \brief ͨ�������ص����� */
    aw_sensor_trigger_cb_t               pfn_trigger_fnc[3];

    /** \brief ͨ�������ص��������� */
    void                                *p_argc[3];

    /** \brief ͨ��������־ */
    uint32_t                             flags[3];

    /** \brief �ж���ʱ�������� */
    struct aw_isr_defer_job              g_myjob[2];

    /** \brief ����Ƶ�ʣ�ÿ������Ĵ��� */
    aw_sensor_val_t                      sampling_rate;

    /** \brief ������ֵ�ľ���ֵ��������ֵ��Ϊ�෴���� */
    aw_sensor_val_t                      threshold_data;

    /** \brief ��ǰ������ */
    uint8_t                              full_scale;

    /** \brief ����׼��������ֵ���� */
    aw_sensor_val_t                      current_data[3];

    /** \brief ����ʹ��λ��ÿλ��Ӧһ��ͨ�� */
    uint8_t                              trigger;

} awbl_lis3mdl_dev_t;

/*
 * \brief ע�� LIS3MDL ����
 */
void awbl_lis3mdl_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LIS3MDL_H */

/* end of file */
