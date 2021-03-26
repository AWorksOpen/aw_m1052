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
 * \brief ������ BMG160 �����ļ�
 *
 * \internal
 * \par Modification history
 * - 1.00 18-12-11  ipk, first implementation.
 * \endinternal
 */


#include "aworks.h"
#include "awbus_lite.h"
#include "aw_errno.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "aw_gpio.h"
#include "aw_isr_defer.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "driver/sensor/awbl_bmg160.h"


/*******************************************************************************
 * �궨��
 ******************************************************************************/

/* �����꣬ͨ�������豸ָ���þ����豸ָ�� */
#define __BMG160_DEV_DECL(p_this,p_dev) struct awbl_bmg160_dev *(p_this) = \
                                        (struct awbl_bmg160_dev *)(p_dev)

/* �����꣬ͨ�������豸ָ�����豸��Ϣָ�� */
#define __BMG160_DEVINFO_DECL(p_devinfo,p_dev) \
            struct awbl_bmg160_devinfo *(p_devinfo) = \
           (struct awbl_bmg160_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* �����꣬ͨ�������豸ָ���ô���������ָ�� */
#define __BMG160_SENSORSERV_DECL(p_serv,p_dev) \
            struct awbl_sensor_service_t *(p_serv) = \
           (struct awbl_sensor_service_t *)&((p_dev)->sensor_serv)

#define __BMG160_CHIP_ID        (0x00)  /**< \brief �豸ID��ַ */
#define __BMG160_MY_ID          (0x0F)  /**< \brief �豸ID */
#define __BMG160_RATE_X_LSB     (0x02)  /**< \brief ����Xͨ�������ʶ���ֵ�������Чλ */
#define __BMG160_RATE_X_MSB     (0x03)  /**< \brief ����Xͨ�������ʶ���ֵ�������Чλ */
#define __BMG160_RATE_Y_LSB     (0x04)  /**< \brief ����Yͨ�������ʶ���ֵ�������Чλ */
#define __BMG160_RATE_Y_MSB     (0x05)  /**< \brief ����Yͨ�������ʶ���ֵ�������Чλ */
#define __BMG160_RATE_Z_LSB     (0x06)  /**< \brief ����Zͨ�������ʶ���ֵ�������Чλ */
#define __BMG160_RATE_Z_MSB     (0x07)  /**< \brief ����Zͨ�������ʶ���ֵ�������Чλ */
#define __BMG160_TEMP           (0x08)  /**< \brief ������ǰоƬ�¶� */
#define __BMG160_INT_STATUS_0   (0x09)  /**< \brief �����ж�״̬λ */
#define __BMG160_INT_STATUS_1   (0x0A)  /**< \brief �����ж�״̬λ */
#define __BMG160_INT_STATUS_2   (0x0B)  /**< \brief �����κ��˶��ж�״̬λ */
#define __BMG160_INT_STATUS_3   (0x0C)  /**< \brief �����������ж�״̬λ */

#define __BMG160_FIFO_STATUS    (0x0E)  /**< \brief ����FIFO״̬��־ */
#define __BMG160_RANGE          (0x0F)  /**< \brief ����BMG160֧��5�ֲ�ͬ�Ľ����ʲ�����Χ */
#define __BMG160_BW             (0x10)  /**< \brief �üĴ�������ѡ�����������˲������� */
#define __BMG160_LPM1           (0x11)  /**< \brief ѡ������Դģʽ */
#define __BMG160_LPM2           (0x12)  /**< \brief �����ϵ���ⲿ�������������� */
#define __BMG160_RATE_HBW       (0x13)  /**< \brief ���������ݲɼ������������ʽ */
#define __BMG160_BGW_SOFTRESET  (0x14)	/**< \brief �����û������Ĵ�������λ */ 
#define __BMG160_INT_EN_0       (0x15)  /**< \brief ����������Щ�ж� */
#define __BMG160_INT_EN_1       (0x16)  /**< \brief �����ж��������� */
#define __BMG160_INT_MAP_0      (0x17)  /**< \brief ������Щ�ж��ź�ӳ�䵽INT1���� */
#define __BMG160_INT_MAP_1      (0x18)  /**< \brief ������Щ�ж��ź�ӳ�䵽INT1���ź�INT2���� */
#define __BMG160_INT_MAP_2      (0x19)  /**< \brief ������Щ�ж��ź�ӳ�䵽INT2���� */
#define __BMG160_INTR_ZERO_ADDR (0x1A)  /**< \brief ���ƾ��п�ѡ����Դ����Щ�жϵ�����Դ����  */
#define __BMG160_INTR_ONE_ADDR  (0x1B)  /**< \brief ���ƿ���ƫ�Ʋ���������Դ������κ��˶���ֵ  */
#define __BMG160_INTR_TWO_ADDR  (0x1C)  /**< \brief  */
#define __BMG160_INTR_4_ADDR    (0x1E)  /**< \brief  */

#define __BMG160_INT_RST_LATCH  (0x21)  /**< \brief �����жϸ�λλ���ж�ģʽѡ�� */
#define __BMG160_High_Th_x      (0x22)  /**< \brief ����x��ĸ�������ֵ�͸������ͺ����� */
#define __BMG160_High_Dur_x     (0x23)  /**< \brief ����x��ĸ����ʳ���ʱ������ */
#define __BMG160_High_Th_y      (0x24)  /**< \brief ����y��ĸ�������ֵ�͸������ͺ����� */
#define __BMG160_High_Dur_y     (0x25)  /**< \brief ����y��ĸ����ʳ���ʱ������ */
#define __BMG160_High_Th_z      (0x26)  /**< \brief ����z��ĸ�������ֵ�͸������ͺ����� */
#define __BMG160_High_Dur_z     (0x27)  /**< \brief ����z��ĸ����ʳ���ʱ������ */
                              
#define __BMG160_SoC            (0x31)  /**< \brief ��������ƫ��ȡ������ */
#define __BMG160_A_FOC          (0x32)  /**< \brief ��������ƫ��ȡ������ */
#define __BMG160_TRIM_NVM_CTRL  (0x33)  /**< \brief �������οɱ�̷���ʧ�Դ洢����NVM���Ŀ������� */
#define __BMG160_BGW_SPI3_WDT   (0x34)  /**< \brief �������ֽӿڵ����� */
                              
#define __BMG160_OFC1           (0x36)  /**< \brief ����ƫ�Ʋ���ֵ */
#define __BMG160_OFC2           (0x37)  /**< \brief ����Xͨ����ƫ�Ʋ���ֵ */
#define __BMG160_OFC3           (0x38)  /**< \brief ����Yͨ����ƫ�Ʋ���ֵ */
#define __BMG160_OFC4           (0x39)  /**< \brief ����Zͨ����ƫ�Ʋ���ֵ */
#define __BMG160_TRIM_GP0       (0x3A)  /**< \brief ��������NVM���ݵ�ͨ�����ݼĴ��� */
#define __BMG160_TRIM_GP1       (0x3B)  /**< \brief ��������NVM���ݵ�ͨ�����ݼĴ��� */
#define __BMG160_BIST           (0x3C)  /**< \brief ���������Բ⣨BIST������ */
#define __BMG160_FIFO_CONFIG_0  (0x3D)  /**< \brief ����FIFOwatermark���� */
#define __BMG160_FIFO_CONFIG_1  (0x3E)  /**< \brief ����FIFO�������� */
#define __BMG160_FIFO_DATA      (0x3F)  /**< \brief FIFO���ݶ����Ĵ��� */

/** \brief ������int8ת��Ϊһ��int16_t���� */
#define __BMG160_UINT8_TO_UINT16(buff) \
                       (int16_t)(((uint16_t)(buff[1]) << 8) \
                               | ((uint16_t)(buff[0])))
                                                          
/** \brief X����ٶȲ��� */ 
#define __BMG160_X_START       (0x2<<0)  
/** \brief Y����ٶȲ��� */ 
#define __BMG160_Y_START       (0x2<<2)                   
/** \brief Z����ٶȲ��� */ 
#define __BMG160_Z_START       (0x2<<5)                                  
 
#define BMG160_ODR_100 (100)
#define BMG160_ODR_200 (200)
#define BMG160_ODR_400 (400)
#define BMG160_ODR_1000 (1000)
#define BMG160_ODR_2000 (2000)

#define BMG160_ODR_100_REG (0x07)
#define BMG160_ODR_200_REG (0x06)
#define BMG160_ODR_400_REG (0x03)
#define BMG160_ODR_1000_REG (0x02)
#define BMG160_ODR_2000_REG (0x01)

/*******************************************************************************
 * ���غ�������
 ******************************************************************************/

/** \brief ��ȡ������ͨ���������� */
aw_local aw_err_t __pfn_data_get (void            *p_drv,
                                  const int       *p_ids,
                                  int              num,
                                  aw_sensor_val_t *p_buf);

/** \brief ʹ�ܴ�����ͨ�� */
aw_local aw_err_t __pfn_enable (void            *p_drv,
                                const int       *p_ids,
                                int              num,
                                aw_sensor_val_t *p_result);

/** \brief ���ܴ�����ͨ�� */
aw_local aw_err_t __pfn_disable (void            *p_drv,
                                 const int       *p_ids,
                                 int              num,
                                 aw_sensor_val_t *p_result);

/** \brief ���ô�����ͨ������ */
aw_local aw_err_t __pfn_attr_set (void                  *p_drv,
                                  int                    id,
                                  int                    attr,
                                  const aw_sensor_val_t *p_val);

/** \brief ��ȡ������ͨ������ */
aw_local aw_err_t __pfn_attr_get (void            *p_drv,
                                  int              id,
                                  int              attr,
                                  aw_sensor_val_t *p_val);

/** \brief ���ô�����һ��ͨ����������һ�������ص����� */
aw_local aw_err_t __pfn_trigger_cfg (void                   *p_drv,
                                     int                     id,
                                     uint32_t                flags,
                                     aw_sensor_trigger_cb_t  pfn_cb,
                                     void                   *p_arg);

/** \brief �򿪴��� */
aw_local aw_err_t __pfn_trigger_on (void *p_drv, int id);

/** \brief �رմ��� */
aw_local aw_err_t __pfn_trigger_off (void *p_drv, int id);

/**
 * \brief ��3�׶γ�ʼ��
 */
aw_local void __bmg160_inst_connect (awbl_dev_t *p_dev);

/**
 * \brief BMG160 ��׼����ӿڻ�ȡ
 */
aw_local aw_err_t __bmg160_sensorserv_get (struct awbl_dev  *p_dev,
                                           void             *p_arg);
/** \breif �жϻص����� */
aw_local void __bmg160_alarm_callback (void *p_arg);

/** \breif �ж��ӳٺ��� */
aw_local void __isr_defer_handle (void *p_arg);

/**
 * \brief BMG160 д����
 */
aw_local aw_err_t __bmg160_write (awbl_bmg160_dev_t *p_this,
                                  uint32_t           subaddr,
                                  uint8_t           *p_buf,
                                  uint32_t           nbytes);
/**
 * \brief BMG160 ������
 */
aw_local aw_err_t __bmg160_read (awbl_bmg160_dev_t *p_this,
                                 uint32_t           subaddr,
                                 uint8_t           *p_buf,
                                 uint32_t           nbytes);

/******************************************************************************/

/**
 *\brief BMG160�������������
 */
aw_local aw_const struct awbl_drvfuncs __g_bmg160_drvfuncs = {
    NULL,
    NULL,
    __bmg160_inst_connect
};

/** \brief ������������Ϣ */
aw_local aw_const awbl_sensor_type_info_t __g_bmg160_typeinfo[] = {
    {AW_SENSOR_TYPE_GYROSCOPE, 3},
};

/** \brief ����������Ϣ */
aw_local aw_const awbl_sensor_servinfo_t __g_bmg160_servinfo = {
    3,
    __g_bmg160_typeinfo,
    AW_NELEMENTS(__g_bmg160_typeinfo),
};

/** \brief ������ */
aw_local aw_const struct awbl_sensor_servfuncs __g_bmg160_servfuncs = {
    __pfn_data_get,
    __pfn_enable,
    __pfn_disable,
    __pfn_attr_set,
    __pfn_attr_get,
    __pfn_trigger_cfg,
    __pfn_trigger_on,
    __pfn_trigger_off,
};

/** \brief �����ṩ�ķ��� */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);
aw_local aw_const struct awbl_dev_method __g_bmg160_methods[] = {
    {AWBL_METHOD_CALL(awbl_sensorserv_get), __bmg160_sensorserv_get},
    AWBL_METHOD_END
};

/** \brief ����ע����Ϣ */
aw_local aw_const struct awbl_drvinfo __g_bmg160_drvinfo = {
    AWBL_VER_1,
    AWBL_BUSID_I2C,
    AWBL_BMG160_NAME,
    &__g_bmg160_drvfuncs,
    __g_bmg160_methods,
    NULL
};

/**
 * \brief BMG160 ��׼����ӿڻ�ȡ
 */
aw_local aw_err_t __bmg160_sensorserv_get (struct awbl_dev  *p_dev,
                                           void             *p_arg)
{
    __BMG160_DEV_DECL(p_this,p_dev);
    __BMG160_DEVINFO_DECL(p_devinfo,p_dev);
    struct awbl_sensor_service *p_serv = &p_this->sensor_serv;

    p_serv->p_next      = NULL;
    p_serv->p_cookie    = (void *)p_this;
    p_serv->start_id    = p_devinfo->start_id;
    p_serv->p_servinfo  = &__g_bmg160_servinfo;
    p_serv->p_servfuncs = &__g_bmg160_servfuncs;

    *(struct awbl_sensor_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief ��3�׶γ�ʼ��
 */
aw_local void __bmg160_inst_connect (awbl_dev_t *p_dev)
{
    __BMG160_DEV_DECL(p_this,p_dev);
    __BMG160_DEVINFO_DECL(p_devinfo,p_dev);

    aw_err_t ret       = AW_OK;
    uint8_t bmg160_reg = 0;
    aw_sensor_val_t val;

    aw_i2c_mkdev(&p_this->i2c_dev,
                 p_dev->p_devhcf->bus_index,
                 p_devinfo->i2c_addr,
                 AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);

    p_this->pfn_trigger_cb[0]  = NULL;
    p_this->pfn_trigger_cb[1]  = NULL;
    p_this->pfn_trigger_cb[2]  = NULL;
    p_this->p_arg[0]           = NULL;
    p_this->p_arg[1]           = NULL;
    p_this->p_arg[2]           = NULL;
    p_this->flags[0]           = 0;
    p_this->flags[1]           = 0;
    p_this->flags[2]           = 0;
    p_this->range.val          = 0;
    p_this->range.unit         = AW_SENSOR_UNIT_INVALID;
    p_this->trigger            = 0;
    p_this->data[0].val        = 0;
    p_this->data[0].unit       = AW_SENSOR_UNIT_INVALID;
    p_this->data[1].val        = 0;
    p_this->data[1].unit       = AW_SENSOR_UNIT_INVALID;
    p_this->data[2].val        = 0;
    p_this->data[2].unit       = AW_SENSOR_UNIT_INVALID;

    if (aw_gpio_pin_request("bmg160", &p_devinfo->trigger_pin, 1) != AW_OK) {
        aw_kprintf("\r\nSensor BMG160 trigger pin request failed ! \r\n");
    }

    if (p_devinfo->trigger_pin != -1) {

        aw_gpio_trigger_connect(p_devinfo->trigger_pin,
                                __bmg160_alarm_callback,
                                (void*)p_dev);
        aw_gpio_trigger_cfg(p_devinfo->trigger_pin, AW_GPIO_TRIGGER_LOW);
    }

    aw_isr_defer_job_init(&p_this->g_myjob, __isr_defer_handle, p_this);

    ret = __bmg160_read(p_this, __BMG160_CHIP_ID, &bmg160_reg, 1);

    if (ret != AW_OK || bmg160_reg != __BMG160_MY_ID) {
        aw_kprintf("\r\nSensor BMG160 Init is ERROR! \r\n");
    }

    val.val  = 2000;
    val.unit = AW_SENSOR_UNIT_BASE;
    __pfn_attr_set(p_this, p_devinfo->start_id, AW_SENSOR_ATTR_FULL_SCALE, &val);

    bmg160_reg = 0x00;
    __bmg160_write(p_this, __BMG160_INT_EN_1, &bmg160_reg, 1);

    __bmg160_read(p_this, __BMG160_INT_MAP_0, &bmg160_reg, 1);

    bmg160_reg |= 0x08;
    __bmg160_write(p_this, __BMG160_INT_MAP_0, &bmg160_reg, 1);

    __bmg160_read(p_this, __BMG160_INT_MAP_1, &bmg160_reg, 1);

    bmg160_reg |= 0x01;
    __bmg160_write(p_this, __BMG160_INT_MAP_1, &bmg160_reg, 1);

}

/*******************************************************************************
  Local functions
*******************************************************************************/
/**
 * \brief BMG160 д����
 */
aw_local aw_err_t __bmg160_write (awbl_bmg160_dev_t *p_this,
                                  uint32_t           subaddr,
                                  uint8_t           *p_buf,
                                  uint32_t           nbytes)
{
    return aw_i2c_write(&p_this->i2c_dev, subaddr, p_buf, nbytes);
}

/**
 * \brief BMG160 ������
 */
aw_local aw_err_t __bmg160_read (awbl_bmg160_dev_t *p_this,
                                 uint32_t           subaddr,
                                 uint8_t           *p_buf,
                                 uint32_t           nbytes)
{
    return aw_i2c_read(&p_this->i2c_dev, subaddr, p_buf, nbytes);
}

/** \breif �жϻص����� */
aw_local void __bmg160_alarm_callback (void *p_arg)
{
    awbl_bmg160_dev_t* p_this = (awbl_bmg160_dev_t*)p_arg;

    aw_isr_defer_job_add(&p_this->g_myjob);   /*< \brief ����ж��ӳٴ������� */
}

/** \breif �󴫸�����ǰ���� */
aw_local int __bmg160_range_resolu_get (void *p_arg, uint16_t *p_val)
{
    awbl_bmg160_dev_t* p_this = (awbl_bmg160_dev_t*)p_arg;

    if (p_this->range.val == 2000) {
        *p_val = 610;
    } else if (p_this->range.val == 1000) {
        *p_val = 305;
    } else if (p_this->range.val == 500) {
        *p_val = 153;
    } else if (p_this->range.val == 250) {
        *p_val = 76;
    } else if (p_this->range.val == 125) {
        *p_val = 38;
    } else {
        return -1;
    }

    return AW_OK;
}

/** \breif �ж��ӳٺ��� */
aw_local void __isr_defer_handle (void *p_arg)
{
    awbl_bmg160_dev_t* p_this = (awbl_bmg160_dev_t*)p_arg;
    uint8_t  current_data[6] = {0};
    int16_t  x_axis_data     = 0;
    int16_t  y_axis_data     = 0;
    int16_t  z_axis_data     = 0;
    uint8_t  reg             = 0;
    uint16_t resolu;

    __bmg160_read(p_this, __BMG160_INT_STATUS_1, &reg, 1);

    if (!AW_BIT_GET(reg, 7)) {
        return;
    }

    if (__bmg160_range_resolu_get(p_arg, &resolu) != AW_OK) {
        return;
    }

    /** \brief ��ȡX����ٶ�ֵ */
    __bmg160_read(p_this, __BMG160_RATE_X_LSB, current_data, 6);

    /* ��ȡX����ٶ� */
    x_axis_data = __BMG160_UINT8_TO_UINT16(current_data);
    p_this->data[0].val = x_axis_data * resolu * 100;
    p_this->data[0].unit = AW_SENSOR_UNIT_MICRO;

    /* ��ȡY����ٶ� */
    y_axis_data = __BMG160_UINT8_TO_UINT16((current_data+2));
    p_this->data[1].val = y_axis_data * resolu * 100;
    p_this->data[1].unit = AW_SENSOR_UNIT_MICRO;

    /* ��ȡZ����ٶ� */
    z_axis_data = __BMG160_UINT8_TO_UINT16((current_data+4));
    p_this->data[2].val = z_axis_data * resolu * 100;
    p_this->data[2].unit = AW_SENSOR_UNIT_MICRO;

    if (AW_BIT_GET(p_this->trigger, 0) &&
        p_this->pfn_trigger_cb[0]      &&
        (p_this->flags[0] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_cb[0](p_this->p_arg[0],
                                  AW_SENSOR_TRIGGER_DATA_READY);
    }

    if (AW_BIT_GET(p_this->trigger, 1) &&
        p_this->pfn_trigger_cb[1]      &&
       (p_this->flags[1] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_cb[1](p_this->p_arg[1],
                                  AW_SENSOR_TRIGGER_DATA_READY);
    }
     
    if (AW_BIT_GET(p_this->trigger, 2) &&
        p_this->pfn_trigger_cb[2]      &&
        (p_this->flags[2] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_cb[2](p_this->p_arg[2],
                                  AW_SENSOR_TRIGGER_DATA_READY);
    }
}

/** \brief ��ȡ������ͨ���������� */
aw_local aw_err_t __pfn_data_get (void            *p_drv,
                                  const int       *p_ids,
                                  int              num,
                                  aw_sensor_val_t *p_buf)
{
    __BMG160_DEV_DECL(p_this, p_drv);
    __BMG160_DEVINFO_DECL(p_devinfo, p_drv);
    
    uint16_t temp_val;

    aw_err_t ret        = AW_OK;

    uint8_t reg_data[6] = {0};
    int16_t x_axis_data = 0;
    int16_t y_axis_data = 0;
    int16_t z_axis_data = 0;
    
    int cur_id = 0;
    int i = 0;

    if (p_drv == NULL) {
        return -AW_EINVAL;
    }

    for (i = 0; i < num; i++) {
        p_buf[i].unit = AW_SENSOR_UNIT_INVALID;
    }

    cur_id = p_ids[0] - p_devinfo->start_id;

    for (i = 0; i < num; i++) {

        /* ��������׼������������ʽ����ֱ�Ӹ�ֵ */
        if (AW_BIT_GET(p_this->trigger, i) &&
            ((p_this->flags[i] & AW_SENSOR_TRIGGER_DATA_READY))) {

            p_buf[i].val = p_this->data[i].val;
            p_buf[i].unit= p_this->data[i].unit;

            return AW_OK;

        }
    }

    if (__bmg160_range_resolu_get(p_this, &temp_val) != AW_OK) {
        return AW_ERROR;
    }

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id == 0) {
            
            /** \brief ��ȡX����ٶ����� */
            ret = __bmg160_read(p_this, __BMG160_RATE_X_LSB, reg_data, 2);
            if (ret != AW_OK) {
                return ret;
            }
            x_axis_data = __BMG160_UINT8_TO_UINT16(reg_data);
            p_buf[i].val  = x_axis_data * temp_val *100;  /*< \brief X����ٶ�����(��/S) */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;         /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        } else if (cur_id == 1) {
            /** \brief ��ȡY����ٶ����� */
            ret = __bmg160_read(p_this, __BMG160_RATE_Y_LSB, reg_data+2, 2);
            if (ret != AW_OK) {
                return ret;
            }
            y_axis_data = __BMG160_UINT8_TO_UINT16((reg_data+2));
            p_buf[i].val  = y_axis_data * temp_val *100; ;  /*< \brief Y����ٶ�����(��/S) */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;           /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        } else if (cur_id == 2) {
            /** \brief ��ȡZ����ٶ����� */
            ret = __bmg160_read(p_this, __BMG160_RATE_Z_LSB, reg_data+4, 2);
            if (ret != AW_OK) {
                return ret;
            }
            z_axis_data = __BMG160_UINT8_TO_UINT16((reg_data+4));
            p_buf[i].val  = z_axis_data * temp_val *100; ;  /*< \brief Z����ٶ�����(��/S) */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;           /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        } else {
              /*< \brief ���˴�ͨ�������ڸô�������ֱ�ӷ��� */
        }
    }
    return ret;
}

/** \brief ʹ�ܴ�����ͨ�� */
aw_local aw_err_t __pfn_enable (void            *p_drv,
                                const int       *p_ids,
                                int              num,
                                aw_sensor_val_t *p_result)
{
     return AW_OK;
}

/** \brief ���ܴ�����ͨ�� */
aw_local aw_err_t __pfn_disable (void            *p_drv,
                                 const int       *p_ids,
                                 int              num,
                                 aw_sensor_val_t *p_result)
{
    return AW_OK;
}
 
/** \brief ���ô�����ͨ�����ԣ�attributes�� */
aw_local aw_err_t __pfn_attr_set (void                  *p_drv,
                                  int                    id,
                                  int                    attr,
                                  const aw_sensor_val_t *p_val)
{
    __BMG160_DEV_DECL(p_this, p_drv);
    __BMG160_DEVINFO_DECL(p_devinfo, p_drv);

    uint8_t reg_data = 0;
    int     cur_id   = id - p_devinfo->start_id;

    aw_sensor_val_t val;

    if (cur_id < 0 || cur_id > 2) {
        return -AW_ENODEV;
    }

    if (AW_SENSOR_ATTR_FULL_SCALE == attr) {
        val.val  = p_val->val;
        val.unit = p_val->unit;
        aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_BASE);

        if (val.val >0 && val.val < 250) {
            p_this->range.val = 125;
            reg_data = 0x04;  /* ��125��/S */
        } else if (val.val >= 250 && val.val < 500) {
            p_this->range.val = 250;
            reg_data = 0x03;  /* ��250��/S */
        } else if (val.val >= 500 && val.val < 1000) {
            p_this->range.val = 500;
            reg_data = 0x02;  /* ��500��/S */
        } else if (val.val >= 1000 && val.val < 2000) {
            p_this->range.val = 1000;
            reg_data = 0x01;  /* ��1000��/S */
        } else if (val.val >= 2000) {
            p_this->range.val = 2000;
            reg_data = 0x00;  /* ��2000��/S */
        } else {
            return -AW_ENOTSUP;
        }

        p_this->range.unit = AW_SENSOR_UNIT_BASE;

        reg_data |= 0x80;
        __bmg160_write(p_this, __BMG160_RANGE, &reg_data, 1);

    }

    return AW_OK;
}
 
/** \brief ��ȡ������ͨ������ */
aw_local aw_err_t __pfn_attr_get (void            *p_drv,
                                  int              id,
                                  int              attr,
                                  aw_sensor_val_t *p_val)
{
    __BMG160_DEV_DECL(p_this, p_drv);
    __BMG160_DEVINFO_DECL(p_devinfo, p_drv);

    int     cur_id   = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id > 2) {
        return -AW_ENODEV;
    }

    if (AW_SENSOR_ATTR_FULL_SCALE == attr) {
        p_val->val  = p_this->range.val;
        p_val->unit = p_this->range.unit;
    }

    return AW_OK;
}


/** \brief ���ô�����һ��ͨ����������һ�������ص����� */
aw_local aw_err_t __pfn_trigger_cfg (void                   *p_drv,
                                     int                     id,
                                     uint32_t                flags,
                                     aw_sensor_trigger_cb_t  pfn_cb,
                                     void                   *p_arg)
{
    __BMG160_DEV_DECL(p_this, p_drv);
    __BMG160_DEVINFO_DECL(p_devinfo, p_drv);

    int      cur_id  = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id > 2) {
        return -AW_ENODEV;
    }

    if (p_drv == NULL) {
        return -AW_EINVAL;
    }

    switch (flags) {
    case AW_SENSOR_TRIGGER_DATA_READY:
        break;
    default:
        return -AW_ENOTSUP;
    }

    p_this->pfn_trigger_cb[cur_id] = pfn_cb;
    p_this->p_arg[cur_id]          = p_arg;
    p_this->flags[cur_id]          = flags;

    return AW_OK;
}

/** \brief �򿪴��� */
aw_local aw_err_t __pfn_trigger_on (void *p_drv, int id)
{
    __BMG160_DEV_DECL(p_this, p_drv);
    __BMG160_DEVINFO_DECL(p_devinfo, p_drv);

    int      cur_id  = id - p_devinfo->start_id;

    aw_err_t ret = AW_OK;

    uint8_t reg_data = 0;

    if (cur_id < 0 || cur_id > 2) {
        return -AW_ENODEV;
    }

    if (p_drv == NULL) {
        return -AW_EINVAL;
    }

    ret = __bmg160_read(p_this, __BMG160_INT_EN_0, &reg_data, 1);
    if ( ret != AW_OK) {
        return ret;
    }
    reg_data |= 0x80;
    ret = __bmg160_write(p_this, __BMG160_INT_EN_0, &reg_data, 1);
    if ( ret != AW_OK) {
        return ret;
    }

    AW_BIT_SET(p_this->trigger, cur_id);

    aw_gpio_trigger_on(p_devinfo->trigger_pin);

    return AW_OK;
}

/** \brief �رմ��� */
aw_local aw_err_t __pfn_trigger_off (void *p_drv, int id)
{
    __BMG160_DEV_DECL(p_this, p_drv);
    __BMG160_DEVINFO_DECL(p_devinfo, p_drv);
    int      cur_id  = id - p_devinfo->start_id;
    aw_err_t ret     = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint8_t reg_data = 0;

    if (cur_id < 0 || cur_id > 2) {
        return -AW_ENODEV;
    }

    if (p_drv == NULL) {
        return -AW_EINVAL;
    }

    AW_BIT_CLR(p_this->trigger, cur_id);

    if ((p_this->trigger & 0x7) == 0) {

        reg_data = 0x00;
        ret = __bmg160_write(p_this, __BMG160_INT_EN_0, &reg_data, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        if ((p_devinfo->trigger_pin != -1) && (cur_ret == AW_OK)) {
            ret = aw_gpio_trigger_off(p_devinfo->trigger_pin);
            if (ret != AW_OK) {
                cur_ret = ret;
            }
        }

        if (cur_ret == AW_OK) {
            AW_BIT_CLR(p_this->trigger, 2);
        }
    }

    return cur_ret;
}

/*******************************************************************************
  Public functions
*******************************************************************************/

/**
 * \brief �� BMG160 ����ע�ᵽ AWBus ��ϵͳ��
 */
void awbl_bmg160_drv_register (void)
{
    awbl_drv_register(&__g_bmg160_drvinfo);
}

/* end of file */
