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
 * \brief SHTC1 (����ʪ��ģ��) ͷ�ļ�
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-23  wan, first implementation
 * \endinternal
 */

#ifndef __AWBL_SHTC1_H
#define __AWBL_SHTC1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "awbl_sensor.h"

#define AWBL_SHTC1_NAME      "awbl_shtc1"
    
/**
 * \brief SHTC1 �豸��Ϣ�ṹ��
 */
typedef struct awbl_shtc1_devinfo {

    /**
     * \brief �ô�����ͨ������ʼid
     *
     * SHTC1 ����1·ʪ�Ȳɼ�ͨ����1·�¶Ȳɼ�ͨ��,��start_id��ʼ(����start_id)
     * ��������ID�������ģ�������ͨ�������� start_id ��Ӧģ���ʪ�ȴ�����ͨ����
     * start_id + 1 ��Ӧģ����¶ȴ�����ͨ��
     * eg����start_id ����Ϊ0, �� ID = 0,��Ӧģ���ͨ��1��ID = 1��Ӧģ���ͨ��2.
     */
    int     start_id;

    /*
     * \brief I2C 7λ �豸��ַ
     *
     * ���豸7λ��ַ�̶�Ϊ 0x70
     */
    uint8_t i2c_addr;

} awbl_shtc1_devinfo_t;

/**
 * \brief SHTC1 �豸�ṹ��
 */
typedef struct awbl_shtc1_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev                      dev;

    /** \brief I2C �ӻ��豸 */
    aw_i2c_device_t                      i2c_dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t                sensor_serv;

} awbl_shtc1_dev_t;

/*
 * \brief ע�� SHTC1 ����
 */
void awbl_shtc1_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_SHTC1_H */

/* end of file */
