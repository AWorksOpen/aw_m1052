/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief CANopen从机接口
 *
 * \note 协议栈正常使用需要以下3个步骤:
 * step1: initialize CANopen stack   => aw_cop_slv_init
 * step2: slave connect to the net   => aw_cop_slv_connect_net
 * step3: CANopen process task start => aw_cop_slv_process
 *
 * \internal
 * \par modification history:
 * - 1.01 19-12-02  anu, modify
 * - 1.00 15-11-16  cae, first implementation
 * \endinternal
 */

#ifndef __AW_COP_SLV_H
#define __AW_COP_SLV_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_cop_slv
 * \copydoc aw_cop_slv.h
 * @{
 */

#include "aw_cop.h"

/**
 * \name CANopen 设置从站状态的命令
 * \anchor grp_aw_cop_slv_status_cmd_t
 * @{
 */

/** \brief 设置从站状态的命令 */
typedef uint32_t aw_cop_slv_status_cmd_t;

/** \brief  启动从站 */
#define AW_COP_SLV_STATUS_CMD_INIT                  0

/** \brief  使从站进入预操作状态 */
#define AW_COP_SLV_STATUS_CMD_OPERATIONAL           1

/** \brief  停止从站 */
#define AW_COP_SLV_STATUS_CMD_STOP                  2

/** \brief  使从站进入预工作状态 */
#define AW_COP_SLV_STATUS_CMD_PRE_OPERATIONAL       128

/** \brief  复位从站  */
#define AW_COP_SLV_STATUS_CMD_RESET                 129

/** \brief  复位从站通信  */
#define AW_COP_SLV_STATUS_CMD_RESET_COMMUNICATION   130

/** @} */

/**
 * \name CANopen 从站状态
 * \anchor grp_aw_cop_slv_status_t
 * @{
 */

/** \brief 从站状态 */
typedef uint32_t aw_cop_slv_status_t;

/** \brief 从站处于初始化状态 */
#define AW_COP_SLV_STATUS_INIT                0

/** \brief 从站处于停止状态 */
#define AW_COP_SLV_STATUS_STOP                4

/** \brief 从站处于操作状态 */
#define AW_COP_SLV_STATUS_OPERATIONAL         5

/** \brief 从站处于预操作状态 */
#define AW_COP_SLV_STATUS_PRE_OPERATIONAL     127

/** @} */

/** \brief CANopen 从站信息 */
typedef struct aw_cop_slv_info {

    uint32_t node_id;          /**< \brief 节点ID */
    uint32_t baudrate;         /**< \brief CAN波特率 单位kbps */

    const uint8_t  *p_seg_cont;      /**< \brief 容器段，描述所有对象的构建规则 */
    uint32_t        seg_cont_size;   /**< \brief 容器段大小 */

    uint8_t        *p_process_image;      /**< \brief 过程镜像 */
    uint32_t        process_image_size;   /**< \brief 过程镜像大小 */

} aw_cop_slv_info_t;

/**
 * \brief  aw_cop_slv_init
 *  初始化CANopen从站
 *
 * \param[in] inst:   CANopen实例号
 * \param[in] p_info: 初始化参数
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_init (uint8_t inst, aw_cop_slv_info_t *p_info);

/**
 * \brief  aw_cop_slv_deinit
 *  解初始化CANopen从站
 *
 * \param[in] inst: CANopen实例号
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_deinit (uint8_t inst);

/**
 * \brief  aw_cop_slv_connect_net
 *  连接到CANopen网络
 *
 * \param[in] inst: CANopen实例号
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_connect_net (uint8_t inst);

/**
 * \brief  aw_cop_slv_boot
 *  从站自动启动，无需主站操作，遵循CANopen规范不建议使用
 *
 * \param[in] inst: CANopen实例号
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_boot (uint8_t inst);

/**
 * \brief  aw_cop_slv_status_set
 *  设置从站状态
 *
 * \param[in] inst:   CANopen实例号
 * \param[in] status: 控制类型，参见\ref grp_aw_cop_slv_status_cmd_t
 *
 * \retval aw_cop_err_t:      见函数返回错误码。
 *
 */
aw_cop_err_t aw_cop_slv_status_set (
    uint8_t inst, aw_cop_slv_status_cmd_t status);

/**
 * \brief  aw_cop_slv_status_get
 *  获取从站的状态
 *
 * \param[in]  inst:     CANopen实例号
 * \param[out] p_status: 从站状态，参见\ref grp_aw_cop_slv_status_t
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_status_get (
    uint8_t inst, aw_cop_slv_status_t *p_status);

/**
 * \brief  aw_cop_slv_obd_read
 *  CANopen内部对象字典读取
 *
 * \param[in]     inst:     CANopen实例号
 * \param[in]     index:    服务数据索引号
 * \param[in]     subindex: 服务数据子索引号
 * \param[out]    p_data:   接收数据缓冲区
 * \param[in/out] p_length: 接收数据大小，返回实际接收到的数据
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_slv_obd_read (uint8_t   inst,
                                  uint16_t  index,
                                  uint8_t   subindex,
                                  uint8_t  *p_data,
                                  uint32_t *p_length);

/**
 * \brief  aw_cop_slv_obd_write
 *  CANopen内部对象字典读取
 *
 * \param[in] inst:     CANopen实例号
 * \param[in] index:    服务数据索引号
 * \param[in] subindex: 服务数据子索引号
 * \param[in] p_data:   发送数据缓冲区
 * \param[in] length:   发送数据长度
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 *
 */
aw_cop_err_t aw_cop_slv_obd_write (uint8_t   inst,
                                   uint16_t  index,
                                   uint8_t   subindex,
                                   uint8_t  *p_data,
                                   uint32_t  length);

/**
 * \brief  aw_cop_slv_tpdo_send
 *  CANopen 发送数据
 *
 * \param[in] inst:     CANopen实例号
 * \param[in] offset:   发送过程镜像偏移
 * \param[in] p_data:   发送数据缓冲区
 * \param[in] length:   发送数据长度
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 *
 */
aw_cop_err_t aw_cop_slv_tpdo_send (uint8_t   inst,
                                   uint32_t  offset,
                                   uint8_t  *p_data,
                                   uint32_t  length);

/**
 * \brief  aw_cop_slv_rpdo_read
 *  CANopen 读取数据
 *
 * \param[in]  inst:     CANopen实例号
 * \param[in]  offset:   接收过程镜像偏移
 * \param[out] p_data:   读取数据缓冲区
 * \param[in]  length:   读取数据长度
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 *
 */
aw_cop_err_t aw_cop_slv_rpdo_read (uint8_t   inst,
                                   uint32_t  offset,
                                   uint8_t  *p_data,
                                   uint32_t  length);

/**
 * \brief  aw_cop_slv_process
 *  CANopen处理函数
 *
 * \param[in] inst: CANopen实例号
 */
void aw_cop_slv_process (uint8_t inst);

/** @} grp_aw_if_cop_slv */

#ifdef __cplusplus
}
#endif

#endif /* __AW_COP_SLV_H */

/* end of file */
