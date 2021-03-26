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
 * \brief BME280 (��ѹ��ʪ�ȴ�����)ͷ�ļ�
 *
 * \internal
 * \par Modification History
 * - 1.00 18-12-26  ipk, first implementation
 * \endinternal
 */

#ifndef __AWBL_BME280_H
#define __AWBL_BME280_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "awbl_sensor.h"
#include "aw_i2c.h"
#include "aw_compiler.h"

#define AWBL_BME280_NAME      "awbl_bme280"

/**
 * \brief BME280 IIR�˲�����
 */
typedef enum bme280_filter {
    AWBL_BME280_FILTER_OFF = 0,                   /**< \brief �ر�IIR�˲�             */
    AWBL_BME280_FILTER_COEFFICIENT_2,             /**< \brief �˲�����Ϊ2             */
    AWBL_BME280_FILTER_COEFFICIENT_4,             /**< \brief �˲�����Ϊ4             */
    AWBL_BME280_FILTER_COEFFICIENT_8,             /**< \brief �˲�����Ϊ8             */
    AWBL_BME280_FILTER_COEFFICIENT_16             /**< \brief �˲�����Ϊ16            */
} awbl_bme280_filter_t;

/**
 * \brief BME280����������
 */
typedef enum bme280_oversampling {
    AWBL_BME280_OVERSAMPLE_SKIP = 0,              /**< \brief ����������              */
    AWBL_BME280_OVERSAMPLE_X_1,                   /**< \brief ����������1             */
    AWBL_BME280_OVERSAMPLE_X_2,                   /**< \brief ����������2             */
    AWBL_BME280_OVERSAMPLE_X_4,                   /**< \brief ����������4             */
    AWBL_BME280_OVERSAMPLE_X_8,                   /**< \brief ����������8             */
    AWBL_BME280_OVERSAMPLE_X_16                   /**< \brief ����������16            */
} awbl_bme280_oversampling_t;

/**
 * \brief BME280ģʽ
 */
typedef enum bme280_mode {
    AWBL_BME280_SLEEP_MODE = 0,                   /**< \brief ˯��ģʽ                */
    AWBL_BME280_FORCED_MODE,                      /**< \brief ǿ��ģʽ                */
    AWBL_BME280_FORCED_MODE_ALSO,                 /**< \brief ǿ��ģʽ��ͬ��          */
    AWBL_BME280_NORMAL_MODE                       /**< \brief ����ģʽ                */
} awbl_bme280_mode_t;


/**
 * \brief BME280 �豸��Ϣ�ṹ��
 */
typedef struct awbl_bme280_devinfo {
    /**
     * \brief �ô�����ͨ������ʼid
     *
     * BME280 ����3·�ɼ�ͨ��,�ֱ�Ϊ��ѹ���ݡ��¶����ݺ�ʪ�����ݣ���
     * start_id��ʼ(����start_id)��������ID�������ģ�������ͨ��������
     * start_id ��Ӧģ����ѹ���ݣ�start_id + 1 ��Ӧģ����¶�����...
     * eg�� ��start_id����Ϊ0, ��ID = 0,��Ӧģ���ͨ��1��ID = 1��Ӧģ���ͨ��2.
     */
    int                        start_id;

    /** \brief I2C�ӻ���ַ,7λ��ַ */
    uint8_t                    i2c_addr;

    /**
     * \brief IIR�˲�����
     *
     * ������IIR�˲�ϵ�����趨��IIR�˲����ֳ�Ϊ��IIR�����˲����������ݹ��˲�������
     * �ݹ��˲���������˼����õݹ��ͽṹ�����ṹ�ϴ��з�����·��
     *
     * ʵ���ϴ������ڲ���ʪ�����ݲ�����ٲ�����Ҳ����Ҫ��ͨ�˲������ǣ�������ѹ�����ܵ�
     * �����ڱ仯��Ӱ�죬���磺ͨ������Ŵ������봫�����ķ�ȵȡ�Ϊ�������������
     * �е���Щ���ţ��û���������BME280�ڲ���IIR�˲���������Ч�Ľ������¶Ⱥ�ѹ��
     * ����źŵĴ�������ѹ�����¶���������ݷֱ�����ߵ�20λ���乫ʽΪ��
     *
     *               dat_filter_old*(filter_coeff-1)+dat_ADC
     * dat_filter = -----------------------------------------
     *                            filter_coeff
     *
     * dat_filter_old�����Ե�ǰ�˲����洢��������
     * dat_ADC       �����Ե�ǰ������ADC��ȡ������
     * dat_filter    ���˲����ڴ����ֵ��Ҳ�Ǽ������͵�����Ĵ�����ֵ
     * filter_coeff  ����Ҫ���õ��˲�ϵ��
     *
     * ���ڴˣ���֪filter_coeff�˲�ϵ��Խ�󣬴�����������Ӧ��Խ�ٻ���
     * BME280��֧�ֵ��˲�ϵ������2,4,8,16���֣�����ֵ����Ϊ0ʱ��ر�IIR�˲���
     */
    awbl_bme280_filter_t       filter_coeff;

    /**
     * \brief �¶ȹ���������ϵ��
     *
     * ��������ָ����Ƶ��Զ�����ο�˹�ز���Ƶ�ʣ���ԭϵͳ����Ƶ��Ϊfs��
     * ��������Ƶ����ߵ�R*fs��R���Ϊ���������ʡ�
     * ����������Խ���ܹ���Ч�����ʱ��ֱ��ʣ���С������
     *
     * BME280�����õĹ���������Ϊ1,2,4,8,16���֣�����Ϊ0ʱ������������
     */
    awbl_bme280_oversampling_t temp_oversampling;

    /**
     * \brief ѹ������������ϵ��
     */
    awbl_bme280_oversampling_t press_oversampling;

    /**
     * \brief ʪ�ȹ���������ϵ��
     */
    awbl_bme280_oversampling_t hum_oversampling;

    /**
     * \brief ����ģʽ�趨
     *
     * -SLEEP_MODE��˯��ģʽ�� ��ģʽ�£��ɶԼĴ���������д����������ִ���κβ���������
     *             һ�㲻��������Ϊ˯��ģʽ��
     *
     * -FORCED_MODE/FORCED_MODE_ALSO��
     *         ǿ��ģʽ��������Ϊ��ģʽ����ȡ����������ʱ�����ǿ��ģʽ��������ѡ���˲���
     *         ѡ����е��β�����������ɺ���Զ�����˯��ģʽ���´λ�ȡ����ʱ���ٴν���
     *         ǿ��ģʽ������Ҫ��Ͳ����ʡ��͹��Ļ��������ͬ����Ӧ�ã�����ʹ�ø�ģʽ��
     *
     * -NORMAL_MODE������ģʽ����ģʽ���ǲ���(��Ч)ʱ�κʹ���(��Ч)ʱ�ε��Զ�����ѭ����
     *         Ҳ���ǰ���һ������Ƶ��ѭ���������ݣ���ģʽ���ĸ�����������ģʽ�������ù�����
     *         ���˲�������������ģʽ����ʼ�������ݼĴ����л�ȡ���µĲ��������ʹ��IIR�˲�
     *         ��ʱ������ʹ�ø�ģʽ�������Թ��˶��ڸ��ŷǳ����á�
     */
    awbl_bme280_mode_t         mode;

} awbl_bme280_devinfo_t;

/**
 * \brief ����ֵ�ṹ��
 */
typedef struct awbl_compensation {

    /* ����¶Ȳ����Ĵ�����ֵ */
    uint16_t   dig_t1;
    int16_t    dig_t2;
    int16_t    dig_t3;

    /* ���ѹ�������Ĵ�����ֵ */
    uint16_t   dig_p1;
    int16_t    dig_p2;
    int16_t    dig_p3;
    int16_t    dig_p4;
    int16_t    dig_p5;
    int16_t    dig_p6;
    int16_t    dig_p7;
    int16_t    dig_p8;
    int16_t    dig_p9;

    /* ���ʪ�Ȳ����Ĵ�����ֵ */
    uint8_t    dig_h1;
    int16_t    dig_h2;
    uint8_t    dig_h3;
    int16_t    dig_h4;
    int16_t    dig_h5;
    int8_t     dig_h6;
} awbl_compensation_t;

/**
 * \brief BME280 �豸�ṹ��
 */
typedef struct awbl_bme280_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev         dev;

    /** \brief I2C�ӻ��豸  */
    aw_i2c_device_t         i2c_dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t   sensor_serv;

    /** \brief ��Ų���ֵ */
    awbl_compensation_t     compensation;

    /** \brief ���㲹��ֵ�����м��� */
    int32_t                 fine_temp;

    /** \brief �������������� */
    uint8_t                 rate;

} awbl_bme280_dev_t;


/*
 * \brief ע�� BME280 ����
 */
void awbl_bme280_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_BME280_H */

/* end of file */
