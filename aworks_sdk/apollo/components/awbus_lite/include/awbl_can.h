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
 * \internal
 * \par modification history:
 * - 1.01 17-08-30  anu, modify
 * - 1.00 15-11-16  cae, first implementation
 * \endinternal
 */
#ifndef __AWBL_CAN_H
#define __AWBL_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_can.h"
#include "aw_msgq.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "aw_list.h"

struct awbl_can_service;

/** \brief can Interrupt status */
typedef uint32_t awbl_can_int_type_t;
#define AWBL_CAN_INT_NONE                 0x00  /**< \brief 无错误*/
#define AWBL_CAN_INT_TX                   0x01  /**< \brief 发送成功 中断*/
#define AWBL_CAN_INT_RX                   0x02  /**< \brief 接收 中断 */
#define AWBL_CAN_INT_WAKE_UP              0x04  /**< \brief 唤醒中断*/
#define AWBL_CAN_INT_ERROR                0x08  /**< \brief 错误中断*/
#define AWBL_CAN_INT_DATAOVER             0x10  /**< \brief 总线超载 中断*/
#define AWBL_CAN_INT_WARN                 0x20  /**< \brief 错误报警 中断*/
#define AWBL_CAN_INT_ERROR_PASSIVE        0x40  /**< \brief 错误被动中断 */
#define AWBL_CAN_INT_BUS_OFF              0x80  /**< \brief 总线关闭中断 */
#define AWBL_CAN_INT_STATUS               0xf8
#define AWBL_CAN_INT_ALL                  0xffffffff

#define AWBL_CAN_CHN_NONE                 0x00  /**< \brief 通道未初始化*/
#define AWBL_CAN_CHN_INIT                 0x01  /**< \brief 通道初始化*/
#define AWBL_CAN_CHN_START                0x02  /**< \brief 通道启动*/
#define AWBL_CAN_CHN_STOP                 0x03  /**< \brief 通道停止*/

typedef struct awbl_can_drv_funcs {

    /** \brief 初始化相关函数 */
    aw_can_err_t (*pfn_can_init)  (
            struct awbl_can_service *p_serv, aw_can_work_mode_type_t work_mode);

    /** \brief CAN启动 */
    aw_can_err_t (*pfn_can_start) (struct awbl_can_service *p_serv);
    
    /** \brief CAN停止 */
    aw_can_err_t (*pfn_can_stop) (struct awbl_can_service *p_serv);
    
    /** \brief CAN休眠 */
    aw_can_err_t (*pfn_can_sleep) (struct awbl_can_service *p_serv);

    /** \brief CAN唤醒 */
    aw_can_err_t (*pfn_can_wakeup) (struct awbl_can_service *p_serv);

    /** \brief 中断使能 、禁能 */
    aw_can_err_t (*pfn_can_int_enable)  (
            struct awbl_can_service *p_serv, awbl_can_int_type_t type);
    aw_can_err_t (*pfn_can_int_disable) (
            struct awbl_can_service *p_servp_serv, awbl_can_int_type_t type);

    /** \brief 波特率设置 */
    aw_can_err_t (*pfn_can_baud_set) (
            struct awbl_can_service   *p_serv,
            const aw_can_baud_param_t *p_param);

    /** \brief 波特率设置 */
    aw_can_err_t (*pfn_can_baud_get) (
            struct awbl_can_service *p_serv, aw_can_baud_param_t *p_param);

    /** \brief 获取错误计数 */
    aw_can_err_t (*pfn_can_err_cnt_get) (
            struct awbl_can_service *p_serv, aw_can_err_cnt_t *p_err_cnt);
    
    /** \brief 清除错误计数 */
    aw_can_err_t (*pfn_can_err_cnt_clr) (struct awbl_can_service *p_serv);

    /** \brief CAN收发相关函数 */
    aw_can_err_t (*pfn_can_reg_msg_read)  (
            struct awbl_can_service *p_serv, aw_can_msg_t *p_msg);
    aw_can_err_t (*pfn_can_reg_msg_write) (
            struct awbl_can_service *p_serv, aw_can_msg_t *p_msg);
    aw_can_err_t (*pfn_can_msg_snd_stop)  (struct awbl_can_service *p_serv);

    /** \brief 验收滤波相关函数 */
    aw_can_err_t (*pfn_can_filter_table_set) (
            struct awbl_can_service *p_serv, uint8_t *p_buf, size_t buf_len);
    aw_can_err_t (*pfn_can_filter_table_get) (
            struct awbl_can_service *p_serv, uint8_t *p_buf, size_t *p_buf_len);

    /** \brief CAN bus相关状态获取 */
    aw_can_err_t (*pfn_can_int_status_get) (struct awbl_can_service *p_serv,
                                            awbl_can_int_type_t     *p_type,
                                            aw_can_bus_err_t        *p_err);

    /** \brief 操作寄存器函数接口，特殊的CAN控制器用到 */
    aw_can_err_t (*pfn_can_reg_write) (struct awbl_can_service *p_serv,
                                       uint32_t                 offset,
                                       uint8_t                 *p_data,
                                       size_t                   length);
    aw_can_err_t (*pfn_can_reg_read ) (struct awbl_can_service *p_serv,
                                       uint32_t                 offset,
                                       uint8_t                 *p_data,
                                       size_t                   length);

} awbl_can_drv_funcs_t;

/** \brief 锁定控制器 */
#define AWBL_CAN_CONTROLLER_LOCK(p_serv) \
    aw_spinlock_isr_take(&((struct awbl_can_service *)p_serv)->lock_dev)

/** \brief 解除控制器锁定 */
#define AWBL_CAN_CONTROLLER_UNLOCK(p_serv) \
    aw_spinlock_isr_give(&((struct awbl_can_service *)p_serv)->lock_dev)

typedef struct awbl_can_servinfo {

    int                 chn;              /**< \brief 当前通道号     */
    aw_can_ctrl_type_t  ctr_type;         /**< \brief 当前控制器类型 */
    uint8_t             sizeof_msg;       /**< \brief 当前报文长度   */
    uint32_t            rxd_buf_size;     /**< \brief 接收报文缓冲区大小*/
    void               *p_rxd_buf;        /**< \brief 接收报文缓冲区指针*/
    struct aw_rngbuf   *p_rxd_buf_ctr;    /**< \brief 接收报文缓冲区控制字*/

}awbl_can_servinfo_t;

/**
 * \brief AWBus CAN 服务实例
 */
struct awbl_can_service {

    /** \brief 指向下一个 CAN 服务 */
    struct awbl_can_service *p_next;

    /** \brief CAN 服务的相关信息 */
    awbl_can_servinfo_t     *p_info;

    /** \brief 通道状态    */
    uint8_t  chn_status;

    /** \brief 发送繁忙标志   */
    uint8_t  txd_busy;

    /** \brief 设备锁 */
    aw_spinlock_isr_t lock_dev;

    /** \brief 设备互斥量 */
    AW_MUTEX_DECL(dev_mux);

    /** \brief 接收信号量 */
    AW_SEMB_DECL(rcv_start_sem);

    /** \brief CAN 控制器消息队列 */
    struct aw_list_head transfer_list;

    /** \brief 当前发送的消息*/
    aw_can_transfer_t *p_curr_trans;

    /** \brief CAN服务的相关函数 */
    awbl_can_drv_funcs_t *p_servfuncs;

    /** \brief 用户APP回调函数 */
    aw_can_app_callbacks_t *p_app_cb_funcs;

};

typedef struct awbl_can_service awbl_can_service_t;

/** \brief 服务初始化函数 */
aw_err_t awbl_can_service_init (awbl_can_service_t    *p_serv,
                                awbl_can_servinfo_t   *p_servinfo,
                                awbl_can_drv_funcs_t  *p_servfuncs);
/** \brief CAN服务注册 */
aw_err_t awbl_can_service_register (awbl_can_service_t *p_serv);

/** \brief 中断服务函数 */
void awbl_can_isr_handler (awbl_can_service_t *p_serv);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_CAN_H */

