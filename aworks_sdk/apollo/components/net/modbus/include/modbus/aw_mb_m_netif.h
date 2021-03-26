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
 * \brief modbus master middle layer interface.
 *
 * \internal
 * \par modification history
 * - 1.01 2019-03-26  cml, use serial interface replace int serial.
 * - 1.00 2016-01-18  cod, first implementation
 * \endinternal
 */
#ifndef __AW_MB_M_NETIF_H  /* { */
#define __AW_MB_M_NETIF_H

#include "modbus/aw_mb_master.h"
#include "modbus/aw_mb_m_port.h"
#include "aw_spinlock.h"

/**
 * \addtogroup grp_aw_if_mb_m_netif
 * \copydoc aw_mb_m_netif.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

/** \brief Modbus master RTU模式私有数据  */
typedef struct aw_mb_m_rtu {
    /** \brief RTU mode ADU Buffer  */
    uint8_t       adu_buf[AW_MB_MAX_SER_ADU_LENGTH];
    uint16_t      snd_len;        /**< \brief send data length  */
    uint16_t      snd_pos;        /**< \brief send data pos  */
    uint16_t      rcv_pos;        /**< \brief receive data pos  */
    uint32_t      serial_id;      /**< \brief serial id */
    uint32_t      t35_time;       /**< \brief T3.5 time   */
} aw_mb_m_rtu_t;
/*****************************************************************************/
/** \brief ASCII模式私有数据 */
typedef struct aw_mb_m_ascii {
    uint8_t       adu_buf[AW_MB_MAX_SER_ADU_LENGTH]; /**< \brief ASCII Buffer */
    uint8_t       high_nibble; /**< \brief =1 high nibble =0 low nibble */
    uint8_t       *p_snddata;  /**< \brief send data buffer */
    uint8_t       sndlen;      /**< \brief send data length */
    uint8_t       rcv_pos;     /**< \brief receive pdu data buffer postion */
    uint32_t      serial_id;   /**< \brief serial id */
} aw_mb_m_ascii_t;
/*****************************************************************************/
/** \brief TCP客户端模式私有数据  */
typedef struct aw_mb_m_tcp {
    /** \brief TCP mode Buffer  */
    uint8_t              adu_buf[AW_MB_MAX_TCP_ADU_LENGTH];
    aw_mb_m_tcp_client_t client;        /**< \brief TCP client information  */
    uint16_t             cnt;           /**< \brief used TID  */
    aw_bool_t            uid_as_addr;   /**< \brief UID as slave address  */
} aw_mb_m_tcp_t;
/*****************************************************************************/
/** \brief master status */
enum aw_mb_master_status {
    AW_MB_MASTER_STATE_UNUSE, /**< \brief Master create but no used state */
    AW_MB_MASTER_STATE_INIT,  /**< \brief Master initial state */
    AW_MB_MASTER_STATE_START, /**< \brief Master startup state */
    AW_MB_MASTER_STATE_STOP,  /**< \brief Master stop state */
};

/** \brief data-link common interface */
typedef struct aw_mb_m_netif {

    /** \brief master init */
    aw_mb_err_t (*init)  (void *p_master, const void *p_param);

    /** \brief master close */
    aw_mb_err_t (*close) (void *p_master);

    /** \brief master start */
    aw_mb_err_t (*start) (void *p_master);

    /** \brief master stop */
    aw_mb_err_t (*stop)  (void *p_master);

    /** \brief master recv slave data after recive a frame */
    aw_mb_err_t (*m_recv)  (void     *p_master,
                            uint8_t  *p_slave_addr,
                            uint8_t **pp_pdu,
                            uint32_t *p_pdulen);

    /** \brief master send a pdu */
    aw_mb_err_t (*m_send)  (void    *p_master,
                            uint8_t  slave_addr,
                            uint8_t *p_pdu,
                            uint32_t pdulen);

    /** \brief get a pdu buffer to send  */
    void (*pdu_get) (void *p_master, uint8_t **pp_pdu, uint32_t *p_pdulen);

} aw_mb_m_netif_t;


/** \brief  function code handler type */
typedef struct {
    uint8_t                     funcode;     /**< \brief function_code */
    aw_mb_snd_funcode_handler_t snd_handler; /**< \brief send handle interface */
    aw_mb_rcv_funcode_handler_t rcv_handler; /**< \brief recv handle interface */
} aw_mb_m_funcode_handler_t;

/**
 * \brief master common data
 */
typedef struct aw_mb_m_dev {

    uint8_t                    slave_addr; /**< \brief slave address */
    enum aw_mb_mode            mode;       /**< \brief master mode   */
    enum aw_mb_master_status   status;     /**< \brief master status */

    /** \brief table of user function’s code handler */
    aw_mb_m_funcode_handler_t *p_funcode_tbl;
    uint8_t                    funcode_num; /**< \brief function code number */
    const aw_mb_m_netif_t     *p_netif;     /**< \brief net interface */
    union {
        aw_mb_m_rtu_t   mbm_rtu;     /**< \brief RTU mode private data */
        aw_mb_m_ascii_t mbm_ascii;   /**< \brief ASCII mode private data */
        aw_mb_m_tcp_t   mbm_tcp;     /**< \brief TCP mode private data */
    } common;                        /**< \brief private data of different mode */
    AW_MUTEX_DECL(          stat_lock);  /**< \brief start & stop lock */
    AW_SEMB_DECL(           req_lock);   /**< \brief Request lock */
    aw_mb_err_t             master_err;  /**< \brief master error */
    uint32_t                ack_timeout; /**< \brief master wait ack timeout */
    uint32_t                bd_delay;    /**< \brief rtu/ascii boardcast delay time */
    uint32_t                tcp_contm;   /**< \brief TCP mode connect timeout */
    aw_bool_t               uid_as_addr; /**< \brief TCP mode UID as slave address */
    aw_mb_master_tcp_hook_t tcp_hook;    /**< \brief TCP socket register callback */
    void                   *p_hook_arg;  /**< \brief TCP callback argument */
    aw_spinlock_isr_t       lock;        /**< \brief lock */
} aw_mb_m_dev_t;

/**
 * \brief 当前请求是否为广播请求
 *
 * \param[in] p_master  主站设备
 *
 * \retval AW_TRUE   请求为广播请求
 * \retval AW_FALSE  求非广播请求
 */
aw_static_inline aw_bool_t aw_mb_m_is_boardcast (struct aw_mb_m_dev *p_master)
{
    return p_master->slave_addr == AW_MB_ADDRESS_BROADCAST ? AW_TRUE : AW_FALSE;
}

/**
 * \brief 允许主站请求
 *
 * \note 该接口在启动主站时调用，或RTU模式下，启动主站后T3.5定时完成后调用，或广播等待时间结束后调用。
 *
 * \param[in] p_master  主站设备
 *
 * \return 无
 */
aw_static_inline void aw_mb_m_req_enable (struct aw_mb_m_dev *p_master)
{
    AW_SEMB_GIVE(p_master->req_lock);
}

/**
 * \brief 获取功能码处理接口
 *
 * \param[in] p_master  主站设备
 * \param[in] funcode   功能码
 *
 * \return 返回相应功能码处理接口，若无该功能码处理接口，返回NULL.
 */
const aw_mb_m_funcode_handler_t *aw_mb_m_funcode_handler_get (\
                                                    aw_mb_m_dev_t *p_master,
                                                    uint8_t        funcode);
/**
 * \brief 获取应答超时时间获取
 *
 * \param[in] p_master  主站设备
 *
 * \return    超时时间
 */
aw_static_inline uint32_t aw_mb_m_ack_timeout_get (aw_mb_m_dev_t *p_master)
{
    return p_master->ack_timeout;
}


/**
 * \brief 获取广播延迟获取
 *
 * \param[in] p_master  主站设备
 *
 * \return    超时时间
 */
aw_static_inline uint32_t aw_mb_master_broadcast_delay_get (aw_mb_m_dev_t *p_master)
{
    return p_master->bd_delay;
}

/**
 * \brief TCP模式是否将地址作为UID
 *
 * \param[in] p_master  主站设备
 *
 * \return AW_TRUE   TCP模式中,UID 作为从机地址；
 *         AW_FALSE  TCP模式中,UID 不作为从机地址。
 */
aw_static_inline aw_bool_t aw_mbm_is_uid_as_addr (aw_mb_m_dev_t *p_master)
{
    return p_master->uid_as_addr;
}

/**
 * \brief 获取TCP模式超时时间获取
 *
 * \param[in] p_master  主站设备
 *
 * \return    超时时间
 */
aw_static_inline uint32_t aw_mb_tcp_contm_get (aw_mb_m_dev_t *p_master)
{
    return p_master->tcp_contm;
}

/**
 * \brief 分配用户功能码
 *
 * \param[in] p_master  主站设备
 * \param[in] num       用户功能码数
 *
 * \return 若无错误返回AW_MB_ERR_NOERR
 */
aw_mb_err_t aw_mb_master_user_funcode_alloc (aw_mb_m_dev_t *p_master,
                                             uint32_t       num);

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_m_netif */

#endif /* } __AW_MB_M_NETIF_H */

/* end of file */
