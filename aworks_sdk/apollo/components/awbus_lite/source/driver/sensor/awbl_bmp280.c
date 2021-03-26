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
 * \brief BMP280 气压传感器驱动程序.
 *
 * \internal
 * \par modification history:
 * - 1.00 18-10-16  dsg, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "driver/sensor/awbl_bmp280.h"
#include "awbl_plb.h"
#include "aw_delay.h"
#include <math.h>


/*******************************************************************************
  defines
*******************************************************************************/

#define __BMP280_ID_ADDR         0xD0  /**< \brief ID 寄存器地址 */
#define __BMP280_RESET_ADDR      0xE0  /**< \brief RESET 寄存器地址 */
#define __BMP280_STATUS_ADDR     0xF3  /**< \brief STATUS 寄存器地址 */
#define __BMP280_CTRL_MEAS_ADDR  0xF4  /**< \brief CTRL_MEAS 寄存器地址 */
#define __BMP280_COMFIG_ADDR     0xF5  /**< \brief CONFIG 寄存器地址 */
#define __BMP280_PRESS_ADDR      0xF7  /**< \brief PRESS 寄存器地址 */
#define __BMP280_TEMP_ADDR       0xFA  /**< \brief TEMP 寄存器地址 */

#define __BMP280_CALI_ADDR 0x88   /**< \brief 校准系数寄存器开始地址 */

#define __BMP280_T1_ADDR   0x88   /**< \brief 校准系数 T1 寄存器地址 */
#define __BMP280_T2_ADDR   0x8A   /**< \brief 校准系数 T2 寄存器地址 */
#define __BMP280_T3_ADDR   0x8C   /**< \brief 校准系数 T3 寄存器地址 */
#define __BMP280_P1_ADDR   0x8E   /**< \brief 校准系数 P1 寄存器地址 */
#define __BMP280_P2_ADDR   0x90   /**< \brief 校准系数 P2 寄存器地址 */
#define __BMP280_P3_ADDR   0x92   /**< \brief 校准系数 P3 寄存器地址 */
#define __BMP280_P4_ADDR   0x94   /**< \brief 校准系数 P4 寄存器地址 */
#define __BMP280_P5_ADDR   0x96   /**< \brief 校准系数 P5 寄存器地址 */
#define __BMP280_P6_ADDR   0x98   /**< \brief 校准系数 P6 寄存器地址 */
#define __BMP280_P7_ADDR   0x9A   /**< \brief 校准系数 P7 寄存器地址 */
#define __BMP280_P8_ADDR   0x9C   /**< \brief 校准系数 P8 寄存器地址 */
#define __BMP280_P9_ADDR   0x9E   /**< \brief 校准系数 P9 寄存器地址 */

#define __BMP280_SLEEP_MODE  0
#define __BMP280_FORCED_MODE 1
#define __BMP280_NORMAL_MODE 3

/*******************************************************************************
  forward declarations
*******************************************************************************/

#define __BMP280_DEV_DECL(p_this, p_dev) \
          awbl_bmp280_dev_t *p_this = \
          AW_CONTAINER_OF(p_dev, awbl_bmp280_dev_t, dev)

#define __BMP280_PARAM_DECL(p_param, p_arg) \
          awbl_bmp280_param_t *p_param = \
         (awbl_bmp280_param_t *)AWBL_DEVINFO_GET(p_arg)

/*******************************************************************************
  function declarations
*******************************************************************************/
/**
 * \brief 进行数据矫正
 */
aw_local aw_err_t __bmp280_sample_fetch (void *p_dev, int chan);

/**
 * \brief 获取传感器数据
 */
aw_local aw_err_t __bmp280_data_get (void            *p_dev,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf);
/**
 * \brief 使能传感器的某一通道
 */
aw_local aw_err_t __bmp280_enable (void            *p_dev,
                                   const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_result);
/**
 * \brief 使能传感器的某一通道
 */
aw_local aw_err_t __bmp280_disable (void            *p_dev,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_result);
/**
 * \brief 初始化第三阶段
 */
aw_local void __bmp280_inst_connect (awbl_dev_t *p_dev);

/**
 * \brief 获取BMP280 传感器服务方法
*/
aw_local void __bmp280_sensorserv_get(awbl_dev_t *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/
/**
 * \brief 实例传感器服务
 */
aw_local aw_const struct awbl_sensor_servfuncs __g_bmp280_servfuncs = {
    __bmp280_data_get,    /* 从指定通道获取采样数据 */
    __bmp280_enable,                 /* 使能传感器的某一通道 */
    __bmp280_disable,                 /* 禁能传感器通道 */
    NULL,                 /* 配置传感器通道属性 */
    NULL,                 /* 获取传感器通道属性 */
    NULL,                 /* 设置触发，一个通道仅能设置一个触发回调函数 */
    NULL,                 /* 打开触发 */
    NULL                  /* 关闭触发 */
};

/**
 * \brief 实例初始化三个阶段
 */
aw_local aw_const struct awbl_drvfuncs __g_sensor_bmp280_drvfuncs = {
    NULL,  /* 初始化第一阶段 */
    NULL,  /* 初始化第二阶段 */
    __bmp280_inst_connect /* 初始化第三阶段 */
};

/**
 * \brief 驱动提供的方法
 */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);
aw_local aw_const struct awbl_dev_method __g_bmp280_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __bmp280_sensorserv_get),
        AWBL_METHOD_END
};

/**
 * \brief 实例注册信息结构体
 */
aw_local aw_const awbl_plb_drvinfo_t __g_sensor_bmp280_drv_registration = {
    {
        AWBL_VER_1,                        /* awb_ver */
        AWBL_BUSID_I2C,                    /* bus_id */
        AWBL_BMP280_NAME,                  /* p_drvname */
        &__g_sensor_bmp280_drvfuncs,       /* p_busfuncs */
        __g_bmp280_dev_methods,            /* p_methods */
        NULL                               /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  functions
*******************************************************************************/

/**
 * \brief   读取传感器校准系数
 */
aw_local aw_err_t __awbl_bmp280_cali_read (void *p_dev)
{
    __BMP280_DEV_DECL(p_this, p_dev);
    awbl_bmp280_cali_t *p_cali = &(p_this->bmp280_cali);

    uint8_t buf[24] = {0};

    aw_err_t ret = AW_OK;

    ret = aw_i2c_read(&(p_this->i2c_bmp280), __BMP280_CALI_ADDR, buf, sizeof(buf));
    if(ret) {
        return ret;
    }

    p_cali->t1 = ((uint16_t)buf[1]  << 8) | buf[0];
    p_cali->t2 = ((uint16_t)buf[3]  << 8) | buf[2];
    p_cali->t3 = ((uint16_t)buf[5]  << 8) | buf[4];
    p_cali->p1 = ((uint16_t)buf[7]  << 8) | buf[6];
    p_cali->p2 = ((uint16_t)buf[9]  << 8) | buf[7];
    p_cali->p3 = ((uint16_t)buf[11] << 8) | buf[10];
    p_cali->p4 = ((uint16_t)buf[13] << 8) | buf[12];
    p_cali->p5 = ((uint16_t)buf[15] << 8) | buf[14];
    p_cali->p6 = ((uint16_t)buf[17] << 8) | buf[16];
    p_cali->p7 = ((uint16_t)buf[19] << 8) | buf[18];
    p_cali->p8 = ((uint16_t)buf[21] << 8) | buf[20];
    p_cali->p9 = ((uint16_t)buf[23] << 8) | buf[22];

    return AW_OK;
}
/**
 * \brief 使能传感器的某一通道
 */
aw_local aw_err_t __bmp280_enable (void            *p_dev,
                                   const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_result)
{
    __BMP280_DEV_DECL(p_this, p_dev);
    aw_err_t ret = AW_OK;

    uint16_t mode = 0x6C | __BMP280_NORMAL_MODE;
    ret = aw_i2c_write(&(p_this->i2c_bmp280), __BMP280_CTRL_MEAS_ADDR, &mode, 1);
    return ret;
    return AW_OK;
    return AW_OK;
}

/**
 * \brief 禁能传感器通道
 */
aw_local aw_err_t __bmp280_disable (void            *p_dev,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_result)
{
    __BMP280_DEV_DECL(p_this, p_dev);
    aw_err_t ret = AW_OK;

    uint16_t mode = 0x6C | __BMP280_SLEEP_MODE;
    ret = aw_i2c_write(&(p_this->i2c_bmp280), __BMP280_CTRL_MEAS_ADDR, &mode, 1);
    return ret;
    return AW_OK;
}

/**
 * \brief   读取传感器UT数据
 */
aw_local aw_err_t __awbl_bmp280_ut_read (void *p_dev)
{

    __BMP280_DEV_DECL(p_this, p_dev);

    awbl_bmp280_cali_t *p_cali = &(p_this->bmp280_cali);

    uint8_t buf[3] = {0};

    aw_err_t ret = AW_OK;

    ret = aw_i2c_read(&(p_this->i2c_bmp280),
                      __BMP280_TEMP_ADDR,
                        buf,
                        sizeof(buf));
    if(ret) {
        return ret;
    }

    p_cali->ut = ((int32_t)buf[0] << 12) |
                   buf[1] << 4           |
                   buf[2] >> 4;

    return AW_OK;
}

/**
 * \brief   读取传感器UP数据
 */
aw_local aw_err_t __awbl_bmp280_up_read (void *p_dev)
{

    __BMP280_DEV_DECL(p_this, p_dev);

    awbl_bmp280_cali_t *p_cali = &(p_this->bmp280_cali);

    uint8_t buf[3] = {0};

    aw_err_t ret = AW_OK;

    ret = aw_i2c_read(&(p_this->i2c_bmp280),
                      __BMP280_PRESS_ADDR,
                      buf,
                      sizeof(buf));
    if(ret) {
        return ret;
    }

    p_cali->up = ((int32_t)buf[0] << 12) |
                   buf[1] << 4           |
                   buf[2] >> 4;

    return AW_OK;

}

/**
 * \brief 进行数据校正
 */
aw_local aw_err_t __bmp280_sample_fetch (void *p_dev, int chan)
{

    __BMP280_DEV_DECL(p_this, p_dev);

    awbl_bmp280_cali_t *p_cali = &(p_this->bmp280_cali);

    int32_t  var1, var2;      /* 温度 换算中间值 */
    int32_t  t_fine;          /* 温度或气压 换算中间值 */
    int32_t  temp;
    int64_t  var3, var4, press;

    aw_err_t ret = AW_OK;

    /* 传感器所测得的数据是有相互关联的，所以一起将数据读取出来，不区分通道 */
    if (p_this->sensor_serv.start_id     > chan &&
        p_this->sensor_serv.start_id + 2 < chan) {
        return -AW_ENOTSUP;
    }

    /* 读取未矫正的温度值 */
    ret = __awbl_bmp280_ut_read(p_this);
    if(ret) {
        return ret;
    }

    /* 读取未矫正的气压值 */
    ret = __awbl_bmp280_up_read(p_this);
    if(ret) {
        return ret;
    }

    /* 将未矫正的温度值转换成矫正的温度值 */
    var1   = (((p_cali->ut >> 3) - ((int32_t)p_cali->t1 << 1))
           * p_cali->t2) >> 11;
    var2   = (((((p_cali->ut) >> 4) - (int32_t)p_cali->t1)
           * (((p_cali->ut) >> 4) - (int32_t)p_cali->t1) >> 12)
           * p_cali->t3) >>14;
    t_fine = var1 + var2;
    temp   = (t_fine * 5 + 128) >> 8;

    p_this->data_cache[0]  = temp;

    /* 将未矫正的气压值装换成矫正过后的气压值 */
    var3   = t_fine - 128000;
    var4   = var3 * var3 * (int64_t)p_cali->p6;
    var4   = var4 + ((var3 * (int64_t)p_cali->p5) << 17);
    var4   = var4 + ((int64_t)p_cali->p4 << 35);
    var3   = ((var3 * var3 * (int64_t)p_cali->p3) >> 8)
           + ((var3 * (int64_t)p_cali->p2) << 12);
    var3   = (((((int64_t)1) << 47) + var3)
           * ((int64_t)p_cali->p1)) >> 33;
    if (var3 == 0) {
        return 0;
    }
    press  = 1048576 - p_cali->up;
    press  = (((press << 31) - var4) * 3125) / var3;
    var3   = (((int64_t)p_cali->p9) * (press >> 13)
           * (press >> 13)) >> 25;
    var4   = (((int64_t)p_cali->p8) * press) >> 19;
    press  = ((press + var3 + var4) >> 8)
           + (((int64_t)p_cali->p7) << 4);

    p_this->data_cache[1] = press / 256;

    return ret;
}
/**
 * \brief 获取传感器服务方法
 */

aw_local void __bmp280_sensorserv_get(awbl_dev_t *p_dev, void *p_arg)
{
    __BMP280_DEV_DECL(p_this, p_dev);
    __BMP280_PARAM_DECL(p_param, p_dev);

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

    p_sensor_serv                  = &p_this->sensor_serv;
    p_sensor_serv->p_cookie        = p_dev;
    p_sensor_serv->p_next          = NULL;
    p_sensor_serv->p_servfuncs     = &__g_bmp280_servfuncs;
    p_sensor_serv->p_servinfo      = &serv_info;
    p_sensor_serv->start_id        = p_param->start_id;

    *((awbl_sensor_service_t **)p_arg) = p_sensor_serv;
}

/**
 * \brief 获取传感器数据
 */
aw_local aw_err_t __bmp280_data_get (void            *p_dev,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf)
{
    __BMP280_DEV_DECL(p_this, p_dev);
    aw_err_t ret = AW_OK;
    uint8_t  i;

    ret = __bmp280_sample_fetch(p_this, *p_ids);
    if(ret != AW_OK){
        return ret;
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
    return AW_OK;
}

/**
 * \brief 初始化第三阶段
 */
aw_local void __bmp280_inst_connect (awbl_dev_t *p_dev)
{
    __BMP280_DEV_DECL(p_this, p_dev);
    __BMP280_PARAM_DECL(p_param, p_dev);

    /* 设备初始化 */
    /* 生成从机设备描述结构 */
    aw_i2c_mkdev(&(p_this->i2c_bmp280),
                   p_dev->p_devhcf->bus_index,
                   p_param->addr,
                  (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));
    /* 读取校准数据 */
    __awbl_bmp280_cali_read(p_this);
}

/******************************************************************************/
/**
 * \brief 注册传感器
 */
void awbl_bmp280_drv_register (void)
{
    awbl_drv_register((awbl_drvinfo_t *)&__g_sensor_bmp280_drv_registration);
}

/* end of file */


