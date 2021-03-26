/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/*
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

#include "demo_lrnet_common.h"
#include "am_lora_utilities.h"
#include "aw_led.h"

#include <ctype.h>

/* DEMO����ʱ�Ƿ����͹���ģʽ�ı�־ */
static volatile am_lora_bool_t __g_lowpower_flg = AM_LORA_TRUE;

/* ������Կ ��������У���ֹ�������޸� */
static const uint8_t  __g_network_key[16] = { 0x01, 0x02, 0x03, 0x04,
                                              0x05, 0x06, 0x07, 0x08,
                                              0x09, 0x0a, 0x0b, 0x0c,
                                              0x0d, 0x0e, 0x0f, 0x10 };

/* LoRaNet�������� ��ȷ�������е��豸ӵ����ͬ���������� */
const am_lrnet_net_cfg_t g_lrnet_network_cfg = {
        NETWORK_ID,                         /* ����ID */
        -95,                                /* ·����СRSSIֵ��dBm�� */
        20,                                 /* ��������̴���ʱ�䣨���룩 */
        0,                                  /* ˯�߼����%�� */
        3000,                               /* ˯�߼�������룩 */
        1000,                               /* �����Ծʱ˯�߼�������룩 */
        0,                                  /* ���������Ծʱ�䣨�룩 */
        __g_network_key,                    /* ������Կ */

        /* ��Ƶ���� */
        {
                490000000,                  /* LoRa�����ŵ�Ƶ�� 490MHz */
                490300000,                  /* LoRa ACK�ŵ�Ƶ�� 490.3MHz */
                AM_LRNET_RF_RATE_4800,      /* LoRa��������: 19.2kbps */
                AM_LRNET_RF_CR_5,           /* LoRa������ 4/5 */
                20,                         /* LoRa���书�� +20dBm */
        }
};

/*  ����͹���ģʽ����Ҫ����ʹ������Ϊ���գ����� IO ���� */
static void __led_io_set_float(am_lora_bool_t  enable)
{
    ;
}

void lrnet_common_rf_state_changed_cb (enum am_lrnet_rf_state state)
{
    /* ���ݵ�ǰRF״̬ �ı�LED������ */
    switch (state) {
    case AM_LRNET_RF_STATE_IDLE:
        if (__g_lowpower_flg) {
            __led_io_set_float(1);
        }
        aw_led_off(LED_STATE);
        aw_led_off(LED_ACK);
        break;

    case AM_LRNET_RF_STATE_RX:
        if (__g_lowpower_flg) {
            __led_io_set_float(0);
        }
        aw_led_on(LED_STATE);
        aw_led_off(LED_ACK);
        break;

    case AM_LRNET_RF_STATE_TX:
        if (__g_lowpower_flg) {
            __led_io_set_float(0);
        }
        aw_led_off(LED_STATE);
        aw_led_on(LED_ACK);
        break;

    default:
        break;
    }
}

static void __debug_data_print (const uint8_t *p_data, uint32_t size)
{
    int i, j;

    for (i = 0, j = 0; i < (int)size; i++) {
        am_lora_kprintf("%02X ", (int) p_data[i]);

        if (++j >= 16) {
            j = 0;
            am_lora_kprintf("\r\n");
        }
    }
}

void lrnet_debug_startup_info (const char                       *p_dev_name,
                               const am_lrnet_net_cfg_t         *p_net_cfg,
                               const am_lrnet_role_common_cfg_t *p_role_cfg)
{
    const char *(str_mode[]) = { "NORMAL", "PERIOD_WAKE", "SLEEP" };

    int rate;   /* unit: 0.1 */

    am_lora_kprintf("\x0cLoRaNet %s Example Startup! LoRaNet Lib %s",
                    p_dev_name, am_lrnet_version_get());

    am_lora_kprintf("\r\n\r\nNet Config:\r\n"
                 "net_id=0x%02X"
                 " wakeup_interval=%d-%dms",
                 (int) p_net_cfg->net_id,
                 (int) p_net_cfg->wakeup_interval_short,
                 (int) p_net_cfg->wakeup_interval);
    am_lora_kprintf("\r\nNetwork Key=");
    __debug_data_print(p_net_cfg->p_network_key, 16);

    rate = 3 << (int)p_net_cfg->rf_cfg.rate;
    am_lora_kprintf("\r\nRF Config:\r\n"
                    "data_channel=%dHz"
                    " ack_channel=%dHz"
                    " rate=%d.%dkbps"
                    " crc=4/%d"
                    " tx_power=%ddbm",
                    (int) p_net_cfg->rf_cfg.data_channel,
                    (int) p_net_cfg->rf_cfg.ack_channel,
                    rate / 10, rate % 10,
                    (int) p_net_cfg->rf_cfg.cr + 5,
                    (int) p_net_cfg->rf_cfg.tx_power);

    am_lora_kprintf("\r\n\r\nRole Config:"
                    "\r\ndev_addr=0x%04X"
                    " send_retry_max=%d"
                    " work_mode=%s",
                    (int) p_role_cfg->dev_addr,
                    (int) p_role_cfg->tx_retry,
                    str_mode[p_role_cfg->mode]);
}

unsigned int lrnet_simple_strtoul(const char *cp,char **endp,unsigned int base)
{
    unsigned int result = 0,value;

    if (*cp == '0') {
        cp++;
        if ((*cp == 'x') && isxdigit((int)cp[1])) {
            base = 16;
            cp++;
        }
        if (!base) {
            base = 8;
        }
    }

    if (!base) {
        base = 10;
    }

    while ( isxdigit((int)*cp) && (value = isdigit((int)*cp) ? *cp-'0' : \
            (islower((int)*cp) ? \
            toupper((int)*cp) : *cp)-'A'+10) < base) {
        result = result*base + value;
        cp++;
    }

    if (endp) {
        *endp = (char *)cp;
    }

    return result;
}
