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
 * \brief modbus从机协议栈用户接口头文件
 *
 * \internal
 * \par modification history
 * - 1.05 2019-03-26  cml, use serial interface replace int serial.
 * - 1.04 2016-09-02  cod, support muti-tcp.
 * - 1.03 2016-04-27  cod, support muti-instance.
 * - 1.02 2015-05-22  cod, fix code style and comments, a little improve.
 * - 1.01 2013-03-29  liuweiyun, fix code style and comments, a little improve.
 * - 1.00 2012-05-03  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_SLAVE_H /* { */
#define __AW_MB_SLAVE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_slave
 * \copydoc aw_mb_slave.h
 * @{
 */

#include "modbus/aw_mb_config.h"
#include "modbus/aw_mb_err.h"
#include "modbus/aw_mb_comm.h"

/** \brief 功能码回调函数类型 */
enum aw_mb_func_cb_type {
    AW_MB_FUNC_COILS_CALLBACK,                /**< \brief 读写线圈操作回调 */
    AW_MB_FUNC_DISCRETEINPUTS_CALLBACK,       /**< \brief 读离散量操作回调 */
    AW_MB_FUNC_HOLDREGISTERS_CALLBACK,        /**< \brief 读写保存寄存器回调 */
    AW_MB_FUNC_INPUTREGISTERS_CALLBACK,       /**< \brief 读输入寄存器回调 */
};

/** \brief 功能码回调函数操作类型 */
enum aw_mb_func_cb_op {
    AW_MB_FUNC_CALLBACK_READ,   /**< \brief 读操作 */
    AW_MB_FUNC_CALLBACK_WRITE,  /**< \brief 写操作 */
};

/** \brief Modbus从站句柄定义 */
typedef void *aw_mb_slave_t;

/******************************************************************************/

/**
 * \file
 *
 * callback function
 *
 * Read the ON/OFF status of discrete outputs(DO)in the slave. The coils status
 * is packed as per bit of data field. Status is indicated as: 1=ON, 0=OFF.
 * The LSB of the first data byte contains the coil addressed in the query.
 * If the coil quantity is not a multiple of eight, the remaining bits in the
 * final data byte will be padded with zeros.
 *
 * For example. addr = 0x0013  num = 10,
 * status of coils 20 ~ 27 is ON-ON-OFF-OFF-ON-OFF-ON-OFF 28~29 ON-ON,
 * so the buf[0] = 01010011(0x53)  buf[1] = 00000011
 *
 * buf[0]:
 * 7  |6  |5  |4  |3  |2  |1  |0
 * ---|---|---|---|---|---|---|---
 * 27 |26 |25 |24 |23 |22 |21 |20
 * OFF|ON |OFF|ON |OFF|OFF|ON |ON
 * 0  |1  |0  |1  |0  |0  |1  |1
 *
 * buf[1]:
 * 7  |6  |5  |4  |3  |2  |1  |0
 * ---|---|---|---|---|---|---|----
 * -  |-  |-  |-  |-  |-  |29 |28
 * 0  |0  |0  |0  |0  |0  |ON |ON
 * 0  |0  |0  |0  |0  |0  |1  |1
 */

/**
 * \brief 读写线圈，读离散量，读写保存寄存器，读输入寄存器回调函数原型
 * \note  当使用本库提供的读写线圈，读离散量，读写保存寄存器，
 *        读输入寄存器等功能码处理函数时，需要注册
 *
 * \par 示例：
 * \code
 * aw_mb_exception_t my_coils_callback(aw_mb_slave_t slave,
 *                                     uint8_t   *buf,
 *                                     uint16_t   addr,
 *                                     uint16_t   num)
 * {
 *     //读线圈操作
 * }
 *
 * //在启动slave之前进行如下操作：
 * aw_mb_slave_register_callback(slave,
 *                               AW_MB_FUNC_COILS_CALLBACK,
 *                               AW_MB_FUNC_CALLBACK_READ,
 *                               my_coils_callback);
 * \endcode
 *
 * \param[in]     slave  从机句柄
 * \param[in,out] buf    读写操作缓冲区
 * \param[in]     addr   读写操作开始地址
 * \param[in]     num    读写操作个数
 *
 * \return  如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 * \see aw_mb_slave_register_callback
 */
typedef aw_mb_exception_t (*aw_mb_slave_fn_code_callback_t)(
                            aw_mb_slave_t  slave,
                            uint8_t       *p_buf,
                            uint16_t       addr,
                            uint16_t       num);

/**
 * \brief 注册读写线圈，读离散量，读写保存寄存器，读输入寄存器回调函数.
 * \note  当使用本库提供的读写线圈，读离散量，读写保存寄存器，读输入寄存器功能码
 *        处理函数时, 必须使用该函数进行回调函数注册，否则主机会获得非法功能码异常
 *
 * \param[in] slave     从机句柄
 * \param[in] type      功能码回调函数类型
 * \param[in] op        功能码回调函数操作类型
 * \param[in] callback  回调函数
 *
 * \retval AW_MB_ERR_NOERR   注册成功.
 * \retval AW_MB_ERR_EINVAL  传入参数错误.
 */
aw_mb_err_t aw_mb_slave_register_callback (
            aw_mb_slave_t                  slave,
            enum aw_mb_func_cb_type        type,
            enum aw_mb_func_cb_op          op,
            aw_mb_slave_fn_code_callback_t callback);

/**
 * \brief 功能码处理函数原型.
 * \note  当从机收到一帧数据以后，将根据功能码调用对应的处理函数。
 *        处理函数由用户通过\a aw_mb_slave_register_handler 来注册。
 *        同时已经定义基本的读写线圈，读离散量，读写保存寄存器，读输入寄存器
 *        等处理函数，具体参考\a aw_mb_regfunction.h
 *
 * \param[in]     slave     Modbus从站句柄
 * \param[in,out] p_pdubuf  PDU数据(包含功能码)
 *                          - [in]  请求PDU数据
 *                          - [out] 应答PDU数据
 * \param[in,out] p_pdulen  PDU数据长度
 *                          - [in]  请求PDU数据长度
 *                          - [out] 应答PDU数据长度
 *
 * \return 如果没有异常，返回AW_MB_EXP_NONE，否则返回对应的异常号
 */
typedef aw_mb_exception_t (*aw_mb_fn_code_handler_t) (aw_mb_slave_t  slave,
                                                      uint8_t       *p_pdubuf,
                                                      uint16_t      *p_pdulen);

/******************************************************************************/
/**
 * \brief 创建并初始化一个Modbus从站
 * \note  在调用所有从机函数之前，必须先调用该函数
 * \note  允许不同网络创建多个从站
 *
 * \param[in] mode     通信模式
 * \param[in] p_param  初始化参数，对于MB_RTU/MB_ASCII模式使用
 *                     \a struct aw_mb_serial_param 作为参数
 *                     对于AW_MB_TCP模式，param作为端口号参数使用,直接传入形参
 * \param[out] p_err   错误号. 如果对错误号不感兴趣可以传递AW_MB_NULL参数。
 *                     返回错误值：
 *                     - AW_MB_ERR_NOERR           : 初始化成功.
 *                     - AW_MB_ERR_EINVAL          : 传入参数错误.
 *                     - AW_MB_ERR_ALLOC_FAIL      : 内存不足，分配从站失败.
 *                     - AW_MB_ERR_MODE_NO_SUPPORT : 模式不支持
 *                     - AW_MB_ERR_EPORTERR        : 串行模式-分配串口或硬件定时器失败.
 *                     - AW_MB_ERR_EIO             : TCP模式-服务端创建失败.
 * \return Modbus从站句柄，如果发生错误返回NULL
 *
 * \note 1.使用RTU模式时默认的串口数据位为8，停止位为1。
 *       2.使用ASCII模式时默认的串口数据位为7，停止位为1，
 *         这是因为常用ASCII码最大值为128，7位足矣。
 */
aw_mb_slave_t aw_mb_slave_init (enum aw_mb_mode  mode,
                                void            *p_param,
                                aw_mb_err_t     *p_err);

/**
 * \brief 关闭Modbus从站.
 * \note  退出系统或者不需要再使用从机时调用
 *
 * \param[in] slave  Modbus从站句柄
 *
 * \retval AW_MB_ERR_NOERR      无错误.
 * \retval AW_MB_ERR_EINVAL     从站参数错误.
 * \retval AW_MB_ERR_EILLSTATE  协议栈非法状态.
 */
aw_mb_err_t aw_mb_slave_close (aw_mb_slave_t slave);

/**
 * \brief 启动Modbus从站.
 * \note  该函数必须在调用\a aw_mb_slave_init 函数后或者调用\a aw_mb_slave_stop 函数后被调用
 *
 * \param[in] slave  Modbus从站句柄
 *
 * \retval AW_MB_ERR_NOERR      无错误.
 * \retval AW_MB_ERR_EINVAL     从站参数错误.
 * \retval AW_MB_ERR_EILLSTATE  协议栈非法状态.
 */
aw_mb_err_t aw_mb_slave_start (aw_mb_slave_t slave);

/**
 * \brief 停止Modbus从站.
 *
 * \param[in] slave  Modbus从站句柄
 *
 * \retval AW_MB_ERR_NOERR      无错误.
 * \retval AW_MB_ERR_EINVAL     从站参数错误.
 * \retval AW_MB_ERR_EILLSTATE  协议栈非法状态.
 */
aw_mb_err_t aw_mb_slave_stop (aw_mb_slave_t slave);

/**
 * \brief 注册功能码处理函数.
 * \note  初始化Modbus从站以后，可以调用该函数来注册对应的功能码处理函数。用来扩展功能码
 *
 * \note 可以注册的功能码处理函数个数在\a aw_mb_config.h 中 AW_MB_FUNC_CODE_HANDLER_MAX 定义。
 *       超过该个数将无法再注册。同一个功能码只能有一个处理函数，如果注册两次同
 *       一个功能码处理函数，将只使用最后注册的处理函数。
 *
 * \param[in] slave    Modbus从站句柄
 * \param[in] funcode  功能码号
 * \param[in] handler  功能码处理函数
 *
 * \retval AW_MB_ERR_NOERR   无错误;
 * \retval AW_MB_ERR_EINVAL  参数错误;
 * \retval AW_MB_ERR_ENORES  无法再注册功能码.
 */
aw_mb_err_t aw_mb_slave_register_handler (aw_mb_slave_t            slave,
                                          uint8_t                  funcode,
                                          aw_mb_fn_code_handler_t  handler);

/**
 * \brief Modbus从站查询操作.
 * \note  接收数据帧并处理该数据帧，其为Modbus从站协议的核心。该函数必须被周期性的调用。
 *        其时间间隔依赖具体应用和主站要求的时间间隔。
 * \note  该接口允许RTU、ASCII、TCP模式.
 * \note  RTU/ASCII串行模式下，该接口阻塞时间取决于\a aw_mb_config.h 协议栈配置接口。
 *        TCP模式下，该接口为非阻塞。
 *
 * \param[in] slave  Modbus从站句柄
 *
 * \retval AW_MB_ERR_NOERR      无错误;
 * \retval AW_MB_ERR_EINVAL     参数错误;
 * \retval AW_MB_ERR_EILLSTATE  协议栈非法状态.
 * \retval AW_MB_ERR_ENORES     串行模式：响应时收到新的请求.
 * \retval AW_MB_ERR_EIO        IO错误. 串行模式：发送状态出错. TCP模式：发送失败.
 */
aw_mb_err_t aw_mb_slave_poll (aw_mb_slave_t slave);

/** \brief 从站配置类型 */
typedef int aw_mbs_cfg_type_t;
#define AW_MBS_TYPE_USER_FUNCODE       0x01   /**< \brief 用户自定义功能码数配置  */
#define AW_MBS_TYPE_TCP_NUM            0x02   /**< \brief TCP模式允许建立连接数配置 */
#define AW_MBS_TYPE_ACCEPT_TCP_NUM_GET 0x03   /**< \brief 获取当前已接收的连接数 */
#define AW_MBS_TYPE_ALLOW_NEW_TCP      0x04   /**< \brief 连接达到设置值后，允许关闭旧连接来接收新连接 */

/**
 * \brief 从站配置设置
 *
 * \note  若无调用该接口，则配置类型按照\a aw_mb_config.h 中参数设置.
 *
 * \param[in] slave  Modbus从站句柄
 * \param[in] type   配置类型：
 *                   - AW_MBS_TYPE_USER_FUNCODE : 用户自定义功能码数，参数p_arg为功能码数（uint8_t）;
 *                   - AW_MBS_TYPE_TCP_NUM      : 允许建立连接数，参数p_arg为连接数（uint32_t）;
 *                   - AW_MBS_TYPE_ACCEPT_TCP_NUM_GET : 获取当前已接收连接数，参数p_arg为获取的连接数（uint32_t *）;
 *                   - AW_MBS_TYPE_ALLOW_NEW_TCP : 达到连接上限后，允许接收新连接而断开旧连接,p_arg为1时允许;
 * \param[in] p_arg  配置参数，取决于请求命令
 *
 * \return 若设置成功，返回AW_MB_ERR_NOERR，反之，返回相应错误值。
 */
aw_mb_err_t aw_mb_slave_ioctl (aw_mb_slave_t     slave,
                               aw_mbs_cfg_type_t type,
                               void             *p_arg);

/**
 * \brief 从站模式获取
 *
 * \param[in]      slave  Modbus从站句柄
 * \param[in,out] p_mode  从站工作模式
 *
 * \retval AW_MB_ERR_EINVAL  传入参数错误
 * \retval AW_MB_ERR_NOERR   无错误
 */
aw_mb_err_t aw_mb_slave_mode_get (aw_mb_slave_t slave, enum aw_mb_mode *p_mode);

/**
 * \brief 设置Modbus从站信息。
 *        用来设置Modbus从站类型说明，当前状态和其他一些特殊信息。
 *
 * \note 本实现定义了Modbus从站信息最大长度为32个字节，其中：
 *       - 栈类型说明占一个字节
 *       - 当前状态占一个字节
 *       - 其他信息长度 <= 30个信息 \n
 *       在使用是要特别注意。
 * \note 该接口支持需配置，默认不支持
 *
 * \param[in] slave         Modbus从站句柄
 * \param[in] slave_id      类型说明
 * \param[in] is_running    当前状态
 * \param[in] p_additional  其他一些特殊信息
 * \param[in] alen          其他一些特殊信息长度
 *
 * \retval AW_MB_ERR_NOERR   无错误;
 * \retval AW_MB_ERR_EINVAL  参数错误;
 * \retval AW_MB_ERR_ENORES  资源不足.
 */
aw_mb_err_t aw_mb_slave_set_id (aw_mb_slave_t  slave,
                                uint8_t        slave_id,
                                uint8_t        is_running,
                                const uint8_t *p_additional,
                                uint16_t       alen);

/**
 * \brief 设置Modbus从站地址.
 * \note  默认情况下在调用\a aw_mb_slave_init 进行初始化时，初始化参数里面已经有设置从站地址；
 *        当如果有需要在运行时修改从站地址，可以使用该函数。
 *
 * \param[in] slave  Modbus从站句柄
 * \param[in] addr   新的Modbus从站地址
 *
 * \retval AW_MB_ERR_NOERR   无错误;
 * \retval AW_MB_ERR_EINVAL  参数错误; 从站不存在，地址超过最大地址， 或设置地址为广播地址
 */
aw_mb_err_t aw_mb_slave_set_addr (aw_mb_slave_t slave, uint8_t addr);

struct sockaddr; /**< \brief 套接字地址 */

/** \brief 接收连接TCP信息 */
struct aw_mb_slave_tcp_info {
    int              socket;      /**< \brief 接收socket，用与设置socket属性  */
    struct sockaddr *p_sockaddr;  /**< \brief IP地址信息  */
};

/**
 * \brief 从站TCP模式，接收连接钩子函数原型
 * \note  服务端接收连接调用该钩子函数.
 *
 * \param[in] p_arg   用户自定义参数
 * \param[in] p_info  接收的连接信息
 *
 * \retval AW_TRUE   允许接收该连接
 * \retval AW_FALSE  拒绝接收该连接
 */
typedef aw_bool_t (*aw_mb_tcp_accept_hook_t) (void                        *p_arg,
                                              struct aw_mb_slave_tcp_info *p_info);

/**
 * \brief 从站TCP模式，连接断开钩子函数原型
 * \note  已接收的连接异常断开，
 *        或连接达到上限值（当设置为达到上限值，允许接收新连接时）主动断开时调用该钩子函数
 *
 * \param[in] p_arg   用户自定义参数
 * \param[in] socket  断开连接的socket
 *
 * \return 无
 */
typedef void (*aw_mb_tcp_close_hook_t) (void *p_arg, int socket);

/**
 * \brief TCP模式socket连接钩子函数注册
 *
 * \param[in] slave        Modbus从站句柄
 * \param[in] accept_hook  接收新连接钩子函数
 * \param[in] close_hook   连接被动断开，或达到连接上限断开处理钩子函数
 * \param[in] p_arg        钩子函数参数
 *
 * \retval AW_MB_ERR_NOERR      注册成功；
 * \retval AW_MB_ERR_EINVAL     参数错误；
 * \retval AW_MB_ERR_EILLSTATE  协议栈非法状态
 */
aw_mb_err_t aw_mb_slave_tcp_hook_register (aw_mb_slave_t            slave,
                                           aw_mb_tcp_accept_hook_t  accept_hook,
                                           aw_mb_tcp_close_hook_t   close_hook,
                                           void                    *p_arg);

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_slave */

#endif /* } __AW_MB_SLAVE_H */

/* end of file */
