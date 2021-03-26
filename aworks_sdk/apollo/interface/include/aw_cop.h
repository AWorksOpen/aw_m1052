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
 * \brief CANopen主机、从机公共文件
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-31  anu, modify
 * - 1.00 15-11-16  cae, first implementation
 * \endinternal
 */

#ifndef __AW_COP_H
#define __AW_COP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_cop
 * \copydoc aw_cop.h
 * @{
 */

#include "aw_types.h"

/**
 * \name CANopen 信息
 * \anchor grp_aw_cop_max_slaves
 * @{
 */

#define AW_COP_MAX_SLAVES         127    /**< \brief 最大从机数量 */

/** @} */

/**
 * \name CANopen 节点
 * \anchor grp_aw_cop_node_id
 * @{
 */

#define AW_COP_NODE_ID_MAX        0x7F   /**< \brief 最大节点ID */
#define AW_COP_NODE_ID_INVAILD    0x00   /**< \brief 无效节点ID */
#define AW_COP_NODE_ID_ALL        0x00   /**< \brief 所有节点ID */

/** @} */

/**
 * \name CANopen ID
 * \anchor grp_aw_cop_id
 * @{
 */

#define AW_COP_ID_SYNC_CLEAR    0x80000080L /**< \brief 清除同步报文ID */
#define AW_COP_ID_SYNC          0x00000080L /**< \brief 同步报文ID */
#define AW_COP_ID_TIME_STAMP    0x100       /**< \brief 时间戳报文ID */
#define AW_COP_ID_SDO_SND       0x580       /**< \brief 服务数据对象发送ID */
#define AW_COP_ID_SDO_RCV       0x600       /**< \brief 服务数据对象发送ID */

/** @} */

/**
 * \name CANopen 错误代码
 * \anchor grp_aw_cop_err_t
 * @{
 */

typedef int aw_cop_err_t;               /**< \brief 错误代码 */
#define AW_COP_ERR_NO_ERROR         0   /**< \brief 没有错误 */
#define AW_COP_ERR_LOADLIB          11  /**< \brief 装载DLL失败 */
#define AW_COP_ERR_GETPROC          12  /**< \brief 获取函数地址失败 */
#define AW_COP_ERR_OPENED           13  /**< \brief 设备已经被打开 */
#define AW_COP_ERR_NOTEXIST         14  /**< \brief 设备不存在 */
#define AW_COP_ERR_INITFAIL         15  /**< \brief 初始化设备失败 */
#define AW_COP_ERR_STARTDEV         16  /**< \brief 启动CANopen设备失败 */
#define AW_COP_ERR_NOTOPEN          17  /**< \brief 设备没有打开 */
#define AW_COP_ERR_INVALIDPARAM     18  /**< \brief 无效参数 */
#define AW_COP_ERR_INVALIDHANDLE    19  /**< \brief 无效的设备句柄 */
#define AW_COP_ERR_CLOSEDEV         20  /**< \brief 无法关闭设备 */
#define AW_COP_ERR_INSTALLDRIVER    22  /**< \brief 驱动安装不正确 */
#define AW_COP_ERR_BUFFERTOOSMALL   23  /**< \brief 存储空间太小 */
#define AW_COP_ERR_INTERNAL         29  /**< \brief 未知内部错误 */
#define AW_COP_ERR_TIMEOUT          30  /**< \brief 等待超时 */
#define AW_COP_ERR_SLAVEEXIST       31  /**< \brief 从站已存在 */
#define AW_COP_ERR_SLAVENOTEXIST    32  /**< \brief 从站不存在 */
#define AW_COP_ERR_SLAVETABFUL      33  /**< \brief 从站注册已满 */
#define AW_COP_ERR_SENDFAILED       34  /**< \brief 发送数据失败 */
#define AW_COP_ERR_NODATA           35  /**< \brief 没有数据 */
#define AW_COP_ERR_GETSTATUS        36  /**< \brief 获取从站状态失败 */
#define AW_COP_ERR_NOTIMPLEMENT     37  /**< \brief 函数没有实现 */
#define AW_COP_ERR_NOTSTARTED       38  /**< \brief 未启动CANopen */
#define AW_COP_ERR_SDOABORT         39  /**< \brief SDO 传输中止 */
#define AW_COP_ERR_PDOTABFULL       40  /**< \brief PDO 注册已满 */
#define AW_COP_ERR_PDONOTREGISTER   41  /**< \brief PDO 未注册 */
#define AW_COP_ERR_CHNLNOTINIT      42  /**< \brief 未初始化通道参数 */
#define AW_COP_ERR_QUEUECREATE      43  /**< \brief 创建队列失败 */
#define AW_COP_ERR_NOSEGCONT        44  /**< \brief 无容器段 */
#define AW_COP_ERR_NOMEM            45  /**< \brief 无可用内存 */
#define AW_COP_ERR_OFFSET           46  /**< \brief 偏移错误 */
#define AW_COP_ERR_LENGTH           47  /**< \brief 长度错误 */

/** @} */

/** \brief CANopen PDO过程数据对象消息 */
typedef struct aw_cop_pdo_msg {

    uint8_t  node_id;          /**< \brief 从站地址 */
    uint32_t pdo_id;           /**< \brief PDO类型  */
    uint8_t  pdo_len;          /**< \brief PDO数据长度 */
    uint8_t  pdo_data[8];      /**< \brief PDO数据 */

} aw_cop_pdo_msg_t;

/** \brief CANopen 紧急报文数据 */
typedef struct aw_cop_emcy_msg {

    uint8_t  node_id;          /**< \brief 从站地址 */
    uint16_t err_code;         /**< \brief 紧急错误代码 */
    uint8_t  err_reg_code;     /**< \brief 当前错误寄存器值 */
    uint8_t  err_specific[5];  /**< \brief 自定义错误代码 */

} aw_cop_emcy_msg_t;

/** \brief CANopen 时间 */
typedef struct aw_cop_time {

    uint16_t year;             /**< \brief 年 */
    uint8_t  month;            /**< \brief 月 */
    uint8_t  day;              /**< \brief 日 */
    uint8_t  hour;             /**< \brief 小时 */
    uint8_t  minute;           /**< \brief 分钟 */
    uint8_t  second;           /**< \brief 秒 */
    uint16_t millisecond;      /**< \brief 毫秒 */

} aw_cop_time_t;

/** \brief CANopen 波特率 */
typedef uint32_t aw_cop_baud_t;
#define AW_COP_BAUD_1M      1000  /**< \brief 1M 波特率 */
#define AW_COP_BAUD_800K    800   /**< \brief 800K 波特率 */
#define AW_COP_BAUD_500K    500   /**< \brief 500K 波特率 */
#define AW_COP_BAUD_250K    250   /**< \brief 250K 波特率 */
#define AW_COP_BAUD_125K    125   /**< \brief 125K 波特率 */
#define AW_COP_BAUD_100K    100   /**< \brief 100K 波特率 */
#define AW_COP_BAUD_50K     50    /**< \brief 50K 波特率 */
#define AW_COP_BAUD_20K     20    /**< \brief 20K 波特率 */
#define AW_COP_BAUD_10K     10    /**< \brief 10K 波特率 */

/** @} grp_aw_if_cop */

#ifdef __cplusplus
}
#endif

#endif /* __AW_COP_H */

/* end of file */
