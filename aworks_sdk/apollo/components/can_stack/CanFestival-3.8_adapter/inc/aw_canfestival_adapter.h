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
 * \brief CanFestival 配置
 *
 * \internal
 * \par modification history:
 * - 1.00 18-10-08  anu, first implementation
 * \endinternal
 */

#ifndef __AW_CANFESTIVAL_ADAPTER_H
#define __AW_CANFESTIVAL_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_canfestival_adapter
 * \copydoc aw_canfestival_adapter.h
 * @{
 */

#include "aw_types.h"
#include "aw_can.h"

#include "canfestival.h"

/**
 * \brief CANfestival 实例的句柄获取
 *
 * \param[in] inst_num 实例号
 *
 * \return 对应实例的句柄
 */
void *aw_canfestival_inst_get (int inst_num);

/**
 * \brief CANfestival 实例的数据获取
 *
 * \param[in] inst_num 实例号
 *
 * \return 对应实例的数据
 */
void *aw_canfestival_inst_data_get (int inst_num);

/**
 * \brief RPDO更新回调函数注册
 *
 * \param[in] inst_num  实例号
 * \param[in] p_msg     要发送的消息
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_msg_send (int inst_num, aw_can_std_msg_t *p_msg);

/**
 * \brief RPDO更新回调函数注册
 *
 * \param[in] inst_num       实例号
 * \param[in] p_callback_fun 回调函数
 * \param[in] p_arg          用户参数
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_rpdo_update_callback_register (
    int    inst_num,
    void (*p_callback_fun) (
        void *p_arg, uint16_t pdo_id, uint8_t *p_data, uint8_t len),
    void  *p_arg);

/**
 * \brief CANfestival 获取实例对应的节点的个数
 *
 * \param[in] inst_num 实例号
 *
 * \return 个数，小于等于0则为无效
 */
int8_t aw_canfestival_inst_node_cnt (int inst_num);

/**
 * \brief CANfestival 获取PDO ID对应的PDO NUM
 *
 * \param[in]  inst_num   实例号
 * \param[in]  node_id    NODE ID 号
 * \param[out] p_node_num NODE NUM 号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_node_num_get (
    int inst_num, int node_id, int *p_node_num);

/**
 * \brief CANfestival 设置实例的某个节点的信息
 *
 * \param[in] inst_num     实例号
 * \param[in] node_num     节点号
 * \param[in] node_id      节点ID
 * \param[in] check_mode   检查模式
 * \param[in] check_period 检查周期
 * \param[in] retry_factor 检查因子
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_node_info_set (int      inst_num,
                                                uint8_t  node_num,
                                                uint8_t  node_id,
                                                uint8_t  check_mode,
                                                uint16_t check_period,
                                                uint16_t retry_factor);

/**
 * \brief CANfestival 获取实例的某个节点的信息
 *
 * \param[in]  inst_num       实例号
 * \param[in]  node_num       节点号
 * \param[out] p_node_id      节点ID
 * \param[out] p_check_mode   检查模式
 * \param[out] p_check_period 检查周期
 * \param[out] p_retry_factor 检查因子
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_node_info_get (int       inst_num,
                                                uint8_t   node_num,
                                                uint8_t  *p_node_id,
                                                uint8_t  *p_check_mode,
                                                uint16_t *p_check_period,
                                                uint16_t *p_retry_factor);

/**
 * \brief CANfestival 实例添加节点信息
 *
 * \param[in] inst_num 实例号
 * \param[in] node_id      节点ID
 * \param[in] check_mode   检查模式
 * \param[in] check_period 检查周期
 * \param[in] retry_factor 检查因子
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_node_add (int      inst_num,
                                           uint8_t  node_id,
                                           uint8_t  check_mode,
                                           uint16_t check_period,
                                           uint16_t retry_factor);

/**
 * \brief CANfestival 实例移除节点
 *
 * \param[in] inst_num 实例号
 * \param[in] node_id  节点ID
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_node_remove (int      inst_num,
                                              uint8_t  node_id);

/**
 * \brief CANfestival 获取实例对应的PDO的个数
 *
 * \param[in] inst_num 实例号
 * \param[in] is_rpdo  是否是RPDO
 *
 * \return 个数，小于等于0则为无效
 */
int8_t aw_canfestival_inst_pdo_cnt (int inst_num, aw_bool_t is_rpdo);

/**
 * \brief CANfestival 获取PDO ID对应的PDO NUM
 *
 * \param[in]  inst_num  实例号
 * \param[in]  pdo_id    PDO ID 号
 * \param[in]  is_rpdo   是否是RPDO
 * \param[out] p_pdo_num PDO NUM 号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_pdo_num_get (
    int inst_num, int pdo_id, aw_bool_t is_rpdo, int *p_pdo_num);

/**
 * \brief CANfestival 失能所有RPDO或TPDO
 *
 * \param[in]  inst_num  实例号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_all_pdo_disable (int inst_num, aw_bool_t is_rpdo);

/**
 * \brief CANfestival 实例添加PDO
 *
 * \param[in] inst_num 实例号
 * \param[in] pdo_id   PDO ID 号
 * \param[in] is_rpdo  是否是RPDO
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_pdo_add (
    int inst_num, int pdo_id, aw_bool_t is_rpdo);

/**
 * \brief CANfestival 实例移除PDO
 *
 * \param[in] inst_num 实例号
 * \param[in] pdo_id   PDO ID 号
 * \param[in] is_rpdo  是否是RPDO
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_pdo_remove (
    int inst_num, int pdo_id, aw_bool_t is_rpdo);

/**
 * \brief CANfestival 设置PDO数据
 *
 * \param[in]  inst_num  实例号
 * \param[in]  pdo_num   PDO NUM 号
 * \param[in]  is_rpdo   是否是RPDO
 * \param[in]  p_data    PDO 数据
 * \param[in]  length    PDO 数据长度
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_pdo_data_set (int        inst_num,
                                          int        pdo_num,
                                          aw_bool_t  is_rpdo,
                                          uint8_t   *p_data,
                                          uint8_t    length);

/**
 * \brief CANfestival 获取PDO数据
 *
 * \param[in]  inst_num  实例号
 * \param[in]  pdo_num   PDO NUM 号
 * \param[in]  is_rpdo   是否是RPDO
 * \param[in]  p_data    PDO 数据
 * \param[in]  p_len     PDO 数据长度
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_pdo_data_get (int        inst_num,
                                          int        pdo_num,
                                          aw_bool_t  is_rpdo,
                                          uint8_t   *p_data,
                                          uint8_t   *p_len);

/**
 * \brief CANfestival 获取PDO数据
 *
 * \param[in]  inst_num  实例号
 * \param[in]  pdo_num   PDO NUM 号
 * \param[in]  p_data    PDO 数据
 * \param[in]  p_len     PDO 数据长度
 * \param[in]  wait_time 获取超时时间 ms
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_rpdo_rngbuf_data_get (int       inst_num,
                                                  int       pdo_num,
                                                  uint8_t  *p_data,
                                                  uint32_t *p_len,
                                                  uint32_t  wait_time);

/**
 * \brief CANfestival 实例启动
 *
 * \param[in] inst_num 实例号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_start (int inst_num);

/**
 * \brief CANfestival 实例停止
 *
 * \param[in] inst_num 实例号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_stop (int inst_num);

/**
 * \brief CANfestival 实例的初始化
 *
 * \param[in] inst_num 实例号
 * \param[in] brt      波特率 单位Kbps
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_init (int inst_num, int brt);

/**
 * \brief CANfestival 实例的解初始化
 *
 * \param[in] inst_num 实例号
 *
 * \return AW_CAN_ERR_NONE操作成功，其他错误原因参考错误码
 */
aw_can_err_t aw_canfestival_inst_deinit (int inst_num);

/** @} grp_aw_if_canfestival_adapter */

#ifdef __cplusplus
}
#endif

#endif /* __AW_CANFESTIVAL_ADAPTER_H */

/* end of file */
