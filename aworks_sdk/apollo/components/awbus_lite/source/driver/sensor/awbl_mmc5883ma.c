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
 * \brief MMC5883MA 三轴磁传感器驱动
 *
 * \internal
 * \par Modification History
 * - 1.00 18-11-30  ipk, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_gpio.h"
#include "aworks.h"
#include "driver/sensor/awbl_mmc5883ma.h"
#include "aw_vdebug.h"
#include "aw_isr_defer.h"
#include "aw_delay.h"

/*******************************************************************************
  本地宏定义
*******************************************************************************/
/** \brief 错误则返回错误值 */
#define __MMC5883MA_EXIT(ret)      \
    if (ret != AW_OK) {            \
        return ret;                \
    }

/** \brief 通过 AWBus 设备，获取MMC5883MA设备 */
#define __MMC5883MA_DEV_DECL(p_this, p_dev)   \
    awbl_mmc5883ma_dev_t *p_this = (awbl_mmc5883ma_dev_t *)p_dev

/** \brief 通过 AWBus 设备，获取 MMC5883MA设备信息 */
#define __MMC5883MA_DEVINFO_DECL(p_devinfo, p_dev)    \
    awbl_mmc5883ma_devinfo_t *p_devinfo =             \
        (awbl_mmc5883ma_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

#define __MMC5883MA_X_OUT_L_REG      (0x00)      /**< \brief X轴数据低字节寄存器  */
#define __MMC5883MA_X_OUT_H_REG      (0x01)      /**< \brief X轴数据高字节寄存器  */
#define __MMC5883MA_Y_OUT_L_REG      (0x02)      /**< \brief Y轴数据低字节寄存器  */
#define __MMC5883MA_Y_OUT_H_REG      (0x03)      /**< \brief Y轴数据高字节寄存器  */
#define __MMC5883MA_Z_OUT_L_REG      (0x04)      /**< \brief Z轴数据低字节寄存器  */
#define __MMC5883MA_Z_OUT_H_REG      (0x05)      /**< \brief Z轴数据高字节寄存器  */
#define __MMC5883MA_TEMP_REG         (0x06)      /**< \brief 温度数据寄存器       */
#define __MMC5883MA_STATUS_REG       (0x07)      /**< \brief 状态寄存器           */
#define __MMC5883MA_CTRL0_REG        (0x08)      /**< \brief 控制寄存器0          */
#define __MMC5883MA_CTRL1_REG        (0x09)      /**< \brief 控制寄存器1          */
#define __MMC5883MA_CTRL2_REG        (0x0A)      /**< \brief 控制寄存器2          */
#define __MMC5883MA_X_THRES_REG      (0x0B)      /**< \brief X轴阈值设置寄存器    */
#define __MMC5883MA_Y_THRES_REG      (0x0C)      /**< \brief Y轴阈值设置寄存器    */
#define __MMC5883MA_Z_THRES_REG      (0x0D)      /**< \brief Z轴阈值设置寄存器    */
#define __MMC5883MA_PROD_ID_REG      (0x2F)      /**< \brief 产品ID寄存器         */
                                                 
#define __MMC5883MA_MAGN_INT_CLEAR   (0x01)      /**< \brief 清状磁场测量中断标志 */
#define __MMC5883MA_TEMP_INT_CLEAR   (0x02)      /**< \brief 清状温度测量中断标志 */
#define __MMC5883MA_MOTION_INT_CLEAR (0x04)      /**< \brief 清状动作检测中断标志 */
#define __MMC5883MA_MEAS_M_DONE      (0x00)      /**< \brief 磁场测量完成标志位   */
#define __MMC5883MA_MEAS_T_DONE      (0x01)      /**< \brief 温度测量完成标志位   */
#define __MMC5883MA_MOTION_DETECTED  (0x02)      /**< \brief 动作检测完成标志位   */

#define __MMC5883MA_MAGN_READ        (0x01 << 0) /**< \brief 初始化磁场数据测量   */
#define __MMC5883MA_TEMP_READ        (0x01 << 1) /**< \brief 初始化温度数据测量   */
#define __MMC5883MA_START_MDT        (0x01 << 2) /**< \brief 开启一个动作检测     */
#define __MMC5883MA_SET              (0x01 << 3) /**< \brief 设置SET模式          */
#define __MMC5883MA_RESET            (0x01 << 4) /**< \brief 设置RESET模式        */
#define __MMC5883MA_OPT_READ         (0x01 << 6) /**< \brief 读取OPT数据          */

#define __MMC5883MA_INT_MDT_EN       (5)         /**< \brief 动作检测中断使能位   */
#define __MMC5883MA_INT_MEAS_DONE_EN (6)         /**< \brief 数据就绪中断使能位   */

/** \brief 测量一次所需的时间 */
enum mmc5883ma_measu_time {
    MMC5883MA_MEASU_10MS       = 0,              /**< \brief 测量时间10ms .       */
    MMC5883MA_MEASU_5MS,                         /**< \brief 测量时间5ms .        */
    MMC5883MA_MEASU_2_5MS,                       /**< \brief 测量时间2.5ms .      */
    MMC5883MA_MEASU_1_6MS                        /**< \brief 测量时间1.6ms .      */
};

/** \brief 连续测量模式下的测量频率 */
typedef enum mmc5883ma_cm_rate {
    MMC5883MA_SEQUENTIAL_OFF   = 0,              /**< \brief 关闭连续测量 .         */
    MMC5883MA_RATE_14HZ,                         /**< \brief 测量频率14HZ         */
    MMC5883MA_RATE_5HZ,                          /**< \brief 测量频率5HZ          */
    MMC5883MA_RATE_2_2HZ,                        /**< \brief 测量频率2.2HZ        */
    MMC5883MA_RATE_1HZ,                          /**< \brief 测量频率1HZ          */
    MMC5883MA_RATE_0_1_2HZ,                      /**< \brief 测量频率1/2HZ        */
    MMC5883MA_RATE_0_1_4HZ,                      /**< \brief 测量频率1/4HZ        */
    MMC5883MA_RATE_0_1_8HZ,                      /**< \brief 测量频率1/8HZ        */
    MMC5883MA_RATE_0_1_16HZ,                     /**< \brief 测量频率1/16HZ       */
    MMC5883MA_RATE_0_1_32HZ,                     /**< \brief 测量频率1/32HZ       */
    MMC5883MA_RATE_0_1_64HZ,                     /**< \brief 测量频率1/64HZ       */
} mmc5883ma_cm_rate_t;

/*******************************************************************************
  本地函数声明
*******************************************************************************/
/**
 * \brief MMC5883MA 写数据
 */
aw_local aw_err_t __mmc5883ma_write (awbl_mmc5883ma_dev_t *p_this,
                                     uint32_t              subaddr,
                                     uint8_t              *p_buf,
                                     uint32_t              nbytes);

/**
 * \brief MMC5883MA 读数据
 */
aw_local aw_err_t __mmc5883ma_read (awbl_mmc5883ma_dev_t *p_this,
                                    uint32_t              subaddr,
                                    uint8_t              *p_buf,
                                    uint32_t              nbytes);

/**
 * \brief 传感器服务获取方法
 */
aw_local aw_err_t __mmc5883ma_sensorserv_get (struct awbl_dev *p_dev,
                                              void            *p_arg);

/**
 * \brief 第二阶段初始化函数
 */
aw_local void __mmc5883ma_inst_init2 (struct awbl_dev *p_dev);

/**
 * \brief 第三阶段初始化函数
 */
aw_local void __mmc5883ma_inst_connect (struct awbl_dev *p_dev);

/**
 * \brief 传感器通道使能
 */
aw_local aw_err_t __mmc5883ma_enable (void            *p_cookie,
                                      const int       *p_ids,
                                      int              num,
                                      aw_sensor_val_t *p_buf);

/**
 * \brief 禁能传感器通道
 */
aw_local aw_err_t __mmc5883ma_disable (void            *p_cookie,
                                       const int       *p_ids,
                                       int              num,
                                       aw_sensor_val_t *p_buf);

/**
 * \brief 从指定通道获取采样数据
 */
aw_local aw_err_t __mmc5883ma_data_get (void            *p_cookie,
                                        const int       *p_ids,
                                        int              num,
                                        aw_sensor_val_t *p_buf);

/**
 * \brief 传感器属性设置
 */
aw_local aw_err_t __mma5883ma_attr_set (void                  *p_cookie,
                                        int                    id,
                                        int                    attr,
                                        const aw_sensor_val_t *p_val);

/**
 * \brief 传感器属性获取
 */
aw_local aw_err_t __mmc5883ma_attr_get (void              *p_cookie,
                                        int                id,
                                        int                attr,
                                        aw_sensor_val_t   *p_val);

/**
 * \brief 传感器通道触发配置
 */
aw_local aw_err_t __mmc5993ma_trigger_cfg (void                  *p_cookie,
                                           int                    id,
                                           uint32_t               flags,
                                           aw_sensor_trigger_cb_t pfn_cb,
                                           void                  *p_arg);

/**
 * \brief 开启传感器通道触发
 */
aw_local aw_err_t __mmc5883ma_trigger_on (void *p_cookie, int id);

/**
 * \brief 关闭传感器通道触发
 */
aw_local aw_err_t __mmc5883ma_trigger_off (void *p_cookie, int id);

/**
 * \brief 传感器触发回调函数
 */
aw_local void __mmc5883ma_callback (void *p_arg);

/**
 * \brief 读温度回调函数
 */
aw_local void __mmc5883ma_temp_callback (void *p_arg);

/**
 * \brief 传感器中断延时作业
 */
aw_local void __mmc5883ma_isr_defer (void *p_arg);

/**
 * \brief 读温度中断延迟作业
 */
aw_local void __mmc5883ma_temp_isr_defer (void *p_arg);


/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/** \brief 驱动提供的方法 */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

aw_local aw_const struct awbl_dev_method __g_mmc5883ma_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __mmc5883ma_sensorserv_get),
        AWBL_METHOD_END
};

/** \brief 驱动入口点 */
aw_local aw_const struct awbl_drvfuncs __g_mmc5883ma_drvfuncs = {
        NULL,                       /*< \brief 第一阶段初始化    */
        __mmc5883ma_inst_init2,     /*< \brief 第二阶段初始化    */
        __mmc5883ma_inst_connect    /*< \brief 第三阶段初始化    */
};

/** \brief 驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_mmc5883ma = {
    AWBL_VER_1,                     /*< \brief AWBus 版本号.     */
    AWBL_BUSID_I2C,                 /*< \brief 总线 ID.          */
    AWBL_MMC5883MA_NAME,            /*< \brief 驱动名.           */
    &__g_mmc5883ma_drvfuncs,        /*< \brief 驱动入口点.       */
    &__g_mmc5883ma_dev_methods[0],  /*< \brief 驱动提供的方法.   */
    NULL                            /*< \brief 驱动探测函数.     */
};

/** \brief 服务函数 */
aw_local aw_const struct awbl_sensor_servfuncs __g_mmc5883ma_servfuncs = {
        __mmc5883ma_data_get,
        __mmc5883ma_enable,
        __mmc5883ma_disable,
        __mma5883ma_attr_set,
        __mmc5883ma_attr_get,
        __mmc5993ma_trigger_cfg,
        __mmc5883ma_trigger_on,
        __mmc5883ma_trigger_off
};

/*******************************************************************************
    本地函数定义
*******************************************************************************/
/**
 * \brief 传感器服务获取方法
 */
aw_local aw_err_t __mmc5883ma_sensorserv_get (struct awbl_dev *p_dev,
                                              void            *p_arg)
{
    __MMC5883MA_DEV_DECL(p_this, p_dev);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_dev);

    awbl_sensor_service_t *p_serv = &p_this->sensor_serv;

    /** \brief 该传感器的通道分配 */
    aw_local aw_const awbl_sensor_type_info_t type_info[2] = {
        {AW_SENSOR_TYPE_MAGNETIC, 3},
        {AW_SENSOR_TYPE_TEMPERATURE, 1},
    };

    /* 传感器服务常量信息*/
    aw_local aw_const awbl_sensor_servinfo_t serv_info = {
        4,                      /* 支持的通道数量 */
        type_info,              /* 传感器类型列表 */
        AW_NELEMENTS(type_info) /* 类型列表数目   */
    };

    p_serv->p_cookie    = p_this;
    p_serv->p_next      = NULL;
    p_serv->p_servfuncs = &__g_mmc5883ma_servfuncs;
    p_serv->p_servinfo  = &serv_info;
    p_serv->start_id    = p_devinfo->start_id;

    *(awbl_sensor_service_t **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
/** \brief 第二次初始化传感器服务获取方法 */
aw_local void __mmc5883ma_inst_init2 (struct awbl_dev *p_dev)
{
    __MMC5883MA_DEV_DECL(p_this, p_dev);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_dev);

    /** \brief 创建IIC设备 */
    aw_i2c_mkdev(&p_this->i2c_dev,
                 p_dev->p_devhcf->bus_index,
                 p_devinfo->i2c_addr,
                 (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));

    p_this->ctrl1_reg_buf         = 0;
    p_this->ctrl2_reg_buf         = 0;
    p_this->channel               = 0;
    p_this->ch_trigger            = 0;
    p_this->flags[0]              = 0;
    p_this->flags[1]              = 0;
    p_this->flags[2]              = 0;
    p_this->flags[3]              = 0;
    p_this->pfn_trigger_cb[0]     = NULL;
    p_this->pfn_trigger_cb[1]     = NULL;
    p_this->pfn_trigger_cb[2]     = NULL;
    p_this->pfn_trigger_cb[3]     = NULL;

    /* 设置默认采样率为2.2HZ */
    p_this->sampling_rate[0].val  = 2200;
    p_this->sampling_rate[0].unit = AW_SENSOR_UNIT_MILLI;
    p_this->sampling_rate[1].val  = 2200;
    p_this->sampling_rate[1].unit = AW_SENSOR_UNIT_MILLI;

    aw_isr_defer_job_init(&p_this->defer_job[0],
                          __mmc5883ma_isr_defer,
                          p_this);
    aw_isr_defer_job_init(&p_this->defer_job[1],
                          __mmc5883ma_temp_isr_defer,
                          p_this);

    aw_timer_init(&p_this->timer, __mmc5883ma_temp_callback, p_this);

    if (p_devinfo->tergger_pin > 0) {

        if (p_devinfo->pfn_plfm_init) {
            p_devinfo->pfn_plfm_init();
        }

        aw_gpio_trigger_cfg(p_devinfo->tergger_pin, AW_GPIO_TRIGGER_HIGH);
        aw_gpio_trigger_connect(p_devinfo->tergger_pin,
                                __mmc5883ma_callback,
                                p_this);
    }

}

/** \brief 第三阶段初始化函数 */
aw_local void __mmc5883ma_inst_connect (struct awbl_dev *p_dev)
{
    __MMC5883MA_DEV_DECL(p_this, p_dev);

    uint8_t   dat = 0;
    aw_err_t  ret;

    /* 复位传感器 */
    AW_BIT_SET(dat, 7);
    __mmc5883ma_write(p_this,
                      __MMC5883MA_CTRL1_REG,
                      &dat,
                      1);
    aw_mdelay(5);

    /* 设置测量时间 */
    p_this->ctrl1_reg_buf |= MMC5883MA_MEASU_5MS;
    __mmc5883ma_write(p_this,
                      __MMC5883MA_CTRL1_REG,
                      &p_this->ctrl1_reg_buf,
                      1);

    dat = __MMC5883MA_SET;
    __mmc5883ma_write(p_this,
                      __MMC5883MA_CTRL0_REG,
                      &dat,
                      1);
    aw_mdelay(5);

    /* 清除中断标志 */
    dat = __MMC5883MA_MOTION_INT_CLEAR |
          __MMC5883MA_TEMP_INT_CLEAR   |
          __MMC5883MA_MAGN_INT_CLEAR;
    ret = __mmc5883ma_write(p_this,
                            __MMC5883MA_STATUS_REG,
                            &dat,
                            1);

    if (ret != AW_OK) {
        aw_kprintf("\r\nSensor MMC5883MA Init is ERROR! \r\n");
    }
}

/******************************************************************************/

/*
 * \brief MMC5883MA 写数据
 */
aw_static_inline aw_err_t __mmc5883ma_write (awbl_mmc5883ma_dev_t *p_this,
                                             uint32_t              subaddr,
                                             uint8_t              *p_buf,
                                             uint32_t              nbytes)
{
    return aw_i2c_write(&p_this->i2c_dev,
                        subaddr,
                        p_buf,
                        nbytes);
}

/*
 * \brief MMC5883MA 读数据
 */
aw_static_inline aw_err_t __mmc5883ma_read (awbl_mmc5883ma_dev_t *p_this,
                                            uint32_t              subaddr,
                                            uint8_t              *p_buf,
                                            uint32_t              nbytes)
{
    return aw_i2c_read(&p_this->i2c_dev,
                       subaddr,
                       p_buf,
                       nbytes);
}

/*
 * \brief 清除相应中断
 */
aw_static_inline void __mmc5883ma_irq_clear (awbl_mmc5883ma_dev_t *p_this,
                                             uint8_t               status)
{
    uint8_t dat = 0;

    if (!AW_BIT_GET(status, 4)) {
        dat = __MMC5883MA_MOTION_INT_CLEAR |
              __MMC5883MA_TEMP_INT_CLEAR   |
              __MMC5883MA_MAGN_INT_CLEAR;
    }
    if (AW_BIT_GET(status, __MMC5883MA_MOTION_DETECTED)) {
        dat |= __MMC5883MA_MOTION_INT_CLEAR;
    }
    if (AW_BIT_GET(status, __MMC5883MA_MEAS_T_DONE)) {
        dat |= __MMC5883MA_TEMP_INT_CLEAR;
    }
    if (AW_BIT_GET(status, __MMC5883MA_MEAS_M_DONE)) {
        dat |= __MMC5883MA_MAGN_INT_CLEAR;
    }

    /* 清除中断标志 */
    __mmc5883ma_write(p_this,
                      __MMC5883MA_STATUS_REG,
                      &dat,
                      1);
}

/**
 * \brief 读温度回调函数
 */
aw_local void __mmc5883ma_temp_callback (void *p_arg)
{
    __MMC5883MA_DEV_DECL(p_this, p_arg);

    aw_isr_defer_job_add(&p_this->defer_job[1]);
}

/**
 * \brief 读温度中断延迟作业
 */
aw_local void __mmc5883ma_temp_isr_defer (void *p_arg)
{
    uint8_t   dat;
    uint32_t  timer_ms;

    __MMC5883MA_DEV_DECL(p_this, p_arg);

    aw_sensor_val_unit_convert(&p_this->sampling_rate[1],
                               1,
                               AW_SENSOR_UNIT_MILLI);
    timer_ms = 1000000 / p_this->sampling_rate[1].val;

    dat = __MMC5883MA_TEMP_READ | __MMC5883MA_SET;
    __mmc5883ma_write(p_this,
                      __MMC5883MA_CTRL0_REG,
                      &dat,
                      1);

    aw_timer_start(&p_this->timer, aw_ms_to_ticks(timer_ms));
}

/**
 * \brief 传感器中断回调函数
 */
aw_local void __mmc5883ma_callback (void *p_arg)
{
    __MMC5883MA_DEV_DECL(p_this, p_arg);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_arg);

    aw_gpio_trigger_off(p_devinfo->tergger_pin);

    aw_isr_defer_job_add(&p_this->defer_job[0]);
}

/**
 * \brief 中断延迟作业函数
 */
aw_local void __mmc5883ma_isr_defer (void *p_arg)
{
    uint8_t         dat;
    uint8_t         status;

    __MMC5883MA_DEV_DECL(p_this, p_arg);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_arg);

    /* 读一次状态寄存器 */
    __mmc5883ma_read(p_this,
                     __MMC5883MA_STATUS_REG,
                     &status,
                     1);

    /* 清除相应中断 */
    __mmc5883ma_irq_clear(p_this, status);

    /* 如果是动作检测中断 */
    if (AW_BIT_GET(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MDT_EN)) {

        if (AW_BIT_GET(status, __MMC5883MA_MOTION_DETECTED)) {

            if ((p_this->pfn_trigger_cb[0]          != NULL) &&
                (AW_BIT_GET(p_this->ch_trigger, 0)  == 1   ) &&
                (p_this->flags[0] & AW_SENSOR_TRIGGER_SLOPE) ) {

                p_this->pfn_trigger_cb[0](p_this->p_argc[0],
                                          AW_SENSOR_TRIGGER_SLOPE);
            }
            if ((p_this->pfn_trigger_cb[1]          != NULL) &&
                (AW_BIT_GET(p_this->ch_trigger, 1)  == 1   ) &&
                (p_this->flags[1] & AW_SENSOR_TRIGGER_SLOPE) ) {

                p_this->pfn_trigger_cb[1](p_this->p_argc[1],
                                          AW_SENSOR_TRIGGER_SLOPE);
            }
            if ((p_this->pfn_trigger_cb[2]          != NULL) &&
                (AW_BIT_GET(p_this->ch_trigger, 2)  == 1   ) &&
                (p_this->flags[2] & AW_SENSOR_TRIGGER_SLOPE) ) {

                p_this->pfn_trigger_cb[2](p_this->p_argc[2],
                                          AW_SENSOR_TRIGGER_SLOPE);
            }

            /* 开启下一个动作检测 */
            dat =  __MMC5883MA_START_MDT | __MMC5883MA_SET;
            __mmc5883ma_write(p_this,
                              __MMC5883MA_CTRL0_REG,
                              &dat,
                              1);
        }
    }

    /* 如果是数据就绪中断 */
    if (AW_BIT_GET(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MEAS_DONE_EN)) {

        /* 磁场数据就绪 */
        if (AW_BIT_GET(status, __MMC5883MA_MEAS_M_DONE)) {

            if ((p_this->pfn_trigger_cb[0]               != NULL) &&
                (AW_BIT_GET(p_this->ch_trigger, 0)       == 1   ) &&
                (p_this->flags[0] & AW_SENSOR_TRIGGER_DATA_READY) ) {

                p_this->pfn_trigger_cb[0](p_this->p_argc[0],
                                          AW_SENSOR_TRIGGER_DATA_READY);

            }
            if ((p_this->pfn_trigger_cb[1]               != NULL) &&
                (AW_BIT_GET(p_this->ch_trigger, 1)       == 1   ) &&
                (p_this->flags[1] & AW_SENSOR_TRIGGER_DATA_READY) ) {

                p_this->pfn_trigger_cb[1](p_this->p_argc[1],
                                          AW_SENSOR_TRIGGER_DATA_READY);

            }
            if ((p_this->pfn_trigger_cb[2]               != NULL) &&
                (AW_BIT_GET(p_this->ch_trigger, 2)       == 1   ) &&
                (p_this->flags[2] & AW_SENSOR_TRIGGER_DATA_READY) ) {

                p_this->pfn_trigger_cb[2](p_this->p_argc[2],
                                          AW_SENSOR_TRIGGER_DATA_READY);

            }
        }

        /* 温度数据就绪 */
        if (AW_BIT_GET(status, __MMC5883MA_MEAS_T_DONE)) {

            if ((p_this->pfn_trigger_cb[3]               != NULL) &&
                (AW_BIT_GET(p_this->ch_trigger, 3)       == 1   ) &&
                (p_this->flags[3] & AW_SENSOR_TRIGGER_DATA_READY) ) {

                p_this->pfn_trigger_cb[3](p_this->p_argc[3],
                                          AW_SENSOR_TRIGGER_DATA_READY);

            }
        }
    }

    aw_gpio_trigger_on(p_devinfo->tergger_pin);

}

/**
 * \brief 传感器数据处理
 */
aw_local aw_err_t __mmc5883ma_data_deal (awbl_mmc5883ma_dev_t *p_this,
                                         uint8_t               reg_addr,
                                         aw_sensor_val_t      *p_buf,
                                         uint32_t              len)
{
    uint8_t  reg_data[2];
    uint16_t value = 0;
    aw_err_t ret;

    ret = __mmc5883ma_read(p_this, reg_addr, &reg_data[0], len);
    __MMC5883MA_EXIT(ret);

    if (reg_addr == __MMC5883MA_TEMP_REG) {

        /* 精度0.7 ℃  温度扩大了10倍 */
        p_buf->val  = ((int32_t)reg_data[0] * 7) - 750;
        p_buf->unit = AW_SENSOR_UNIT_DECI;

    } else {

        value = (uint16_t)reg_data[1] << 8 | reg_data[0];

        /* 磁场强度扩大了1000000倍 */
        p_buf->val  = (int64_t)((int16_t)(value ^ 0x8000)) * 1000000 / 4096;
        p_buf->unit = AW_SENSOR_UNIT_MICRO;
    }

    return AW_OK;

}

/**
 * \brief 门限值设定
 */
aw_local aw_err_t __mmc5883ma_threshould_set (awbl_mmc5883ma_dev_t *p_this,
                                              int                   id,
                                              aw_sensor_val_t      *p_val)
{
    uint8_t  thre_reg;
    aw_err_t ret;
    int      cur_id;

    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_this);

    cur_id = id - p_devinfo->start_id;

    aw_sensor_val_unit_convert(p_val, 1, AW_SENSOR_UNIT_MILLI);

    if (p_val->val > 250) {
        return -AW_ENOTSUP;
    }

    if (cur_id == 0) {
        thre_reg = __MMC5883MA_X_THRES_REG;
    } else if (cur_id == 1) {
        thre_reg = __MMC5883MA_Y_THRES_REG;
    } else if (cur_id == 2) {
        thre_reg = __MMC5883MA_Z_THRES_REG;
    }

    ret = __mmc5883ma_write(p_this,
                            thre_reg,
                            (uint8_t *)&p_val->val,
                            1);

    __MMC5883MA_EXIT(ret);

    p_this->threshold[cur_id].val  = p_val->val;
    p_this->threshold[cur_id].unit = p_val->unit;

    return AW_OK;

}

/**
 * \brief 采样速率转化为对应枚举值
 */
aw_static_inline
aw_err_t __mmc5883ma_rateval_to_rate (aw_sensor_val_t      val,
                                      mmc5883ma_cm_rate_t *p_rate)
{
    aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_MILLI);

    if (val.val <= 10) {
        *p_rate = MMC5883MA_SEQUENTIAL_OFF;
    } else if (val.val > 10 && val.val < 30) {
        *p_rate = MMC5883MA_RATE_0_1_64HZ;
    } else if (val.val >= 30 && val.val < 60) {
        *p_rate = MMC5883MA_RATE_0_1_32HZ;
    } else if (val.val >= 60 && val.val < 120) {
        *p_rate = MMC5883MA_RATE_0_1_16HZ;
    } else if (val.val >= 120 && val.val < 240) {
        *p_rate = MMC5883MA_RATE_0_1_8HZ;
    } else if (val.val >= 200 && val.val < 400) {
        *p_rate = MMC5883MA_RATE_0_1_4HZ;
    } else if (val.val >= 400 && val.val < 800) {
        *p_rate = MMC5883MA_RATE_0_1_2HZ;
    } else if (val.val >= 800 && val.val < 1600) {
        *p_rate = MMC5883MA_RATE_1HZ;
    } else if (val.val >= 1600 && val.val < 3200) {
        *p_rate = MMC5883MA_RATE_2_2HZ;
    } else if (val.val >= 3200 && val.val < 6400) {
        *p_rate = MMC5883MA_RATE_5HZ;
    } else if (val.val >= 6400 && val.val < 15000) {
        *p_rate = MMC5883MA_RATE_14HZ;
    } else {
        return -AW_ENOTSUP;
    }
    return AW_OK;
}

/**
 * \brief 采样速率设置
 */
aw_local aw_err_t __mmc5883ma_rate_set (awbl_mmc5883ma_dev_t *p_this,
                                        aw_sensor_val_t      *p_val)
{
    mmc5883ma_cm_rate_t buf_rate = 0;
    aw_err_t            ret;

    ret = __mmc5883ma_rateval_to_rate(*p_val, &buf_rate);
    __MMC5883MA_EXIT(ret);

    p_this->ctrl2_reg_buf |= buf_rate;

    p_this->sampling_rate[0].val  = p_val->val;
    p_this->sampling_rate[0].unit = p_val->unit;

    return AW_OK;
}

/**
 * \brief 开始测量一个类型的数据
 */
aw_local aw_err_t __mmc5883ma_measure_start (awbl_mmc5883ma_dev_t *p_this,
                                             uint8_t               type)
{
    uint8_t  dat;
    aw_err_t ret;
    uint8_t  wait_type;

    dat = type | __MMC5883MA_SET;

    ret = __mmc5883ma_write(p_this,
                            __MMC5883MA_CTRL0_REG,
                            &dat,
                            1);

    __MMC5883MA_EXIT(ret);

    wait_type = (type == __MMC5883MA_MAGN_READ)?__MMC5883MA_MEAS_M_DONE:
                 __MMC5883MA_MEAS_T_DONE;

    dat = 0;

    /* 等待测量完成 */
    while (AW_BIT_GET(dat, wait_type) == 0) {
        ret = __mmc5883ma_read(p_this,
                                __MMC5883MA_STATUS_REG,
                               &dat,
                               1);
        __MMC5883MA_EXIT(ret);

    }

    return AW_OK;
}

/******************************************************************************/

/**
 * \brief 从指定通道获取采样数据
 */
aw_local aw_err_t __mmc5883ma_data_get (void            *p_cookie,
                                        const int       *p_ids,
                                        int              num,
                                        aw_sensor_val_t *p_buf)
{
    uint8_t   reg_addr;
    int       i;
    aw_err_t  ret;
    uint8_t   ready_flag = 0;

    __MMC5883MA_DEV_DECL(p_this, p_cookie);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_cookie);

    int cur_id = p_ids[0] - p_devinfo->start_id;

    /* 若打开数据就绪中断,则不需要发送测量命令*/
    if (AW_BIT_GET(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MEAS_DONE_EN)) {
        ready_flag = 1;
    }

    if (!ready_flag) {
        ret = __mmc5883ma_measure_start(p_this, __MMC5883MA_MAGN_READ);
        __MMC5883MA_EXIT(ret);
    }

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id > 3) {
            break;
        }

        switch (cur_id) {

            case 0: reg_addr = __MMC5883MA_X_OUT_L_REG;
                break;

            case 1: reg_addr = __MMC5883MA_Y_OUT_L_REG;
                break;

            case 2: reg_addr = __MMC5883MA_Z_OUT_L_REG;
                break;

            case 3: reg_addr = __MMC5883MA_TEMP_REG;
                break;

            default:
                break;
        }

        /* 若该通道未使能，则跳过 */
        if (AW_BIT_GET(p_this->channel, cur_id) != 1) {
            continue;
        }

        if ((cur_id == 3) && (!ready_flag)) {
            ret = __mmc5883ma_measure_start(p_this, __MMC5883MA_TEMP_REG);
            __MMC5883MA_EXIT(ret);
        }

        ret = __mmc5883ma_data_deal(p_this,
                                    reg_addr,
                                    &p_buf[i],
                                    2);
        __MMC5883MA_EXIT(ret);

    }

    return AW_OK;
}

/**
 * \brief 使能传感器通道
 */
aw_local aw_err_t __mmc5883ma_enable (void            *p_cookie,
                                      const int       *p_ids,
                                      int              num,
                                      aw_sensor_val_t *p_result)
{
    int      cur_id, i;
    aw_err_t ret;

    __MMC5883MA_DEV_DECL(p_this, p_cookie);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_cookie);

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id > 3) {
            break;
        }

        AW_BIT_SET(p_this->channel, cur_id);

        if (cur_id < 3) {
            AW_BIT_CLR(p_this->ctrl1_reg_buf, cur_id + 2);
        }

        ret = __mmc5883ma_write(p_this,
                                __MMC5883MA_CTRL1_REG,
                                &p_this->ctrl1_reg_buf,
                                1);
        if (p_result != NULL) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], ret);
        }

    }
    return AW_OK;
}

/**
 * \brief 禁能传感器通道
 */
aw_local aw_err_t __mmc5883ma_disable (void            *p_cookie,
                                       const int       *p_ids,
                                       int              num,
                                       aw_sensor_val_t *p_result)
{
    int cur_id, i;
    aw_err_t ret;

    __MMC5883MA_DEV_DECL(p_this, p_cookie);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_cookie);

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id > 3) {
            break;
        }

        AW_BIT_CLR(p_this->channel, cur_id);

        if (cur_id < 3) {
            AW_BIT_SET(p_this->ctrl1_reg_buf, cur_id + 2);
        }

        ret = __mmc5883ma_write(p_this,
                                __MMC5883MA_CTRL1_REG,
                                &p_this->ctrl1_reg_buf,
                                1);

        if (p_result != NULL) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], ret);
        }

    }

    return AW_OK;

}

/**
 * \brief 传感器属性设置
 */
aw_local aw_err_t __mma5883ma_attr_set (void                    *p_cookie,
                                        int                      id,
                                        int                      attr,
                                        const aw_sensor_val_t   *p_val)
{
    __MMC5883MA_DEV_DECL(p_this, p_cookie);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_cookie);

    aw_sensor_val_t val    = {0, 0};

    if (id < p_devinfo->start_id || id > p_devinfo->start_id + 3) {
        return -AW_ENODEV;
    }

    val.val  = p_val->val;
    val.unit = p_val->unit;

    if (attr == AW_SENSOR_ATTR_SAMPLING_RATE) {

        if (id == p_devinfo->start_id + 3) {

            p_this->sampling_rate[1].val  = p_val->val;
            p_this->sampling_rate[1].unit = p_val->unit;

            return AW_OK;

        } else {
            return __mmc5883ma_rate_set(p_this, &val);
        }

    } else if (attr == AW_SENSOR_ATTR_THRESHOLD_SLOPE) {

        if (id == p_devinfo->start_id + 3) {
            return -AW_ENOTSUP;
        }

        return __mmc5883ma_threshould_set(p_this, id, &val);

    }

    return -AW_EINVAL;
}

/**
 * \brief 传感器属性获取
 */
aw_local aw_err_t __mmc5883ma_attr_get (void              *p_cookie,
                                        int                id,
                                        int                attr,
                                        aw_sensor_val_t   *p_val)
{
    __MMC5883MA_DEV_DECL(p_this, p_cookie);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_cookie);

    if (id < p_devinfo->start_id || id > p_devinfo->start_id + 3) {
        return -AW_ENODEV;
    }

    if (attr == AW_SENSOR_ATTR_SAMPLING_RATE) {

        if (id != p_devinfo->start_id + 3) {

            p_val->val  = p_this->sampling_rate[0].val;
            p_val->unit = p_this->sampling_rate[0].unit;

        } else {

            p_val->val  = p_this->sampling_rate[1].val;
            p_val->unit = p_this->sampling_rate[1].unit;

        }

    } else if (attr == AW_SENSOR_ATTR_THRESHOLD_SLOPE) {

        if (id == p_devinfo->start_id + 3) {
            return -AW_ENOTSUP;
        }

        p_val->val  = p_this->threshold[id - p_devinfo->start_id].val;
        p_val->unit = p_this->threshold[id - p_devinfo->start_id].unit;

    }
    return AW_OK;
}

/**
 * \brief 触发设置
 */
aw_local aw_err_t __mmc5993ma_trigger_cfg (void                     *p_cookie,
                                           int                       id,
                                           uint32_t                  flags,
                                           aw_sensor_trigger_cb_t    pfn_cb,
                                           void                     *p_arg)
{
    __MMC5883MA_DEV_DECL(p_this, p_cookie);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_cookie);

    if (id < p_devinfo->start_id || id > p_devinfo->start_id + 3) {
        return -AW_ENODEV;
    }

    int cur_id = id - p_devinfo->start_id;

    switch (flags) {
        case AW_SENSOR_TRIGGER_DATA_READY:
            break;
        case AW_SENSOR_TRIGGER_SLOPE:
            break;
        case (AW_SENSOR_TRIGGER_DATA_READY | AW_SENSOR_TRIGGER_SLOPE):
            break;
        default:
            return -AW_ENOTSUP;
    }

    if (cur_id == 3 && flags != AW_SENSOR_TRIGGER_DATA_READY) {
        return -AW_ENOTSUP;
    }

    p_this->pfn_trigger_cb[cur_id] = pfn_cb;
    p_this->p_argc[cur_id]         = p_arg;
    p_this->flags[cur_id]          = flags;

    return AW_OK;

}

/**
 * \brief 开启触发
 */
aw_local aw_err_t __mmc5883ma_trigger_on (void *p_cookie, int id)
{
    __MMC5883MA_DEV_DECL(p_this, p_cookie);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_cookie);

    aw_err_t            ret;
    mmc5883ma_cm_rate_t rate;
    uint8_t             dat = 0;
    uint8_t             ctrl2;
    uint16_t            timer_ms;

    if (id < p_devinfo->start_id || id > p_devinfo->start_id + 3) {
        return -AW_ENODEV;
    }

    int cur_id = id - p_devinfo->start_id;

    switch (p_this->flags[cur_id]) {
        case AW_SENSOR_TRIGGER_DATA_READY:

            AW_BIT_SET(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MEAS_DONE_EN);

            break;
        case AW_SENSOR_TRIGGER_SLOPE:

            AW_BIT_SET(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MDT_EN);

            break;
        case (AW_SENSOR_TRIGGER_DATA_READY | AW_SENSOR_TRIGGER_SLOPE):

            AW_BIT_SET(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MDT_EN);
            AW_BIT_SET(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MEAS_DONE_EN);

            break;
        default:
            return -AW_ENOTSUP;
    }

    /* 如果开启触发的是磁场测量通道 */
    if (cur_id < 3) {

        /* 如果没有开启对应通道 */
        if (AW_BIT_GET(p_this->channel, cur_id) != 1) {
            return -AW_EPERM;
        }

        /* 温度通道无法和磁场通道同时开启触发，如果开启了温度通道触发，则不支持 */
        if (AW_BIT_GET(p_this->ch_trigger, 3) != 0) {
            return -AW_EPERM;
        }

        AW_BIT_SET(p_this->ch_trigger, cur_id);

        ret = __mmc5883ma_rateval_to_rate(p_this->sampling_rate[0], &rate);
        __MMC5883MA_EXIT(ret);

        /* 设置连续测量模式，默认频率2.2HZ */
        p_this->ctrl2_reg_buf |= rate;

        if (p_this->flags[cur_id] != AW_SENSOR_TRIGGER_DATA_READY) {

            /* 开启一个动作检测 */
            dat = __MMC5883MA_START_MDT | __MMC5883MA_SET;
            ret = __mmc5883ma_write(p_this,
                                    __MMC5883MA_CTRL0_REG,
                                    &dat,
                                    1);
            __MMC5883MA_EXIT(ret);

        }

        ctrl2 = p_this->ctrl2_reg_buf;

    } else {

        /* 如果没有开启温度通道 */
        if (AW_BIT_GET(p_this->channel, cur_id) != 1) {
            return -AW_EPERM;
        }

        /* 温度通道无法和磁场通道同时开启触发，如果已经开启磁场通道触发，则不支持 */
        if (AW_BIT_GET(p_this->ch_trigger, 0) == 1 ||
            AW_BIT_GET(p_this->ch_trigger, 1) == 1 ||
            AW_BIT_GET(p_this->ch_trigger, 2) == 1) {
            return -AW_EPERM;
        }

        AW_BIT_SET(p_this->ch_trigger, cur_id);

        dat = __MMC5883MA_TEMP_READ | __MMC5883MA_SET;
        __mmc5883ma_write(p_this,
                          __MMC5883MA_CTRL0_REG,
                          &dat,
                          1);

        aw_sensor_val_unit_convert(&p_this->sampling_rate[1],
                                   1,
                                   AW_SENSOR_UNIT_MILLI);
        timer_ms = 1000000 / p_this->sampling_rate[1].val;

        ctrl2 = p_this->ctrl2_reg_buf & 0xf0;
        aw_timer_start(&p_this->timer, aw_ms_to_ticks(timer_ms));

    }

    /* 使能相应中断 */
    ret = __mmc5883ma_write(p_this,
                            __MMC5883MA_CTRL2_REG,
                            &ctrl2,
                            1);
    __MMC5883MA_EXIT(ret);

    return aw_gpio_trigger_on(p_devinfo->tergger_pin);

}

/**
 * \brief 关闭触发
 */
aw_local aw_err_t __mmc5883ma_trigger_off (void *p_cookie, int id)
{
    __MMC5883MA_DEV_DECL(p_this, p_cookie);
    __MMC5883MA_DEVINFO_DECL(p_devinfo, p_cookie);

    aw_err_t ret;

    if (id < p_devinfo->start_id || id > p_devinfo->start_id + 3) {
        return -AW_ENODEV;
    }

    int cur_id = id - p_devinfo->start_id;

    switch (p_this->flags[cur_id]) {
        case AW_SENSOR_TRIGGER_DATA_READY:

            AW_BIT_CLR(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MEAS_DONE_EN);

            break;
        case AW_SENSOR_TRIGGER_THRESHOLD:

            AW_BIT_CLR(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MDT_EN);

            break;
        case (AW_SENSOR_TRIGGER_DATA_READY | AW_SENSOR_TRIGGER_THRESHOLD):

            AW_BIT_CLR(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MDT_EN);
            AW_BIT_CLR(p_this->ctrl2_reg_buf, __MMC5883MA_INT_MEAS_DONE_EN);

            break;
        default:
            return -AW_ENOTSUP;
    }

    AW_BIT_CLR(p_this->ch_trigger, cur_id);

    /* 三个磁场通道都关闭了 */
    if ((AW_BIT_GET(p_this->ch_trigger, 0) == 0) &&
        (AW_BIT_GET(p_this->ch_trigger, 1) == 0) &&
        (AW_BIT_GET(p_this->ch_trigger, 2) == 0) ) {

        /* 关闭连续测量模式 */
        p_this->ctrl2_reg_buf &= 0xf0;

        /* 温度通道也关闭了，则关闭引脚触发 */
        if (AW_BIT_GET(p_this->ch_trigger, 3) == 0) {
            aw_gpio_trigger_off(p_devinfo->tergger_pin);
        }
    }

    /* 关闭相应中断 */
    if (cur_id == 3) {

        aw_timer_stop(&p_this->timer);

    } else {

        return __mmc5883ma_write(p_this,
                                 __MMC5883MA_CTRL2_REG,
                                 &p_this->ctrl2_reg_buf,
                                 1);
    }

    return AW_OK;
}

/*******************************************************************************
  extern functions
*******************************************************************************/
/**
 * \brief 将 MMC5883MA 驱动注册到 AWBus 子系统中
 */
void awbl_mmc5883ma_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_mmc5883ma);
}


/* end of file */

