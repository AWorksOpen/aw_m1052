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
 * \brief MMC5883MA (����Ŵ�����) ͷ�ļ�
 *
 * \internal
 * \par Modification History
 * - 1.00 18-11-30  ipk, first implementation
 * \endinternal
 */

#ifndef __AWBL_MMC5883MA_H
#define __AWBL_MMC5883MA_H

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
#include "aw_timer.h"

#define AWBL_MMC5883MA_NAME      "awbl_mmc5883ma"

/**
 * \brief MMC5883MA �豸��Ϣ�ṹ��
 */
typedef struct awbl_mmc5883ma_devinfo {
    /**
     * \brief �ô�����ͨ������ʼid
     *
     * MMC5883MA ����4·�ɼ�ͨ��,�ֱ�ΪX�ᣬY�ᣬZ��Ÿ�Ӧǿ�Ⱥ�һ·�¶����ݣ���
     * start_id��ʼ(����start_id)�����ĸ�ID�������ģ����ĸ�ͨ��������
     * start_id ��Ӧģ��X��Ÿ�Ӧǿ�ȣ�start_id + 1 ��Ӧģ���Y��Ÿ�Ӧǿ��
     * eg�� ��start_id����Ϊ0, ��ID = 0,��Ӧģ���ͨ��1��ID = 1��Ӧģ���ͨ��2.
     */
    int     start_id;

    /** \brief I2C�ӻ���ַ,7λ��ַ */
    uint8_t i2c_addr;

    /** \brief �жϴ������� */
    int     tergger_pin;

    /** \brief ƽ̨��ʼ�� */
    void   (* pfn_plfm_init)(void);


} awbl_mmc5883ma_devinfo_t;

/**
 * \brief MMC5883MA �豸�ṹ��
 */
typedef struct awbl_mmc5883ma_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev         dev;

    /** \brief I2C�ӻ��豸  */
    aw_i2c_device_t         i2c_dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t   sensor_serv;

    /** \brief �����ʱ�������ڶ��¶�ͨ����ʱ�ɼ� */
    aw_timer_t              timer;

    /** \brief ͨ��ʹ�ܣ���ӦλΪ1ʱ��ʾ��ͨ����ʹ�� (4ͨ����0~3bit��Ч)*/
    uint8_t                 channel;

    /** \brief ͨ������ʹ�� ����ӦλΪ1ʱ��ʾ��ͨ����ʹ�� (4ͨ����0~3bit��Ч)*/
    uint8_t                 ch_trigger;

    /** \brief ��¼���ƼĴ���1��ֵ */
    uint8_t                 ctrl1_reg_buf;

    /** \brief ��¼���ƼĴ���2��ֵ */
    uint8_t                 ctrl2_reg_buf;

    /** \brief ÿ���������������Ƶ�ʣ� */
    aw_sensor_val_t         sampling_rate[2];

    /** \brief X,Y,Zͨ����������ֵ */
    aw_sensor_val_t         threshold[3];

    /** \brief �ж��ӳ���ҵ */
    struct aw_isr_defer_job defer_job[2];

    /** \brief ͨ�������ص����� */
    aw_sensor_trigger_cb_t  pfn_trigger_cb[4];

    /** \brief ͨ�������ص��������� */
    void                   *p_argc[4];

    /** \brief ͨ��������־ */
    uint32_t                flags[4];

} awbl_mmc5883ma_dev_t;


/*
 * \brief ע�� MMC5883MA ����
 */
void awbl_mmc5883ma_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_MMC5883MA_H */

/* end of file */
