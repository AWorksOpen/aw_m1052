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
 * \brief 标准信号采集模块 TPS08U 驱动
 *
 * 本驱动为 TPS08U 模块的操作接口库，驱动的底层使用 SPI 标准接口来操作 TPS08U 模
 * 块
 *
 * TPS08U 共有 8 个通道(CH1~CH8)，每 2 个通道为一组，如 CH1 与 CH2 为一组，CH3 
 * 与CH4 为一组，以此类推。每组通道可以自由配置为电压通道或者电流通道。
 *
 * 在使用传感器接口(aw_sensor.h)操作 TPS08U 的时候，TPS08U 提供了 8 个电压/电流
 * 通道，通道 ID 为设备信息中的 start_id ~ start_id + 7。
 *
 * 用户可以手动通过 aw_sensor_data_get() 获取通道数据，也可以使用 
 * aw_sensor_attr_set()配置采样频率，推荐的采样频率为 50Hz，由于 TPS08U 所有通道
 * 数据是一起更新的，所以配置采样频率时，通道参数被忽略。在配置采样频率后，系统
 * 将自动按照采样频率获取传感器所有使能的通道的采样值，并存于内部缓存中。若用户
 * 设置了数据准备就绪触发，则每次数据采样结束后，都将触发用户回调函数，用户可在
 * 回调函数中使用 aw_sensor_data_get() 获取通道数据。
 *
 * 由于 TPS08U 本身不支持触发功能，驱动中的数据准备就绪触发、门限触发以及增量触
 * 发都是通过软件模拟实现的，所以需要配置采样频率大于 0 后，配置好的触发回调函数
 * 才会被正常回调。
 *
 * 还需要注意的是，当采样频率配置大于 0 的时候，系统会自动获取传感器值，此时使用
 * 其它接口操作TPS08U 会返回错误 -AW_EBUSY，所以建议用户配置完成需要配置的选项之
 * 后再配置采样频率。
 *
 * \internal
 * \par modification history
 * - 1.00 18-05-03  pea, first implementation
 * \endinternal
 */

#ifndef __AWBL_TPS08U_H
#define __AWBL_TPS08U_H

#include "awbus_lite.h"
#include "aw_spi.h"
#include "aw_adc.h"
#include "aw_types.h"
#include "aw_delayed_work.h"
#include "awbl_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief TPS08U 设备名称 */
#define    AWBL_TPS08U_NAME    "awbl_tps08u"

/** \brief TPS08U 设备信息 */
struct awbl_tps08u_devinfo {
    int      start_id;    /**< \brief 传感器起始 ID */
    uint32_t sclk_freq;   /**< \brief SPI 时钟频率 */
    int      cs_pin;      /**< \brief SPI 片选引脚号 */
    
    /** \brief 通道模式，每位表示一组通道，0 为电压，1 为电流 */
    uint8_t  chan_mode;   
};

/** \brief TPS08U 设备实例 */
struct awbl_tps08u_dev {
    struct awbl_dev         dev;             /**< \brief AWBus 设备实例结构体 */

    awbl_sensor_service_t   sensor_service;     /**< \brief 传感器服务  */
    uint32_t                sampling_rate;      /**< \brief 传感器采样频率 */
    int32_t                 offset[8];          /**< \brief 传感器偏移值 */
    int32_t                 threshold_lower[8]; /**< \brief 传感器下门限 */
    int32_t                 threshold_upper[8]; /**< \brief 传感器上门限 */
    int32_t                 threshold_slope[8]; /**< \brief 传感器增量门限 */
    int32_t                 duration_data[8];   /**< \brief 采样数据持续次数 */
    int32_t                 duration_slope[8];  /**< \brief 增量持续次数 */
    int32_t                 threshold_type[8];  /**< \brief 当前门限触发类型 */
    uint32_t                threshold_cnt[8];   /**< \brief 门限触发次数 */
    uint32_t                slope_cnt[8];       /**< \brief 斜率触发次数 */
    uint32_t                flags[8];           /**< \brief 触发标志 */
    
    /** \brief 触发时执行的回调函数的指针 */
    aw_sensor_trigger_cb_t  pfn_cb[8];  
    void                   *p_arg[8];           /**< \brief 回调函数用户参数 */
    
    /** \brief 触发使能位，每位对应一个通道 */
    uint8_t                 trigger;    

    struct aw_delayed_work  sampling_work;     /**< \brief 传感器采样延迟作业 */

    aw_spi_device_t         spi_dev;           /**< \brief SPI 从设备描述结构 */
    aw_spi_message_t        msg[2];            /**< \brief SPI 消息 */
    aw_spi_transfer_t       trans[2];          /**< \brief SPI 传输 */

    volatile aw_bool_t is_busy;          /**< \brief 设备当前是否繁忙 */
    volatile uint8_t   spi_state;        /**< \brief SPI 状态 */
    uint8_t            reg_addr;         /**< \brief 当前操作的寄存器地址 */
    void              *p_buf;            /**< \brief 当前操作的缓冲区地址 */
    aw_bool_t          is_read;          /**< \brief 当前传输是否为读数据 */
    uint8_t            cmd;              /**< \brief 当前传输的 CMD */
    uint8_t            current_chan;     /**< \brief 设备当前转换的通道 */
    uint8_t            read_state;       /**< \brief 读取状态 */
    uint8_t            reg_state;        /**< \brief 保存读取的状态寄存器 */
    uint8_t            reg_chan_enable;  /**< \brief 保存读取的通道使能寄存器 */
    uint8_t            reg_chan_mode;    /**< \brief 保存读取的通道模式寄存器 */
    uint32_t           reg_data[8];      /**< \brief 当前采集到的通道数据 */
    uint32_t           reg_data_last[8]  /**< \brief 上一次采集到的通道数据 */
};

#define AWBL_TPS08U_MODE_CH1_2_V  0         /**< \breif 通道 1~2 配置为电压通道 */
#define AWBL_TPS08U_MODE_CH1_2_C  AW_BIT(0) /**< \breif 通道 1~2 配置为电流通道 */
#define AWBL_TPS08U_MODE_CH3_4_V  0         /**< \breif 通道 3~4 配置为电压通道 */
#define AWBL_TPS08U_MODE_CH3_4_C  AW_BIT(1) /**< \breif 通道 3~4 配置为电流通道 */
#define AWBL_TPS08U_MODE_CH5_6_V  0         /**< \breif 通道 5~6 配置为电压通道 */
#define AWBL_TPS08U_MODE_CH5_6_C  AW_BIT(2) /**< \breif 通道 5~6 配置为电流通道 */
#define AWBL_TPS08U_MODE_CH7_8_V  0         /**< \breif 通道 7~8 配置为电压通道 */
#define AWBL_TPS08U_MODE_CH7_8_C  AW_BIT(3) /**< \breif 通道 7~8 配置为电流通道 */

#define AWBL_TPS08U_STATE_ADDR_ERROR   AW_BIT(0) /**< \breif 寄存器地址错误 */
#define AWBL_TPS08U_STATE_UNABLE_WRITE AW_BIT(1) /**< \breif 寄存器不可写入 */
#define AWBL_TPS08U_STATE_WRITE_FAILED AW_BIT(2) /**< \breif 寄存器写入失败 */
#define AWBL_TPS08U_STATE_SPI_ERROR    AW_BIT(3) /**< \breif SPI 状态异常 */
#define AWBL_TPS08U_STATE_ADC_ERROR    AW_BIT(4) /**< \breif ADC 异常 */
#define AWBL_TPS08U_STATE_DATA_ERROR   AW_BIT(5) /**< \breif 写入数据异常 */
#define AWBL_TPS08U_STATE_DATA_UPDATE  AW_BIT(6) /**< \breif 通道数据更新标志 */

/**
 * \brief 检测与 TPS08U 的通信是否正常
 *
 * \param[in] id 设备号
 *
 * \retval  AW_OK     通信成功
 * \retval -AW_ENODEV 通信失败
 */
aw_err_t awbl_tps08u_detect (int id);

/**
 * \brief 获取 TPS08U 使能的通道
 *
 * \param[in]  id            设备号
 * \param[in]  chan          通道号，为 1~8 时获取对应通道状态，为 0 时获取所有
 *                           通道使能状态
 * \param[out] p_chan_enable 指向存储获取到的使能的通道的指针，
 *                           当通道号为 1~8 时， 0 表示该通道失能，1 表示该通道使
 *                           能；
 *                           当通道号配置为 0 时，获取所有通道状态，每一位指示一
 *                           个通道的状态，Bit0 表示通道 1，Bit1 表示通道 2，
 *                           Bit2 表示通道 3，以此类推，相应位为 0 时表示该通道
 *                           失能，为 1 时表示该通道使能
 *
 * \retval  AW_OK 获取成功
 * \retval   <0   获取失败
 */
aw_err_t awbl_tps08u_chan_enable_get (int      id,
                                      uint8_t  chan,
                                      uint8_t *p_chan_enable);

/**
 * \brief 配置 TPS08U 使能的通道
 *
 * \param[in] id          设备号
 * \param[in] chan        通道号，为 1~8 时配置对应通道状态，为 0 时配置所有通道
 *                        使能状态
 * \param[in] chan_enable 待配置的通道使能配置，
 *                        当通道号为 1~8 时，0 表示该通道失能，1 表示该通道使能；
 *                        当通道号配置为 0 时，配置所有通道状态，每一位指示一个
 *                        通道的状态，Bit0 表示通道 1，Bit1 表示通道 2，Bit2 表
 *                        示通道 3，以此类推，相应位为 0 时表示该通道失能，为 1 
 *                        时表示该通道使能
 *
 * \retval  AW_OK 配置成功
 * \retval   <0   配置失败
 *
 * \note 用户可以自由配置每个通道是失能或者使能，失能的通道不进行采样，
 *       所以关闭不使用的通道可以加快数据更新速率
 */
aw_err_t awbl_tps08u_chan_enable_set (int id, uint8_t chan, uint8_t chan_enable);

/**
 * \brief 获取 TPS08U 通道模式
 *
 * \param[in]  id          设备号
 * \param[in]  chan        通道号，为 1~8 时获取对应通道状态，为 0 时获取所有通
 *                         道模式
 * \param[out] p_chan_mode 指向存储获取到的通道模式的指针，
 *                         当通道号为 1~8 时，0 表示该通道为电压通道，1 表示该通
 *                         道为电流通道；
 *                         当通道号配置为 0 时，每一位代表 2 个通道，
 *                         Bit0 表示通道 1 和通道 2，Bit1 表示通道 3 和通道 4，
 *                         Bit2 表示通道 5 和通道 6，以此类推，
 *                         相应位为 0 时表示该通道为电压通道，为 1 时表示该通道
 *                         为电流通道
 *
 * \retval  AW_OK 获取成功
 * \retval   <0   获取失败
 */
aw_err_t awbl_tps08u_chan_mode_get (int id, uint8_t  chan, uint8_t *p_chan_mode);

/**
 * \brief 配置 TPS08U 通道模式
 *
 * \param[in] id        设备号
 * \param[in] chan      通道号，为 1~8 时配置对应通道状态，为 0 时配置所有通道模
 *                      式
 * \param[in] chan_mode 待配置的通道模式，
 *                      当通道号为 1~8 时，0 表示该通道为电压通道，1 表示该通道
 *                      为电流通道；
 *                      当通道号配置为 0 时，每一位代表 2 个通道，
 *                      Bit0 表示通道 1 和通道 2，Bit1 表示通道 3 和通道 4，
 *                      Bit2 表示通道 5 和通道 6，以此类推，
 *                      相应位为 0 时表示该通道为电压通道，为 1 时表示该通道为
 *                      电流通道
 *
 * \retval  AW_OK 配置成功
 * \retval   <0   配置失败
 *
 * \note 由于 2 个通道为一组，所以改变单个通道时，与其同组的通道模式也会被改变
 */
aw_err_t awbl_tps08u_chan_mode_set (int id, uint8_t chan, uint8_t chan_mode);

/**
 * \brief 获取 TPS08U 通道数据
 *
 * \param[in]  id          设备号
 * \param[in]  chan        通道号，为 1~8 时获取对应通道数据，为 0 时获取所有通
 *                         道数据
 * \param[out] p_chan_data 指向存储获取到的通道数据的指针，
 *                         当通道号为 1~8 时，缓冲区大小应为 1 个单精度的大小；
 *                         当通道号配置为 0 时，缓冲区大小应为 8 个单精度的大小；
 *
 * \retval  AW_OK 获取成功
 * \retval   <0   获取失败
 *
 * \note 获取到的通道数据的单位由通道模式决定，
 *       如果为电压通道，单位为 V，范围为 0~5V，
 *       如果为电流通道，单位为 mA，范围为 4~20mA
 */
aw_err_t awbl_tps08u_chan_data_get (int id, uint8_t chan, uint32_t *p_chan_data);

/**
 * \brief 获取 TPS08U 状态
 *
 * \param[in]  id      设备号
 * \param[out] p_state 指向存储获取到的 TPS08U 状态的指针，每一位指示一种状态，
 *                     参考 AWBL_TPS08U_STATE_*即可，如
 *                     AWBL_TPS08U_STATE_DATA_UPDATE
 *                     表示通道数据已更新
 *
 * \retval  AW_OK 获取成功
 * \retval   <0   获取失败
 */
aw_err_t awbl_tps08u_state_get (int id, uint8_t *p_state);

/**
 * \brief 复位 TPS08U
 *
 * \param[in] id 设备号
 *
 * \retval  AW_OK 复位成功
 * \retval   <0   复位失败
 */
aw_err_t awbl_tps08u_reset (int id);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_TPS08U_H */

/* end of file */
