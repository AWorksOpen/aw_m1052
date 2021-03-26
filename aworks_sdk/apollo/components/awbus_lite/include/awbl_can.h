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
#define AWBL_CAN_INT_NONE                 0x00  /**< \brief �޴���*/
#define AWBL_CAN_INT_TX                   0x01  /**< \brief ���ͳɹ� �ж�*/
#define AWBL_CAN_INT_RX                   0x02  /**< \brief ���� �ж� */
#define AWBL_CAN_INT_WAKE_UP              0x04  /**< \brief �����ж�*/
#define AWBL_CAN_INT_ERROR                0x08  /**< \brief �����ж�*/
#define AWBL_CAN_INT_DATAOVER             0x10  /**< \brief ���߳��� �ж�*/
#define AWBL_CAN_INT_WARN                 0x20  /**< \brief ���󱨾� �ж�*/
#define AWBL_CAN_INT_ERROR_PASSIVE        0x40  /**< \brief ���󱻶��ж� */
#define AWBL_CAN_INT_BUS_OFF              0x80  /**< \brief ���߹ر��ж� */
#define AWBL_CAN_INT_STATUS               0xf8
#define AWBL_CAN_INT_ALL                  0xffffffff

#define AWBL_CAN_CHN_NONE                 0x00  /**< \brief ͨ��δ��ʼ��*/
#define AWBL_CAN_CHN_INIT                 0x01  /**< \brief ͨ����ʼ��*/
#define AWBL_CAN_CHN_START                0x02  /**< \brief ͨ������*/
#define AWBL_CAN_CHN_STOP                 0x03  /**< \brief ͨ��ֹͣ*/

typedef struct awbl_can_drv_funcs {

    /** \brief ��ʼ����غ��� */
    aw_can_err_t (*pfn_can_init)  (
            struct awbl_can_service *p_serv, aw_can_work_mode_type_t work_mode);

    /** \brief CAN���� */
    aw_can_err_t (*pfn_can_start) (struct awbl_can_service *p_serv);
    
    /** \brief CANֹͣ */
    aw_can_err_t (*pfn_can_stop) (struct awbl_can_service *p_serv);
    
    /** \brief CAN���� */
    aw_can_err_t (*pfn_can_sleep) (struct awbl_can_service *p_serv);

    /** \brief CAN���� */
    aw_can_err_t (*pfn_can_wakeup) (struct awbl_can_service *p_serv);

    /** \brief �ж�ʹ�� ������ */
    aw_can_err_t (*pfn_can_int_enable)  (
            struct awbl_can_service *p_serv, awbl_can_int_type_t type);
    aw_can_err_t (*pfn_can_int_disable) (
            struct awbl_can_service *p_servp_serv, awbl_can_int_type_t type);

    /** \brief ���������� */
    aw_can_err_t (*pfn_can_baud_set) (
            struct awbl_can_service   *p_serv,
            const aw_can_baud_param_t *p_param);

    /** \brief ���������� */
    aw_can_err_t (*pfn_can_baud_get) (
            struct awbl_can_service *p_serv, aw_can_baud_param_t *p_param);

    /** \brief ��ȡ������� */
    aw_can_err_t (*pfn_can_err_cnt_get) (
            struct awbl_can_service *p_serv, aw_can_err_cnt_t *p_err_cnt);
    
    /** \brief ���������� */
    aw_can_err_t (*pfn_can_err_cnt_clr) (struct awbl_can_service *p_serv);

    /** \brief CAN�շ���غ��� */
    aw_can_err_t (*pfn_can_reg_msg_read)  (
            struct awbl_can_service *p_serv, aw_can_msg_t *p_msg);
    aw_can_err_t (*pfn_can_reg_msg_write) (
            struct awbl_can_service *p_serv, aw_can_msg_t *p_msg);
    aw_can_err_t (*pfn_can_msg_snd_stop)  (struct awbl_can_service *p_serv);

    /** \brief �����˲���غ��� */
    aw_can_err_t (*pfn_can_filter_table_set) (
            struct awbl_can_service *p_serv, uint8_t *p_buf, size_t buf_len);
    aw_can_err_t (*pfn_can_filter_table_get) (
            struct awbl_can_service *p_serv, uint8_t *p_buf, size_t *p_buf_len);

    /** \brief CAN bus���״̬��ȡ */
    aw_can_err_t (*pfn_can_int_status_get) (struct awbl_can_service *p_serv,
                                            awbl_can_int_type_t     *p_type,
                                            aw_can_bus_err_t        *p_err);

    /** \brief �����Ĵ��������ӿڣ������CAN�������õ� */
    aw_can_err_t (*pfn_can_reg_write) (struct awbl_can_service *p_serv,
                                       uint32_t                 offset,
                                       uint8_t                 *p_data,
                                       size_t                   length);
    aw_can_err_t (*pfn_can_reg_read ) (struct awbl_can_service *p_serv,
                                       uint32_t                 offset,
                                       uint8_t                 *p_data,
                                       size_t                   length);

} awbl_can_drv_funcs_t;

/** \brief ���������� */
#define AWBL_CAN_CONTROLLER_LOCK(p_serv) \
    aw_spinlock_isr_take(&((struct awbl_can_service *)p_serv)->lock_dev)

/** \brief ������������� */
#define AWBL_CAN_CONTROLLER_UNLOCK(p_serv) \
    aw_spinlock_isr_give(&((struct awbl_can_service *)p_serv)->lock_dev)

typedef struct awbl_can_servinfo {

    int                 chn;              /**< \brief ��ǰͨ����     */
    aw_can_ctrl_type_t  ctr_type;         /**< \brief ��ǰ���������� */
    uint8_t             sizeof_msg;       /**< \brief ��ǰ���ĳ���   */
    uint32_t            rxd_buf_size;     /**< \brief ���ձ��Ļ�������С*/
    void               *p_rxd_buf;        /**< \brief ���ձ��Ļ�����ָ��*/
    struct aw_rngbuf   *p_rxd_buf_ctr;    /**< \brief ���ձ��Ļ�����������*/

}awbl_can_servinfo_t;

/**
 * \brief AWBus CAN ����ʵ��
 */
struct awbl_can_service {

    /** \brief ָ����һ�� CAN ���� */
    struct awbl_can_service *p_next;

    /** \brief CAN ����������Ϣ */
    awbl_can_servinfo_t     *p_info;

    /** \brief ͨ��״̬    */
    uint8_t  chn_status;

    /** \brief ���ͷ�æ��־   */
    uint8_t  txd_busy;

    /** \brief �豸�� */
    aw_spinlock_isr_t lock_dev;

    /** \brief �豸������ */
    AW_MUTEX_DECL(dev_mux);

    /** \brief �����ź��� */
    AW_SEMB_DECL(rcv_start_sem);

    /** \brief CAN ��������Ϣ���� */
    struct aw_list_head transfer_list;

    /** \brief ��ǰ���͵���Ϣ*/
    aw_can_transfer_t *p_curr_trans;

    /** \brief CAN�������غ��� */
    awbl_can_drv_funcs_t *p_servfuncs;

    /** \brief �û�APP�ص����� */
    aw_can_app_callbacks_t *p_app_cb_funcs;

};

typedef struct awbl_can_service awbl_can_service_t;

/** \brief �����ʼ������ */
aw_err_t awbl_can_service_init (awbl_can_service_t    *p_serv,
                                awbl_can_servinfo_t   *p_servinfo,
                                awbl_can_drv_funcs_t  *p_servfuncs);
/** \brief CAN����ע�� */
aw_err_t awbl_can_service_register (awbl_can_service_t *p_serv);

/** \brief �жϷ����� */
void awbl_can_isr_handler (awbl_can_service_t *p_serv);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_CAN_H */

