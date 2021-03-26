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
 * \brief ͨ��CAN�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_can.h
 * \code
 * #include aw_can.h
 * \endcode
 *
 * \par ��ʾ��
 * \code
 * #include "aworks.h"
 * #include "aw_can.h"
 * #include "aw_prj_params.h"
 *
 * static uint32_t __can_timestamp_get_cb (uint32_t timestamp)
 * {
 *     return timestamp;
 * }
 *
 * static aw_bool_t __can_err_sta_cb (
 *     int chn, aw_can_bus_err_t err, aw_can_bus_status_t status)
 * {
 *     aw_can_err_cnt_t  can_err_reg;
 *
 *     aw_can_err_cnt_get(chn, &can_err_reg);
 *     if ((can_err_reg.tx_err_cnt > 127) || (can_err_reg.rx_err_cnt > 127)) {
 *         aw_can_err_cnt_clr(chn);
 *     }
 *
 *     return AW_TRUE; // ��������ֵ����������Ϣ�Ƿ����rngbuf,
 *                     // AW_TRUE:������,AW_FALSE:����
 * }
 *
 * static aw_can_app_callbacks_t __g_can_app_callback_fun = {
 *     __can_timestamp_get_cb,
 *     NULL,
 *     NULL,
 *     __can_err_sta_cb,
 *     NULL
 * };
 *
 * static aw_can_baud_param_t __g_can_btr = AW_CFG_CAN_BTR_500K;
 *
 * void aw_main(void)
 * {
 *     int can_id = 0;
 *
 *     aw_can_init(can_id,                      // CANͨ����
 *                 AW_CAN_WORK_MODE_NORMAL,     // ����������ģʽ
 *                 &__g_can_btr,                // ��ǰ������
 *                 &__g_can_app_callback_fun);  // �û��ص�����
 *     aw_can_start(can_id);
 * }
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.03 18-09-21  anu, modify demo
 * - 1.02 18-06-20  anu, modify aw_can_baud_param_t val not add 1
 * - 1.01 17-08-30  anu, modify
 * - 1.00 15-12-02  cae, first implementation
 * \endinternal
 */

#ifndef __AW_CAN_H
#define __AW_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_can
 * \copydoc aw_can.h
 * @{
 */

#include "aw_types.h"

/** \brief CAN���������Ͷ��� */
typedef uint8_t aw_can_ctrl_type_t;             /**< \brief CAN���������� */
#define AW_CAN_CTLR_HS                  0x00    /**< \brief CAN */
#define AW_CAN_CTLR_FD                  0x01    /**< \brief CANFD */

/** \brief CAN����ģʽ���� */
typedef uint8_t aw_can_work_mode_type_t;        /**< \brief CAN����ģʽ */
#define AW_CAN_WORK_MODE_NORMAL         0x00    /**< \brief ��������ģʽ */
#define AW_CAN_WORK_MODE_LISTEN_ONLY    0x01    /**< \brief ֻ������ģʽ */
#define AW_CAN_WORK_MODE_NON_ISO        0x02    /**< \brief ��ISO��׼�Ĺ���ģʽ(������׼) */

/** \brief CAN���ߴ����� */
typedef uint32_t aw_can_bus_err_t;              /**< \brief CAN���ߴ������� */
#define AW_CAN_BUS_ERR_NONE             0x00    /**< \brief �޴��� */
#define AW_CAN_BUS_ERR_BIT              0x01    /**< \brief λ���� */
#define AW_CAN_BUS_ERR_ACK              0x02    /**< \brief Ӧ����� */
#define AW_CAN_BUS_ERR_CRC              0x04    /**< \brief CRC���� */
#define AW_CAN_BUS_ERR_FORM             0x08    /**< \brief ��ʽ���� */
#define AW_CAN_BUS_ERR_STUFF            0x10    /**< \brief ������ */
#define AW_CAN_BUS_ERR_DATAOVER         0x20    /**< \brief ���ش��� */
#define AW_CAN_BUS_ERR_RCVBUFF_FULL     0x40    /**< \brief ���ջ������� */
#define AW_CAN_BUS_ERR_TXDBUFF_FULL     0x80    /**< \brief ���ͻ������� */

/** \brief CAN����״̬���� */
typedef uint32_t aw_can_bus_status_t;           /**< \brief CAN����״̬���� */
#define AW_CAN_BUS_STATUS_OK            0x00    /**< \brief �������� */
#define AW_CAN_BUS_STATUS_ERROR         0xE1    /**< \brief ���ߴ��� */
#define AW_CAN_BUS_STATUS_WARN          0xE2    /**< \brief ���߾��� */
#define AW_CAN_BUS_STATUS_PASSIVE       0xE3    /**< \brief �������� */
#define AW_CAN_BUS_STATUS_OFF           0xE4    /**< \brief ���߹ر� */
#define AW_CAN_BUS_STATUS_DATAOVER      0xE5    /**< \brief ���߳��� */
#define AW_CAN_BUS_STATUS_WAKE          0xE6    /**< \brief ���߻��� */

/** \brief CAN��������ֵ������붨�� */
typedef int aw_can_err_t;                       /**< \brief CAN������ */

/** \brief �޴��� */
#define AW_CAN_ERR_NONE                       0     

/** \brief ������� */                 
#define AW_CAN_ERR_BASE                       0          

/** \brief ����æ */            
#define AW_CAN_ERR_BUSY                      (AW_CAN_ERR_BASE + 80) 

/** \brief �豸δ��ʼ�� */ 
#define AW_CAN_ERR_NOT_INITIALIZED           (AW_CAN_ERR_BASE + 81)  

/** \brief ��ע���������� */
#define AW_CAN_ERR_NONE_DRIVER_FUNC          (AW_CAN_ERR_BASE + 82)

/** \brief �Ƿ�ͨ���� */  
#define AW_CAN_ERR_ILLEGAL_CHANNEL_NO        (AW_CAN_ERR_BASE + 83) 

/** \brief �Ƿ�ͨ��ģʽ */ 
#define AW_CAN_ERR_ILLEGAL_CHANNEL_MODE      (AW_CAN_ERR_BASE + 84) 

/** \brief �Ƿ��Ŀ��������� */ 
#define AW_CAN_ERR_ILLEGAL_CTRL_TYPE         (AW_CAN_ERR_BASE + 85) 

/** \brief ���������Ͳ�ƥ�� */ 
#define AW_CAN_ERR_INCOMPATIBLE_CTRL_TYPE    (AW_CAN_ERR_BASE + 86)  

/** \brief ������������ */
#define AW_CAN_ERR_CTRL_STARTED              (AW_CAN_ERR_BASE + 87)  

/** \brief ������δ���� */
#define AW_CAN_ERR_CTRL_NOT_START            (AW_CAN_ERR_BASE + 88)  

/** \brief �޿ɲ�����Դ */
#define AW_CAN_ERR_NO_OP                     (AW_CAN_ERR_BASE + 89) 

/** \brief �豸������ */ 
#define AW_CAN_ERR_NO_DEV                    (AW_CAN_ERR_BASE + 90)  

/** \brief �Ƿ����ݳ��� */
#define AW_CAN_ERR_ILLEGAL_DATA_LENGTH       (AW_CAN_ERR_BASE + 91)  

/** \brief �Ƿ�������ֵ */
#define AW_CAN_ERR_ILLEGAL_MASK_VALUE        (AW_CAN_ERR_BASE + 92)  

/** \brief ��Ч���� */
#define AW_CAN_ERR_INVALID_PARAMETER         (AW_CAN_ERR_BASE + 93)  

/** \brief �Ƿ�ƫ��ֵ */
#define AW_CAN_ERR_ILLEGAL_OFFSET            (AW_CAN_ERR_BASE + 94)  

/** \brief ����ģʽ������ */
#define AW_CAN_ERR_SLEEP_IN_CFG_MODE         (AW_CAN_ERR_BASE + 95)  

/** \brief �޷����û����ж� */
#define AW_CAN_ERR_CANNOT_SET_WAKEUPINT      (AW_CAN_ERR_BASE + 96) 

/** \brief �޷��������߹ر��ж� */ 
#define AW_CAN_ERR_CANNOT_SET_BOFFINT        (AW_CAN_ERR_BASE + 97)  

/** \brief �޷����ô����ж� */
#define AW_CAN_ERR_CANNOT_SET_ERRINT         (AW_CAN_ERR_BASE + 98)  

/** \brief �����ʴ��� */
#define AW_CAN_ERR_BAUDRATE_ERROR            (AW_CAN_ERR_BASE + 99)  

/** \brief ��ֹ���� */
#define AW_CAN_ERR_ABORT_TRANSMITTED         (AW_CAN_ERR_BASE + 100) 

/** \brief ���䳬ʱ */
#define AW_CAN_ERR_TRANSMITTED_TIMEOUT       (AW_CAN_ERR_BASE + 101) 

/** \brief ���ߴ��� */
#define AW_CAN_ERR_BUS_FAULT                 (AW_CAN_ERR_BASE + 102) 

/** \brief �޽��ջ����� */
#define AW_CAN_ERR_RXD_BUFF_UNUSED           (AW_CAN_ERR_BASE + 103) 

/** \brief ��ע����պ��� */
#define AW_CAN_ERR_NONE_RCV_FUNC             (AW_CAN_ERR_BASE + 104) 

/** \brief ��֧�ָò��� */
#define AW_CAN_ERR_NOT_SUPPORT               (AW_CAN_ERR_BASE + 105) 

/** \brief δ֪���� */
#define AW_CAN_ERR_UNKNOWN_ERROR             (AW_CAN_ERR_BASE + 255) 

/** \brief CAN���������ݳ��ȶ��� */
#define AW_CAN_HS_MAX_LENGTH                 8        /**< \brief CAN���ݳ��� */
#define AW_CAN_FD_MAX_LENGTH                 64       /**< \brief CANFD���ݳ���*/

/** \brief CAN ID�����붨�� */
#define AW_CAN_STD_IDS_MASK                  0x7FF      /**< \brief CAN��׼֡ID������ */
#define AW_CAN_EXT_IDS_MASK                  0x1FFFFFFF /**< \brief CAN��չ֡ID������ */

/** \brief CAN �����ĳ�ʱ���� */
#define AW_CAN_WAIT_FOREVER                  AW_WAIT_FOREVER /**< \brief CAN���õȴ� */
#define AW_CAN_NO_WAIT                       AW_NO_WAIT      /**< \brief CAN�޵ȴ� */

/** \brief CAN��Ϣ�ı�־λ���� */
typedef uint32_t aw_can_msg_flag_t;                    /**< \brief CAN��־ */
#define AW_CAN_MSG_FLAG_SEND_NORMAL          0x0000    /**< \brief �������� */
#define AW_CAN_MSG_FLAG_SEND_ONCE            0x0001    /**< \brief ���η��� */
#define AW_CAN_MSG_FLAG_SEND_SELF            0x0002    /**< \brief �Է����� */
#define AW_CAN_MSG_FLAG_SEND_SELFONCE        0x0003    /**< \brief �����Է����� */
#define AW_CAN_MSG_FLAG_CAN_CTRL             0x0000    /**< \brief ��ͨCAN���� */
#define AW_CAN_MSG_FLAG_FD_CTRL              0x0010    /**< \brief CANFD���� */
#define AW_CAN_MSG_FLAG_REMOTE               0x0100    /**< \brief Զ��֡��� */
#define AW_CAN_MSG_FLAG_EXTERND              0x0200    /**< \brief ��չ֡��� */
#define AW_CAN_MSG_FLAG_ERROR                0x0400    /**< \brief ������ */
#define AW_CAN_MSG_FLAG_BRS                  0x0800    /**< \brief �л������� */
#define AW_CAN_MSG_FLAG_ESI                  0x1000    /**< \brief ���Ͷ˱�������*/

/** \brief ��Ϣ��־�з����������� */
#define AW_CAN_MSG_FLAG_SEND_TYPE_MASK       0x000F    
/** \brief ��Ϣ��־�б����������� */
#define AW_CAN_MSG_FLAG_CTRL_TYPE_MASK       0x00F0    

/** \brief ��Ϣ��־�з������͵�ƫ��λ */
#define AW_CAN_MSG_FLAG_SEND_TYPE_SHIFT      0x0000    
/** \brief ��Ϣ��־�б������͵�ƫ��λ */
#define AW_CAN_MSG_FLAG_CTRL_TYPE_SHIFT      0x0004    

/** \brief CAN��Ϣ����ͷ���� */
typedef struct aw_can_msg {
    uint32_t timestamp;       /**< \brief ʱ��� */
    uint32_t id;              /**< \brief ֡ID */

    aw_can_msg_flag_t flags;  /**< \brief ������Ϣ����־ */

    uint8_t resv[2];          /**< \brief �������ں�����չ */

    uint8_t chn;              /**< \brief ��������ͨ���� */
    uint8_t length;           /**< \brief ���ĳ��� */
} aw_can_msg_t;

/** \brief CAN��׼���Ķ��� */
typedef struct aw_can_std_msg {
    aw_can_msg_t      can_msg;                       /**< \brief CAN ������Ϣͷ */
    uint8_t           msgbuff[AW_CAN_HS_MAX_LENGTH]; /**< \brief CAN �������� */
} aw_can_std_msg_t;

/** \brief CANFD���Ķ��� */
typedef struct aw_can_fd_msg {
    aw_can_msg_t      can_msg;                       /**< \brief CANFD ������Ϣͷ */
    uint8_t           msgbuff[AW_CAN_FD_MAX_LENGTH]; /**< \brief CAN �������� */
} aw_can_fd_msg_t;

/** \brief CAN����� */
typedef struct aw_can_transfer {
    aw_can_msg_t     *p_msgs;         /**< \brief ������CAN���Ļ������׵�ַ*/
    uint16_t          msgs_num;       /**< \brief �����䱨�ĸ��� */
    uint16_t          done_num;       /**< \brief �ɹ�����Ĵ��䱨�ĸ��� */
    aw_pfuncvoid_t    pfunc_complete; /**< \brief ������ɻص�����,�첽������Ч*/
    void             *p_arg;          /**< \brief ���ݸ� pfunc_complete �Ĳ��� */
    
    /**
     * \brief ��ǰ������״̬����:
     *                -AW_ENOTCONN   :δ�Ŷ�
     *                -AW_EISCONN    :�Ŷ�
     *                -AW_EINPROGRESS:����
     *                -AW_ETIME      :��ʱ
     *                -AW_ENETDOWN   :���߹رգ��豸ֹͣ
     *                 AW_OK         :���
     */
    int               status;         
    void             *ctlrdata[2];    /**< \brief ������ʹ�� */
} aw_can_transfer_t;

/**
 * \brief CAN��׼�����ʶ���
 *
 * ----------------------------------------------------------
 * | sync |        tseg1      |   tseg2                     |
 * |                     sample point                       |
 * ----------------------------------------------------------
 * f(tq)= fpclk(can����ʱ��) / brp����Ƶֵ��
 *
 * baudrate = ftq / (sync + tseg1 + tseg2)
 *
 * sync: ͬ���� sync = 1
 *
 * sjw: ͬ����ת���
 *
 * tseg1: ��λ�����1: tseg1��Tseg1����ϸ�ο�CAN����������
 *
 * tseg2: ��λ�����2: tseg2��tseg2 >= sjw
 *
 * smp: ��������: �������������ߴ�����ͨ������Ϊ���β���
 *
 * brp: ��Ƶֵ
 *
 */
typedef struct aw_can_baud_param {
    uint8_t     tseg1;          /**< \brief tseg1 ��λ��1 */
    uint8_t     tseg2;          /**< \brief tseg2 ��λ��2 */
    uint8_t     sjw;            /**< \brief sjw ͬ����ת��� */
    uint8_t     smp;            /**< \brief smp ����ģʽ0:���� 1:���� */
    uint16_t    brp;            /**< \brief brp ��Ƶֵ */
} aw_can_baud_param_t;

/** \brief CANFD�����ʶ��� */
typedef struct aw_can_fd_baud_param {
    aw_can_baud_param_t nominal_baud;  /**< \brief �ٲ���CRC��Ȳ����� */
    aw_can_baud_param_t data_baud;     /**< \brief ���������� */
} aw_can_fd_baud_param_t;

/** \brief ������������� */
typedef struct aw_can_err_cnt{
    uint8_t rx_err_cnt;         /**< \brief rx_error_cnt  ���մ�������� */
    uint8_t tx_err_cnt;         /**< \brief tx_error_cnt  ���ʹ�������� */
} aw_can_err_cnt_t;

/** \brief ʱ�����ȡ�Ļص����� */
typedef uint32_t (*aw_can_timestamp_get_callback_t) (uint32_t);

/** \brief �������ݵĻص����� */
typedef aw_bool_t (*aw_can_proto_rx_callback_t) (int chn, aw_can_msg_t* p_msg);

/** \brief �������ݵĻص����� */
typedef aw_bool_t (*aw_can_proto_tx_callback_t) (int chn);

/** \brief ���ߴ���״̬�Ļص����� */
typedef aw_bool_t (*aw_can_bus_err_sta_callback_t) (
        int chn, aw_can_bus_err_t err, aw_can_bus_status_t status);

/** \brief �豸���ѵĻص����� */
typedef aw_bool_t (*aw_can_dev_wakeup_callback_t) (int chn);

/** \brief Ӧ�ÿ����õĻص����� */
typedef struct aw_can_app_callbacks {

    /** \brief  ��ȡϵͳʱ����ص�������ַ  */
    aw_can_timestamp_get_callback_t  timestamp_get_cb;

    /**
     * \brief  ���߽���һ֡���Ļص�������ַ
     * ��������ֵ�������յ��ı����Ƿ����rngbuf�� AW_TRUE:�����棬FALSE:����
     */
    aw_can_proto_rx_callback_t       proto_rx_cb;

    /**
     * \brief  ���߷�����ɻص�������ַ
     * ��������ֵ�����Ƿ��Զ������ͱ��ģ� 
     *              AW_TRUE :�ص����д���
     *              AW_FALSE:�Զ�������
     */
    aw_can_proto_tx_callback_t       proto_tx_cb;

    /**
     * \brief  Ӧ�ò��������״̬����ص�������ַ
     * ��������ֵ����������Ϣ�Ƿ����rngbuf�� 
     *              AW_TRUE :������
     *              AW_FALSE:����
     */
    aw_can_bus_err_sta_callback_t    bus_err_sta_cb;

    /**
     * \brief  ���߻��ѻص�������ַ
     * ��������ֵ����������Ϣ�Ƿ����rngbuf
     *              AW_TRUE :������
     *              AW_FALSE:����
     */
    aw_can_dev_wakeup_callback_t     dev_wakeup_cb;

} aw_can_app_callbacks_t;

/**
 * \brief CAN��ʼ����������ʼ��ͨ����ģʽ�������ʣ��ص������Ȳ���
 *
 * \param[in] chn        ѡ��ͨ����chn: 0~AW_CFG_NUM_CAN
 *
 * \param[in] work_mode  ���� ����ģʽ��
 *                       AW_CAN_WORK_MODE_NORMAL(����ģʽ)��
 *                       AW_CAN_WORK_MODE_LISTEN_ONLY��ֻ��ģʽ��
 *
 * \param[in] p_baud     ���ò����ʣ������ʲ����ο��ṹ��˵����
 *                       ����������ΪCANʱ�������ʲ���Ϊaw_can_baud_param_t
*                        ����������ΪCANFDʱ�� �����ʲ���Ϊaw_can_fd_baud_param_t
 *
 * \param[in] p_app_cb   �û��ص���������Ӧ���¼����ص��û�APP��������ΪNULL
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_init (int                        chn,
                          aw_can_work_mode_type_t    work_mode,
                          const aw_can_baud_param_t *p_baud,
                          aw_can_app_callbacks_t    *p_app_cb);

/**
 * \brief ����CAN��������ʹCAN��������������ģʽ�����Խ���ͨ��
 *
 * \param[in] chn           ͨ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_start (int chn);

/**
 * \brief ֹͣCAN��������ʹCAN��������������ģʽ��CAN����������������ͨ��
 *
 * \param[in] chn           ͨ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_stop (int chn);

/**
 * \brief ��CAN����������˯��ģʽ
 *
 * \param[in] chn           ͨ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_sleep (int chn);

/**
 * \brief ����CAN������
 *
 * \param[in] chn           ͨ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_wakeup (int chn);

/**
 * \brief ����CAN������
 *
 * \attention ��׼CANʱ�������ʲ���Ϊaw_can_baud_param_t
 * \attention CANFDʱ�������ʲ���Ϊaw_can_fd_baud_param_t
 *
 * \param[in] chn           ͨ����
 *
 * \param[in] p_baud        �����ʲ���
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_baudrate_set (int chn, aw_can_baud_param_t *p_baud);

/**
 * \brief ��ȡCAN������
 *
 * \attention ��׼CANʱ�������ʲ���Ϊaw_can_baud_param_t
 * \attention CANFDʱ�������ʲ���Ϊaw_can_fd_baud_param_t
 *
 * \param[in] chn           ͨ����
 *
 * \param[in] p_baud        �����ʲ���
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_baudrate_get (int chn, aw_can_baud_param_t *p_baud);

/**
 * \brief ������ͺͽ��ܴ������
 *
 * \param[in] chn           ͨ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_err_cnt_clr (int chn);

/**
 * \brief ��ȡ���ͺͽ��մ��������
 *
 * \param[in] chn       ͨ����
 *
 * \param[in] p_err_cnt ����Ĵ���ֵָ��
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_err_cnt_get (int chn, aw_can_err_cnt_t *p_err_cnt);

/**
 * \brief ����CAN�˲���
 *
 * \note �˽ӿں���Ϊͨ�ø�ʽ����ֻ�б��׵�ַ�ͱ��ȣ��ֽڸ�������
 *       ��ͬ�Ŀ������˲���ͬ����ʽҲ��ͬ����˾���������˲���ʽ
 *       ��ο���Ӧ���������̡�
 *
 * \param[in] chn           ͨ���š�
 *
 * \param[in] p_filter_buff �˲����׵�ַ
 *
 * \param[in] length        �˲����ȣ��ֽڸ�����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_filter_table_set (
        int chn, uint8_t *p_filter_buff, size_t length);

/**
 * \brief ��ȡCAN�˲���
 *
 * \note �˽ӿں���Ϊͨ�ø�ʽ����ֻ�б��׵�ַ�ͱ��ȣ��ֽڸ�������
 *       ��ͬ�Ŀ������˲���ͬ����ʽҲ��ͬ����˾���������˲���ʽ
 *       ��ο���Ӧ���������̡�
 *
 * \param[in] chn           ͨ����
 *
 * \param[in] p_filter_buff �˲����׵�ַ
 *
 * \param[in] p_length      �����˲����ȣ��ֽڸ�����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_filter_table_get (int       chn, 
                                      uint8_t  *p_filter_buff,
                                      size_t   *p_length);

/**
 * \brief ����CAN����ṹ�����
 *
 * \param p_transfer        CAN���Ĵ���������ָ��
 * \param p_msgs            ��ɴ����ı���
 * \param msgs_num          ���ĵĸ���
 * \param pfunc_complete    ������ɻص�����
 *                          (������ aw_can_msgs_send_async() ʱ�˲�����Ч,
 *                          ���� aw_can_msgs_send_sync()ʱ��������Ч)
 * \param p_arg             ���ݸ��ص������Ĳ���
 *
 * \return ��
 *
 * \par ����
 * \code
 * #include "aw_can.h"
 *
 * aw_can_transfer_t transfer;  // ����������ṹ��
 * aw_can_std_msg_t  msgs[2];   // ��ɴ����ı��Ļ�����
 *
 * // ���ô��������ṹ��
 * aw_i2c_mktrans(&transfer,    // ����������ṹ
 *                msgs,         // ��ɴ����ı��Ļ������׵�ַ
 *                2��                       // �������
 *                my_callback,  // ������ɻص�����
 *                my_arg);      // ������ɻص������Ĳ���
 *
 *  ����ʵ�֣�
 *  p_transfer->p_msgs         = (aw_can_msg_t*)p_msgs;
 *  p_transfer->msgs_num       = msgs_num;
 *  p_transfer->done_num       = 0;
 *  p_transfer->pfunc_complete = pfunc_complete;
 *  p_transfer->p_arg          = p_arg;
 *  p_transfer->status         = -AW_ENOTCONN;
 *
 * \endcode
 */
 void aw_can_mkmsg (aw_can_transfer_t  *p_transfer,
                    aw_can_msg_t       *p_msgs,
                    uint16_t            msgs_num,
                    aw_pfuncvoid_t      pfunc_complete,
                    void               *p_arg);

/**
 * \brief CAN�����첽����
 *
 * \note ����һ��CAN���ĵ��첽���䣬�ӿڲ���ʹ�ô���飬
 *       �����ǰ�д�������ڴ�������´������ӵ������б�
 *       �û��Ĵ�����������Ч����飬δ�������֮ǰ�����ͷŸô���飬������ʧ�ܡ�
 *       ��������������ο�����aw_can_mkmsg���ݡ�
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] p_transfer    �����p_transfer�ĵ�ַ
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_msgs_send_async (int chn, aw_can_transfer_t *p_transfer);

/**
 * \brief CAN����ͬ������
 *
 * \note ����һ��ͬ�����䣬����һ��CAN���ģ����ĸ������û�����
 *       �û��Ĵ�����������Ч����飬δ�������֮ǰ�����ͷŸô���飬������ʧ�ܡ�
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] p_transfer    ������ַ
 *
 * \param[in] timeout       ���䳬ʱ��
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_msgs_send_sync (int                chn,
                                    aw_can_transfer_t *p_transfer,
                                    uint32_t           timeout);

/**
 * \brief CANFD���������ͱ���
 *
 * \note CANFD�������ı��ķ��ͺ�����ֻ������ΪCANFD��������ʱ��ʹ�ã�
 *       ���ĸ�ʽ����CANFD���ĸ�ʽ����ο���aw_can_fd_msg_t
 *       �û��Ĵ�����������Ч����飬δ�������֮ǰ�����ͷŸĴ���飬������
 *       ʧ�ܡ�
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] p_msgs        FD���Ļ�������ַ
 *
 * \param[in] msgs_num      ���������ĸ���
 *
 * \param[in] p_done_num    ʵ�ʷ��ͳɹ�������ַ
 *
 * \param[in] timeout       ���ͳ�ʱʱ�䣬�����0
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_fd_msgs_send (int              chn,
                                  aw_can_fd_msg_t *p_msgs,
                                  uint32_t         msgs_num,
                                  uint32_t        *p_done_num,
                                  uint32_t         timeout);

/**
 * \brief ����CAN���������ͱ���
 *
 * \note ����CAN�������ı��ķ��ͺ�����ֻ������Ϊ����CAN��������ʱ��ʹ�ã�
 *       ���ĸ�ʽ���ñ�׼CAN���ĸ�ʽ����ο���aw_can_std_msg_t
 *       �û��Ĵ�����������Ч����飬δ�������֮ǰ�����ͷŸĴ���飬������ʧ�ܡ�
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] p_msgs        ��׼���Ļ�������ַ
 *
 * \param[in] msgs_num      ���������ĸ���
 *
 * \param[in] p_done_num    ʵ�ʷ��ͳɹ�������ַ
 *
 * \param[in] timeout       ���ͳ�ʱʱ�䣬�����0
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_std_msgs_send (int               chn,
                                   aw_can_std_msg_t *p_msgs,
                                   uint32_t          msgs_num,
                                   uint32_t         *p_done_num,
                                   uint32_t          timeout);

/**
 * \brief ֹͣ���ͺ���
 *
 * \attention �����첽����ֹͣ���ͣ��û����ô˽ӿ�aw_can_msgs_send_async��
 *            ʹ��ͬ�����䣬������ʱ�¼���ϵͳ���Զ�����ֹͣ���䣬�û�������á�
 *            �û���ֹͣ��ǰ���ķ��ͣ������������б�
 *
 * \param[in] chn           CANͨ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_msgs_stop_send (int chn);

/**
 * \brief ��ȡ���ջ������Ѿ�����can���ĵĸ�����
 *
 * \param[in] chn       CANͨ����
 *
 * \return frame count  ���������ĸ���
 *
 */
uint32_t aw_can_rcv_msgs_num (int chn);

/**
 * \brief ������ֻ��С��
 *
 * \param[in] chn      CANͨ����
 *
 * \return  ������ʵ�ʴ�С�����ĸ�����
 *
 */
uint32_t aw_can_rcv_fifo_size (int chn);

/**
 * \brief ����CAN����
 *
 * \note ����CAN���ģ�������ֻҪ�����ݾͻ�ɹ����أ�
 *       ʵ�ʸ���ֵ��aw_can_transfer_t��Ԫ��done_num�У�
 *       ֻ�л�����û�����ݵ�ʱ�򣬳�ʱʱ����Ч��
 *       Ϊ�˱�֤��������ʵʱ�ԣ��ڳ�ʱ�ȴ�ʱ�򣬵����յ���Ч���ݣ����̷��ء�
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] p_transfer    ������ַ
 *
 * \param[in] timeout       ���ջ��������ޱ��ĵĳ�ʱʱ��
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_msgs_rcv (int                chn,
                              aw_can_transfer_t *p_transfer,
                              uint32_t           timeout);

/**
 * \brief ����CAN���������ձ���
 *
 * \note ����CAN�������ı��Ľ��պ�����������ֻҪ�����ݾͻ�ɹ����أ�
 *       ֻ������Ϊ����CAN��������ʱ��ʹ��
 *       ʵ�ʸ���ֵ�ڴ����p_done_num�У�
 *       ֻ�л�����û�����ݵ�ʱ�򣬳�ʱʱ����Ч��
 *       Ϊ�˱�֤��������ʵʱ�ԣ��ڳ�ʱ�ȴ�ʱ�򣬵����յ���Ч���ݣ����̷��ء�
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] p_msgs        ���ձ��Ļ�������ַ
 *
 * \param[in] msgs_num      ���������ĸ���
 *
 * \param[in] p_done_num    ʵ�ʶ�ȡ������ַ
 *
 * \param[in] timeout       ���ջ��������ޱ��ĵĳ�ʱʱ��
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_std_msgs_rcv (int               chn,
                                  aw_can_std_msg_t *p_msgs,
                                  uint32_t          msgs_num,
                                  uint32_t         *p_done_num,
                                  uint32_t          timeout);

/**
 * \brief CANFD���������ձ���
 *
 * \note CANFD�������ı��Ľ��պ�����������ֻҪ�����ݾͻ�ɹ����أ�
 *       ֻ������ΪCANFD��������ʱ��ʹ��
 *       ʵ�ʸ���ֵ�ڴ����p_done_num�У�
 *       ֻ�л�����û�����ݵ�ʱ�򣬳�ʱʱ����Ч��
 *       Ϊ�˱�֤��������ʵʱ�ԣ��ڳ�ʱ�ȴ�ʱ�򣬵����յ���Ч���ݣ����̷��ء�
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] p_msgs        ����FD���Ļ�������ַ
 *
 * \param[in] msgs_num      ���������ĸ���
 *
 * \param[in] p_done_num    ʵ�ʶ�ȡ������ַ
 *
 * \param[in] timeout       �ޱ��ĳ�ʱʱ��
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_fd_msgs_rcv (int              chn,
                                 aw_can_fd_msg_t *p_msgs,
                                 uint32_t         msgs_num,
                                 uint32_t        *p_done_num,
                                 uint32_t         timeout);

/**
 * \brief дCAN�Ĵ���
 *
 * \note дCAN�Ĵ�����������CAN������ܲ������ģ������CAN����
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] offset        �Ĵ�����ַ
 *
 * \param[in] p_data        ����
 *
 * \param[in] length        ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_reg_write (int        chn,
                               uint32_t   offset,
                               uint8_t   *p_data,
                               size_t     length);

/**
 * \brief ��ȡCAN�Ĵ���
 *
 * \note ��ȡCAN�Ĵ�����������CAN������ܲ������ģ������CAN����
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] offset        �Ĵ�����ַ
 *
 * \param[in] p_data        ��ȡ����
 *
 * \param[in] length        ��ȡ����
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_reg_read (int        chn,
                              uint32_t   offset,
                              uint8_t   *p_data,
                              size_t     length);
/**
 * \brief ��CAN�Ĵ���д���ģ�
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] p_msg         ����ָ��
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_reg_msg_write (int chn, aw_can_msg_t *p_msg);

/**
 * \brief ��ȡCAN����
 *
 * ��CAN�Ĵ����ж�ȡ����
 *
 * \param[in] chn           CANͨ����
 *
 * \param[in] p_msg         ����ָ��
 *
 * \return AW_CAN_ERR_NONE�����ɹ�����������ԭ��ο�������
 *
 */
aw_can_err_t aw_can_reg_msg_read (int chn, aw_can_msg_t *p_msg);

/** @} grp_aw_if_can */

#ifdef __cplusplus
}
#endif

#endif /* __AW_CAN_H */

/* end of file */
