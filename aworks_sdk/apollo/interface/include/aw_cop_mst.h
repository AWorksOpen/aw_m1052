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
 * \brief CANopen主机接口
 *
 * \note 协议栈正常使用需要以下5个步骤:
 * step1: initialize CANopen stack   => aw_cop_mst_init
 * step2: register node              => aw_cop_mst_add_node
 * step3: register pdo               => aw_cop_mst_input_pdo_install
 * step4: CANopen process task start => aw_cop_mst_process
 * step5: CANopen stack start        => aw_cop_mst_start
 *
 * \internal
 * \par modification history:
 * - 1.01 18-05-31  anu, modify
 * - 1.00 15-11-16  cae, first implementation
 * \endinternal
 */

#ifndef __AW_COP_MST_H
#define __AW_COP_MST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_cop_mst
 * \copydoc aw_cop_mst.h
 * @{
 */

#include "aw_cop.h"

/**
 * \name CANopen 主站设置从站节点状态的命令
 * \anchor grp_aw_cop_mst_node_status_cmd_t
 * @{
 */

/** \brief 主站设置从站节点状态的命令 */
typedef uint32_t aw_cop_mst_node_status_cmd_t;

/** \brief  启动从站 */
#define AW_COP_MST_NODE_STATUS_CMD_START              1

/** \brief  停止从站 */
#define AW_COP_MST_NODE_STATUS_CMD_STOP               2

/** \brief  使从站进入预工作状态 */
#define AW_COP_MST_NODE_STATUS_CMD_PRE_OPERATIONAL    128

/** \brief  复位从站  */
#define AW_COP_MST_NODE_STATUS_CMD_RESET              129

/** \brief  复位从站连接  */
#define AW_COP_MST_NODE_STATUS_CMD_RESET_CONNECTION   130

/** @} */

/**
 * \name CANopen 主站管理的从站节点的状态
 * \anchor grp_aw_cop_mst_node_status_t
 * @{
 */

/** \brief 主站管理的从站节点的状态 */
typedef uint32_t aw_cop_mst_node_status_t;

/** \brief 主站管理的从站节点正在初始化 */
#define AW_COP_MST_NODE_STATUS_INIT         0

/** \brief 主站管理的从站节点已断线 */
#define AW_COP_MST_NODE_STATUS_DISCONNECT   1

/** \brief 主站管理的从站节点正在连接 */
#define AW_COP_MST_NODE_STATUS_CONNECTING   2

/** \brief 主站管理的从站节点准备数据通讯中 */
#define AW_COP_MST_NODE_STATUS_PREPARING    3

/** \brief 主站管理的从站节点已停止 */
#define AW_COP_MST_NODE_STATUS_STOP         4

/** \brief 主站管理的从站节点正在工作 */
#define AW_COP_MST_NODE_STATUS_WORK         5

/** \brief 主站管理的从站节点处于预工作状态 */
#define AW_COP_MST_NODE_STATUS_PREWORK      127

/** @} */

/**
 * \name CANopen 主站在线检查从节点方式
 * \anchor grp_aw_cop_mst_node_check_way_t
 * @{
 */

typedef uint8_t aw_cop_mst_node_check_way_t;     /**< \brief 从站在线检查方式 */
#define AW_COP_MST_NODE_CHECK_WAY_GUARDING     0 /**< \brief 节点守护 */
#define AW_COP_MST_NODE_CHECK_WAY_HEARTBEAT    1 /**< \brief 心跳包 */
#define AW_COP_MST_NODE_CHECK_WAY_DISABLE      2 /**< \brief 禁用 */

/** @} */

/**
 * \brief  aw_cop_mst_pfn_pdo_recv_t
 *  PDO 过程数据对象接收应用回调
 *
 * \param[out] p_msg: 接收到的 PDO过程数据对象消息
 *
 * \retval  AW_FALSE: 接收到的数据不保存到队列里面，
 *                 则通过aw_cop_mst_input_pdo_get()不能获取到接收到的数据
 * \retval  AW_TRUE:  接收到的数据保存到队列里面，
 *                 则通过aw_cop_mst_input_pdo_get()可以获取到接收到的数据
 *
 */
typedef aw_bool_t (*aw_cop_mst_pfn_pdo_recv_t) (aw_cop_pdo_msg_t *p_msg);

/** \brief EMCC 紧急事件应用回调 */
typedef void (*aw_cop_mst_pfn_emcc_event_t) (aw_cop_emcy_msg_t *p_emcy_msg);

/** \brief CANopen 主站信息 */
typedef struct aw_cop_mst_info {

    uint32_t node_id;          /**< \brief 节点ID */

    aw_cop_baud_t baudrate;    /**< \brief CAN波特率 单位kbps */

    /** \brief PDO 过程数据对象接收应用回调 */
    aw_cop_mst_pfn_pdo_recv_t   pfn_pdo_recv_callback;

    /** \brief EMCC 紧急事件应用回调 */
    aw_cop_mst_pfn_emcc_event_t pfn_emcc_event_callback;

} aw_cop_mst_info_t;

/** \brief CANopen 主站管理的从站节点信息 */
typedef struct aw_cop_mst_node_info {

    uint8_t  node_id;                         /**< \brief 从站节点ID */

    /**
     * \brief 从站在线检查方式
     *        参见\ref grp_aw_cop_mst_node_check_way_t
     */
    aw_cop_mst_node_check_way_t  check_mode;

    /**
     * \brief 从站在线检查周期，单位ms
     *        当check_mode为心跳包 AW_COP_MST_NODE_CHECK_WAY_HEARTBEAT 时，
     *        表示从站发送心跳周期
     */
    uint16_t check_period;

    /**
     * \brief 重试检查从站次数
     *        当check_mode为禁止 AW_COP_MST_NODE_CHECK_WAY_DISABLE 时无效
     */
    uint16_t retry_factor;

} aw_cop_mst_node_info_t;

/**
 * \brief  aw_cop_mst_init
 *  初始化CANopen 主站
 *
 * \param[in] inst:   CANopen实例号
 * \param[in] p_info: 初始化参数
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_init (uint8_t inst, aw_cop_mst_info_t *p_info);

/**
 * \brief  aw_cop_mst_deinit
 *  关闭CANopen 主站
 *
 * \param[in] inst: CANopen实例号
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_deinit (uint8_t inst);

/**
 * \brief  aw_cop_mst_start
 *  启动CANopen 主站
 *
 * \param[in] inst: CANopen实例号
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_start (uint8_t inst);


/**
 * \brief  aw_cop_mst_stop
 *  停止CANopen 主站
 *
 * \param[in] inst: CANopen实例号
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_stop (uint8_t inst);

/**
 * \brief  aw_cop_mst_add_node
 *  添加从站节点
 *
 * \param[in] inst: CANopen实例号
 * \param[in] p_info: 从站节点信息
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 *
 */
aw_cop_err_t aw_cop_mst_add_node (uint8_t inst, aw_cop_mst_node_info_t *p_info);

/**
 * \brief  aw_cop_mst_remove_node
 *  添加从站节点
 *
 * \param[in] inst: CANopen实例号
 * \param[in] node_id: 从站信息
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 *
 */
aw_cop_err_t aw_cop_mst_remove_node (uint8_t inst, uint8_t node_id);

/**
 * \brief  aw_cop_mst_sdo_upload
 *  调用此函数获取从站服务数据
 *
 * \param[in]     inst:      CANopen实例号
 * \param[in]     node_id:   从站地址
 * \param[in]     index:     服务数据索引号
 * \param[in]     subindex:  服务数据子索引号
 * \param[out]    p_data:    接收数据缓冲区
 * \param[in/out] p_length:  接收数据大小，返回实际接收到的数据
 * \param[in]     wait_time: 等待超时时间
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_sdo_upload (uint8_t   inst,
                                    uint8_t   node_id,
                                    uint16_t  index,
                                    uint8_t   subindex,
                                    uint8_t  *p_data,
                                    uint32_t *p_length,
                                    uint32_t  wait_time);

/**
 * \brief  aw_cop_mst_sdo_dwonload
 *  调用此函数设置从站服务数据
 *
 * \param[in] inst:      CANopen实例号
 * \param[in] node_id:   从站地址
 * \param[in] index:     服务数据索引号
 * \param[in] subindex:  服务数据子索引号
 * \param[in] p_data:    发送数据缓冲区
 * \param[in] length:    发送数据长度
 * \param[in] wait_time: 等待超时时间
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_sdo_dwonload (uint8_t    inst,
                                      uint8_t    node_id,
                                      uint16_t   index,
                                      uint8_t    subindex,
                                      uint8_t   *p_data,
                                      uint32_t   length,
                                      uint32_t   wait_time);

/**
 * \brief  aw_cop_mst_input_pdo_install
 *  添加指定从站的PDO到主站， 使主站可以接收从站发出来的数据
 *
 * \param[in] inst:     CANopen实例号
 * \param[in] node_id:  从站地址
 * \param[in] pdo_id:   PDO ID
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_input_pdo_install (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id);

/**
 * \brief  aw_cop_mst_input_pdo_remove
 *  删除注册在主站中的RPDO
 *
 * \param[in] inst:     CANopen实例号
 * \param[in] node_id:  从站地址
 * \param[in] pdo_id:   PDO ID
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_input_pdo_remove (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id);

/**
 * \brief  aw_cop_mst_output_pdo_install
 *  添加指定从站的PDO到主站， 使主站可以发送数据到从站
 *
 * \param[in] inst:     CANopen实例号
 * \param[in] node_id:  从站地址
 * \param[in] pdo_id:   PDO ID
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_output_pdo_install (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id);

/**
 * \brief  aw_cop_mst_output_pdo_remove
 *  删除注册在主站中的TPDO
 *
 * \param[in] inst:     CANopen实例号
 * \param[in] node_id:  从站地址
 * \param[in] pdo_id:   PDO ID
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_output_pdo_remove (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id);

/**
 * \brief  aw_cop_mst_input_pdo_get
 *  获取从站PDO报文
 *
 * \param[in]     inst:      CANopen实例号
 * \param[in]     node_id:   从站地址
 * \param[in]     pdo_id:    PDO ID
 * \param[out]    p_data:    接收数据缓冲区
 * \param[in/out] p_length:  接收数据大小，返回实际接收到的数据
 * \param[in]     wait_time: 等待超时时间
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_input_pdo_get (uint8_t   inst,
                                       uint8_t   node_id,
                                       uint32_t  pdo_id,
                                       uint8_t  *p_data,
                                       uint32_t *p_length,
                                       uint32_t  wait_time);

/**
 * \brief  aw_cop_mst_output_pdo_set
 *  往指定从站发送PDO输出数据
 *
 * \param[in] inst:      CANopen实例号
 * \param[in] node_id:   从站地址
 * \param[in] pdo_id:    PDO ID
 * \param[in] p_data:    发送数据缓冲区
 * \param[in] length:    发送数据长度
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_output_pdo_set (uint8_t   inst,
                                        uint8_t   node_id,
                                        uint32_t  pdo_id,
                                        uint8_t  *p_data,
                                        uint8_t   length);

/**
 * \brief  aw_cop_mst_remote_pdo_send
 *  往指定从站发送远程PDO（不带数据），从站收到后往主站回发PDO输入数据
 *
 * \param[in] inst:      CANopen实例号
 * \param[in] node_id:   从站地址
 * \param[in] pdo_id:    PDO ID
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_remote_pdo_send (uint8_t   inst,
                                         uint8_t   node_id,
                                         uint32_t  pdo_id);

/**
 * \brief  aw_cop_mst_node_status_set
 *  主站设置从站状态
 *
 * \param[in] inst:      CANopen实例号
 * \param[in] node_id:   从站地址
 * \param[in] status:    控制类型，参见\ref grp_aw_cop_mst_node_status_cmd_t
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_node_status_set (uint8_t                      inst,
                                         uint8_t                      node_id,
                                         aw_cop_mst_node_status_cmd_t status);

/**
 * \brief  aw_cop_mst_node_status_get
 *  获取主站管理的从站节点的工作状态
 *
 * \param[in]  inst:      CANopen实例号
 * \param[in]  node_id:   从站地址
 * \param[out] p_status:  从站状态，参见\ref grp_aw_cop_mst_node_status_t
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_node_status_get (uint8_t                    inst,
                                         uint8_t                    node_id,
                                         aw_cop_mst_node_status_t  *p_status);

/**
 * \brief  aw_cop_mst_sync_prodcer_cfg
 *  配置同步帧
 *
 * \param[in] inst:      CANopen实例号
 * \param[in] sync_id:   同步报文ID
 * \param[in] cyc_time:  循环周期，单位ms
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_sync_prodcer_cfg (uint8_t    inst,
                                          uint32_t   sync_id,
                                          uint16_t   cyc_time);

/**
 * \brief  aw_cop_mst_timestamp_send
 *  发送时间标识到网络
 *
 * \param[in] inst:      CANopen实例号
 * \param[in] sync_id:   同步报文ID
 *
 * \return  错误代码，参见\ref grp_aw_cop_err_t
 */
aw_cop_err_t aw_cop_mst_timestamp_send (uint8_t inst, aw_cop_time_t *p_time);

/**
 * \brief  aw_cop_mst_mst_process
 *  运行CANopen的所有处理函数
 *
 * \param[in] inst:      CANopen实例号
 */
void aw_cop_mst_process (uint8_t inst);

/** @} grp_aw_if_cop_mst */

#ifdef __cplusplus
}
#endif

#endif /* __AW_COP_MST_H */

/* end of file */
