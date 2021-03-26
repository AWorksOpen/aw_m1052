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
 * \brief Generic SX1276/77/78/79 driver implementation
 * \note SX1276/77/78/79 可共用一套驱动
 *
 * \internal
 * \par Modification history
 * - 1.0.0 17-09-25  sky, first implementation.
 * \endinternal
 */

#ifndef __AM_SX127X_H
#define __AM_SX127X_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lpsoftimer.h"
#include "am_sx127x_regs_fsk.h"
#include "am_sx127x_regs_lora.h"

/**
 * @addtogroup am_if_sx1276
 * @copydoc am_sx1276.h
 * @{
 */

/* #define LORA_ENABLE_FSK */

/**
 * \brief 射频驱动支持的调制方式
 */
typedef enum {
    MODEM_FSK = 0,
    MODEM_LORA,
} am_lora_radio_modems_t;

/**
 * \brief 射频驱动内部状态机状态定义
 */
typedef enum {
    LORA_RF_IDLE = 0,
    LORA_RF_RX_RUNNING,
    LORA_RF_TX_RUNNING,
    LORA_RF_CAD,
} am_lora_radio_state_t;

/**
 * \brief 射频睡眠模式的唤醒时间
 */
#define AM_LORA_RADIO_OSC_STARTUP           1 /* [ms] */

/**
 * \brief 射频PLL锁及模式准备时间(可能会随温度变化)
 */
#define AM_LORA_RADIO_SLEEP_TO_RX           2 /* [ms] */

/**
 * \brief 射频完成唤醒时间带温度补偿临界值
 */
#define AM_LORA_RADIO_WAKEUP_TIME           (AM_LORA_RADIO_OSC_STARTUP + \
                                             AM_LORA_RADIO_SLEEP_TO_RX)

/**
 * \brief LoRa私网同步字
 */
#define AM_LORA_MAC_PRIVATE_SYNCWORD        0x12

/**
 * \brief LoRa公网同步字
 */
#define AM_LORA_MAC_PUBLIC_SYNCWORD         0x34

/**
 * \brief SX1276 固有定义
 */
#define AM_LORA_XTAL_FREQ           32000000
#define AM_LORA_FREQ_STEP           ((int64_t)61035156250)
#define AM_LORA_RX_BUFFER_SIZE      256

typedef void (*am_sx127x_pfnvoid_t) (void *);

/**
 * \brief 射频FSK调制参数
 */
typedef struct {
    int8_t   power;
    uint32_t freq_dev;
    uint32_t bandwidth;
    uint32_t bandwidth_afc;
    uint32_t datarate;
    uint16_t preamble_len;
    uint8_t  fix_len;
    uint8_t  payload_len;
    uint8_t  crc_on;
    uint8_t  iq_inverted;
    uint8_t  rx_continuous;
    uint32_t tx_timeout;
} am_lora_radio_fsk_settings_t;

/**
 * \brief 射频FSK包处理结构
 */
typedef struct {
    uint8_t  preamble_detected;
    uint8_t  sync_word_detected;
    int8_t   rssi_value;
    int32_t  afc_value;
    uint8_t  rx_gain;
    uint16_t size;
    uint16_t nb_bytes;
    uint8_t  fifo_thresh;
    uint8_t  chunk_size;
} am_lora_radio_fsk_packet_handler_t;

/**
 * \brief 射频LoRa调制参数
 */
typedef struct {
    int8_t   power;
    uint32_t bandwidth;
    uint32_t datarate;
    uint8_t  low_datarate_optimize;
    uint8_t  coderate;
    uint16_t preamble_len;
    uint8_t  fix_len;
    uint8_t  payload_len;
    uint8_t  crc_on;
    uint8_t  freq_hop_on;
    uint8_t  hop_period;
    uint8_t  iq_inverted;
    uint8_t  rx_continuous;
    uint32_t tx_timeout;
} am_lora_radio_lora_settings_t;

/**
 * \brief 射频LoRa包处理结构
 */
typedef struct {
    int8_t  snr_value;
    int16_t rssi_value;
    uint8_t size;
} am_lora_radio_lora_packet_handler_t;

/**
 * \brief 射频设置
 */
typedef struct {
    am_lora_radio_state_t               state;
    am_lora_radio_modems_t              modem;
    uint32_t                            channel;
    am_lora_radio_fsk_settings_t        fsk;
    am_lora_radio_fsk_packet_handler_t  fsk_packet_handler;
    am_lora_radio_lora_settings_t       lora;
    am_lora_radio_lora_packet_handler_t lora_packet_handler;
} am_lora_radio_settings_t;

/**
 * \brief 射频驱动回调函数
 */
typedef struct {

    /**
     * \brief 传输完成回调函数原型
     */
    void (*pfn_tx_done_cb)(void);

    /**
     * \brief 传输超时回调函数原型
     */
    void (*pfn_tx_timeout_cb)(void);

    /**
     * \brief 接收完成回调函数原型
     *
     * \param[in] p_payload : 指向接收缓冲区
     * \param[in] size      : 接收到的数据大小
     * \param[in] rssi      : 接收该帧时的计算得的 RSSI 值 [dBm]
     * \param[in] snr       : 二进制补码换算得的信噪比
     *                         FSK : N/A (set to 0)
     *                         LoRa: SNR value in dB
     */
    void (*pfn_rx_done_cb)(uint8_t *p_payload, uint16_t size, int16_t rssi, int8_t snr);

    /**
     * \brief 接收超时回调函数原型
     */
    void (*pfn_rx_timeout_cb)(void);

    /**
     * \brief 接收错误回调函数原型
     */
    void (*pfn_rx_error_cb)(void);

    /**
     * \brief FHSS跳频回调函数原型
     *
     * \param[in] current_channel : 当前信道的编号
     */
    void (*pfn_fhss_change_channel_cb)(uint8_t current_channel);

    /**
     * \brief 信道活跃检测完成回调函数原型
     *
     * \param[in] channel_activity_detected : 是否活跃
     */
    void (*pfn_cad_done_cb)(uint8_t channel_activity_detected);

    /**
     * \brief 有效报头回调函数原型
     */
    void (*pfn_valid_header_cb)(void);

} am_lora_radio_events_t;

/** \brief LORA SX1276 驱动函数前向声明 */
typedef struct am_sx127x_drv_funcs am_sx127x_drv_funcs_t;

/**
 * \brief LORA SX1276 设备结构体
 */
typedef struct am_sx127x_dev {

    am_sx127x_drv_funcs_t   *p_drv_funcs;      /**< \brief 驱动函数表 */
    am_lora_radio_events_t  *p_radio_events;   /**< \brief 射频事件回调 */

    am_lora_radio_settings_t settings;         /**< \brief 射频硬件全局参数 */
    uint8_t                  radio_is_active;  /**< \brief 低功耗模式下RF开关标志 */

    am_lpsoftimer_t tx_timeout_timer;          /**< \brief 发送超时定时器 */
    am_lpsoftimer_t rx_timeout_timer;          /**< \brief 接收超时定时器 */
    am_lpsoftimer_t rx_timeout_sync_word_timer;/**< \brief 同步字超时定时器 */

    /** \brief 收发缓冲区(保留5个字节校验有效帧头) */
    uint8_t rx_tx_buffer[AM_LORA_RX_BUFFER_SIZE + 5];

    /** \brief 指向收发缓冲区的实际地址 */
    uint8_t *p_rx_tx_buffer;

} am_sx127x_dev_t;

/** \brief LORA SX1276 操作句柄定义 */
typedef am_sx127x_dev_t *am_sx127x_handle_t;

/**
 * \brief LORA SX1276 驱动函数定义
 */
struct am_sx127x_drv_funcs {
    void (*pfn_reset)(am_sx127x_handle_t handle);
    int  (*pfn_buf_read)(am_sx127x_handle_t handle,
                         uint8_t            addr,
                         uint8_t           *p_buff,
                         uint32_t           len);
    int  (*pfn_buf_write)(am_sx127x_handle_t handle,
                          uint8_t            addr,
                          const uint8_t     *p_buff,
                          uint32_t           len);
};

/**
 * 公共函数原型
 */

/**
 * \brief 初始化SX1276
 *
 * \param[in] p_dev       : 指向 LORA SX1276 设备结构体的指针
 * \param[in] p_drv_funcs : 指向 LORA SX1276 驱动函数列表
 *
 * \return LORA SX1276 服务操作句柄,如果为 NULL，表明初始化失败
 */
am_sx127x_handle_t am_sx127x_init (am_sx127x_dev_t       *p_dev,
                                   am_sx127x_drv_funcs_t *p_drv_funcs);

/**
 * \brief 去SX1276初始化
 *
 * \param[in] p_dev : 指向 LORA SX1276 设备结构体的指针
 *
 * \return 无
 */
void am_sx127x_deinit (am_sx127x_dev_t *p_dev);

/**
 * \brief SX127x 复位操作
 * \param[in] p_dev : 指向 LORA SX1276 设备结构体的指针
 * \return 无
 */
void am_sx127x_reset (am_sx127x_dev_t *p_dev);

/**
 * \brief DIO0 中断回调
 *
 * \param[in] handle : SX1276 服务操作句柄
 *
 * \return 无
 */
void am_sx127x_dio0_isr (am_sx127x_handle_t handle);

/**
 * \brief DIO1 中断回调
 *
 * \param[in] handle : SX1276 服务操作句柄
 *
 * \return 无
 */
void am_sx127x_dio1_isr (am_sx127x_handle_t handle);

/**
 * \brief DIO2 中断回调
 *
 * \param[in] handle : SX1276 服务操作句柄
 *
 * \return 无
 */
void am_sx127x_dio2_isr (am_sx127x_handle_t handle);

/**
 * \brief DIO3 中断回调
 *
 * \param[in] handle : SX1276 服务操作句柄
 *
 * \return 无
 */
void am_sx127x_dio3_isr (am_sx127x_handle_t handle);

/**
 * \brief DIO4 中断回调
 *
 * \param[in] handle : SX1276 服务操作句柄
 *
 * \return 无
 */
void am_sx127x_dio4_isr (am_sx127x_handle_t handle);

/**
 * \brief DIO5 中断回调
 * \param[in] handle : SX1276 服务操作句柄
 * \return 无
 */
void am_sx127x_dio5_isr (am_sx127x_handle_t handle);

/**
 * \brief 设置射频事件回调
 *
 * \param[in] handle   : SX1276 服务操作句柄
 * \param[in] p_events : 事件回调函数结构
 *
 * \return 无
 */
void am_sx127x_events_set (am_sx127x_handle_t      handle,
                           am_lora_radio_events_t *p_events);

/**
 * \brief 返回设备当前状态
 * \param[in] handle : SX1276 服务操作句柄
 * \return 当前状态 [RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
 */
am_lora_radio_state_t am_sx127x_status_get (am_sx127x_handle_t handle);

/**
 * \brief 射频模式设置
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] modem  : 使用的模式 [0: FSK, 1: LoRa]
 *
 * \return 无
 */
void am_sx127x_modem_set (am_sx127x_handle_t     handle,
                          am_lora_radio_modems_t modem);

/**
 * \brief 设置信道频率
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] freq : 信道频率
 *
 * \return 无
 */
void am_sx127x_channel_set (am_sx127x_handle_t handle, uint32_t freq);

/**
 * \brief 获取信道频率
 *
 * \param[in] handle : SX1276 服务操作句柄
 *
 * \return 信道频率
 */
uint32_t am_sx127x_channel_get (am_sx127x_handle_t handle);

/**
 * \brief 查询信道是否空闲
 *
 * \param[in] handle      : SX1276 服务操作句柄
 * \param[in] modem       : 使用的射频模式 [0: FSK, 1: LoRa]
 * \param[in] freq        : 信道频率
 * \param[in] rssi_thresh : RSSI 临界值
 *
 * \retval isFree [true: Channel is free, false: Channel is not free]
 */
uint8_t am_sx127x_is_channel_free (am_sx127x_handle_t     handle,
                                   am_lora_radio_modems_t modem,
                                   uint32_t               freq,
                                   int16_t                rssi_thresh);

/**
 * \brief 基于RSSI生成32位随机数
 *
 * \param[in] handle : SX1276 服务操作句柄
 *
 * \remark 该函数使用LoRa调制模式并禁能所有中断，调用该函数后必须调用
 *          am_lora_am_sx127x_rx_config_set 或 am_lora_am_sx127x_tx_config_set。
 *
 * \retval 32位随机数的值
 */
uint32_t am_sx127x_random (am_sx127x_handle_t handle);

/**
 * \brief 接收参数设置
 *
 * \remark 使用LoRa调制模式时仅支持 125、250及500kHz
 *
 * \param[in] handle        : SX1276 服务操作句柄
 * \param[in] modem         : 使用的射频模式 [0: FSK, 1: LoRa]
 * \param[in] bandwidth     : 设置带宽
 *                             FSK : >= 2600 and <= 250000 Hz
 *                             LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                    2: 500 kHz, 3: Reserved]
 * \param[in] datarate      : 设置数据速率
 *                             FSK : 600..300000 bits/s
 *                             LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                   10: 1024, 11: 2048, 12: 4096  chips]
 * \param[in] coderate      : 设置编码率 (LoRa only)
 *                             FSK : N/A (set to 0)
 *                             LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param[in] bandwidth_afc : 设置 AFC 带宽 (FSK only)
 *                             FSK : >= 2600 and <= 250000 Hz
 *                             LoRa: N/A (set to 0)
 * \param[in] preamble_len  : 设置前导码长度
 *                             FSK : Number of bytes
 *                             LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param[in] symb_timeout  : 设置单次接收超时值 (LoRa only)
 *                             FSK : N/A (set to 0)
 *                             LoRa: timeout in symbols
 * \param[in] fix_len       : 固有长度使能 [0: variable, 1: fixed]
 * \param[in] payload_len   : 设置负载长度 (当固有长度使能时)
 * \param[in] crc_on        : CRC校验使能 [0: OFF, 1: ON]
 * \param[in] freq_hop_on   : 内部包跳频使能
 *                             FSK : N/A (set to 0)
 *                             LoRa: [0: OFF, 1: ON]
 * \param[in] hop_period    : 每次跳频的符号周期
 *                             FSK : N/A (set to 0)
 *                             LoRa: Number of symbols
 * \param[in] iq_inverted   : 转变 IQ 信号 (LoRa only)
 *                             FSK : N/A (set to 0)
 *                             LoRa: [0: not inverted, 1: inverted]
 * \param[in] rx_continuous : 设置连续接收模式
 *                             [false: single mode, true: continuous mode]
 */
void am_sx127x_rx_config_set (am_sx127x_handle_t     handle,
                              am_lora_radio_modems_t modem, 
                              uint32_t               bandwidth,
                              uint32_t               datarate,      
                              uint8_t                coderate,
                              uint32_t               bandwidth_afc, 
                              uint16_t               preamble_len,
                              uint16_t               symb_timeout,  
                              uint8_t                fix_len,
                              uint8_t                payload_len,
                              uint8_t                crc_on,        
                              uint8_t                freq_hop_on, 
                              uint8_t                hop_period,
                              uint8_t                iq_inverted,   
                              uint8_t                rx_continuous);

/**
 * \brief 传输参数设置
 *
 * \remark 使用LoRa调制模式时仅支持 125、250及500kHz
 *
 * \param[in] handle       : SX1276 服务操作句柄
 * \param[in] modem        : 使用的射频模式 [0: FSK, 1: LoRa]
 * \param[in] power        : 设置功率 [dBm]
 * \param[in] freq_dev     : 设置分频器 (FSK only)
 *                            FSK : [Hz]
 *                            LoRa: 0
 * \param[in] bandwidth    : 设置带宽 (LoRa only)
 *                            FSK : 0
 *                            LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                   2: 500 kHz, 3: Reserved]
 * \param[in] datarate     : 设置数据速率
 *                            FSK : 600..300000 bits/s
 *                            LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                  10: 1024, 11: 2048, 12: 4096  chips]
 * \param[in] coderate     : 设置编码率 (LoRa only)
 *                            FSK : N/A (set to 0)
 *                            LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param[in] preamble_len : 设置前导码长度
 *                            FSK : Number of bytes
 *                            LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param[in] fix_len      : 固定包长使能 [0: variable, 1: fixed]
 * \param[in] crc_on       : CRC校验使能 [0: OFF, 1: ON]
 * \param[in] freq_hop_on  : 内部跳频使能
 *                            FSK : N/A (set to 0)
 *                            LoRa: [0: OFF, 1: ON]
 * \param[in] hop_period   : 每次跳频的符号周期
 *                            FSK : N/A (set to 0)
 *                            LoRa: Number of symbols
 * \param[in] iq_inverted  : 转变 IQ 信号 (LoRa only)
 *                            FSK : N/A (set to 0)
 *                            LoRa: [0: not inverted, 1: inverted]
 * \param[in] timeout      : 传输超时 [ms]
 */
void am_sx127x_tx_config_set (am_sx127x_handle_t     handle,
                              am_lora_radio_modems_t modem, 
                              int8_t                 power,
                              uint32_t               freq_dev,
                              uint32_t               bandwidth, 
                              uint32_t               datarate,
                              uint8_t                coderate,   
                              uint16_t               preamble_len,
                              uint8_t                fix_len,    
                              uint8_t                crc_on,      
                              uint8_t                freq_hop_on,
                              uint8_t                hop_period, 
                              uint8_t                iq_inverted, 
                              uint32_t               timeout);

/**
 * \brief 检查硬件是否支持给出的射频频率
 *
 * \param[in] handle    : SX1276 服务操作句柄
 * \param[in] frequency : 要检查的频率
 *
 * \retval [true: 支持, false: 不支持]
 */
uint8_t am_sx127x_rf_frequency_check (am_sx127x_handle_t handle, uint32_t frequency);

/**
 * \brief 计算给出负载对应的符号数
 *
 * \param[in] handle  : SX1276 服务操作句柄
 * \param[in] pkt_len : 负载包长度
 *
 * \retval 符号数
 */
uint32_t  am_sx127x_time_pkt_symbol_get(am_sx127x_handle_t  handle,
                                        uint8_t             pkt_len);

/**
 * \brief 计算给出负载的在空时间（ms）
 *
 * \Remark 只能在am_lora_am_sx127x_rx_config_set 或 
 *         am_lora_am_sx127x_tx_config_set被调用后调用一次。
 *
 * \param[in] handle  : SX1276 服务操作句柄
 * \param[in] modem   : 使用的射频模式 [0: FSK, 1: LoRa]
 * \param[in] pkt_len : 负载包长度
 *
 * \retval 在空时间（ms）
 */
uint32_t am_sx127x_time_on_air_get (am_sx127x_handle_t     handle,
                                    am_lora_radio_modems_t modem,
                                    uint8_t                pkt_len);

/**
 * \brief 发送指定长度的数据
 *
 * \param[in] handle   : SX1276 服务操作句柄
 * \param[in] p_buffer : 指向待发送的数据
 * \param[in] size     : 欲发送的数据长度
 *
 * \return 无
 */
void am_sx127x_send (am_sx127x_handle_t  handle,
                     uint8_t            *p_buffer,
                     uint8_t             size);

/**
 * \brief 进入睡眠模式
 * \param[in] handle : SX1276 服务操作句柄
 * \return 无
 */
void am_sx127x_sleep (am_sx127x_handle_t handle);

/**
 * \brief 进入待命模式
 * \param[in] handle : SX1276 服务操作句柄
 * \return 无
 */
void am_sx127x_standby (am_sx127x_handle_t handle);

/**
 * \brief 启动射频接收
 *
 * \param[in] handle  : SX1276 服务操作句柄
 * \param[in] timeout : 接收超时 [ms] [0: continuous, others: timeout]
 *
 * \return 无
 */
void am_sx127x_rx (am_sx127x_handle_t handle, uint32_t timeout);

/**
 * \brief 启动一次信道活跃检测
 * \param[in] handle : SX1276 服务操作句柄
 * \return 无
 */
void am_sx127x_cad_start (am_sx127x_handle_t handle);

/**
 * \brief 设置射频进入连续传输模式(FSK下有用)
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] freq   : 信道频率
 * \param[in] power  : 发射功率 [dBm]
 * \param[in] time   : 传输超时 [s]
 *
 * \return 无
 */
void am_sx127x_tx_continuous_wave_set (am_sx127x_handle_t handle,
                                       uint32_t           freq,
                                       int8_t             power,
                                       uint16_t           time);

/**
 * \brief 获取当前的RSSI值
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] modem  : 使用的射频模式 [0: FSK, 1: LoRa]
 *
 * \retval RSSI值 [dBm]
 */
int16_t am_sx127x_rssi_get (am_sx127x_handle_t     handle,
                            am_lora_radio_modems_t modem);

/**
 * \brief 写特定地址的射频寄存器
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] addr   : 寄存器地址
 * \param[in] data   : 新的寄存器值
 *
 * \return 无
 */
void am_sx127x_write (am_sx127x_handle_t handle, uint8_t addr, uint8_t data);

/**
 * \brief 读特定地址的射频寄存器
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] addr   : 寄存器地址
 *
 * \retval 寄存器值
 */
uint8_t am_sx127x_read (am_sx127x_handle_t handle, uint8_t addr);

/**
 * \brief 从起始地址写多个寄存器
 *
 * \param[in] handle   : SX1276 服务操作句柄
 * \param[in] addr     : 起始地址
 * \param[in] p_buffer : 包含新寄存器值的缓冲区
 * \param[in] size     : 要写的寄存器个数
 *
 * \return 无
 */
void am_sx127x_buffer_write (am_sx127x_handle_t handle,
                             uint8_t            addr,
                             uint8_t           *p_buffer,
                             uint8_t            size);

/**
 * \brief 从起始地址读多个寄存器
 *
 * \param[in]  handle   : SX1276 服务操作句柄
 * \param[in]  addr     : 起始地址
 * \param[out] p_buffer : 拷贝新寄存器值的缓冲区
 * \param[in]  size     : 要读的寄存器个数
 *
 * \return 无
 */
void am_sx127x_buffer_read (am_sx127x_handle_t handle,
                            uint8_t            addr,
                            uint8_t           *p_buffer,
                            uint8_t            size);

/**
 * \brief 设置最大负载长度
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] modem  : 使用的射频模式 [0: FSK, 1: LoRa]
 * \param[in] max    : 负载最大字节数
 *
 * \return 无
 */
void am_sx127x_max_payload_length_set (am_sx127x_handle_t     handle,
                                       am_lora_radio_modems_t modem,
                                       uint8_t                max);

/**
 * \brief 设置公网或私网(更新同步字)
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] enable : 1 公网， 0 私网
 *
 * \remark 只适用于LoRa模式
 *
 * \return 无
 */
void am_sx127x_public_network_set (am_sx127x_handle_t handle, uint8_t enable);

/**
 * note: extra interfaces added by ebi
 */

/**
 * \brief 设置同步字
 *
 * \param[in] handle   : SX1276 服务操作句柄
 * \param[in] syncword : 同步字
 *
 * \return 无
 */
void am_sx127x_syncword_set (am_sx127x_handle_t handle, uint8_t syncword);

/**
 * \brief 设置连续接收
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] enable : 使能或禁能
 *
 * \return 无
 */
void am_sx127x_rx_continuous_set (am_sx127x_handle_t handle, uint8_t enable);

/**
 * \brief 设置符号超时(ms)
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] enable : 使能或禁能
 *
 * \return 无
 */
void am_sx127x_symb_timeout_set (am_sx127x_handle_t handle, uint32_t ms);

/**
 * \brief 重传(如果size不为0，转换一部分负载到FIFO中)
 *
 * \param[in] handle : SX1276 服务操作句柄
 * \param[in] p_data : 重传的数据
 * \param[in] size   : 数据大小
 * \param[in] offset : 数据偏移
 *
 * \return 无
 */
void am_sx127x_tx_repeat_set (am_sx127x_handle_t  handle,
                              uint8_t            *p_data,
                              uint8_t             size,
                              uint8_t             offset);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_SX127X_H */

/* end of file */
