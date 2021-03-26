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
 * \brief fm175xx mifare card
 *
 * \internal
 * \par modification history:
 * - 1.03 18-11-10  mkr, add PICCB and LPCD features
 * - 1.02 18-1-10   mkr, solve copybits() function bugs
 * - 1.01 18-1-5    mkr, solve awbl_fm175xx_iso14443_anticoll() function bugs
 * - 1.00 17-11-30  mkr, first implementation
 * \endinternal
 */

#ifndef AWBL_FM175XX_H
#define AWBL_FM175XX_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aw_spi.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "awbl_spibus.h"

#define AWBL_FM175XX_NAME "awbl_FM175xx"

/**
 * \brief ISO/IEC 类型
 * @{
 */
#define AWBL_ISO_IEC_14443_A          (0x01U)
#define AWBL_ISO_IEC_14443_B          (0x02U)

/* @} */

/** \brief 支持的通讯速率 */
#define AWBL_FM175XX_COMM_SPEECH_106K (0) /**< \brief 106Kbps */
#define AWBL_FM175XX_COMM_SPEECH_212K (1) /**< \brief 212Kbps */
#define AWBL_FM175XX_COMM_SPEECH_424K (2) /**< \brief 424Kbps */

/** \brief 配置信息结构体 */
typedef struct awbl_fm175xx_cfg_param {
    /** \brief 射频输出管脚， 0:TX1, 1:TX2 */
    uint8_t  rf_tx_pin;

    /** \brief 译码器能够接收接收信号最小强度 */
    uint8_t  rx_min_level;

    /** \brief 接收译码器冲突位最小信号强度 */
    uint8_t  rx_coll_min_level;

    /** \brief 调制宽度 */
    uint8_t modwidth;

    /** \brief 接收器增益 */
    uint8_t rx_gain;

    /** \brief NMOS连续载波电导值 */
    uint8_t cwgsn;

    /** \brief NMOS调制电导值 */
    uint8_t modgsn;

    /** \brief PMOS连续载波电导值 */
    uint8_t cwgsp;

    /** \brief PMOS调制电导值 */
    uint8_t modgsp;

    /** \brief 通讯速率 */
    uint8_t comm_speed;

    /** \brief 协议类型 */
    uint8_t prtl_type;
} awbl_fm175xx_cfg_param_t;

/* A类卡的一种典型配置 */
#define AWBL_FM175XX_CFG_TYPEA_1       \
    {                                  \
        0x01,                          \
        0x05,                          \
        0x05,                          \
        0x26,                          \
        0x04,                          \
        0x0F,                          \
        0x08,                          \
        0x3F,                          \
        0x20,                          \
        AWBL_FM175XX_COMM_SPEECH_106K, \
        AWBL_ISO_IEC_14443_A           \
    }

/* B类卡的一种典型配置 */
#define AWBL_FM175XX_CFG_TYPEB_1       \
    {                                  \
        0x00,                          \
        0x05,                          \
        0x05,                          \
        0x26,                          \
        0x05,                          \
        0x0F,                          \
        0x04,                          \
        0x3F,                          \
        0x20,                          \
        AWBL_FM175XX_COMM_SPEECH_106K, \
        AWBL_ISO_IEC_14443_B           \
    }

/** \brief FM175xx device information */
typedef struct awbl_fm175xx_devinfo {
    /** \brief SPI模式 */
    uint16_t spi_mode;

    /** \brief SPI接口使能管脚 */
    int      nss_pin;

    /** \brief 复位/休眠控制脚 */
    int      npd_pin;

    /** \brief irq中断管脚 */
    int      irq_pin;

    /** \brief SPI 速率 */
    uint32_t spi_speed;

    /**
     * \brief FM175xx支持的协议类型,
     * \ref ISO/IEC 类型, 如 AWBL_ISO_IEC_14443_A 支持多种协议使用上述宏的或值
     */
    uint32_t prtl_type;

    /** \brief 卡片对应的芯片默认配置：
     *  - AWBL_FM175XX_CFG_TYPEA_1
     *  - AWBL_FM175XX_CFG_TYPEB_2
     */
    awbl_fm175xx_cfg_param_t config_type;

    /** \brief 平台初始化回调函数 */
    void (*pfunc_plfm_init)(void);
} awbl_fm175xx_devinfo_t;

/** \brief FM175xx device */
typedef struct awbl_fm175xx_dev {
    /** \brief SPI 设备 */
    struct awbl_spi_device  spi_dev;

    /** \brief 设备锁 */
    AW_MUTEX_DECL(devlock);
    AW_MUTEX_DECL(req_lock);

    /** \brief 射频输出管脚 */
    uint8_t rf_tx_pin;

    /** \brief 数据速率 */
    uint8_t speed;

    /** \brief lpcd pointer */
    void *p_lpcd;
} awbl_fm175xx_dev_t;

/******************************************************************************/
/**
 * \brief 设备复位，所有寄存器的配置项恢复成出厂设置
 * \note  设备复位后，可以使用 awbl_fm175xx_config 接口重新配置
 *
 * \param[in] id 设备ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_reset (uint8_t id);

/**
 * \brief FM175xx 配置
 *
 * \param[in] id         FM175xx 设备ID
 * \param[in] type       卡片类型
 * \param[in] cfg        配置参数
 *
 * \note 配置说明：
 *  +------+--------+--------+--------+
 *  | 包络     | 引脚TX  | GSPMos | GSNMos |
 *  +======+========+========+========+
 *  | 0    | 0      | pMod   | nMod   |
 *  +------+--------+--------+--------+
 *  | 1    | RF_n   | pCW    | nCW    |
 *  +------+--------+--------+--------+
 *
 *  - RF_n    13.56MHz反向时钟
 *  - GSPMos  电导，PMOS组配置
 *  - GSNMOS  电导，NMOS组配置
 *  - pCW     由 cwgsp 定义的PMOS连续载波电导值
 *  - pMod    由  modgsp 定义的PMOS调制电导值
 *  - nCW     由 cwgsn 定义的NMOS连续载波电导值
 *  - nMod    由  modgsn 定义的NMOS调制电导值
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_config (uint8_t                   id,
                              awbl_fm175xx_cfg_param_t *p_param);


/**
 * \brief 对设备进行上锁
 *
 * \param[in] id         FM175xx 设备ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_lock (uint8_t id);

/**
 * \brief 对设备解锁
 *
 * \param[in] id         FM175xx 设备ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_unlock (uint8_t id);

/**
 * \brief 使能输出调制波
 *
 * \param[in] id  设备ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_rf_on (uint8_t id);

/**
 * \brief 禁能输出调制波
 *
 * \param[in] id  设备ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_rf_off (uint8_t id);

/**
 * \brief CRC 设置
 *
 * \param[in] id      FM175xx 设备ID
 * \param[in] is_tx   是否是发送CRC，
 *                    - AW_TRUE 发送CRC
 *                    - AW_FALSE 接收CRC
 * \param[in] is_en   是否使能
 *
 * \return AW_OK
 */
aw_err_t awbl_fm175xx_crc_set (uint8_t id, aw_bool_t is_tx, aw_bool_t is_en);

/**
 * \brief 超时时间设置
 *
 * \param[in] id  FM175xx 设备ID
 * \param[in] ms  超时值（毫秒）
 */
aw_err_t awbl_fm175xx_timeout_set (uint8_t id, uint32_t ms);

/**
 * \brief FM175XX 指令
 * @{
 */
/** \brief idle */
#define AWBL_FM175XX_CMD_IDLE        (0x00)  
/** \brief activate crc coprocessor */
#define AWBL_FM175XX_CMD_CALCCRC     (0x03)  
/** \brief transmit data in fifo */
#define AWBL_FM175XX_CMD_TRANSMIT    (0x04)  
/** \brief change Command register but un-effect the primitive */
#define AWBL_FM175XX_CMD_NOCMDCHANGE (0x07)  
/** \brief activate receiver */
#define AWBL_FM175XX_CMD_RECEIVE     (0x08)  
/** \brief transmit fifo data and activate receiver */
#define AWBL_FM175XX_CMD_TRANSCEIVE  (0x0C)  
/** \brief execute M1 security authentication */
#define AWBL_FM175XX_CMD_AUTHENT     (0x0E)  
/** \brief software reset */
#define AWBL_FM175XX_CMD_SOFTRESET   (0x0F)  
/* @} */

/** \brief fm175xx 命令结构信息 */
typedef struct fm175xx_cmd {
    uint8_t        cmd;           /**< \brief 命令代码，可选 AWBL_FM175XX_CMD_* */
    uint8_t        nbits_to_snd;  /**< \brief 最后一个字节需要发送的位数 */
    uint8_t        nbytes_to_snd; /**< \brief 准备发送的字节 */
    uint8_t        nbytes_to_rcv; /**< \brief 准备接收的字节 */
    uint8_t        nbytes_snded;  /**< \brief 已发送字节数 */
    uint8_t        nbytes_rcved;  /**< \brief 已接收的字节数 */
    uint8_t        nbits_rcved;   /**< \brief 已接收的位数   */

    const uint8_t *p_sndbuf;      /**< \brief 发送数据缓冲区 */
    uint8_t       *p_rcvbuf;      /**< \brief 接收数据缓冲区 */

    uint8_t        collpos;       /**< \brief 碰撞位置       */
    aw_err_t       error;         /**< \brief 错误状态       */
    uint8_t        timeout;       /**< \brief 超时时间       */

    aw_bool_t      snd_finish;    /**< \brief 发送完成标志 */
} awbl_fm175xx_cmd_t;

/**
 * \brief cmd construct
 *
 * \param[in] p_cmd  FM175xx 命令结构指针
 * \param[in] cmd    命令值，可选 AWBL_FM175XX_CMD_*
 * \param[in] p_tx   发送缓冲区
 * \param[in] txlen  发送字节数
 * \param[in] txbits 最后一个发送字节可发送的有效位数，0表示整个字节发送
 * \param[in] p_rx   接收缓冲区
 * \param[in] rxlen  接收缓冲区长度
 *
 * \return N/A
 */
aw_static_inline void awbl_fm175xx_mkcmd (awbl_fm175xx_cmd_t *p_cmd,
                                          uint8_t             cmd,
                                          const uint8_t      *p_tx,
                                          uint8_t             txlen,
                                          uint8_t             txbits,
                                          uint8_t            *p_rx,
                                          uint8_t             rxlen)
{
    p_cmd->cmd           = cmd;
    p_cmd->p_rcvbuf      = p_rx;
    p_cmd->nbits_to_snd  = txbits;
    p_cmd->nbytes_to_rcv = rxlen;
    p_cmd->nbytes_to_snd = txlen;
    p_cmd->p_sndbuf      = p_tx;
}

/**
 * \brief command execute
 *
 * \param[in] id     FM175XX 设备ID
 * \param[in] p_cmd  命令信息指针
 *
 * \note p_cmd 信息推荐使用 awbl_fm175xx_mkcmd 构造使用。
 * \return errno
 */
aw_err_t awbl_fm175xx_cmd_execute (uint8_t id, awbl_fm175xx_cmd_t *p_cmd);

/** \brief 芯片模式选择 */
#define AWBL_FM175XX_DEEP_PWRDOWN  (0)  /**< \brief Deep Power Down */
#define AWBL_FM175XX_HARD_PWRDOWN  (1)  /**< \brief Hard Power Down */
#define AWBL_FM175XX_SOFT_PWRDOWN  (2)  /**< \brief Soft Power Down */

/**
 * \brief FM175XX mode setting
 *
 * \param[in] id    FM175XX 设备ID
 * \param[in] mode  进入模式
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_low_power_enter (uint8_t id, uint8_t mode);

/**
 * \brief FM175XX mode setting
 *
 * \param[in] id    FM175XX 设备ID
 * \param[in] mode  退出模式
 *
 * \note 退出AWBL_FM175XX_DEEP_PWRDOWN模式时，需要重新配置初始化FM175XX设备
 * \return errno
 */
aw_err_t awbl_fm175xx_low_power_exit (uint8_t id, uint8_t mode);

/**
 * \brief LPCD 功能
 * @{
 */
#define AWBL_FM175XX_LPCD_BIAS_CURRENT      (0)
/** \brief TX2输出使能，且与TX1反向 */
#define AWBL_FM175XX_LPCD_TX2RFEN           (1)   
/** \brief N管驱动为大驱动 */
#define AWBL_FM175XX_LPCD_CWN               (0x1) 
/** \brief P管驱动能力配置, 0~7逐渐增大，默认选择3 */
#define AWBL_FM175XX_LPCD_CWP               (0x3) 
/** \brief 低功耗模式控制 */
#define AWBL_FM175XX_LPCD_MODE              (0x0) 
/** \brief T1阶段配置 */
#define AWBL_FM175XX_LPCD_TIMER1_CFG        (3)  
/** \brief T2阶段配置 */ 
#define AWBL_FM175XX_LPCD_TIMER2_CFG        (13)  
/** \brief T3阶段配置 */
#define AWBL_FM175XX_LPCD_TIMER3_CFG        (11)  
/** \brief Vmid配置 */
#define AWBL_FM175XX_VMID_BG_CFG            (8)   
/** \brief 使能自动唤醒功能 */
#define AWBL_FM175XX_AUTO_WUP_EN            (1)   
/**
 * \brief 自动唤醒时间：
 * - 0 ： 6S
 * - 1 : 12S
 * - 2 : 15minute
 * - 4 : 1hour
 * - 5 : 1.8hour
 * - 6 : 3.6hour
 * - 7 : 7.2hour
 * 默认为12S，是否生效取决于自动唤醒是否使能
 */
#define AWBL_FM175XX_AUTO_WUP_CFG           (1)
#define AWBL_FM175XX_LPCD_AUTO_DETECT_TIMES (0)   /**< \brief T3阶段配置 */

typedef struct awbl_fm175xx_lpcd {
    /** \brief LPCD放大器倍数 */
    uint8_t lpcd_gain_amplify;

    /** \brief LPCD衰减倍数 */
    uint8_t lpcd_gain_reduce;

    /** \brief LPCD基准电流 */
    uint8_t lpcd_bias_current;

    /** \brief ADC参考电平 */
    uint8_t lpcd_adc_reference;

    /** \brief T3阶段工作时钟分频比 */
    uint8_t t3_clk_div;

    /** \brief T1时长 */
    uint8_t timer1_cfg;

    /** \brief T2时长 */
    uint8_t timer2_cfg;

    /** \brief T3时长 */
    uint8_t timer3_cfg;

    /** \brief 检测灵敏度 */
    uint8_t det_sensitive;

    /** \brief T3 下满幅ADCResult信息 */
    uint8_t adc_result_full_scale;

    /** \brief LPCD幅度中心值 */
    uint8_t adc_result_center;

    /** \brief 检测幅值，设置成相对值 */
    uint8_t adc_result_threshold;

    aw_pfuncvoid_t  lpcd_func;
    void           *p_arg;
} awbl_fm175xx_lpcd_t;

/**
 * \brief LPCD参数初始化
 *
 * \param[in] p_lpcd    指向awbl_fm175xx_lpcd_t的指针
 * \param[in] lpcd_func 唤醒或检测触发回调函数
 * \param[in] p_usr     回调函数用户参数
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_lpcd_param_init (uint8_t              id,
                                       awbl_fm175xx_lpcd_t *p_lpcd,
                                       aw_pfuncvoid_t       lpcd_func,
                                       void                *p_usr);

/**
 * \brief LPCD 配置
 *
 * \param[in] id      FM175XX设备ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_lpcd_config (uint8_t id);

/**
 * \brief LPCD 校准初始化.
 * \note 中断不安全
 *
 * \param[in] id             FM175XX设备ID
 * \param[in] det_sensitive  检测灵敏度设置，灵敏度越大，能够检测卡的距离越小，
 *                           正常是在4~15%之间，单位为1/100。
 *
 * \return
 */
aw_err_t awbl_fm175xx_lpcd_calibra_init (uint8_t id, uint8_t det_sensitive);

/**
 * \brief LPCD 控制（进入或退出）
 * \note LPCD有事件触发时，自动退出LPCD模式，用户如果需要再次进入，需要再次调用本函数。
 *
 * \param[in] id       FM175XX设备ID
 * \param[in] is_enter 是否进入LPCD，TRUE-进入；FALSE-退出
 *
 * \return
 */
aw_err_t awbl_fm175xx_lpcd_ctrl (uint8_t id, aw_bool_t is_enter);

#define AWBL_FM175XX_LPCD_CARDDET_EVENT     (1U << 0) /**< \brief 卡片侦测事件 */
#define AWBL_FM175XX_LPCD_AWUP_EVENT        (1U << 4) /**< \brief 自动唤醒事件 */

/**
 * \brief LPCD 事件预处理，LPCD时间触发之后执行。
 * \note 中断不安全
 *
 * \param[in] id       FM175XX设备ID
 * \param[out] p_type  事件类型
 *
 * \return
 */
aw_err_t awbl_fm175xx_lpcd_event_pre_handle (uint8_t id, uint8_t *p_type);

/* @} */

/******************************************************************************/
/**
 * \brief 请求模式
 * @{
 */
/** \brief 仅请求所有空闲卡（不包括处于HALT状态的卡） */
#define AWBL_FM175XX_PICC_REQ_IDLE  (0) 
/** \brief 请求所有卡（包括处于HALT状态的卡） */
#define AWBL_FM175XX_PICC_REQ_ALL   (1) 

/* @} */

/**
 * \brief 操作命令
 * @{
 */
/** \brief halt */
#define AWBL_PICC_CMD_HALT   (0x50) 

/** \brief TYPE A 操作命令 */
/** \brief request */
#define AWBL_PICC_CMD_REQA   (0x26)  
/** \brief wakeup */
#define AWBL_PICC_CMD_WUPA   (0x52)  

/* @} */

/**
 * \brief PICCA 卡接口，符合ISO/IEC 14443 A协议
 * @{
 */
/**
 * \brief 卡片(ISO/IEC 14443 Type A协议)请求
 *
 * \param[in]  id        fm175xx 设备ID
 * \param[in]  req_mode  请求模式:
 *                       - AWBL_FM175XX_PICCA_REQ_IDLE
 *                       - AWBL_FM175XX_PICCA_REQ_ALL
 * \param[out] p_atqa    IC卡应答信息（2字节大小）
 *
 * \note 当多张卡同类型的卡同时被唤醒时，调用该接口尽管可以接收到卡片的应答信息，
 *       在接收到的应答信息p_atqa的[bit4 ~ bit0]位段(Bit frame anticollision)
 *       记录冲突情况，默认情况下该位段在无冲突发生时应该只有一个位置位的，
 *       但是不影响后续的操作，因为卡片已被唤醒。
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_picca_request (uint8_t    id,
                                     uint8_t    req_mode,
                                     uint16_t  *p_atqa);

/**
 * \brief 卡片(ISO/IEC 14443 Type A协议)防碰撞操作
 *
 * \param[in]  id      fm175xx 设备ID
 * \param[in]  cl      cascade level
 * \param[out] p_dat   IC卡应答信息缓存（5字节大小）
 *
 * \note p_dat至少应该为5字节大小，防碰撞成功后，得到的数据如下：
 *       4字节UID + 1字节BCC 或
 *       1字节CT + 3字节UID + 1字节BCC
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_picca_anticoll (uint8_t  id,
                                      uint8_t  cl,
                                      uint8_t *p_dat);

/**
 * \brief 卡片(ISO/IEC 14443 Type A协议)选择
 *
 * \param[in]  id      fm175xx 设备ID
 * \param[in]  cl      cascade level
 * \param[in]  p_dat   IC卡应答信息缓存（5字节大小），防碰撞中得到的数据
 * \param[out] p_sak   selection应答信息
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_picca_select (uint8_t  id,    
                                    uint8_t  cl,
                                    uint8_t *p_dat, 
                                    uint8_t *p_sak);

/**
 * \brief 卡片激活(ISO/IEC 14443 Type A协议)
 *
 * \param[in]  id         fm175xx 设备单元号
 * \param[in]  req_mode   请求模式:
 *                        - AWBL_FM175XX_PICCA_REQ_IDLE
 *                        - AWBL_FM175XX_PICCA_REQ_ALL
 * \param[out] p_atqa     ATQA
 * \param[out] p_uid      卡片UID信息
 * \param[out] p_uid_len  uid信息长度(字节)
 * \param[out] p_sak      SAK应答信息
 *
 * \return
 */
aw_err_t awbl_fm175xx_picca_active (uint8_t   id,
                                    uint8_t   req_mode,
                                    uint16_t *p_atqa,
                                    uint8_t  *p_uid,
                                    uint8_t  *p_uid_len,
                                    uint8_t  *p_sak);

/**
 * \brief 卡片(ISO/IEC 14443 Type A协议)停止
 *
 * \param[in] id      fm175xx 设备ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_picca_halt (uint8_t id);

/* @} */

/******************************************************************************/
/**
 * \brief AFI(Application Family ID)类型定义
 * \note 应用类型
 * @{
 */
/** \brief AFI main families type */
/** \brief All main Family */
#define AWBL_PICCB_AFI_MAIN_ALL                 (0U) 
#define AWBL_PICCB_AFI_MAIN_TRANSPORT           (1U)
#define AWBL_PICCB_AFI_MAIN_FINANCIAL           (2U)
#define AWBL_PICCB_AFI_MAIN_IDENTIFICATION      (3U)
#define AWBL_PICCB_AFI_MAIN_TELECOMMUNICATION   (4U)
#define AWBL_PICCB_AFI_MAIN_MEDICAL             (5U)
#define AWBL_PICCB_AFI_MAIN_MULTIMEDIA          (6U)
#define AWBL_PICCB_AFI_MAIN_GAMING              (7U)
#define AWBL_PICCB_AFI_MAIN_DATASTORAGE         (8U)

/** \brief AFI sub families type */
/** \brief All sub Family */
#define AWBL_PICCB_AFI_SUB_ALL                  (0U) 

/**
 * \brief AFI 类型定义
 *
 * \param[in] main  AFI 应用主类型, \ref AWBL_PICCB_AFI_xxx
 * \param[in] sub   AFI 应用此类型, 范围：0x00 ~ 0x0F
 */
#define awbl_piccb_afi(main, sub)               ((main << 4) | sub)

/* @} */

/** \brief PICC B ATQB */
typedef struct awbl_piccb_dev {
    /** \brief Pseudo unique PICC Identifier, 卡号 */
    uint8_t pupi[4];

    /** \brief Application Data, 应用数据 */
    struct app {
        uint8_t afi;
        uint8_t crcb_aid[2];
        uint8_t app_num;
    } app_dat;

    /** \brief protocol info, 协议信息 */
    struct protocol {
        uint8_t bitrate_cap;
        uint8_t protocol_type;
        uint8_t maxframe_size;
        uint8_t fo;
        uint8_t adc;
        uint8_t fwi;
    } prtl_info;
} awbl_piccb_dev_t;

/**
 * \brief PICC B 卡接口，符合ISO/IEC 14443 B协议
 * @{
 */
/**
 * \brief 请求卡片
 * \note 卡片类型需要符合(ISO/IEC 14443 B协议)
 *
 * \param[in] id        设备 ID
 * \param[in] req_mode  请求模式：
 *                      - AWBL_FM175XX_PICC_REQ_IDLE
 *                      - AWBL_FM175XX_PICC_REQ_ALL
 * \param[in] afi       Application Family ID,应用族ID
 * \param[in] slots     槽序号
 * \param[out] p_atqb   ATQB
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_piccb_request (uint8_t           id,
                                     uint8_t           req_mode,
                                     uint8_t           afi,
                                     uint8_t           slots,
                                     awbl_piccb_dev_t *p_dev);

/**
 * \brief 发送Slot-MARKER
 * \note 卡片类型需要符合(ISO/IEC 14443 B协议)
 * \note Slot-MARKER命令在如下两种情况下需要发送：
 * - PCD接收到PICC的ATQB命令后开始下一个槽(时隙)时，即发送下一条命令时；
 * - 如果知道该槽号是空的，并且没有ATQB到来，那么不需要一直等待ATQB到该槽结束。
 * 该命令不是槽卡片强制要求支持的，卡片不支持时，该命令将会被忽视。此时需要通过
 * awbl_fm175xx_piccb_request 函数，利用 PROBABILISTIC(概率)方法获取卡片的ATQB.
 *
 * \param[in] id        FM175XX 设备 ID
 * \param[in] slot_seq  槽序号
 * \param[out] p_piccb  pointer to awbl_piccb_dev_t
 *
 * \return errno.
 */
aw_err_t awbl_fm175xx_piccb_slot_marker_send (uint8_t           id,
                                              uint8_t           slot_seq,
                                              awbl_piccb_dev_t *p_piccb);

/** \brief 最小TR0值 */
enum awbl_piccb_attrib_min_tr0 {
    AWBL_PICCB_ATTRIB_MIN_TR0_DEFAULT, /**< \brief 默认值 */
    AWBL_PICCB_ATTRIB_MIN_TR0_48_FS,   /**< \brief 48/fs */
    AWBL_PICCB_ATTRIB_MIN_TR0_16_FS    /**< \brief 16/fs */
};

/** \brief 最小TR1值 */
enum awbl_piccb_attrib_min_tr1 {
    AWBL_PICCB_ATTRIB_MIN_TR1_DEFAULT, /**< \brief 默认值 */
    AWBL_PICCB_ATTRIB_MIN_TR1_64_FS,   /**< \brief 64/fs */
    AWBL_PICCB_ATTRIB_MIN_TR1_16_FS    /**< \brief 16/fs */
};

/** \brief PCD能够接收的最大帧大小 */
enum awbl_piccb_attrib_max_frame_size {
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_16,  /**< \brief 16 字节 */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_24,  /**< \brief 24 字节 */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_32,  /**< \brief 32 字节 */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_40,  /**< \brief 40 字节 */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_48,  /**< \brief 48 字节 */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_64,  /**< \brief 64 字节 */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_96,  /**< \brief 96 字节 */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_128, /**< \brief 128 字节 */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_256, /**< \brief 256 字节 */
};

/** \brief PCD 到 PICC位速率 */
enum awbl_piccb_attrib_pcd2picc_bitrate {
    AWBL_PICCB_ATTRIB_PCD2PICC_BITRATE_106, /**< \brief PCD到PICC，位率为 106 kbit/s */
    AWBL_PICCB_ATTRIB_PCD2PICC_BITRATE_212, /**< \brief PCD到PICC，位率为 212 kbit/s */
    AWBL_PICCB_ATTRIB_PCD2PICC_BITRATE_424, /**< \brief PCD到PICC，位率为 424 kbit/s */
    AWBL_PICCB_ATTRIB_PCD2PICC_BITRATE_847, /**< \brief PCD到PICC，位率为 847 kbit/s */
};

/** \brief PICC 到 PCD位速率 */
enum awbl_piccb_attrib_picc2pcd_bitrate {
    AWBL_PICCB_ATTRIB_PICC2PCD_BITRATE_106, /**< \brief PCD到PICC，位率为 106 kbit/s */
    AWBL_PICCB_ATTRIB_PICC2PCD_BITRATE_212, /**< \brief PCD到PICC，位率为 212 kbit/s */
    AWBL_PICCB_ATTRIB_PICC2PCD_BITRATE_424, /**< \brief PCD到PICC，位率为 424 kbit/s */
    AWBL_PICCB_ATTRIB_PICC2PCD_BITRATE_847, /**< \brief PCD到PICC，位率为 847 kbit/s */
};

/** \brief ATTRIB */
typedef struct awbl_piccb_attrib {
    aw_bool_t   is_eof;    /**< \brief 是否需要帧结束标识，FALSE : 不需要；TRUE : 需要 */
    aw_bool_t   is_sof;    /**< \brief 是否需要帧起始标识，FALSE : 不需要；TRUE : 需要 */
    uint8_t  cid;          /**< \brief 定义卡片的CID值，如果卡片不支持，设置为0 */
    uint8_t  hinfo_len;    /**< \brief 高层信息长度 */
    uint8_t *p_hinfo;      /**< \brief 高层信息，可选内容 */

    /** \brief 最小TR0值 */
    enum awbl_piccb_attrib_min_tr0 tr0;

    /** \brief 最小TR1值 */
    enum awbl_piccb_attrib_min_tr1 tr1;

    /** \brief PCD 能够接收的最大帧大小 */
    enum awbl_piccb_attrib_max_frame_size   max_frame_size;

    /** \brief PCD 到 PICC 的位率 */
    enum awbl_piccb_attrib_pcd2picc_bitrate pcd2picc_bitrate;

    /** \brief PICC 到 PCD 的位率 */
    enum awbl_piccb_attrib_picc2pcd_bitrate picc2pcd_bitrate;
} awbl_piccb_attrib_t;

typedef struct awbl_piccb_attrib_rsp {
    /**
     * \brief 最大缓冲区长度指示(MBLI)，卡片内部缓冲到接收帧的限制。
     *
     * mbli为0，表示卡片没有提供信息给内部的输入缓冲；mbli大于0，则最大缓冲长度为 =
     * (卡片最大帧大小) * 2^(mbli-1)，卡片会在ATQB中返回卡片的最大帧大小。
     */
    uint8_t  mbli;

    uint8_t  cid;      /**< \brief PICC不支持时该值为0 */
    uint8_t *p_info;   /**< \brief 高层信息 */
    uint8_t  info_len; /**< \brief 高层信息长度 */
} awbl_piccb_attrib_rsp_t;

/**
 * \brief 读取卡片属性
 * \note 卡片类型需要符合(ISO/IEC 14443 B协议)
 *
 * \param[in] id     FM175XX 设备 ID
 * \param[in] p_arg
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_piccb_attrib_get (uint8_t                  id,
                                        awbl_piccb_dev_t        *p_dev,
                                        awbl_piccb_attrib_t     *p_attrib,
                                        awbl_piccb_attrib_rsp_t *p_rsp);

#if 0
/**
 * \brief 读取UID数据
 * \note 需要获取uid数据时，用户需要确保p_uid指向的缓存能够容纳uid数据的长度，
 *       PICCB类型卡的UID长度为10字节。
 *
 * \param[in] id     FM175XX 设备 ID
 * \param[in] cid    通过 ``awbl_fm175xx_piccb_attrib_get`` 函数获取，
 *                   位于 awbl_piccb_attrib_rsp_t成员中
 * \param[out] p_uid 存储uid信息数据缓存
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_piccb_uid_read (uint8_t id, uint8_t cid, uint8_t *p_uid);

#endif

/**
 * \brief 停止PICC
 *
 * \param[in] id    FM175XX 设备 ID
 * \param[in] p_id  卡片ID，长度为4字节，可通过ATQB获得。
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_piccb_halt (uint8_t           id,
                                  awbl_piccb_dev_t *p_dev);

/* @} */
/******************************************************************************/
/**
 * \brief mifare 指令
 * @{
 */
#define AWBL_FM175XX_MIFARE_KEYA     (0x60) /**< \brief KEY A 认证指令 */
#define AWBL_FM175XX_MIFARE_KEYB     (0x61) /**< \brief KEY B 认证指令 */
#define AWBL_FM175XX_MIFARE_READ     (0x30) /**< \brief 读 */
#define AWBL_FM175XX_MIFARE_WRITE    (0xA0) /**< \brief 写 */
#define AWBL_FM175XX_MIFARE_DECR     (0xC0) /**< \brief 减小数值 */
#define AWBL_FM175XX_MIFARE_INCR     (0xC1) /**< \brief 增加数值 */
#define AWBL_FM175XX_MIFARE_REST     (0xC2) /**< \brief 恢复存储数值 */
#define AWBL_FM175XX_MIFARE_TRAN     (0xB0) /**< \brief 传输 */
/* @} */

/**
 * \brief Mifare 卡接口
 * @{
 */

/**
 * \brief mifare 卡 authentication
 *
 * \param[in] id        FM175XX 设备ID
 * \param[in] key_type  秘钥类型，可选：
 *                      - AWBL_KEY_A
 *                      - AWBL_KEY_B
 * \param[in] blk_addr  块（block）号，只需对一个扇区中的其中一块进行验证即可
 * \param[in] p_key     秘钥缓存
 * \param[in] p_uid     UID缓存
 * \param[in] key_num   秘钥个数
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_auth (uint8_t         id,
                                   uint8_t         key_type,
                                   uint8_t         blk_addr,
                                   const uint8_t  *p_key,
                                   const uint8_t  *p_uid,
                                   uint8_t         key_num);

/**
 * \brief mifare read block data
 *
 * \param[in] id        FM175XX 设备ID
 * \param[in] blk_addr  块地址
 * \param[in] p_buf     数据缓冲区（存储读取的数据）
 *
 * \note 用户调用该接口时，确保数据缓冲区的大小可以足够放下一个数据块的大小
 *       如S50的一个块大小为16字节
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_read (uint8_t   id,
                                   uint8_t   blk_addr,
                                   uint8_t  *p_buf,
                                   uint8_t   blk_size);

/**
 * \brief mifare write block data
 *
 * \param[in] id        FM175XX 设备ID
 * \param[in] blk_addr  块地址
 * \param[in] p_buf     数据缓冲区（存储待写入数据）
 * \param[in] blk_size  块大小
 *
 * \note 用户调用该接口时，确保数据缓冲区的大小和blk_size指定的大小相符合
 *       应该足够容纳一块数据大小如S50的一块数据为16字节，则p_buf至少16字节大小
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_write (uint8_t         id,
                                    uint8_t         blk_addr,
                                    const uint8_t  *p_buf,
                                    uint8_t         blk_size);

/**
 * \brief mifare increment
 *
 * \param[in] id        FM175XX 设备ID
 * \param[in] blk_addr  块地址
 * \param[in] p_dat     数据缓存（存储需要增加的数据）
 * \param[in] len       数据个数
 *
 * \note 只针对value block有效，执行该命令前需要将块（block）初始化成value block,
 *       p_dat 必须是4字节大小，len 大小为4
 * \note errno
 */
aw_err_t awbl_fm175xx_mifare_increment (uint8_t         id,
                                        uint8_t         blk_addr,
                                        const uint8_t  *p_dat,
                                        uint8_t         len);

/**
 * \brief mifare decrement
 *
 * \param[in] id        FM175XX 设备ID
 * \param[in] blk_addr  块地址
 * \param[in] p_dat     数据缓存（存储需要减小的数据）
 * \param[in] len       数据个数
 *
 * \note 只针对value block有效，执行该命令前需要将块（block）初始化成value block
 * \note errno
 */
aw_err_t awbl_fm175xx_mifare_decrement (uint8_t         id,
                                        uint8_t         blk_addr,
                                        const uint8_t  *p_dat,
                                        uint8_t         len);

/**
 * \brief mifare restore
 *
 * \param[in] id        FM175XX 设备ID
 * \param[in] blk_addr  块地址
 *
 * \note 只针对value block有效，执行该命令前需要将块（block）初始化成value block
 *       执行该函数之后，数据仅仅保存在transfer buffer中，需要执行 
 *       awbl_mifare_transfer之后才会把数据重新保存到block中
 *
 * \note errno
 */
aw_err_t awbl_fm175xx_mifare_restore (uint8_t id, uint8_t blk_addr);

/**
 * \brief mifare transfer
 *
 * \param[in] id        FM175XX 设备ID
 * \param[in] blk_addr  块地址
 *
 * \note 只针对value block有效，执行该命令前需要将块（block）初始化成value block
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_transfer (uint8_t id, uint8_t blk_addr);

/**
 * \brief value block 初始化
 *        将一个block初始化成value block
 *
 * \param[in] id        FM175XX 设备ID
 * \param[in] blkaddr   块地址
 * \param[in] dat       初始值
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_valblk_construct (uint8_t  id,
                                               uint8_t  blkaddr,
                                               int32_t  dat);

/**
 * \brief modify access bit field
 *
 * \param[in] id        FM175XX 设备ID
 * \param[in] sec_addr  扇区(sector)地址
 * \param[in] p_keya    KEYA 秘钥缓存
 * \param[in] p_keyb    KEYB 秘钥缓存
 * \param[in] p_ab      访问位（access bit）缓存
 * \param[in] key_len   秘钥个数（字节）
 * \param[in] ab_len    访问位个数（字节）
 *
 * \note 可以通过该接口对一个扇区的秘钥值进行修改，需要格外注意，修改某个秘钥值
 *       或者访问位时，需要确保不修改的位域其值保持不变，如：
 *       KEYA秘钥：0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
 *       KEYB秘钥：0x11, 0x11, 0x11, 0x11, 0x11, 0x11
 *       访问位：0xFF, 0x07, 0x80
 *       只修改KEYA秘钥时，传入的参数中，p_keyb存储的秘钥值为上述KEYB秘钥值，
 *       p_ab存储的访问位为上述访问位值，确保这两部分在修改前后保持不变。
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_sector_trailer_modify (uint8_t         id,
                                                    uint8_t         sec_addr,
                                                    const uint8_t  *p_keya,
                                                    const uint8_t  *p_keyb,
                                                    const uint8_t  *p_ab,
                                                    uint8_t         key_len,
                                                    uint8_t         ab_len);

/**
 * \brief FM175XX driver register
 */
void awbl_fm175xx_drv_register (void);

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif

/* end of file*/

