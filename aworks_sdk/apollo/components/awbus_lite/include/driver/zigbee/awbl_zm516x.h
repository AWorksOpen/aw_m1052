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
 * \brief zigbee 模块 ZM516X 驱动
 *
 * 本驱动为 ZM516X 模块的操作接口库，驱动的底层使用 UART 标准接口来操作 ZM516X 
 * 模块
 *
 * \internal
 * \par modification history
 * - 1.02 18-03-30  pea, optimize API
 * - 1.01 18-01-22  pea, update API to v1.03 manual
 * - 1.00 14-11-05  afm, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZM516X_H
#define __AWBL_ZM516X_H

#include "awbus_lite.h"
#include "aw_types.h"
#include "aw_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief ZM516X 设备名称 */
#define    AWBL_ZM516X_DRV_NAME    "zm516x"

/** \brief ZM516X 设备信息 */
struct awbl_zm516x_devinfo {
    int       com_id;               /**< \brief COM ID */
    uint32_t  baudrate;             /**< \brief 串口波特率 */
    uint32_t  serial_opts;          /**< \brief 串口通讯参数，如 (CREAD | CS8) */
    int       rst_pin;              /**< \brief 复位引脚 */
    int       ack_pin;              /**< \brief ACK 引脚 */
    uint16_t  ack_timeout;          /**< \brief ACK 超时时间 */
    uint8_t  *p_txbuf;              /**< \brief 发送缓冲区 */
    uint32_t  txbuf_size;           /**< \brief 发送缓冲区大小（字节数） */
    uint8_t  *p_rxbuf;              /**< \brief 接收缓冲区 */
    uint32_t  rxbuf_size;           /**< \brief 接收缓冲区大小（字节数） */
    void    (*pfn_plfm_init)(void); /**< \brief 平台初始化函数 */
};

/** \brief ZM516X 设备实例 */
struct awbl_zm516x_dev {
    struct awbl_dev dev;       /**< \brief AWBus 设备实例结构体 */
    AW_SEMB_DECL(ack_wait);    /**< \brief ACK 等待信号量 */
};

#define AWBL_ZM516X_BAUDRATE_2400   1    /**< \brief 串口波特率 2400 */
#define AWBL_ZM516X_BAUDRATE_4800   2    /**< \brief 串口波特率 4800 */
#define AWBL_ZM516X_BAUDRATE_9600   3    /**< \brief 串口波特率 9600 */
#define AWBL_ZM516X_BAUDRATE_19200  4    /**< \brief 串口波特率 19200 */
#define AWBL_ZM516X_BAUDRATE_38400  5    /**< \brief 串口波特率 38400 */
#define AWBL_ZM516X_BAUDRATE_57600  6    /**< \brief 串口波特率 57600 */
#define AWBL_ZM516X_BAUDRATE_115200 7    /**< \brief 串口波特率 115200 */

#define AWBL_ZM516X_PARITY_NONE    0     /**< \brief 串口无校验位 */
#define AWBL_ZM516X_PARITY_ODD     1     /**< \brief 串口奇校验 */
#define AWBL_ZM516X_PARITY_EVEN    2     /**< \brief 串口偶校验 */

#define AWBL_ZM516X_ADC_CHANNEL0    0    /**< \brief ADC 通道 0 */
#define AWBL_ZM516X_ADC_CHANNEL1    1    /**< \brief ADC 通道 1 */
#define AWBL_ZM516X_ADC_CHANNEL2    2    /**< \brief ADC 通道 2 */
#define AWBL_ZM516X_ADC_CHANNEL3    3    /**< \brief ADC 通道 3 */

#define AWBL_ZM516X_GPIO_INPUT1     (0 << 0) /**< \brief GPIO1 输入 */
#define AWBL_ZM516X_GPIO_INPUT2     (0 << 1) /**< \brief GPIO2 输入 */
#define AWBL_ZM516X_GPIO_INPUT3     (0 << 2) /**< \brief GPIO3 输入 */
#define AWBL_ZM516X_GPIO_INPUT4     (0 << 3) /**< \brief GPIO4 输入 */

#define AWBL_ZM516X_GPIO_OUTPUT1    (1 << 0) /**< \brief GPIO1 输出 */
#define AWBL_ZM516X_GPIO_OUTPUT2    (1 << 1) /**< \brief GPIO2 输出 */
#define AWBL_ZM516X_GPIO_OUTPUT3    (1 << 2) /**< \brief GPIO3 输出 */
#define AWBL_ZM516X_GPIO_OUTPUT4    (1 << 3) /**< \brief GPIO4 输出 */

#define AWBL_ZM516X_GPIO_LOW1       (0 << 0) /**< \brief GPIO1 输出低电平 */
#define AWBL_ZM516X_GPIO_LOW2       (0 << 1) /**< \brief GPIO2 输出低电平 */
#define AWBL_ZM516X_GPIO_LOW3       (0 << 2) /**< \brief GPIO3 输出低电平 */
#define AWBL_ZM516X_GPIO_LOW4       (0 << 3) /**< \brief GPIO4 输出低电平 */

#define AWBL_ZM516X_GPIO_HIGH1      (1 << 0) /**< \brief GPIO1 输出高电平 */
#define AWBL_ZM516X_GPIO_HIGH2      (1 << 1) /**< \brief GPIO2 输出高电平 */
#define AWBL_ZM516X_GPIO_HIGH3      (1 << 2) /**< \brief GPIO3 输出高电平 */
#define AWBL_ZM516X_GPIO_HIGH4      (1 << 3) /**< \brief GPIO4 输出高电平 */

#define AWBL_ZM516X_EXTI_FALL1      (0 << 0) /**< \brief IO1 下降沿触发 */
#define AWBL_ZM516X_EXTI_FALL2      (0 << 1) /**< \brief IO2 下降沿触发 */
#define AWBL_ZM516X_EXTI_FALL3      (0 << 2) /**< \brief IO3 下降沿触发 */
#define AWBL_ZM516X_EXTI_FALL4      (0 << 3) /**< \brief IO4 下降沿触发 */

#define AWBL_ZM516X_EXTI_RISE1      (1 << 0) /**< \brief IO1 上升沿触发 */
#define AWBL_ZM516X_EXTI_RISE2      (1 << 1) /**< \brief IO2 上升沿触发 */
#define AWBL_ZM516X_EXTI_RISE3      (1 << 2) /**< \brief IO3 上升沿触发 */
#define AWBL_ZM516X_EXTI_RISE4      (1 << 3) /**< \brief IO4 上升沿触发 */

#define AWBL_ZM516X_SEND_MODE_UNICAST    0    /**< \brief 单播 */
#define AWBL_ZM516X_SEND_MODE_BROADCAST  1    /**< \brief 广播 */

#define AWBL_ZM516X_NETWORK_TYPE_MASTER  0    /**< \brief 主机节点 */
#define AWBL_ZM516X_NETWORK_TYPE_SLAVE   1    /**< \brief 从机结点 */

#define AWBL_ZM516X_NETWORK_STATE_MASTER_IDLE    0  /**< \brief 主机空闲 */
#define AWBL_ZM516X_NETWORK_STATE_MASTER_DETECT  1  /**< \brief 主机正在检测网络 */
#define AWBL_ZM516X_NETWORK_STATE_MASTER_JOIN    2  /**< \brief 主机允许从机加入网络 */

#define AWBL_ZM516X_NETWORK_STATE_SLAVE_JOIN     0   /**< \brief 从机正在加入网络 */
#define AWBL_ZM516X_NETWORK_STATE_SLAVE_ONLINE   1   /**< \brief 从机已加入网络 */
#define AWBL_ZM516X_NETWORK_STATE_SLAVE_OFFLINE  2   /**< \brief 从机已退出网络 */

#define AWBL_ZM516X_SOFTWARE_FASTZIGBEE  1  /**< \brief 固件类型为 Fastzigbee */
#define AWBL_ZM516X_SOFTWARE_ZLGNET      2  /**< \brief 固件类型为 zlgnet */
#define AWBL_ZM516X_SOFTWARE_AW          3  /**< \brief 固件类型为 aw 系列 */

/** \brief ZM516X 模块的配置信息 */
typedef struct awbl_zm516x_cfg_info {
    char     dev_name[16];     /**< \brief 设备名称 */
    char     dev_pwd[16];      /**< \brief 设备密码 */
    uint8_t  dev_mode;         /**< \brief 设备类型，0:终端设备 1:路由设备 */
    uint8_t  chan;             /**< \brief 通道号 */
    uint16_t panid;            /**< \brief 网络 ID (PanID) */
    uint16_t my_addr;          /**< \brief 本地网络地址 */
    uint8_t  my_mac[8];        /**< \brief 本地物理地址(MAC) */
    uint16_t dst_addr;         /**< \brief 目标网络地址 */
    uint8_t  dst_mac[8];       /**< \brief 目标物理地址(保留) */

    /**
     * \brief 发射功率，值为 0~3，分别对应发射功率：
     *        -32dBm、-20.5dBm、-9dBm、+2.5dBm
     */
    uint8_t  power_level;
    uint8_t  retry_num;        /**< \brief 发送数据重试次数 */
    uint8_t  tran_timeout;     /**< \brief 发送数据重试时间间隔(10ms) */

    /**
     * \brief 串口波特率，值为 1~7，分别对应波特率：
     *        2400、4800、9600、19200、38400、57600、115200
     */
    uint8_t  serial_rate;
    uint8_t  serial_data;    /**< \brief 串口数据位，值为 5~8 */
    uint8_t  serial_stop;    /**< \brief 串口停止位，值为 1~2 */
    uint8_t  serial_parity;  /**< \brief 串口校验位 0:无校验 1:奇校验 2:偶校验 */
    uint8_t  send_mode;      /**< \brief 发送模式 0:单播  1:广播 */

    uint8_t  state;        /**< \brief 运行状态 */
    uint16_t software;     /**< \brief 固件类型 1:fastzigbee 2:zlgnet 3:aw系列 */
    uint16_t version;      /**< \brief 固件版本 */
} awbl_zm516x_cfg_info_t;

/** \brief ZM516X 模块的基本信息(通过搜索命令获得) */
typedef struct awbl_zm516x_base_info {
    uint16_t software;      /**< \brief 固件类型 1:fastzigbee 2:zlgnet 3:aw系列 */
    uint8_t  chan;          /**< \brief 通道号 */
    uint8_t  data_rate;     /**< \brief RF 通讯速率  */
    uint16_t panid;         /**< \brief 网络 ID (PanID) */
    uint16_t addr;          /**< \brief 网络地址 */
    uint8_t  state;         /**< \brief 运行状态 */
} awbl_zm516x_base_info_t;

/** \brief ZM516X 从机模块信息(通过查询主机模块存储的从机信息命令获得) */
typedef struct awbl_zm516x_slave_info {
    uint8_t  mac[8];             /**< \brief MAC 地址 */
    uint16_t addr;               /**< \brief 网络地址 */
} awbl_zm516x_slave_info_t;

/**
 * \brief ZM516X 模块的 4 通道 PWM 信息
 */
typedef struct awbl_zm516x_pwm {

    /**
     * \brief PWM 频率，单位为 Hz，通道 1、2、4 的频率范围可设定从 62Hz~800000Hz，
     *        通道 3 的频率范围可设定从 62Hz~60000Hz。当 PWM 信号的频率为 0 时，
     *        表示不输出 PWM 信号
     */
    uint32_t freq[4];

    /**
     * \brief PWM 占空比，占空比的值可设定从 1~99，表示占空比的百分比
     */
    uint8_t  duty_cycle[4];
} awbl_zm516x_pwm_t;

/**
 * \brief 将 ZM516X 驱动注册到 AWBus 子系统中
 *
 * \note 本函数中禁止调用操作系统 API
 */
void awbl_zm516x_drv_register (void);

/**
 * \brief 复位指定地址的 ZM516X 模块
 *
 * \param[in] id       设备号
 * \param[in] dst_addr 目标地址
 *
 * \retval AW_OK 复位成功
 * \retval  <0   复位失败
 *
 * \note 复位远端模块时，返回 AW_OK 只代表复位命令已发送，不代表复位成功
 */
aw_err_t awbl_zm516x_reset (int id, uint16_t dst_addr);

/**
 * \brief 恢复指定地址的 ZM516X 模块为出厂设置
 *
 * \param[in] id       设备号
 * \param[in] dst_addr 目标地址
 *
 * \retval  AW_OK 设置成功
 * \retval -EPERM 设置失败
 */
aw_err_t awbl_zm516x_default_set (int id, uint16_t dst_addr);

/**
 * \brief 获取指定地址的 ZM516X 模块的配置信息
 *
 * \param[in]  id         设备号
 * \param[in]  is_local   是否是获取本地模块的配置信息
 * \param[in]  dst_addr   如果需要获取远端模块配置信息，需要设置目标地址，否则忽
 *                        略此参数
 * \param[out] p_cfg_info 指向存储获取到的 ZM516X 模块的配置信息的指针
 *
 * \retval  AW_OK  获取成功
 * \retval -EPERM  获取失败
 * \retval -EINVAL 无效参数
 *
 * \attention 串口数据位如果设置为 5、6、7 位，则不可以获取配置信息
 */
aw_err_t awbl_zm516x_cfg_info_get (int                     id,
                                   aw_bool_t                  is_local,
                                   uint16_t                dst_addr,
                                   awbl_zm516x_cfg_info_t *p_cfg_info);

/**
 * \brief 修改指定地址的 ZM516X 模块的配置信息
 *
 * \param[in] id         设备号
 * \param[in] dst_addr   目标地址
 * \param[in] p_cfg_info 指向 ZM516X 模块配置信息的指针
 *
 * \retval  AW_OK 修改成功
 * \retval -EPERM 修改失败
 *
 * \attention ZM516X 模块只有在串口数据位为 8 位，校验为无校验，停止位为 1 位时
 *            才能修改配置信息，在更改模块的数据位，校验位，停止位时需要注意，
 *            如果不小心改错了，需要把模块恢复出厂默认才能操作
 */
aw_err_t awbl_zm516x_cfg_info_set (int                     id,
                                   uint16_t                dst_addr,
                                   awbl_zm516x_cfg_info_t *p_cfg_info);

/**
 * \brief 设置 ZM516X 模块通道号
 *
 * 模块可工作在 16 个物理通道上，根据模块的载波频率不同，不同通道的模块彼此间物
 * 理通道不通，因此在设计上可以实现物理上划分网段的效果
 *
 * \param[in] id      设备号
 * \param[in] chan    通道号 11~26
 * \param[in] is_temp 设置是否掉电丢失
 *
 * \retval  AW_OK 设置成功
 * \retval -EPERM 设置失败
 */
aw_err_t awbl_zm516x_chan_set (int id, uint8_t chan, aw_bool_t is_temp);

/**
 * \brief 设置 ZM516X 模块接收的数据包包头是否显示源地址
 *
 * \param[in] id      设备号
 * \param[in] is_show 包头是否显示源地址
 * \param[in] is_temp 设置是否掉电丢失
 *
 * \retval  AW_OK 设置成功
 * \retval -EPERM 设置失败
 */
aw_err_t awbl_zm516x_display_head_set (int       id, 
                                       aw_bool_t is_show, 
                                       aw_bool_t is_temp);

/**
 * \brief 设置 ZM516X 模块进入深度休眠模式
 *
 * \param[in] id 设备号
 *
 * \return 无
 *
 * \note 进入休眠后不保存临时参数配置，可通过复位模块或拉低 WAKE 管脚进行唤醒，
 *       模块被唤醒后，所有管脚将恢复到初始状态
 */
aw_err_t awbl_zm516x_enter_deep_sleep (int id);

/**
 * \brief 设置 ZM516X 模块的发送模式
 *
 * \param[in] id        设备号
 * \param[in] send_mode 模块的发送模式，参见 AWBL_ZM516X_SEND_MODE_*
 *
 * \retval  AW_OK 设置成功
 * \retval -EPERM 设置失败
 */
aw_err_t awbl_zm516x_send_mode_temp_set (int id, uint8_t send_mode);

/**
 * \brief 设置 ZM516X 模块目标地址
 *
 * \param[in] id       设备号
 * \param[in] dst_addr 要设置的目标地址
 *
 * \retval  AW_OK 设置成功
 * \retval -EPERM 设置失败
 */
aw_err_t awbl_zm516x_dst_addr_temp_set (int id, uint16_t dst_addr);

/**
 * \brief 读取指定地址的 ZM516X 模块的信号强度
 *
 * \param[in]  id       设备号
 * \param[in]  dst_addr 要读取的 ZM516X 模块的网络地址
 * \param[out] p_signal 指向保存读取到的信号强度的指针
 *
 * \retval  AW_OK 读取成功
 * \retval -EPERM 读取失败
 */
aw_err_t awbl_zm516x_sigal_get (int id, uint16_t dst_addr, uint8_t *p_signal);

/**
 * \brief 搜索其它模块
 *
 * 模块接收到本命令后，会向本网段内的相同通道的其他模块发出广播搜索包，运行本公
 * 司固件的 ZM516X 模块都会应答此广播，应答内容会将自己的相关基本信息返回到搜索
 * 发起目标节点。所以搜索前需要设置通道号才能搜索到对应通道的模块
 *
 * \param[in]  id          设备号
 * \param[in]  buf_size    p_base_info 指向的缓冲区大小，单位为
 *                         sizeof(awbl_zm516x_base_info_t)
 * \param[out] p_base_info 指向存储接收到的从机信息的指针
 * \param[out] p_get_size  指向存储实际搜索到的模块数量的指针
 *
 * \retval  AW_OK  读取成功
 * \retval -EPERM  读取失败
 * \retval -EINVAL 无效参数
 */
aw_err_t awbl_zm516x_discover (int                      id,
                               uint8_t                  buf_size,
                               awbl_zm516x_base_info_t *p_base_info,
                               uint8_t                 *p_get_size);

/**
 * \brief 获取指定地址的 ZM516X 模块的 GPIO 输入输出方向
 *
 * \param[in]  id       设备号
 * \param[in]  dst_addr 要获取的 ZM516X 模块的网络地址
 * \param[out] p_dir    获取到的管脚方向，参见 AWBL_ZM516X_GPIO_INPUT* 或
 *                      AWBL_ZM516X_GPIO_OUTPUT*
 *
 * \retval  AW_OK  获取成功
 * \retval -EPERM  获取失败
 * \retval -EINVAL 无效参数
 */
aw_err_t awbl_zm516x_gpio_dir_get (int id, uint16_t dst_addr, uint8_t *p_dir);

/**
 * \brief 设置指定地址的 ZM516X 模块的 GPIO 输入输出方向
 *
 * \param[in] id       设备号
 * \param[in] dst_addr 要设置的 ZM516X 模块的网络地址
 * \param[in] dir      管脚方向，需要使用或运算一次性传入所有管脚的配置，默认为
 *                     输入，参见AWBL_ZM516X_GPIO_INPUT* 或 
 *                     AWBL_ZM516X_GPIO_OUTPUT*，如 AWBL_ZM516X_GPIO_INPUT1 | 
 *                     AWBL_ZM516X_GPIO_OUTPUT2，将配置 IO1、IO3、IO4 为输入，
 *                     IO2 为输出
 *                     
 * \param[in] is_temp  设置是否掉电丢失
 *
 * \retval  AW_OK 设置成功
 * \retval -EPERM 设置失败
 */
aw_err_t awbl_zm516x_gpio_dir_set (int         id,
                                   uint16_t    dst_addr,
                                   uint8_t     dir,
                                   aw_bool_t   is_temp);

/**
 * \brief 读取指定地址的 ZM516X 模块的 GPIO 输入值
 *
 * \param[in]  id       设备号
 * \param[in]  dst_addr 要读取的 ZM516X 模块的网络地址
 * \param[out] p_value  读取到的管脚输入值，参见 AWBL_ZM516X_GPIO_LOW* 或
 *                      AWBL_ZM516X_GPIO_HIGH*
 *
 * \retval  AW_OK  读取成功
 * \retval -EPERM  读取失败
 * \retval -EINVAL 无效参数
 */
aw_err_t awbl_zm516x_gpio_get (int id, uint16_t dst_addr, uint8_t *p_value);

/**
 * \brief 设置指定地址的 ZM516X 模块的 GPIO 输出值
 *
 * \param[in] id       设备号
 * \param[in] dst_addr 要设置的 ZM516X 模块的网络地址
 * \param[in] value    管脚输出值，需要使用或运算一次性传入所有管脚的输出值，
 *                     默认输出低电平，参见AWBL_ZM516X_GPIO_LOW* 或 
 *                     AWBL_ZM516X_GPIO_HIGH*，
 *                     如 AWBL_ZM516X_GPIO_LOW1 | AWBL_ZM516X_GPIO_HIGH2，将
 *                     配置 IO1、IO3、IO4 输出低电平，IO2 为输出高电平
 * \param[in] is_temp  设置是否掉电丢失
 *
 * \retval  AW_OK 设置成功
 * \retval -EPERM 设置失败
 */
aw_err_t awbl_zm516x_gpio_set (int      id,
                               uint16_t dst_addr,
                               uint8_t  value,
                               aw_bool_t   is_temp);

/**
 * \brief 读取指定地址的 ZM516X 模块的 AD 转换值
 *
 * 读取到的 AD 值为 AD 的转换值，需要参考 ZM516X 模块用户手册
 * 根据 ZM516X 模块 AD 的位数及参考电压转换为电压值
 *
 * \param[in]  id       设备号
 * \param[in]  dst_addr 要读取的 ZM516X 模块的网络地址
 * \param[in]  chan     AD 的通道号，参见 AWBL_ZM516X_ADC_CHANNEL*
 * \param[out] p_value  AD 的转换值
 *
 * \retval  AW_OK  读取成功
 * \retval -EPERM  读取失败
 * \retval -EINVAL 无效参数
 */
aw_err_t awbl_zm516x_adc_get (int       id,
                              uint16_t  dst_addr,
                              uint8_t   chan,
                              uint16_t *p_value);

/**
 * \brief 获取指定地址的 ZM516X 模块的 IO/AD 采集配置
 *
 * \param[in]  id            设备号
 * \param[in]  dst_addr      要获取的 ZM516X 模块的网络地址
 * \param[out] p_trigger     获取到的 IO 触发边沿配置，如果不需要获取，传入 NULL 
 *                           即可。，参见 AWBL_ZM516X_EXTI_FALL* 或
 *                           AWBL_ZM516X_EXTI_RISE*
 * \param[out] p_period_10ms 获取到的上传周期配置，单位为 10ms，如果不需要获取，
 *                           传入 NULL 即可
 * \param[out] p_is_dormant  获取到的是否休眠配置，如果不需要获取，传入 NULL 即
 *                           可。指示模块是否处于休眠状态，TRUE 表示模块处于休眠状
 *                           态，IO 根据配置的状态发生变化时唤醒模块，并向目标节
 *                           点地址发送 IO/AD 采集帧，如果周期大于 0，模块除了可
 *                           以 IO唤醒外，还会根据周期定时定时唤醒，然后向目标节
 *                           点地址发送IO/AD 采集帧，当发送完 IO/AD 采集帧后，模
 *                           块进入休眠状态，FALSE 表示模块不休眠
 *
 * \retval  AW_OK 获取成功
 * \retval -EPERM 获取失败
 */
aw_err_t awbl_zm516x_io_adc_upload_get (int       id,
                                        uint16_t  dst_addr,
                                        uint8_t  *p_trigger,
                                        uint16_t *p_period_10ms,
                                        aw_bool_t   *p_is_dormant);

/**
 * \brief 设置指定地址的 ZM516X 模块的 IO/AD 采集配置
 *
 * AW516x 系列 ZM516X 模块有 4 路的 IO 和 4 路的 AD，IO/AD 采集设置命令可设定模块
 * IO 触发上传 IO 和 AD 状态及根据配置的周期时间定时上传 IO 和 AD 状态
 *
 * \param[in] id          设备号
 * \param[in] p_zb_addr   要设置的 ZM516X 模块的网络地址
 * \param[in] trigger     IO 触发边沿，参见 AWBL_ZM516X_EXTI_FALL* 或
 *                        AWBL_ZM516X_EXTI_RISE*
 * \param[in] period_10ms 上传周期，单位为 10ms，最大可设置的值为 65535，
 *                        即设置的最大周期为 655350ms
 * \param[in] is_dormant  是否休眠，指示模块是否处于休眠状态，该字节为 1 表示模
 *                        块处于休眠状态，IO 根据配置的状态发生变化时唤醒模块，
 *                        并向目标节点地址发送 IO/AD 采集帧，如果周期大于 0，模
 *                        块除了可以 IO唤醒外，还会根据周期定时定时唤醒，然后向
 *                        目标节点地址发送IO/AD 采集帧，当发送完 IO/AD 采集帧后，
 *                        模块进入休眠状态，该字节为 0 表示模块不休眠
 *
 * \retval  AW_OK 设置成功
 * \retval -EPERM 设置失败
 */
aw_err_t awbl_zm516x_io_adc_upload_set (int      id,
                                        uint16_t dst_addr,
                                        uint8_t  trigger,
                                        uint16_t period_10ms,
                                        aw_bool_t   is_dormant);

/**
 * \brief 获取指定地址的 ZM516X 模块的 PWM 输出值配置
 *
 * \param[in]  id       设备号
 * \param[in]  dst_addr 要获取的 ZM516X 模块的网络地址
 * \param[out] p_pwm    指向存储获取到的 awbl_zm516x_pwm_t 结构的指针
 *
 * \retval  AW_OK  获取成功
 * \retval -EPERM  获取失败
 * \retval -EINVAL 无效参数
 */
aw_err_t awbl_zm516x_pwm_get (int                id,
                              uint16_t           dst_addr,
                              awbl_zm516x_pwm_t *p_pwm);

/**
 * \brief 设置指定地址的 ZM516X 模块的 PWM 输出值
 *
 * \param[in] id       设备号
 * \param[in] dst_addr 要设置的 ZM516X 模块的网络地址
 * \param[in] p_pwm    指向 awbl_zm516x_pwm_t 结构的指针
 *
 * \retval  AW_OK  设置成功
 * \retval -EPERM  设置失败
 * \retval -EINVAL 无效参数
 */
aw_err_t awbl_zm516x_pwm_set (int                id,
                              uint16_t           dst_addr,
                              awbl_zm516x_pwm_t *p_pwm);

/**
 * \brief 设置自组网功能
 *
 * 自组网功能为一主多从的星形网络拓扑结构，在主机到从机信号不可达时，可加入中继
 * 节点进行信号传递。模块在自组网模式下，主机模块会自动选择周围没有被使用的 PANID 
 * 和通道号形成一个独立的网络，并能自动分配一个唯一的本地网络地址给从机模块，从
 * 机模块使用时在使能了自组网功能后就不需要进行任何的配置操作，从机模块在加入网络
 * 后就能跟主机进行通讯。一个主机模块最多可连 200 个从机模块。详细流程请参考文档
 * AW516x_ZigBee.pdf
 *
 * \param[in] id        设备号
 * \param[in] type      节点类型，参见 AWBL_ZM516X_NETWORK_TYPE_*
 * \param[in] is_enable 是否使能自组网
 *
 * \retval  AW_OK  设置成功
 * \retval -EPERM  设置失败
 * \retval -EINVAL 无效参数
 */
aw_err_t awbl_zm516x_network_config (int id, uint8_t type, aw_bool_t is_enable);

/**
 * \brief 主机允许从机加入网络
 *
 * 允许加入网络开启的窗口时间为当给主机发送了允许加入网络命令后，主机开始接受从
 * 机的入网请求，到达这个窗口时间后，主机不再接受从机的入网请求，窗口时间结束后，
 * 主机进入正常的工作状态
 *
 * \param[in] id          设备号
 * \param[in] time_second 允许加入网络开启的窗口时间，单位为秒
 *
 * \retval  AW_OK 设置成功
 * \retval -EPERM 设置失败
 */
aw_err_t awbl_zm516x_master_join_enable (int id, uint16_t time_second);

/**
 * \brief 查询主机模块存储的从机信息
 *
 * \param[in]  id           设备号
 * \param[in]  buf_size     p_slave_info 指向的缓冲区大小，单位为
 *                          sizeof(awbl_zm516x_slave_info_t)
 * \param[out] p_slave_info 指向存储接收到的从机信息的指针
 * \param[out] p_get_size   指向存储实际获取到的从机信息数量的指针
 *
 * \retval  AW_OK  查询成功
 * \retval -EPERM  查询失败
 * \retval -EINVAL 无效参数
 */
aw_err_t awbl_zm516x_master_slave_info_get (int                       id,
                                            uint8_t                   buf_size,
                                            awbl_zm516x_slave_info_t *p_slave_info,
                                            uint8_t                  *p_get_size);

/**
 * \brief 查询节点是主机还是从机
 *
 * \param[in]  id               设备号
 * \param[out] p_type           获取到的节点类型，参见 AWBL_ZM516X_NETWORK_TYPE_*，
 *                              如不需要获取该参数，传入 NULL 即可
 * \param[out] p_response_state 获取到的响应状态，当模块配置为主机时，
 *                                  参见 AWBL_ZM516X_NETWORK_STATE_MASTER_*
 *                              当模块配置为从机时，
 *                                  参见 AWBL_ZM516X_NETWORK_STATE_SLAVE_*
 *                              如不需要获取该参数，传入 NULL 即可
 *
 * \retval  AW_OK 查询成功
 * \retval -EPERM 查询失败
 */
aw_err_t awbl_zm516x_network_state_get (int      id,
                                        uint8_t *p_type,
                                        uint8_t *p_response_state);

/**
 * \brief 向 ZM516X 模块发送 nbytes 个字节数据
 *
 * 这个接口向 ZM516X 模块发送 nbytes 个字节数据。若设备的内部缓冲不够，将做如下处
 * 理：剩余的数据不会被写入，返回值为已经发送的数据个数
 *
 * \param[in] id     设备号
 * \param[in] p_buf  要写入模块的数据缓冲区指针
 * \param[in] nbytes 要写入的字节个数
 *
 * \return 成功发送的数据个数，小于 0 为失败，失败原因可查看 errno
 *
 * \attention 该发送函数为异步操作函数，ZM516X 的配置命令是根据串口字符帧间隔时
 *            间来判断配置命令的结束，如果在调用了 awbl_zm516x_send 发送函数后，
 *            要紧接着调用 ZM516X 的配置命令，需要延时等待发送缓存数据完全发送的
 *            时间加上 ZM516X 的帧间隔时间后，才能调用 ZM516X 的配置命令，否则 
 *            ZM516X的配置命令会当作是数据发到 ZM516X 网络上，延时等待时间建议是：
 *            T(s) = (1 / 波特率) * 10 * 字节数 + 0.05
 */
ssize_t awbl_zm516x_send (int id, const void *p_buf, size_t nbytes);

/**
 * \brief 向 ZM516X 模块发送 nbytes 个字节数据，并等待 ACK
 *
 * 这个接口向 ZM516X 模块发送 nbytes 个字节数据。若设备的内部缓冲不够，将做如下
 * 处理：剩余的数据不会被写入，返回值为已经发送的数据个数
 *
 * \param[in] id     设备号
 * \param[in] p_buf  要写入模块的数据缓冲区指针
 * \param[in] nbytes 要写入的字节个数
 *
 * \retval  > 0       成功发送的数据个数
 * \retval -ENOTSUP   没有配置 ACK 引脚
 * \retval -ETIMEDOUT 发送无应答
 *
 * \attention 1. 该发送函数为同步操作函数，发送数据后将等待 ACK，当收到 ACK 或者
 *               超时时才会返回；
 *            2. 发送模式为广播模式时不会产生 ACK。
 */
ssize_t awbl_zm516x_send_with_ack (int id, const void *p_buf, size_t nbytes);

/**
 * \brief 从 ZM516X 模块接收 maxbytes 个字节数据
 *
 * - 这个接口从 ZM516X 模块接收最多 maxbytes 个字节数据
 * - 若设备可接收数据个数为 0，将立即返回
 *
 * \param[in]  id       设备号
 * \param[out] p_buf    存放读取数据的缓冲区指针
 * \param[in]  maxbytes 最多要读取的字节个数
 *
 * \return 成功接收的数据个数，小于 0 为失败，失败原因可查看 errno
 */
aw_err_t awbl_zm516x_receive (int id, void *p_buf, size_t maxbytes);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_ZM516X_H */

/* end of file */
