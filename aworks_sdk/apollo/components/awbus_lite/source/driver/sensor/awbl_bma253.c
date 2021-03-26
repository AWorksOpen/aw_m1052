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
 * \brief bma253三轴加速度传感器设备
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
  宏定义
*******************************************************************************/
/**< \brief 通过 AWBus设备  定义并初始化  BMA253 设备 */
#define BMA253_DEV_DECL(p_this, p_dev) \
        awbl_sensor_bma253_dev_t *p_this = ( awbl_sensor_bma253_dev_t *)(p_dev)

/**< \brief 通过 AWBus设备 定义并初始化  BMA253 设备信息 */
#define BMA253_DEVINFO_DECL(p_devinfo, p_dev)                  \
         awbl_sensor_bma253_devinfo_t *p_devinfo =             \
        (awbl_sensor_bma253_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

/**< brief 函数返回值检查 */
#define BMA253_CHECK_RET(ret) do { \
        if (ret != AW_OK) {        \
            return ret;            \
        }                          \
    } while (0);

#define BMA253_ENABLE             (1)    /**< \brief 使能 */
#define BMA253_DISABLE            (0)    /**< \brief 失能 */

#define BMA253_ACCEL_X            (0)    /**< \brief 加速度通道x */
#define BMA253_ACCEL_Y            (1)    /**< \brief 加速度通道y */
#define BMA253_ACCEL_Z            (2)    /**< \brief 加速度通道z */
#define BMA253_TEMPERATURE        (3)    /**< \brief 温度数据通道 */
#define BMA253_ACCEL_MASK         (0x07) /**< \brief 加速度通道掩码 */

#define BMA253_INT_NEW_READY      (1)    /**< \brief 新数据就绪中断 */
#define BMA253_INT_THRESHOLD      (2)    /**< \brief 门限触发中断 */

#define BMA253_REG_ACCEL_ADDR     (0x02)    /**< \brief 加速度寄存器子地址 */

#define BMA253_REG_TEMP_ADDR      (0x08)    /**< \brief 温度数据寄存器子地址 */

#define BMA253_REG_INT_STATU1     (0x09)    /**< \brief 配置中断状态寄存器1地址 */
#define BMA253_REG_INT_STATU2     (0x0c)    /**< \brief 配置中断状态寄存器2地址 */

#define BMA253_REG_RANGE_ADDR     (0x0f)    /**< \brief 测量范围寄存器子地址 */

#define BMA253_REG_BW_ADDR        (0x10)    /**< \brief 滤波器带宽寄存器子地址 */

#define BMA253_REG_INT_EN_ADDR    (0x17)    /**< \brief 中断使能寄存器子地址 */

#define BMA253_REG_INT_ADDR1      (0x19)    /**< \brief 配置中断寄存器地址1 */
#define BMA253_REG_INT_ADDR2      (0x1a)    /**< \brief 配置中断寄存器地址2 */
#define BMA253_REG_INT_ADDR3      (0x1b)    /**< \brief 配置中断寄存器地址3 */

#define BMA253_REG_LOW_TH_ADDR    (0x23)    /**< \brief 下门限寄存器子地址 */
#define BMA253_REG_HIGH_TH_ADDR   (0x26)    /**< \brief 上门限寄存器子地址 */
#define BMA253_LOW_TH_MASK        (0x01)    /**< \brief 下门限寄存器掩码 */

#define BMA253_REG_OFFSET_X_ADDR  (0x38)    /**< \brief x通道偏移寄存器子地址 */
#define BMA253_REG_OFFSET_Y_ADDR  (0x39)    /**< \brief y通道偏移寄存器子地址 */
#define BMA253_REG_OFFSET_Z_ADDR  (0x3a)    /**< \brief z通道偏移寄存器子地址 */

#define BMA253_RESET_SOFT         (0x14)    /**< \brief 软件复位寄存器子地址 */
#define BMA253_RESET_INT          (0x21)    /**< \brief 复位中断配置寄存器子地址 */
#define BMA253_RESET_OFFSET       (0x36)    /**< \brief 复位偏移量寄存器子地址 */
/*******************************************************************************
  本地函数声明
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
  本地全局变量定义
*******************************************************************************/
awbl_sensor_bma253_dev_t __g_dev;

struct aw_isr_defer_job __int_high_defer_task;     /* 门限中断延迟处理任务 */
struct aw_isr_defer_job __int_new_defer_task;      /* 新数据中断延迟处理任务 */

/* 量程值对应的寄存器值 */
aw_local const uint8_t __bma253_sample_range[4] = {0x03, 0x05, 0x08, 0x0c};

/* 采样频率对应的BW寄存器的值 */
aw_local const uint8_t __bma253_sample_rate[8] = {0x08, 0x09, 0x0a, 0x0b,
                                                  0x0c, 0x0d, 0x0e, 0x0f};

/** \brief 驱动入口点 */
aw_local aw_const struct awbl_drvfuncs __g_bma253_drvfuncs = {
        __bma253_inst_init1,
        __bma253_inst_init2,
        __bma253_inst_connect
};

/** \brief 驱动提供的方法 */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);
aw_local aw_const struct awbl_dev_method __g_bma253_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __bma253_sensorserv_get),
        AWBL_METHOD_END
};

/** \brief 驱动注册信息 */
aw_local aw_const awbl_plb_drvinfo_t __g_bma253_drv_registration = {
    {
        AWBL_VER_1,                    /* AWBus 版本号 */
        AWBL_BUSID_I2C,                /* 总线 ID */
        AWBL_BMA253_NAME,              /* 驱动名 */
        &__g_bma253_drvfuncs,          /* 驱动入口点 */
        __g_bma253_dev_methods,        /* 驱动提供的方法 */
        NULL                           /* 驱动探测函数 */
    }
};

/** \brief 服务函数 */
aw_local aw_const struct awbl_sensor_servfuncs __g_bma253_servfuncs = {
        __bma253_data_get,             /* 从指定通道获取采样数据 */
        NULL,
        NULL,
        __bma253_attr_set,             /* 配置传感器通道属性 */
        __bma253_attr_get,             /* 获取传感器通道属性 */
        __bma253_trigger_cfg,          /* 设置通道触发 */
        __bma253_trigger_on,           /* 打开触发 */
        __bma253_trigger_off,          /* 关闭触发 */
};

/*******************************************************************************
    本地函数定义
*******************************************************************************/
/*
 * \brief bma253 写数据
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
 * \brief bma253 读数据
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
 * \brief bma253 写寄存器具体位
 */
aw_local aw_err_t __bma253_reg_bit_set (awbl_sensor_bma253_dev_t *p_this,
                                        uint8_t                   subaddr,
                                        uint8_t                   off,
                                        uint8_t                   data)
{
    aw_err_t ret = AW_OK;

    uint8_t reg_data = 0;

    /* 获取寄存器数据 */
    ret = __bma253_read(p_this, subaddr, &reg_data, 1);
    BMA253_CHECK_RET(ret);

    /* 判断是置位还是清零 */
    if (data) {
        reg_data = reg_data | (0x01 << off);
    } else {
        reg_data = reg_data & ~(0x01 << off);
    }

    /* 写入寄存器数据 */
    ret = __bma253_write(p_this, subaddr, &reg_data, 1);
    BMA253_CHECK_RET(ret);

    return AW_OK;
}

/**
 * \brief 获取温度数据
 */
aw_local aw_err_t __bma253_temp_get (awbl_sensor_bma253_dev_t *p_this,
                                     uint8_t                  *p_temp)
{
    aw_err_t ret = AW_OK;

    /* 获取数据 */
    ret = __bma253_read(p_this, BMA253_REG_TEMP_ADDR, p_temp, 1);
    BMA253_CHECK_RET(ret);

    return AW_OK;
}

/**
 * \brief 获取加速度数据
 */
aw_local aw_err_t __bma253_accel_get (awbl_sensor_bma253_dev_t *p_this,
                                      uint16_t                 *p_accel)
{
    uint8_t  accel[6] = {0};

    uint16_t accel_x = 0;
    uint16_t accel_y = 0;
    uint16_t accel_z = 0;

    aw_err_t ret = AW_OK;

    /* 获取数据 */
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
 * \brief 设置新数据就绪触发
 */
aw_local aw_err_t __bma253_new_data_int_set(awbl_sensor_bma253_dev_t *p_this,
                                            uint8_t                   status)
{
    aw_err_t ret = AW_OK;

    switch (status) {
    /* 设置新数据就绪触发 */
    case BMA253_ENABLE:
        /* 绑定新数据触发到int2管脚 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR2,   7, 1);
        BMA253_CHECK_RET(ret);

        /* 使能新数据中断 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, 4, 1);
        BMA253_CHECK_RET(ret);
        break;

    /* 取消新数据就绪触发 */
    case BMA253_DISABLE:
        /* 取消新数据就绪中断绑定 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR2,   7, 0);
        BMA253_CHECK_RET(ret);

        /* 失能新数据就绪中断 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, 4, 0);
        BMA253_CHECK_RET(ret);
        break;

    default:
        break;
    }

    return AW_OK;
}

/**
 * \brief 设置上门限触发
 */
aw_local aw_err_t __bma253_high_trigger_int_set(awbl_sensor_bma253_dev_t *p_this,
                                                uint8_t                   id,
                                                uint8_t                   status)
{
    aw_err_t ret = AW_OK;

    switch (status) {
    /* 设置门限触发 */
    case BMA253_ENABLE:
        /* 绑定上门限触发到int1管脚 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR1, 1, 1);
        BMA253_CHECK_RET(ret);

        /* 使能上门限中断 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, id, 1);
        BMA253_CHECK_RET(ret);
        break;

    /* 取消门限触发设置 */
    case BMA253_DISABLE:
        /* 取消上门限触发绑定 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR1, 1, 0);
        BMA253_CHECK_RET(ret);

        /* 失能上门限中断 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, id, 0);
        BMA253_CHECK_RET(ret);
        break;

    default:
        break;
    }

    return AW_OK;
}

/**
 * \brief 设置下门限触发
 */
aw_local aw_err_t __bma253_low_trigger_int_set(awbl_sensor_bma253_dev_t *p_this,
                                               uint8_t                   status)
{
    aw_err_t ret = AW_OK;

    switch (status) {
    /* 设置下门限触发 */
    case BMA253_ENABLE:
        /* 绑定下门限触发到int1管脚 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR1, 0, 1);
        BMA253_CHECK_RET(ret);

        /* 使能下门限中断 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, 3,  1);
        BMA253_CHECK_RET(ret);
        break;

    /* 取消下门限触发设置 */
    case BMA253_DISABLE:
        /* 取消下门限触发绑定 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_ADDR1, 0, 0);
        BMA253_CHECK_RET(ret);

        /* 失能下门限中断 */
        ret = __bma253_reg_bit_set(p_this, BMA253_REG_INT_EN_ADDR, 3,  0);
        BMA253_CHECK_RET(ret);
        break;

    default:
        break;
    }

    return AW_OK;
}

/**
 * \brief 门限触发的中断服务函数
 */
aw_local void __bma253_int1_isr (void * p_arg)
{
    aw_isr_defer_job_add(&__int_high_defer_task);  /* 添加中断延迟处理任务 */
}

/**
 * \brief 新数据就绪触发的中断服务函数
 */
aw_local void __bma253_int2_isr (void * p_arg)
{
    aw_isr_defer_job_add(&__int_new_defer_task);  /* 添加中断延迟处理任务 */
}

/**
 * \brief 门限触发中断延时处理任务
 */
static void defer_gate_trigger_job_entry (void *p_arg)
{
    awbl_sensor_bma253_dev_t *p_this = ( awbl_sensor_bma253_dev_t *)p_arg;

    uint8_t  trig_chnl = 0;
    uint8_t  trig_src  = 0;

    /* 获取中断状态用于判断触发源 */
    __bma253_read(p_this, BMA253_REG_INT_STATU1, &trig_src, 1);

    /* 获取中断状态用于判断上门限触发通道 */
    __bma253_read(p_this, BMA253_REG_INT_STATU2, &trig_chnl, 1);

    /* 下门限触发 */
    if (trig_src & BMA253_LOW_TH_MASK) {

        p_this->pfn_low_trigger_cb(p_this->p_arg[3],            \
                                   AW_SENSOR_TRIGGER_THRESHOLD);
    } else {  /* 上门限触发 */

        /* 判断触发源 */
        trig_chnl &= BMA253_ACCEL_MASK;
        switch (trig_chnl) {

        /* x轴触发上门限 */
        case (0x01 << BMA253_ACCEL_X):
            if (p_this->high_trigger_set[0]) {
                p_this->pfn_accel_trigger_cb[0](p_this->p_arg[0],            \
                                                AW_SENSOR_TRIGGER_THRESHOLD);
            }
            break;

        /* y轴触发上门限 */
        case (0x01 << BMA253_ACCEL_Y):
            if (p_this->high_trigger_set[1]) {
                p_this->pfn_accel_trigger_cb[1](p_this->p_arg[1],            \
                                                AW_SENSOR_TRIGGER_THRESHOLD);
            }
            break;

        /* z轴触发上门限 */
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
 * \brief 新数据触发中断延时处理任务
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
 * \brief 第一阶段初始化
 */
aw_local void __bma253_inst_init1 (struct awbl_dev *p_dev)
{
    ;
}

/**
 * \brief 第二阶段初始化
 */
aw_local void __bma253_inst_init2 (struct awbl_dev *p_dev)
{
    ;
}

/**
 * \brief 第三阶段初始化
 */
aw_local void __bma253_inst_connect (awbl_dev_t *p_dev)
{
    BMA253_DEV_DECL(p_this, p_dev);
    BMA253_DEVINFO_DECL(p_devinfo, p_dev);

    uint8_t reg_data = 0;

    /* 平台初始化 */
    p_devinfo->pfunc_plfm_init();

    /* 创建从机设备 */
    aw_i2c_mkdev(&p_this->i2c_dev,
                 p_dev->p_devhcf->bus_index,
                 p_devinfo->addr,
                (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));

    /* 上电复位 */
    reg_data = 0xb6;
    __bma253_write(p_this, BMA253_RESET_SOFT, &reg_data, 1); /* 软件复位用户配置 */

    __bma253_reg_bit_set(p_this, BMA253_RESET_INT,    7, 0); /* 清除任何锁存的中断 */
    __bma253_reg_bit_set(p_this, BMA253_RESET_OFFSET, 7, 0); /* 清空偏移量寄存器值 */

    /* 初始化数据 */
    p_this->bma253_val.accel_x = 0;
    p_this->bma253_val.accel_y = 0;
    p_this->bma253_val.accel_z = 0;
    p_this->bma253_val.temp    = 0;
    p_this->accel_z_int[0]     = 0;    /* 通道z的新数据触发 */
    p_this->accel_z_int[1]     = 0;    /* 通道z的上门限触发 */

    /* 传感器默认值 */
    p_this->sample_rate  = AW_BMA253_2000_HZ;
    p_this->sample_range = AW_BMA253_2G;

    p_this->high_th      = 0xc0;  /* 192 * 7.81mg = 1.4995g ≈ 1.5g */

    /* 保存一个本地设备信息 */
    __g_dev = (*p_this);
}

/**
 * \brief 方法
 */
aw_local void __bma253_sensorserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    BMA253_DEV_DECL(p_this, p_dev);
    BMA253_DEVINFO_DECL(p_devinfo, p_dev);

    awbl_sensor_service_t *p_sensor_serv = NULL;

    /*
     *  该传感器的通道分配
     */
    aw_local const awbl_sensor_type_info_t type_info[] = {
         {AW_SENSOR_TYPE_ACCELEROMETER, 3},    /* 3路加速度  */
         {AW_SENSOR_TYPE_TEMPERATURE, 1}       /* 1路温度 */
    };

    aw_local const awbl_sensor_servinfo_t serv_info = {
         4,                                   /* 共支持4个通道  */
         type_info,
         AW_NELEMENTS(type_info)              /* 类型列表数目    */
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
 * \brief 获取指定通道的采样数据
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
    uint8_t  temp     =  0;     /* 温度为8bit数据 */
    uint16_t accel[3] = {0};    /* 加速度为12bit数据 */
    uint16_t unit[4]  = {976, 1953, 3906, 7812};
    uint32_t cur_id   =  0;
    uint32_t start_id = p_devinfo->start_id;

    /* 获取加速度 */
    ret = __bma253_accel_get(p_this, accel);
    BMA253_CHECK_RET(ret);

    /* 获取温度 */
    ret = __bma253_temp_get(p_this, &temp);
    BMA253_CHECK_RET(ret);

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i];
        chnl    = cur_id - start_id;
        if (chnl >= 0 && chnl < 3) {

            p_buf[i].val  = accel[chnl];          /* 获取加速度通道 */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO; /* 单位10^-6 */

            /* 输出数据格式转换 */
            (p_buf[i].val & (1 << 11)) ? p_buf[i].val |=
                              (~0xFFF) : p_buf[i].val;
            p_buf[i].val  = ~p_buf[i].val;
            p_buf[i].val *= unit[p_this->sample_range];  /* ±2g范围应乘以976ug */

        } else if ( chnl == 3) {

            p_buf[i].val  = (int8_t)temp;        /* 获取温度通道 */
            p_buf[i].unit = AW_SENSOR_UNIT_DECI; /* 单位10^-1 */

            p_buf[i].val  = p_buf[i].val * 5 + 230;

        } else {
            return AW_OK;                /* 若此次通道不属于该传感器，直接退出 */
        }
    }

    return AW_OK;
}

/**
 * \brief 配置传感器通道属性
 */
aw_local aw_err_t __bma253_attr_set (void                    *p_cookie,
                                     int                      id,
                                     int                      attr,
                                     const aw_sensor_val_t   *p_val)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);

    aw_err_t ret = AW_OK;

    int8_t  chnl     = id - p_devinfo->start_id; /* 获取当前通道号 */
    uint8_t freq     = 0;
    uint8_t range    = 0;
    int8_t  offset   = 0;
    uint8_t offset_w = 0;
    uint8_t high_th  = 0;
    uint8_t bma253_reg_addr = 0;
    uint16_t unit[4] = {7812, 15624, 31248, 62496};
    double  data_float = 0.0;

    switch (attr) {

    /* 设置采样频率 */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        if (!(chnl >= 0 && chnl < 3)) {
            return -AW_ENODEV; /* 通道不支持 */
        }

        data_float = (double)p_val->val;
        data_float = data_float * pow(10, p_val->unit);

        if ((int)data_float >= 0 && (int)data_float <= 7) {
            freq = __bma253_sample_rate[(uint8_t)data_float];

            /* 写入采样频率设置 */
            ret = __bma253_write(p_this, BMA253_REG_BW_ADDR, &freq, 1);
            BMA253_CHECK_RET(ret);

            /* 写入设备实例 */
            p_this->sample_rate = (uint8_t)data_float;

        } else {

            return -AW_EINVAL;  /* 无效的参数 */
        }
        break;

    /* 设置量程值 */
    case AW_SENSOR_ATTR_FULL_SCALE:
        if (!(chnl >= 0 && chnl < 3)) {
            return -AW_ENODEV; /* 通道不支持 */
        }
        if (p_val->val >= 0 && p_val->val <= 3) {
            range = __bma253_sample_range[(uint8_t)p_val->val];

            /* 写入采样范围设置 */
            ret = __bma253_write(p_this, BMA253_REG_RANGE_ADDR, &range, 1);
            BMA253_CHECK_RET(ret);

            /* 写入设备实例 */
            p_this->sample_range = (uint8_t)p_val->val;

        } else {

            return -AW_EINVAL;  /* 无效的参数 */
        }
        break;

    /* 设置偏移量 */
    case AW_SENSOR_ATTR_OFFSET:
        switch (chnl) {
        /* 通道x */
        case BMA253_ACCEL_X:
            bma253_reg_addr = BMA253_REG_OFFSET_X_ADDR;
            break;
        /* 通道y */
        case BMA253_ACCEL_Y:
            bma253_reg_addr = BMA253_REG_OFFSET_Y_ADDR;
            break;
        /* 通道z */
        case BMA253_ACCEL_Z:
            bma253_reg_addr = BMA253_REG_OFFSET_Z_ADDR;
            break;

        default:
            return -AW_ENOTSUP; /* 不支持 */
        }

        data_float = (double)p_val->val;
        data_float = data_float * pow(10, p_val->unit);
        data_float = data_float * pow(10, 6);
        data_float = data_float / 7800;

        offset   = (int8_t)data_float;
        offset_w = (uint8_t)offset;

        /* 写入通道补偿值 */
        ret = __bma253_write(p_this, bma253_reg_addr, &offset_w, 1);
        BMA253_CHECK_RET(ret);

        /* 写入设备实例 */
        p_this->offset[chnl] = (uint32_t)offset;

        break;

    /* 设置下门限值，用于门限触发模式 */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        return -AW_ENODEV;  /* 不支持 */

    /* 设置上门限值，用于门限触发模式 */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:

        data_float = (double)p_val->val;
        data_float = data_float * pow(10, p_val->unit);
        data_float = data_float * pow(10, 6);
        data_float = data_float / unit[p_this->sample_range];

        high_th = (uint8_t)data_float;

        /* 写入寄存器数据 */
        ret = __bma253_write(p_this, BMA253_REG_HIGH_TH_ADDR, &high_th, 1);
        BMA253_CHECK_RET(ret);
        break;

    /* 采样数据持续次数，用于门限触发模式*/
    case AW_SENSOR_ATTR_DURATION_DATA:
        return -AW_ENODEV;  /* 不支持 */

    default:
        break;
    }

    return AW_OK;
}

/**
 * \brief 获取传感器通道属性
 */
aw_local aw_err_t __bma253_attr_get (void                   *p_cookie,
                                     int                     id,
                                     int                     attr,
                                     aw_sensor_val_t        *p_val)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);

    /* 获取当前通道号 */
    int8_t chnl = id - p_devinfo->start_id;

    /* 温度通道无属性设置 */
    if (chnl == BMA253_TEMPERATURE) {
        return -AW_ENOTSUP;
    }

    switch (attr) {
    /* 获取采样频率 */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        p_val->val  = p_this->sample_rate;
        p_val->unit = 0;
        break;

    /* 获取采样量程 */
    case AW_SENSOR_ATTR_FULL_SCALE:
        p_val->val  = p_this->sample_range;
        p_val->unit = 0;
        break;

    /* 获取通道补偿量 */
    case AW_SENSOR_ATTR_OFFSET:
        p_val->val  = p_this->offset[chnl];
        p_val->unit = 0;
        break;

    /* 下门限值，用于门限触发模式 */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        p_val->val  = p_this->low_th;
        p_val->unit = 0;
        break;

    /* 上门限值，用于门限触发模式 */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        p_val->val  = p_this->high_th;
        p_val->unit = 0;
        break;

    /* 采样数据持续次数，用于门限触发模式*/
    case AW_SENSOR_ATTR_DURATION_DATA:
        return -AW_ENOTSUP;  /* 不支持 */

    default:
        break;
    }

    return AW_OK;
}

/**
 * \brief 设置触发，一个通道仅能设置一个触发回调函数
 */
aw_local aw_err_t  __bma253_trigger_cfg (void                     *p_cookie,
                                         int                       id,
                                         uint32_t                  flags,
                                         aw_sensor_trigger_cb_t    pfn_cb,
                                         void                     *p_arg)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);

    /* 获取当前通道号 */
    int8_t chnl = id - p_devinfo->start_id;

    switch(flags) {

    /* 新数据就绪触发 */
    case AW_SENSOR_TRIGGER_DATA_READY:
        if (chnl != BMA253_ACCEL_Z) {
            return -AW_ENOTSUP;
        }
        p_this->pfn_accel_trigger_cb[chnl] = pfn_cb;
        p_this->p_arg[chnl]                = p_arg;
        p_this->accel_z_int[0]             = BMA253_ENABLE;
        break;

    /* 门限触发 */
    case AW_SENSOR_TRIGGER_THRESHOLD:
        if (!(chnl >= 0 && chnl < 3)) {
            return -AW_ENODEV; /* 通道不支持 */
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
 * \brief 打开触发
 */
aw_local aw_err_t __bma253_trigger_on (void *p_cookie, int id)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);

    /* 获取当前通道号 */
    uint8_t chnl = id - p_devinfo->start_id;

    switch (chnl) {

    case BMA253_ACCEL_X:
    case BMA253_ACCEL_Y:
        p_this->high_trigger_set[chnl] = BMA253_ENABLE;

        aw_isr_defer_job_init(&__int_high_defer_task, \
                               defer_gate_trigger_job_entry, p_this);
        /* 连接回调函数到引脚 */
        aw_gpio_trigger_connect(p_devinfo->int1_port, __bma253_int1_isr, NULL);
        /* 设置引脚触发类型，上升沿触发 */
        aw_gpio_trigger_cfg(p_devinfo->int1_port, AW_GPIO_TRIGGER_RISE);
        /* 开启引脚上的触发器 */
        aw_gpio_trigger_on(p_devinfo->int1_port);

        /* 设置传感器上门限中断 */
        __bma253_high_trigger_int_set(p_this, chnl, BMA253_ENABLE);
        break;

    case BMA253_ACCEL_Z:
        if (p_this->accel_z_int[0] == BMA253_ENABLE) { /* 新数据就绪触发 */

            aw_isr_defer_job_init(&__int_new_defer_task, \
                                   defer_new_trigger_job_entry, p_cookie);
            /* 连接回调函数到引脚 */
            aw_gpio_trigger_connect(p_devinfo->int2_port, \
                                    __bma253_int2_isr, NULL);
            /* 设置引脚触发类型，上升沿触发 */
            aw_gpio_trigger_cfg(p_devinfo->int2_port, AW_GPIO_TRIGGER_RISE);
            /* 开启引脚上的触发器 */
            aw_gpio_trigger_on(p_devinfo->int2_port);

            /* 设置传感器新数据触发中断 */
            __bma253_new_data_int_set(p_this, BMA253_ENABLE);

        } else if (p_this->accel_z_int[1] == BMA253_ENABLE) { /* 门限触发 */

            p_this->high_trigger_set[chnl] = BMA253_ENABLE;

            aw_isr_defer_job_init(&__int_high_defer_task, \
                                   defer_gate_trigger_job_entry, p_this);
            /* 连接回调函数到引脚 */
            aw_gpio_trigger_connect(p_devinfo->int1_port, \
                                    __bma253_int1_isr, NULL);
            /* 设置引脚触发类型，双边沿触发 */
            aw_gpio_trigger_cfg(p_devinfo->int1_port, AW_GPIO_TRIGGER_RISE);
            /* 开启引脚上的触发器 */
            aw_gpio_trigger_on(p_devinfo->int1_port);

            /* 设置传感器上门限中断 */
            __bma253_high_trigger_int_set(p_this, chnl, BMA253_ENABLE);
        }
        break;

    default:
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

/**
 * \brief 关闭触发
 */
aw_local aw_err_t __bma253_trigger_off (void *p_cookie, int id)
{
    BMA253_DEV_DECL(p_this, p_cookie);
    BMA253_DEVINFO_DECL(p_devinfo, p_cookie);

    /* 获取当前通道号 */
    uint8_t chnl = id - p_devinfo->start_id;

    switch (chnl) {

    case BMA253_ACCEL_X:
    case BMA253_ACCEL_Y:
        /* 失能对应通道上门限触发 */
        p_this->high_trigger_set[chnl] = BMA253_DISABLE;

        break;

    case BMA253_ACCEL_Z:
        if (p_this->accel_z_int[0] == BMA253_ENABLE) {        /* 关闭新数据触发 */

            p_this->accel_z_int[0] = BMA253_DISABLE;
            /* 关闭引脚上的触发器 */
            aw_gpio_trigger_off(p_devinfo->int2_port);
            /* 禁能新数据中断 */
            __bma253_new_data_int_set(p_this, BMA253_DISABLE);

        } else if (p_this->accel_z_int[1] == BMA253_ENABLE) { /* 关闭门限触发 */

            p_this->accel_z_int[1] = BMA253_DISABLE;
            p_this->high_trigger_set[chnl] = BMA253_DISABLE;
        }
        break;

    default:
        return -AW_ENOTSUP;
    }

    /* 如果所有通道的上门限触发均失能，则关闭中断 */
    if (p_this->high_trigger_set[0] + \
        p_this->high_trigger_set[1] + \
        p_this->high_trigger_set[2] == 0) {

        /* 关闭传感器上门限中断 */
        __bma253_high_trigger_int_set(p_this, chnl, BMA253_DISABLE);

        if (p_this->low_trigger_set == BMA253_DISABLE) {
            /* 关闭引脚上的触发器 */
            aw_gpio_trigger_off(p_devinfo->int1_port);
        }
    }

    return AW_OK;
}
/*******************************************************************************
    外部函数定义
*******************************************************************************/
/**
 * \brief 配置 BMA253 的下门限触发
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

        return -AW_EINVAL;  /* 参数不正确 */
    }

    /* 设置下门限值 */
    gate_th = (uint16_t)p_val->val;

    low_th_float = (double)gate_th;
    low_th_float = low_th_float * pow(10, p_val->unit);
    low_th_float = low_th_float * pow(10, 6);
    low_th_float = low_th_float / unit[__g_dev.sample_range];

    low_th = (uint8_t)low_th_float;

    /* 写入下门限数据 */
    ret = __bma253_write(&__g_dev, BMA253_REG_LOW_TH_ADDR, &low_th, 1);
    BMA253_CHECK_RET(ret);

    /* 保存回调信息 */
    __g_dev.pfn_low_trigger_cb = pfn_cb;
    __g_dev.p_arg[3]           = p_arg;

    return AW_OK;
}

/**
 * \brief BMA253 下门限触发 开启
 */
aw_err_t awbl_bma253_low_trigger_on (int dev_id)
{
    BMA253_DEVINFO_DECL(p_devinfo, &__g_dev);

    if (dev_id != 0) {

        return -AW_EINVAL;  /* 参数不正确 */
    }

    __g_dev.low_trigger_set = BMA253_ENABLE;

    /* 设置传感器下门限中断 */
    __bma253_low_trigger_int_set(&__g_dev, BMA253_ENABLE);

    aw_isr_defer_job_init(&__int_high_defer_task, \
                           defer_gate_trigger_job_entry, &__g_dev);
    /* 连接回调函数到引脚 */
    aw_gpio_trigger_connect(p_devinfo->int1_port, __bma253_int1_isr, NULL);
    /* 设置引脚触发类型，上升沿触发 */
    aw_gpio_trigger_cfg(p_devinfo->int1_port, AW_GPIO_TRIGGER_RISE);
    /* 开启引脚上的触发器 */
    aw_gpio_trigger_on(p_devinfo->int1_port);

    return AW_OK;
}

/**
 * \brief BMA253 下门限触发 关闭
 */
aw_err_t awbl_bma253_low_trigger_off (int dev_id)
{
    BMA253_DEVINFO_DECL(p_devinfo, &__g_dev);

    if (dev_id != 0) {

        return -AW_EINVAL;  /* 参数不正确 */
    }

    __g_dev.low_trigger_set = BMA253_DISABLE;

    /* 设置传感器下门限中断 */
    __bma253_low_trigger_int_set(&__g_dev, BMA253_DISABLE);

    /* 如果所有通道的上门限触发均失能，则关闭中断 */
    if (__g_dev.high_trigger_set[0] + \
        __g_dev.high_trigger_set[1] + \
        __g_dev.high_trigger_set[2] == 0) {

        /* 关闭引脚上的触发器 */
        aw_gpio_trigger_off(p_devinfo->int1_port);
    }

    return AW_OK;
}

/**
 * \brief 将 BMA253 驱动注册到 AWBus 子系统中
 */
void awbl_bma253_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_bma253_drv_registration);
}

/* end of file */
