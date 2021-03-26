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
 * \brief bma253������ٶȴ������豸
 *
 * \internal
 * \par modification history:
 * - 2018-10-24 cat, first implementation.
 * \endinternals
 */

#ifndef __AWBL_BMA253_H
#define __AWBL_BMA253_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_sensor.h"
#include "awbl_sensor.h"
#include "aw_i2c.h"
#include "awbl_i2cbus.h"
#include "aw_task.h"
#include "aw_sem.h"

#define AWBL_BMA253_NAME        "awbl_bma253"  /**< \brief �豸�� */

/**
 * \brief �������Ƶ�ʣ�ͨ������Ƶ�ʣ�
 */
#define AW_BMA253_15_63_HZ        (0)
#define AW_BMA253_31_25_HZ        (1)
#define AW_BMA253_62_5_HZ         (2)
#define AW_BMA253_125_HZ          (3)
#define AW_BMA253_250_HZ          (4)
#define AW_BMA253_500_HZ          (5)
#define AW_BMA253_1000_HZ         (6)
#define AW_BMA253_2000_HZ         (7)

/**
 * \brief ͨ������ֵ
 */
#define AW_BMA253_2G              (0)
#define AW_BMA253_4G              (1)
#define AW_BMA253_8G              (2)
#define AW_BMA253_16G             (3)

/**
 * \brief bma253 ���ݽṹ��
 */
struct awbl_bma253_data {

    int16_t  accel_x;                     /**< \brief ���ٶȼ�X�� */
    int16_t  accel_y;                     /**< \brief ���ٶȼ�Y�� */
    int16_t  accel_z;                     /**< \brief ���ٶȼ�Z�� */
    int16_t  temp;                        /**< \brief �¶� */
};

/**
 * \brief sensor-bma253 �豸��Ϣ
 */
typedef struct awbl_sensor_bma253_devinfo {

    uint8_t start_id;                  /**< \brief ������ͨ����ʼID */

    uint8_t addr;                      /**< \brief �豸�ӻ���ַ */

    uint8_t int1_port;                 /**< \brief �ж�1���ź� */
    uint8_t int2_port;                 /**< \brief �ж�2���ź� */

    void (*pfunc_plfm_init) (void);    /**< \brief ƽ̨��ʼ���ص����� */

}awbl_sensor_bma253_devinfo_t;

/**
 * \brief sensor-bma253 �豸ʵ��
 */
typedef struct awbl_sensor_bma253_dev {

    /**< \brief �̳��� AWBus I2C �ӻ�ʵ�� */
    struct awbl_i2c_device    super;

    /**< \brief sensor �豸����  */
    struct awbl_sensor_service  sensor_serv;

    /**< \brief ͨ������ */
    struct awbl_bma253_data    bma253_val;

    /**< \brief bma253���豸�ṹ */
    aw_i2c_device_t    i2c_dev;

    /**< \brief bma253����������ٶȣ�����Ƶ�ʣ� */
    uint8_t   sample_rate;  /* Ĭ��ֵ ��2000Hz */

    /**< \brief bma253�ļ�ⷶΧ */
    uint8_t   sample_range;  /* Ĭ��ֵ ����2g */

    /**< \brief bma253ͨ�������� */
    uint8_t    high_th;

    /**< \brief bma253ͨ�������� */
    uint8_t    low_th;

    /**< \brief bma253 zͨ���жϿ���״̬ */
    uint8_t    accel_z_int[2];

    /**< \brief bma253ͨ�����޴������� */
    uint8_t    high_trigger_set[3];

    /**< \brief bma253�����޴���ʹ�� */
    uint8_t    low_trigger_set;

    /**< \brief bma253��ͨ������ֵ */
    uint32_t   offset[3];

    /**< \brief bma253ͨ�������ص����� */
    aw_sensor_trigger_cb_t pfn_accel_trigger_cb[3];

    /**< \brief bma253�����޴����ص����� */
    aw_sensor_trigger_cb_t pfn_low_trigger_cb;

    /**< \brief ���޴����������ݾ��������ص��������� */
    void      *p_arg[4];

} awbl_sensor_bma253_dev_t;

/**
 * \brief sensor-bma253 driver register
 */
void awbl_bma253_drv_register (void);

/**
 * \brief ����bma253�����޴���
 *
 * \param[in] dev_id   �豸��Ԫ��
 * \param[in] p_low_th bma253��������ֵ
 * \param[in] pfn_cb   �����޴����Ļص�����
 * \param[in] p_arg    �ص���������
 *
 * \retval AW_OK ���óɹ�
 * \retval  <0   ����ʧ��
 */
aw_err_t awbl_bma253_low_trigger_cfg (int                      dev_id,
                                      const aw_sensor_val_t   *p_val,
                                      aw_sensor_trigger_cb_t   pfn_cb,
                                      void                    *p_arg);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_BMA253_H */


