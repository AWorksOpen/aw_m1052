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

/**
 * \file
 * \brief LoRaNet���̣��նˣ�
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_SX127X (����ģ��ʹ�õ�SPI��GPIO���ţ���鿴awbus_hwconf_sx127x.h)
 *      - AW_COM_CONSOLE
 *      - AW_DEV_GPIO_LED
 *      - AW_DEV_EXTEND_PCF85063
 *      - ���Դ���ʹ�ܺ�
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ׼�������豸��һ���豸���ظ����̣���һ������demo_lrnet_center.c���̣�
 *
 * - ʵ������
 *   1. ��������Ҫ���demo_lrnet_center.c����ʹ�ã�ʵ������μ�demo_lrnet_center.c���̡�
 *
 * \par Դ����
 * \snippet demo_lrnet_terminal.c src_lrnet_terminal
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lrnet_terminal LoRaNet�ն�
 * \copydoc demo_lrnet_terminal.c
 */

/** [src_lrnet_terminal] */

#include <stdio.h>
#include <string.h>
#include "am_lrnet.h"
#include "am_lora_utilities.h"
#include "common/demo_lrnet_common.h"



static volatile float __g_temperature;

/** Fetch����ص� */
static
int __terminal_fetch_request_handler (am_lrnet_fetch_request_t *p_request)
{
    if (NULL == p_request) {
        return 0;
    }

    am_lora_kprintf("\r\nFetch Request: '%s'", p_request->p_data);

    /* �յ����� ��ȡ�¶� */
    if (0 == strcmp((char*) p_request->p_data, CMD_GET_TEMPERATURE)) {
        int   temp_integer, temp_decimal, nchar;

        /* �����λС�� */
        temp_integer = (int) __g_temperature;
        temp_decimal = (__g_temperature - temp_integer) * 100;
        nchar = snprintf((char *)p_request->p_respond_buffer,
                         p_request->respond_buffer_size,
                         "%d.%02d", temp_integer, temp_decimal);
        if (nchar + 1 > p_request->respond_buffer_size) {
            am_lora_kprintf("\r\nRespond buffer lack!");
            return 0;
        } else {
            am_lora_kprintf("\r\nRespond: '%s'", p_request->p_respond_buffer);
            return nchar + 1;
        }
    } else {
        am_lora_kprintf("\r\nUndefined Command");
        return 0;
    }
}

static
int __terminal_upstream_request_handler (am_lrnet_upstream_request_t *p_request)
{
    const uint8_t data[] = "Upstream Test!";

    if (p_request->respond_buffer_size >= sizeof(data)) {
        memcpy(p_request->p_respond_buffer, data, sizeof(data));
        return sizeof(data);
    } else {
        return 0;
    }
}

/** �յ��쳣�ϱ�ACK�ص� */
static
void __terminal_exception_ack_rx_handler (void)
{
    am_lora_kprintf("\r\nException ACK Received.");
}

/**
 * \brief LoRaNet terminal demo
 *
 * \return ��
 */
void demo_lrnet_terminal_entery (void)
{
    int  ret = 0;

    static am_lrnet_terminal_cfg_t cfg = {
        &g_lrnet_network_cfg,                           /* ������Կ */
        {
            AM_LRNET_ADDR_TERMINAL_LOWEST+1,            /* �豸��ַ */
            0,                                          /* �ط����� */
            AM_LRNET_MODE_PERIOD_WAKE                   /* ����ģʽ */
        },
        {
            {lrnet_common_rf_state_changed_cb },        /* RF״̬�ı�ص� */
            __terminal_fetch_request_handler,           /* Fetch����ص� */
            __terminal_upstream_request_handler,        /* ��ʱ�ϱ�����ص� */
            __terminal_exception_ack_rx_handler         /* �쳣�ϱ�ACK���ջص� */
        }
    };

    /* ������Ϣ��� */
    lrnet_debug_startup_info("Terminal", cfg.p_net_cfg, &cfg.role_cfg);

    /* ��ʼ��LoRaNet�ն��豸 */
    ret = am_lrnet_terminal_init(&cfg);
    if (0 != ret) {
        am_lora_kprintf("\r\nInitialize Failed. Error Code %d.", ret);
        while(1);
    }
}

/** [src_lrnet_terminal] */

/* end of file */
