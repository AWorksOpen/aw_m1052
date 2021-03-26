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
 * \brief BME280 (气压温湿度传感器)头文件
 *
 * \internal
 * \par Modification History
 * - 1.00 18-12-26  ipk, first implementation
 * \endinternal
 */

#ifndef __AWBL_BME280_H
#define __AWBL_BME280_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "awbl_sensor.h"
#include "aw_i2c.h"
#include "aw_compiler.h"

#define AWBL_BME280_NAME      "awbl_bme280"

/**
 * \brief BME280 IIR滤波参数
 */
typedef enum bme280_filter {
    AWBL_BME280_FILTER_OFF = 0,                   /**< \brief 关闭IIR滤波             */
    AWBL_BME280_FILTER_COEFFICIENT_2,             /**< \brief 滤波参数为2             */
    AWBL_BME280_FILTER_COEFFICIENT_4,             /**< \brief 滤波参数为4             */
    AWBL_BME280_FILTER_COEFFICIENT_8,             /**< \brief 滤波参数为8             */
    AWBL_BME280_FILTER_COEFFICIENT_16             /**< \brief 滤波参数为16            */
} awbl_bme280_filter_t;

/**
 * \brief BME280过采样比率
 */
typedef enum bme280_oversampling {
    AWBL_BME280_OVERSAMPLE_SKIP = 0,              /**< \brief 跳过过采样              */
    AWBL_BME280_OVERSAMPLE_X_1,                   /**< \brief 过采样比率1             */
    AWBL_BME280_OVERSAMPLE_X_2,                   /**< \brief 过采样比率2             */
    AWBL_BME280_OVERSAMPLE_X_4,                   /**< \brief 过采样比率4             */
    AWBL_BME280_OVERSAMPLE_X_8,                   /**< \brief 过采样比率8             */
    AWBL_BME280_OVERSAMPLE_X_16                   /**< \brief 过采样比率16            */
} awbl_bme280_oversampling_t;

/**
 * \brief BME280模式
 */
typedef enum bme280_mode {
    AWBL_BME280_SLEEP_MODE = 0,                   /**< \brief 睡眠模式                */
    AWBL_BME280_FORCED_MODE,                      /**< \brief 强制模式                */
    AWBL_BME280_FORCED_MODE_ALSO,                 /**< \brief 强制模式，同上          */
    AWBL_BME280_NORMAL_MODE                       /**< \brief 正常模式                */
} awbl_bme280_mode_t;


/**
 * \brief BME280 设备信息结构体
 */
typedef struct awbl_bme280_devinfo {
    /**
     * \brief 该传感器通道的起始id
     *
     * BME280 共有3路采集通道,分别为气压数据、温度数据和湿度数据，从
     * start_id开始(包括start_id)连续三个ID分配给该模块的三个通道，其中
     * start_id 对应模块气压数据，start_id + 1 对应模块的温度数据...
     * eg： 将start_id配置为0, 则ID = 0,对应模块的通道1，ID = 1对应模块的通道2.
     */
    int                        start_id;

    /** \brief I2C从机地址,7位地址 */
    uint8_t                    i2c_addr;

    /**
     * \brief IIR滤波参数
     *
     * 传感器IIR滤波系数的设定。IIR滤波器又称为“IIR数字滤波器”、“递归滤波器”，
     * 递归滤波器，顾名思义采用递归型结构，即结构上带有反馈环路。
     *
     * 实际上传感器内部的湿度数据不会快速波动，也不需要低通滤波。但是，环境的压力会受到
     * 许多短期变化的影响，例如：通过抨击门窗、吹入传感器的风等等。为了抑制输出数据
     * 中的这些干扰，用户可以配置BME280内部的IIR滤波器。它有效的降低了温度和压力
     * 输出信号的带宽，并将压力和温度输出的数据分辨率提高到20位。其公式为：
     *
     *               dat_filter_old*(filter_coeff-1)+dat_ADC
     * dat_filter = -----------------------------------------
     *                            filter_coeff
     *
     * dat_filter_old：来自当前滤波器存储器的数据
     * dat_ADC       ：来自当前传感器ADC获取的数据
     * dat_filter    ：滤波器内存的新值，也是即将发送到输出寄存器的值
     * filter_coeff  ：需要设置的滤波系数
     *
     * 基于此，可知filter_coeff滤波系数越大，传感器输入响应则越迟缓。
     * BME280可支持的滤波系数包括2,4,8,16四种，若该值设置为0时则关闭IIR滤波。
     */
    awbl_bme280_filter_t       filter_coeff;

    /**
     * \brief 温度过采样比率系数
     *
     * 过采样是指采样频率远大于奈奎斯特采样频率，设原系统采样频率为fs，
     * 若将采样频率提高到R*fs，R则称为过采样比率。
     * 过采样比率越大能够有效的提高时域分辨率，减小噪音。
     *
     * BME280可设置的过采样比率为1,2,4,8,16五种，设置为0时则跳过过采样
     */
    awbl_bme280_oversampling_t temp_oversampling;

    /**
     * \brief 压力过采样比率系数
     */
    awbl_bme280_oversampling_t press_oversampling;

    /**
     * \brief 湿度过采样比率系数
     */
    awbl_bme280_oversampling_t hum_oversampling;

    /**
     * \brief 工作模式设定
     *
     * -SLEEP_MODE：睡眠模式， 该模式下，可对寄存器正常读写，传感器不执行任何测量操作，
     *             一般不主动设置为睡眠模式。
     *
     * -FORCED_MODE/FORCED_MODE_ALSO：
     *         强制模式，若设置为该模式，获取传感器数据时会进入强制模式，根据所选的滤波器
     *         选项进行单次测量，测量完成后会自动返回睡眠模式，下次获取数据时会再次进入
     *         强制模式。对于要求低采样率、低功耗或基于主机同步的应用，建议使用该模式。
     *
     * -NORMAL_MODE：正常模式，该模式下是测量(有效)时段和待机(无效)时段的自动永久循环，
     *         也就是按照一定采样频率循环测量数据，该模式功耗高于上述两种模式。在设置过采样
     *         和滤波器后启用正常模式，可始终在数据寄存器中获取最新的测量结果。使用IIR滤波
     *         器时，建议使用该模式，这样对过滤短期干扰非常有用。
     */
    awbl_bme280_mode_t         mode;

} awbl_bme280_devinfo_t;

/**
 * \brief 补偿值结构体
 */
typedef struct awbl_compensation {

    /* 存放温度补偿寄存器的值 */
    uint16_t   dig_t1;
    int16_t    dig_t2;
    int16_t    dig_t3;

    /* 存放压力补偿寄存器的值 */
    uint16_t   dig_p1;
    int16_t    dig_p2;
    int16_t    dig_p3;
    int16_t    dig_p4;
    int16_t    dig_p5;
    int16_t    dig_p6;
    int16_t    dig_p7;
    int16_t    dig_p8;
    int16_t    dig_p9;

    /* 存放湿度补偿寄存器的值 */
    uint8_t    dig_h1;
    int16_t    dig_h2;
    uint8_t    dig_h3;
    int16_t    dig_h4;
    int16_t    dig_h5;
    int8_t     dig_h6;
} awbl_compensation_t;

/**
 * \brief BME280 设备结构体
 */
typedef struct awbl_bme280_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev         dev;

    /** \brief I2C从机设备  */
    aw_i2c_device_t         i2c_dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t   sensor_serv;

    /** \brief 存放补偿值 */
    awbl_compensation_t     compensation;

    /** \brief 计算补偿值所需中间量 */
    int32_t                 fine_temp;

    /** \brief 传感器采样速率 */
    uint8_t                 rate;

} awbl_bme280_dev_t;


/*
 * \brief 注册 BME280 驱动
 */
void awbl_bme280_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_BME280_H */

/* end of file */
