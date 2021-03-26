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
 * \brief sx127x ����ʹ������
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
 *   sx127x ����500msΪ���ڶ�ʱ��������
 *
 * \par Դ����
 * \snippet demo_lora_tx.c src_lora_tx
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lora_tx lora(����)
 * \copydoc demo_lora_tx.c
 */

/** [src_lora_tx] */

#include "am_lora_type.h"
#include "am_sx127x.h"
#include "am_lora_utilities.h"
#include "driver/lora/awbl_sx127x.h"
#include "aw_led.h"

/**
 * ��������(���ֲ�����֤���Ͷ�����ն�һ��)
 */
#define RF_FREQUENCY                                490000000 // Hz
#define TX_OUTPUT_POWER                             17        // 17 dBm
#define TX_TIMEOUT                                  65535     // seconds (MAX value
#define LORA_BANDWIDTH                              1         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       8         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx  8
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
static void __on_tx_done (void);

/**
 * ���ͳ�ʱ�ص�����
 */
static void __on_tx_timeout (void);

/**
 * SX127x�����������
 */
static am_sx127x_handle_t __g_sx127x_handle;

static volatile aw_bool_t  __g_tx_done = AW_FALSE;

/**
 * sx127x ��ʱ���Ͳ��Գ������
 */
void demo_lora_tx_entry (void)
{
    char  str_buf[] = {"sx127x send"};

    /* ��ȡָ�� ID ���豸 */
    __g_sx127x_handle = awbl_sx127x_get_handle_by_id(LORA_TEST_DEV_ID);
    if (!__g_sx127x_handle) {
        am_lora_kprintf("awbl_sx127x_get_handle_by_id err!\r\n");
        while(1);
    }

    /* �����¼���ʼ�� */
    __g_radio_events.pfn_tx_done_cb    = __on_tx_done;
    __g_radio_events.pfn_tx_timeout_cb = __on_tx_timeout;
    am_sx127x_events_set(__g_sx127x_handle, &__g_radio_events);

    /* �����ŵ����������� */
    am_sx127x_channel_set(__g_sx127x_handle, RF_FREQUENCY);
    am_sx127x_tx_config_set(__g_sx127x_handle,
                            MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                            LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                            LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                            AM_LORA_TRUE, 0, 0, LORA_IQ_INVERSION_ON, 3000);

    __g_tx_done = AW_TRUE;

    while (1) {
        if (__g_tx_done) {
            /* �������� */
            am_sx127x_send(__g_sx127x_handle, (uint8_t *)str_buf, strlen(str_buf));
            __g_tx_done = AW_FALSE;
        }
        am_lora_mdelay(500);
    }
}

/**
 * ������ɻص�����
 */
static void __on_tx_done (void)
{
    am_lora_kprintf("TX Done!\r\n");
    aw_led_toggle(LORA_TEST_LED_ID);
    __g_tx_done = AW_TRUE;
}

/**
 * ���ͳ�ʱ�ص�����
 */
static void __on_tx_timeout (void)
{
    am_lora_kprintf("TX Timeout!\r\n");
}

/** [src_lora_tx] */

/* end of file */
