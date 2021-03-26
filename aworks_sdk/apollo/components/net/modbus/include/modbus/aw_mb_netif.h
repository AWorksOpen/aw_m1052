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

/** \brief �ӻ�ID��Ϣ������ */
#define AW_MB_FUNC_OTHER_REP_SLAVEID_BUF  32

/******************************************************************************
 * RTUģʽ˽������
 ******************************************************************************/
/** \brief RTU˽������  */
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
 * ASCIIģʽ˽������
 ******************************************************************************/
/** \brief ASCII��ߵ��ֽڱ�־  */
enum aw_mb_byte_pos {
    AW_BYTE_HIGH_NIBBLE,   /**< \brief Character for high nibble of byte. */
    AW_BYTE_LOW_NIBBLE     /**< \brief Character for low nibble of byte. */
};

/** \brief ASCII˽������  */
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
 * TCPģʽ˽������
 ******************************************************************************/
/** \brief modbus tcp private data */
struct aw_mb_tcp {
    /** \brief TCP���ݻ��� */
    uint8_t         tcp_buf[AW_MB_MAX_TCP_ADU_LENGTH];
    uint16_t        tcp_buf_pos;         /**< \brief ���������ݵ�ǰλ�� */
    uint16_t        tcp_frame_bytesleft; /**< \brief ��Ҫ�������ݳ��� */
    uint16_t        port;                /**< \brief �˿ں� */
    aw_bool_t       is_accept_new;       /**< \brief ���Ӵﵽ����ֵ���Ƿ������������������ */

    int             listen_socket;       /**< \brief ����socket */
    int             cur_socket;          /**< \brief ��ǰ����socket���  */
    int             socket_num;          /**< \brief ���������ӵ�socket��  */
    int             accept_num;          /**< \brief ��ǰ������  */
    int            *p_sockets;           /**< \brief socket��  */

    aw_mb_tcp_accept_hook_t accept_hook; /**< \brief TCP�������ӹ��Ӻ���  */
    aw_mb_tcp_close_hook_t  close_hook;  /**< \brief TCP�ر����ӹ��Ӻ���  */
    void                   *p_arg;       /**< \brief TCP���ӹ��Ӻ�������  */

    aw_spinlock_isr_t isr_lock;          /**< \brief ������ */
};
/******************************************************************************
 * ��վ����
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
    /** \brief ��ȡ��վ��ַ */
    uint8_t     (*slave_addr_get)(void *p_cookie);
    /** \brief ���ô�վ��ַ */
    aw_mb_err_t (*slave_addr_set)(void *p_cookie, uint8_t addr);
    /** \brief �ر�Modbus��վ */
    void        (*close)(void *p_cookie);
    /** \brief ����Modbus��վ */
    void        (*start)(void *p_cookie);
    /** \brief ֹͣModbus��վ */
    void        (*stop) (void *p_cookie);
    /** \brief ��վ�������� */
    aw_mb_err_t (*recv) (void      *p_cookie,
                         uint8_t   *slave_addr,
                         uint8_t  **pp_pdu,
                         uint16_t  *p_len);
    /** \brief ��վ�������� */
    aw_mb_err_t (*send) (void *p_cookie, uint8_t *p_pud, uint16_t len);
};

/** \brief slave date type */
struct aw_mb_slave_struct {
    enum aw_mb_slave_status   status;           /**< \brief ��վ״̬  */
    enum aw_mb_mode           mode;             /**< \brief ��ǰ����ģʽ */
    const struct aw_mb_netif *p_netif;          /**< \brief ģʽ�ӿ� */
    void                     *p_cookie;         /**< \brief ģʽ�ӿھ�� */

    aw_funcode_handle        *p_hdl_tbl_head;   /**< \brief �û�ע�Ṧ�����ͷ */
    uint8_t                   user_funcode_num; /**< \brief �û���������  */

    /** \brief ͨ�ù�����ص�  */
    aw_mb_slave_fn_code_callback_t fn_read_coils_cb;  /**< \brief ����Ȧ�ص����� */
    aw_mb_slave_fn_code_callback_t fn_write_coils_cb; /**< \brief д��Ȧ�ص����� */
    aw_mb_slave_fn_code_callback_t fn_read_discre_cb; /**< \brief ����ɢ���ص����� */
    aw_mb_slave_fn_code_callback_t fn_read_input_cb;  /**< \brief ������Ĵ����ص����� */
    aw_mb_slave_fn_code_callback_t fn_read_hold_cb;   /**< \brief �����ּĴ����ص����� */
    aw_mb_slave_fn_code_callback_t fn_write_hold_cb;  /**< \brief д���ּĴ����ص����� */
#if 0
    /** \brief �û�ID��Ϣ���ɲ�����  */
    uint8_t  id[AW_MB_FUNC_OTHER_REP_SLAVEID_BUF];
    uint16_t id_len; /**< \brief ID��Ϣ���� */
#endif

    aw_spinlock_isr_t lock; /**< \brief ��������������  */
};
/******************************************************************************/
/**
 * \brief Modbus RTUģʽ�ӿ����ó�ʼ��
 *
 * \param[in] pp_cookie  ģʽ���
 * \param[in] p_param    ��ʼ������,ת��Ϊ\a struct aw_mb_serial_param ��Ϊ����
 *
 * \return modbus ����ţ����û�д�����ֵΪAW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_netif_rtu_init (void **pp_cookie, void *p_param);

/**
 * \brief Modbus ASCIIģʽ�ӿ����ó�ʼ��
 *
 * \param[in] pp_cookie  ģʽ���
 * \param[in] p_param    ��ʼ������,ת��Ϊ\a struct aw_mb_serial_param ��Ϊ����
 *
 * \return modbus ����ţ����û�д�����ֵΪAW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_netif_ascii_init (void **pp_cookie, void *p_param);

/**
 * \brief Modbus TCPģʽ�ӿ����ó�ʼ��
 *
 * \param[in] pp_cookie  ģʽ���
 * \param[in] p_param    ��ʼ������,ת��Ϊ\a struct aw_mb_serial_param ��Ϊ����
 *
 * \return modbus ����ţ����û�д�����ֵΪAW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_netif_tcp_init (void **pp_cookie, void *p_param);

/**
 * \brief ��Modbus���ýӿڣ�������Ӧģʽ����,ע�Ṧ�ܺ���
 *
 * \param[in] p_slave  ��վ�豸ʵ��
 * \param[in] mode     Modbusͨ��ģʽ
 * \param[in] p_param  ��ʼ������������AW_MB_RTU/AW_MB_ASCIIģʽʹ��
 *                     \a struct aw_mb_serial_param ��Ϊ����
 *                     ����AW_MB_TCPģʽ����Ϊ�˿ںŲ���ʹ��
 *
 * \return modbus ����ţ����û�д�����ֵΪAW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_netif_open (struct aw_mb_slave_struct *p_slave,
                              enum aw_mb_mode            mode,
                              void                      *p_param);

/**
 * \brief �����û�������
 *
 * \param[in] p_slave  ��վ�豸ʵ��.
 * \param[in] num      �������
 *
 * \return modbus ����ţ����û�д�����ֵΪAW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mbs_user_funcode_alloc (struct aw_mb_slave_struct *p_slave,
                                       uint8_t                    num);

/**
 * \brief ����TCP������
 *
 * \param[in] p_cookie  TCP���
 * \param[in] tcp_num   ����������
 *
 * \return modbus ����ţ����û�д�����ֵΪAW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_netif_tcp_num_set (void *p_cookie, uint16_t tcp_num);

/**
 * \brief ��ȡ�ѽ�����������
 *
 * \param[in] p_cookie  Modbus TCP�豸���
 *
 * \return �ѽ��յ�������
 */
aw_static_inline uint16_t aw_mb_netif_tcp_num_get (void *p_cookie)
{
    return ((struct aw_mb_tcp *)p_cookie)->accept_num;
}

/**
 * \brief �Ƿ���������������رվ����ӣ�����������
 *
 * \param[in] p_cookie  Modbus TCP�豸���
 * \param[in] is_allow  �Ƿ�����.
 *
 * \return ��
 */
void aw_mb_netif_allow_new_tcp (void *p_cookie, aw_bool_t is_allow);

/**
 * \brief TCP���Ӻ���ע��
 *
 * \param[in] p_cookie     modbus tcp ˽������
 * \param[in] accept_hook  �������ӹ��Ӻ���
 * \param[in] close_hook   �Ͽ����ӹ��Ӻ���
 * \param[in] p_arg        ���Ӻ�������
 *
 * \return ��
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
