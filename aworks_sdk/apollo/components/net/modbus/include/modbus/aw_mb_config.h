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
 * \brief Modbus用户配置文件
 *
 * \internal
 * \par modification history
 * - 1.03 2019-03-26  cml, use serial interface replace int serial.
 * - 1.02 2015-05-13  cod, Modify for aworks1.0, fix code style
 * - 1.01 2012-12-04  liuweiyun, Modify for aworks1.0, fix code style
 * - 1.00 2012-05-13  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_CONFIG_H /* { */
#define __AW_MB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AIXO_AWORKS
#include "aw_modbus_autoconf.h"
#endif

/**
 * \addtogroup grp_aw_if_mb_config
 * \copydoc aw_mb_config.h
 * @{
 */

/** \note 本文件所有配置为默认配置，生成库后，直接修改宏定义无效 */

/** \brief 从机通信模式配置 */

/** \brief 串口RTU模式 */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_SERIAL_RTU_ENABLE
#define AW_MB_SERIAL_RTU_ENABLE    1
#else
#define AW_MB_SERIAL_RTU_ENABLE    0
#endif
#else
#define AW_MB_SERIAL_RTU_ENABLE    1
#endif

/** \brief 串口ASCII模式 */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_SERIAL_ASCII_ENABLE
#define AW_MB_SERIAL_ASCII_ENABLE    1
#else
#define AW_MB_SERIAL_ASCII_ENABLE    0
#endif
#else
#define AW_MB_SERIAL_ASCII_ENABLE    1
#endif

/** \brief TCP/IP模式 */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_TCP_ENABLE
#define AW_MB_TCP_ENABLE    1
#else
#define AW_MB_TCP_ENABLE    0
#endif
#else
#define AW_MB_TCP_ENABLE    1
#endif


/** \brief 主站通信模式配置 */

/** \brief 串口RTU模式 */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_MASTER_RTU_ENABLE
#define AW_MB_MASTER_RTU_ENABLE    1
#else
#define AW_MB_MASTER_RTU_ENABLE    0
#endif
#else
#define AW_MB_MASTER_RTU_ENABLE    1
#endif


/** \brief 串口ASCII模式 */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_MASTER_RTU_ENABLE
#define AW_MB_MASTER_ASCII_ENABLE    1
#else
#define AW_MB_MASTER_ASCII_ENABLE    0
#endif
#else
#define AW_MB_MASTER_ASCII_ENABLE    1
#endif


/** \brief TCP/IP模式 */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_MASTER_TCP_ENABLE
#define AW_MB_MASTER_TCP_ENABLE    1
#else
#define AW_MB_MASTER_TCP_ENABLE    0
#endif
#else
#define AW_MB_MASTER_TCP_ENABLE    1
#endif


/**
 * \brief 串行最大ADU长度
 * - 该长度包括从机地址和检验数据长度
 * - 对于某些应用，可能不需要很长的一帧数据，可以考虑适当剪裁以降低RAM使用
 * - RTU模式长度为4~256， ASCII模式长度为3~255
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_MAX_SER_ADU_LENGTH)
#define AW_MB_MAX_SER_ADU_LENGTH    CONFIG_AW_MB_MAX_SER_ADU_LENGTH
#else
#define AW_MB_MAX_SER_ADU_LENGTH    256
#endif

/**
 * \brief TCP模式下最大ADU长度
 * - 对于某些应用，可能不需要很长的一帧数据，可以考虑适当剪裁以降低RAM使用
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_MAX_TCP_ADU_LENGTH)
#define AW_MB_MAX_TCP_ADU_LENGTH    CONFIG_AW_MB_MAX_TCP_ADU_LENGTH
#else
#define AW_MB_MAX_TCP_ADU_LENGTH    263
#endif

/**
 * \brief RTU模式T3.5时间设置（单位：微秒）
 * - 在RTU模式中，隐含着对t3.5（帧间延迟时间）引起的大量中断的处理，
 *   在较高的通信波特率下，这将导致CPU负担加重，
 *   因此当波特率小于或等于19200位/秒的情况下，必须严格遵守这个定时
 * - 而当波特率大于19200位/秒时，应该使用固定值，建议帧间延迟时间设置为1.750ms，
 * - 当使用ASCII模式时，不需要对该宏进行配置
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_SERIAL_RTU_BAUDRATE_OVER_19200_FRAME_DELAY3_5_US)
#define AW_MB_SERIAL_RTU_BAUDRATE_OVER_19200_FRAME_DELAY3_5_US    CONFIG_AW_MB_SERIAL_RTU_BAUDRATE_OVER_19200_FRAME_DELAY3_5_US
#else
#define AW_MB_SERIAL_RTU_BAUDRATE_OVER_19200_FRAME_DELAY3_5_US    1750ul
#endif

/**
 * \brief ASCII模式字符间超时设置（单位：ms）
 * 在ASCII模式中，字符间超时时间没有固定，可以根据具体需求来设置
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_ASCII_TIMEOUT_SEC)
#define AW_MB_ASCII_TIMEOUT_SEC    CONFIG_AW_MB_ASCII_TIMEOUT_SEC
#else
#define AW_MB_ASCII_TIMEOUT_SEC    1000
#endif

/**
 * \brief 用户可扩展注册的最大功能码个数
 * 配置使用aw_mb_slave_register_handler函数来注册功能码操作函数时可以注册最大个数
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_FUNC_CODE_HANDLER_MAX)
#define AW_MB_FUNC_CODE_HANDLER_MAX    CONFIG_AW_MB_FUNC_CODE_HANDLER_MAX
#else
#define AW_MB_FUNC_CODE_HANDLER_MAX    4
#endif

/**
 * \brief 默认从站允许接收连接数（当超过默认连接数时，关闭最早建立的连接）
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_SLAVE_TCP_CONNECT_NUM)
#define AW_MB_SLAVE_TCP_CONNECT_NUM    CONFIG_AW_MB_SLAVE_TCP_CONNECT_NUM
#else
#define AW_MB_SLAVE_TCP_CONNECT_NUM    4
#endif

/**
 * \brief 主站等待从站应答超时时间（毫秒）
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_MASTER_TIMEOUT_MS_RESPOND)
#define AW_MB_MASTER_TIMEOUT_MS_RESPOND    CONFIG_AW_MB_MASTER_TIMEOUT_MS_RESPOND
#else
#define AW_MB_MASTER_TIMEOUT_MS_RESPOND    200
#endif

#if ((AW_MB_MASTER_TIMEOUT_MS_RESPOND < 50)  ||\
     (AW_MB_MASTER_TIMEOUT_MS_RESPOND > 100000))
#error "AW_MB_MASTER_TIMEOUT_MS_RESPOND value must be in 50~100000ms"
#endif

/**
 * \brief 主站广播时的转换延迟时间（毫秒）
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_MASTER_DELAY_MS_CONVERT)
#define AW_MB_MASTER_DELAY_MS_CONVERT    CONFIG_AW_MB_MASTER_DELAY_MS_CONVERT
#else
#define AW_MB_MASTER_DELAY_MS_CONVERT    100
#endif

/**
 * \brief 主站用户可扩展注册的最大功能码个数
 * 配置使用aw_mb_master_funcode_register函数来注册功能码操作函数时可以注册最大个数
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_MASTER_FUNCODE_HANDLER_MAX)
#define AW_MB_MASTER_FUNCODE_HANDLER_MAX    CONFIG_AW_MB_MASTER_FUNCODE_HANDLER_MAX
#else
#define AW_MB_MASTER_FUNCODE_HANDLER_MAX   4
#endif

/**
 * \brief TCP模式下单元标示符使用从站地址使能
 * - 对于某些应用，如Modbus服务器连接到串行链路，可使用单元标示符识别从站地址
 * - 若使能，单元标示符当做目标从站地址使用；若不使能，单元标示符值默认为0xFF
 */
#ifdef AXIO_AWORKS
#ifdef CONFIG_AW_MB_MASTER_RTU_ENABLE
#define AW_MB_TCP_UID_AS_SLAVEADDR    1
#else
#define AW_MB_TCP_UID_AS_SLAVEADDR    0
#endif
#ifndef AXIO_AWORKS 
#define AW_MB_TCP_UID_AS_SLAVEADDR    1
#endif
#endif


/**
 * \brief 主站TCP模式下，默认连接超时时间(ms)
 */
#if defined(AXIO_AWORKS) && defined(CONFIG_AW_MB_TCP_CONNECT_TIMEOUT)
#define AW_MB_TCP_CONNECT_TIMEOUT    CONFIG_AW_MB_TCP_CONNECT_TIMEOUT
#else
#define AW_MB_TCP_CONNECT_TIMEOUT   5000
#endif

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_config */

#endif /* } __AW_MB_CONFIG_H */


/* end of file */
