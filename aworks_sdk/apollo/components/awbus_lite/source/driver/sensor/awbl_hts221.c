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
 * \brief HTS221 驱动
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-12  wan, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_gpio.h"
#include "aworks.h"
#include "driver/sensor/awbl_hts221.h"
#include "aw_isr_defer.h"
#include "aw_vdebug.h"
    
/*******************************************************************************
  本地宏定义
*******************************************************************************/
/** \brief 通过 AWBus 设备，获取 HTS221设备 */
#define __MY_GET_DEV(p_dev) (awbl_hts221_dev_t*)(p_dev)

/** \brief 通过 AWBus 设备，获取 HTS221设备信息 */
#define __MY_DEVINFO_GET(p_dev) (awbl_hts221_devinfo_t*)AWBL_DEVINFO_GET(p_dev)

#define __HTS221_REG_WHO_IM_I     (0x0F)    /*< \brief 设备ID地址             */
#define __HTS221_MY_ID            (0xBC)    /*< \brief 设备ID                 */
#define __HTS221_REG_AV_CONF      (0x10)    /*< \brief 设置采样样本的值地址   */
#define __HTS221_REG_CTRL_REG1    (0x20)    /*< \brief 控制寄存器1地址        */
#define __HTS221_REG_CTRL_REG2    (0x21)    /*< \brief 控制寄存器2地址        */
#define __HTS221_REG_CTRL_REG3    (0x22)    /*< \brief 控制寄存器3地址        */
#define __HTS221_REG_STATUS       (0x27)    /*< \brief 状态寄存器地址         */
#define __HTS221_REG_H_OUT_L      (0x28)    /*< \brief 湿度低字节地址         */
#define __HTS221_REG_H_OUT_H      (0x29)    /*< \brief 湿度高字节地址         */
#define __HTS221_REG_T_OUT_L      (0x2A)    /*< \brief 温度低字节地址         */
#define __HTS221_REG_T_OUT_H      (0x2B)    /*< \brief 温度高字节地址         */

#define __HTS221_REG_H0_OUT       (0x36)    /*< \brief 湿度校准值0地址        */
#define __HTS221_REG_H0_OUT_LEN   (2)       /*< \brief 湿度校准值0地址长度    */
#define __HTS221_REG_H1_OUT       (0x3A)    /*< \brief 湿度校准值1地址        */
#define __HTS221_REG_H1_OUT_LEN   (2)       /*< \brief 湿度校准值1地址长度    */
#define __HTS221_REG_H0_rH_2      (0x30)    /*< \brief 湿度校准参数0地址      */
#define __HTS221_REG_H1_rH_2      (0x31)    /*< \brief 湿度校准参数1地址      */

#define __HTS221_REG_T0_OUT       (0x3C)    /*< \brief 温度校准值0地址        */
#define __HTS221_REG_T0_OUT_LEN   (2)       /*< \brief 温度校准值0地址长度    */
#define __HTS221_REG_T1_OUT       (0x3E)    /*< \brief 温度校准值1地址        */
#define __HTS221_REG_T1_OUT_LEN   (2)       /*< \brief 温度校准值1地址长度    */
#define __HTS221_REG_T0_degC_8    (0x32)    /*< \brief 温度校准参数0地址      */
#define __HTS221_REG_T1_degC_8    (0x33)    /*< \brief 温度校准参数1地址      */

#define __HTS221_REG_T1_T0_MSB    (0x35)    /*< \brief 温度高位参数地址       */

/** \brief 利用校准值x0，y0，x1，y1 计算出传入x的温度湿度实际值 并扩大10^6倍 */
#define __GET_VALUE(x, x0, y0, x1, y1)                                 \
        ((int32_t)(1000000 * (int64_t)(((y1)-(y0))*(x) + (x1)*(y0) -   \
         (x0)*(y1)) / ((x1) - (x0))))

/** \brief 将两个int8转换为一个int16类型 */
#define __HTS221_UINT8_TO_UINT16(buff) ((int16_t)((buff[1] << 8) | buff[0]))

/** \brief 持续读取寄存器 */
#define __HTS221_IIC_CONTINUE_READ     (0x80)

/** \brief 设置频率为一次 */
#define __HTS221_RATE_SET_ONE(data)    (((data) & (~0x3)) | 0x0)

/** \brief 设置频率为1 */
#define __HTS221_RATE_SET_1S(data)     (((data) & (~0x3)) | 0x1)

/** \brief 设置频率为7 */
#define __HTS221_RATE_SET_7S(data)     (((data) & (~0x3)) | 0x2)

/** \brief 设置频率为12 */
#define __HTS221_RATE_SET_12S(data)    (((data) & (~0x3)) | 0x3)

/** \brief 开始一次数据采集 */
#define __HTS221_ONE_SHOT_START        (0x1)

/** \brief 模块启动 */
#define __HTS221_START                 (0x1<<7)

/** \brief 模块关闭 */
#define __HTS221_CLOSE                 (~(0x1<<7))

/** \brief 获取HUM状态位 */
#define __HTS221_GET_HUM_STATUS(reg)   (((reg) >> 1) & 0x1)

/** \brief 获取TEM状态位 */
#define __HTS221_GET_TEM_STATUS(reg)   ((reg) & 0x1)

/** \brief 使能数据可读触发 */
#define __HTS221_TRIGGER_READ          (0x1<<2)

/** \brief 禁能数据可读触发 */
#define __HTS221_TRIGGER_CLOSE         (~(0x1<<2))

/** \brief 获取温度校准值1 */
#define __HTS221_GET_TEM_VALUE1(val, data) \
                      ((int8_t)(((((val) & 0x3) << 8) | (data)) >> 3))

/** \brief 获取温度校准值2 */
#define __HTS221_GET_TEM_VALUE2(val, data) \
                      ((int8_t)(((((val) & 0xc) << 6) | (data)) >> 3))

/*******************************************************************************
  本地函数声明
*******************************************************************************/
/*
 * \brief 第二阶段初始化函数
 */
aw_local void __hts221_inst_init2 (struct awbl_dev *p_dev);

/*
 * \brief 第三阶段初始化函数
 */
aw_local void __hts221_inst_connect (struct awbl_dev *p_dev);

/**
 * \brief 传感器服务获取方法
 */
aw_local aw_err_t __hts221_sensorserv_get (struct awbl_dev *p_dev, void *p_arg);

/**
 * \brief 从指定通道获取采样数据
 */
aw_local aw_err_t __hts221_data_get (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf);

/**
 * \brief 使能传感器的某一通道
 */
aw_local aw_err_t __hts221_enable (void                  *p_cookie,
                                   const int             *p_ids,
                                   int                    num,
                                   aw_sensor_val_t       *p_result);

/**
 * \brief 禁能传感器通道
 */
aw_local aw_err_t __hts221_disable (void            *p_cookie,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_result);

/*
 * \brief 属性的设置
 */
aw_local aw_err_t __hts221_attr_set (void                  *p_cookie,
                                     int                    id,
                                     int                    attr,
                                     const aw_sensor_val_t *p_val);

/*
 * \brief 获取属性
 */
aw_local aw_err_t __hts221_attr_get (void            *p_cookie,
                                     int              id,
                                     int              attr,
                                     aw_sensor_val_t *p_val);

/**
 * \brief 触发配置
 *
 * \note 只支持一路触发，若再次调用会覆盖之前设置的
 */
aw_local aw_err_t __hts221_trigger_cfg (void                  *p_cookie,
                                        int                    id,
                                        uint32_t               flags,
                                        aw_sensor_trigger_cb_t pfn_cb,
                                        void                  *p_arg);

/*
 * \brief 打开触发
 */
aw_local aw_err_t __hts221_trigger_on (void *p_cookie, int id);

/*
 * \brief 关闭触发
 */
aw_local aw_err_t __hts221_trigger_off (void *p_cookie, int id);

/*******************************************************************************
  local functions
*******************************************************************************/
/*
 * \brief hts221 写数据
 */
aw_local aw_err_t __hts221_write (awbl_hts221_dev_t *p_this,
                                  uint8_t            subaddr,
                                  uint8_t           *p_buf,
                                  uint32_t           nbytes)
{
    return aw_i2c_write(&p_this->i2c_dev,
                        subaddr | __HTS221_IIC_CONTINUE_READ,
                        p_buf,
                        nbytes);
}

/*
 * \brief hts221 读数据
 */
aw_local aw_err_t __hts221_read (awbl_hts221_dev_t *p_this,
                                 uint8_t            subaddr,
                                 uint8_t           *p_buf,
                                 uint32_t           nbytes)
{
    return aw_i2c_read(&p_this->i2c_dev,
                       subaddr | __HTS221_IIC_CONTINUE_READ,
                       p_buf,
                       nbytes);
}

/*
 * \brief 获取湿度校准值
 */
aw_local aw_err_t __hts221_get_hum_cal (awbl_hts221_dev_t *p_this)
{
    uint8_t buff[2] = {0};

    aw_err_t ret = AW_OK;

    /*
     * \brief 以下为获取湿度的校准数据的2组
     */
    ret = __hts221_read(p_this,
                        __HTS221_REG_H0_OUT,
                        buff,
                        __HTS221_REG_H0_OUT_LEN);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->cal_val[0].x0 = __HTS221_UINT8_TO_UINT16(buff);

    ret = __hts221_read(p_this, __HTS221_REG_H0_rH_2, buff, 1);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->cal_val[0].y0 = (int8_t)(buff[0] >> 1);

    ret = __hts221_read(p_this,
                        __HTS221_REG_H1_OUT,
                        buff,
                        __HTS221_REG_H1_OUT_LEN);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->cal_val[0].x1 = __HTS221_UINT8_TO_UINT16(buff);

    ret = __hts221_read(p_this, __HTS221_REG_H1_rH_2, buff, 1);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->cal_val[0].y1 = (int8_t)(buff[0] >> 1);

    return ret;
}

/*
 * \brief 获取温度校准值
 */
aw_local aw_err_t __hts221_get_tem_cal (awbl_hts221_dev_t *p_this)
{
    uint8_t buff[2] = {0};

    uint8_t tem_msb_val = 0;

    aw_err_t ret = AW_OK;

    /*
     * \brief 以下为获取温度的校准数据的2组
     */
    ret = __hts221_read(p_this, __HTS221_REG_T1_T0_MSB, &buff[0], 1);
    if (ret != AW_OK) {
        return ret;
    }
    tem_msb_val = buff[0];

    ret = __hts221_read(p_this,
                        __HTS221_REG_T0_OUT,
                        buff,
                        __HTS221_REG_T0_OUT_LEN);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->cal_val[1].x0 = __HTS221_UINT8_TO_UINT16(buff);

    ret = __hts221_read(p_this, __HTS221_REG_T0_degC_8, buff, 1);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->cal_val[1].y0 = __HTS221_GET_TEM_VALUE1(tem_msb_val, buff[0]);

    ret = __hts221_read(p_this,
                        __HTS221_REG_T1_OUT,
                        buff,
                        __HTS221_REG_T1_OUT_LEN);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->cal_val[1].x1 = __HTS221_UINT8_TO_UINT16(buff);

    ret = __hts221_read(p_this, __HTS221_REG_T1_degC_8, buff, 1);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->cal_val[1].y1 = __HTS221_GET_TEM_VALUE2(tem_msb_val, buff[0]);

    return ret;
}

/*
 * \brief 配置选择
 */
aw_local aw_err_t  __reg_attr_set (awbl_hts221_dev_t *p_this, uint8_t rate)
{
     aw_err_t ret = AW_OK;

     uint8_t rate_cfg = 0;

     ret = __hts221_read(p_this, __HTS221_REG_CTRL_REG1, &rate_cfg, 1);
     if (ret != AW_OK) {
         return ret;
     }

     switch (rate) {
     case 0:
         rate_cfg = __HTS221_RATE_SET_ONE(rate_cfg);
         break;
     case 1:
         rate_cfg = __HTS221_RATE_SET_1S(rate_cfg);
         break;
     case 2:
         rate_cfg = __HTS221_RATE_SET_7S(rate_cfg);
         break;
     case 3:
         rate_cfg = __HTS221_RATE_SET_12S(rate_cfg);
         break;
     default:
         break;
     }

     return __hts221_write(p_this, __HTS221_REG_CTRL_REG1, &rate_cfg, 1);
}

/**
 * \brief 中断延时处理函数
 */
aw_local void __hts221_trigger_data_get (void *param)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(param);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(param);

    awbl_hts221_calibration_data_t *hum = &p_this->cal_val[0];
    awbl_hts221_calibration_data_t *tem = &p_this->cal_val[1];

    uint8_t current_data[4] = {0};
    uint8_t *p_current_data = current_data + 2;

    int16_t hum_data = 0;
    int16_t tem_data = 0;

    /* 读出数据后，自动清中断标志位 */
    __hts221_read(p_this, __HTS221_REG_H_OUT_L, current_data, 4);

    /* 获取湿度 */
    hum_data = __HTS221_UINT8_TO_UINT16(current_data);
    p_this->current_data[0].val = __GET_VALUE(hum_data,
                                              hum->x0,
                                              hum->y0,
                                              hum->x1,
                                              hum->y1);
    p_this->current_data[0].unit = AW_SENSOR_UNIT_MICRO;

    /* 获取温度 */
    tem_data = __HTS221_UINT8_TO_UINT16(p_current_data);
    p_this->current_data[1].val = __GET_VALUE(tem_data,
                                              tem->x0,
                                              tem->y0,
                                              tem->x1,
                                              tem->y1);
    p_this->current_data[1].unit = AW_SENSOR_UNIT_MICRO;

    if (p_this->pfn_trigger_cb[0] &&
            (p_this->flags[0] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_cb[0](p_this->p_arg[0],
                                  AW_SENSOR_TRIGGER_DATA_READY);
    }

    if (p_this->pfn_trigger_cb[1] &&
            (p_this->flags[1] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_cb[1](p_this->p_arg[1],
                                  AW_SENSOR_TRIGGER_DATA_READY);
    }

    aw_gpio_trigger_on(p_devinfo->trigger_pin);
}

/*
 * \brief 数据准备就绪引脚中断
 */
aw_local void __hts221_alarm_callback (void *p_arg)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_arg);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_arg);

    aw_gpio_trigger_off(p_devinfo->trigger_pin);

    aw_isr_defer_job_add(&p_this->g_myjob);   /*< \brief 添加中断延迟处理任务 */
}

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/
/** \brief 驱动提供的方法 */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

aw_local aw_const struct awbl_dev_method __g_hts221_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __hts221_sensorserv_get),
        AWBL_METHOD_END
};

/** \brief 驱动入口点 */
aw_local aw_const struct awbl_drvfuncs __g_hts221_drvfuncs = {
        NULL,                     /*< \brief 第一阶段初始化                   */
        __hts221_inst_init2,      /*< \brief 第二阶段初始化                   */
        __hts221_inst_connect     /*< \brief 第三阶段初始化                   */
};

/** \brief 驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_hts221 = {
    AWBL_VER_1,                   /*< \brief AWBus 版本号                     */
    AWBL_BUSID_I2C,               /*< \brief 总线 ID                          */
    AWBL_HTS221_NAME,             /*< \brief 驱动名                           */
    &__g_hts221_drvfuncs,         /*< \brief 驱动入口点                       */
    &__g_hts221_dev_methods[0],   /*< \brief 驱动提供的方法                   */
    NULL                          /*< \brief 驱动探测函数                     */
};

/** \brief 服务函数 */
aw_local aw_const struct awbl_sensor_servfuncs __g_hts221_servfuncs = {
        __hts221_data_get,
        __hts221_enable,
        __hts221_disable,
        __hts221_attr_set,
        __hts221_attr_get,
        __hts221_trigger_cfg,
        __hts221_trigger_on,
        __hts221_trigger_off
};

/*******************************************************************************
    本地函数定义
*******************************************************************************/
/**
 * \brief 传感器服务获取方法
 */
aw_local aw_err_t __hts221_sensorserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_hts221_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    awbl_sensor_service_t *p_sensor_serv = &p_this->sensor_serv;

    /** \brief 该传感器的通道分配 */
    aw_local aw_const awbl_sensor_type_info_t __g_senter_type[2] = {
            {AW_SENSOR_TYPE_HUMIDITY, 1},
            {AW_SENSOR_TYPE_TEMPERATURE, 1},
    };

    /** \brief 传感器服务常量信息 */
    aw_local aw_const awbl_sensor_servinfo_t  __g_senser_info = {
            2,
            __g_senter_type,
            AW_NELEMENTS(__g_senter_type)
    };

    p_sensor_serv->p_servinfo  = &__g_senser_info;
    p_sensor_serv->start_id    = p_devinfo->start_id;
    p_sensor_serv->p_servfuncs = &__g_hts221_servfuncs;
    p_sensor_serv->p_cookie    = (awbl_hts221_dev_t*)p_this;
    p_sensor_serv->p_next      = NULL;

    *((awbl_sensor_service_t**)p_arg) = p_sensor_serv;

    return AW_OK;
}

/******************************************************************************/
/** \brief 第二次初始化传感器服务获取方法 */
aw_local void __hts221_inst_init2 (struct awbl_dev *p_dev)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_dev);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    /** \brief 创建IIC设备 */
    aw_i2c_mkdev(&p_this->i2c_dev,
                 p_dev->p_devhcf->bus_index,
                 p_devinfo->i2c_addr,
                 (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));

    p_this->pfn_trigger_cb[0] = NULL;
    p_this->p_arg[0]          = NULL;
    p_this->pfn_trigger_cb[1] = NULL;
    p_this->p_arg[1]          = NULL;
    p_this->flags[0]          = 0;
    p_this->flags[1]          = 0;

    p_this->cal_val[0].x0     = 0;
    p_this->cal_val[0].y0     = 0;
    p_this->cal_val[0].x1     = 0;
    p_this->cal_val[0].y1     = 0;

    p_this->cal_val[1].x0     = 0;
    p_this->cal_val[1].y0     = 0;
    p_this->cal_val[1].x1     = 0;
    p_this->cal_val[1].y1     = 0;

    p_this->sampling_rate.val = 0;
    p_this->sampling_rate.unit = 0;
    p_this->trigger           = 0;

    p_this->current_data[0].val  = 0;
    p_this->current_data[0].unit = 0;
    p_this->current_data[1].val  = 0;
    p_this->current_data[1].unit = 0;

    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    aw_isr_defer_job_init(&p_this->g_myjob, __hts221_trigger_data_get, p_this);

    if (p_devinfo->trigger_pin != -1) {

        aw_gpio_trigger_cfg(p_devinfo->trigger_pin, AW_GPIO_TRIGGER_HIGH);
        aw_gpio_trigger_connect(p_devinfo->trigger_pin,
                                __hts221_alarm_callback,
                                p_dev);
    }

}

/******************************************************************************/
/** \brief 第三次初始化传感器服务获取方法 */
aw_local void __hts221_inst_connect (struct awbl_dev *p_dev)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_dev);

    uint8_t hts221_id = 0;

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    ret = __hts221_read(p_this, __HTS221_REG_WHO_IM_I, &hts221_id, 1);
    if (ret != AW_OK || hts221_id != __HTS221_MY_ID) {
        cur_ret = ret;
    } else {

        /* 获取湿度校准值 */
        ret = __hts221_get_hum_cal(p_this);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        /* 获取温度校准值 */
        ret = __hts221_get_tem_cal(p_this);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    }

    if (cur_ret != AW_OK) {
        aw_kprintf("\r\nSensor HTS221 Init is ERROR! \r\n");
    }
}


/******************************************************************************/
/** \brief 从指定通道获取采样数据 */
aw_local aw_err_t __hts221_data_get (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_cookie);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    awbl_hts221_calibration_data_t *hum = &p_this->cal_val[0];
    awbl_hts221_calibration_data_t *tem = &p_this->cal_val[1];

    aw_err_t ret = AW_OK;

    uint8_t status_val = 0;

    uint8_t reg_data[2] = {0};

    int16_t hum_data = 0;
    int16_t tem_data = 0;

    int cur_id = p_ids[0] - p_devinfo->start_id;
    int i = 0;

    if (AW_BIT_GET(p_this->trigger, 7) != 1) {
        return -AW_EPERM;
    }

    /* 若打开数据准备就绪触发方式，则直接赋值 */
    if ((AW_BIT_GET(p_this->trigger, 2)) &&
            ((p_this->flags[0] & AW_SENSOR_TRIGGER_DATA_READY) ||
             (p_this->flags[1] & AW_SENSOR_TRIGGER_DATA_READY))) {
        p_buf->val = p_this->current_data[cur_id].val;
        p_buf->unit= p_this->current_data[cur_id].unit;
        return AW_OK;
    }

    /* 获取可读状态值 */
    do {
        ret = __hts221_read(p_this, __HTS221_REG_STATUS, &status_val, 1);
        if (ret != AW_OK) {
            return ret;
        }
    } while (__HTS221_GET_HUM_STATUS(status_val) != 0x1 ||
             __HTS221_GET_TEM_STATUS(status_val) != 0x1 );

    for ( i = 0; i < num; i++ ) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id == 0) {

            /* 获取湿度*/
            ret = __hts221_read(p_this, __HTS221_REG_H_OUT_L, reg_data, 2);
            if (ret != AW_OK) {
                return ret;
            }
            hum_data = __HTS221_UINT8_TO_UINT16(reg_data);
            p_buf[i].val  = __GET_VALUE(hum_data,
                                        hum->x0,
                                        hum->y0,
                                        hum->x1,
                                        hum->y1); /* 湿度 */
                                        
            /* 单位默认为-6:10^(-6)*/
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO; 

        } else if (cur_id == 1) {

            /* 获取温度 */
            ret = __hts221_read(p_this, __HTS221_REG_T_OUT_L, reg_data, 2);
            if (ret != AW_OK) {
                return ret;
            }
            tem_data = __HTS221_UINT8_TO_UINT16(reg_data);
            p_buf[i].val  = __GET_VALUE(tem_data,
                                        tem->x0,
                                        tem->y0,
                                        tem->x1,
                                        tem->y1); /* 温度 */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO; /* 单位默认为-6:10^(-6)*/

        } else {
            break;              /* 若此次通道不属于该传感器，直接退出 */
        }
    }

    ret = __hts221_read(p_this, __HTS221_REG_CTRL_REG2, &status_val, 1);
    if (ret != AW_OK) {
        return ret;
    }

    status_val |= __HTS221_ONE_SHOT_START;
    ret = __hts221_write(p_this, __HTS221_REG_CTRL_REG2, &status_val, 1);
    if (ret != AW_OK) {
        return ret;
    }

    return ret;
}

/******************************************************************************/
/** \brief 使能该传感器的一个或多个通道 */
aw_local aw_err_t __hts221_enable (void            *p_cookie,
                                   const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_result)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_cookie);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    int i = 0;
    int cur_id = 0;

    uint8_t ctrl_reg1 = 0;
    uint8_t open_one_shot = 0;

    aw_err_t ret = AW_OK;
    aw_err_t curent_ret = AW_OK;

    ret = __hts221_read(p_this, __HTS221_REG_CTRL_REG1, &ctrl_reg1, 1);
    if (ret != AW_OK) {
        curent_ret = ret;
    }

    /** \brief 使能该通道 */
    ctrl_reg1 |= __HTS221_START;
    ret = __hts221_write(p_this, __HTS221_REG_CTRL_REG1, &ctrl_reg1, 1);
    if (ret != AW_OK) {
        curent_ret = ret;
    }

    if (curent_ret == AW_OK) {
        AW_BIT_SET(p_this->trigger, 7);
        AW_BIT_SET(p_this->trigger, 6);
        AW_BIT_SET(p_this->trigger, 5);
    }

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id < 0 || cur_id > 1) {
            break;              /* 若此次通道不属于该传感器，直接退出 */
        }

        /* 设置返回值 */
        if (p_result != NULL) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], curent_ret);
        }
    }

    ret = __hts221_read(p_this, __HTS221_REG_CTRL_REG2, &open_one_shot, 1);
    if (ret != AW_OK) {
        curent_ret = ret;
    }

    open_one_shot |= __HTS221_ONE_SHOT_START;
    ret = __hts221_write(p_this, __HTS221_REG_CTRL_REG2, &open_one_shot, 1);
    if (ret != AW_OK) {
        curent_ret = ret;
    }

    return curent_ret;
}

/******************************************************************************/
/** \brief 禁能传感器通道 */
aw_local aw_err_t __hts221_disable (void            *p_cookie,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_result)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_cookie);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    int i = 0;
    int cur_id = 0;

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint8_t ctrl_reg = 0;

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id == 0) {

            AW_BIT_CLR(p_this->trigger, 6);

        } else if (cur_id == 1) {

            AW_BIT_CLR(p_this->trigger, 5);

        } else {
            break;                  /* 若此次通道不属于该传感器，退出 */
        }

        if (p_result != NULL) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], AW_OK);
        }
    }

    if ((AW_BIT_GET(p_this->trigger, 6) == 0)
            && (AW_BIT_GET(p_this->trigger, 5) == 0)) {

        ret = __hts221_read(p_this, __HTS221_REG_CTRL_REG1, &ctrl_reg, 1);
        if (ret != AW_OK){
            cur_ret = ret;
        }

        /* 关闭该通道 */
        ctrl_reg &= __HTS221_CLOSE;
        ret = __hts221_write(p_this, __HTS221_REG_CTRL_REG1, &ctrl_reg, 1);
        if (ret != AW_OK){
            cur_ret = ret;
        }

        if (cur_ret == AW_OK) {
            AW_BIT_CLR(p_this->trigger, 7);
        }
    }

    return cur_ret;
}

/******************************************************************************/
/** \brief 属性的设置 */
aw_local aw_err_t __hts221_attr_set (void                  *p_cookie,
                                     int                    id,
                                     int                    attr,
                                     const aw_sensor_val_t *p_val)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_cookie);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    aw_sensor_val_t sensor_val = {0, 0};

    aw_sensor_val_t __sampling_rate = {0, 0};

    uint8_t rate = 0;

    if (id < p_devinfo->start_id || id > (p_devinfo->start_id + 1)) {
        return -AW_ENODEV;
    }

    switch (attr) {

    /* 修改频率 */
    case AW_SENSOR_ATTR_SAMPLING_RATE:

        if (sensor_val.val < 0 || sensor_val.val > 13) {
            return -AW_ENOTSUP;
        }

        sensor_val.unit = p_val->unit;
        sensor_val.val  = p_val->val;
        aw_sensor_val_unit_convert(&sensor_val, 1, AW_SENSOR_UNIT_BASE);

        if (sensor_val.val == 0) {
            rate = 0;
            __sampling_rate.val = 0;
        } else if (sensor_val.val > 0 && sensor_val.val < 4) {
            rate = 1;
            __sampling_rate.val = 1;
        } else if (sensor_val.val >= 4 && sensor_val.val < 10) {
            rate = 2;
            __sampling_rate.val = 7;
        } else if (sensor_val.val >= 10 && sensor_val.val <= 13) {
            rate = 3;
            __sampling_rate.val = 125;
            __sampling_rate.unit = AW_SENSOR_UNIT_DECI;
        }

        ret = __reg_attr_set(p_this, rate);
        if (ret != AW_OK) {
            cur_ret = ret;
        } else {
            p_this->sampling_rate.val  = __sampling_rate.val;
            p_this->sampling_rate.unit = __sampling_rate.unit;
        }

        break;

    default:
        cur_ret = -AW_ENOTSUP;
        break;
    }

    return cur_ret;
}

/******************************************************************************/
/** \brief 获取属性 */
aw_local aw_err_t __hts221_attr_get (void            *p_cookie,
                                     int              id,
                                     int              attr,
                                     aw_sensor_val_t *p_val)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_cookie);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;

    if (id < p_devinfo->start_id || id > (p_devinfo->start_id + 1)) {
        return -AW_ENODEV;
    }

    switch (attr) {

    /* 获取采样频率 */
    case AW_SENSOR_ATTR_SAMPLING_RATE:

        p_val->val  = p_this->sampling_rate.val;
        p_val->unit = p_this->sampling_rate.unit;
        break;

    default:
        ret = -AW_ENOTSUP;
        break;

    }

    return ret;
}

/******************************************************************************/
/**
 * \brief 触发配置
 *
 * \note 只支持一路触发，若再次调用会覆盖之前设置的
 */
aw_local aw_err_t __hts221_trigger_cfg (void                  *p_cookie,
                                        int                    id,
                                        uint32_t               flags,
                                        aw_sensor_trigger_cb_t pfn_cb,
                                        void                  *p_arg)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_cookie);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    int cur_id = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id > 1) {
        return -AW_ENODEV;
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

/******************************************************************************/
/** \brief 打开触发 */
aw_local aw_err_t __hts221_trigger_on (void *p_cookie, int id)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_cookie);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint8_t reg_data = 0;

    if (id < p_devinfo->start_id || id > p_devinfo->start_id + 1) {
        return -AW_EAGAIN;
    }

    if (p_this->trigger & 0x4) {
        return AW_OK;
    }

    ret = __hts221_read(p_this, __HTS221_REG_CTRL_REG3, &reg_data, 1);
    if ( ret != AW_OK) {
        cur_ret = ret;
    }

    /** \brief 使能数据可读触发 */
    reg_data |= __HTS221_TRIGGER_READ;
    ret = __hts221_write(p_this, __HTS221_REG_CTRL_REG3, &reg_data, 1);
    if ( ret != AW_OK) {
        cur_ret = ret;
    }

    if (p_devinfo->trigger_pin != -1) {
        ret = aw_gpio_trigger_on(p_devinfo->trigger_pin);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    }

    if (cur_ret == AW_OK && id == p_devinfo->start_id) {
        AW_BIT_SET(p_this->trigger, 0);
        AW_BIT_SET(p_this->trigger, 2);
    }
    if (cur_ret == AW_OK && id == (p_devinfo->start_id + 1)) {
        AW_BIT_SET(p_this->trigger, 1);
        AW_BIT_SET(p_this->trigger, 2);
    }

    return cur_ret;
}

/******************************************************************************/
/** \brief 关闭触发 */
aw_local aw_err_t __hts221_trigger_off (void *p_cookie, int id)
{
    awbl_hts221_dev_t *p_this = __MY_GET_DEV(p_cookie);
    awbl_hts221_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint8_t reg_data = 0;

    aw_sensor_val_t p_val = {0, 0};

    int cur_id = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id > 1) {
        return -AW_EAGAIN;
    }

    p_this->pfn_trigger_cb[cur_id] = NULL;
    p_this->p_arg[cur_id]          = NULL;
    p_this->flags[cur_id]          = 0;

    AW_BIT_CLR(p_this->trigger, cur_id);

    if ((p_this->trigger & 0x3) == 0) {

        ret = __hts221_read(p_this, __HTS221_REG_CTRL_REG3, &reg_data, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        /** \brief 禁能数据可读触发 */
        reg_data &= __HTS221_TRIGGER_CLOSE;
        ret = __hts221_write(p_this, __HTS221_REG_CTRL_REG3, &reg_data, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        ret = __hts221_attr_set(p_cookie,
                                id, 
                                AW_SENSOR_ATTR_SAMPLING_RATE,
                               &p_val);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        if (p_devinfo->trigger_pin != -1) {
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
  extern functions
*******************************************************************************/
/**
 * \brief 将 HTS221 驱动注册到 AWBus 子系统中
 */
void awbl_hts221_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_hts221);
}


/* end of file */

