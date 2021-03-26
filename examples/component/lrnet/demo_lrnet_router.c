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
 * \brief LoRaNet���̣�·�ɣ�
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
 * \snippet demo_lrnet_router.c src_lrnet_router
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lrnet_router LoRaNet·��
 * \copydoc demo_lrnet_router.c
 */

/** [src_lrnet_router] */

#include "am_lrnet.h"
#include "am_lora_utilities.h"
#include "common/demo_lrnet_common.h"

static int __fetch_request_cb (am_lrnet_fetch_request_t *p_request)
{
    am_lora_kprintf("\r\nFetch Request: %s", p_request->p_data);
    return 0;   /* do not reply */
}

/**
 * \brief LoRaNet router demo
 *
 * \return ��
 */
void demo_lrnet_router_entry (void)
{
    int  ret = 0;

    const am_lrnet_router_cfg_t cfg = {
            &g_lrnet_network_cfg,                       /* ������Կ */
            {
                AM_LRNET_ADDR_ROUTER_LOWEST,            /* �豸��ַ */
                3,                                      /* �ط����� */
                AM_LRNET_MODE_PERIOD_WAKE               /* ����ģʽ */
            },
            {
                {lrnet_common_rf_state_changed_cb },/* RF״̬�ı�ص� */
                __fetch_request_cb                  /* ��ȡ���� */
            }
    };

    /* ������Ϣ��� */
    lrnet_debug_startup_info("Router", cfg.p_net_cfg, &cfg.role_cfg);

    /* ��ʼ��LoRaNet�������豸 */
    ret = am_lrnet_router_init(&cfg);
    if (ret != AM_LORA_RET_OK) {
        am_lora_kprintf("\r\nInitialize Failed. Error Code %d", ret);
        while(1);
    }

}

/** [src_lrnet_router] */

/* end of file */
