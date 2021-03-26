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


/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "awbus_lite.h"

#include "awbl_plb.h"
#include "aw_vdebug.h"
#include "aw_gpio.h"
#include "aw_delay.h"
#include "aw_assert.h"
#include "aw_isr_defer.h"
#include "math.h"

#include "driver/sensor/awbl_bma253.h"

/*******************************************************************************
  �궨��
*******************************************************************************/
/**< \brief ͨ�� AWBus�豸  ���岢��ʼ��  BMA253 �豸 */
#define BMA253_DEV_DECL(p_this, p_dev) \
        awbl_sensor_bma253_dev_t *p_this = ( awbl_sensor_bma253_dev_t *)(p_dev)

/**< \brief ͨ�� AWBus�豸 ���岢��ʼ��  BMA253 �豸��Ϣ */
#define BMA253_DEVINFO_DECL(p_devinfo, p_dev)                  \
         awbl_sensor_bma253_devinfo_t *p_devinfo =             \
        (awbl_sensor_bma253_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

/**< brief ��������ֵ��� */
#define BMA253_CHECK_RET(ret) do { \
        if (ret != AW_OK) {        \
            return ret;            \
        }                          \
    } while (0);

#define BMA253_ENABLE             (1)    /**< \brief ʹ�� */
#define BMA253_DISABLE            (0)    /**< \brief ʧ�� */

#define BMA253_ACCEL_X            (0)    /**< \brief ���ٶ�ͨ��x */
#define BMA253_ACCEL_Y            (1)    /**< \brief ���ٶ�ͨ��y */
#define BMA253_ACCEL_Z            (2)    /**< \brief ���ٶ�ͨ��z */
#define BMA253_TEMPERATURE        (3)    /**< \brief �¶�����ͨ�� */
#define BMA253_ACCEL_MASK         (0x07) /**< \brief ���ٶ�ͨ������ */

#define BMA253_INT_NEW_READY      (1)    /**< \brief �����ݾ����ж� */
#define BMA253_INT_THRESHOLD      (2)    /**< \brief ���޴����ж� */

#define BMA253_REG_ACCEL_ADDR     (0x02)    /**< \brief ���ٶȼĴ����ӵ�ַ */

#define BMA253_REG_TEMP_ADDR      (0x08)    /**< \brief �¶����ݼĴ����ӵ�ַ */

#define BMA253_REG_INT_STATU1     (0x09)    /**< \brief �����ж�״̬�Ĵ���1��ַ */
#define BMA253_REG_INT_STATU2     (0x0c)    /**< \brief �����ж�״̬�Ĵ���2��ַ */

#define BMA253_REG_RANGE_ADDR     (0x0f)    /**< \brief ������Χ�Ĵ����ӵ�ַ */

#define BMA253_REG_BW_ADDR        (0x10)    /**< \brief �˲�������Ĵ����ӵ�ַ */

#define BMA253_REG_INT_EN_ADDR    (0x17)    /**< \brief �ж�ʹ�ܼĴ����ӵ�ַ */

#define BMA253_REG_INT_ADDR1      (0x19)    /**< \brief �����жϼĴ�����ַ1 */
#define BMA253_REG_INT_ADDR2      (0x1a)    /**< \brief �����жϼĴ�����ַ2 */
#define BMA253_REG_INT_ADDR3      (0x1b)    /**< \brief �����жϼĴ�����ַ3 */

#define BMA253_REG_LOW_TH_ADDR    (0x23)    /**< \brief �����޼Ĵ����ӵ�ַ */
#define BMA253_REG_HIGH_TH_ADDR   (0x26)    /**< \brief �����޼Ĵ����ӵ�ַ */
#define BMA253_LOW_TH_MASK        (0x01)    /**< \brief �����޼Ĵ������� */

#define BMA253_REG_OFFSET_X_ADDR  (0x38)    /**< \brief xͨ��ƫ�ƼĴ����ӵ�ַ */
#define BMA253_REG_OFFSET_Y_ADDR  (0x39)    /**< \brief yͨ��ƫ�ƼĴ����ӵ�ַ */
#define BMA253_REG_OFFSET_Z_ADDR  (0x3a)    /**< \brief zͨ��ƫ�ƼĴ����ӵ�ַ */

#define BMA253_RESET_SOFT         (0x14)    /**< \brief �����λ�Ĵ����ӵ�ַ */
#define BMA253_RESET_INT          (0x21)    /**< \brief ��λ�ж����üĴ����ӵ�ַ */
#define BMA253_RESET_OFFSET       (0x36)    /**< \brief ��λƫ�����Ĵ����ӵ�ַ */
/*******************************************************************************
  ���غ�������
*******************************************************************************/
aw_local void __bma253_inst_init1 (struct awbl_dev *p_dev);
aw_local void __bma253_inst_init2 (struct awbl_dev *p_dev);
aw_local void __bma253_inst_connect (awbl_dev_t *p_dev);
aw_local void __bma253_sensorserv_get (struct awbl_dev *p_dev, void *p_arg);

aw_local aw_err_t __bma253_data_get (void              *p_cookie,
                                     const int         *p_ids,
                                     int                num,
                                     aw_sensor_val_t   *p_buf);

aw_local aw_err_t __bma253_attr_set (void                    *p_cookie,
                                     int                      id,
                                     int                      attr,
                                     const aw_sensor_val_t   *p_val);

aw_local aw_err_t __bma253_attr_get (void                   *p_cookie,
                                     int                     id,
                                     int                     attr,
                                     aw_sensor_val_t        *p_val);

aw_local aw_err_t __bma253_trigger_cfg (void                     *p_cookie,
                                        int                       id,
                                        uint32_t                  flags,
                                        aw_sensor_trigger_cb_t    pfn_cb,
                                        void                     *p_arg);

aw_local aw_err_t __bma253_trigger_on  (void *p_cookie, int id);
aw_local aw_err_t __bma253_trigger_off (void *p_cookie, int id);

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/
awbl_sensor_bma253_dev_t __g_dev;

struct aw_isr_defer_job __int_high_defer_task;     /* �����ж��ӳٴ������� */
struct aw_isr_defer_job __int_new_defer_task;      /* �������ж��ӳٴ������� */

/* ����ֵ��Ӧ�ļĴ���ֵ */
aw_local const uint8_t __bma253_sample_range[4] = {0x03, 0x05, 0x08, 0x0c};

/* ����Ƶ�ʶ�Ӧ��BW�Ĵ�����ֵ */
aw_local const uint8_t __bma253_sample_rate[8] = {0x08, 0x09, 0x0a, 0x0b,
                                                  0x0c, 0x0d, 0x0e, 0x0f};

/** \brief ������ڵ� */
aw_local aw_const struct awbl_drvfuncs __g_bma253_drvfuncs = {
        __bma253_inst_init1,
        __bma253_inst_init2,
        __bma253_inst_connect
};

/** \brief �����ṩ�ķ��� */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);
aw_local aw_const struct awbl_dev_method __g_bma253_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __bma253_sensorserv_get),
        AWBL_METHOD_END
};

/** \brief ����ע����Ϣ */
aw_local aw_const awbl_plb_drvinfo_t __g_bma253_drv_registration = {
    {
        AWBL_VER_1,                    /* AWBus �汾�� */
        AWBL_BUSID_I2C,                /* ���� ID */
        AWBL_BMA253_NAME,              /* ������ */
        &__g_bma253_drvfuncs,          /* ������ڵ� */
        __g_bma253_dev_methods,        /* �����ṩ�ķ��� */
        NULL                           /* ����̽�⺯�� */
    }
};

/** \brief ������ */
aw_local aw_const struct awbl_sensor_servfuncs __g_bma253_servfuncs = {
        __bma253_data_get,             /* ��ָ��ͨ����ȡ�������� */
        NULL,
        NULL,
        __bma253_attr_set,             /* ���ô�����ͨ������ */
        __bma253_attr_get,             /* ��ȡ������ͨ������ */
        __bma253_trigger_cfg,          /* ����ͨ������ */
        __bma253_trigger_on,           /* �򿪴��� */
        __bma253_trigger_off,          /* �رմ��� */
};

/*******************************************************************************
    ���غ�������
*******************************************************************************/
/*
 * \brief bma253 д����
 */
aw_local aw_err_t __bma253_write (awbl_sensor_bma253_dev_t *p_this,
                                  uint8_t                   subaddr,
                                  uint8_t                  *p_buf,
                                  uint32_t                  nbytes)
{
    return aw_i2c_write(&p_this->i2c_dev,
                        subaddr,
                        p_buf,
                        nbytes);
}

/**
 * \brief bma253 ������
 */
aw_local aw_err_t __bma253_read (awbl_sensor_bma253_dev_t *p_this,
                                 uint8_t                   subaddr,
                                 uint8_t                  *p_buf,
                                 uint32_t                  nbytes)
{
    return aw_i2c_read(&p_this->i2c_dev,
                       subaddr,
                       p_buf,
                       nbytes);
}

/**
 * \brief bma253 д�Ĵ�������λ
 */
aw_local aw_err_t __bma253_reg_bit_set (awbl_sensor_bma253_dev_t *p_this,
                                        uint8_t                   subaddr,
                                        uint8_t                   off,
                                        uint8_t                   data)
{
    aw_err_t ret = AW_OK;

    uint8_t reg_data = 0;

    /* ��ȡ�Ĵ������� */
    ret = __bma253_read(p_this, subaddr, &reg_data, 1);
    BMA253_CHECK_RET(ret);

    /* �ж�����λ�������� */
    if (data) {
        reg_data = reg_data | (0x01 << off);
    } else {
        reg_data = reg_data & ~(0x01 << off);
    }

    /* д��Ĵ������� */
    ret = __bma253_write(p_this, subaddr, &reg_data, 1);
    BMA253_CHECK_RET(ret);

    return AW_OK;
}

/**
 * \brief ��ȡ�¶�����
 */
aw_local aw_err_t __bma253_temp_get (awbl_sensor_bma253_dev_t *p_this,
                                     uint8_t                  *p_temp)
{
    aw_err_t ret = AW_OK;

    /* ��ȡ���� */
    ret = __bma253_read(p_this, BMA253_REG_TEMP_ADDR, p_temp, 1);
    BMA253_CHECK_RET(ret);

    return AW_OK;
}

/**
 * \brief ��ȡ���ٶ�����
 */
aw_local aw_err_t __bma253_accel_get (awbl_sensor_bma253_dev_t *p_this,
                                      uint16_t                 *p_accel)
{
    uint8_t  accel[6] = {0};

    uint16_t accel_x = 0;
    uint16_t accel_y = 0;
    uint16_t accel_z = 0;

    aw_err_t ret = AW_OK;

    /* ��ȡ���� */
    ret = __bma253_read(p_this, BMA253_REG_ACCEL_ADDR, accel, 6);
    BMA253_CHECK_RET(ret);

    accel_x = (uint16_t)(accel[1] << 4) + (accel[0] >> 4);
    accel_y = (uint16_t)(accel[3] << 4) + (accel[2] >> 4);
    accel_z = (uint16_t)(accel[5] << 4) + (accel[4] >> 4);

    p_accel[0] = accel_x;
    p_accel[1] = accel_y;
    p_accel[2] = accel_z;

    return AW_OK;
}

/**
 * \brief ���������ݾ�������
 */
aw_local aw_err_t __bma253_new_data_int_set(awbl_sensor_bma253_dev_t *p_this,
                                            uint8_t                   status)
{
    aw_err_t ret = AW_OK;

    switch (status) {
    /* ���������ݾ������� */
    case BMA253_ENABLE:
        /* �������ݴ�����int2�ܽ� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR2,   7, 1);
        BMA253_CHECK_RET(ret);

        /* ʹ���������ж� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, 4, 1);
        BMA253_CHECK_RET(ret);
        break;

    /* ȡ�������ݾ������� */
    case BMA253_DISABLE:
        /* ȡ�������ݾ����жϰ� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR2,   7, 0);
        BMA253_CHECK_RET(ret);

        /* ʧ�������ݾ����ж� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, 4, 0);
        BMA253_CHECK_RET(ret);
        break;

    default:
        break;
    }

    return AW_OK;
}

/**
 * \brief ���������޴���
 */
aw_local aw_err_t __bma253_high_trigger_int_set(awbl_sensor_bma253_dev_t *p_this,
                                                uint8_t                   id,
                                                uint8_t                   status)
{
    aw_err_t ret = AW_OK;

    switch (status) {
    /* �������޴��� */
    case BMA253_ENABLE:
        /* �������޴�����int1�ܽ� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR1, 1, 1);
        BMA253_CHECK_RET(ret);

        /* ʹ���������ж� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, id, 1);
        BMA253_CHECK_RET(ret);
        break;

    /* ȡ�����޴������� */
    case BMA253_DISABLE:
        /* ȡ�������޴����� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR1, 1, 0);
        BMA253_CHECK_RET(ret);

        /* ʧ���������ж� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, id, 0);
        BMA253_CHECK_RET(ret);
        break;

    default:
        break;
    }

    return AW_OK;
}

/**
 * \brief ���������޴���
 */
aw_local aw_err_t __bma253_low_trigger_int_set(awbl_sensor_bma253_dev_t *p_this,
                                               uint8_t                   status)
{
    aw_err_t ret = AW_OK;

    switch (status) {
    /* ���������޴��� */
    case BMA253_ENABLE:
        /* �������޴�����int1�ܽ� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR1, 0, 1);
        BMA253_CHECK_RET(ret);

        /* ʹ���������ж� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, 3,  1);
        BMA253_CHECK_RET(ret);
        break;

    /* ȡ�������޴������� */
    case BMA253_DISABLE:
        /* ȡ�������޴����� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR1, 0, 0);
        BMA253_CHECK_RET(ret);

        /* ʧ���������ж� */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, 3,  0);
        BMA253_CHECK_RET(ret);
        break;

    default:
        break;
    }

    return AW_OK;
}

/**
 * \brief ���޴������жϷ�����
 */
aw_local void __bma253_int1_isr (void * p_arg)
{
    aw_isr_defer_job_add(&__int_high_defer_task);  /* ����ж��ӳٴ������� */
}

/**
 * \brief �����ݾ����������жϷ�����
 */
aw_local void __bma253_int2_isr (void * p_arg)
{
    aw_isr_defer_job_add(&__int_new_defer_task);  /* ����ж��ӳٴ������� */
}

/**
 * \brief ���޴����ж���ʱ��������
 */
static void defer_gate_trigger_job_entry (void *p_arg)
{
    awbl_sensor_bma253_dev_t *p_this = ( awbl_sensor_bma253_dev_t *)p_arg;

    uint8_t  trig_chnl = 0;
    uint8_t  trig_src  = 0;

    /* ��ȡ�ж�״̬�����жϴ���Դ */
    __bma253_read(p_this, BMA253_REG_INT_STATU1, &trig_src, 1);

    /* ��ȡ�ж�״̬�����ж������޴���ͨ�� */
    __bma253_read(p_this, BMA253_REG_INT_STATU2, &trig_chnl, 1);

    /* �����޴��� */
    if (trig_src & BMA253_LOW_TH_MASK) {

        p_this->pfn_low_trigger_cb(p_this->p_arg[3],            \
                                   AW_SENSOR_TRIGGER_THRESHOLD);
    } else {  /* �����޴��� */

        /* �жϴ���Դ */
        trig_chnl &= BMA253_ACCEL_MASK;
        switch (trig_chnl) {

        /* x�ᴥ�������� */
        case (0x01 << BMA253_ACCEL_X):
            if (p_this->high_trigger_set[0]) {
                p_this->pfn_accel_trigger_cb[0](p_this->p_arg[0],            \
                                                AW_SENSOR_TRIGGER_THRESHOLD);
            }
            break;

        /* y�ᴥ�������� */
        case (0x01 << BMA253_ACCEL_Y):
            if (p_this->high_trigger_set[1]) {
                p_this->pfn_accel_trigger_cb[1](p_this->p_arg[1],            \
                                                AW_SENSOR_TRIGGER_THRESHOLD);
            }
            break;

        /* z�ᴥ�������� */
        case (0x01 << BMA253_ACCEL_Z):
            if (p_this->high_trigger_set[2]) {
                p_this->pfn_accel_trigger_cb[2](p_this->p_arg[2],            \
                                                AW_SENSOR_TRIGGER_THRESHOLD);
            }
            break;

        default:
            break;
        }
    }
}

/**
 * \brief �����ݴ����ж���ʱ��������
 */
static void defer_new_trigger_job_entry (void *p_arg) {

    BMA253_DEV_DECL(p_this, p_arg);
    BMA253_DEVINFO_DECL(p_devinfo, p_arg);

//    __bma253_new_data_int_set(p_this, BMA253_DISABLE);

    aw_gpio_trigger_off(p_devinfo->int1_port);

    p_this->pfn_accel_trigger_cb[2](p_this->p_arg[2],             \
                                   AW_SENSOR_TRIGGER_DATA_READY);

    aw_gpio_trigger_cfg(p_devinfo->int1_port, AW_GPIO_TRIGGER_RISE);
    aw_gpio_trigger_on(p_devinfo->int1_port);
//    __bma253_new_data_int_set(p_this, BMA253_ENABLE);
}

/*******************************************************************************

*******************************************************************************/
/**
 * \brief ��һ�׶γ�ʼ��
 */
aw_local void __bma253_inst_init1 (struct awbl_dev *p_dev)
{
    ;
}

/**
 * \brief �ڶ��׶γ�ʼ��
 */
aw_local void __bma253_inst_init2 (struct awbl_dev *p_dev)
{
    ;
}

/**
 * \brief �����׶γ�ʼ��
 */
aw_local void __bma253_inst_connect (awbl_dev_t *p_dev)
{
    BMA253_DEV_DECL(p_this, p_dev);
    BMA253_DEVINFO_DECL(p_devinfo, p_dev);

    uint8_t reg_data = 0;

    /* ƽ̨��ʼ�� */
    p_devinfo->pfunc_plfm_init();

    /* �����ӻ��豸 */
    aw_i2c_mkdev(&p_this->i2c_dev,
                 p_dev->p_devhcf->bus_index,
                 p_devinfo->addr,
                (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));

    /* �ϵ縴λ */
    reg_data = 0xb6;
    __bma253_write(p_this, BMA253_RESET_SOFT, &reg_data, 1); /* �����λ�û����� */

    __bma253_reg_bit_set(p_this, BMA253_RESET_INT,    7, 0); /* ����κ�������ж� */
    __bma253_reg_bit_set(p_this, BMA253_RESET_OFFSET, 7, 0); /* ���ƫ�����Ĵ���ֵ */

    /* ��ʼ������ */
    p_this->bma253_val.accel_x = 0;
    p_this->bma253_val.accel_y = 0;
    p_this->bma253_val.accel_z = 0;
    p_this->bma253_val.temp    = 0;
    p_this->accel_z_int[0]     = 0;    /* ͨ��z�������ݴ��� */
    p_this->accel_z_int[1]     = 0;    /* ͨ��z�������޴��� */

    /* ������Ĭ��ֵ */
    p_this->sample_rate  = AW_BMA253_2000_HZ;
    p_this->sample_range = AW_BMA253_2G;

    p_this->high_th      = 0xc0;  /* 192 * 7.81mg = 1.4995g �� 1.5g */

    /* ����һ�������豸��Ϣ */
    __g_dev = (*p_this);
}

/**
 * \brief ����
 */
aw_local void __bma253_sensorserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    BMA253_DEV_DECL(p_this, p_dev);
    BMA253_DEVINFO_DECL(p_devinfo, p_dev);

    awbl_sensor_service_t *p_sensor_serv = NULL;

    /*
     *  �ô�������ͨ������
     */
    aw_local const awbl_sensor_type_info_t type_info[] = {
         {AW_SENSOR_TYPE_ACCELEROMETER, 3},    /* 3·���ٶ�  */
         {AW_SENSOR_TYPE_TEMPERATURE, 1}       /* 1·�¶� */
    };

    aw_local const awbl_sensor_servinfo_t serv_info = {
         4,                                   /* ��֧��4��ͨ��  */
         type_info,
         AW_NELEMENTS(type_info)              /* �����б���Ŀ    */
    };

    p_sensor_serv                  = &p_this->sensor_serv;
    p_sensor_serv->p_cookie        = p_dev;
    p_sensor_serv->p_next          = NULL;
    p_sensor_serv->p_servfuncs     = &__g_bma253_servfuncs;
    p_sensor_serv->p_servinfo      = &serv_info;
    p_sensor_serv->start_id        = p_devinfo->start_id;

    *((awbl_sensor_service_t **)p_arg) = p_sensor_serv;
}

/**
 * \brief ��ȡָ��ͨ���Ĳ�������
 */
aw_local aw_err_t __bma253_data_get (void              *p_cookie,
                                     const int         *p_ids,
                                     int                num,
                                     aw_sensor_val_t   *p_buf)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);
    aw_err_t ret      = AW_OK;

    uint8_t  i        =  0;
    int8_t   chnl     =  0;
    uint8_t  temp     =  0;     /* �¶�Ϊ8bit���� */
    uint16_t accel[3] = {0};    /* ���ٶ�Ϊ12bit���� */
    uint16_t unit[4]  = {976, 1953, 3906, 7812};
    uint32_t cur_id   =  0;
    uint32_t start_id = p_devinfo->start_id;

    /* ��ȡ���ٶ� */
    ret = __bma253_accel_get(p_this, accel);
    BMA253_CHECK_RET(ret);

    /* ��ȡ�¶� */
    ret = __bma253_temp_get(p_this, &temp);
    BMA253_CHECK_RET(ret);

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i];
        chnl    = cur_id - start_id;
        if (chnl >= 0 && chnl < 3) {

            p_buf[i].val  = accel[chnl];          /* ��ȡ���ٶ�ͨ�� */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO; /* ��λ10^-6 */

            /* ������ݸ�ʽת�� */
            (p_buf[i].val & (1 << 11)) ? p_buf[i].val |=
                              (~0xFFF) : p_buf[i].val;
            p_buf[i].val  = ~p_buf[i].val;
            p_buf[i].val *= unit[p_this->sample_range];  /* ��2g��ΧӦ����976ug */

        } else if ( chnl == 3) {

            p_buf[i].val  = (int8_t)temp;        /* ��ȡ�¶�ͨ�� */
            p_buf[i].unit = AW_SENSOR_UNIT_DECI; /* ��λ10^-1 */

            p_buf[i].val  = p_buf[i].val * 5 + 230;

        } else {
            return AW_OK;                /* ���˴�ͨ�������ڸô�������ֱ���˳� */
        }
    }

    return AW_OK;
}

/**
 * \brief ���ô�����ͨ������
 */
aw_local aw_err_t __bma253_attr_set (void                    *p_cookie,
                                     int                      id,
                                     int                      attr,
                                     const aw_sensor_val_t   *p_val)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);

    aw_err_t ret = AW_OK;

    int8_t  chnl     = id - p_devinfo->start_id; /* ��ȡ��ǰͨ���� */
    uint8_t freq     = 0;
    uint8_t range    = 0;
    int8_t  offset   = 0;
    uint8_t offset_w = 0;
    uint8_t high_th  = 0;
    uint8_t bma253_reg_addr = 0;
    uint16_t unit[4] = {7812, 15624, 31248, 62496};
    double  data_float = 0.0;

    switch (attr) {

    /* ���ò���Ƶ�� */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        if (!(chnl >= 0 && chnl < 3)) {
            return -AW_ENODEV; /* ͨ����֧�� */
        }

        data_float = (double)p_val->val;
        data_float = data_float * pow(10, p_val->unit);

        if ((int)data_float >= 0 && (int)data_float <= 7) {
            freq = __bma253_sample_rate[(uint8_t)data_float];

            /* д�����Ƶ������ */
            ret = __bma253_write(p_this, BMA253_REG_BW_ADDR, &freq, 1);
            BMA253_CHECK_RET(ret);

            /* д���豸ʵ�� */
            p_this->sample_rate = (uint8_t)data_float;

        } else {

            return -AW_EINVAL;  /* ��Ч�Ĳ��� */
        }
        break;

    /* ��������ֵ */
    case AW_SENSOR_ATTR_FULL_SCALE:
        if (!(chnl >= 0 && chnl < 3)) {
            return -AW_ENODEV; /* ͨ����֧�� */
        }
        if (p_val->val >= 0 && p_val->val <= 3) {
            range = __bma253_sample_range[(uint8_t)p_val->val];

            /* д�������Χ���� */
            ret = __bma253_write(p_this, BMA253_REG_RANGE_ADDR, &range, 1);
            BMA253_CHECK_RET(ret);

            /* д���豸ʵ�� */
            p_this->sample_range = (uint8_t)p_val->val;

        } else {

            return -AW_EINVAL;  /* ��Ч�Ĳ��� */
        }
        break;

    /* ����ƫ���� */
    case AW_SENSOR_ATTR_OFFSET:
        switch (chnl) {
        /* ͨ��x */
        case BMA253_ACCEL_X:
            bma253_reg_addr = BMA253_REG_OFFSET_X_ADDR;
            break;
        /* ͨ��y */
        case BMA253_ACCEL_Y:
            bma253_reg_addr = BMA253_REG_OFFSET_Y_ADDR;
            break;
        /* ͨ��z */
        case BMA253_ACCEL_Z:
            bma253_reg_addr = BMA253_REG_OFFSET_Z_ADDR;
            break;

        default:
            return -AW_ENOTSUP; /* ��֧�� */
        }

        data_float = (double)p_val->val;
        data_float = data_float * pow(10, p_val->unit);
        data_float = data_float * pow(10, 6);
        data_float = data_float / 7800;

        offset   = (int8_t)data_float;
        offset_w = (uint8_t)offset;

        /* д��ͨ������ֵ */
        ret = __bma253_write(p_this, bma253_reg_addr, &offset_w, 1);
        BMA253_CHECK_RET(ret);

        /* д���豸ʵ�� */
        p_this->offset[chnl] = (uint32_t)offset;

        break;

    /* ����������ֵ���������޴���ģʽ */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        return -AW_ENODEV;  /* ��֧�� */

    /* ����������ֵ���������޴���ģʽ */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:

        data_float = (double)p_val->val;
        data_float = data_float * pow(10, p_val->unit);
        data_float = data_float * pow(10, 6);
        data_float = data_float / unit[p_this->sample_range];

        high_th = (uint8_t)data_float;

        /* д��Ĵ������� */
        ret = __bma253_write(p_this, BMA253_REG_HIGH_TH_ADDR, &high_th, 1);
        BMA253_CHECK_RET(ret);
        break;

    /* �������ݳ����������������޴���ģʽ*/
    case AW_SENSOR_ATTR_DURATION_DATA:
        return -AW_ENODEV;  /* ��֧�� */

    default:
        break;
    }

    return AW_OK;
}

/**
 * \brief ��ȡ������ͨ������
 */
aw_local aw_err_t __bma253_attr_get (void                   *p_cookie,
                                     int                     id,
                                     int                     attr,
                                     aw_sensor_val_t        *p_val)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);

    /* ��ȡ��ǰͨ���� */
    int8_t chnl = id - p_devinfo->start_id;

    /* �¶�ͨ������������ */
    if (chnl == BMA253_TEMPERATURE) {
        return -AW_ENOTSUP;
    }

    switch (attr) {
    /* ��ȡ����Ƶ�� */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        p_val->val  = p_this->sample_rate;
        p_val->unit = 0;
        break;

    /* ��ȡ�������� */
    case AW_SENSOR_ATTR_FULL_SCALE:
        p_val->val  = p_this->sample_range;
        p_val->unit = 0;
        break;

    /* ��ȡͨ�������� */
    case AW_SENSOR_ATTR_OFFSET:
        p_val->val  = p_this->offset[chnl];
        p_val->unit = 0;
        break;

    /* ������ֵ���������޴���ģʽ */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        p_val->val  = p_this->low_th;
        p_val->unit = 0;
        break;

    /* ������ֵ���������޴���ģʽ */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        p_val->val  = p_this->high_th;
        p_val->unit = 0;
        break;

    /* �������ݳ����������������޴���ģʽ*/
    case AW_SENSOR_ATTR_DURATION_DATA:
        return -AW_ENOTSUP;  /* ��֧�� */

    default:
        break;
    }

    return AW_OK;
}

/**
 * \brief ���ô�����һ��ͨ����������һ�������ص�����
 */
aw_local aw_err_t  __bma253_trigger_cfg (void                     *p_cookie,
                                         int                       id,
                                         uint32_t                  flags,
                                         aw_sensor_trigger_cb_t    pfn_cb,
                                         void                     *p_arg)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);

    /* ��ȡ��ǰͨ���� */
    int8_t chnl = id - p_devinfo->start_id;

    switch(flags) {

    /* �����ݾ������� */
    case AW_SENSOR_TRIGGER_DATA_READY:
        if (chnl != BMA253_ACCEL_Z) {
            return -AW_ENOTSUP;
        }
        p_this->pfn_accel_trigger_cb[chnl] = pfn_cb;
        p_this->p_arg[chnl]                = p_arg;
        p_this->accel_z_int[0]             = BMA253_ENABLE;
        break;

    /* ���޴��� */
    case AW_SENSOR_TRIGGER_THRESHOLD:
        if (!(chnl >= 0 && chnl < 3)) {
            return -AW_ENODEV; /* ͨ����֧�� */
        }
        p_this->pfn_accel_trigger_cb[chnl] = pfn_cb;
        p_this->p_arg[chnl]                = p_arg;

        if (chnl == BMA253_ACCEL_Z) {
            p_this->accel_z_int[1]         = BMA253_ENABLE;
        }
        break;

    default:
        return -AW_ENOTSUP;
        break;
    }

    return AW_OK;
}

/**
 * \brief �򿪴���
 */
aw_local aw_err_t __bma253_trigger_on (void *p_cookie, int id)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);

    /* ��ȡ��ǰͨ���� */
    uint8_t chnl = id - p_devinfo->start_id;

    switch (chnl) {

    case BMA253_ACCEL_X:
    case BMA253_ACCEL_Y:
        p_this->high_trigger_set[chnl] = BMA253_ENABLE;

        aw_isr_defer_job_init(&__int_high_defer_task, \
                               defer_gate_trigger_job_entry, p_this);
        /* ���ӻص����������� */
        aw_gpio_trigger_connect(p_devinfo->int1_port, __bma253_int1_isr, NULL);
        /* �������Ŵ������ͣ������ش��� */
        aw_gpio_trigger_cfg(p_devinfo->int1_port, AW_GPIO_TRIGGER_RISE);
        /* ���������ϵĴ����� */
        aw_gpio_trigger_on(p_devinfo->int1_port);

        /* ���ô������������ж� */
        __bma253_high_trigger_int_set(p_this, chnl, BMA253_ENABLE);
        break;

    case BMA253_ACCEL_Z:
        if (p_this->accel_z_int[0] == BMA253_ENABLE) { /* �����ݾ������� */

            aw_isr_defer_job_init(&__int_new_defer_task, \
                                   defer_new_trigger_job_entry, p_cookie);
            /* ���ӻص����������� */
            aw_gpio_trigger_connect(p_devinfo->int2_port, \
                                    __bma253_int2_isr, NULL);
            /* �������Ŵ������ͣ������ش��� */
            aw_gpio_trigger_cfg(p_devinfo->int2_port, AW_GPIO_TRIGGER_RISE);
            /* ���������ϵĴ����� */
            aw_gpio_trigger_on(p_devinfo->int2_port);

            /* ���ô����������ݴ����ж� */
            __bma253_new_data_int_set(p_this, BMA253_ENABLE);

        } else if (p_this->accel_z_int[1] == BMA253_ENABLE) { /* ���޴��� */

            p_this->high_trigger_set[chnl] = BMA253_ENABLE;

            aw_isr_defer_job_init(&__int_high_defer_task, \
                                   defer_gate_trigger_job_entry, p_this);
            /* ���ӻص����������� */
            aw_gpio_trigger_connect(p_devinfo->int1_port, \
                                    __bma253_int1_isr, NULL);
            /* �������Ŵ������ͣ�˫���ش��� */
            aw_gpio_trigger_cfg(p_devinfo->int1_port, AW_GPIO_TRIGGER_RISE);
            /* ���������ϵĴ����� */
            aw_gpio_trigger_on(p_devinfo->int1_port);

            /* ���ô������������ж� */
            __bma253_high_trigger_int_set(p_this, chnl, BMA253_ENABLE);
        }
        break;

    default:
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

/**
 * \brief �رմ���
 */
aw_local aw_err_t __bma253_trigger_off (void *p_cookie, int id)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);

    /* ��ȡ��ǰͨ���� */
    uint8_t chnl = id - p_devinfo->start_id;

    switch (chnl) {

    case BMA253_ACCEL_X:
    case BMA253_ACCEL_Y:
        /* ʧ�ܶ�Ӧͨ�������޴��� */
        p_this->high_trigger_set[chnl] = BMA253_DISABLE;

        break;

    case BMA253_ACCEL_Z:
        if (p_this->accel_z_int[0] == BMA253_ENABLE) {        /* �ر������ݴ��� */

            p_this->accel_z_int[0] = BMA253_DISABLE;
            /* �ر������ϵĴ����� */
            aw_gpio_trigger_off(p_devinfo->int2_port);
            /* �����������ж� */
            __bma253_new_data_int_set(p_this, BMA253_DISABLE);

        } else if (p_this->accel_z_int[1] == BMA253_ENABLE) { /* �ر����޴��� */

            p_this->accel_z_int[1] = BMA253_DISABLE;
            p_this->high_trigger_set[chnl] = BMA253_DISABLE;
        }
        break;

    default:
        return -AW_ENOTSUP;
    }

    /* �������ͨ���������޴�����ʧ�ܣ���ر��ж� */
    if (p_this->high_trigger_set[0] + \
        p_this->high_trigger_set[1] + \
        p_this->high_trigger_set[2] == 0) {

        /* �رմ������������ж� */
        __bma253_high_trigger_int_set(p_this, chnl, BMA253_DISABLE);

        if (p_this->low_trigger_set == BMA253_DISABLE) {
            /* �ر������ϵĴ����� */
            aw_gpio_trigger_off(p_devinfo->int1_port);
        }
    }

    return AW_OK;
}
/*******************************************************************************
    �ⲿ��������
*******************************************************************************/
/**
 * \brief ���� BMA253 �������޴���
 */
aw_err_t awbl_bma253_low_trigger_cfg (int                      dev_id,
                                      const aw_sensor_val_t   *p_val,
                                      aw_sensor_trigger_cb_t   pfn_cb,
                                      void                    *p_arg)
{
    uint8_t  low_th  = 0;

    uint16_t gate_th = 0;
    uint16_t unit[4] = {7810, 15630, 31250, 62500};

    double low_th_float = 0.0;

    aw_err_t ret = AW_OK;

    if (dev_id != 0) {

        return -AW_EINVAL;  /* ��������ȷ */
    }

    /* ����������ֵ */
    gate_th = (uint16_t)p_val->val;

    low_th_float = (double)gate_th;
    low_th_float = low_th_float * pow(10, p_val->unit);
    low_th_float = low_th_float * pow(10, 6);
    low_th_float = low_th_float / unit[__g_dev.sample_range];

    low_th = (uint8_t)low_th_float;

    /* д������������ */
    ret = __bma253_write(&__g_dev, BMA253_REG_LOW_TH_ADDR, &low_th, 1);
    BMA253_CHECK_RET(ret);

    /* ����ص���Ϣ */
    __g_dev.pfn_low_trigger_cb = pfn_cb;
    __g_dev.p_arg[3]           = p_arg;

    return AW_OK;
}

/**
 * \brief BMA253 �����޴��� ����
 */
aw_err_t awbl_bma253_low_trigger_on (int dev_id)
{
    BMA253_DEVINFO_DECL(p_devinfo, &__g_dev);

    if (dev_id != 0) {

        return -AW_EINVAL;  /* ��������ȷ */
    }

    __g_dev.low_trigger_set = BMA253_ENABLE;

    /* ���ô������������ж� */
    __bma253_low_trigger_int_set(&__g_dev, BMA253_ENABLE);

    aw_isr_defer_job_init(&__int_high_defer_task, \
                           defer_gate_trigger_job_entry, &__g_dev);
    /* ���ӻص����������� */
    aw_gpio_trigger_connect(p_devinfo->int1_port, __bma253_int1_isr, NULL);
    /* �������Ŵ������ͣ������ش��� */
    aw_gpio_trigger_cfg(p_devinfo->int1_port, AW_GPIO_TRIGGER_RISE);
    /* ���������ϵĴ����� */
    aw_gpio_trigger_on(p_devinfo->int1_port);

    return AW_OK;
}

/**
 * \brief BMA253 �����޴��� �ر�
 */
aw_err_t awbl_bma253_low_trigger_off (int dev_id)
{
    BMA253_DEVINFO_DECL(p_devinfo, &__g_dev);

    if (dev_id != 0) {

        return -AW_EINVAL;  /* ��������ȷ */
    }

    __g_dev.low_trigger_set = BMA253_DISABLE;

    /* ���ô������������ж� */
    __bma253_low_trigger_int_set(&__g_dev, BMA253_DISABLE);

    /* �������ͨ���������޴�����ʧ�ܣ���ر��ж� */
    if (__g_dev.high_trigger_set[0] + \
        __g_dev.high_trigger_set[1] + \
        __g_dev.high_trigger_set[2] == 0) {

        /* �ر������ϵĴ����� */
        aw_gpio_trigger_off(p_devinfo->int1_port);
    }

    return AW_OK;
}

/**
 * \brief �� BMA253 ����ע�ᵽ AWBus ��ϵͳ��
 */
void awbl_bma253_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_bma253_drv_registration);
}

/* end of file */
