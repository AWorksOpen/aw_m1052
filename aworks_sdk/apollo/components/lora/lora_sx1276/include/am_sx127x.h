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
 * \brief Generic SX1276/77/78/79 driver implementation
 * \note SX1276/77/78/79 �ɹ���һ������
 *
 * \internal
 * \par Modification history
 * - 1.0.0 17-09-25  sky, first implementation.
 * \endinternal
 */

#ifndef __AM_SX127X_H
#define __AM_SX127X_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lpsoftimer.h"
#include "am_sx127x_regs_fsk.h"
#include "am_sx127x_regs_lora.h"

/**
 * @addtogroup am_if_sx1276
 * @copydoc am_sx1276.h
 * @{
 */

/* #define LORA_ENABLE_FSK */

/**
 * \brief ��Ƶ����֧�ֵĵ��Ʒ�ʽ
 */
typedef enum {
    MODEM_FSK = 0,
    MODEM_LORA,
} am_lora_radio_modems_t;

/**
 * \brief ��Ƶ�����ڲ�״̬��״̬����
 */
typedef enum {
    LORA_RF_IDLE = 0,
    LORA_RF_RX_RUNNING,
    LORA_RF_TX_RUNNING,
    LORA_RF_CAD,
} am_lora_radio_state_t;

/**
 * \brief ��Ƶ˯��ģʽ�Ļ���ʱ��
 */
#define AM_LORA_RADIO_OSC_STARTUP           1 /* [ms] */

/**
 * \brief ��ƵPLL����ģʽ׼��ʱ��(���ܻ����¶ȱ仯)
 */
#define AM_LORA_RADIO_SLEEP_TO_RX           2 /* [ms] */

/**
 * \brief ��Ƶ��ɻ���ʱ����¶Ȳ����ٽ�ֵ
 */
#define AM_LORA_RADIO_WAKEUP_TIME           (AM_LORA_RADIO_OSC_STARTUP + \
                                             AM_LORA_RADIO_SLEEP_TO_RX)

/**
 * \brief LoRa˽��ͬ����
 */
#define AM_LORA_MAC_PRIVATE_SYNCWORD        0x12

/**
 * \brief LoRa����ͬ����
 */
#define AM_LORA_MAC_PUBLIC_SYNCWORD         0x34

/**
 * \brief SX1276 ���ж���
 */
#define AM_LORA_XTAL_FREQ           32000000
#define AM_LORA_FREQ_STEP           ((int64_t)61035156250)
#define AM_LORA_RX_BUFFER_SIZE      256

typedef void (*am_sx127x_pfnvoid_t) (void *);

/**
 * \brief ��ƵFSK���Ʋ���
 */
typedef struct {
    int8_t   power;
    uint32_t freq_dev;
    uint32_t bandwidth;
    uint32_t bandwidth_afc;
    uint32_t datarate;
    uint16_t preamble_len;
    uint8_t  fix_len;
    uint8_t  payload_len;
    uint8_t  crc_on;
    uint8_t  iq_inverted;
    uint8_t  rx_continuous;
    uint32_t tx_timeout;
} am_lora_radio_fsk_settings_t;

/**
 * \brief ��ƵFSK������ṹ
 */
typedef struct {
    uint8_t  preamble_detected;
    uint8_t  sync_word_detected;
    int8_t   rssi_value;
    int32_t  afc_value;
    uint8_t  rx_gain;
    uint16_t size;
    uint16_t nb_bytes;
    uint8_t  fifo_thresh;
    uint8_t  chunk_size;
} am_lora_radio_fsk_packet_handler_t;

/**
 * \brief ��ƵLoRa���Ʋ���
 */
typedef struct {
    int8_t   power;
    uint32_t bandwidth;
    uint32_t datarate;
    uint8_t  low_datarate_optimize;
    uint8_t  coderate;
    uint16_t preamble_len;
    uint8_t  fix_len;
    uint8_t  payload_len;
    uint8_t  crc_on;
    uint8_t  freq_hop_on;
    uint8_t  hop_period;
    uint8_t  iq_inverted;
    uint8_t  rx_continuous;
    uint32_t tx_timeout;
} am_lora_radio_lora_settings_t;

/**
 * \brief ��ƵLoRa������ṹ
 */
typedef struct {
    int8_t  snr_value;
    int16_t rssi_value;
    uint8_t size;
} am_lora_radio_lora_packet_handler_t;

/**
 * \brief ��Ƶ����
 */
typedef struct {
    am_lora_radio_state_t               state;
    am_lora_radio_modems_t              modem;
    uint32_t                            channel;
    am_lora_radio_fsk_settings_t        fsk;
    am_lora_radio_fsk_packet_handler_t  fsk_packet_handler;
    am_lora_radio_lora_settings_t       lora;
    am_lora_radio_lora_packet_handler_t lora_packet_handler;
} am_lora_radio_settings_t;

/**
 * \brief ��Ƶ�����ص�����
 */
typedef struct {

    /**
     * \brief ������ɻص�����ԭ��
     */
    void (*pfn_tx_done_cb)(void);

    /**
     * \brief ���䳬ʱ�ص�����ԭ��
     */
    void (*pfn_tx_timeout_cb)(void);

    /**
     * \brief ������ɻص�����ԭ��
     *
     * \param[in] p_payload : ָ����ջ�����
     * \param[in] size      : ���յ������ݴ�С
     * \param[in] rssi      : ���ո�֡ʱ�ļ���õ� RSSI ֵ [dBm]
     * \param[in] snr       : �����Ʋ��뻻��õ������
     *                         FSK : N/A (set to 0)
     *                         LoRa: SNR value in dB
     */
    void (*pfn_rx_done_cb)(uint8_t *p_payload, uint16_t size, int16_t rssi, int8_t snr);

    /**
     * \brief ���ճ�ʱ�ص�����ԭ��
     */
    void (*pfn_rx_timeout_cb)(void);

    /**
     * \brief ���մ���ص�����ԭ��
     */
    void (*pfn_rx_error_cb)(void);

    /**
     * \brief FHSS��Ƶ�ص�����ԭ��
     *
     * \param[in] current_channel : ��ǰ�ŵ��ı��
     */
    void (*pfn_fhss_change_channel_cb)(uint8_t current_channel);

    /**
     * \brief �ŵ���Ծ�����ɻص�����ԭ��
     *
     * \param[in] channel_activity_detected : �Ƿ��Ծ
     */
    void (*pfn_cad_done_cb)(uint8_t channel_activity_detected);

    /**
     * \brief ��Ч��ͷ�ص�����ԭ��
     */
    void (*pfn_valid_header_cb)(void);

} am_lora_radio_events_t;

/** \brief LORA SX1276 ��������ǰ������ */
typedef struct am_sx127x_drv_funcs am_sx127x_drv_funcs_t;

/**
 * \brief LORA SX1276 �豸�ṹ��
 */
typedef struct am_sx127x_dev {

    am_sx127x_drv_funcs_t   *p_drv_funcs;      /**< \brief ���������� */
    am_lora_radio_events_t  *p_radio_events;   /**< \brief ��Ƶ�¼��ص� */

    am_lora_radio_settings_t settings;         /**< \brief ��ƵӲ��ȫ�ֲ��� */
    uint8_t                  radio_is_active;  /**< \brief �͹���ģʽ��RF���ر�־ */

    am_lpsoftimer_t tx_timeout_timer;          /**< \brief ���ͳ�ʱ��ʱ�� */
    am_lpsoftimer_t rx_timeout_timer;          /**< \brief ���ճ�ʱ��ʱ�� */
    am_lpsoftimer_t rx_timeout_sync_word_timer;/**< \brief ͬ���ֳ�ʱ��ʱ�� */

    /** \brief �շ�������(����5���ֽ�У����Ч֡ͷ) */
    uint8_t rx_tx_buffer[AM_LORA_RX_BUFFER_SIZE + 5];

    /** \brief ָ���շ���������ʵ�ʵ�ַ */
    uint8_t *p_rx_tx_buffer;

} am_sx127x_dev_t;

/** \brief LORA SX1276 ����������� */
typedef am_sx127x_dev_t *am_sx127x_handle_t;

/**
 * \brief LORA SX1276 ������������
 */
struct am_sx127x_drv_funcs {
    void (*pfn_reset)(am_sx127x_handle_t handle);
    int  (*pfn_buf_read)(am_sx127x_handle_t handle,
                         uint8_t            addr,
                         uint8_t           *p_buff,
                         uint32_t           len);
    int  (*pfn_buf_write)(am_sx127x_handle_t handle,
                          uint8_t            addr,
                          const uint8_t     *p_buff,
                          uint32_t           len);
};

/**
 * ��������ԭ��
 */

/**
 * \brief ��ʼ��SX1276
 *
 * \param[in] p_dev       : ָ�� LORA SX1276 �豸�ṹ���ָ��
 * \param[in] p_drv_funcs : ָ�� LORA SX1276 ���������б�
 *
 * \return LORA SX1276 ����������,���Ϊ NULL��������ʼ��ʧ��
 */
am_sx127x_handle_t am_sx127x_init (am_sx127x_dev_t       *p_dev,
                                   am_sx127x_drv_funcs_t *p_drv_funcs);

/**
 * \brief ȥSX1276��ʼ��
 *
 * \param[in] p_dev : ָ�� LORA SX1276 �豸�ṹ���ָ��
 *
 * \return ��
 */
void am_sx127x_deinit (am_sx127x_dev_t *p_dev);

/**
 * \brief SX127x ��λ����
 * \param[in] p_dev : ָ�� LORA SX1276 �豸�ṹ���ָ��
 * \return ��
 */
void am_sx127x_reset (am_sx127x_dev_t *p_dev);

/**
 * \brief DIO0 �жϻص�
 *
 * \param[in] handle : SX1276 ����������
 *
 * \return ��
 */
void am_sx127x_dio0_isr (am_sx127x_handle_t handle);

/**
 * \brief DIO1 �жϻص�
 *
 * \param[in] handle : SX1276 ����������
 *
 * \return ��
 */
void am_sx127x_dio1_isr (am_sx127x_handle_t handle);

/**
 * \brief DIO2 �жϻص�
 *
 * \param[in] handle : SX1276 ����������
 *
 * \return ��
 */
void am_sx127x_dio2_isr (am_sx127x_handle_t handle);

/**
 * \brief DIO3 �жϻص�
 *
 * \param[in] handle : SX1276 ����������
 *
 * \return ��
 */
void am_sx127x_dio3_isr (am_sx127x_handle_t handle);

/**
 * \brief DIO4 �жϻص�
 *
 * \param[in] handle : SX1276 ����������
 *
 * \return ��
 */
void am_sx127x_dio4_isr (am_sx127x_handle_t handle);

/**
 * \brief DIO5 �жϻص�
 * \param[in] handle : SX1276 ����������
 * \return ��
 */
void am_sx127x_dio5_isr (am_sx127x_handle_t handle);

/**
 * \brief ������Ƶ�¼��ص�
 *
 * \param[in] handle   : SX1276 ����������
 * \param[in] p_events : �¼��ص������ṹ
 *
 * \return ��
 */
void am_sx127x_events_set (am_sx127x_handle_t      handle,
                           am_lora_radio_events_t *p_events);

/**
 * \brief �����豸��ǰ״̬
 * \param[in] handle : SX1276 ����������
 * \return ��ǰ״̬ [RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
 */
am_lora_radio_state_t am_sx127x_status_get (am_sx127x_handle_t handle);

/**
 * \brief ��Ƶģʽ����
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] modem  : ʹ�õ�ģʽ [0: FSK, 1: LoRa]
 *
 * \return ��
 */
void am_sx127x_modem_set (am_sx127x_handle_t     handle,
                          am_lora_radio_modems_t modem);

/**
 * \brief �����ŵ�Ƶ��
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] freq : �ŵ�Ƶ��
 *
 * \return ��
 */
void am_sx127x_channel_set (am_sx127x_handle_t handle, uint32_t freq);

/**
 * \brief ��ȡ�ŵ�Ƶ��
 *
 * \param[in] handle : SX1276 ����������
 *
 * \return �ŵ�Ƶ��
 */
uint32_t am_sx127x_channel_get (am_sx127x_handle_t handle);

/**
 * \brief ��ѯ�ŵ��Ƿ����
 *
 * \param[in] handle      : SX1276 ����������
 * \param[in] modem       : ʹ�õ���Ƶģʽ [0: FSK, 1: LoRa]
 * \param[in] freq        : �ŵ�Ƶ��
 * \param[in] rssi_thresh : RSSI �ٽ�ֵ
 *
 * \retval isFree [true: Channel is free, false: Channel is not free]
 */
uint8_t am_sx127x_is_channel_free (am_sx127x_handle_t     handle,
                                   am_lora_radio_modems_t modem,
                                   uint32_t               freq,
                                   int16_t                rssi_thresh);

/**
 * \brief ����RSSI����32λ�����
 *
 * \param[in] handle : SX1276 ����������
 *
 * \remark �ú���ʹ��LoRa����ģʽ�����������жϣ����øú�����������
 *          am_lora_am_sx127x_rx_config_set �� am_lora_am_sx127x_tx_config_set��
 *
 * \retval 32λ�������ֵ
 */
uint32_t am_sx127x_random (am_sx127x_handle_t handle);

/**
 * \brief ���ղ�������
 *
 * \remark ʹ��LoRa����ģʽʱ��֧�� 125��250��500kHz
 *
 * \param[in] handle        : SX1276 ����������
 * \param[in] modem         : ʹ�õ���Ƶģʽ [0: FSK, 1: LoRa]
 * \param[in] bandwidth     : ���ô���
 *                             FSK : >= 2600 and <= 250000 Hz
 *                             LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                    2: 500 kHz, 3: Reserved]
 * \param[in] datarate      : ������������
 *                             FSK : 600..300000 bits/s
 *                             LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                   10: 1024, 11: 2048, 12: 4096  chips]
 * \param[in] coderate      : ���ñ����� (LoRa only)
 *                             FSK : N/A (set to 0)
 *                             LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param[in] bandwidth_afc : ���� AFC ���� (FSK only)
 *                             FSK : >= 2600 and <= 250000 Hz
 *                             LoRa: N/A (set to 0)
 * \param[in] preamble_len  : ����ǰ���볤��
 *                             FSK : Number of bytes
 *                             LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param[in] symb_timeout  : ���õ��ν��ճ�ʱֵ (LoRa only)
 *                             FSK : N/A (set to 0)
 *                             LoRa: timeout in symbols
 * \param[in] fix_len       : ���г���ʹ�� [0: variable, 1: fixed]
 * \param[in] payload_len   : ���ø��س��� (�����г���ʹ��ʱ)
 * \param[in] crc_on        : CRCУ��ʹ�� [0: OFF, 1: ON]
 * \param[in] freq_hop_on   : �ڲ�����Ƶʹ��
 *                             FSK : N/A (set to 0)
 *                             LoRa: [0: OFF, 1: ON]
 * \param[in] hop_period    : ÿ����Ƶ�ķ�������
 *                             FSK : N/A (set to 0)
 *                             LoRa: Number of symbols
 * \param[in] iq_inverted   : ת�� IQ �ź� (LoRa only)
 *                             FSK : N/A (set to 0)
 *                             LoRa: [0: not inverted, 1: inverted]
 * \param[in] rx_continuous : ������������ģʽ
 *                             [false: single mode, true: continuous mode]
 */
void am_sx127x_rx_config_set (am_sx127x_handle_t     handle,
                              am_lora_radio_modems_t modem, 
                              uint32_t               bandwidth,
                              uint32_t               datarate,      
                              uint8_t                coderate,
                              uint32_t               bandwidth_afc, 
                              uint16_t               preamble_len,
                              uint16_t               symb_timeout,  
                              uint8_t                fix_len,
                              uint8_t                payload_len,
                              uint8_t                crc_on,        
                              uint8_t                freq_hop_on, 
                              uint8_t                hop_period,
                              uint8_t                iq_inverted,   
                              uint8_t                rx_continuous);

/**
 * \brief �����������
 *
 * \remark ʹ��LoRa����ģʽʱ��֧�� 125��250��500kHz
 *
 * \param[in] handle       : SX1276 ����������
 * \param[in] modem        : ʹ�õ���Ƶģʽ [0: FSK, 1: LoRa]
 * \param[in] power        : ���ù��� [dBm]
 * \param[in] freq_dev     : ���÷�Ƶ�� (FSK only)
 *                            FSK : [Hz]
 *                            LoRa: 0
 * \param[in] bandwidth    : ���ô��� (LoRa only)
 *                            FSK : 0
 *                            LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                   2: 500 kHz, 3: Reserved]
 * \param[in] datarate     : ������������
 *                            FSK : 600..300000 bits/s
 *                            LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                  10: 1024, 11: 2048, 12: 4096  chips]
 * \param[in] coderate     : ���ñ����� (LoRa only)
 *                            FSK : N/A (set to 0)
 *                            LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param[in] preamble_len : ����ǰ���볤��
 *                            FSK : Number of bytes
 *                            LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param[in] fix_len      : �̶�����ʹ�� [0: variable, 1: fixed]
 * \param[in] crc_on       : CRCУ��ʹ�� [0: OFF, 1: ON]
 * \param[in] freq_hop_on  : �ڲ���Ƶʹ��
 *                            FSK : N/A (set to 0)
 *                            LoRa: [0: OFF, 1: ON]
 * \param[in] hop_period   : ÿ����Ƶ�ķ�������
 *                            FSK : N/A (set to 0)
 *                            LoRa: Number of symbols
 * \param[in] iq_inverted  : ת�� IQ �ź� (LoRa only)
 *                            FSK : N/A (set to 0)
 *                            LoRa: [0: not inverted, 1: inverted]
 * \param[in] timeout      : ���䳬ʱ [ms]
 */
void am_sx127x_tx_config_set (am_sx127x_handle_t     handle,
                              am_lora_radio_modems_t modem, 
                              int8_t                 power,
                              uint32_t               freq_dev,
                              uint32_t               bandwidth, 
                              uint32_t               datarate,
                              uint8_t                coderate,   
                              uint16_t               preamble_len,
                              uint8_t                fix_len,    
                              uint8_t                crc_on,      
                              uint8_t                freq_hop_on,
                              uint8_t                hop_period, 
                              uint8_t                iq_inverted, 
                              uint32_t               timeout);

/**
 * \brief ���Ӳ���Ƿ�֧�ָ�������ƵƵ��
 *
 * \param[in] handle    : SX1276 ����������
 * \param[in] frequency : Ҫ����Ƶ��
 *
 * \retval [true: ֧��, false: ��֧��]
 */
uint8_t am_sx127x_rf_frequency_check (am_sx127x_handle_t handle, uint32_t frequency);

/**
 * \brief ����������ض�Ӧ�ķ�����
 *
 * \param[in] handle  : SX1276 ����������
 * \param[in] pkt_len : ���ذ�����
 *
 * \retval ������
 */
uint32_t  am_sx127x_time_pkt_symbol_get(am_sx127x_handle_t  handle,
                                        uint8_t             pkt_len);

/**
 * \brief ����������ص��ڿ�ʱ�䣨ms��
 *
 * \Remark ֻ����am_lora_am_sx127x_rx_config_set �� 
 *         am_lora_am_sx127x_tx_config_set�����ú����һ�Ρ�
 *
 * \param[in] handle  : SX1276 ����������
 * \param[in] modem   : ʹ�õ���Ƶģʽ [0: FSK, 1: LoRa]
 * \param[in] pkt_len : ���ذ�����
 *
 * \retval �ڿ�ʱ�䣨ms��
 */
uint32_t am_sx127x_time_on_air_get (am_sx127x_handle_t     handle,
                                    am_lora_radio_modems_t modem,
                                    uint8_t                pkt_len);

/**
 * \brief ����ָ�����ȵ�����
 *
 * \param[in] handle   : SX1276 ����������
 * \param[in] p_buffer : ָ������͵�����
 * \param[in] size     : �����͵����ݳ���
 *
 * \return ��
 */
void am_sx127x_send (am_sx127x_handle_t  handle,
                     uint8_t            *p_buffer,
                     uint8_t             size);

/**
 * \brief ����˯��ģʽ
 * \param[in] handle : SX1276 ����������
 * \return ��
 */
void am_sx127x_sleep (am_sx127x_handle_t handle);

/**
 * \brief �������ģʽ
 * \param[in] handle : SX1276 ����������
 * \return ��
 */
void am_sx127x_standby (am_sx127x_handle_t handle);

/**
 * \brief ������Ƶ����
 *
 * \param[in] handle  : SX1276 ����������
 * \param[in] timeout : ���ճ�ʱ [ms] [0: continuous, others: timeout]
 *
 * \return ��
 */
void am_sx127x_rx (am_sx127x_handle_t handle, uint32_t timeout);

/**
 * \brief ����һ���ŵ���Ծ���
 * \param[in] handle : SX1276 ����������
 * \return ��
 */
void am_sx127x_cad_start (am_sx127x_handle_t handle);

/**
 * \brief ������Ƶ������������ģʽ(FSK������)
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] freq   : �ŵ�Ƶ��
 * \param[in] power  : ���书�� [dBm]
 * \param[in] time   : ���䳬ʱ [s]
 *
 * \return ��
 */
void am_sx127x_tx_continuous_wave_set (am_sx127x_handle_t handle,
                                       uint32_t           freq,
                                       int8_t             power,
                                       uint16_t           time);

/**
 * \brief ��ȡ��ǰ��RSSIֵ
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] modem  : ʹ�õ���Ƶģʽ [0: FSK, 1: LoRa]
 *
 * \retval RSSIֵ [dBm]
 */
int16_t am_sx127x_rssi_get (am_sx127x_handle_t     handle,
                            am_lora_radio_modems_t modem);

/**
 * \brief д�ض���ַ����Ƶ�Ĵ���
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] addr   : �Ĵ�����ַ
 * \param[in] data   : �µļĴ���ֵ
 *
 * \return ��
 */
void am_sx127x_write (am_sx127x_handle_t handle, uint8_t addr, uint8_t data);

/**
 * \brief ���ض���ַ����Ƶ�Ĵ���
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] addr   : �Ĵ�����ַ
 *
 * \retval �Ĵ���ֵ
 */
uint8_t am_sx127x_read (am_sx127x_handle_t handle, uint8_t addr);

/**
 * \brief ����ʼ��ַд����Ĵ���
 *
 * \param[in] handle   : SX1276 ����������
 * \param[in] addr     : ��ʼ��ַ
 * \param[in] p_buffer : �����¼Ĵ���ֵ�Ļ�����
 * \param[in] size     : Ҫд�ļĴ�������
 *
 * \return ��
 */
void am_sx127x_buffer_write (am_sx127x_handle_t handle,
                             uint8_t            addr,
                             uint8_t           *p_buffer,
                             uint8_t            size);

/**
 * \brief ����ʼ��ַ������Ĵ���
 *
 * \param[in]  handle   : SX1276 ����������
 * \param[in]  addr     : ��ʼ��ַ
 * \param[out] p_buffer : �����¼Ĵ���ֵ�Ļ�����
 * \param[in]  size     : Ҫ���ļĴ�������
 *
 * \return ��
 */
void am_sx127x_buffer_read (am_sx127x_handle_t handle,
                            uint8_t            addr,
                            uint8_t           *p_buffer,
                            uint8_t            size);

/**
 * \brief ��������س���
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] modem  : ʹ�õ���Ƶģʽ [0: FSK, 1: LoRa]
 * \param[in] max    : ��������ֽ���
 *
 * \return ��
 */
void am_sx127x_max_payload_length_set (am_sx127x_handle_t     handle,
                                       am_lora_radio_modems_t modem,
                                       uint8_t                max);

/**
 * \brief ���ù�����˽��(����ͬ����)
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] enable : 1 ������ 0 ˽��
 *
 * \remark ֻ������LoRaģʽ
 *
 * \return ��
 */
void am_sx127x_public_network_set (am_sx127x_handle_t handle, uint8_t enable);

/**
 * note: extra interfaces added by ebi
 */

/**
 * \brief ����ͬ����
 *
 * \param[in] handle   : SX1276 ����������
 * \param[in] syncword : ͬ����
 *
 * \return ��
 */
void am_sx127x_syncword_set (am_sx127x_handle_t handle, uint8_t syncword);

/**
 * \brief ������������
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] enable : ʹ�ܻ����
 *
 * \return ��
 */
void am_sx127x_rx_continuous_set (am_sx127x_handle_t handle, uint8_t enable);

/**
 * \brief ���÷��ų�ʱ(ms)
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] enable : ʹ�ܻ����
 *
 * \return ��
 */
void am_sx127x_symb_timeout_set (am_sx127x_handle_t handle, uint32_t ms);

/**
 * \brief �ش�(���size��Ϊ0��ת��һ���ָ��ص�FIFO��)
 *
 * \param[in] handle : SX1276 ����������
 * \param[in] p_data : �ش�������
 * \param[in] size   : ���ݴ�С
 * \param[in] offset : ����ƫ��
 *
 * \return ��
 */
void am_sx127x_tx_repeat_set (am_sx127x_handle_t  handle,
                              uint8_t            *p_data,
                              uint8_t             size,
                              uint8_t             offset);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_SX127X_H */

/* end of file */
