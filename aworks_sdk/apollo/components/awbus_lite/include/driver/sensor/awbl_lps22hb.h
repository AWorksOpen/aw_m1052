/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief LPS22HB ��ѹ����������
 *

 *
 * \par 1.������Ϣ
 *
 *  - ������:   "lps22hb_pressure"
 *  - ��������:   AWBL_BUSID_I2C
 *  - �豸��Ϣ:   struct awbl_lps22hb_par
 *
 * \par 2.�����豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_lps22hb_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_lps22hb
 *
 * \internal
 * \par modification history
 * - 1.00 18-10-21  dsg, first implementation.
 * \endinternal
 */

#ifndef __AWBL_LPS22HB_H
#define __AWBL_LPS22HB_H

/**
 * \addtogroup grp_awbl_if_lps22hb
 * \copydoc awbl_lps22hb.h
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_sensor.h"
#include "aw_sensor.h"
#include "aw_i2c.h"
#include "aw_common.h"
#include "aw_sys_defer.h"
#include "aw_sem.h"
#include "aw_int.h"
#include "aw_isr_defer.h"

#define AWBL_LPS22HB_NAME "sensor-lps22hb"

#define AW_LPS22HB_TRIGGER_FIFO  255

/**
 * \brief ��������������
 */
enum sampling_rate {
    AW_LPS22HB_OUTPUT_DATA_1Hz  = 1,
    AW_LPS22HB_OUTPUT_DATA_10Hz = 2,
    AW_LPS22HB_OUTPUT_DATA_25Hz = 3,
    AW_LPS22HB_OUTPUT_DATA_50Hz = 4,
    AW_LPS22HB_OUTPUT_DATA_75Hz = 5,
};

/* LPS22HB �������豸�ṹ�� */
typedef struct awbl_lps22hb_param {
    uint8_t   start_id;             /**< \brief ��������ʼID (ͨ��) */
    uint16_t  alert_pin;            /**< \brief �������� */
    uint16_t  addr;                 /**< \brief �������ӻ���ַ */

    void    (*pfn_plfm_init)(void); /**< \brief ƽ̨��ʼ������ */
} awbl_lps22hb_param_t;

/* LPS22HB �������豸��Ϣ�ṹ�� */
typedef struct awbl_lps22hb_dev {
    /** \brief �̳��� AWBus �豸 */
    awbl_dev_t              dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t   sensor_serv;

    /** \brief lps22hb���豸�ṹ */
    aw_i2c_device_t         i2c_lps22hb;

    /** \brief �ص� ����*/
    aw_sensor_trigger_cb_t  pfn_trigger_cb;

    /** \brief �ص��������� */
    void                   *p_arg;

    /** \brief ����ֵ */
    aw_sensor_val_t         threshold;

    /** \brief ����ģʽ */
    uint8_t                 threshold_mod;

    /** \brief ����ͨ�� */
    uint8_t                 alert_chan;

    /** \brief ����Ƶ�ʣ�ÿ������Ĵ��� */
    uint8_t                 sampling_rate;

    /** \brief FIFO ģʽ */
    uint8_t                 fifo_mode;

    /** \brief FIFO ˮӡ��־ */
    uint8_t                 watermark;

    /** \brief ����׼��������־ */
    uint8_t                 data_ready;

    /** \brief ���ݻ�����*/
    int32_t                 data_cache[2];

    /** \brief �ӳٶ���ڵ� */
    struct aw_isr_defer_job g_djob;

} awbl_lps22hb_dev_t;

/**
 * \brief ע�� sensor-lps22hb ����
 */
void awbl_lps22hb_drv_register (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AWBL_LPS22HB_H */
