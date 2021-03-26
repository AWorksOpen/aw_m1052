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
 * \brief BME280 压力温湿度传感器驱动
 *
 * \internal
 * \par Modification History
 * - 1.00 18-12-26  ipk, first implementation
 * \endinternal
 */

#include "aw_vdebug.h"
#include "aw_delay.h"
#include "driver/sensor/awbl_bme280.h"

#define       __BME280_HUM_REG       (0XFD) /**< \brief 湿度数据寄存器，16位数据*/
#define       __BME280_TEMP_REG      (0xFA) /**< \brief 温度数据寄存器，20位数据*/
#define       __BME280_PRESS_REG     (0xF7) /**< \brief 压力数据寄存器，20位数据*/

#define       __BME280_CONFIG_REG    (0xF5) /**< \brief 配置寄存器              */
#define       __BME280_CTRL_MEAS_REG (0XF4) /**< \brief 测量控制寄存器          */
#define       __BME280_STATUS_REG    (0XF3) /**< \brief 状态寄存器              */
#define       __BME280_CTRL_HUM_REG  (0XF2) /**< \brief 湿度控制寄存器          */
                                                                                
#define       __BME280_DIG_T1        (0X88) /**< \brief 温度补偿寄存器1         */
#define       __BME280_DIG_T2        (0X8A) /**< \brief 温度补偿寄存器2         */
#define       __BME280_DIG_T3        (0X8C) /**< \brief 温度补偿寄存器3         */
#define       __BME280_DIG_P1        (0X8E) /**< \brief 压力补偿寄存器1         */
#define       __BME280_DIG_P2        (0X90) /**< \brief 压力补偿寄存器2         */
#define       __BME280_DIG_P3        (0X92) /**< \brief 压力补偿寄存器3         */
#define       __BME280_DIG_P4        (0X94) /**< \brief 压力补偿寄存器4         */
#define       __BME280_DIG_P5        (0X96) /**< \brief 压力补偿寄存器5         */
#define       __BME280_DIG_P6        (0X98) /**< \brief 压力补偿寄存器6         */
#define       __BME280_DIG_P7        (0X9A) /**< \brief 压力补偿寄存器7         */
#define       __BME280_DIG_P8        (0X9C) /**< \brief 压力补偿寄存器8         */
#define       __BME280_DIG_P9        (0X9E) /**< \brief 压力补偿寄存器9         */
#define       __BME280_DIG_H1        (0XA1) /**< \brief 湿度补偿寄存器1         */
#define       __BME280_DIG_H2        (0XE1) /**< \brief 湿度补偿寄存器2         */
#define       __BME280_DIG_H3        (0XE3) /**< \brief 湿度补偿寄存器3         */
#define       __BME280_DIG_H4        (0XE4) /**< \brief 湿度补偿寄存器4         */
#define       __BME280_DIG_H5        (0XE5) /**< \brief 湿度补偿寄存器5         */
#define       __BME280_DIG_H6        (0XE7) /**< \brief 湿度补偿寄存器6         */

#define       __BME280_RESET_REG     (0XE0) /**< \brief 复位寄存器              */
#define       __BME280_ID_REG        (0XD0) /**< \brief ID寄存器                */

#define       __BME280_ID            (0x60) /**< \brief 传感器ID(固定值)        */

/**
 * \brief BME280测量频率
 */
typedef enum bme280_rate {
    BME280_RATE_2000_HZ = 0,                /**< \brief 频率2000HZ              */
    BME280_RATE_16_HZ,                      /**< \brief 频率16HZ                */
    BME280_RATE_8_HZ,                       /**< \brief 频率8HZ                 */
    BME280_RATE_4_HZ,                       /**< \brief 频率4HZ                 */
    BME280_RATE_2_HZ,                       /**< \brief 频率2HZ                 */
    BME280_RATE_1_HZ,                       /**< \brief 频率1HZ                 */
    BME280_RATE_100_HZ,                     /**< \brief 频率100HZ               */
    BME280_RATE_50_HZ                       /**< \brief 频率50HZ                */
} bme280_rate_t;

/**
 * \brief 传感器服务获取方法
 */
aw_local aw_err_t __bme280_sensorserv_get (struct awbl_dev *p_dev,
                                           void            *p_arg);

/** \brief 第二次初始化传感器服务获取方法 */
aw_local void __bme280_inst_init2 (struct awbl_dev *p_dev);

/** \brief 第三阶段初始化函数 */
aw_local void __bme280_inst_connect (struct awbl_dev *p_dev);

/**
 * \brief BME280 写数据
 */
aw_static_inline aw_err_t __bme280_write (awbl_bme280_dev_t *p_this,
                                          uint32_t           subaddr,
                                          uint8_t           *p_buf,
                                          uint32_t           nbytes);

/**
 * \brief BME280 读数据
 */
aw_static_inline aw_err_t __bme280_read (awbl_bme280_dev_t *p_this,
                                         uint32_t           subaddr,
                                         uint8_t           *p_buf,
                                         uint32_t           nbytes);

/**
 * \brief BME280 连续读两字节
 */
aw_static_inline aw_err_t __bme280_read_2bytes (awbl_bme280_dev_t *p_this,
                                                uint8_t            subaddr,
                                                uint16_t          *p_dat);

/**
 * \brief BME280 采样率转化
 */
aw_static_inline bme280_rate_t __rate_change (uint16_t rate);

/**
 * \brief BME280 滤波参数转化
 */
aw_static_inline awbl_bme280_filter_t __filter_change (uint16_t filter);

/**
 * \brief BME280 过采样比率转化
 */
aw_static_inline awbl_bme280_oversampling_t __oversample_change (uint16_t oversample);

/**
 * \brief 从指定通道获取采样数据
 */
aw_local aw_err_t __bme280_data_get (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf);

/**
 * \brief 传感器属性设置
 */
aw_local aw_err_t __bme280_attr_set (void                  *p_cookie,
                                     int                    id,
                                     int                    attr,
                                     const aw_sensor_val_t *p_val);

/**
 * \brief 传感器属性获取
 */
aw_local aw_err_t __bme280_attr_get (void              *p_cookie,
                                     int                id,
                                     int                attr,
                                     aw_sensor_val_t   *p_val);

/*******************************************************************************
  本地宏定义
*******************************************************************************/
/** \brief 不为AW_OK则返回错误值 */
#define __BME280_EXIT(ret)      \
    if (ret != AW_OK) {         \
        return ret;             \
    }

/** \brief 通过 AWBus 设备，获取BME280设备 */
#define __BME280_DEV_DECL(p_this, p_dev)   \
    awbl_bme280_dev_t *p_this = (awbl_bme280_dev_t *)p_dev

/** \brief 通过 AWBus 设备，获取 BME280设备信息 */
#define __BME280_DEVINFO_DECL(p_devinfo, p_dev)    \
    awbl_bme280_devinfo_t *p_devinfo =             \
        (awbl_bme280_devinfo_t *)AWBL_DEVINFO_GET(p_dev)


/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/** \brief 驱动提供的方法 */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

aw_local aw_const struct awbl_dev_method __g_bme280_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __bme280_sensorserv_get),
        AWBL_METHOD_END
};

/** \brief 驱动入口点 */
aw_local aw_const struct awbl_drvfuncs __g_bme280_drvfuncs = {
    NULL,                           /*< \brief 第一阶段初始化    */
    __bme280_inst_init2,            /*< \brief 第二阶段初始化    */
    __bme280_inst_connect           /*< \brief 第三阶段初始化    */
};

/** \brief 驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_bme280 = {
    AWBL_VER_1,                     /*< \brief AWBus 版本号.     */
    AWBL_BUSID_I2C,                 /*< \brief 总线 ID.          */
    AWBL_BME280_NAME,               /*< \brief 驱动名.           */
    &__g_bme280_drvfuncs,           /*< \brief 驱动入口点.       */
    &__g_bme280_dev_methods[0],     /*< \brief 驱动提供的方法.   */
    NULL                            /*< \brief 驱动探测函数.     */
};

/** \brief 服务函数 */
aw_local aw_const struct awbl_sensor_servfuncs __g_bme280_servfuncs = {
    __bme280_data_get,
    NULL,
    NULL,
    __bme280_attr_set,
    __bme280_attr_get,
    NULL,
    NULL,
    NULL
};

/*******************************************************************************
    本地函数定义
*******************************************************************************/
/**
 * \brief 传感器服务获取方法
 */
aw_local aw_err_t __bme280_sensorserv_get (struct awbl_dev *p_dev,
                                           void            *p_arg)
{
    __BME280_DEV_DECL(p_this, p_dev);
    __BME280_DEVINFO_DECL(p_devinfo, p_dev);

    awbl_sensor_service_t *p_serv = &p_this->sensor_serv;

    /** \brief 该传感器的通道分配 */
    aw_local aw_const awbl_sensor_type_info_t type_info[3] = {
        {AW_SENSOR_TYPE_PRESS, 1},
        {AW_SENSOR_TYPE_TEMPERATURE, 1},
        {AW_SENSOR_TYPE_HUMIDITY, 1},
    };

    /* 传感器服务常量信息*/
    aw_local aw_const awbl_sensor_servinfo_t serv_info = {
        3,                      /* 支持的通道数量 */
        type_info,              /* 传感器类型列表 */
        AW_NELEMENTS(type_info) /* 类型列表数目   */
    };

    p_serv->p_cookie    = p_this;
    p_serv->p_next      = NULL;
    p_serv->p_servfuncs = &__g_bme280_servfuncs;
    p_serv->p_servinfo  = &serv_info;
    p_serv->start_id    = p_devinfo->start_id;

    *(awbl_sensor_service_t **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
/** \brief 第二次初始化传感器服务获取方法 */
aw_local void __bme280_inst_init2 (struct awbl_dev *p_dev)
{

    __BME280_DEV_DECL(p_this, p_dev);
    __BME280_DEVINFO_DECL(p_devinfo, p_dev);

    aw_i2c_mkdev(&p_this->i2c_dev,
                 p_dev->p_devhcf->bus_index,
                 p_devinfo->i2c_addr,
                 AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);

    p_this->rate = BME280_RATE_100_HZ;
}

/** \brief 第三阶段初始化函数 */
aw_local void __bme280_inst_connect (struct awbl_dev *p_dev)
{
    uint8_t    bme280_id;
    uint8_t    dat;
    uint16_t   h4, h5;
    uint8_t    status = 1;
    aw_err_t   ret;

    __BME280_DEV_DECL(p_this, p_dev);
    __BME280_DEVINFO_DECL(p_devinfo, p_dev);

    /* 复位传感器 */
    dat = 0xb6;
    ret = __bme280_write(p_this, __BME280_RESET_REG, &dat, 1);
    aw_mdelay(5);

    /* 等待 NVM数据复制完成*/
    while (AW_BIT_GET(status, 0)) {
        ret = __bme280_read(p_this, __BME280_STATUS_REG, &status, 1);
        if (ret != AW_OK) {
            aw_kprintf("\r\nBME280 I2C read error %d !\r\n", ret);
            return;
        }
    }

    /* 读补偿数据 */
    if (__bme280_read_2bytes(p_this, __BME280_DIG_T1,
                            (uint16_t *)&p_this->compensation.dig_t1) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_T2,
                            (uint16_t *)&p_this->compensation.dig_t2) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_T3,
                            (uint16_t *)&p_this->compensation.dig_t3) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_P1,
                            (uint16_t *)&p_this->compensation.dig_p1) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_P2,
                            (uint16_t *)&p_this->compensation.dig_p2) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_P3,
                            (uint16_t *)&p_this->compensation.dig_p3) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_P4,
                            (uint16_t *)&p_this->compensation.dig_p4) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_P5,
                            (uint16_t *)&p_this->compensation.dig_p5) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_P6,
                            (uint16_t *)&p_this->compensation.dig_p6) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_P7,
                            (uint16_t *)&p_this->compensation.dig_p7) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_P8,
                            (uint16_t *)&p_this->compensation.dig_p8) ||
        __bme280_read_2bytes(p_this, __BME280_DIG_P9,
                            (uint16_t *)&p_this->compensation.dig_p9) ||
        __bme280_read(p_this, __BME280_DIG_H1,
                      (uint8_t *)&p_this->compensation.dig_h1, 1)     ||
        __bme280_read_2bytes(p_this, __BME280_DIG_H2,
                            (uint16_t *)&p_this->compensation.dig_h2) ||
        __bme280_read(p_this, __BME280_DIG_H3,
                     (uint8_t *)&p_this->compensation.dig_h3, 1)      ||
        __bme280_read_2bytes(p_this, __BME280_DIG_H4, &h4)            ||
        __bme280_read_2bytes(p_this, __BME280_DIG_H5, &h5)            ||
        __bme280_read(p_this, __BME280_DIG_H6,
                     (uint8_t *)&p_this->compensation.dig_h6, 1) != AW_OK) {

        aw_kprintf("BME280 read compensation error !\r\n");

    }
    p_this->compensation.dig_h4 = (h4 & 0x00ff) << 4 | (h4 & 0x0f00) >> 8;
    p_this->compensation.dig_h5 = h5 >> 4;

    /* 设置采样率和滤波参数 */
    dat = (p_this->rate << 5) |
          (__filter_change(p_devinfo->filter_coeff) << 2);
    __bme280_write(p_this, __BME280_CONFIG_REG, &dat, 1);

    /* 设置湿度的过采样比率 */
    dat = __oversample_change(p_devinfo->hum_oversampling);
    __bme280_write(p_this, __BME280_CTRL_HUM_REG, &dat, 1);

    if (p_devinfo->mode != AWBL_BME280_SLEEP_MODE       &&
        p_devinfo->mode != AWBL_BME280_FORCED_MODE      &&
        p_devinfo->mode != AWBL_BME280_FORCED_MODE_ALSO &&
        p_devinfo->mode != AWBL_BME280_NORMAL_MODE) {
        aw_kprintf("BME280 not support mode!\r\n");
    }

    /* 设置温度和起气压的过采样比率，以及模式 */
    dat = (__oversample_change(p_devinfo->temp_oversampling) << 5)  |
          (__oversample_change(p_devinfo->press_oversampling) << 2) |
          p_devinfo->mode;
    __bme280_write(p_this, __BME280_CTRL_MEAS_REG, &dat, 1);

    /* 读ID */
    __bme280_read(p_this, __BME280_ID_REG, &bme280_id, 1);

    if (bme280_id != __BME280_ID) {
        aw_kprintf("BME280 sensor init error !\r\n");
    }
}

/******************************************************************************/

/*
 * \brief BME280 写数据
 */
aw_static_inline aw_err_t __bme280_write (awbl_bme280_dev_t *p_this,
                                          uint32_t           subaddr,
                                          uint8_t           *p_buf,
                                          uint32_t           nbytes)
{
    return aw_i2c_write(&p_this->i2c_dev,
                        subaddr,
                        p_buf,
                        nbytes);
}

/*
 * \brief BME280 读数据
 */
aw_static_inline aw_err_t __bme280_read (awbl_bme280_dev_t *p_this,
                                         uint32_t           subaddr,
                                         uint8_t           *p_buf,
                                         uint32_t           nbytes)
{
    return aw_i2c_read(&p_this->i2c_dev,
                       subaddr,
                       p_buf,
                       nbytes);
}

/******************************************************************************/

/*
 * \brief BME280 连续读两字节
 */
aw_static_inline aw_err_t __bme280_read_2bytes (awbl_bme280_dev_t *p_this,
                                                uint8_t            subaddr,
                                                uint16_t          *p_dat)
{
    aw_err_t ret;
    uint8_t  rx_buf[2] = {0};

    ret = __bme280_read(p_this, subaddr, rx_buf, 2);
    __BME280_EXIT(ret);

    *p_dat = (uint16_t)rx_buf[1] << 8 | rx_buf[0];

    return AW_OK;
}

/*
 * \brief BME280 采样率转化
 */
aw_static_inline bme280_rate_t __rate_change (uint16_t rate)
{
    bme280_rate_t rate_cfg;
    if (rate <= 1) {
        rate_cfg = BME280_RATE_1_HZ;
    } else if (rate > 1 && rate <= 3) {
        rate_cfg = BME280_RATE_2_HZ;
    } else if (rate > 3 && rate <= 6) {
        rate_cfg = BME280_RATE_4_HZ;
    } else if (rate > 6 && rate <= 12) {
        rate_cfg = BME280_RATE_8_HZ;
    } else if (rate > 12 && rate <= 24) {
        rate_cfg = BME280_RATE_16_HZ;
    } else if (rate > 24 && rate <= 75) {
        rate_cfg = BME280_RATE_50_HZ;
    } else if (rate > 75 && rate <= 500) {
        rate_cfg = BME280_RATE_100_HZ;
    } else {
        rate_cfg = BME280_RATE_2000_HZ;
    }

    return rate_cfg;
}

/*
 * \brief BME280 滤波参数转化
 */
aw_static_inline awbl_bme280_filter_t __filter_change (uint16_t filter)
{
    awbl_bme280_filter_t filter_coeff;

    if (filter == 0) {
        filter_coeff = AWBL_BME280_FILTER_OFF;
    } else if (filter >= 1 && filter <= 3) {
        filter_coeff = AWBL_BME280_FILTER_COEFFICIENT_2;
    } else if (filter > 3 && filter <= 6) {
        filter_coeff = AWBL_BME280_FILTER_COEFFICIENT_4;
    } else if (filter > 6 && filter <= 12) {
        filter_coeff = AWBL_BME280_FILTER_COEFFICIENT_8;
    } else {
        filter_coeff = AWBL_BME280_FILTER_COEFFICIENT_16;
    }

    return filter_coeff;
}

/*
 * \brief BME280 过采样比率转化
 */
aw_static_inline awbl_bme280_oversampling_t __oversample_change (uint16_t oversample)
{
    awbl_bme280_oversampling_t oversample_cfg;

    if (oversample == 0) {
        oversample_cfg = AWBL_BME280_OVERSAMPLE_SKIP;
    } else if (oversample == 1) {
        oversample_cfg = AWBL_BME280_OVERSAMPLE_X_1;
    } else if (oversample > 1 && oversample <= 3) {
        oversample_cfg = AWBL_BME280_OVERSAMPLE_X_2;
    } else if (oversample > 3 && oversample <= 6) {
        oversample_cfg = AWBL_BME280_OVERSAMPLE_X_4;
    } else if (oversample > 6 && oversample <= 12) {
        oversample_cfg = AWBL_BME280_OVERSAMPLE_X_8;
    } else {
        oversample_cfg = AWBL_BME280_OVERSAMPLE_X_16;
    }

    return oversample_cfg;
}

/*
 * \brief BME280 温度补偿
 */
aw_static_inline int32_t __bme280_temp_compensate (awbl_bme280_dev_t *p_this,
                                                   int32_t            adc_temp)
{
    int32_t var1, var2;

    var1 = ((((adc_temp >> 3) - ((int32_t) p_this->compensation.dig_t1 << 1)))
            * (int32_t) p_this->compensation.dig_t2) >> 11;

    var2 = (((((adc_temp >> 4) - (int32_t) p_this->compensation.dig_t1)
            * ((adc_temp >> 4) - (int32_t) p_this->compensation.dig_t1)) >> 12)
            * (int32_t) p_this->compensation.dig_t3) >> 14;

    p_this->fine_temp = var1 + var2;

    return ((p_this->fine_temp * 5 + 128) >> 8);
}

/*
 * \brief BME280气压补偿
 */
aw_static_inline uint32_t __bm3280_press_compensate (awbl_bme280_dev_t *p_this,
                                                     int32_t            adc_press)
{
    int64_t var1, var2, p;

    var1 = (int64_t) p_this->fine_temp - 128000;

    var2 = var1 * var1 * (int64_t) p_this->compensation.dig_p6;

    var2 = var2 + ((var1 * (int64_t) p_this->compensation.dig_p5) << 17);

    var2 = var2 + (((int64_t) p_this->compensation.dig_p4) << 35);

    var1 = ((var1 * var1 * (int64_t) p_this->compensation.dig_p3) >> 8)
            + ((var1 * (int64_t) p_this->compensation.dig_p2) << 12);

    var1 = (((int64_t) 1 << 47) + var1) * ((int64_t) p_this->compensation.dig_p1) >> 33;

    if (var1 == 0) {
        return 0;     /* avoid exception caused by division by zero */
    }

    p = 1048576 - adc_press;
    p = (((p << 31) - var2) * 3125) / var1;

    var1 = ((int64_t) p_this->compensation.dig_p9 * (p >> 13) * (p >> 13)) >> 25;

    var2 = ((int64_t) p_this->compensation.dig_p8 * p) >> 19;

    p = ((p + var1 + var2) >> 8) + ((int64_t) p_this->compensation.dig_p7 << 4);

    return p;
}

/*
 * \brief BME280湿度数据补偿
 */
aw_static_inline uint32_t __bme280_hum_compensate (awbl_bme280_dev_t *p_this,
                                                   int32_t            adc_hum)
{
    int32_t v_x1_u32r;

    v_x1_u32r = p_this->fine_temp - (int32_t) 76800;

    v_x1_u32r = ((((adc_hum << 14) - ((int32_t) p_this->compensation.dig_h4 << 20)
                - ((int32_t) p_this->compensation.dig_h5 * v_x1_u32r)) + (int32_t) 16384) >> 15)
                * (((((((v_x1_u32r * (int32_t) p_this->compensation.dig_h6) >> 10)
                    * (((v_x1_u32r * (int32_t) p_this->compensation.dig_h3) >> 11)
                            + (int32_t) 32768)) >> 10) + (int32_t) 2097152)
                    * (int32_t) p_this->compensation.dig_h2 + 8192) >> 14);

    v_x1_u32r = v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7)
                * (int32_t) p_this->compensation.dig_h1) >> 4);

    v_x1_u32r = v_x1_u32r < 0 ? 0 : v_x1_u32r;

    v_x1_u32r = v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r;

    return (v_x1_u32r >> 12);
}

/******************************************************************************/
/**
 * \brief 从指定通道获取采样数据
 */
aw_local aw_err_t __bme280_data_get (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf)
{
    __BME280_DEV_DECL(p_this, p_cookie);
    __BME280_DEVINFO_DECL(p_devinfo, p_cookie);

    int      i;
    uint8_t  read_data[8] = {0};
    uint32_t tmp_dat;
    aw_err_t ret;
    int      cur_id;

    /* 如果模式设置为强制模式，则每次需要先进入强制模式再读取寄存器数据 */
    if (p_devinfo->mode == AWBL_BME280_FORCED_MODE     ||
        p_devinfo->mode == AWBL_BME280_FORCED_MODE_ALSO) {

        uint8_t dat;
        uint8_t status = 0xff;

        /* 设置温度和起气压的过采样比率，以及模式 */
        dat = (__oversample_change(p_devinfo->temp_oversampling) << 5)  |
              (__oversample_change(p_devinfo->press_oversampling) << 2) |
              p_devinfo->mode;
        __bme280_write(p_this, __BME280_CTRL_MEAS_REG, &dat, 1);

        /* 等待测量结果转化完成 */
        while (AW_BIT_GET(status, 3)) {
            ret = __bme280_read(p_this, __BME280_STATUS_REG, &status, 1);
            __BME280_EXIT(ret);
        }
    }

    ret = __bme280_read(p_this, __BME280_PRESS_REG, read_data, 8);
    __BME280_EXIT(ret);

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        switch (cur_id) {

            case 0: {

                tmp_dat = (uint32_t)read_data[0] << 12 |
                          (uint32_t)read_data[1] << 4  |
                          (uint32_t)read_data[2] >> 4;

                /* 气压数据扩大1000倍 */
                p_buf[i].val  = (uint64_t)__bm3280_press_compensate(p_this, tmp_dat)
                                 * 1000 / 256;
                p_buf[i].unit = AW_SENSOR_UNIT_MILLI;

                break;
            }
            case 1: {

                tmp_dat = (uint32_t)read_data[3] << 12 |
                          (uint32_t)read_data[4] << 4  |
                          (uint32_t)read_data[5] >> 4;

                /* 温度数据已经扩大100倍，再扩大10倍为1000倍 */
                p_buf[i].val  = __bme280_temp_compensate(p_this, tmp_dat) * 10;
                p_buf[i].unit = AW_SENSOR_UNIT_MILLI;

                break;
            }
            case 2: {

                tmp_dat = (uint16_t)read_data[6] << 8 |
                          (uint16_t)read_data[7];

                /* 湿度数据扩大1000倍 */
                p_buf[i].val  = __bme280_hum_compensate(p_this, tmp_dat)
                                * 1000 / 1024;
                p_buf[i].unit = AW_SENSOR_UNIT_MILLI;

                break;
            }
            default:
                break;
        }

    }
    return AW_OK;
}

/**
 * \brief 传感器属性设置
 */
aw_local aw_err_t __bme280_attr_set (void                  *p_cookie,
                                     int                    id,
                                     int                    attr,
                                     const aw_sensor_val_t *p_val)
{
    uint8_t         dat;
    aw_err_t        ret;
    aw_sensor_val_t val;

    __BME280_DEV_DECL(p_this, p_cookie);
    __BME280_DEVINFO_DECL(p_devinfo, p_cookie);

    if (attr != AW_SENSOR_ATTR_SAMPLING_RATE) {
        return -AW_ENOTSUP;
    }
    if (id < p_devinfo->start_id || id > p_devinfo->start_id + 2) {
        return -AW_ENODEV;
    }

    val.val  = p_val->val;
    val.unit = p_val->unit;

    aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_BASE);

    /* 设置采样率需要先进入睡眠模式 */
    dat = AWBL_BME280_SLEEP_MODE;
    ret = __bme280_write(p_this, __BME280_CTRL_MEAS_REG, &dat, 1);
    __BME280_EXIT(ret)

    p_this->rate = __rate_change(val.val);

    /* 设置采样率和默认的滤波参数 */
    dat = (p_this->rate << 5) |
          (__filter_change(p_devinfo->filter_coeff) << 2);
    __bme280_write(p_this, __BME280_CONFIG_REG, &dat, 1);

    /* 设置采样率完成后进入默认配置的模式 */
    dat = (__oversample_change(p_devinfo->temp_oversampling) << 5)  |
          (__oversample_change(p_devinfo->press_oversampling) << 2) |
          p_devinfo->mode;

    return __bme280_write(p_this, __BME280_CTRL_MEAS_REG, &dat, 1);
}

/**
 * \brief 传感器属性获取
 */
aw_local aw_err_t __bme280_attr_get (void              *p_cookie,
                                     int                id,
                                     int                attr,
                                     aw_sensor_val_t   *p_val)
{
    __BME280_DEV_DECL(p_this, p_cookie);
    __BME280_DEVINFO_DECL(p_devinfo, p_cookie);

    if (attr != AW_SENSOR_ATTR_SAMPLING_RATE) {
        return -AW_ENOTSUP;
    }
    if (id < p_devinfo->start_id || id > p_devinfo->start_id + 2) {
        return -AW_ENODEV;
    }

    if (p_this->rate == BME280_RATE_2000_HZ) {
        p_val->val = 2000;
    } else if (p_this->rate == BME280_RATE_16_HZ) {
        p_val->val = 16;
    } else if (p_this->rate == BME280_RATE_8_HZ) {
        p_val->val = 8;
    } else if (p_this->rate == BME280_RATE_4_HZ) {
        p_val->val = 4;
    } else if (p_this->rate == BME280_RATE_2_HZ) {
        p_val->val = 2;
    } else if (p_this->rate == BME280_RATE_1_HZ) {
        p_val->val = 1;
    } else if (p_this->rate == BME280_RATE_100_HZ) {
        p_val->val = 100;
    } else if (p_this->rate == BME280_RATE_50_HZ) {
        p_val->val = 50;
    }
    p_val->unit = AW_SENSOR_UNIT_BASE;

    return AW_OK;
}

/*******************************************************************************
  extern functions
*******************************************************************************/
/**
 * \brief 将 BME280 驱动注册到 AWBus 子系统中
 */
void awbl_bme280_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_bme280);
}


/* end of file */

