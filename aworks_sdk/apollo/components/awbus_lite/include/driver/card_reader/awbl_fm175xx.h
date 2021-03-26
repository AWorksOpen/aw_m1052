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
 * \brief fm175xx mifare card
 *
 * \internal
 * \par modification history:
 * - 1.03 18-11-10  mkr, add PICCB and LPCD features
 * - 1.02 18-1-10   mkr, solve copybits() function bugs
 * - 1.01 18-1-5    mkr, solve awbl_fm175xx_iso14443_anticoll() function bugs
 * - 1.00 17-11-30  mkr, first implementation
 * \endinternal
 */

#ifndef AWBL_FM175XX_H
#define AWBL_FM175XX_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aw_spi.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "awbl_spibus.h"

#define AWBL_FM175XX_NAME "awbl_FM175xx"

/**
 * \brief ISO/IEC ����
 * @{
 */
#define AWBL_ISO_IEC_14443_A          (0x01U)
#define AWBL_ISO_IEC_14443_B          (0x02U)

/* @} */

/** \brief ֧�ֵ�ͨѶ���� */
#define AWBL_FM175XX_COMM_SPEECH_106K (0) /**< \brief 106Kbps */
#define AWBL_FM175XX_COMM_SPEECH_212K (1) /**< \brief 212Kbps */
#define AWBL_FM175XX_COMM_SPEECH_424K (2) /**< \brief 424Kbps */

/** \brief ������Ϣ�ṹ�� */
typedef struct awbl_fm175xx_cfg_param {
    /** \brief ��Ƶ����ܽţ� 0:TX1, 1:TX2 */
    uint8_t  rf_tx_pin;

    /** \brief �������ܹ����ս����ź���Сǿ�� */
    uint8_t  rx_min_level;

    /** \brief ������������ͻλ��С�ź�ǿ�� */
    uint8_t  rx_coll_min_level;

    /** \brief ���ƿ�� */
    uint8_t modwidth;

    /** \brief ���������� */
    uint8_t rx_gain;

    /** \brief NMOS�����ز��絼ֵ */
    uint8_t cwgsn;

    /** \brief NMOS���Ƶ絼ֵ */
    uint8_t modgsn;

    /** \brief PMOS�����ز��絼ֵ */
    uint8_t cwgsp;

    /** \brief PMOS���Ƶ絼ֵ */
    uint8_t modgsp;

    /** \brief ͨѶ���� */
    uint8_t comm_speed;

    /** \brief Э������ */
    uint8_t prtl_type;
} awbl_fm175xx_cfg_param_t;

/* A�࿨��һ�ֵ������� */
#define AWBL_FM175XX_CFG_TYPEA_1       \
    {                                  \
        0x01,                          \
        0x05,                          \
        0x05,                          \
        0x26,                          \
        0x04,                          \
        0x0F,                          \
        0x08,                          \
        0x3F,                          \
        0x20,                          \
        AWBL_FM175XX_COMM_SPEECH_106K, \
        AWBL_ISO_IEC_14443_A           \
    }

/* B�࿨��һ�ֵ������� */
#define AWBL_FM175XX_CFG_TYPEB_1       \
    {                                  \
        0x00,                          \
        0x05,                          \
        0x05,                          \
        0x26,                          \
        0x05,                          \
        0x0F,                          \
        0x04,                          \
        0x3F,                          \
        0x20,                          \
        AWBL_FM175XX_COMM_SPEECH_106K, \
        AWBL_ISO_IEC_14443_B           \
    }

/** \brief FM175xx device information */
typedef struct awbl_fm175xx_devinfo {
    /** \brief SPIģʽ */
    uint16_t spi_mode;

    /** \brief SPI�ӿ�ʹ�ܹܽ� */
    int      nss_pin;

    /** \brief ��λ/���߿��ƽ� */
    int      npd_pin;

    /** \brief irq�жϹܽ� */
    int      irq_pin;

    /** \brief SPI ���� */
    uint32_t spi_speed;

    /**
     * \brief FM175xx֧�ֵ�Э������,
     * \ref ISO/IEC ����, �� AWBL_ISO_IEC_14443_A ֧�ֶ���Э��ʹ��������Ļ�ֵ
     */
    uint32_t prtl_type;

    /** \brief ��Ƭ��Ӧ��оƬĬ�����ã�
     *  - AWBL_FM175XX_CFG_TYPEA_1
     *  - AWBL_FM175XX_CFG_TYPEB_2
     */
    awbl_fm175xx_cfg_param_t config_type;

    /** \brief ƽ̨��ʼ���ص����� */
    void (*pfunc_plfm_init)(void);
} awbl_fm175xx_devinfo_t;

/** \brief FM175xx device */
typedef struct awbl_fm175xx_dev {
    /** \brief SPI �豸 */
    struct awbl_spi_device  spi_dev;

    /** \brief �豸�� */
    AW_MUTEX_DECL(devlock);
    AW_MUTEX_DECL(req_lock);

    /** \brief ��Ƶ����ܽ� */
    uint8_t rf_tx_pin;

    /** \brief �������� */
    uint8_t speed;

    /** \brief lpcd pointer */
    void *p_lpcd;
} awbl_fm175xx_dev_t;

/******************************************************************************/
/**
 * \brief �豸��λ�����мĴ�����������ָ��ɳ�������
 * \note  �豸��λ�󣬿���ʹ�� awbl_fm175xx_config �ӿ���������
 *
 * \param[in] id �豸ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_reset (uint8_t id);

/**
 * \brief FM175xx ����
 *
 * \param[in] id         FM175xx �豸ID
 * \param[in] type       ��Ƭ����
 * \param[in] cfg        ���ò���
 *
 * \note ����˵����
 *  +------+--------+--------+--------+
 *  | ����     | ����TX  | GSPMos | GSNMos |
 *  +======+========+========+========+
 *  | 0    | 0      | pMod   | nMod   |
 *  +------+--------+--------+--------+
 *  | 1    | RF_n   | pCW    | nCW    |
 *  +------+--------+--------+--------+
 *
 *  - RF_n    13.56MHz����ʱ��
 *  - GSPMos  �絼��PMOS������
 *  - GSNMOS  �絼��NMOS������
 *  - pCW     �� cwgsp �����PMOS�����ز��絼ֵ
 *  - pMod    ��  modgsp �����PMOS���Ƶ絼ֵ
 *  - nCW     �� cwgsn �����NMOS�����ز��絼ֵ
 *  - nMod    ��  modgsn �����NMOS���Ƶ絼ֵ
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_config (uint8_t                   id,
                              awbl_fm175xx_cfg_param_t *p_param);


/**
 * \brief ���豸��������
 *
 * \param[in] id         FM175xx �豸ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_lock (uint8_t id);

/**
 * \brief ���豸����
 *
 * \param[in] id         FM175xx �豸ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_unlock (uint8_t id);

/**
 * \brief ʹ��������Ʋ�
 *
 * \param[in] id  �豸ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_rf_on (uint8_t id);

/**
 * \brief ����������Ʋ�
 *
 * \param[in] id  �豸ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_rf_off (uint8_t id);

/**
 * \brief CRC ����
 *
 * \param[in] id      FM175xx �豸ID
 * \param[in] is_tx   �Ƿ��Ƿ���CRC��
 *                    - AW_TRUE ����CRC
 *                    - AW_FALSE ����CRC
 * \param[in] is_en   �Ƿ�ʹ��
 *
 * \return AW_OK
 */
aw_err_t awbl_fm175xx_crc_set (uint8_t id, aw_bool_t is_tx, aw_bool_t is_en);

/**
 * \brief ��ʱʱ������
 *
 * \param[in] id  FM175xx �豸ID
 * \param[in] ms  ��ʱֵ�����룩
 */
aw_err_t awbl_fm175xx_timeout_set (uint8_t id, uint32_t ms);

/**
 * \brief FM175XX ָ��
 * @{
 */
/** \brief idle */
#define AWBL_FM175XX_CMD_IDLE        (0x00)  
/** \brief activate crc coprocessor */
#define AWBL_FM175XX_CMD_CALCCRC     (0x03)  
/** \brief transmit data in fifo */
#define AWBL_FM175XX_CMD_TRANSMIT    (0x04)  
/** \brief change Command register but un-effect the primitive */
#define AWBL_FM175XX_CMD_NOCMDCHANGE (0x07)  
/** \brief activate receiver */
#define AWBL_FM175XX_CMD_RECEIVE     (0x08)  
/** \brief transmit fifo data and activate receiver */
#define AWBL_FM175XX_CMD_TRANSCEIVE  (0x0C)  
/** \brief execute M1 security authentication */
#define AWBL_FM175XX_CMD_AUTHENT     (0x0E)  
/** \brief software reset */
#define AWBL_FM175XX_CMD_SOFTRESET   (0x0F)  
/* @} */

/** \brief fm175xx ����ṹ��Ϣ */
typedef struct fm175xx_cmd {
    uint8_t        cmd;           /**< \brief ������룬��ѡ AWBL_FM175XX_CMD_* */
    uint8_t        nbits_to_snd;  /**< \brief ���һ���ֽ���Ҫ���͵�λ�� */
    uint8_t        nbytes_to_snd; /**< \brief ׼�����͵��ֽ� */
    uint8_t        nbytes_to_rcv; /**< \brief ׼�����յ��ֽ� */
    uint8_t        nbytes_snded;  /**< \brief �ѷ����ֽ��� */
    uint8_t        nbytes_rcved;  /**< \brief �ѽ��յ��ֽ��� */
    uint8_t        nbits_rcved;   /**< \brief �ѽ��յ�λ��   */

    const uint8_t *p_sndbuf;      /**< \brief �������ݻ����� */
    uint8_t       *p_rcvbuf;      /**< \brief �������ݻ����� */

    uint8_t        collpos;       /**< \brief ��ײλ��       */
    aw_err_t       error;         /**< \brief ����״̬       */
    uint8_t        timeout;       /**< \brief ��ʱʱ��       */

    aw_bool_t      snd_finish;    /**< \brief ������ɱ�־ */
} awbl_fm175xx_cmd_t;

/**
 * \brief cmd construct
 *
 * \param[in] p_cmd  FM175xx ����ṹָ��
 * \param[in] cmd    ����ֵ����ѡ AWBL_FM175XX_CMD_*
 * \param[in] p_tx   ���ͻ�����
 * \param[in] txlen  �����ֽ���
 * \param[in] txbits ���һ�������ֽڿɷ��͵���Чλ����0��ʾ�����ֽڷ���
 * \param[in] p_rx   ���ջ�����
 * \param[in] rxlen  ���ջ���������
 *
 * \return N/A
 */
aw_static_inline void awbl_fm175xx_mkcmd (awbl_fm175xx_cmd_t *p_cmd,
                                          uint8_t             cmd,
                                          const uint8_t      *p_tx,
                                          uint8_t             txlen,
                                          uint8_t             txbits,
                                          uint8_t            *p_rx,
                                          uint8_t             rxlen)
{
    p_cmd->cmd           = cmd;
    p_cmd->p_rcvbuf      = p_rx;
    p_cmd->nbits_to_snd  = txbits;
    p_cmd->nbytes_to_rcv = rxlen;
    p_cmd->nbytes_to_snd = txlen;
    p_cmd->p_sndbuf      = p_tx;
}

/**
 * \brief command execute
 *
 * \param[in] id     FM175XX �豸ID
 * \param[in] p_cmd  ������Ϣָ��
 *
 * \note p_cmd ��Ϣ�Ƽ�ʹ�� awbl_fm175xx_mkcmd ����ʹ�á�
 * \return errno
 */
aw_err_t awbl_fm175xx_cmd_execute (uint8_t id, awbl_fm175xx_cmd_t *p_cmd);

/** \brief оƬģʽѡ�� */
#define AWBL_FM175XX_DEEP_PWRDOWN  (0)  /**< \brief Deep Power Down */
#define AWBL_FM175XX_HARD_PWRDOWN  (1)  /**< \brief Hard Power Down */
#define AWBL_FM175XX_SOFT_PWRDOWN  (2)  /**< \brief Soft Power Down */

/**
 * \brief FM175XX mode setting
 *
 * \param[in] id    FM175XX �豸ID
 * \param[in] mode  ����ģʽ
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_low_power_enter (uint8_t id, uint8_t mode);

/**
 * \brief FM175XX mode setting
 *
 * \param[in] id    FM175XX �豸ID
 * \param[in] mode  �˳�ģʽ
 *
 * \note �˳�AWBL_FM175XX_DEEP_PWRDOWNģʽʱ����Ҫ�������ó�ʼ��FM175XX�豸
 * \return errno
 */
aw_err_t awbl_fm175xx_low_power_exit (uint8_t id, uint8_t mode);

/**
 * \brief LPCD ����
 * @{
 */
#define AWBL_FM175XX_LPCD_BIAS_CURRENT      (0)
/** \brief TX2���ʹ�ܣ�����TX1���� */
#define AWBL_FM175XX_LPCD_TX2RFEN           (1)   
/** \brief N������Ϊ������ */
#define AWBL_FM175XX_LPCD_CWN               (0x1) 
/** \brief P��������������, 0~7������Ĭ��ѡ��3 */
#define AWBL_FM175XX_LPCD_CWP               (0x3) 
/** \brief �͹���ģʽ���� */
#define AWBL_FM175XX_LPCD_MODE              (0x0) 
/** \brief T1�׶����� */
#define AWBL_FM175XX_LPCD_TIMER1_CFG        (3)  
/** \brief T2�׶����� */ 
#define AWBL_FM175XX_LPCD_TIMER2_CFG        (13)  
/** \brief T3�׶����� */
#define AWBL_FM175XX_LPCD_TIMER3_CFG        (11)  
/** \brief Vmid���� */
#define AWBL_FM175XX_VMID_BG_CFG            (8)   
/** \brief ʹ���Զ����ѹ��� */
#define AWBL_FM175XX_AUTO_WUP_EN            (1)   
/**
 * \brief �Զ�����ʱ�䣺
 * - 0 �� 6S
 * - 1 : 12S
 * - 2 : 15minute
 * - 4 : 1hour
 * - 5 : 1.8hour
 * - 6 : 3.6hour
 * - 7 : 7.2hour
 * Ĭ��Ϊ12S���Ƿ���Чȡ�����Զ������Ƿ�ʹ��
 */
#define AWBL_FM175XX_AUTO_WUP_CFG           (1)
#define AWBL_FM175XX_LPCD_AUTO_DETECT_TIMES (0)   /**< \brief T3�׶����� */

typedef struct awbl_fm175xx_lpcd {
    /** \brief LPCD�Ŵ������� */
    uint8_t lpcd_gain_amplify;

    /** \brief LPCD˥������ */
    uint8_t lpcd_gain_reduce;

    /** \brief LPCD��׼���� */
    uint8_t lpcd_bias_current;

    /** \brief ADC�ο���ƽ */
    uint8_t lpcd_adc_reference;

    /** \brief T3�׶ι���ʱ�ӷ�Ƶ�� */
    uint8_t t3_clk_div;

    /** \brief T1ʱ�� */
    uint8_t timer1_cfg;

    /** \brief T2ʱ�� */
    uint8_t timer2_cfg;

    /** \brief T3ʱ�� */
    uint8_t timer3_cfg;

    /** \brief ��������� */
    uint8_t det_sensitive;

    /** \brief T3 ������ADCResult��Ϣ */
    uint8_t adc_result_full_scale;

    /** \brief LPCD��������ֵ */
    uint8_t adc_result_center;

    /** \brief ����ֵ�����ó����ֵ */
    uint8_t adc_result_threshold;

    aw_pfuncvoid_t  lpcd_func;
    void           *p_arg;
} awbl_fm175xx_lpcd_t;

/**
 * \brief LPCD������ʼ��
 *
 * \param[in] p_lpcd    ָ��awbl_fm175xx_lpcd_t��ָ��
 * \param[in] lpcd_func ���ѻ��ⴥ���ص�����
 * \param[in] p_usr     �ص������û�����
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_lpcd_param_init (uint8_t              id,
                                       awbl_fm175xx_lpcd_t *p_lpcd,
                                       aw_pfuncvoid_t       lpcd_func,
                                       void                *p_usr);

/**
 * \brief LPCD ����
 *
 * \param[in] id      FM175XX�豸ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_lpcd_config (uint8_t id);

/**
 * \brief LPCD У׼��ʼ��.
 * \note �жϲ���ȫ
 *
 * \param[in] id             FM175XX�豸ID
 * \param[in] det_sensitive  ������������ã�������Խ���ܹ���⿨�ľ���ԽС��
 *                           ��������4~15%֮�䣬��λΪ1/100��
 *
 * \return
 */
aw_err_t awbl_fm175xx_lpcd_calibra_init (uint8_t id, uint8_t det_sensitive);

/**
 * \brief LPCD ���ƣ�������˳���
 * \note LPCD���¼�����ʱ���Զ��˳�LPCDģʽ���û������Ҫ�ٴν��룬��Ҫ�ٴε��ñ�������
 *
 * \param[in] id       FM175XX�豸ID
 * \param[in] is_enter �Ƿ����LPCD��TRUE-���룻FALSE-�˳�
 *
 * \return
 */
aw_err_t awbl_fm175xx_lpcd_ctrl (uint8_t id, aw_bool_t is_enter);

#define AWBL_FM175XX_LPCD_CARDDET_EVENT     (1U << 0) /**< \brief ��Ƭ����¼� */
#define AWBL_FM175XX_LPCD_AWUP_EVENT        (1U << 4) /**< \brief �Զ������¼� */

/**
 * \brief LPCD �¼�Ԥ����LPCDʱ�䴥��֮��ִ�С�
 * \note �жϲ���ȫ
 *
 * \param[in] id       FM175XX�豸ID
 * \param[out] p_type  �¼�����
 *
 * \return
 */
aw_err_t awbl_fm175xx_lpcd_event_pre_handle (uint8_t id, uint8_t *p_type);

/* @} */

/******************************************************************************/
/**
 * \brief ����ģʽ
 * @{
 */
/** \brief ���������п��п�������������HALT״̬�Ŀ��� */
#define AWBL_FM175XX_PICC_REQ_IDLE  (0) 
/** \brief �������п�����������HALT״̬�Ŀ��� */
#define AWBL_FM175XX_PICC_REQ_ALL   (1) 

/* @} */

/**
 * \brief ��������
 * @{
 */
/** \brief halt */
#define AWBL_PICC_CMD_HALT   (0x50) 

/** \brief TYPE A �������� */
/** \brief request */
#define AWBL_PICC_CMD_REQA   (0x26)  
/** \brief wakeup */
#define AWBL_PICC_CMD_WUPA   (0x52)  

/* @} */

/**
 * \brief PICCA ���ӿڣ�����ISO/IEC 14443 AЭ��
 * @{
 */
/**
 * \brief ��Ƭ(ISO/IEC 14443 Type AЭ��)����
 *
 * \param[in]  id        fm175xx �豸ID
 * \param[in]  req_mode  ����ģʽ:
 *                       - AWBL_FM175XX_PICCA_REQ_IDLE
 *                       - AWBL_FM175XX_PICCA_REQ_ALL
 * \param[out] p_atqa    IC��Ӧ����Ϣ��2�ֽڴ�С��
 *
 * \note �����ſ�ͬ���͵Ŀ�ͬʱ������ʱ�����øýӿھ��ܿ��Խ��յ���Ƭ��Ӧ����Ϣ��
 *       �ڽ��յ���Ӧ����Ϣp_atqa��[bit4 ~ bit0]λ��(Bit frame anticollision)
 *       ��¼��ͻ�����Ĭ������¸�λ�����޳�ͻ����ʱӦ��ֻ��һ��λ��λ�ģ�
 *       ���ǲ�Ӱ������Ĳ�������Ϊ��Ƭ�ѱ����ѡ�
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_picca_request (uint8_t    id,
                                     uint8_t    req_mode,
                                     uint16_t  *p_atqa);

/**
 * \brief ��Ƭ(ISO/IEC 14443 Type AЭ��)����ײ����
 *
 * \param[in]  id      fm175xx �豸ID
 * \param[in]  cl      cascade level
 * \param[out] p_dat   IC��Ӧ����Ϣ���棨5�ֽڴ�С��
 *
 * \note p_dat����Ӧ��Ϊ5�ֽڴ�С������ײ�ɹ��󣬵õ����������£�
 *       4�ֽ�UID + 1�ֽ�BCC ��
 *       1�ֽ�CT + 3�ֽ�UID + 1�ֽ�BCC
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_picca_anticoll (uint8_t  id,
                                      uint8_t  cl,
                                      uint8_t *p_dat);

/**
 * \brief ��Ƭ(ISO/IEC 14443 Type AЭ��)ѡ��
 *
 * \param[in]  id      fm175xx �豸ID
 * \param[in]  cl      cascade level
 * \param[in]  p_dat   IC��Ӧ����Ϣ���棨5�ֽڴ�С��������ײ�еõ�������
 * \param[out] p_sak   selectionӦ����Ϣ
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_picca_select (uint8_t  id,    
                                    uint8_t  cl,
                                    uint8_t *p_dat, 
                                    uint8_t *p_sak);

/**
 * \brief ��Ƭ����(ISO/IEC 14443 Type AЭ��)
 *
 * \param[in]  id         fm175xx �豸��Ԫ��
 * \param[in]  req_mode   ����ģʽ:
 *                        - AWBL_FM175XX_PICCA_REQ_IDLE
 *                        - AWBL_FM175XX_PICCA_REQ_ALL
 * \param[out] p_atqa     ATQA
 * \param[out] p_uid      ��ƬUID��Ϣ
 * \param[out] p_uid_len  uid��Ϣ����(�ֽ�)
 * \param[out] p_sak      SAKӦ����Ϣ
 *
 * \return
 */
aw_err_t awbl_fm175xx_picca_active (uint8_t   id,
                                    uint8_t   req_mode,
                                    uint16_t *p_atqa,
                                    uint8_t  *p_uid,
                                    uint8_t  *p_uid_len,
                                    uint8_t  *p_sak);

/**
 * \brief ��Ƭ(ISO/IEC 14443 Type AЭ��)ֹͣ
 *
 * \param[in] id      fm175xx �豸ID
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_picca_halt (uint8_t id);

/* @} */

/******************************************************************************/
/**
 * \brief AFI(Application Family ID)���Ͷ���
 * \note Ӧ������
 * @{
 */
/** \brief AFI main families type */
/** \brief All main Family */
#define AWBL_PICCB_AFI_MAIN_ALL                 (0U) 
#define AWBL_PICCB_AFI_MAIN_TRANSPORT           (1U)
#define AWBL_PICCB_AFI_MAIN_FINANCIAL           (2U)
#define AWBL_PICCB_AFI_MAIN_IDENTIFICATION      (3U)
#define AWBL_PICCB_AFI_MAIN_TELECOMMUNICATION   (4U)
#define AWBL_PICCB_AFI_MAIN_MEDICAL             (5U)
#define AWBL_PICCB_AFI_MAIN_MULTIMEDIA          (6U)
#define AWBL_PICCB_AFI_MAIN_GAMING              (7U)
#define AWBL_PICCB_AFI_MAIN_DATASTORAGE         (8U)

/** \brief AFI sub families type */
/** \brief All sub Family */
#define AWBL_PICCB_AFI_SUB_ALL                  (0U) 

/**
 * \brief AFI ���Ͷ���
 *
 * \param[in] main  AFI Ӧ��������, \ref AWBL_PICCB_AFI_xxx
 * \param[in] sub   AFI Ӧ�ô�����, ��Χ��0x00 ~ 0x0F
 */
#define awbl_piccb_afi(main, sub)               ((main << 4) | sub)

/* @} */

/** \brief PICC B ATQB */
typedef struct awbl_piccb_dev {
    /** \brief Pseudo unique PICC Identifier, ���� */
    uint8_t pupi[4];

    /** \brief Application Data, Ӧ������ */
    struct app {
        uint8_t afi;
        uint8_t crcb_aid[2];
        uint8_t app_num;
    } app_dat;

    /** \brief protocol info, Э����Ϣ */
    struct protocol {
        uint8_t bitrate_cap;
        uint8_t protocol_type;
        uint8_t maxframe_size;
        uint8_t fo;
        uint8_t adc;
        uint8_t fwi;
    } prtl_info;
} awbl_piccb_dev_t;

/**
 * \brief PICC B ���ӿڣ�����ISO/IEC 14443 BЭ��
 * @{
 */
/**
 * \brief ����Ƭ
 * \note ��Ƭ������Ҫ����(ISO/IEC 14443 BЭ��)
 *
 * \param[in] id        �豸 ID
 * \param[in] req_mode  ����ģʽ��
 *                      - AWBL_FM175XX_PICC_REQ_IDLE
 *                      - AWBL_FM175XX_PICC_REQ_ALL
 * \param[in] afi       Application Family ID,Ӧ����ID
 * \param[in] slots     �����
 * \param[out] p_atqb   ATQB
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_piccb_request (uint8_t           id,
                                     uint8_t           req_mode,
                                     uint8_t           afi,
                                     uint8_t           slots,
                                     awbl_piccb_dev_t *p_dev);

/**
 * \brief ����Slot-MARKER
 * \note ��Ƭ������Ҫ����(ISO/IEC 14443 BЭ��)
 * \note Slot-MARKER���������������������Ҫ���ͣ�
 * - PCD���յ�PICC��ATQB�����ʼ��һ����(ʱ϶)ʱ����������һ������ʱ��
 * - ���֪���òۺ��ǿյģ�����û��ATQB��������ô����Ҫһֱ�ȴ�ATQB���ò۽�����
 * ������ǲۿ�Ƭǿ��Ҫ��֧�ֵģ���Ƭ��֧��ʱ��������ᱻ���ӡ���ʱ��Ҫͨ��
 * awbl_fm175xx_piccb_request ���������� PROBABILISTIC(����)������ȡ��Ƭ��ATQB.
 *
 * \param[in] id        FM175XX �豸 ID
 * \param[in] slot_seq  �����
 * \param[out] p_piccb  pointer to awbl_piccb_dev_t
 *
 * \return errno.
 */
aw_err_t awbl_fm175xx_piccb_slot_marker_send (uint8_t           id,
                                              uint8_t           slot_seq,
                                              awbl_piccb_dev_t *p_piccb);

/** \brief ��СTR0ֵ */
enum awbl_piccb_attrib_min_tr0 {
    AWBL_PICCB_ATTRIB_MIN_TR0_DEFAULT, /**< \brief Ĭ��ֵ */
    AWBL_PICCB_ATTRIB_MIN_TR0_48_FS,   /**< \brief 48/fs */
    AWBL_PICCB_ATTRIB_MIN_TR0_16_FS    /**< \brief 16/fs */
};

/** \brief ��СTR1ֵ */
enum awbl_piccb_attrib_min_tr1 {
    AWBL_PICCB_ATTRIB_MIN_TR1_DEFAULT, /**< \brief Ĭ��ֵ */
    AWBL_PICCB_ATTRIB_MIN_TR1_64_FS,   /**< \brief 64/fs */
    AWBL_PICCB_ATTRIB_MIN_TR1_16_FS    /**< \brief 16/fs */
};

/** \brief PCD�ܹ����յ����֡��С */
enum awbl_piccb_attrib_max_frame_size {
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_16,  /**< \brief 16 �ֽ� */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_24,  /**< \brief 24 �ֽ� */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_32,  /**< \brief 32 �ֽ� */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_40,  /**< \brief 40 �ֽ� */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_48,  /**< \brief 48 �ֽ� */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_64,  /**< \brief 64 �ֽ� */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_96,  /**< \brief 96 �ֽ� */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_128, /**< \brief 128 �ֽ� */
    AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_256, /**< \brief 256 �ֽ� */
};

/** \brief PCD �� PICCλ���� */
enum awbl_piccb_attrib_pcd2picc_bitrate {
    AWBL_PICCB_ATTRIB_PCD2PICC_BITRATE_106, /**< \brief PCD��PICC��λ��Ϊ 106 kbit/s */
    AWBL_PICCB_ATTRIB_PCD2PICC_BITRATE_212, /**< \brief PCD��PICC��λ��Ϊ 212 kbit/s */
    AWBL_PICCB_ATTRIB_PCD2PICC_BITRATE_424, /**< \brief PCD��PICC��λ��Ϊ 424 kbit/s */
    AWBL_PICCB_ATTRIB_PCD2PICC_BITRATE_847, /**< \brief PCD��PICC��λ��Ϊ 847 kbit/s */
};

/** \brief PICC �� PCDλ���� */
enum awbl_piccb_attrib_picc2pcd_bitrate {
    AWBL_PICCB_ATTRIB_PICC2PCD_BITRATE_106, /**< \brief PCD��PICC��λ��Ϊ 106 kbit/s */
    AWBL_PICCB_ATTRIB_PICC2PCD_BITRATE_212, /**< \brief PCD��PICC��λ��Ϊ 212 kbit/s */
    AWBL_PICCB_ATTRIB_PICC2PCD_BITRATE_424, /**< \brief PCD��PICC��λ��Ϊ 424 kbit/s */
    AWBL_PICCB_ATTRIB_PICC2PCD_BITRATE_847, /**< \brief PCD��PICC��λ��Ϊ 847 kbit/s */
};

/** \brief ATTRIB */
typedef struct awbl_piccb_attrib {
    aw_bool_t   is_eof;    /**< \brief �Ƿ���Ҫ֡������ʶ��FALSE : ����Ҫ��TRUE : ��Ҫ */
    aw_bool_t   is_sof;    /**< \brief �Ƿ���Ҫ֡��ʼ��ʶ��FALSE : ����Ҫ��TRUE : ��Ҫ */
    uint8_t  cid;          /**< \brief ���忨Ƭ��CIDֵ�������Ƭ��֧�֣�����Ϊ0 */
    uint8_t  hinfo_len;    /**< \brief �߲���Ϣ���� */
    uint8_t *p_hinfo;      /**< \brief �߲���Ϣ����ѡ���� */

    /** \brief ��СTR0ֵ */
    enum awbl_piccb_attrib_min_tr0 tr0;

    /** \brief ��СTR1ֵ */
    enum awbl_piccb_attrib_min_tr1 tr1;

    /** \brief PCD �ܹ����յ����֡��С */
    enum awbl_piccb_attrib_max_frame_size   max_frame_size;

    /** \brief PCD �� PICC ��λ�� */
    enum awbl_piccb_attrib_pcd2picc_bitrate pcd2picc_bitrate;

    /** \brief PICC �� PCD ��λ�� */
    enum awbl_piccb_attrib_picc2pcd_bitrate picc2pcd_bitrate;
} awbl_piccb_attrib_t;

typedef struct awbl_piccb_attrib_rsp {
    /**
     * \brief ��󻺳�������ָʾ(MBLI)����Ƭ�ڲ����嵽����֡�����ơ�
     *
     * mbliΪ0����ʾ��Ƭû���ṩ��Ϣ���ڲ������뻺�壻mbli����0������󻺳峤��Ϊ =
     * (��Ƭ���֡��С) * 2^(mbli-1)����Ƭ����ATQB�з��ؿ�Ƭ�����֡��С��
     */
    uint8_t  mbli;

    uint8_t  cid;      /**< \brief PICC��֧��ʱ��ֵΪ0 */
    uint8_t *p_info;   /**< \brief �߲���Ϣ */
    uint8_t  info_len; /**< \brief �߲���Ϣ���� */
} awbl_piccb_attrib_rsp_t;

/**
 * \brief ��ȡ��Ƭ����
 * \note ��Ƭ������Ҫ����(ISO/IEC 14443 BЭ��)
 *
 * \param[in] id     FM175XX �豸 ID
 * \param[in] p_arg
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_piccb_attrib_get (uint8_t                  id,
                                        awbl_piccb_dev_t        *p_dev,
                                        awbl_piccb_attrib_t     *p_attrib,
                                        awbl_piccb_attrib_rsp_t *p_rsp);

#if 0
/**
 * \brief ��ȡUID����
 * \note ��Ҫ��ȡuid����ʱ���û���Ҫȷ��p_uidָ��Ļ����ܹ�����uid���ݵĳ��ȣ�
 *       PICCB���Ϳ���UID����Ϊ10�ֽڡ�
 *
 * \param[in] id     FM175XX �豸 ID
 * \param[in] cid    ͨ�� ``awbl_fm175xx_piccb_attrib_get`` ������ȡ��
 *                   λ�� awbl_piccb_attrib_rsp_t��Ա��
 * \param[out] p_uid �洢uid��Ϣ���ݻ���
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_piccb_uid_read (uint8_t id, uint8_t cid, uint8_t *p_uid);

#endif

/**
 * \brief ֹͣPICC
 *
 * \param[in] id    FM175XX �豸 ID
 * \param[in] p_id  ��ƬID������Ϊ4�ֽڣ���ͨ��ATQB��á�
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_piccb_halt (uint8_t           id,
                                  awbl_piccb_dev_t *p_dev);

/* @} */
/******************************************************************************/
/**
 * \brief mifare ָ��
 * @{
 */
#define AWBL_FM175XX_MIFARE_KEYA     (0x60) /**< \brief KEY A ��ָ֤�� */
#define AWBL_FM175XX_MIFARE_KEYB     (0x61) /**< \brief KEY B ��ָ֤�� */
#define AWBL_FM175XX_MIFARE_READ     (0x30) /**< \brief �� */
#define AWBL_FM175XX_MIFARE_WRITE    (0xA0) /**< \brief д */
#define AWBL_FM175XX_MIFARE_DECR     (0xC0) /**< \brief ��С��ֵ */
#define AWBL_FM175XX_MIFARE_INCR     (0xC1) /**< \brief ������ֵ */
#define AWBL_FM175XX_MIFARE_REST     (0xC2) /**< \brief �ָ��洢��ֵ */
#define AWBL_FM175XX_MIFARE_TRAN     (0xB0) /**< \brief ���� */
/* @} */

/**
 * \brief Mifare ���ӿ�
 * @{
 */

/**
 * \brief mifare �� authentication
 *
 * \param[in] id        FM175XX �豸ID
 * \param[in] key_type  ��Կ���ͣ���ѡ��
 *                      - AWBL_KEY_A
 *                      - AWBL_KEY_B
 * \param[in] blk_addr  �飨block���ţ�ֻ���һ�������е�����һ�������֤����
 * \param[in] p_key     ��Կ����
 * \param[in] p_uid     UID����
 * \param[in] key_num   ��Կ����
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_auth (uint8_t         id,
                                   uint8_t         key_type,
                                   uint8_t         blk_addr,
                                   const uint8_t  *p_key,
                                   const uint8_t  *p_uid,
                                   uint8_t         key_num);

/**
 * \brief mifare read block data
 *
 * \param[in] id        FM175XX �豸ID
 * \param[in] blk_addr  ���ַ
 * \param[in] p_buf     ���ݻ��������洢��ȡ�����ݣ�
 *
 * \note �û����øýӿ�ʱ��ȷ�����ݻ������Ĵ�С�����㹻����һ�����ݿ�Ĵ�С
 *       ��S50��һ�����СΪ16�ֽ�
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_read (uint8_t   id,
                                   uint8_t   blk_addr,
                                   uint8_t  *p_buf,
                                   uint8_t   blk_size);

/**
 * \brief mifare write block data
 *
 * \param[in] id        FM175XX �豸ID
 * \param[in] blk_addr  ���ַ
 * \param[in] p_buf     ���ݻ��������洢��д�����ݣ�
 * \param[in] blk_size  ���С
 *
 * \note �û����øýӿ�ʱ��ȷ�����ݻ������Ĵ�С��blk_sizeָ���Ĵ�С�����
 *       Ӧ���㹻����һ�����ݴ�С��S50��һ������Ϊ16�ֽڣ���p_buf����16�ֽڴ�С
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_write (uint8_t         id,
                                    uint8_t         blk_addr,
                                    const uint8_t  *p_buf,
                                    uint8_t         blk_size);

/**
 * \brief mifare increment
 *
 * \param[in] id        FM175XX �豸ID
 * \param[in] blk_addr  ���ַ
 * \param[in] p_dat     ���ݻ��棨�洢��Ҫ���ӵ����ݣ�
 * \param[in] len       ���ݸ���
 *
 * \note ֻ���value block��Ч��ִ�и�����ǰ��Ҫ���飨block����ʼ����value block,
 *       p_dat ������4�ֽڴ�С��len ��СΪ4
 * \note errno
 */
aw_err_t awbl_fm175xx_mifare_increment (uint8_t         id,
                                        uint8_t         blk_addr,
                                        const uint8_t  *p_dat,
                                        uint8_t         len);

/**
 * \brief mifare decrement
 *
 * \param[in] id        FM175XX �豸ID
 * \param[in] blk_addr  ���ַ
 * \param[in] p_dat     ���ݻ��棨�洢��Ҫ��С�����ݣ�
 * \param[in] len       ���ݸ���
 *
 * \note ֻ���value block��Ч��ִ�и�����ǰ��Ҫ���飨block����ʼ����value block
 * \note errno
 */
aw_err_t awbl_fm175xx_mifare_decrement (uint8_t         id,
                                        uint8_t         blk_addr,
                                        const uint8_t  *p_dat,
                                        uint8_t         len);

/**
 * \brief mifare restore
 *
 * \param[in] id        FM175XX �豸ID
 * \param[in] blk_addr  ���ַ
 *
 * \note ֻ���value block��Ч��ִ�и�����ǰ��Ҫ���飨block����ʼ����value block
 *       ִ�иú���֮�����ݽ���������transfer buffer�У���Ҫִ�� 
 *       awbl_mifare_transfer֮��Ż���������±��浽block��
 *
 * \note errno
 */
aw_err_t awbl_fm175xx_mifare_restore (uint8_t id, uint8_t blk_addr);

/**
 * \brief mifare transfer
 *
 * \param[in] id        FM175XX �豸ID
 * \param[in] blk_addr  ���ַ
 *
 * \note ֻ���value block��Ч��ִ�и�����ǰ��Ҫ���飨block����ʼ����value block
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_transfer (uint8_t id, uint8_t blk_addr);

/**
 * \brief value block ��ʼ��
 *        ��һ��block��ʼ����value block
 *
 * \param[in] id        FM175XX �豸ID
 * \param[in] blkaddr   ���ַ
 * \param[in] dat       ��ʼֵ
 *
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_valblk_construct (uint8_t  id,
                                               uint8_t  blkaddr,
                                               int32_t  dat);

/**
 * \brief modify access bit field
 *
 * \param[in] id        FM175XX �豸ID
 * \param[in] sec_addr  ����(sector)��ַ
 * \param[in] p_keya    KEYA ��Կ����
 * \param[in] p_keyb    KEYB ��Կ����
 * \param[in] p_ab      ����λ��access bit������
 * \param[in] key_len   ��Կ�������ֽڣ�
 * \param[in] ab_len    ����λ�������ֽڣ�
 *
 * \note ����ͨ���ýӿڶ�һ����������Կֵ�����޸ģ���Ҫ����ע�⣬�޸�ĳ����Կֵ
 *       ���߷���λʱ����Ҫȷ�����޸ĵ�λ����ֵ���ֲ��䣬�磺
 *       KEYA��Կ��0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
 *       KEYB��Կ��0x11, 0x11, 0x11, 0x11, 0x11, 0x11
 *       ����λ��0xFF, 0x07, 0x80
 *       ֻ�޸�KEYA��Կʱ������Ĳ����У�p_keyb�洢����ԿֵΪ����KEYB��Կֵ��
 *       p_ab�洢�ķ���λΪ��������λֵ��ȷ�������������޸�ǰ�󱣳ֲ��䡣
 * \return errno
 */
aw_err_t awbl_fm175xx_mifare_sector_trailer_modify (uint8_t         id,
                                                    uint8_t         sec_addr,
                                                    const uint8_t  *p_keya,
                                                    const uint8_t  *p_keyb,
                                                    const uint8_t  *p_ab,
                                                    uint8_t         key_len,
                                                    uint8_t         ab_len);

/**
 * \brief FM175XX driver register
 */
void awbl_fm175xx_drv_register (void);

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif

/* end of file*/

