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
 * \brief 传感器 BMG160 驱动文件
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
 * 宏定义
 ******************************************************************************/

/* 辅助宏，通过基础设备指针获得具体设备指针 */
#define __BMG160_DEV_DECL(p_this,p_dev) struct awbl_bmg160_dev *(p_this) = \
                                        (struct awbl_bmg160_dev *)(p_dev)

/* 辅助宏，通过基础设备指针获得设备信息指针 */
#define __BMG160_DEVINFO_DECL(p_devinfo,p_dev) \
            struct awbl_bmg160_devinfo *(p_devinfo) = \
           (struct awbl_bmg160_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* 辅助宏，通过基础设备指针获得传感器服务指针 */
#define __BMG160_SENSORSERV_DECL(p_serv,p_dev) \
            struct awbl_sensor_service_t *(p_serv) = \
           (struct awbl_sensor_service_t *)&((p_dev)->sensor_serv)

#define __BMG160_CHIP_ID        (0x00)  /**< \brief 设备ID地址 */
#define __BMG160_MY_ID          (0x0F)  /**< \brief 设备ID */
#define __BMG160_RATE_X_LSB     (0x02)  /**< \brief 包含X通道角速率读数值的最低有效位 */
#define __BMG160_RATE_X_MSB     (0x03)  /**< \brief 包含X通道角速率读数值的最高有效位 */
#define __BMG160_RATE_Y_LSB     (0x04)  /**< \brief 包含Y通道角速率读数值的最低有效位 */
#define __BMG160_RATE_Y_MSB     (0x05)  /**< \brief 包含Y通道角速率读数值的最高有效位 */
#define __BMG160_RATE_Z_LSB     (0x06)  /**< \brief 包含Z通道角速率读数值的最低有效位 */
#define __BMG160_RATE_Z_MSB     (0x07)  /**< \brief 包含Z通道角速率读数值的最高有效位 */
#define __BMG160_TEMP           (0x08)  /**< \brief 包含当前芯片温度 */
#define __BMG160_INT_STATUS_0   (0x09)  /**< \brief 包含中断状态位 */
#define __BMG160_INT_STATUS_1   (0x0A)  /**< \brief 包含中断状态位 */
#define __BMG160_INT_STATUS_2   (0x0B)  /**< \brief 包含任何运动中断状态位 */
#define __BMG160_INT_STATUS_3   (0x0C)  /**< \brief 包含高速率中断状态位 */

#define __BMG160_FIFO_STATUS    (0x0E)  /**< \brief 包含FIFO状态标志 */
#define __BMG160_RANGE          (0x0F)  /**< \brief 包含BMG160支持5种不同的角速率测量范围 */
#define __BMG160_BW             (0x10)  /**< \brief 该寄存器允许选择速率数据滤波器带宽 */
#define __BMG160_LPM1           (0x11)  /**< \brief 选择主电源模式 */
#define __BMG160_LPM2           (0x12)  /**< \brief 快速上电和外部触发的配置设置 */
#define __BMG160_RATE_HBW       (0x13)  /**< \brief 角速率数据采集和数据输出格式 */
#define __BMG160_BGW_SOFTRESET  (0x14)	/**< \brief 控制用户触发的传感器复位 */ 
#define __BMG160_INT_EN_0       (0x15)  /**< \brief 控制启用哪些中断 */
#define __BMG160_INT_EN_1       (0x16)  /**< \brief 包含中断引脚配置 */
#define __BMG160_INT_MAP_0      (0x17)  /**< \brief 控制哪些中断信号映射到INT1引脚 */
#define __BMG160_INT_MAP_1      (0x18)  /**< \brief 控制哪些中断信号映射到INT1引脚和INT2引脚 */
#define __BMG160_INT_MAP_2      (0x19)  /**< \brief 控制哪些中断信号映射到INT2引脚 */
#define __BMG160_INTR_ZERO_ADDR (0x1A)  /**< \brief 控制具有可选数据源的那些中断的数据源定义  */
#define __BMG160_INTR_ONE_ADDR  (0x1B)  /**< \brief 控制快速偏移补偿的数据源定义和任何运动阈值  */
#define __BMG160_INTR_TWO_ADDR  (0x1C)  /**< \brief  */
#define __BMG160_INTR_4_ADDR    (0x1E)  /**< \brief  */

#define __BMG160_INT_RST_LATCH  (0x21)  /**< \brief 包含中断复位位和中断模式选择 */
#define __BMG160_High_Th_x      (0x22)  /**< \brief 包含x轴的高速率阈值和高速率滞后设置 */
#define __BMG160_High_Dur_x     (0x23)  /**< \brief 包含x轴的高速率持续时间设置 */
#define __BMG160_High_Th_y      (0x24)  /**< \brief 包含y轴的高速率阈值和高速率滞后设置 */
#define __BMG160_High_Dur_y     (0x25)  /**< \brief 包含y轴的高速率持续时间设置 */
#define __BMG160_High_Th_z      (0x26)  /**< \brief 包含z轴的高速率阈值和高速率滞后设置 */
#define __BMG160_High_Dur_z     (0x27)  /**< \brief 包含z轴的高速率持续时间设置 */
                              
#define __BMG160_SoC            (0x31)  /**< \brief 包含慢速偏移取消设置 */
#define __BMG160_A_FOC          (0x32)  /**< \brief 包含快速偏移取消设置 */
#define __BMG160_TRIM_NVM_CTRL  (0x33)  /**< \brief 包含几次可编程非易失性存储器（NVM）的控制设置 */
#define __BMG160_BGW_SPI3_WDT   (0x34)  /**< \brief 包含数字接口的设置 */
                              
#define __BMG160_OFC1           (0x36)  /**< \brief 包含偏移补偿值 */
#define __BMG160_OFC2           (0x37)  /**< \brief 包含X通道的偏移补偿值 */
#define __BMG160_OFC3           (0x38)  /**< \brief 包含Y通道的偏移补偿值 */
#define __BMG160_OFC4           (0x39)  /**< \brief 包含Z通道的偏移补偿值 */
#define __BMG160_TRIM_GP0       (0x3A)  /**< \brief 包含具有NVM备份的通用数据寄存器 */
#define __BMG160_TRIM_GP1       (0x3B)  /**< \brief 包含具有NVM备份的通用数据寄存器 */
#define __BMG160_BIST           (0x3C)  /**< \brief 包含内置自测（BIST）功能 */
#define __BMG160_FIFO_CONFIG_0  (0x3D)  /**< \brief 包含FIFOwatermark级别 */
#define __BMG160_FIFO_CONFIG_1  (0x3E)  /**< \brief 包含FIFO配置设置 */
#define __BMG160_FIFO_DATA      (0x3F)  /**< \brief FIFO数据读出寄存器 */

/** \brief 将两个int8转换为一个int16_t类型 */
#define __BMG160_UINT8_TO_UINT16(buff) \
                       (int16_t)(((uint16_t)(buff[1]) << 8) \
                               | ((uint16_t)(buff[0])))
                                                          
/** \brief X轴角速度测量 */ 
#define __BMG160_X_START       (0x2<<0)  
/** \brief Y轴角速度测量 */ 
#define __BMG160_Y_START       (0x2<<2)                   
/** \brief Z轴角速度测量 */ 
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
 * 本地函数声明
 ******************************************************************************/

/** \brief 获取传感器通道采样数据 */
aw_local aw_err_t __pfn_data_get (void            *p_drv,
                                  const int       *p_ids,
                                  int              num,
                                  aw_sensor_val_t *p_buf);

/** \brief 使能传感器通道 */
aw_local aw_err_t __pfn_enable (void            *p_drv,
                                const int       *p_ids,
                                int              num,
                                aw_sensor_val_t *p_result);

/** \brief 禁能传感器通道 */
aw_local aw_err_t __pfn_disable (void            *p_drv,
                                 const int       *p_ids,
                                 int              num,
                                 aw_sensor_val_t *p_result);

/** \brief 配置传感器通道属性 */
aw_local aw_err_t __pfn_attr_set (void                  *p_drv,
                                  int                    id,
                                  int                    attr,
                                  const aw_sensor_val_t *p_val);

/** \brief 获取传感器通道属性 */
aw_local aw_err_t __pfn_attr_get (void            *p_drv,
                                  int              id,
                                  int              attr,
                                  aw_sensor_val_t *p_val);

/** \brief 设置触发，一个通道仅能设置一个触发回调函数 */
aw_local aw_err_t __pfn_trigger_cfg (void                   *p_drv,
                                     int                     id,
                                     uint32_t                flags,
                                     aw_sensor_trigger_cb_t  pfn_cb,
                                     void                   *p_arg);

/** \brief 打开触发 */
aw_local aw_err_t __pfn_trigger_on (void *p_drv, int id);

/** \brief 关闭触发 */
aw_local aw_err_t __pfn_trigger_off (void *p_drv, int id);

/**
 * \brief 第3阶段初始化
 */
aw_local void __bmg160_inst_connect (awbl_dev_t *p_dev);

/**
 * \brief BMG160 标准服务接口获取
 */
aw_local aw_err_t __bmg160_sensorserv_get (struct awbl_dev  *p_dev,
                                           void             *p_arg);
/** \breif 中断回调函数 */
aw_local void __bmg160_alarm_callback (void *p_arg);

/** \breif 中断延迟函数 */
aw_local void __isr_defer_handle (void *p_arg);

/**
 * \brief BMG160 写数据
 */
aw_local aw_err_t __bmg160_write (awbl_bmg160_dev_t *p_this,
                                  uint32_t           subaddr,
                                  uint8_t           *p_buf,
                                  uint32_t           nbytes);
/**
 * \brief BMG160 读数据
 */
aw_local aw_err_t __bmg160_read (awbl_bmg160_dev_t *p_this,
                                 uint32_t           subaddr,
                                 uint8_t           *p_buf,
                                 uint32_t           nbytes);

/******************************************************************************/

/**
 *\brief BMG160的驱动函数入口
 */
aw_local aw_const struct awbl_drvfuncs __g_bmg160_drvfuncs = {
    NULL,
    NULL,
    __bmg160_inst_connect
};

/** \brief 传感器类型信息 */
aw_local aw_const awbl_sensor_type_info_t __g_bmg160_typeinfo[] = {
    {AW_SENSOR_TYPE_GYROSCOPE, 3},
};

/** \brief 驱动服务信息 */
aw_local aw_const awbl_sensor_servinfo_t __g_bmg160_servinfo = {
    3,
    __g_bmg160_typeinfo,
    AW_NELEMENTS(__g_bmg160_typeinfo),
};

/** \brief 服务函数 */
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

/** \brief 驱动提供的方法 */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);
aw_local aw_const struct awbl_dev_method __g_bmg160_methods[] = {
    {AWBL_METHOD_CALL(awbl_sensorserv_get), __bmg160_sensorserv_get},
    AWBL_METHOD_END
};

/** \brief 驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_bmg160_drvinfo = {
    AWBL_VER_1,
    AWBL_BUSID_I2C,
    AWBL_BMG160_NAME,
    &__g_bmg160_drvfuncs,
    __g_bmg160_methods,
    NULL
};

/**
 * \brief BMG160 标准服务接口获取
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
 * \brief 第3阶段初始化
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
 * \brief BMG160 写数据
 */
aw_local aw_err_t __bmg160_write (awbl_bmg160_dev_t *p_this,
                                  uint32_t           subaddr,
                                  uint8_t           *p_buf,
                                  uint32_t           nbytes)
{
    return aw_i2c_write(&p_this->i2c_dev, subaddr, p_buf, nbytes);
}

/**
 * \brief BMG160 读数据
 */
aw_local aw_err_t __bmg160_read (awbl_bmg160_dev_t *p_this,
                                 uint32_t           subaddr,
                                 uint8_t           *p_buf,
                                 uint32_t           nbytes)
{
    return aw_i2c_read(&p_this->i2c_dev, subaddr, p_buf, nbytes);
}

/** \breif 中断回调函数 */
aw_local void __bmg160_alarm_callback (void *p_arg)
{
    awbl_bmg160_dev_t* p_this = (awbl_bmg160_dev_t*)p_arg;

    aw_isr_defer_job_add(&p_this->g_myjob);   /*< \brief 添加中断延迟处理任务 */
}

/** \breif 求传感器当前精度 */
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

/** \breif 中断延迟函数 */
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

    /** \brief 获取X轴角速度值 */
    __bmg160_read(p_this, __BMG160_RATE_X_LSB, current_data, 6);

    /* 获取X轴角速度 */
    x_axis_data = __BMG160_UINT8_TO_UINT16(current_data);
    p_this->data[0].val = x_axis_data * resolu * 100;
    p_this->data[0].unit = AW_SENSOR_UNIT_MICRO;

    /* 获取Y轴角速度 */
    y_axis_data = __BMG160_UINT8_TO_UINT16((current_data+2));
    p_this->data[1].val = y_axis_data * resolu * 100;
    p_this->data[1].unit = AW_SENSOR_UNIT_MICRO;

    /* 获取Z轴角速度 */
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

/** \brief 获取传感器通道采样数据 */
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

        /* 若打开数据准备就绪触发方式，则直接赋值 */
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
            
            /** \brief 获取X轴角速度数据 */
            ret = __bmg160_read(p_this, __BMG160_RATE_X_LSB, reg_data, 2);
            if (ret != AW_OK) {
                return ret;
            }
            x_axis_data = __BMG160_UINT8_TO_UINT16(reg_data);
            p_buf[i].val  = x_axis_data * temp_val *100;  /*< \brief X轴角速度数据(°/S) */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;         /*< \brief 单位默认为-6:10^(-6)*/
        } else if (cur_id == 1) {
            /** \brief 获取Y轴角速度数据 */
            ret = __bmg160_read(p_this, __BMG160_RATE_Y_LSB, reg_data+2, 2);
            if (ret != AW_OK) {
                return ret;
            }
            y_axis_data = __BMG160_UINT8_TO_UINT16((reg_data+2));
            p_buf[i].val  = y_axis_data * temp_val *100; ;  /*< \brief Y轴角速度数据(°/S) */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;           /*< \brief 单位默认为-6:10^(-6)*/
        } else if (cur_id == 2) {
            /** \brief 获取Z轴角速度数据 */
            ret = __bmg160_read(p_this, __BMG160_RATE_Z_LSB, reg_data+4, 2);
            if (ret != AW_OK) {
                return ret;
            }
            z_axis_data = __BMG160_UINT8_TO_UINT16((reg_data+4));
            p_buf[i].val  = z_axis_data * temp_val *100; ;  /*< \brief Z轴角速度数据(°/S) */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;           /*< \brief 单位默认为-6:10^(-6)*/
        } else {
              /*< \brief 若此次通道不属于该传感器，直接返回 */
        }
    }
    return ret;
}

/** \brief 使能传感器通道 */
aw_local aw_err_t __pfn_enable (void            *p_drv,
                                const int       *p_ids,
                                int              num,
                                aw_sensor_val_t *p_result)
{
     return AW_OK;
}

/** \brief 禁能传感器通道 */
aw_local aw_err_t __pfn_disable (void            *p_drv,
                                 const int       *p_ids,
                                 int              num,
                                 aw_sensor_val_t *p_result)
{
    return AW_OK;
}
 
/** \brief 配置传感器通道属性（attributes） */
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
            reg_data = 0x04;  /* ±125°/S */
        } else if (val.val >= 250 && val.val < 500) {
            p_this->range.val = 250;
            reg_data = 0x03;  /* ±250°/S */
        } else if (val.val >= 500 && val.val < 1000) {
            p_this->range.val = 500;
            reg_data = 0x02;  /* ±500°/S */
        } else if (val.val >= 1000 && val.val < 2000) {
            p_this->range.val = 1000;
            reg_data = 0x01;  /* ±1000°/S */
        } else if (val.val >= 2000) {
            p_this->range.val = 2000;
            reg_data = 0x00;  /* ±2000°/S */
        } else {
            return -AW_ENOTSUP;
        }

        p_this->range.unit = AW_SENSOR_UNIT_BASE;

        reg_data |= 0x80;
        __bmg160_write(p_this, __BMG160_RANGE, &reg_data, 1);

    }

    return AW_OK;
}
 
/** \brief 获取传感器通道属性 */
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


/** \brief 设置触发，一个通道仅能设置一个触发回调函数 */
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

/** \brief 打开触发 */
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

/** \brief 关闭触发 */
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
 * \brief 将 BMG160 驱动注册到 AWBus 子系统中
 */
void awbl_bmg160_drv_register (void)
{
    awbl_drv_register(&__g_bmg160_drvinfo);
}

/* end of file */
