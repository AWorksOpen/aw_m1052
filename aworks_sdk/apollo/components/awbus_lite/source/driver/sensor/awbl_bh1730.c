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
 ******************************************************************************/
 
 /**
  * \file  awbl_ltr553.c
  * \brief LTR-553ALS-01驱动源码
  *
  * \internal
  * \par Modification History
  * - 1.00 18-11-04  cod, first implementation.
  * \endinternal
 */
 
#include "aworks.h"
#include "awbus_lite.h"
#include "aw_errno.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "aw_assert.h"
#include "aw_gpio.h"
#include "aw_isr_defer.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "driver/sensor/awbl_bh1730.h"

/*******************************************************************************
 宏定义
*******************************************************************************/

/* 使用Isr_Defer在读取I2C数据 */
#define __BH1730_ISR_DEFER

/* 辅助宏，通过基础设备指针获得具体设备指针 */
#define __BH1730_DEV_DECL(p_this,p_dev) struct awbl_bh1730_dev *(p_this) = \
                                        (struct awbl_bh1730_dev *)(p_dev)

/* 辅助宏，通过基础设备指针获得设备信息指针 */
#define __BH1730_DEVINFO_DECL(p_devinfo,p_dev) \
            struct awbl_bh1730_devinfo *(p_devinfo) = \
           (struct awbl_bh1730_devinfo *)AWBL_DEVINFO_GET(p_dev)
           
/* 辅助宏，通过基础设备指针获得传感器服务指针 */
#define __BH1730_SENSORSERV_DECL(p_serv,p_dev) \
            struct awbl_sensor_service_t *(p_serv) = \
           (struct awbl_sensor_service_t *)&((p_dev)->sensor_serv)

/* BH1730寄存器定义 */
#define __BH1730_REG_CONTROL            0X00        /**< \brief 控制寄存器    */
#define __BH1730_VAL_CONTROL_INTR       (1 << 5)    /**< \brief 中断标志      */
#define __BH1730_VAL_CONTROL_VALID      (1 << 4)    /**< \brief 数据已更新    */
#define __BH1730_VAL_CONTROL_ONETIME    (1 << 3)    /**< \brief 更新后继续测量*/
#define __BH1730_VAL_CONTROL_ENABLE     (1 << 1)    /**< \brief ADC测量启动   */
#define __BH1730_VAL_CONTROL_POWER      (1 << 1)    /**< \brief 电源使能      */

#define __BH1730_REG_TIMING             0X01        /**< \brief 测量时间      */

#define __BH1730_REG_INTERRUPT          0X02        /**< \brief 中断控制      */

#define __BH1730_REG_TH_LOW_0           0X03        /**< \brief 低门限值低8位 */
#define __BH1730_REG_TH_LOW_1           0X04        /**< \brief 低门限值高8位 */
#define __BH1730_REG_TH_UP_0            0X05        /**< \brief 高门限值低8位 */
#define __BH1730_REG_TH_UP_1            0X06        /**< \brief 高门限值高8位 */

#define __BH1730_REG_GAIN               0X07        /**< \brief 增益          */

#define __BH1730_REG_PART_ID            0X12        /**< \brief PART ID       */
#define __BH1730_REG_DATA0_0            0X14        /**< \brief DATA0 LSB     */
#define __BH1730_REG_DATA0_1            0X15        /**< \brief DATA0 MSB     */
#define __BH1730_REG_DATA1_0            0X16        /**< \brief DATA1 LSB     */
#define __BH1730_REG_DATA1_1            0X17        /**< \brief DATA1 MSB     */

#define __BH1730_VAL_SPEC_CMD_INT_RST   0X01        /**< \brief  中断输出复位 */
#define __BH1730_VAL_SPEC_CMD_START     0X02        /**< \brief 开始测量      */
#define __BH1730_VAL_SPEC_CMD_STOP      0X03        /**< \brief 停止测量      */
#define __BH1730_VAL_SPEC_CMD_RST       0X04        /**< \brief 软件复位      */

#define __BH1730_VAL_SPEC_INT_RST      (0XE0 | 0X01)/**< \brief 清中断        */
#define __BH1730_VAL_SPEC_MANU_STOP    (0XE0 | 0X02)/**< \brief 手动测量停止  */
#define __BH1730_VAL_SPEC_MANU_START   (0XE0 | 0X03)/**< \brief 手动测量开始  */
#define __BH1730_VAL_SPEC_SOFT_RST     (0XE0 | 0X04)/**< \brief 软件复位      */

/** \brief 辅助宏，将地址转换为寄存器中地址格式 */
#define __BH1730_VAL_CMD(addr)          (0X80 | ((addr) & 0x1f))

/** \brief 始终周期，放大10倍 */
#define __BH1730_TINT_US                28          

/** \brief  ADC转换时间，放大10倍的us */
#define __BH1730_ADC_SAMPLE_TIME_US     (__BH1730_TINT_US * 714)

/** \brief 测量周期的最大最小值 ,单位为pow10(-4)ms */
#define __BH1730_MEAS_TIME_MAX          (__BH1730_TINT_US * 964 * 255 + \
                                        __BH1730_ADC_SAMPLE_TIME_US)
#define __BH1730_MEAS_TIME_MIN          (__BH1730_TINT_US * 964 + \
                                        __BH1730_ADC_SAMPLE_TIME_US)
                                        
/** \brief 寄存器值与窗口取样时间之间的转换 */
#define __BH1730_REG_2_ITIME(reg)       (__BH1730_TINT_US * 964 * (256 - (reg)))

/** \brief 寄存器值与测量总时间之间的转换 */
#define __BH1730_REG_2_MEAS_TIME(reg)   (__BH1730_REG_2_ITIME(reg) + \
                                        __BH1730_ADC_SAMPLE_TIME_US)
                                        
/** \brief 测量总时间与寄存器值之间的转换 */                                        
#define __BH1730_MEAS_TIME_2_REG(x)     (256 - ((x) - __BH1730_ADC_SAMPLE_TIME_US) /\
                                        (__BH1730_TINT_US * 964))


#define __BH1730_MODE_STOP              0X00    /**< \brief 停止模式          */
#define __BH1730_MODE_START             0X01    /**< \brief 开始测量模式      */

/*******************************************************************************
  本地函数声明
*******************************************************************************/
aw_local void __bh1730_inst_connect (awbl_dev_t *p_dev);
aw_local aw_err_t __bh1730_sensorserv_get (struct awbl_dev *p_dev, void *p_arg);
aw_local aw_err_t __bh1730_enable (void            *p_cookie,
                                   const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_result);
aw_local aw_err_t __bh1730_disable (void            *p_cookie,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_result);
aw_local aw_err_t __bh1730_data_get (void                     *p_cookie,
                                     const int                *p_ids,
                                     int                       num,
                                     aw_sensor_val_t          *p_buf);

aw_local aw_err_t __bh1730_attr_set (void                    *p_cookie,
                                     int                      id,
                                     int                      attr,
                                     const aw_sensor_val_t   *p_val);

aw_local aw_err_t __bh1730_attr_get (void             *p_cookie,
                                     int               id,
                                     int               attr,
                                     aw_sensor_val_t  *p_val);
aw_local aw_err_t __bh1730_trigger_cfg (void                     *p_cookie,
                                        int                       id,
                                        uint32_t                  flags,
                                        aw_sensor_trigger_cb_t    pfn_cb,
                                        void                     *p_arg);

aw_local aw_err_t __bh1730_trigger_on (void *p_cookie, int id);
aw_local aw_err_t __bh1730_trigger_off (void *p_cookie, int id);
aw_local void __bh1730_alarm_callback (void *p_arg);

/******************************************************************************
 本地全局变量定义
******************************************************************************/
/**
 *\brief BH1730的驱动函数入口
 *考虑到低功耗问题，在系统启动时默认让传感器处在standby模式
 */
aw_local aw_const struct awbl_drvfuncs __g_bh1730_drvfuncs = {
    NULL,
    NULL,
    __bh1730_inst_connect
};

/** \brief 传感器类型信息 */
aw_local const awbl_sensor_type_info_t __g_bh1730_typeinfo[] = {
    {AW_SENSOR_TYPE_LIGHT, 1},
};

/** \brief 驱动服务信息 */
aw_local aw_const awbl_sensor_servinfo_t __g_bh1730_servinfo = {
    1,
    &__g_bh1730_typeinfo[0],
    AW_NELEMENTS(__g_bh1730_typeinfo),
};

/** \brief 服务函数 */
aw_local const struct awbl_sensor_servfuncs __g_bh1730_servfuncs = {
    __bh1730_data_get,
    __bh1730_enable,
    __bh1730_disable,
    __bh1730_attr_set,
    __bh1730_attr_get,
    __bh1730_trigger_cfg,
    __bh1730_trigger_on,
    __bh1730_trigger_off,
};

/** \brief 驱动提供的方法 */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);
aw_local struct awbl_dev_method __g_bh1730_methods[] = {
    {AWBL_METHOD_CALL(awbl_sensorserv_get), __bh1730_sensorserv_get},
    AWBL_METHOD_END
};

/** \brief 驱动注册信息 */
aw_local struct awbl_drvinfo __g_bh1730_drvinfo = {
    AWBL_VER_1,
    AWBL_BUSID_I2C,
    AWBL_BH1730_NAME,
    &__g_bh1730_drvfuncs,
    __g_bh1730_methods,
    NULL
};

/** \brief 增益与寄存器映射表*/
aw_local aw_const uint8_t __bh1730_gain_table[4] = {1, 2, 64, 128};

/** \brief 中断延迟处理 */
//aw_local struct aw_isr_defer_job __g_bh1730_isr_defer;

/**
 * \brief BH1730驱动I2C写寄存器
 */
aw_local aw_err_t  __bh1730_reg_write (void    *p_this,
                                       uint8_t  reg_addr,
                                       uint8_t *p_val,
                                       int32_t  len)
{
    __BH1730_DEVINFO_DECL(p_devinfo, p_this);

    return awbl_i2c_write(p_this,
                          AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                          p_devinfo->i2c_addr ,
                          (uint32_t)__BH1730_VAL_CMD(reg_addr),
                          p_val,
                          len);
}

/**
 * \brief BH1730驱动I2C写特殊命令
 */
aw_local aw_err_t  __bh1730_reg_spec_write(void *p_this, uint8_t cmd)
{
    __BH1730_DEVINFO_DECL(p_devinfo, p_this);

    return awbl_i2c_write((struct awbl_i2c_device *)p_this,
                          AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_NONE,
                          p_devinfo->i2c_addr ,
                          0,
                          &cmd,
                          1);
}

/**
 * \brief BH1730驱动I2C读寄存器
 */
aw_local aw_err_t __bh1730_reg_read(awbl_bh1730_dev_t *p_this,
                                    uint8_t            reg_addr,
                                    uint8_t           *p_buf,
                                    int32_t            len)
{
    __BH1730_DEVINFO_DECL(p_devinfo, p_this);
    return awbl_i2c_read((struct awbl_i2c_device *)p_this,
                         AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                         p_devinfo->i2c_addr,
                         (uint32_t)__BH1730_VAL_CMD(reg_addr),
                         p_buf,
                         len);
}

/*
 * \brief SENSORNAME引脚中断后延时处理函数
 */
aw_local void __bh1730_isr_handle (void *p_arg)
{
    __BH1730_DEV_DECL(p_this, p_arg);

    /* 调用用户中断回调 */
    if (p_this->pfn_trigger_cb) {
        (p_this->pfn_trigger_cb)(p_arg,AW_SENSOR_TRIGGER_THRESHOLD);
    }

    /* 清传感器芯片中断 */
    __bh1730_reg_spec_write(p_this, __BH1730_VAL_SPEC_INT_RST);
}

/*
 * \brief 报警引脚中断
 */
aw_local void __bh1730_alarm_callback (void *p_arg)
{
    __BH1730_DEV_DECL(p_this, p_arg);

    if (p_this->pfn_trigger_cb) {
        aw_isr_defer_job_add(&p_this->defer_job);
    }
}

/**
 * \brief 第3阶段初始化
 */
aw_local void __bh1730_inst_connect (awbl_dev_t *p_dev)
{
    __BH1730_DEV_DECL(p_this, p_dev);
    __BH1730_DEVINFO_DECL(p_devinfo, p_dev);
    uint8_t reg_val;

    /* parm init */
    p_this->pfn_trigger_cb  = NULL;
    p_this->p_arg           = NULL;
    p_this->mode            = __BH1730_MODE_STOP;
    p_this->thres_low.val   = -1;
    p_this->thres_low.unit  = AW_SENSOR_UNIT_BASE;
    p_this->thres_up.val    = -1;
    p_this->thres_up.unit   = AW_SENSOR_UNIT_BASE;
    p_this->itime_ms        = (int32_t)(__BH1730_REG_2_ITIME(0xDA));

    /* 配置中断引脚 */    
    if (p_devinfo->alert_pin != -1) {
        aw_gpio_trigger_cfg(p_devinfo->alert_pin, AW_GPIO_TRIGGER_FALL);
        aw_gpio_trigger_connect(p_devinfo->alert_pin,
                                __bh1730_alarm_callback,
                                p_dev);
    }

    int ret;
    uint8_t id;

    /* 读取ID */
    ret = __bh1730_reg_read(p_this, __BH1730_REG_PART_ID, &id, 1);
    if (ret != AW_OK || (id & 0xF0) != 0x70) {
    }

    /* 软件复位芯片 */
    __bh1730_reg_spec_write(p_this,__BH1730_VAL_SPEC_SOFT_RST);

    /* ADC power down */
    reg_val = 0;
    __bh1730_reg_write(p_this,
                       __BH1730_REG_CONTROL,
                       &reg_val,
                       1);

    /* int enable */
    reg_val = (uint8_t)(0x01 | (1 << 4));
    __bh1730_reg_write(p_this,
                       __BH1730_REG_INTERRUPT,
                       &reg_val,
                       1);

    /* isr defer init */
    aw_isr_defer_job_init(&p_this->defer_job,
                          __bh1730_isr_handle,
                          p_dev);
}

/******************************************************************************/
/** \brief 断电重连*/
aw_local void __bh1730_reconnect (awbl_dev_t *p_dev)
{
    __BH1730_DEV_DECL(p_this, p_dev);

    __bh1730_inst_connect ((awbl_dev_t *)p_dev);
    __bh1730_enable(p_this, &p_this->sensor_serv.start_id, 1, NULL);

}

/******************************************************************************/
/**
 * \brief 根据寄存器值计算光照度
 */
aw_local int32_t __bh1730_reg_2_lux(uint8_t *p_val,
                                    uint8_t  gain,
                                    int32_t  itime)
{
    int32_t data0,data1;
    int32_t ratio;
    int32_t lx;

    data0 = (int32_t)(p_val[0] + (p_val[1] << 8));
    data1 = (int32_t)(p_val[2] + (p_val[3] << 8));

    if (data0 == 0) {
        return 0;
    }

    ratio = (int32_t)((100 * data1) / data0);/* 放大100倍避免浮点数 */

    if (ratio < 26) {
        lx = ((1290 * data0 - 2733 * data1) * (uint64_t)1026000) /
            (__bh1730_gain_table[gain] * itime);
    } else if (ratio < 55) {
        lx = ((795 * data0 - 859 * data1) * (uint64_t)1026000) /
                (__bh1730_gain_table[gain] * itime);
    } else if (ratio < 109) {
        lx = ((510 * data0 - 345 * data1) * (uint64_t)1026000) /
                (__bh1730_gain_table[gain] * itime);
    } else if (ratio < 213) {
        lx = ((276 * data0 - 130 * data1) * (uint64_t)1026000) /
                (__bh1730_gain_table[gain] * itime);
    }else {
        return 0;
    }
    
    return lx;
}

/**
 * \brief 根据光照度计算寄存器值
 */
aw_local uint16_t __bh1730_lux_2_reg (const aw_sensor_val_t *p_val,
                                      int32_t                ratio,
                                      int32_t                gain,
                                      int32_t                itime)
{
    int32_t reg_val;
    aw_sensor_val_t temp = *p_val;
    int64_t lx ;

    aw_sensor_val_unit_convert(&temp, 1, AW_SENSOR_UNIT_MILLI);
    lx = (int64_t)(temp.val);/* 使用64位防止溢出 */
    if (ratio < 26) {
        reg_val = (int32_t)((lx * __bh1730_gain_table[gain] * itime ) /
                            ((int64_t)10260 * (129000 - 2733 * ratio)));
    } else if (ratio < 55) {
        reg_val = (int32_t)((lx * __bh1730_gain_table[gain] * itime * 100) /
                            ((int64_t)1026 * (79500 - 859 * ratio)));
    } else if (ratio < 109) {
        reg_val = (int32_t)((lx * __bh1730_gain_table[gain] * itime * 100) /
                            ((int64_t)1026 * (51000 - 345 * ratio)));
    } else if (ratio < 213) {
        reg_val = (int32_t)((lx * __bh1730_gain_table[gain] * itime * 100) /
                            ((int64_t)1026 * (27600 - 130 * ratio)));
    }else {
        return 0;
    }

    return (uint16_t)(reg_val);
}

/**
 * \brief 更新thres low寄存器值
 */
aw_local aw_err_t __bh1730_thres_low_set (awbl_bh1730_dev_t     *p_this,
                                          uint8_t               *p_adcval,
                                          const aw_sensor_val_t *p_val)
{
    __BH1730_DEVINFO_DECL(p_devinfo, p_this);
    uint8_t reg_val[2];
    int32_t ratio;
    aw_err_t ret;
    int32_t adc_thres;

    ratio = 100 * (p_adcval[2] + (p_adcval[3] << 8)) /
                    (p_adcval[0] + (p_adcval[1] << 8));

    if (p_this->thres_low.val != -1) {
        adc_thres = __bh1730_lux_2_reg(p_val,
                                    ratio,
                                    p_devinfo->gain,
                                    p_this->itime_ms);
        reg_val[0] = (uint8_t)adc_thres;
        reg_val[1] = (uint8_t)(adc_thres >> 8);
        ret =  __bh1730_reg_write(p_this,
                                 __BH1730_REG_TH_LOW_0,
                                 reg_val,
                                 2);
        if (ret != AW_OK) {
            return ret;
        }
    }
    return AW_OK;
}

/**
 * \brief 更新thres up寄存器值
 */
aw_local aw_err_t __bh1730_thres_up_set (awbl_bh1730_dev_t     *p_this,
                                         uint8_t               *p_adcval,
                                         const aw_sensor_val_t *p_val)
{
    __BH1730_DEVINFO_DECL(p_devinfo, p_this);
    uint8_t reg_val[2];
    int32_t ratio;
    aw_err_t ret;
    uint16_t adc_thres;

    ratio = 100 * (p_adcval[2] + (p_adcval[3] << 8)) /
                    (p_adcval[0] + (p_adcval[1] << 8));

    if (p_this->thres_low.val != -1) {
        adc_thres = __bh1730_lux_2_reg(p_val,
                                       ratio,
                                       p_devinfo->gain,
                                       p_this->itime_ms);
        reg_val[0] = (uint8_t)adc_thres;
        reg_val[1] = (uint8_t)(adc_thres >> 8);
        ret = __bh1730_reg_write(p_this,
                                 __BH1730_REG_TH_UP_0,
                                 reg_val,
                                 2);
        if (ret != AW_OK) {
            return ret;
        }
    }
    return AW_OK;
}

/**
 * \brief 获取BH1730传感器数据
 */
aw_local aw_err_t __bh1730_data_get (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf)
{
    __BH1730_DEV_DECL(p_this, p_cookie);
    __BH1730_DEVINFO_DECL(p_devinfo, p_cookie);

    int32_t i;/* 循环结构体变量 */
    uint8_t idx;/* 本传感器对应的通道偏移量 */
    uint8_t start_id   = p_devinfo->start_id;
    uint8_t reg_val[4] = {0};
    aw_err_t ret;
    uint8_t  reg;
    uint8_t timeout = 0;
	
    aw_assert((num != 0) && (p_buf != NULL));

    for (i = 0; i < num; i++){
        if (p_ids[i] == start_id){
            idx = p_ids[i] - start_id;

            if (idx == 0){

                while (!(reg & __BH1730_VAL_CONTROL_VALID)) {
                    ret = __bh1730_reg_read(p_this,
                                            __BH1730_REG_CONTROL,
                                            &reg,
                                            1);
                    if (ret != AW_OK) {
                        return ret;
                    }
                    if (++timeout > 100){
                        __bh1730_reconnect((awbl_dev_t *)p_cookie);
                        return -1;
                    }
                }

                ret = __bh1730_reg_read(p_this,
                                        __BH1730_REG_DATA0_0,
                                        reg_val,
                                        4);
                if (ret != AW_OK) {
                    return ret;
                }
                p_buf[i].val  = __bh1730_reg_2_lux(reg_val,
                                                   p_devinfo->gain,
                                                   p_this->itime_ms);
                p_buf[i].unit = AW_SENSOR_UNIT_MILLI;
            }
        }
    }

    return AW_OK;
}

/******************************************************************************/
/**
 * \brief 使能BH1730传感器的一个或多个通道
 */
aw_local aw_err_t __bh1730_enable (void             *p_cookie,
                                   const int        *p_ids,
                                   int               num,
                                   aw_sensor_val_t  *p_result)
{
    __BH1730_DEV_DECL(p_this, p_cookie);
    __BH1730_DEVINFO_DECL(p_devinfo, p_cookie);

    int32_t i;/* 循环结构体变量 */
    uint8_t idx;/* 本传感器对应的通道偏移量 */
    uint8_t start_id = p_devinfo->start_id;
    aw_err_t ret;
    uint8_t reg_val[4];

    for (i = 0; i < num; i++){
        if (p_ids[i] == start_id){
            idx = p_ids[i] - start_id;
            /* 只支持一个通道 */
            if (idx == 0) {
                if (p_this->mode == __BH1730_MODE_START) {
                    return AW_OK;
                }
                reg_val[0] = 0x03;
                ret = __bh1730_reg_write(p_this,
                                         __BH1730_REG_CONTROL,
                                         &reg_val[0],
                                         1);
                if (ret != AW_OK) {
                    return ret;
                }
                p_this->mode = __BH1730_MODE_START;
                /* 检查是否需要更新thres */
                if ((p_this->thres_low.val == -1) &&
                    (p_this->thres_up.val == -1)) {
                    return AW_OK;
                }
                /*如果需要更新，必须先采集一次数据，因为thres计算依赖于当前光照条件 */
                aw_mdelay((p_this->itime_ms + __BH1730_ADC_SAMPLE_TIME_US) /
                            10000);

                ret = __bh1730_reg_read(p_this,
                                        __BH1730_REG_DATA0_0,
                                        reg_val,
                                        4);
                if (ret != AW_OK) {
                    return ret;
                }
                ret = __bh1730_thres_low_set(p_this, reg_val, &p_this->thres_low);
                if(ret != AW_OK) {
                    return ret;
                }
                return __bh1730_thres_up_set(p_this, reg_val, &p_this->thres_up);
            }
        }
    }

    return -AW_ENOTSUP;
}

/******************************************************************************/
/**
 * \brief 禁能BH1730传感器的一个或多个通道
 */
aw_local aw_err_t __bh1730_disable (void                *p_cookie,
                                    const int           *p_ids,
                                    int                  num,
                                    aw_sensor_val_t     *p_result)
{
    __BH1730_DEV_DECL(p_this, p_cookie);
    __BH1730_DEVINFO_DECL(p_devinfo, p_cookie);

    int32_t i;/* 循环结构体变量 */
    uint8_t idx;/* 本传感器对应的通道偏移量 */
    uint8_t start_id = p_devinfo->start_id;
    aw_err_t ret;
    uint8_t reg_val;

    aw_assert((num != 0) && (p_ids != NULL));

    for (i = 0; i < num; i++){
        if (p_ids[i] == start_id){
            idx = p_ids[i] - start_id;
            if (idx == 0) {
                if (p_this->mode == __BH1730_MODE_STOP) {
                    return AW_OK;
                }
                reg_val = 0x00;
                ret = __bh1730_reg_write(p_this,
                                         __BH1730_REG_CONTROL,
                                         &reg_val,
                                         1);
                if (ret == AW_OK) {
                    p_this->mode = __BH1730_MODE_STOP;
                    return AW_OK;
                }
            }
        }
    }

    return AW_OK;
}

/******************************************************************************/
/**
 * \brief 检查thres是否合法
 * \note  使用该函数时应保证p_val的单位为AW_SENSOR_UNIT_MILLI
 */
aw_local aw_err_t __bh1730_check_thres_is_valid(awbl_bh1730_dev_t *p_this,
                                                aw_sensor_val_t   *p_val)
{
    __BH1730_DEVINFO_DECL(p_devinfo, p_this);

    if (p_val->val == 0) {
        return AW_OK;
    }

    switch (p_devinfo->gain) {

    case BH1730_GAIN_1X:
        if (p_val->val > 100000000) {
            return -AW_EINVAL;
        }
        break;

    case BH1730_GAIN_2X:
        if (p_val->val > 50000000) {
            return -AW_EINVAL;
        }
        break;

    case BH1730_GAIN_64X:
        if (p_val->val > (100000000 >> 6)) {
            return -AW_EINVAL;
        }
        break;

    case BH1730_GAIN_128X:
        if (p_val->val > (100000000 >> 7)) {
            return -AW_EINVAL;
        }
        break;

    default :
        return -AW_EINVAL;
    }

    return AW_OK;
}

/******************************************************************************/
/**
 * \brief 设置BH1730的一个属性
 */
aw_local aw_err_t __bh1730_attr_set (void                  *p_cookie,
                                     int                    id,
                                     int                    attr,
                                     const aw_sensor_val_t *p_val)
{
    __BH1730_DEV_DECL(p_this, p_cookie);
    __BH1730_DEVINFO_DECL(p_devinfo, p_cookie);
    uint8_t reg_val[4];
    aw_err_t ret;
    aw_sensor_val_t attr_val = *p_val;

    if (p_devinfo->start_id != id) {
        return -AW_ENOTSUP;
    }
    
    switch ( attr ){

    /* 设置下限值 */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        /* 最高精度为0.001 lux，将其转换为MILLI */
        ret = aw_sensor_val_unit_convert(&attr_val, 1, AW_SENSOR_UNIT_MILLI);
        if (ret != AW_OK) {
            return ret;
        }
        /* 参数有效性检查 */
        ret = __bh1730_check_thres_is_valid(p_this, &attr_val);
        if (ret != AW_OK) {
            return ret;
        }
        /* thres寄存器值的计算依赖于采样数据，所以在未enable时只保存参数 */
        if (p_this->mode != __BH1730_MODE_START) {
            p_this->thres_low = attr_val;
            return AW_OK;
        }
        ret = __bh1730_reg_read(p_this,
                                __BH1730_REG_DATA0_0,
                                reg_val,
                                4);
        if (ret != AW_OK) {
            return ret;
        }
        ret = __bh1730_thres_low_set(p_this, reg_val, &attr_val);
        if (ret != AW_OK) {
            return ret;
        }
        p_this->thres_low = attr_val;
        break;
                                 
    /* 设置上限值 */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        /* 最高精度为0.001 lux，将其转换为MILLI */
        ret = aw_sensor_val_unit_convert(&attr_val, 1, AW_SENSOR_UNIT_MILLI);
        if (ret != AW_OK) {
            return ret;
        }
        /* 参数有效性检查 */
        ret = __bh1730_check_thres_is_valid(p_this, &attr_val);
        if (ret != AW_OK) {
            return ret;
        }
        /* thres寄存器值的计算依赖于采样数据，所以在未enable时只保存参数 */
        if (p_this->mode != __BH1730_MODE_START) {
            p_this->thres_up = attr_val;
            return AW_OK;
        }
        ret = __bh1730_reg_read(p_this,
                                __BH1730_REG_DATA0_0,
                                reg_val,
                                4);
        if (ret != AW_OK) {
            return ret;
        }
        ret = __bh1730_thres_up_set(p_this, reg_val, &attr_val);
        if (ret != AW_OK) {
            return ret;
        }
        p_this->thres_up = attr_val;
        break;

    /* 设置采样频率  */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        /* 采样频率为整数值 */
        ret = aw_sensor_val_unit_convert(&attr_val, 1, AW_SENSOR_UNIT_BASE);
        if (ret != AW_OK) {
            return ret;
        }

        if (attr_val.val != 0) {/* 避免被除数为0 */
            /* 放大10000倍，以与宏定义的值保持一致 */
            attr_val.val = 10000 / attr_val.val;
        }
        if ((attr_val.val < __BH1730_MEAS_TIME_MIN) ||
            (attr_val.val >__BH1730_MEAS_TIME_MAX)) {
            return -AW_ENOTSUP;
        }
        reg_val[0] = (uint8_t)__BH1730_MEAS_TIME_2_REG(attr_val.val);

        ret = __bh1730_reg_write(p_this,
                                 __BH1730_REG_TIMING,
                                 reg_val,
                                 2);
        if (ret != AW_OK) {
            return ret;
        }
        p_this->itime_ms = attr_val.val - __BH1730_ADC_SAMPLE_TIME_US;
        break;

    case AW_SENSOR_ATTR_DURATION_DATA:
        ret = aw_sensor_val_unit_convert(&attr_val, 1, AW_SENSOR_UNIT_BASE);
        if (ret != AW_OK) {
            return ret;
        }
        /* 根据芯片定义，最大只支持到0x0f次 */
        if ((attr_val.val > 0x0f) || (attr_val.val < 0)) {
            return -AW_EINVAL;
        }
        ret = __bh1730_reg_read(p_this, __BH1730_REG_INTERRUPT, reg_val, 1);
        if (ret != AW_OK) {
            return ret;
        }
        reg_val[0] = (uint8_t)((reg_val[0] & 0xf0) | (attr_val.val & 0x0f));
        return __bh1730_reg_write(p_this, __BH1730_REG_INTERRUPT, reg_val, 1);

    default:
        return -AW_ENOTSUP;
    }
    
    return AW_OK;
}

/******************************************************************************/
/**
 * \brief 获取BH1730的一个通道属性
 */
aw_local aw_err_t __bh1730_attr_get (void            *p_cookie,
                                     int              id,
                                     int              attr,
                                     aw_sensor_val_t *p_val)
{
    __BH1730_DEV_DECL(p_this, p_cookie);
    uint8_t reg_val;
    aw_err_t ret;

    switch ( attr ){
    /* 获取下限值 */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        if (p_this->thres_low.val != -1) {
            p_val->val  = p_this->thres_low.val;
            p_val->unit = p_this->thres_low.unit;
        } else {
            p_val->val = 0;
            p_val->unit = AW_SENSOR_UNIT_BASE;
        }

        return AW_OK;

    /* 获取上限值 */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        if (p_this->thres_up.val != -1) {
            p_val->val  = p_this->thres_up.val;
            p_val->unit = p_this->thres_up.unit;
        } else {
            p_val->val = 0;
            p_val->unit = AW_SENSOR_UNIT_BASE;
        }

        return AW_OK;

    /* 获取采样频率  */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        p_val->val  = 10000 / (p_this->itime_ms + __BH1730_ADC_SAMPLE_TIME_US);
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    case AW_SENSOR_ATTR_DURATION_DATA:
        ret = __bh1730_reg_read(p_this, __BH1730_REG_INTERRUPT, &reg_val, 1);
        if (ret != AW_OK) {
            return ret;
        }
        p_val->val  = reg_val & 0x0f;
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    default:
        return -AW_ENOTSUP;
    }
    
    return AW_OK;
}

/******************************************************************************/
/**
 * \brief 设置触发，一个通道仅能设置一个触发回调函数
 */
aw_local aw_err_t __bh1730_trigger_cfg (void                  *p_cookie,
                                        int                    id,
                                        uint32_t               flags,
                                        aw_sensor_trigger_cb_t pfn_cb,
                                        void                  *p_arg)
{
    __BH1730_DEV_DECL(p_this, p_cookie);
    __BH1730_DEVINFO_DECL(p_devinfo, p_cookie);

    uint8_t start_id = p_devinfo->start_id;

    if (((uint8_t)id != start_id) || (flags != AW_SENSOR_TRIGGER_THRESHOLD)) {
        return -AW_ENOTSUP;
    }

    /* 只保存参数，不做传感器中断使能、引脚中断使能 */
    if (id == p_devinfo->start_id) {
        p_this->pfn_trigger_cb  = pfn_cb;
        p_this->p_arg           = p_arg;
    } else {
       return -AW_ENOTSUP; 
    }

    return AW_OK;
}

/**
 * \brief 设置触发，一个通道仅能设置一个触发回调函数
 */
aw_local aw_err_t __bh1730_trigger_on (void *p_cookie,int id)
{
    __BH1730_DEVINFO_DECL(p_devinfo, p_cookie);

    if (p_devinfo->start_id != id) {
        return -AW_ENOTSUP;
    }
    
    return aw_gpio_trigger_on(p_devinfo->alert_pin);
}

/**
 * \brief 关闭触发，一个通道仅能设置一个触发回调函数
 */
aw_local aw_err_t __bh1730_trigger_off (void *p_cookie, int id)
{
    __BH1730_DEVINFO_DECL(p_devinfo,p_cookie);

    if (p_devinfo->start_id != id) {
        return -AW_ENOTSUP;
    }
    
    return aw_gpio_trigger_off(p_devinfo->alert_pin);
}

/**
 * \brief BH1730 标准服务接口获取
 */
aw_local aw_err_t __bh1730_sensorserv_get (struct awbl_dev  *p_dev,
                                           void             *p_arg)
{
    __BH1730_DEV_DECL(p_this,p_dev);
    __BH1730_DEVINFO_DECL(p_devinfo,p_dev);
    struct awbl_sensor_service *p_serv = &p_this->sensor_serv;

    p_serv->p_next      = NULL;
    p_serv->p_cookie    = (void *)p_dev;
    p_serv->start_id    = p_devinfo->start_id;
    p_serv->p_servinfo  = &__g_bh1730_servinfo;
    p_serv->p_servfuncs = &__g_bh1730_servfuncs;

    *(struct awbl_sensor_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief 将 BH1730 驱动注册到 AWBus 子系统中
 */
void awbl_bh1730_drv_register (void)
{
    awbl_drv_register(&__g_bh1730_drvinfo);
}

/* end of file */
 

