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
 * \brief HTS221 ����
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-12  wan, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_gpio.h"
#include "aworks.h"
#include "aw_vdebug.h"
#include "driver/sensor/awbl_shtc1.h"

/*******************************************************************************
  ���غ궨��
*******************************************************************************/
/** \brief ͨ�� AWBus �豸����ȡ HTS221�豸 */
#define __MY_GET_DEV(p_dev) (awbl_shtc1_dev_t*)(p_dev)

/** \brief ͨ�� AWBus �豸����ȡ HTS221�豸��Ϣ */
#define __MY_DEVINFO_GET(p_dev) (awbl_shtc1_devinfo_t*)AWBL_DEVINFO_GET(p_dev)

#define __SHTC1_CLK_ENABLE_T_FIRST   (0x7CA2)  /*< \brief ��CLK���ȶ��¶�   */
#define __SHTC1_CLK_ENABLE_H_FIRST   (0x5C24)  /*< \brief ��CLK���ȶ�ʪ��   */

#define __SHTC1_CLK_DISABLE_T_FIRST  (0x7866)  /*< \brief �ر�CLK���ȶ��¶�   */
#define __SHTC1_CLK_DISABLE_H_FIRST  (0x58E0)  /*< \brief �ر�CLK���ȶ�ʪ��   */

#define __SHTC1_SOFT_RESET           (0x805D)  /*< \brief �����λ����        */

#define __SHTC1_READ_ID              (0xEFC8)  /*< \brief ��ȡ�ô�����ID����  */

#define __SHTC1_PRODUCT_CODE         (0x7)     /*< \brief ���ض�SHTC1��Ʒ���� */

/** \brief ����ʪ�ȵ�ʵ��ֵ */
#define __SHTC1_MEASURE_HUM_VAL(data)  ((int32_t)((1000000*100*   \
                                       (int64_t)(data))/65536))

/** \brief �����¶ȵ�ʵ��ֵ */
#define __SHTC1_MEASURE_TEM_VAL(data)  ((int32_t)((1000000*175*   \
                                       (int64_t)(data))/65536 - 45*1000000))

/*******************************************************************************
  ���غ�������
*******************************************************************************/
/*
 * \brief �ڶ��׶γ�ʼ������
 */
aw_local void __shtc1_inst_init2 (struct awbl_dev *p_dev);

/*
 * \brief �����ν׶γ�ʼ��
 */
aw_local void __shtc1_dev_connect (struct awbl_dev *p_dev);

/**
 * \brief �����������ȡ����
 */
aw_local aw_err_t __shtc1_sensorserv_get (struct awbl_dev *p_dev, void *p_arg);

/**
 * \brief ��ָ��ͨ����ȡ��������
 */
aw_local aw_err_t __shtc1_data_get (void            *p_cookie,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_buf);

/*******************************************************************************
  local functions
*******************************************************************************/
/*
 * \brief shtc1 д����
 */
aw_local aw_err_t __shtc1_write (awbl_shtc1_dev_t *p_this,
                                 uint32_t          subaddr,
                                 uint8_t          *p_buf,
                                 uint32_t          nbytes)
{
    return aw_i2c_write(&p_this->i2c_dev,
                        subaddr,
                        p_buf,
                        nbytes);
}

/*
 * \brief shtc1 ������
 */
aw_local aw_err_t __shtc1_read (awbl_shtc1_dev_t *p_this,
                                uint8_t           subaddr,
                                uint8_t          *p_buf,
                                uint32_t          nbytes)
{
    return aw_i2c_read(&p_this->i2c_dev,
                       subaddr,
                       p_buf,
                       nbytes);
}

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/
/** \brief �����ṩ�ķ��� */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

aw_local aw_const struct awbl_dev_method __g_shtc1_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __shtc1_sensorserv_get),
        AWBL_METHOD_END
};

/** \brief ������ڵ� */
aw_local aw_const struct awbl_drvfuncs __g_shtc1_drvfuncs = {
        NULL,                     /*< \brief ��һ�׶γ�ʼ�� */
        __shtc1_inst_init2,       /*< \brief �ڶ��׶γ�ʼ�� */
        __shtc1_dev_connect       /*< \brief �����׶γ�ʼ�� */
};

/** \brief ����ע����Ϣ */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_shtc1 = {
    AWBL_VER_1,                   /*< \brief AWBus �汾��   */
    AWBL_BUSID_I2C,               /*< \brief ���� ID        */
    AWBL_SHTC1_NAME,              /*< \brief ������         */
    &__g_shtc1_drvfuncs,          /*< \brief ������ڵ�     */
    &__g_shtc1_dev_methods[0],    /*< \brief �����ṩ�ķ��� */
    NULL                          /*< \brief ����̽�⺯��   */
};

/** \brief ������ */
aw_local aw_const struct awbl_sensor_servfuncs __g_shtc1_servfuncs = {
        __shtc1_data_get,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
};

/*******************************************************************************
    ���غ�������
*******************************************************************************/
/**
 * \brief �����������ȡ����
 */
aw_local aw_err_t __shtc1_sensorserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_shtc1_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_shtc1_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    awbl_sensor_service_t *p_sensor_serv = &p_this->sensor_serv;

    /** \brief �ô�������ͨ������ */
    aw_local aw_const awbl_sensor_type_info_t __g_senter_type[2] = {
            {AW_SENSOR_TYPE_HUMIDITY, 1},
            {AW_SENSOR_TYPE_TEMPERATURE, 1},
    };

    /** \brief ��������������Ϣ */
    aw_local aw_const awbl_sensor_servinfo_t  __g_senser_info = {
            2,
            __g_senter_type,
            AW_NELEMENTS(__g_senter_type)
    };

    p_sensor_serv->p_servinfo  = &__g_senser_info;
    p_sensor_serv->start_id    = p_devinfo->start_id;
    p_sensor_serv->p_servfuncs = &__g_shtc1_servfuncs;
    p_sensor_serv->p_cookie    = (awbl_shtc1_dev_t*)p_this;
    p_sensor_serv->p_next      = NULL;

    *((awbl_sensor_service_t**)p_arg) = p_sensor_serv;

    return AW_OK;
}

/******************************************************************************/
/** \brief �ڶ��γ�ʼ�������������ȡ���� */
aw_local void __shtc1_inst_init2 (struct awbl_dev *p_dev)
{
    awbl_shtc1_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_shtc1_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    /** \brief ����IIC�豸 */
    aw_i2c_mkdev(&p_this->i2c_dev,
                 p_dev->p_devhcf->bus_index,
                 p_devinfo->i2c_addr,
                 AW_I2C_ADDR_7BIT);

}

/******************************************************************************/
/** \brief �����ν׶γ�ʼ�� */
aw_local void __shtc1_dev_connect (struct awbl_dev *p_dev)
{
    awbl_shtc1_dev_t     *p_this    = __MY_GET_DEV(p_dev);

    uint8_t send_cmd[2] = {0};
    uint8_t recv_buff[3] = {0};

    send_cmd[0] = ((uint16_t)__SHTC1_READ_ID) >> 8;
    send_cmd[1] = (uint8_t)__SHTC1_READ_ID;

    __shtc1_write(p_this, 0, send_cmd, 2);
    __shtc1_read(p_this, 0, recv_buff, 3);

    /** \brief �Ա��豸ID */
    if ((recv_buff[1] & (~(0x3 << 6))) != __SHTC1_PRODUCT_CODE) {
        aw_kprintf("\r\nSensor SHTC1 is not ready! \r\n");
        return;
    }
}
/******************************************************************************/
/** \brief ��ָ��ͨ����ȡ�������� */
aw_local aw_err_t __shtc1_data_get (void            *p_cookie,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_buf)
{
    awbl_shtc1_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_shtc1_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    int i = 0;
    int cur_id = 0;

    aw_err_t ret = AW_OK;

    uint8_t sensor_data[6] = {0};

    uint16_t hum_data = 0;
    uint16_t tem_data = 0;

    uint8_t cmd_buff[2] = {0};

    cmd_buff[0] = ((uint16_t)__SHTC1_CLK_DISABLE_H_FIRST) >> 8;
    cmd_buff[1] = (uint8_t)__SHTC1_CLK_DISABLE_H_FIRST;

    ret = __shtc1_write(p_this, 0, cmd_buff, 2);
    if (ret != AW_OK) {
        AWBL_SENSOR_VAL_SET_RESULT(&p_buf[i], ret);
        return ret;
    }

    do {
        ret = __shtc1_read(p_this, 0, sensor_data, 6);
        i++;
        if (i > 3) {
            return ret;
        }
    } while (ret != AW_OK);

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id == 0) {

            hum_data = (uint16_t)((sensor_data[0] << 8) | sensor_data[1]);

            p_buf[i].val  = __SHTC1_MEASURE_HUM_VAL(hum_data);
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;

        } else if (cur_id == 1) {

            tem_data = (uint16_t)((sensor_data[3] << 8) | sensor_data[4]);

            p_buf[i].val  = __SHTC1_MEASURE_TEM_VAL(tem_data);
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;

        } else {
            break;
        }
    }

    return ret;
}

/*******************************************************************************
  extern functions
*******************************************************************************/
/**
 * \brief �� SHTC1 ����ע�ᵽ AWBus ��ϵͳ��
 */
void awbl_shtc1_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_shtc1);
}

/* end of file */
