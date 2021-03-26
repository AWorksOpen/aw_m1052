/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief modbus netif header file
 *
 * \internal
 * \par modification history
 * - 1.04 2019-03-26  cml, use serial interface replace int serial.
 * - 1.03 2016-04-27  cod, support muti-instance.
 * - 1.02 2015-05-21  cod, fix code style and comments, a little improve.
 * - 1.01 2013-03-29  liuweiyun, fix code style and comments, a little improve.
 * - 1.00 2012-05-03  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_NETIF_H /* { */
#define __AW_MB_NETIF_H

#include "aw_mb_regfunction.h"
#include "aw_mb_port.h"
#include "aw_spinlock.h"
#include "aw_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_netif
 * \copydoc aw_mb_netif.h
 * @{
 */

/** \brief 从机ID信息缓冲区 */
#define AW_MB_FUNC_OTHER_REP_SLAVEID_BUF  32

/******************************************************************************
 * RTU模式私有数据
 ******************************************************************************/
/** \brief RTU私有数据  */
struct aw_mb_rtu {
    uint8_t  slave_addr;   /**< \brief slave address */
    uint16_t snd_data_len; /**< \brief send data length */
    uint8_t *snd_data;     /**< \brief send data buffer */

    /** \brief pdu buffer */
    uint8_t  pdu_buf[AW_MB_MAX_SER_ADU_LENGTH];

    /** \brief receive data count */
    uint16_t rcv_data_pos;

    /** \brief serial id */
    uint32_t serial_id;

    /**< \brief T3.5 time */
    uint32_t      t35_time;
};
/******************************************************************************
 * ASCII模式私有数据
 ******************************************************************************/
/** \brief ASCII码高低字节标志  */
enum aw_mb_byte_pos {
    AW_BYTE_HIGH_NIBBLE,   /**< \brief Character for high nibble of byte. */
    AW_BYTE_LOW_NIBBLE     /**< \brief Character for low nibble of byte. */
};

/** \brief ASCII私有数据  */
struct aw_mb_ascii {
    uint8_t  slave_addr;    /**< \brief slave address  */
    uint8_t  high_nibble;   /**< \brief =1 high nibble  =0 low nibble  */
    uint16_t snd_data_len;  /**< \brief send data length */
    uint8_t *p_snd_data;    /**< \brief send data buffer */

    /** \brief recv data count  */
    uint16_t rcv_pos;

    /** \brief pdu buffer */
    uint8_t  adu_buffer[AW_MB_MAX_SER_ADU_LENGTH];

    /** \brief serial id  */
    uint32_t  serial_id;
};

/******************************************************************************
 * TCP模式私有数据
 ******************************************************************************/
/** \brief modbus tcp private data */
struct aw_mb_tcp {
    /** \brief TCP数据缓冲 */
    uint8_t         tcp_buf[AW_MB_MAX_TCP_ADU_LENGTH];
    uint16_t        tcp_buf_pos;         /**< \brief 缓冲区数据当前位置 */
    uint16_t        tcp_frame_bytesleft; /**< \brief 需要接收数据长度 */
    uint16_t        port;                /**< \brief 端口号 */
    aw_bool_t       is_accept_new;       /**< \brief 连接达到设置值后是否允许允许接收新连接 */

    int             listen_socket;       /**< \brief 侦听socket */
    int             cur_socket;          /**< \brief 当前操作socket编号  */
    int             socket_num;          /**< \brief 允许建立连接的socket数  */
    int             accept_num;          /**< \brief 当前连接数  */
    int            *p_sockets;           /**< \brief socket槽  */

    aw_mb_tcp_accept_hook_t accept_hook; /**< \brief TCP接收连接钩子函数  */
    aw_mb_tcp_close_hook_t  close_hook;  /**< \brief TCP关闭连接钩子函数  */
    void                   *p_arg;       /**< \brief TCP连接钩子函数参数  */

    aw_spinlock_isr_t isr_lock;          /**< \brief 自旋锁 */
};
/******************************************************************************
 * 从站对象
 ******************************************************************************/
/** \brief slave status type */
enum aw_mb_slave_status {
    AW_STATE_UNUSE,   /**< \brief slave unused state */
    AW_STATE_INIT,    /**< \brief slave initial state */
    AW_STATE_DISABLE, /**< \brief slave disable state */
    AW_STATE_ENABLE,  /**< \brief slave enable state */
};

/** \brief data-link common interface */
struct aw_mb_netif {
    /** \brief 获取从站地址 */
    uint8_t     (*slave_addr_get)(void *p_cookie);
    /** \brief 设置从站地址 */
    aw_mb_err_t (*slave_addr_set)(void *p_cookie, uint8_t addr);
    /** \brief 关闭Modbus从站 */
    void        (*close)(void *p_cookie);
    /** \brief 启动Modbus从站 */
    void        (*start)(void *p_cookie);
    /** \brief 停止Modbus从站 */
    void        (*stop) (void *p_cookie);
    /** \brief 从站接收数据 */
    aw_mb_err_t (*recv) (void      *p_cookie,
                         uint8_t   *slave_addr,
                         uint8_t  **pp_pdu,
                         uint16_t  *p_len);
    /** \brief 从站发送数据 */
    aw_mb_err_t (*send) (void *p_cookie, uint8_t *p_pud, uint16_t len);
};

/** \brief slave date type */
struct aw_mb_slave_struct {
    enum aw_mb_slave_status   status;           /**< \brief 从站状态  */
    enum aw_mb_mode           mode;             /**< \brief 当前工作模式 */
    const struct aw_mb_netif *p_netif;          /**< \brief 模式接口 */
    void                     *p_cookie;         /**< \brief 模式接口句柄 */

    aw_funcode_handle        *p_hdl_tbl_head;   /**< \brief 用户注册功能码表头 */
    uint8_t                   user_funcode_num; /**< \brief 用户功能码数  */

    /** \brief 通用功能码回调  */
    aw_mb_slave_fn_code_callback_t fn_read_coils_cb;  /**< \brief 读线圈回调函数 */
    aw_mb_slave_fn_code_callback_t fn_write_coils_cb; /**< \brief 写线圈回调函数 */
    aw_mb_slave_fn_code_callback_t fn_read_discre_cb; /**< \brief 读离散量回调函数 */
    aw_mb_slave_fn_code_callback_t fn_read_input_cb;  /**< \brief 读输入寄存器回调函数 */
    aw_mb_slave_fn_code_callback_t fn_read_hold_cb;   /**< \brief 读保持寄存器回调函数 */
    aw_mb_slave_fn_code_callback_t fn_write_hold_cb;  /**< \brief 写保持寄存器回调函数 */
#if 0
    /** \brief 用户ID信息，可不定义  */
    uint8_t  id[AW_MB_FUNC_OTHER_REP_SLAVEID_BUF];
    uint16_t id_len; /**< \brief ID信息长度 */
#endif

    aw_spinlock_isr_t lock; /**< \brief 自旋锁操作保护  */
};
/******************************************************************************/
/**
 * \brief Modbus RTU模式接口配置初始化
 *
 * \param[in] pp_cookie  模式句柄
 * \param[in] p_param    初始化参数,转换为\a struct aw_mb_serial_param 作为参数
 *
 * \return modbus 错误号，如果没有错误，其值为AW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_netif_rtu_init (void **pp_cookie, void *p_param);

/**
 * \brief Modbus ASCII模式接口配置初始化
 *
 * \param[in] pp_cookie  模式句柄
 * \param[in] p_param    初始化参数,转换为\a struct aw_mb_serial_param 作为参数
 *
 * \return modbus 错误号，如果没有错误，其值为AW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_netif_ascii_init (void **pp_cookie, void *p_param);

/**
 * \brief Modbus TCP模式接口配置初始化
 *
 * \param[in] pp_cookie  模式句柄
 * \param[in] p_param    初始化参数,转换为\a struct aw_mb_serial_param 作为参数
 *
 * \return modbus 错误号，如果没有错误，其值为AW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_netif_tcp_init (void **pp_cookie, void *p_param);

/**
 * \brief 打开Modbus配置接口，配置相应模式参数,注册功能函数
 *
 * \param[in] p_slave  从站设备实例
 * \param[in] mode     Modbus通信模式
 * \param[in] p_param  初始化参数，对于AW_MB_RTU/AW_MB_ASCII模式使用
 *                     \a struct aw_mb_serial_param 作为参数
 *                     对于AW_MB_TCP模式，作为端口号参数使用
 *
 * \return modbus 错误号，如果没有错误，其值为AW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_netif_open (struct aw_mb_slave_struct *p_slave,
                              enum aw_mb_mode            mode,
                              void                      *p_param);

/**
 * \brief 分配用户功能码
 *
 * \param[in] p_slave  从站设备实例.
 * \param[in] num      分配个数
 *
 * \return modbus 错误号，如果没有错误，其值为AW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mbs_user_funcode_alloc (struct aw_mb_slave_struct *p_slave,
                                       uint8_t                    num);

/**
 * \brief 设置TCP连接数
 *
 * \param[in] p_cookie  TCP句柄
 * \param[in] tcp_num   建立连接数
 *
 * \return modbus 错误号，如果没有错误，其值为AW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_netif_tcp_num_set (void *p_cookie, uint16_t tcp_num);

/**
 * \brief 获取已建立的连接数
 *
 * \param[in] p_cookie  Modbus TCP设备句柄
 *
 * \return 已接收的连接数
 */
aw_static_inline uint16_t aw_mb_netif_tcp_num_get (void *p_cookie)
{
    return ((struct aw_mb_tcp *)p_cookie)->accept_num;
}

/**
 * \brief 是否允许在连接满后关闭旧连接，接收新连接
 *
 * \param[in] p_cookie  Modbus TCP设备句柄
 * \param[in] is_allow  是否允许.
 *
 * \return 无
 */
void aw_mb_netif_allow_new_tcp (void *p_cookie, aw_bool_t is_allow);

/**
 * \brief TCP钩子函数注册
 *
 * \param[in] p_cookie     modbus tcp 私有数据
 * \param[in] accept_hook  接收连接钩子函数
 * \param[in] close_hook   断开连接钩子函数
 * \param[in] p_arg        钩子函数参数
 *
 * \return 无
 */
void aw_mb_netif_tcp_hook_register (void                    *p_cookie,
                                    aw_mb_tcp_accept_hook_t  accept_hook,
                                    aw_mb_tcp_close_hook_t   close_hook,
                                    void                    *p_arg);

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_netif */

#endif /* } __AW_MB_NETIF_H */

/* end of file */
