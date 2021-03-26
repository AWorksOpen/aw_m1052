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
 * \brief 通用CAN接口
 *
 * 使用本服务需要包含头文件 aw_can.h
 * \code
 * #include aw_can.h
 * \endcode
 *
 * \par 简单示例
 * \code
 * #include "aworks.h"
 * #include "aw_can.h"
 * #include "aw_prj_params.h"
 *
 * static uint32_t __can_timestamp_get_cb (uint32_t timestamp)
 * {
 *     return timestamp;
 * }
 *
 * static aw_bool_t __can_err_sta_cb (
 *     int chn, aw_can_bus_err_t err, aw_can_bus_status_t status)
 * {
 *     aw_can_err_cnt_t  can_err_reg;
 *
 *     aw_can_err_cnt_get(chn, &can_err_reg);
 *     if ((can_err_reg.tx_err_cnt > 127) || (can_err_reg.rx_err_cnt > 127)) {
 *         aw_can_err_cnt_clr(chn);
 *     }
 *
 *     return AW_TRUE; // 函数返回值决定错误消息是否存入rngbuf,
 *                     // AW_TRUE:不保存,AW_FALSE:保存
 * }
 *
 * static aw_can_app_callbacks_t __g_can_app_callback_fun = {
 *     __can_timestamp_get_cb,
 *     NULL,
 *     NULL,
 *     __can_err_sta_cb,
 *     NULL
 * };
 *
 * static aw_can_baud_param_t __g_can_btr = AW_CFG_CAN_BTR_500K;
 *
 * void aw_main(void)
 * {
 *     int can_id = 0;
 *
 *     aw_can_init(can_id,                      // CAN通道号
 *                 AW_CAN_WORK_MODE_NORMAL,     // 控制器工作模式
 *                 &__g_can_btr,                // 当前波特率
 *                 &__g_can_app_callback_fun);  // 用户回调函数
 *     aw_can_start(can_id);
 * }
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.03 18-09-21  anu, modify demo
 * - 1.02 18-06-20  anu, modify aw_can_baud_param_t val not add 1
 * - 1.01 17-08-30  anu, modify
 * - 1.00 15-12-02  cae, first implementation
 * \endinternal
 */

#ifndef __AW_CAN_H
#define __AW_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_can
 * \copydoc aw_can.h
 * @{
 */

#include "aw_types.h"

/** \brief CAN控制器类型定义 */
typedef uint8_t aw_can_ctrl_type_t;             /**< \brief CAN控制器类型 */
#define AW_CAN_CTLR_HS                  0x00    /**< \brief CAN */
#define AW_CAN_CTLR_FD                  0x01    /**< \brief CANFD */

/** \brief CAN工作模式定义 */
typedef uint8_t aw_can_work_mode_type_t;        /**< \brief CAN工作模式 */
#define AW_CAN_WORK_MODE_NORMAL         0x00    /**< \brief 正常工作模式 */
#define AW_CAN_WORK_MODE_LISTEN_ONLY    0x01    /**< \brief 只听工作模式 */
#define AW_CAN_WORK_MODE_NON_ISO        0x02    /**< \brief 非ISO标准的工作模式(博世标准) */

/** \brief CAN总线错误定义 */
typedef uint32_t aw_can_bus_err_t;              /**< \brief CAN总线错误类型 */
#define AW_CAN_BUS_ERR_NONE             0x00    /**< \brief 无错误 */
#define AW_CAN_BUS_ERR_BIT              0x01    /**< \brief 位错误 */
#define AW_CAN_BUS_ERR_ACK              0x02    /**< \brief 应答错误 */
#define AW_CAN_BUS_ERR_CRC              0x04    /**< \brief CRC错误 */
#define AW_CAN_BUS_ERR_FORM             0x08    /**< \brief 格式错误 */
#define AW_CAN_BUS_ERR_STUFF            0x10    /**< \brief 填充错误 */
#define AW_CAN_BUS_ERR_DATAOVER         0x20    /**< \brief 超载错误 */
#define AW_CAN_BUS_ERR_RCVBUFF_FULL     0x40    /**< \brief 接收缓冲区满 */
#define AW_CAN_BUS_ERR_TXDBUFF_FULL     0x80    /**< \brief 发送缓冲区满 */

/** \brief CAN总线状态定义 */
typedef uint32_t aw_can_bus_status_t;           /**< \brief CAN总线状态类型 */
#define AW_CAN_BUS_STATUS_OK            0x00    /**< \brief 总线正常 */
#define AW_CAN_BUS_STATUS_ERROR         0xE1    /**< \brief 总线错误 */
#define AW_CAN_BUS_STATUS_WARN          0xE2    /**< \brief 总线警告 */
#define AW_CAN_BUS_STATUS_PASSIVE       0xE3    /**< \brief 总线消极 */
#define AW_CAN_BUS_STATUS_OFF           0xE4    /**< \brief 总线关闭 */
#define AW_CAN_BUS_STATUS_DATAOVER      0xE5    /**< \brief 总线超载 */
#define AW_CAN_BUS_STATUS_WAKE          0xE6    /**< \brief 总线唤醒 */

/** \brief CAN函数返回值错误代码定义 */
typedef int aw_can_err_t;                       /**< \brief CAN错误码 */

/** \brief 无错误 */
#define AW_CAN_ERR_NONE                       0     

/** \brief 错误基号 */                 
#define AW_CAN_ERR_BASE                       0          

/** \brief 总线忙 */            
#define AW_CAN_ERR_BUSY                      (AW_CAN_ERR_BASE + 80) 

/** \brief 设备未初始化 */ 
#define AW_CAN_ERR_NOT_INITIALIZED           (AW_CAN_ERR_BASE + 81)  

/** \brief 无注册驱动函数 */
#define AW_CAN_ERR_NONE_DRIVER_FUNC          (AW_CAN_ERR_BASE + 82)

/** \brief 非法通道号 */  
#define AW_CAN_ERR_ILLEGAL_CHANNEL_NO        (AW_CAN_ERR_BASE + 83) 

/** \brief 非法通道模式 */ 
#define AW_CAN_ERR_ILLEGAL_CHANNEL_MODE      (AW_CAN_ERR_BASE + 84) 

/** \brief 非法的控制器类型 */ 
#define AW_CAN_ERR_ILLEGAL_CTRL_TYPE         (AW_CAN_ERR_BASE + 85) 

/** \brief 控制器类型不匹配 */ 
#define AW_CAN_ERR_INCOMPATIBLE_CTRL_TYPE    (AW_CAN_ERR_BASE + 86)  

/** \brief 控制器已启动 */
#define AW_CAN_ERR_CTRL_STARTED              (AW_CAN_ERR_BASE + 87)  

/** \brief 控制器未启动 */
#define AW_CAN_ERR_CTRL_NOT_START            (AW_CAN_ERR_BASE + 88)  

/** \brief 无可操作资源 */
#define AW_CAN_ERR_NO_OP                     (AW_CAN_ERR_BASE + 89) 

/** \brief 设备不存在 */ 
#define AW_CAN_ERR_NO_DEV                    (AW_CAN_ERR_BASE + 90)  

/** \brief 非法数据长度 */
#define AW_CAN_ERR_ILLEGAL_DATA_LENGTH       (AW_CAN_ERR_BASE + 91)  

/** \brief 非法的掩码值 */
#define AW_CAN_ERR_ILLEGAL_MASK_VALUE        (AW_CAN_ERR_BASE + 92)  

/** \brief 无效参数 */
#define AW_CAN_ERR_INVALID_PARAMETER         (AW_CAN_ERR_BASE + 93)  

/** \brief 非法偏移值 */
#define AW_CAN_ERR_ILLEGAL_OFFSET            (AW_CAN_ERR_BASE + 94)  

/** \brief 配置模式中休眠 */
#define AW_CAN_ERR_SLEEP_IN_CFG_MODE         (AW_CAN_ERR_BASE + 95)  

/** \brief 无法设置唤醒中断 */
#define AW_CAN_ERR_CANNOT_SET_WAKEUPINT      (AW_CAN_ERR_BASE + 96) 

/** \brief 无法设置总线关闭中断 */ 
#define AW_CAN_ERR_CANNOT_SET_BOFFINT        (AW_CAN_ERR_BASE + 97)  

/** \brief 无法设置错误中断 */
#define AW_CAN_ERR_CANNOT_SET_ERRINT         (AW_CAN_ERR_BASE + 98)  

/** \brief 波特率错误 */
#define AW_CAN_ERR_BAUDRATE_ERROR            (AW_CAN_ERR_BASE + 99)  

/** \brief 终止传输 */
#define AW_CAN_ERR_ABORT_TRANSMITTED         (AW_CAN_ERR_BASE + 100) 

/** \brief 传输超时 */
#define AW_CAN_ERR_TRANSMITTED_TIMEOUT       (AW_CAN_ERR_BASE + 101) 

/** \brief 总线错误 */
#define AW_CAN_ERR_BUS_FAULT                 (AW_CAN_ERR_BASE + 102) 

/** \brief 无接收缓存区 */
#define AW_CAN_ERR_RXD_BUFF_UNUSED           (AW_CAN_ERR_BASE + 103) 

/** \brief 无注册接收函数 */
#define AW_CAN_ERR_NONE_RCV_FUNC             (AW_CAN_ERR_BASE + 104) 

/** \brief 不支持该操作 */
#define AW_CAN_ERR_NOT_SUPPORT               (AW_CAN_ERR_BASE + 105) 

/** \brief 未知错误 */
#define AW_CAN_ERR_UNKNOWN_ERROR             (AW_CAN_ERR_BASE + 255) 

/** \brief CAN控制器数据长度定义 */
#define AW_CAN_HS_MAX_LENGTH                 8        /**< \brief CAN数据长度 */
#define AW_CAN_FD_MAX_LENGTH                 64       /**< \brief CANFD数据长度*/

/** \brief CAN ID域掩码定义 */
#define AW_CAN_STD_IDS_MASK                  0x7FF      /**< \brief CAN标准帧ID号掩码 */
#define AW_CAN_EXT_IDS_MASK                  0x1FFFFFFF /**< \brief CAN扩展帧ID号掩码 */

/** \brief CAN 操作的超时定义 */
#define AW_CAN_WAIT_FOREVER                  AW_WAIT_FOREVER /**< \brief CAN永久等待 */
#define AW_CAN_NO_WAIT                       AW_NO_WAIT      /**< \brief CAN无等待 */

/** \brief CAN消息的标志位定义 */
typedef uint32_t aw_can_msg_flag_t;                    /**< \brief CAN标志 */
#define AW_CAN_MSG_FLAG_SEND_NORMAL          0x0000    /**< \brief 正常发送 */
#define AW_CAN_MSG_FLAG_SEND_ONCE            0x0001    /**< \brief 单次发送 */
#define AW_CAN_MSG_FLAG_SEND_SELF            0x0002    /**< \brief 自发自收 */
#define AW_CAN_MSG_FLAG_SEND_SELFONCE        0x0003    /**< \brief 单次自发自收 */
#define AW_CAN_MSG_FLAG_CAN_CTRL             0x0000    /**< \brief 普通CAN报文 */
#define AW_CAN_MSG_FLAG_FD_CTRL              0x0010    /**< \brief CANFD报文 */
#define AW_CAN_MSG_FLAG_REMOTE               0x0100    /**< \brief 远程帧标记 */
#define AW_CAN_MSG_FLAG_EXTERND              0x0200    /**< \brief 扩展帧标记 */
#define AW_CAN_MSG_FLAG_ERROR                0x0400    /**< \brief 错误标记 */
#define AW_CAN_MSG_FLAG_BRS                  0x0800    /**< \brief 切换波特率 */
#define AW_CAN_MSG_FLAG_ESI                  0x1000    /**< \brief 发送端被动错误*/

/** \brief 消息标志中发送类型掩码 */
#define AW_CAN_MSG_FLAG_SEND_TYPE_MASK       0x000F    
/** \brief 消息标志中报文类型掩码 */
#define AW_CAN_MSG_FLAG_CTRL_TYPE_MASK       0x00F0    

/** \brief 消息标志中发送类型的偏移位 */
#define AW_CAN_MSG_FLAG_SEND_TYPE_SHIFT      0x0000    
/** \brief 消息标志中报文类型的偏移位 */
#define AW_CAN_MSG_FLAG_CTRL_TYPE_SHIFT      0x0004    

/** \brief CAN消息报文头定义 */
typedef struct aw_can_msg {
    uint32_t timestamp;       /**< \brief 时间戳 */
    uint32_t id;              /**< \brief 帧ID */

    aw_can_msg_flag_t flags;  /**< \brief 报文信息、标志 */

    uint8_t resv[2];          /**< \brief 保留用于后续扩展 */

    uint8_t chn;              /**< \brief 报文所在通道号 */
    uint8_t length;           /**< \brief 报文长度 */
} aw_can_msg_t;

/** \brief CAN标准报文定义 */
typedef struct aw_can_std_msg {
    aw_can_msg_t      can_msg;                       /**< \brief CAN 报文信息头 */
    uint8_t           msgbuff[AW_CAN_HS_MAX_LENGTH]; /**< \brief CAN 报文数据 */
} aw_can_std_msg_t;

/** \brief CANFD报文定义 */
typedef struct aw_can_fd_msg {
    aw_can_msg_t      can_msg;                       /**< \brief CANFD 报文信息头 */
    uint8_t           msgbuff[AW_CAN_FD_MAX_LENGTH]; /**< \brief CAN 报文数据 */
} aw_can_fd_msg_t;

/** \brief CAN传输块 */
typedef struct aw_can_transfer {
    aw_can_msg_t     *p_msgs;         /**< \brief 传输块的CAN报文缓冲器首地址*/
    uint16_t          msgs_num;       /**< \brief 请求传输报文个数 */
    uint16_t          done_num;       /**< \brief 成功处理的传输报文个数 */
    aw_pfuncvoid_t    pfunc_complete; /**< \brief 传输完成回调函数,异步传输有效*/
    void             *p_arg;          /**< \brief 传递给 pfunc_complete 的参数 */
    
    /**
     * \brief 当前传输块的状态如下:
     *                -AW_ENOTCONN   :未排队
     *                -AW_EISCONN    :排队
     *                -AW_EINPROGRESS:处理
     *                -AW_ETIME      :超时
     *                -AW_ENETDOWN   :总线关闭，设备停止
     *                 AW_OK         :完成
     */
    int               status;         
    void             *ctlrdata[2];    /**< \brief 控制器使用 */
} aw_can_transfer_t;

/**
 * \brief CAN标准波特率定义
 *
 * ----------------------------------------------------------
 * | sync |        tseg1      |   tseg2                     |
 * |                     sample point                       |
 * ----------------------------------------------------------
 * f(tq)= fpclk(can外设时钟) / brp（分频值）
 *
 * baudrate = ftq / (sync + tseg1 + tseg2)
 *
 * sync: 同步段 sync = 1
 *
 * sjw: 同步跳转宽度
 *
 * tseg1: 相位缓冲段1: tseg1，Tseg1请详细参考CAN控制器解释
 *
 * tseg2: 相位缓冲段2: tseg2，tseg2 >= sjw
 *
 * smp: 采样次数: 控制器采样总线次数，通常设置为单次采样
 *
 * brp: 分频值
 *
 */
typedef struct aw_can_baud_param {
    uint8_t     tseg1;          /**< \brief tseg1 相位段1 */
    uint8_t     tseg2;          /**< \brief tseg2 相位段2 */
    uint8_t     sjw;            /**< \brief sjw 同步跳转宽度 */
    uint8_t     smp;            /**< \brief smp 采样模式0:单次 1:三次 */
    uint16_t    brp;            /**< \brief brp 分频值 */
} aw_can_baud_param_t;

/** \brief CANFD波特率定义 */
typedef struct aw_can_fd_baud_param {
    aw_can_baud_param_t nominal_baud;  /**< \brief 仲裁域及CRC域等波特率 */
    aw_can_baud_param_t data_baud;     /**< \brief 数据域波特率 */
} aw_can_fd_baud_param_t;

/** \brief 错误计数器定义 */
typedef struct aw_can_err_cnt{
    uint8_t rx_err_cnt;         /**< \brief rx_error_cnt  接收错误计数器 */
    uint8_t tx_err_cnt;         /**< \brief tx_error_cnt  发送错误计数器 */
} aw_can_err_cnt_t;

/** \brief 时间戳获取的回调函数 */
typedef uint32_t (*aw_can_timestamp_get_callback_t) (uint32_t);

/** \brief 接收数据的回调函数 */
typedef aw_bool_t (*aw_can_proto_rx_callback_t) (int chn, aw_can_msg_t* p_msg);

/** \brief 发送数据的回调函数 */
typedef aw_bool_t (*aw_can_proto_tx_callback_t) (int chn);

/** \brief 总线错误状态的回调函数 */
typedef aw_bool_t (*aw_can_bus_err_sta_callback_t) (
        int chn, aw_can_bus_err_t err, aw_can_bus_status_t status);

/** \brief 设备唤醒的回调函数 */
typedef aw_bool_t (*aw_can_dev_wakeup_callback_t) (int chn);

/** \brief 应用可配置的回调函数 */
typedef struct aw_can_app_callbacks {

    /** \brief  获取系统时间戳回调函数地址  */
    aw_can_timestamp_get_callback_t  timestamp_get_cb;

    /**
     * \brief  总线接收一帧报文回调函数地址
     * 函数返回值决定接收到的报文是否存入rngbuf， AW_TRUE:不保存，FALSE:保存
     */
    aw_can_proto_rx_callback_t       proto_rx_cb;

    /**
     * \brief  总线发送完成回调函数地址
     * 函数返回值决定是否自动处理发送报文， 
     *              AW_TRUE :回调自行处理，
     *              AW_FALSE:自动处理发送
     */
    aw_can_proto_tx_callback_t       proto_tx_cb;

    /**
     * \brief  应用层错误及总线状态处理回调函数地址
     * 函数返回值决定错误消息是否存入rngbuf， 
     *              AW_TRUE :不保存
     *              AW_FALSE:保存
     */
    aw_can_bus_err_sta_callback_t    bus_err_sta_cb;

    /**
     * \brief  总线唤醒回调函数地址
     * 函数返回值决定唤醒消息是否存入rngbuf
     *              AW_TRUE :不保存
     *              AW_FALSE:保存
     */
    aw_can_dev_wakeup_callback_t     dev_wakeup_cb;

} aw_can_app_callbacks_t;

/**
 * \brief CAN初始化函数，初始化通道，模式，波特率，回调函数等操作
 *
 * \param[in] chn        选择通道，chn: 0~AW_CFG_NUM_CAN
 *
 * \param[in] work_mode  设置 工作模式，
 *                       AW_CAN_WORK_MODE_NORMAL(正常模式)、
 *                       AW_CAN_WORK_MODE_LISTEN_ONLY（只听模式）
 *
 * \param[in] p_baud     配置波特率，波特率参数参考结构体说明。
 *                       控制器类型为CAN时，波特率参数为aw_can_baud_param_t
*                        控制器类型为CANFD时， 波特率参数为aw_can_fd_baud_param_t
 *
 * \param[in] p_app_cb   用户回调，发生相应的事件，回调用户APP，可设置为NULL
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_init (int                        chn,
                          aw_can_work_mode_type_t    work_mode,
                          const aw_can_baud_param_t *p_baud,
                          aw_can_app_callbacks_t    *p_app_cb);

/**
 * \brief 启动CAN控制器，使CAN控制器进入正常模式，可以进行通信
 *
 * \param[in] chn           通道号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_start (int chn);

/**
 * \brief 停止CAN控制器，使CAN控制器进入配置模式，CAN控制器不进行总线通信
 *
 * \param[in] chn           通道号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_stop (int chn);

/**
 * \brief 将CAN控制器进入睡眠模式
 *
 * \param[in] chn           通道号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_sleep (int chn);

/**
 * \brief 唤醒CAN控制器
 *
 * \param[in] chn           通道号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_wakeup (int chn);

/**
 * \brief 设置CAN波特率
 *
 * \attention 标准CAN时，波特率参数为aw_can_baud_param_t
 * \attention CANFD时，波特率参数为aw_can_fd_baud_param_t
 *
 * \param[in] chn           通道号
 *
 * \param[in] p_baud        波特率参数
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_baudrate_set (int chn, aw_can_baud_param_t *p_baud);

/**
 * \brief 获取CAN波特率
 *
 * \attention 标准CAN时，波特率参数为aw_can_baud_param_t
 * \attention CANFD时，波特率参数为aw_can_fd_baud_param_t
 *
 * \param[in] chn           通道号
 *
 * \param[in] p_baud        波特率参数
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_baudrate_get (int chn, aw_can_baud_param_t *p_baud);

/**
 * \brief 清除发送和接受错误计数
 *
 * \param[in] chn           通道号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_err_cnt_clr (int chn);

/**
 * \brief 获取发送和接收错误计数器
 *
 * \param[in] chn       通道号
 *
 * \param[in] p_err_cnt 错误寄存器值指针
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_err_cnt_get (int chn, aw_can_err_cnt_t *p_err_cnt);

/**
 * \brief 设置CAN滤波表。
 *
 * \note 此接口函数为通用格式，即只有表首地址和表长度（字节个数）。
 *       不同的控制器滤波表不同，格式也不同，因此具体控制器滤波格式
 *       请参考对应控制器例程。
 *
 * \param[in] chn           通道号。
 *
 * \param[in] p_filter_buff 滤波表首地址
 *
 * \param[in] length        滤波表长度（字节个数）
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_filter_table_set (
        int chn, uint8_t *p_filter_buff, size_t length);

/**
 * \brief 获取CAN滤波表。
 *
 * \note 此接口函数为通用格式，即只有表首地址和表长度（字节个数）。
 *       不同的控制器滤波表不同，格式也不同，因此具体控制器滤波格式
 *       请参考对应控制器例程。
 *
 * \param[in] chn           通道号
 *
 * \param[in] p_filter_buff 滤波表首地址
 *
 * \param[in] p_length      返回滤波表长度（字节个数）
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_filter_table_get (int       chn, 
                                      uint8_t  *p_filter_buff,
                                      size_t   *p_length);

/**
 * \brief 设置CAN传输结构体参数
 *
 * \param p_transfer        CAN报文传输描述符指针
 * \param p_msgs            组成传输块的报文
 * \param msgs_num          报文的个数
 * \param pfunc_complete    传输完成回调函数
 *                          (仅调用 aw_can_msgs_send_async() 时此参数有效,
 *                          调用 aw_can_msgs_send_sync()时本参数无效)
 * \param p_arg             传递给回调函数的参数
 *
 * \return 无
 *
 * \par 范例
 * \code
 * #include "aw_can.h"
 *
 * aw_can_transfer_t transfer;  // 传输块描述结构体
 * aw_can_std_msg_t  msgs[2];   // 组成传输块的报文缓冲区
 *
 * // 设置传输描述结构体
 * aw_i2c_mktrans(&transfer,    // 传输块描述结构
 *                msgs,         // 组成传输块的报文缓冲区首地址
 *                2，                       // 传输个数
 *                my_callback,  // 传输完成回调函数
 *                my_arg);      // 传输完成回调函数的参数
 *
 *  函数实现：
 *  p_transfer->p_msgs         = (aw_can_msg_t*)p_msgs;
 *  p_transfer->msgs_num       = msgs_num;
 *  p_transfer->done_num       = 0;
 *  p_transfer->pfunc_complete = pfunc_complete;
 *  p_transfer->p_arg          = p_arg;
 *  p_transfer->status         = -AW_ENOTCONN;
 *
 * \endcode
 */
 void aw_can_mkmsg (aw_can_transfer_t  *p_transfer,
                    aw_can_msg_t       *p_msgs,
                    uint16_t            msgs_num,
                    aw_pfuncvoid_t      pfunc_complete,
                    void               *p_arg);

/**
 * \brief CAN报文异步传输
 *
 * \note 产生一次CAN报文的异步传输，接口参数使用传输块，
 *       如果当前有传输块正在传输则把新传输块添加到发送列表，
 *       用户的传输块必须是有效传输块，未传输结束之前不能释放该传输块，否则发送失败。
 *       传输块如何制作请参考函数aw_can_mkmsg内容。
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] p_transfer    传输块p_transfer的地址
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_msgs_send_async (int chn, aw_can_transfer_t *p_transfer);

/**
 * \brief CAN报文同步传输
 *
 * \note 产生一次同步传输，发送一批CAN报文，报文个数由用户给出
 *       用户的传输块必须是有效传输块，未传输结束之前不能释放该传输块，否则发送失败。
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] p_transfer    传输块地址
 *
 * \param[in] timeout       传输超时间
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_msgs_send_sync (int                chn,
                                    aw_can_transfer_t *p_transfer,
                                    uint32_t           timeout);

/**
 * \brief CANFD控制器发送报文
 *
 * \note CANFD控制器的报文发送函数，只有配置为CANFD控制器的时候使用，
 *       报文格式采用CANFD报文格式，请参考：aw_can_fd_msg_t
 *       用户的传输块必须是有效传输块，未传输结束之前不能释放改传输块，否则发送
 *       失败。
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] p_msgs        FD报文缓冲区地址
 *
 * \param[in] msgs_num      缓冲区报文个数
 *
 * \param[in] p_done_num    实际发送成功个数地址
 *
 * \param[in] timeout       发送超时时间，须大于0
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_fd_msgs_send (int              chn,
                                  aw_can_fd_msg_t *p_msgs,
                                  uint32_t         msgs_num,
                                  uint32_t        *p_done_num,
                                  uint32_t         timeout);

/**
 * \brief 高速CAN控制器发送报文
 *
 * \note 高速CAN控制器的报文发送函数，只有配置为高速CAN控制器的时候使用，
 *       报文格式采用标准CAN报文格式，请参考：aw_can_std_msg_t
 *       用户的传输块必须是有效传输块，未传输结束之前不能释放改传输块，否则发送失败。
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] p_msgs        标准报文缓冲区地址
 *
 * \param[in] msgs_num      缓冲区报文个数
 *
 * \param[in] p_done_num    实际发送成功个数地址
 *
 * \param[in] timeout       发送超时时间，须大于0
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_std_msgs_send (int               chn,
                                   aw_can_std_msg_t *p_msgs,
                                   uint32_t          msgs_num,
                                   uint32_t         *p_done_num,
                                   uint32_t          timeout);

/**
 * \brief 停止发送函数
 *
 * \attention 用于异步传输停止发送，用户调用此接口aw_can_msgs_send_async。
 *            使用同步传输，发生超时事件，系统会自动调用停止传输，用户无需调用。
 *            用户会停止当前报文发送，并清除传输块列表。
 *
 * \param[in] chn           CAN通道号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_msgs_stop_send (int chn);

/**
 * \brief 获取接收缓冲区已经接收can报文的个数，
 *
 * \param[in] chn       CAN通道号
 *
 * \return frame count  缓冲区报文个数
 *
 */
uint32_t aw_can_rcv_msgs_num (int chn);

/**
 * \brief 缓冲区只大小。
 *
 * \param[in] chn      CAN通道号
 *
 * \return  缓冲区实际大小（报文个数）
 *
 */
uint32_t aw_can_rcv_fifo_size (int chn);

/**
 * \brief 接收CAN报文
 *
 * \note 接收CAN报文，缓冲区只要有数据就会成功返回，
 *       实际个数值在aw_can_transfer_t的元素done_num中，
 *       只有缓冲区没有数据的时候，超时时间有效，
 *       为了保证接收数据实时性，在超时等待时候，当接收到有效数据，立刻返回。
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] p_transfer    传输块地址
 *
 * \param[in] timeout       接收缓冲区中无报文的超时时间
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_msgs_rcv (int                chn,
                              aw_can_transfer_t *p_transfer,
                              uint32_t           timeout);

/**
 * \brief 高速CAN控制器接收报文
 *
 * \note 高速CAN控制器的报文接收函数，缓冲区只要有数据就会成功返回，
 *       只有配置为高速CAN控制器的时候使用
 *       实际个数值在存放在p_done_num中，
 *       只有缓冲区没有数据的时候，超时时间有效，
 *       为了保证接收数据实时性，在超时等待时候，当接收到有效数据，立刻返回。
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] p_msgs        接收报文缓冲区地址
 *
 * \param[in] msgs_num      缓冲区报文个数
 *
 * \param[in] p_done_num    实际读取个数地址
 *
 * \param[in] timeout       接收缓冲区中无报文的超时时间
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_std_msgs_rcv (int               chn,
                                  aw_can_std_msg_t *p_msgs,
                                  uint32_t          msgs_num,
                                  uint32_t         *p_done_num,
                                  uint32_t          timeout);

/**
 * \brief CANFD控制器接收报文
 *
 * \note CANFD控制器的报文接收函数，缓冲区只要有数据就会成功返回，
 *       只有配置为CANFD控制器的时候使用
 *       实际个数值在存放在p_done_num中，
 *       只有缓冲区没有数据的时候，超时时间有效，
 *       为了保证接收数据实时性，在超时等待时候，当接收到有效数据，立刻返回。
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] p_msgs        接收FD报文缓冲区地址
 *
 * \param[in] msgs_num      缓冲区报文个数
 *
 * \param[in] p_done_num    实际读取个数地址
 *
 * \param[in] timeout       无报文超时时间
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_fd_msgs_rcv (int              chn,
                                 aw_can_fd_msg_t *p_msgs,
                                 uint32_t         msgs_num,
                                 uint32_t        *p_done_num,
                                 uint32_t         timeout);

/**
 * \brief 写CAN寄存器
 *
 * \note 写CAN寄存器，适用于CAN驱动框架不包含的，特殊的CAN操作
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] offset        寄存器地址
 *
 * \param[in] p_data        内容
 *
 * \param[in] length        长度
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_reg_write (int        chn,
                               uint32_t   offset,
                               uint8_t   *p_data,
                               size_t     length);

/**
 * \brief 读取CAN寄存器
 *
 * \note 读取CAN寄存器，适用于CAN驱动框架不包含的，特殊的CAN操作
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] offset        寄存器地址
 *
 * \param[in] p_data        读取内容
 *
 * \param[in] length        读取长度
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_reg_read (int        chn,
                              uint32_t   offset,
                              uint8_t   *p_data,
                              size_t     length);
/**
 * \brief 向CAN寄存器写报文，
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] p_msg         报文指针
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_reg_msg_write (int chn, aw_can_msg_t *p_msg);

/**
 * \brief 读取CAN报文
 *
 * 从CAN寄存器中读取报文
 *
 * \param[in] chn           CAN通道号
 *
 * \param[in] p_msg         报文指针
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 *
 */
aw_can_err_t aw_can_reg_msg_read (int chn, aw_can_msg_t *p_msg);

/** @} grp_aw_if_can */

#ifdef __cplusplus
}
#endif

#endif /* __AW_CAN_H */

/* end of file */
