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
 * \brief TPS02R ����
 *
 * \internal
 * \par Modification History
 * - 1.00 18-05-11  vir, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "driver/sensor/awbl_tps02r.h"
#include "aw_gpio.h"

/*******************************************************************************
      �궨��
*******************************************************************************/

/** \brief ͨ��ID ���������豸  */
#define __AWBL_DEV_DECL_BY_ID(p_dev,id)  \
              awbl_dev_t *p_dev = awbl_dev_find_by_name(AWBL_TPS02R_NAME,id)

/** \brief ͨ�� AWBus�豸  ���岢��ʼ��  TPS02R �豸 */
#define __TPS02R_DEV_DECL(p_this, p_dev)          \
       awbl_tps02r_dev_t *p_this = ( awbl_tps02r_dev_t *)(p_dev)

/** \brief  ͨ�� AWBus�豸 ���岢��ʼ��  TPS02R �豸��Ϣ */
#define __TPS02R_DEVINFO_DECL(p_devinfo, p_dev)  \
     awbl_tps02r_devinfo_t *p_devinfo =          \
        (awbl_tps02r_devinfo_t *)AWBL_DEVINFO_GET(p_dev)


/** \brief  �ж������� */
#define __TPS02R_UART_SPIN_LOCK_ISR_TAKE(p_chan)    \
    aw_spinlock_isr_take(&p_chan->spinlock_isr)

#define __TPS02R_UART_SPIN_LOCK_ISR_GIVE(p_chan)    \
    aw_spinlock_isr_give(&p_chan->spinlock_isr)


#define __TPS02R_REG_TEMP_ADDR     (0X00)    /*< \brief �¶ȼĴ����ӵ�ַ */
#define __TPS02R_REG_TEMP_LEN      (0X06)    /*< \brief �¶ȼĴ��� ��С*/

#define __TPS02R_REG_CFG_ADDR      (0X01)    /*< \brief ���üĴ����ӵ�ַ */
#define __TPS02R_REG_CFG_LEN       (0X02)    /*< \brief ���üĴ��� ��С*/

#define __TPS02R_REG_TLOW_ADDR     (0X02)    /*< \brief �¶����޼Ĵ����ӵ�ַ */
#define __TPS02R_REG_TLOW_LEN      (0X06)    /*< \brief �¶����޼Ĵ��� ��С*/

#define __TPS02R_REG_THIGH_ADDR    (0X03)    /*< \brief �¶����޼Ĵ����ӵ�ַ */
#define __TPS02R_REG_THIGH_LEN     (0X06)    /*< \brief �¶����޼Ĵ��� ��С*/


#define __TPS02R_CFG_MODE_MASK     (0X06)     /*< \brief ���ñ���ģʽ���� */

#define __TPS02R_CFG_NUM_MASK      (0X18)     /*< \brief ���ô��������������� */
#define __TPS02R_CFG_NUM_SHIFT     (0X03)     /*< \brief ���ô�������������λ*/

#define __TPS02R_CFG_RATE_MASK     (0X20)     /*< \brief ���ò����������� */
#define __TPS02R_CFG_RATE_SHIFT    (0X05)     /*< \brief ���ò���������λ*/

#define __TPS02R_CFG_ALERT_MASK    (0X40)     /*< \brief ����״̬���� */
#define __TPS02R_CFG_ALERT_SHIFT   (0X06)     /*< \brief ����״̬��λ*/

#define __TPS02R_CFG_EN_MASK       (0X80)     /*< \brief ʹ���������� */
#define __TPS02R_CFG_EN_SHIFT      (0X07)     /*< \brief ʹ��������λ*/


/*
 * \brief TPS02R �������ܵ�����ģʽ
 */

#define  __TPS02R_TM        (0X02)   /*< \brief �����ź�״̬���� */
#define  __TPS02R_POL       (0X04)   /*< \brief �����������״̬  */

/**
 * \name grp_tps02r_alarm_mode
 */
/* \brief ģʽ0,�����ź�����Ƚ�ģʽ���������ſ�������ߵ�ƽ */
#define  __TPS02R_ALARM_MODE_0    ( 0 | 0 )

/* \brief ģʽ1,�����ź�����ж�ģʽ���������ſ�������ߵ�ƽ */
#define  __TPS02R_ALARM_MODE_1    ( 0 | __TPS02R_TM )

/* \brief ģʽ2,�����ź�����Ƚ�ģʽ���������ſ�������͵�ƽ */
#define  __TPS02R_ALARM_MODE_2    ( __TPS02R_TM | 0 )

/* \brief ģʽ3,�����ź�����ж�ģʽ���������ſ�������͵�ƽ */
#define  __TPS02R_ALARM_MODE_3    ( __TPS02R_TM | __TPS02R_POL )

/*
 * \name grp_tps02r_sample_rate
 */
#define  __TPS02R_SAMPLE_RATE_10HZ  (0X00)  /*< \brief ����������Ϊ10HZ*/
#define  __TPS02R_SAMPLE_RATE_40HZ  (0X01)  /*< \brief ����������Ϊ40HZ*/

/*******************************************************************************
  ���غ�������
*******************************************************************************/
aw_local void __tps02r_inst_init1 (awbl_dev_t *p_dev);
aw_local void __tps02r_inst_init2 (awbl_dev_t *p_dev);
aw_local void __tps02r_inst_connect (awbl_dev_t *p_dev);
aw_local void __tps02r_sensorserv_get(struct awbl_dev *p_dev, void *p_arg);

aw_local aw_err_t __tps02r_data_get(void                     *p_cookie,
                                    const int                *p_ids,
                                    int                       num,
                                    aw_sensor_val_t          *p_buf);

aw_local aw_err_t __tps02r_attr_set(void                    *p_cookie,
                                    int                      id,
                                    int                      attr,
                                    const aw_sensor_val_t   *p_val);

aw_local aw_err_t __tps02r_attr_get (void                   *p_cookie,
                                     int                     id,
                                     int                     attr,
                                     aw_sensor_val_t        *p_val);
aw_local aw_err_t __tps02r_trigger_cfg (void                     *p_cookie,
                                        int                       id,
                                        uint32_t                  flags,
                                        aw_sensor_trigger_cb_t    pfn_cb,
                                        void                     *p_arg);

aw_local aw_err_t __tps02r_trigger_on (void *p_cookie, int id);
aw_local aw_err_t __tps02r_trigger_off(void *p_cookie, int id);

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/
/** \brief ������ڵ� */
aw_local aw_const struct awbl_drvfuncs __g_tps02r_drvfuncs = {
        __tps02r_inst_init1,                   /* ��һ�׶γ�ʼ�� */
        __tps02r_inst_init2,                   /* �ڶ��׶γ�ʼ�� */
        __tps02r_inst_connect                  /* �����׶γ�ʼ�� */
};

/** \brief �����ṩ�ķ��� */

AWBL_METHOD_IMPORT(awbl_sensorserv_get);
aw_local aw_const struct awbl_dev_method __g_tps02r_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __tps02r_sensorserv_get),
        AWBL_METHOD_END
};

/** \brief ����ע����Ϣ */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_tps02r = {
    AWBL_VER_1,                   /* AWBus �汾�� */
    AWBL_BUSID_I2C,               /* ���� ID */
    AWBL_TPS02R_NAME,             /* ������ */
    &__g_tps02r_drvfuncs,         /* ������ڵ� */
    __g_tps02r_dev_methods,       /* �����ṩ�ķ��� */
    NULL                          /* ����̽�⺯�� */
};

/** \brief ������ */
aw_local aw_const struct awbl_sensor_servfuncs __g_tps02r_servfuncs = {
        __tps02r_data_get,
        NULL,
        NULL,
        __tps02r_attr_set,
        __tps02r_attr_get,
        __tps02r_trigger_cfg,
        __tps02r_trigger_on,
        __tps02r_trigger_off,
};

/*******************************************************************************
    ���غ�������
*******************************************************************************/
aw_local uint32_t __pow10 (int n)
{
    /* 32λ������ʾ�� 4 294 967 296�� ��෵�أ�10^9 ���� ��1 000 000 000 */
    aw_local const uint32_t pow10[] = {
        1UL,                  /* 0 */
        10UL,                 /* 1 */
        100UL,                /* 2 */
        1000UL,               /* 3 */
        10000UL,              /* 4 */
        100000UL,             /* 5 */
        1000000UL,            /* 6 */
        10000000UL,           /* 7 */
        100000000UL,          /* 8 */
        1000000000UL,         /* 9 */
    };

    if ((n >= 0) && (n <= 9)) {
        return pow10[n];
    }

    return 0;
}

/*
 * \brief tps02r ������
 */
aw_local aw_err_t __tps02r_write(awbl_tps02r_dev_t *p_this,
                                uint8_t            subaddr,
                                uint8_t           *p_buf,
                                uint32_t           nbytes)
{
    return aw_i2c_write(&p_this->i2c_dev,
                        subaddr,
                        p_buf,
                        nbytes);

}

/**
 * \brief tps02r д����
 */
aw_local aw_err_t __tps02r_read(awbl_tps02r_dev_t *p_this,
                                uint8_t            subaddr,
                                uint8_t           *p_buf,
                                uint32_t           nbytes)
{
    return aw_i2c_read(&p_this->i2c_dev,
                        subaddr,
                        p_buf,
                        nbytes);

}

/*
 * \brief ��ȡͨ�����¶�
 *
 * \note Ĭ��ֱ�Ӷ�ȡ����ͨ�����¶� ������ p_temp Ϊ ָ������Ԫ��������׵�ַ
 */
aw_local aw_err_t __tps02r_temp_get(awbl_tps02r_dev_t *p_this, int32_t *p_code)
{
    aw_err_t         i    = 0;
    uint32_t code[2] = {0,0};
    uint8_t  dat[6]  = {0};
    int      ret     = AW_OK;

    /* ��ȡ���� */
    ret = __tps02r_read(p_this, 
                        __TPS02R_REG_TEMP_ADDR, 
                        dat,
                        __TPS02R_REG_TEMP_LEN);
    if( ret != AW_OK) {
        return ret;
    }

    for ( i = 0; i < 2; i++) {
        /* ͨ�� �¶�����ֵ */
        code[i] = 0;
        code[i] |= dat[i*3]     << 16;
        code[i] |= dat[i*3 + 1] << 8;
        code[i] |= dat[i*3 + 2];

        /*  �� 1<<13  <==> �Ŵ� 8192 �� */
        if (code[i] >= 8388608) {
            p_code[i] = (int32_t)(-( ((int64_t)(16777216 - code[i])) \
                                        * 1000000 / (0x1 << 13)));
        } else {
            p_code[i]  = (int32_t)( ((int64_t)code[i]) * 1000000 / (0x1 << 13));
        }
    }

    return AW_OK;
}

/**
 * \brief ��������ֵ
 */
aw_local aw_err_t __tps02r_tlow_set (awbl_tps02r_dev_t *p_this, 
                                     uint8_t            chan, 
                                     int32_t            temp)
{
    aw_err_t      ret   = AW_OK;
    uint8_t       dat[6] = {0};
    uint32_t      code   = 0;
    /* ��ȡ���� */
    ret = __tps02r_read(p_this, 
                        __TPS02R_REG_TLOW_ADDR, 
                        dat,
                        __TPS02R_REG_TLOW_LEN);
    if( AW_OK != ret ) {
        return ret;
    }

    if( temp < 0 ) {
        temp = -temp;
        code = 16777216 - temp;    /*  ��Ų��� */
    } else {
        code = temp;
    }

    if (chan == 0) {

        dat[0] = (code >> 16) & 0xff;
        dat[1] = (code >> 8)  & 0xff;
        dat[2] = (code >> 0)  & 0xff;

    } else if( chan == 1 ) {

        dat[3] = (code >> 16) & 0xff;
        dat[4] = (code >> 8)  & 0xff;
        dat[5] = (code >> 0)  & 0xff;
    }

    return __tps02r_write(p_this,
                          __TPS02R_REG_TLOW_ADDR, 
                          dat,
                          __TPS02R_REG_TLOW_LEN);

}

/**
 * \brief ��������ֵ
 */
aw_local aw_err_t __tps02r_thigh_set (awbl_tps02r_dev_t *p_this, 
                                      uint8_t            chan, 
                                      int32_t            temp)
{
    aw_err_t ret    = AW_OK;
    uint8_t  dat[6] = {0};
    uint32_t code   = 0;

    /* ��ȡ���� */
    ret = __tps02r_read(p_this, 
                        __TPS02R_REG_THIGH_ADDR, 
                        dat,
                        __TPS02R_REG_THIGH_LEN);
    if( AW_OK != ret ) {
        return ret;
    }

    if( temp < 0 ) {
        temp = -temp;
        code = 16777216 - temp;    /*  ��Ų��� */
    } else {
        code = temp;
    }

    if (chan == 0) {

        dat[0] = (code >> 16) & 0xff;
        dat[1] = (code >> 8)  & 0xff;
        dat[2] = (code >> 0)  & 0xff;

    } else if( chan == 1 ) {

        dat[3] = (code >> 16) & 0xff;
        dat[4] = (code >> 8)  & 0xff;
        dat[5] = (code >> 0)  & 0xff;
    }

    return __tps02r_write(p_this,
                          __TPS02R_REG_THIGH_ADDR, 
                          dat,
                          __TPS02R_REG_THIGH_LEN);

}

/*
 * \brief ���ô���������Ҫ�Ĵ���
 */
aw_local aw_err_t __tps02r_trigger_num_set(awbl_tps02r_dev_t *p_this, 
                                           uint8_t            chan, 
                                           uint8_t            num)
{
    aw_err_t  ret   = AW_OK;
     uint8_t cfg[2] = {0};

     ret = __tps02r_read(p_this,
                         __TPS02R_REG_CFG_ADDR,
                        &cfg[0], 
                         __TPS02R_REG_CFG_LEN);
     if( ret != AW_OK) {
         return ret;
     }

     cfg[chan] &=  ~__TPS02R_CFG_NUM_MASK;
     cfg[chan] |= (num << __TPS02R_CFG_NUM_SHIFT) & __TPS02R_CFG_NUM_MASK;

     return __tps02r_write(p_this,
                           __TPS02R_REG_CFG_ADDR,
                          &cfg[0],
                           __TPS02R_REG_CFG_LEN);

}
/*
 * \brief ����ѡ��
 */
aw_local aw_err_t  __tps02r_cfg_choose(awbl_tps02r_dev_t *p_this ,uint8_t chan)
{
    aw_err_t err;

     uint8_t cfg[2] = {0};

     err = __tps02r_read(p_this,
                         __TPS02R_REG_CFG_ADDR, 
                        &cfg[0],
                         __TPS02R_REG_CFG_LEN);
     if( err != AW_OK) {
         return err;
     }

     if ( chan == 1) {
         cfg[0] |= 1 << __TPS02R_CFG_EN_SHIFT;
         cfg[1] &= ~(1 << __TPS02R_CFG_EN_SHIFT);
     } else {
         cfg[0] &= ~(1 << __TPS02R_CFG_EN_SHIFT);
     }

     return __tps02r_write(p_this,
                           __TPS02R_REG_CFG_ADDR,
                          &cfg[0],
                           __TPS02R_REG_CFG_LEN);
}

/*
 * \brief ����ģʽ
 *
 *  \note mode  �ο�  grp_tps02r_alarm_mode
 */
aw_local aw_err_t  __tps02r_alarm_mode_set(awbl_tps02r_dev_t *p_this, 
                                           uint8_t            chan, 
                                           uint8_t            mode)
{
    aw_err_t err;

     uint8_t cfg[2] = {0};

     err = __tps02r_read(p_this,
                         __TPS02R_REG_CFG_ADDR, 
                        &cfg[0],
                         __TPS02R_REG_CFG_LEN);
     if( err != AW_OK) {
         return err;
     }

     cfg[chan] &= ~__TPS02R_CFG_MODE_MASK;
     cfg[chan] |= mode & __TPS02R_CFG_MODE_MASK;

     return __tps02r_write(p_this,
                           __TPS02R_REG_CFG_ADDR,
                          &cfg[0],
                           __TPS02R_REG_CFG_LEN);
}
/*
 * \brief ���ò�������
 * note rate �ο� grp_tps02r_sample_rate
 */
aw_local aw_err_t  __tps02r_sample_rate_set(awbl_tps02r_dev_t *p_this,
                                            uint8_t            rate)
{

     aw_err_t err;
     uint8_t cfg[2] = {0};

     err = __tps02r_read(p_this,
                         __TPS02R_REG_CFG_ADDR,
                        &cfg[0],
                         __TPS02R_REG_CFG_LEN);
     if( err != AW_OK) {
         return err;
     }

     cfg[0] &=  ~__TPS02R_CFG_RATE_MASK;
     cfg[0] |= (rate << __TPS02R_CFG_RATE_SHIFT) & __TPS02R_CFG_RATE_MASK;

     cfg[1] &=  ~__TPS02R_CFG_RATE_MASK;
     cfg[1] |= (rate << __TPS02R_CFG_RATE_SHIFT) & __TPS02R_CFG_RATE_MASK;

     return __tps02r_write(p_this,
                           __TPS02R_REG_CFG_ADDR,
                          &cfg[0],
                          __TPS02R_REG_CFG_LEN);
}
/*
 * \brief ���������ж�
 */
aw_local void __tps02r_alarm_callback(void *p_arg)
{
    __TPS02R_DEV_DECL(p_this, p_arg);
    __TPS02R_DEVINFO_DECL(p_devinfo, p_arg);

    if(p_this->pfn_trigger_cb) {
        p_this->pfn_trigger_cb(p_this->p_arg, AW_SENSOR_TRIGGER_THRESHOLD);
    }
}

/*******************************************************************************

*******************************************************************************/
/**
 * \brief ��һ�׶γ�ʼ��
 */
aw_local void __tps02r_inst_init1 (awbl_dev_t *p_dev)
{

}

/**
 * \brief �ڶ��׶γ�ʼ��
 */
aw_local void __tps02r_inst_init2 (awbl_dev_t *p_dev)
{

}

/**
 * \brief �����׶γ�ʼ��
 */
aw_local void __tps02r_inst_connect (awbl_dev_t *p_dev)
{
    __TPS02R_DEV_DECL(p_this, p_dev);
    __TPS02R_DEVINFO_DECL(p_devinfo, p_dev);
    int  i = 0;

    aw_i2c_mkdev(&p_this->i2c_dev,
                 p_dev->p_devhcf->bus_index,
                 p_devinfo->i2c_addr,
                 (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));

    for( i = 0; i < 2; i++ ) {
        p_this->count[i]          = 6;
        p_this->temp_high[i].val  = 1024;
        p_this->temp_high[i].unit = AW_SENSOR_UNIT_BASE;
        p_this->temp_low[i].val   = -1024;
        p_this->temp_low[i].unit  = AW_SENSOR_UNIT_BASE;
    }

    p_this->pfn_trigger_cb = NULL;
    p_this->p_arg          = NULL;
    p_this->alert_chan     = 0;
    p_this->sampling_rate  = 1;

    if(p_devinfo->alert_pin != -1) {

        aw_gpio_trigger_cfg(p_devinfo->alert_pin, AW_GPIO_TRIGGER_BOTH_EDGES);
        aw_gpio_trigger_connect(p_devinfo->alert_pin,
                                __tps02r_alarm_callback,
                                p_dev);
    }

}

/**
 * \brief ����
 */

aw_local void __tps02r_sensorserv_get(struct awbl_dev *p_dev, void *p_arg)
{
    __TPS02R_DEV_DECL(p_this, p_dev);
    __TPS02R_DEVINFO_DECL(p_devinfo, p_dev);

    awbl_sensor_service_t *p_sensor_serv = NULL;

    /*
     *  �ô�������ͨ������
     */
    aw_local const awbl_sensor_type_info_t type_info[] = {
         {AW_SENSOR_TYPE_TEMPERATURE, 2},     /* 2·�¶�  */
    };

    aw_local const awbl_sensor_servinfo_t serv_info = {
         2,                                   /* ��֧��2��ͨ��  */
         type_info,
         AW_NELEMENTS(type_info)              /* �����б���Ŀ       */
    };

    p_sensor_serv                  = &p_this->sensor_serv;
    p_sensor_serv->p_cookie        = p_dev;
    p_sensor_serv->p_next          = NULL;
    p_sensor_serv->p_servfuncs     = &__g_tps02r_servfuncs;
    p_sensor_serv->p_servinfo      = &serv_info;
    p_sensor_serv->start_id        = p_devinfo->start_id;

    *((awbl_sensor_service_t **)p_arg) = p_sensor_serv;
}
/*
 * \brief ���ݻ�ȡ
 */
aw_local aw_err_t __tps02r_data_get(void                     *p_cookie,
                                    const int                *p_ids,
                                    int                       num,
                                    aw_sensor_val_t          *p_buf)
{
    __TPS02R_DEV_DECL(p_this, p_cookie);
    __TPS02R_DEVINFO_DECL(p_devinfo, p_cookie);

    int       i  = 0;
    aw_err_t ret = AW_OK;

    int32_t    temp[2]  = {0.0,0.0};
    uint32_t   cur_id   = 0;

    int  start_id =  p_devinfo->start_id;
    int  off     = 0;

    /* ��ȡ�¶� */
    ret = __tps02r_temp_get(p_this, &temp[0]);
    if( ret != AW_OK) {
        return ret;
    }
    for ( i = 0; i < num; i++ ) {

        cur_id = p_ids[i];
        off    = cur_id - start_id;
        if( off >= 0  && off < 2 ) {

            p_buf[i].val  = temp[off];                /* �¶� */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;     /* ��λĬ��Ϊ 6: 10^(-6)*/

        } else {
            break;   /* ���˴�ͨ�������ڸô�������ֱ���˳�  */
        }
    }

    return AW_OK;;

}

/*
 * \brief ���Ե�����
 */
aw_local aw_err_t __tps02r_attr_set(void                    *p_cookie,
                                    int                      id,
                                    int                      attr,
                                    const aw_sensor_val_t   *p_val)
{

    __TPS02R_DEV_DECL(p_this, p_cookie);
    __TPS02R_DEVINFO_DECL(p_devinfo, p_cookie);
    aw_err_t ret = AW_OK;

    int32_t  temp      = 0;
    uint8_t  chan      = 0;
    uint32_t count     = 0;
    uint8_t  count_reg = 0;
    int      n         = 0;
    
    aw_sensor_val_t  sensor_val = {0, 0}; 
    
    chan = id - p_devinfo->start_id;

    switch(attr) {
    /* ��������ֵ */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        if(p_val->unit >= 0) {
            temp = ( int64_t)(p_val->val) * (0x1 << 13) * __pow10(p_val->unit);
        } else {
            n = -p_val->unit;
            temp = ( int64_t)(p_val->val) * (0x1 << 13) / __pow10(n);
        }

        ret = __tps02r_tlow_set(p_this,chan, temp);
        if ( ret == AW_OK) {
            p_this->temp_low[chan].val  = p_val->val;
            p_this->temp_low[chan].unit = p_val->unit;
        }
        break;

    /* ��������ֵ */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        if(p_val->unit >= 0) {
            temp = ( int64_t)(p_val->val) * (0x1 << 13) * __pow10(p_val->unit);
        } else {
            n = - p_val->unit;
            temp = ( int64_t)(p_val->val) * (0x1 << 13) / __pow10(n);
        }
        ret = __tps02r_thigh_set(p_this,chan, temp);
        if ( ret == AW_OK) {
            p_this->temp_high[chan].val  = p_val->val;
            p_this->temp_high[chan].unit = p_val->unit;
        }
        break;

    /* ���ô�����������Ĵ���  */
    case AW_SENSOR_ATTR_DURATION_DATA:

        if(p_val->unit >= 0) {
            count = (p_val->val) * __pow10(p_val->unit);
        } else {
            n = -p_val->unit;
            count = (p_val->val) / __pow10(n);
        }
        /* ֻ֧������ 1�� 2�� 4,6 �� */
        if( count >= 5) {
            count     = 6;
            count_reg = 3;
        } else if ( count >= 3 ){
            count     = 4;
            count_reg = 2;
        } else if( count  == 2) {
            count     = 2;
            count_reg = 1;
        } else {
            count     = 1;
            count_reg = 0;
        }
        ret = __tps02r_trigger_num_set(p_this, chan, count_reg);
        if( ret == AW_OK ) {
            p_this->count[chan] = count;
        }
        break;
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        sensor_val.unit = p_val->unit;
        sensor_val.val  = p_val->val;
        aw_sensor_val_unit_convert(&sensor_val,1,AW_SENSOR_UNIT_BASE);
        if(sensor_val.val > 4) {
            ret = -AW_ENOTSUP;
        } else if( sensor_val.val > 1) {
            /*
             * �ֱ�Ҫ�ɼ�����ͨ�����¶�ֵ��40Hz�£�ÿ��ͨ�����軨125ms��ʱ��,
             * Ҳ����ÿ��������ܲɼ�4��
             */ 
            __tps02r_sample_rate_set(p_this, __TPS02R_SAMPLE_RATE_40HZ);
            p_this->sampling_rate = 4;
        } else {
            /* 
             * �ֱ�Ҫ�ɼ�����ͨ�����¶�ֵ��10Hz�£�ÿ��ͨ�����軨500ms��ʱ��,
             * Ҳ����ÿ��������ܲɼ�1�� 
             */
            __tps02r_sample_rate_set(p_this, __TPS02R_SAMPLE_RATE_10HZ);
            p_this->sampling_rate = 1;
        }
        break;

    default:
        ret = -AW_ENOTSUP;
        break;

    }

    return ret;

}

/*
 * \brief ��ȡ����
 */
aw_local aw_err_t __tps02r_attr_get (void                   *p_cookie,
                                     int                     id,
                                     int                     attr,
                                     aw_sensor_val_t        *p_val)
{
    __TPS02R_DEV_DECL(p_this, p_cookie);
    __TPS02R_DEVINFO_DECL(p_devinfo, p_cookie);
    aw_err_t ret  = AW_OK;
    uint8_t  chan      = 0;

    chan = id - p_devinfo->start_id;

    switch(attr) {
    /* ����ֵ */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        p_val->val = p_this->temp_low[chan].val;
        p_val->unit = p_this->temp_low[chan].unit;
        break;

    /* ����ֵ */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        p_val->val = p_this->temp_high[chan].val;
        p_val->unit = p_this->temp_high[chan].unit;
        break;

    /* ���ô�����������Ĵ���  */
    case AW_SENSOR_ATTR_DURATION_DATA:
        p_val->val  = p_this->count[chan];
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    case AW_SENSOR_ATTR_FULL_SCALE:
        p_val->val = 1024;
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        p_val->val = p_this->sampling_rate;
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    default:
        ret = -AW_ENOTSUP;
        break;

    }

    return ret;
}


/**
 * \brief ��������
 *
 * \note ֻ֧��һ·���������ٴε��ûḲ��֮ǰ���õ�
 */
aw_local aw_err_t __tps02r_trigger_cfg (void                     *p_cookie,
                                        int                       id,
                                        uint32_t                  flags,
                                        aw_sensor_trigger_cb_t    pfn_cb,
                                        void                     *p_arg)
{
    __TPS02R_DEV_DECL(p_this, p_cookie);
    __TPS02R_DEVINFO_DECL(p_devinfo, p_cookie);
    aw_err_t ret  = AW_OK;

    switch(flags) {
    /* ���޴��� */
    case AW_SENSOR_TRIGGER_THRESHOLD:

        if ( id == p_devinfo->start_id) {
            p_this->alert_chan = 0;
        } else {
            p_this->alert_chan = 1;
        }
        __tps02r_cfg_choose(p_this, p_this->alert_chan);

        p_this->pfn_trigger_cb = pfn_cb;
        p_this->p_arg          = p_arg;
        break;
    default:
        ret = -AW_ENOTSUP;
        break;

    }

    return ret;
}

/*
 * \brief �򿪴���
 */
aw_local aw_err_t __tps02r_trigger_on (void *p_cookie, int id)
{
    __TPS02R_DEV_DECL(p_this, p_cookie);
    __TPS02R_DEVINFO_DECL(p_devinfo, p_cookie);

    if((id - p_devinfo->start_id) != p_this->alert_chan ) {
        return -AW_EAGAIN;
    }
    if(p_devinfo->alert_pin != -1) {
        return aw_gpio_trigger_on(p_devinfo->alert_pin);
    } else {
        return -AW_ENOTSUP;
    }

}

/*
 * \brief �رմ���
 */
aw_local aw_err_t __tps02r_trigger_off(void *p_cookie, int id)
{
    __TPS02R_DEV_DECL(p_this, p_cookie);
    __TPS02R_DEVINFO_DECL(p_devinfo, p_cookie);

    if((id - p_devinfo->start_id) != p_this->alert_chan ) {
        return -AW_EAGAIN;
    }

    if(p_devinfo->alert_pin != -1) {
        return aw_gpio_trigger_off(p_devinfo->alert_pin);
    } else {
        return -AW_ENOTSUP;
    }
}

/*******************************************************************************
    �ⲿ��������
*******************************************************************************/
/**
 * \brief �� TPS02R ����ע�ᵽ AWBus ��ϵͳ��
 */
void awbl_tps02r_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_tps02r);
}

/* end of file */
