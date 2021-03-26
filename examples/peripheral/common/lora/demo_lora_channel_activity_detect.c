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
 * \brief sx127x CADģʽʹ������
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
 *   �� demo �����ԵĿ���CAD����⵱ǰ�ŵ��Ƿ������ݷ���
 *
 * \par Դ����
 * \snippet demo_lora_channel_activity_detect.c src_lora_cad
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lora_cad lora(CADģʽ)
 * \copydoc demo_lora_channel_activity_detect.c
 */

/** [src_lora_cad] */

#include "am_lora_type.h"
#include "am_sx127x.h"
#include "am_lora_utilities.h"
#include "driver/lora/awbl_sx127x.h"
#include "aw_led.h"

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
static am_lora_radio_events_t __g_lora_radio_events;

/**
 * CAD��ɻص�����
 */
static void __on_cad_done (am_lora_bool_t channelActivityDetected);

/**
 * SX1276�����������
 */
static am_sx127x_handle_t __g_sx127x_handle;

/**
 * CAD��ɱ�־
 */
static volatile aw_bool_t  __g_sx127x_cad_done = AW_TRUE;

/**
 * LoRa�ŵ���Ծ���Գ������
 */
void demo_lora_channel_activity_detect_entry (void)
{
    /* ��ȡָ�� ID ���豸 */
    __g_sx127x_handle = awbl_sx127x_get_handle_by_id(LORA_TEST_DEV_ID);
    if (!__g_sx127x_handle) {
        am_lora_kprintf("awbl_sx127x_get_handle_by_id err!\r\n");
        while(1);
    }

    /* �����¼���ʼ�� */
    __g_lora_radio_events.pfn_cad_done_cb = __on_cad_done;
    am_sx127x_events_set(__g_sx127x_handle, &__g_lora_radio_events);

    /* �����ŵ����������� */
    am_sx127x_channel_set(__g_sx127x_handle, RF_FREQUENCY);
    am_sx127x_rx_config_set(__g_sx127x_handle,
                            MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                            LORA_CODINGRATE, 0, 90,
                            LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                            0, AM_LORA_TRUE, 0, 0, LORA_IQ_INVERSION_ON, AM_LORA_FALSE);

    while (1) {
        if (__g_sx127x_cad_done) {
            /* ����CAD���
             *
             * ���ʱ�䣺(2^SF + 32)/BW (��λ��)
             *
             */
            __g_sx127x_cad_done = AW_FALSE;
            am_sx127x_cad_start(__g_sx127x_handle);
        }

        am_lora_mdelay(10);
    }
}

/**
 * CAD��ɻص�����
 */
static void __on_cad_done (am_lora_bool_t channel_activity_detected)
{
    if (channel_activity_detected == AM_LORA_TRUE) {
        am_lora_kprintf("Channel is active.\r\n");
        aw_led_toggle(LORA_TEST_LED_ID);

    } else {
        /* ��ⳬʱ�л���˯��ģʽ */
        am_sx127x_sleep(__g_sx127x_handle);
        am_lora_kprintf("Channel is negative.\r\n");
    }

    __g_sx127x_cad_done = AW_TRUE;
}

/** [src_lora_cad] */

/* end of file */
