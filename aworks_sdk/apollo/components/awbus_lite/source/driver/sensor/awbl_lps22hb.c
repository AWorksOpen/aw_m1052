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
 * \brief LPS22HB 气压传感器驱动程序.
 *
 * \internal
 * \par modification history:
 * - 1.00 18-10-21  dsg, first implementation.
 * \endinternal
 */


/*******************************************************************************
  includes
*******************************************************************************/
#include "driver/sensor/awbl_lps22hb.h"
#include "aw_delay.h"
#include "awbl_plb.h"
#include "aw_gpio.h"
#include <math.h>

/*******************************************************************************
  defines
*******************************************************************************/

#define __LPS22HB_INT_CFG_ADDR      0x0B  /**< \brief INTTERUPT_CONF 寄存器地址 */
#define __LPS22HB_THS_P_L_ADDR      0x0C  /**< \brief THS_P_L 寄存器地址 */
#define __LPS22HB_THS_P_H_ADDR      0x0D  /**< \brief THS_P_H 寄存器地址 */
#define __LPS22HB_WHO_AM_I_ADDR     0x0F  /**< \brief WHO_AM_I 寄存器地址 */
#define __LPS22HB_CTRL_REG1_ADDR    0x10  /**< \brief CTRL_REG1 寄存器地址 */
#define __LPS22HB_CTRL_REG2_ADDR    0x11  /**< \brief CTRL_REG2 寄存器地址 */
#define __LPS22HB_CTRL_REG3_ADDR    0x12  /**< \brief CTRL_REG3 寄存器地址 */
#define __LPS22HB_FIFO_CTRL_ADDR    0x14  /**< \brief FIFO_CTRL 寄存器地址 */
#define __LPS22HB_REF_P_XL_ADDR     0x15  /**< \brief REF_P_XL 寄存器地址 */
#define __LPS22HB_REF_P_L_ADDR      0x16  /**< \brief REF_P_L 寄存器地址 */
#define __LPS22HB_REF_P_H_ADDR      0x17  /**< \brief REF_P_H 寄存器地址 */
#define __LPS22HB_RPDS_L_ADDR       0x18  /**< \brief RPDS_L 寄存器地址 */
#define __LPS22HB_RPDS_H_ADDR       0x19  /**< \brief RPDS_H 寄存器地址 */
#define __LPS22HB_RES_CONF_ADDR     0x1A  /**< \brief RES_CONF 寄存器地址 */
#define __LPS22HB_INT_SOURCE_ADDR   0x25  /**< \brief INT_SOURCE 寄存器地址 */
#define __LPS22HB_FIFO_STATUS_ADDR  0x26  /**< \brief FIFO_STATUS 寄存器地址 */
#define __LPS22HB_STATUS_ADDR       0x27  /**< \brief STATUS 寄存器地址 */
#define __LPS22HB_PRESS_OUT_XL_ADDR 0x28  /**< \brief PRESS_OUT_XL 寄存器地址 */
#define __LPS22HB_PRESS_OUT_L_ADDR  0x29  /**< \brief PRESS_OUT_L 寄存器地址 */
#define __LPS22HB_PRESS_OUT_H_ADDR  0x2A  /**< \brief PRESS_OUT_H 寄存器地址 */
#define __LPS22HB_TEMP_OUT_L_ADDR   0x2B  /**< \brief TEMP_OUT_L 寄存器地址 */
#define __LPS22HB_TEMP_OUT_H_ADDR   0x2C  /**< \brief TEMP_OUT_H 寄存器地址 */
#define __LPS22HB_LPFP_RES_ADDR     0x33  /**< \brief LPFP_RES 寄存器地址 */

/*******************************************************************************
  forward declarations
*******************************************************************************/

#define __LPS22HB_DEV_DECL(p_this, p_dev)    \
          awbl_lps22hb_dev_t *p_this =       \
          AW_CONTAINER_OF(p_dev, awbl_lps22hb_dev_t, dev)

#define __LPS22HB_PARAM_DECL(p_param, p_arg) \
          awbl_lps22hb_param_t *p_param =    \
         (awbl_lps22hb_param_t *)AWBL_DEVINFO_GET(p_arg)

/*******************************************************************************
  function declarations
*******************************************************************************/

/**
 * \brief 进行数据矫正
 */
aw_local aw_err_t __lps22hb_sample_fetch (void *p_dev);

/**
 * \brief 获取传感器数据
 */
aw_local aw_err_t __lps22hb_data_get (void            *p_dev,
                                      const int       *p_ids,
                                      int              num,
                                      aw_sensor_val_t *p_buf);

/**
 * \brief 配置传感器通道属性
 */
aw_local aw_err_t __lps22hb_attr_set (void                  *p_dev,
                                      int                    id,
                                      int                    attr,
                                      const aw_sensor_val_t *p_val);

/**
 * \brief 获取传感器通道属性
 */
aw_local aw_err_t __lps22hb_attr_get (void            *p_dev,
                                      int              id,
                                      int              attr,
                                      aw_sensor_val_t *p_val);

/**
 * \brief 设置触发，一个通道仅能设置一个触发回调函数
 */
aw_local aw_err_t  __lps22hb_trigger_cfg (void                   *p_dev,
                                          int                     id,
                                          uint32_t                flags,
                                          aw_sensor_trigger_cb_t  pfn_cb,
                                          void                   *p_arg);

/**
 * \brief 打开触发
 */
aw_local aw_err_t __lps22hb_trigger_on (void *p_dev, int id);

/**
 * \brief 关闭触发
 */
aw_local aw_err_t __lps22hb_trigger_off (void *p_dev, int id);

/**
 * \brief 初始化第三阶段
 */
aw_local void __lps22hb_inst_connect (awbl_dev_t *p_dev);

/**
 * \brief 获取LPS22HB 传感器服务方法
 */
aw_local void __lps22hb_sensorserv_get (awbl_dev_t *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/
aw_local void  __job_call(void *p_dev)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);
    __lps22hb_sample_fetch(p_this);
    p_this->data_ready = AW_TRUE;
    aw_gpio_trigger_on(p_param->alert_pin);
}

aw_local void __lps22hb_to_sys_defer(void *p_dev)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);
    if (p_this->data_ready == AW_TRUE) {
        p_this->pfn_trigger_cb(p_this->p_arg, 0);
    }
    aw_isr_defer_job_add(&p_this->g_djob);
    aw_gpio_trigger_off(p_param->alert_pin);
}

/**
 * \brief 实例传感器服务
 */
aw_local aw_const struct awbl_sensor_servfuncs __g_lps22hb_servfuncs = {
    __lps22hb_data_get,    /* 从指定通道获取采样数据 */
    NULL,                  /* 使能传感器的某一通道 */
    NULL,                  /* 禁能传感器通道 */
    __lps22hb_attr_set,    /* 配置传感器通道属性 */
    __lps22hb_attr_get,    /* 获取传感器通道属性 */
    __lps22hb_trigger_cfg, /* 设置触发，一个通道仅能设置一个触发回调函数 */
    __lps22hb_trigger_on,  /* 打开触发 */
    __lps22hb_trigger_off  /* 关闭触发 */
};

/**
 * \brief 实例初始化三个阶段
 */
aw_local aw_const struct awbl_drvfuncs __g_sensor_lps22hb_drvfuncs = {
    NULL,                  /* 初始化第一阶段 */
    NULL,                  /* 初始化第二阶段 */
    __lps22hb_inst_connect /* 初始化第三阶段 */
};

/**
 * \brief 驱动提供的方法
 */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

aw_local aw_const struct awbl_dev_method __g_lps22hb_dev_methods[] = {
    AWBL_METHOD(awbl_sensorserv_get, __lps22hb_sensorserv_get),
    AWBL_METHOD_END
};

/**
 * \brief 实例注册信息结构体
 */
aw_local aw_const awbl_plb_drvinfo_t __g_sensor_lps22hb_drv_registration = {
    {
        AWBL_VER_1,                         /* awb_ver */
        AWBL_BUSID_I2C,                     /* bus_id */
        AWBL_LPS22HB_NAME,                  /* p_drvname */
        &__g_sensor_lps22hb_drvfuncs,       /* p_busfuncs */
        __g_lps22hb_dev_methods,            /* p_methods */
        NULL                                /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  functions
*******************************************************************************/
aw_local int __pow10(uint16_t n)
{
    if (n == 0) {
        return 1;
    }
    return 10 * __pow10(n-1);
}

/**
 * \brief 设置门限值
 */
aw_local aw_err_t __lps22hb_threshold_set(void *p_dev)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);

    int32_t  press,press_t;
    int8_t   buf[3];
    uint8_t  rbuf[3];

    aw_err_t ret = AW_OK;

    while(1){
        aw_i2c_read(&p_this->i2c_lps22hb, __LPS22HB_STATUS_ADDR, rbuf, 1);
        if (rbuf[0] & 0x03) {
            break;
        }
    }

    aw_i2c_read(&p_this->i2c_lps22hb, __LPS22HB_PRESS_OUT_XL_ADDR, rbuf, 3);

    if(p_this->threshold.unit >= 0) {
        press = (int64_t)(p_this->threshold.val)
              * __pow10(p_this->threshold.unit);
    } else {
        press = (int64_t)(p_this->threshold.val)
              / __pow10(-p_this->threshold.unit);
    }

    press_t = (int64_t) press * 4096 / 100;
    buf[0]  = press_t & 0xff;
    buf[1]  = press_t >> 8  & 0xff;
    buf[2]  = press_t >> 16 & 0xff;

    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_REF_P_XL_ADDR, buf, 3);
    return ret;
}

/**
 * \brief   读取传感器UT数据
 */
aw_local aw_err_t __awbl_lps22hb_ut_read (void *p_dev)
{

    __LPS22HB_DEV_DECL(p_this, p_dev);

    int8_t   buf[2] = {0};
    int32_t  temp   = 0;
    aw_err_t ret    = AW_OK;

    ret |= aw_i2c_read(&(p_this->i2c_lps22hb),
                       __LPS22HB_TEMP_OUT_L_ADDR,
                       (uint8_t*)&buf[0],
                       1);
    ret |= aw_i2c_read(&(p_this->i2c_lps22hb),
                       __LPS22HB_TEMP_OUT_H_ADDR,
                       (uint8_t*)&buf[1],
                       1);
    if(ret) {
        return ret;
    }

    temp = (uint8_t)buf[0] + (int16_t)buf[1] * 256;
    p_this->data_cache[0] = temp;
    return AW_OK;
}

/**
 * \brief   读取传感器UP数据
 */
aw_local aw_err_t __awbl_lps22hb_up_read (void *p_dev)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);

    int8_t   buf[3] = {0};
    int64_t  press;
    aw_err_t ret = AW_OK;

    ret |= aw_i2c_read(&(p_this->i2c_lps22hb),
                       __LPS22HB_PRESS_OUT_XL_ADDR,
                       (uint8_t*)&buf[0],
                       1);
    ret |= aw_i2c_read(&(p_this->i2c_lps22hb),
                       __LPS22HB_PRESS_OUT_L_ADDR,
                       (uint8_t*)&buf[1],
                       1);
    ret |= aw_i2c_read(&(p_this->i2c_lps22hb),
                       __LPS22HB_PRESS_OUT_H_ADDR,
                       (uint8_t*)&buf[2],
                       1);
    if(ret) {
        return ret;
    }

    press = (int32_t)buf[2] * 65536         |
            ((uint16_t)buf[1] & 0xff) * 256 |
            ((uint8_t)buf[0]  & 0xff);

    p_this->data_cache[1] = press * 100 / 4096;
    ret = aw_i2c_read(&(p_this->i2c_lps22hb),
                      __LPS22HB_INT_SOURCE_ADDR,
                      (uint8_t*)&buf[0],
                      1);
    return AW_OK;
}

/**
 * \brief 进行数据校正
 */
aw_local aw_err_t __lps22hb_sample_fetch (void *p_dev)
{

    __LPS22HB_DEV_DECL(p_this, p_dev);

    aw_err_t ret = AW_OK;
    /* 温度和气压是同时获取的，所以一同读取出来 */
    /* 读取温度值 */
    ret = __awbl_lps22hb_ut_read(p_this);
    /* 读取未矫正的气压值 */
    ret = __awbl_lps22hb_up_read(p_this);

    return ret;
}

/**
 * \brief 获取传感器服务方法
 */
aw_local void __lps22hb_sensorserv_get (awbl_dev_t *p_dev, void *p_arg)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);

    awbl_sensor_service_t *p_sensor_serv = NULL;
    /*
     *  该传感器的通道分配
     */
    aw_local const awbl_sensor_type_info_t type_info[] = {
         {AW_SENSOR_TYPE_TEMPERATURE, 1},     /* 1路温度  */
         {AW_SENSOR_TYPE_PRESS,1}             /* 1路气压 */
    };

    aw_local const awbl_sensor_servinfo_t serv_info = {
         2,                                   /* 共支持2个通道  */
         type_info,
         AW_NELEMENTS(type_info)              /* 类型列表数目       */
    };

    p_sensor_serv              = &p_this->sensor_serv;
    p_sensor_serv->p_cookie    = p_dev;
    p_sensor_serv->p_next      = NULL;
    p_sensor_serv->p_servfuncs = &__g_lps22hb_servfuncs;
    p_sensor_serv->p_servinfo  = &serv_info;
    p_sensor_serv->start_id    = p_param->start_id;

    *((awbl_sensor_service_t **)p_arg) = p_sensor_serv;
}

/**
 * \brief 获取传感器数据
 */
aw_local aw_err_t __lps22hb_data_get (void            *p_dev,
                                      const int       *p_ids,
                                      int              num,
                                      aw_sensor_val_t *p_buf)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    aw_err_t ret    =  AW_OK;
    uint8_t  i      =  0;
    uint8_t rbuf[3] = {0};
    uint8_t  errcnt = 0;

    /* 传感器所测得的数据是有相互关联的，所以一起将数据读取出来，不区分通道 */
    if (p_this->sensor_serv.start_id     > *p_ids &&
        p_this->sensor_serv.start_id + 2 < *p_ids) {
        return -AW_ENOTSUP;
    }

    if (p_this->data_ready == AW_FALSE) {
        while ((rbuf[0] & 0x03) == 0) {
            ret = aw_i2c_read(&p_this->i2c_lps22hb,
                        __LPS22HB_STATUS_ADDR,
                        &rbuf[0],
                        1);
            if (ret != AW_OK) {
                errcnt++;
                aw_mdelay(5);
                if (errcnt == 20) {
                    return ret;
                }
            }
        }
        ret = __lps22hb_sample_fetch(p_this);
        if(ret != AW_OK){
            return ret;
        }

    }
    for (i = 0; i < num; i++) {
        if (p_ids[i] == p_this->sensor_serv.start_id) {

            p_buf[i].unit = AW_SENSOR_UNIT_CENTI;

            /* 根据传感器数据特性讲数据分开赋值给p_buf */
            p_buf[i].val = p_this->data_cache[0];

        }
        if (p_ids[i] == p_this->sensor_serv.start_id + 1) {

            p_buf[i].unit = AW_SENSOR_UNIT_BASE;

            p_buf[i].val = p_this->data_cache[1];

        }
    }

    p_this->data_ready = AW_FALSE;
    return AW_OK;
}

/**
 * \brief 配置传感器通道属性
 */
aw_local aw_err_t __lps22hb_attr_set (void                  *p_dev,
                                      int                    id,
                                      int                    attr,
                                      const aw_sensor_val_t *p_val)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);
    uint8_t  chan = 0;

    chan = id - p_param->start_id;

    switch (attr) {
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        if (chan != 1) {
            return -AW_ENOTSUP;
        }
        p_this->threshold_mod  = AW_SENSOR_ATTR_THRESHOLD_LOWER;
        p_this->threshold.val  = p_val->val;
        p_this->threshold.unit = p_val->unit;
        break;
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        if (chan != 1) {
            return -AW_ENOTSUP;
        }
        p_this->threshold_mod  = AW_SENSOR_ATTR_THRESHOLD_UPPER;
        p_this->threshold.val  = p_val->val;
        p_this->threshold.unit = p_val->unit;
        break;
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        if ((p_val->unit != 0) ||
            (chan        >  1)) {
            return -AW_ENOTSUP;
        }
        p_this->sampling_rate = p_val->val;
        break;
    default :
        break;
    }
    return AW_OK;
}

/**
 * \brief 获取传感器通道属性
 */
aw_local aw_err_t __lps22hb_attr_get (void            *p_dev,
                                      int              id,
                                      int              attr,
                                      aw_sensor_val_t *p_val)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);
    uint8_t  chan = 0;

    chan = id - p_param->start_id;

    switch (attr) {
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        if (chan != 1) {
            return -AW_ENOTSUP;
        }
        p_val->val  = p_this->threshold.val;
        p_val->unit = p_this->threshold.unit;
        break;
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        if (chan != 1) {
            return -AW_ENOTSUP;
        }
        p_val->val  = p_this->threshold.val;
        p_val->unit = p_this->threshold.unit;
        break;
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        if (p_val->unit != 0 ||
            chan != 1) {
            return -AW_ENOTSUP;
        }
        p_val->val = p_this->sampling_rate;
        break;
    default :
        break;
    }
    return AW_OK;
}

/**
 * \brief 传感器准备就绪触发配置属性
 */
aw_local void __lps22hb_ready_set_cfg (void *p_dev)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);

    uint8_t buf;

    buf = 0x50;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_INT_CFG_ADDR, &buf, 1);
    aw_mdelay(10);

    buf = (p_this->sampling_rate && 0x07) << 4;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG1_ADDR, &buf, 1);

    buf = 0x10 & 0xF7;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG2_ADDR, &buf, 1);

    buf = (((uint32_t)p_this->p_arg + 1) ) << 7 | 0x04;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG3_ADDR, &buf, 1);

    aw_isr_defer_job_init(&p_this->g_djob, __job_call, p_this);

    aw_gpio_trigger_connect(p_param->alert_pin,
                            __lps22hb_to_sys_defer,
                            p_this);
    aw_gpio_trigger_cfg(p_param->alert_pin,
                        (uint32_t)p_this->p_arg);
    aw_mdelay(100);
}

/**
 * \brief 传感器门限触发配置属性
 */
aw_local void __lps22hb_threshold_set_cfg (void *p_dev)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);

    uint8_t buf;

    buf = 0x2C | (2 >> (p_this->threshold_mod & 0x01));
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_INT_CFG_ADDR, &buf, 1);

    buf = 0x01;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_THS_P_L_ADDR, &buf, 1);

    buf = 0x00;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_THS_P_H_ADDR, &buf, 1);

    buf = (p_this->sampling_rate && 0x07) << 4;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG1_ADDR, &buf, 1);

    buf = 0x10 & 0xF7;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG2_ADDR, &buf, 1);

    buf = ((uint32_t)p_this->p_arg + 1)  << 7 |
          (2 >> (p_this->threshold_mod & 0x01));
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG3_ADDR, &buf, 1);

    aw_isr_defer_job_init(&p_this->g_djob, __job_call, p_this);

    aw_gpio_trigger_connect(p_param->alert_pin,
                            __lps22hb_to_sys_defer,
                            p_this);
    aw_gpio_trigger_cfg(p_param->alert_pin,
                       (uint32_t)p_this->p_arg);
    aw_mdelay(100);
    __lps22hb_threshold_set(p_this);
}

/**
 * \brief 传感器FIFO 触发配置属性
 */
aw_local void __lps22hb_fifo_set_cfg (void *p_dev)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);

    uint8_t buf;

    buf = 0x50;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_INT_CFG_ADDR, &buf, 1);

    buf = 0x00;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_THS_P_L_ADDR, &buf, 1);

    buf = 0x00;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_THS_P_H_ADDR, &buf, 1);

    buf = 0x00;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_FIFO_CTRL_ADDR, &buf, 1);

    buf = 0x2A;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_FIFO_CTRL_ADDR, &buf, 1);

    buf = (p_this->sampling_rate && 0x07) << 4;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG1_ADDR, &buf, 1);

    buf = 0x50 & 0xF7;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG2_ADDR, &buf, 1);

    buf =((((uint32_t)p_this->p_arg + 1) & 0x01) ) << 7 | 0x08;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG3_ADDR, &buf, 1);

    aw_gpio_trigger_connect(p_param->alert_pin,
                            (aw_pfuncvoid_t)p_this->pfn_trigger_cb,
                            p_this);
    aw_gpio_trigger_cfg(p_param->alert_pin,
                       (uint32_t)p_this->p_arg);
    aw_mdelay(100);
}

/**
 * \brief 触发配置
 *
 * \note 只支持一路触发，若再次调用会覆盖之前设置的
 */
aw_local aw_err_t __lps22hb_trigger_cfg (void                     *p_dev,
                                         int                       id,
                                         uint32_t                  flags,
                                         aw_sensor_trigger_cb_t    pfn_cb,
                                         void                     *p_arg)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);

    aw_err_t ret  = AW_OK;

    p_this->data_ready = AW_FALSE;

    switch(flags) {
    /* 准备就绪触发 */
    case AW_SENSOR_TRIGGER_DATA_READY:
        if (id == p_param->start_id) {
            p_this->alert_chan = 0;
        } else if (id == p_param->start_id + 1){
            p_this->alert_chan = 1;
        } else {
            return -AW_ENOTSUP;
        }
        p_this->pfn_trigger_cb = pfn_cb;
        p_this->p_arg          = p_arg;
        __lps22hb_ready_set_cfg(p_this);
        break;
    /* 门限触发 */
    case AW_SENSOR_TRIGGER_THRESHOLD:
        if (id == p_param->start_id + 1){
            p_this->alert_chan = 1;
        }  else {
            return -AW_ENOTSUP;
        }
        p_this->pfn_trigger_cb = pfn_cb;
        p_this->p_arg          = p_arg;
        __lps22hb_threshold_set_cfg(p_this);
        break;
    /* FIFO 触发 */
    case AW_LPS22HB_TRIGGER_FIFO:
        if (id == p_param->start_id) {
            p_this->alert_chan = 0;
        } else if (id == p_param->start_id + 1){
            p_this->alert_chan = 1;
        } else {
            return -AW_ENOTSUP;
        }
        p_this->pfn_trigger_cb = pfn_cb;
        p_this->p_arg          = p_arg;
        __lps22hb_fifo_set_cfg(p_this);
        break;
    default:
        ret = -AW_ENOTSUP;
        break;
    }

    return ret;
}

/*
 * \brief 打开触发
 */
aw_local aw_err_t __lps22hb_trigger_on (void *p_dev, int id)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);

    if((id - p_param->start_id) != p_this->alert_chan) {
        return -AW_EAGAIN;
    }
    if(p_param->alert_pin != -1) {
        return aw_gpio_trigger_on(p_param->alert_pin);
    } else {
        return -AW_ENOTSUP;
    }

}

/*
 * \brief 关闭触发
 */
aw_local aw_err_t __lps22hb_trigger_off(void *p_dev, int id)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);

    if((id - p_param->start_id) != p_this->alert_chan) {
        return -AW_EAGAIN;
    }

    if(p_param->alert_pin != -1) {
        return aw_gpio_trigger_off(p_param->alert_pin);
    } else {
        return -AW_ENOTSUP;
    }
}

/**
 * \brief 初始化第三阶段
 */
aw_local void __lps22hb_inst_connect (awbl_dev_t *p_dev)
{
    __LPS22HB_DEV_DECL(p_this, p_dev);
    __LPS22HB_PARAM_DECL(p_param, p_dev);

    uint8_t buf;

    /* 生成从机设备描述结构 */
    aw_i2c_mkdev(&(p_this->i2c_lps22hb),
                 p_dev->p_devhcf->bus_index,
                 p_param->addr,
                 (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));

    buf = 0x10;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG1_ADDR, &buf, 1);

    buf = 0x94 & 0xF7;
    aw_i2c_write(&p_this->i2c_lps22hb, __LPS22HB_CTRL_REG2_ADDR, &buf, 1);
}

/******************************************************************************/
/**
 * \brief 注册传感器
 */
void awbl_lps22hb_drv_register (void)
{
    awbl_drv_register((awbl_drvinfo_t *)&__g_sensor_lps22hb_drv_registration);
}

/* end of file */
