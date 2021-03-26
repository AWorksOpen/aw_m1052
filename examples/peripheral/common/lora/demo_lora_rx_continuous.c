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
 * \brief sx127x ��������ģʽʹ������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_SX127X (����ģ��ʹ�õ�SPI��GPIO���ţ���鿴awbus_hwconf_sx127x.h)
 *      - AW_COM_CONSOLE
 *      - AW_DEV_GPIO_LED
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   sx127x ������ģʽ��������
 *
 * \par Դ����
 * \snippet demo_lora_rx_continuous.c src_lora_rx_continuous
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lora_rx_continuous lora(��������)
 * \copydoc demo_lora_rx_continuous.c
 */

/** [src_lora_rx_continuous] */

#include "am_lora_type.h"
#include "am_sx127x.h"
#include "am_lora_utilities.h"
#include "driver/lora/awbl_sx127x.h"
#include "aw_led.h"

#include <string.h>

/**
 * ��������
 */
#define RF_FREQUENCY                                490000000 // Hz
#define LORA_BANDWIDTH                              1         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       8         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_FIX_LENGTH_PAYLOAD_ON                  AM_LORA_FALSE
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_IQ_INVERSION_ON                        AM_LORA_FALSE

#define LORA_TEST_LED_ID       0
#define LORA_TEST_DEV_ID       0

/**
 * �����¼�
 */
static am_lora_radio_events_t __g_radio_events;

/**
 * ������ɻص�����
 */
static void __on_rx_done (uint8_t *p_payload, uint16_t size, int16_t rssi, int8_t snr);

/**
 * ���մ���ص�����
 */
static void __on_rx_error (void);

/**
 * SX127x �����������
 */
static am_sx127x_handle_t __g_sx127x_handle;

/**
 * SX127x �������ղ��Գ������
 */
void demo_lora_rx_continuous_entry (void)
{
    /* ��ȡָ�� ID ���豸 */
    __g_sx127x_handle = awbl_sx127x_get_handle_by_id(LORA_TEST_DEV_ID);
    if (!__g_sx127x_handle) {
        am_lora_kprintf("awbl_sx127x_get_handle_by_id err!\r\n");
        while(1);
    }

    /* �����¼���ʼ�� */
    __g_radio_events.pfn_rx_done_cb    = __on_rx_done;
    __g_radio_events.pfn_rx_error_cb   = __on_rx_error;
    am_sx127x_events_set(__g_sx127x_handle, &__g_radio_events);

    /* �����ŵ����������� */
    am_sx127x_channel_set(__g_sx127x_handle, RF_FREQUENCY);
    am_sx127x_rx_config_set(__g_sx127x_handle,
                            MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                            LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                            LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                            0, AM_LORA_TRUE, 0, 0, LORA_IQ_INVERSION_ON, AM_LORA_TRUE);

    /* ������������ */
    am_sx127x_rx(__g_sx127x_handle, 0);
}

/**
 * ������ɻص�����
 */
static void __on_rx_done (uint8_t *p_payload, uint16_t size, int16_t rssi, int8_t snr)
{
    char temp_buffer[256] = {0};
    memset(temp_buffer, 0, sizeof(temp_buffer));
    memcpy(temp_buffer, p_payload, size);
    am_lora_kprintf("rssi is %d, snr is %d.\r\n", rssi, snr);
    am_lora_kprintf("payload is %s\r\n", temp_buffer);

    aw_led_toggle(LORA_TEST_LED_ID);
}

/**
 * ���մ���ص�����
 */
static void __on_rx_error (void)
{
    am_lora_kprintf("RX error!\r\n");
}

/** [src_lora_rx_continuous] */

/* end of file */
