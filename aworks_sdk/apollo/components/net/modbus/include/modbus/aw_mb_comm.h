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
 * \brief modbus公共信息
 *
 * \internal
 * \par modification history
 * - 1.03 2019-03-26  cml, use serial interface replace int serial.
 * - 1.02 2015-05-18  cod, fix code style and comments, a little improve.
 * - 1.01 2013-03-29  liuweiyun, fix code style and comments, a little improve.
 * - 1.00 2012-05-03  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_COMM_H /* { */
#define __AW_MB_COMM_H

#include "modbus/aw_mb_compiler.h"

/**
 * \addtogroup grp_aw_if_mb_comm
 * \copydoc aw_mb_comm.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

/**
 * \name Modbus地址
 * @{
 */
#define AW_MB_ADDRESS_BROADCAST    0   /**< \brief Modbus广播地址 */
#define AW_MB_ADDRESS_MIN          1   /**< \brief 最小从机地址 */
#define AW_MB_ADDRESS_MAX          247 /**< \brief 最大从机地址 */
/** @} */

/**
 * \name 常用功能码
 * @{
 */
#define AW_MB_FUNC_NONE                           0   /**< \brief 不使用的功能码 */
#define AW_MB_FUNC_READ_COILS                     1   /**< \brief 读线圈功能码 */
#define AW_MB_FUNC_READ_DISCRETE_INPUTS           2   /**< \brief 读离散量功能码 */
#define AW_MB_FUNC_READ_HOLDING_REGISTER          3   /**< \brief 读保存寄存器功能码 */
#define AW_MB_FUNC_READ_INPUT_REGISTER            4   /**< \brief 读输入寄存器功能码 */
#define AW_MB_FUNC_WRITE_SINGLE_COIL              5   /**< \brief 写单个线圈功能码 */
#define AW_MB_FUNC_WRITE_REGISTER                 6   /**< \brief 写单个寄存器功能码 */
#define AW_MB_FUNC_WRITE_MULTIPLE_COILS          15   /**< \brief 写多个线功能码 */
#define AW_MB_FUNC_WRITE_MULTIPLE_REGISTERS      16   /**< \brief 写多个寄存器功能码 */
#define AW_MB_FUNC_READWRITE_MULTIPLE_REGISTERS  23   /**< \brief 读写多个寄存器功能码 */
#define AW_MB_FUNC_OTHER_REPORT_SLAVEID          17   /**< \brief 获取从机信息 */

#define AW_MB_FUNC_ERROR                         128  /**< \brief 异常功能码 */
/** @} */

/**
 * \name PDU defines
 * @{
 */
#define AW_MB_PDU_SIZE_MAX       253 /**< \brief 最大PDU长度 */
#define AW_MB_PDU_SIZE_MIN       1   /**< \brief 功能码长度 */
#define AW_MB_PDU_FUNC_OFF       0   /**< \brief 功能码在PDU中偏移 */
#define AW_MB_PDU_DATA_OFF       1   /**< \brief 数据在PDU中偏移 */
/** @} */

/**
 * \name Modbus TCP MBAP defines
 * @{
 */
#define AW_MB_TCP_TID            0    /**< \brief 事务处理标识符 */
#define AW_MB_TCP_PID            2    /**< \brief 协议标识符 */
#define AW_MB_TCP_LEN            4    /**< \brief 长度 */
#define AW_MB_TCP_UID            6    /**< \brief 单元标识符 */
#define AW_MB_TCP_FUNC           7    /**< \brief 功能码 */
/** @} */

/**
 * \name PID define
 * @{
 */
#define AW_MB_TCP_PROTOCOL_ID    0    /**< \brief 协议标识符 ID*/
/** @} */

/**
 * \name  TCP pseudo address define
 * @{
 */
#define AW_MB_TCP_PSEUDO_ADDRESS 255       /**< \brief Modbus TCP伪地址 */
/** @} */
/******************************************************************************/

/** \brief 异常类型 */
typedef enum aw_mb_exception {
    AW_MB_EXP_NONE                 = 0x00,     /**< \brief 没有异常 */
    AW_MB_EXP_ILLEGAL_FUNCTION     = 0x01,     /**< \brief 非法功能 */
    AW_MB_EXP_ILLEGAL_DATA_ADDRESS = 0x02,     /**< \brief 非法数据地址 */
    AW_MB_EXP_ILLEGAL_DATA_VALUE   = 0x03,     /**< \brief 非法数据值 */
    AW_MB_EXP_SLAVE_DEVICE_FAILURE = 0x04,     /**< \brief 从机设备故障 */
    AW_MB_EXP_ACKNOWLEDGE          = 0x05,     /**< \brief 确认 */
    AW_MB_EXP_SLAVE_BUSY           = 0x06,     /**< \brief 从机设备忙 */
    AW_MB_EXP_MEMORY_PARITY_ERROR  = 0x08,     /**< \brief 存储奇偶性差错*/
    AW_MB_EXP_GATEWAY_PATH_FAILED  = 0x0A,     /**< \brief 网关路径不可用 */
    AW_MB_EXP_GATEWAY_TGT_FAILED   = 0x0B      /**< \brief 网关目标设备响应失败 */
} aw_mb_exception_t;

/** \brief 串口奇偶校验模式 */
enum aw_mb_parity {
    AW_MB_PAR_NONE,    /**< \brief 不校验 */
    AW_MB_PAR_ODD,     /**< \brief 奇校验 */
    AW_MB_PAR_EVEN     /**< \brief 偶校验 */
};

/** \brief 通信模式类型 */
enum aw_mb_mode {
    AW_MB_RTU,         /**< \brief 基于RTU模式的串口MODBUS   */
    AW_MB_ASCII,       /**< \brief 基于ASCII模式的串口MODBUS */
    AW_MB_TCP,         /**< \brief 基于TCP/IP协议栈的MODBUS  */
};

/******************************************************************************/

/** \brief 基于串口从站的MODBUS，初始化参数 */
struct aw_mb_serial_param {
    uint8_t           slave_addr;    /**< \brief 从机地址 */
    enum aw_mb_parity parity;        /**< \brief 串口校验模式 */
    uint32_t           port;          /**< \brief 串口号 */
    uint32_t          baud_rate;     /**< \brief 串口波特率 */
};

/** \brief 基于串口的MODBUS主站，初始化参数 */
typedef struct aw_mb_master_serial_params {
    uint32_t          port;          /**< \brief 串口号 */
    enum aw_mb_parity parity;        /**< \brief 串口校验模式 */
    uint32_t          baud_rate;     /**< \brief 串口波特率 */
} aw_mb_master_serial_params_t;

/** \brief Modbus TCP客户端的初始化参数 */
typedef struct aw_mb_tcp_client_params {
    char    *ip_addr;      /**< \brief 目标IP地址，如“192.168.28.111” */
    uint16_t port;         /**< \brief 目标端口 */
} aw_mb_tcp_client_params_t;

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_comm */

#endif /* } __AW_MB_COMM_H */

/* end of file */
