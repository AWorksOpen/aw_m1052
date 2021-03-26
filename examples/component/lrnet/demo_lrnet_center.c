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
 * \brief LoRaNet���̣���������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_SX127X (����ģ��ʹ�õ�SPI��GPIO���ţ���鿴awbus_hwconf_sx127x.h)
 *      - AW_COM_CONSOLE
 *      - AW_COM_SHELL_SERIAL
 *      - AW_DEV_GPIO_LED
 *      - AW_DEV_EXTEND_PCF85063
 *      - ���Դ���ʹ�ܺ�
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ׼�������豸��һ���豸���ظ����̣���һ������demo_lrnet_router.c���̻���demo_lrnet_terminal.c���̣�
 *
 * - ʵ������
 *   1. ��demo��ע�����shell����磺
 *      - cdetect   ��������̽����Χ��·�ɻ��ն�
 *      - cping     �������� ping ·�ɻ��ն�
 *      - cgfetch   ����������·�ɱ��е������ն˽��з���ɼ�����Ҫ��ִ�� cdetect ������·�ɱ���Ϣ��
 *      �û�����ͨ��ִ��shell����������LoRaNetЭ��ջ center ��ؽӿ�
 *   2. ִ�� "cdetect" shell������ڽ���ӡ��������̽�⵽�� "·��" ���� "�ն�" �豸��Ϣ��
 *   3. �ӵ� 2 ���л���ն˻���·�ɵĵ�ַ��ִ�� "cping 0x0101" shell������ڽ���ӡ���������͵�ַΪ  "0x0101" ��  "�ն�" ������״̬��
 *      "0x0101" ��ַ�ɸı䣬�û���ͨ�� "cdetect" �鿴"·��" ���� "�ն�"��ַ��
 *   5. ִ��  "cgfetch" shell������ڽ���ӡ����������·�ɱ��е� "�ն�" �豸�����շ�������Ϣ��
 *      ִ��  "cgfetch" shell��ǰ�� ��Ҫ��ִ�� cdetect ������·�ɱ���Ϣ��
 *
 * - ��ע��
 *   1.�ն˿���ֱ�ӹ����ڼ������£�Ҳ���Թ�����·���£�ͨ��·��(��������)�ͼ��������ӡ���ϵͼ����:
 *                      ���������� ���� ����>·�ɡ��� ���� ����>�ն�
 *                        |
 *                        |
 *                       \|/
 *                        .
 *                       �ն�
 *
 * \par Դ����
 * \snippet demo_lrnet_center.c src_lrnet_center
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lrnet_center LoRaNet������
 * \copydoc demo_lrnet_center.c
 */

/** [src_lrnet_center] */

#include <stdio.h>
#include "aworks.h"
#include "aw_shell.h"
#include "am_lrnet.h"
#include "am_lora_utilities.h"
#include "common/demo_lrnet_common.h"


/*
 * �����ӡ·�ɱ�
 */
static void __print_route_table (void)
{
    uint8_t  r_buf[2];
    uint16_t t_buf[2];
    int      r_origin, t_origin, r_num, t_num, i, j, terminal_cnt, router_cnt;

    am_lora_kprintf("\r\n"
                    "\r\n--------------------------------------"
                    "\r\n  Route Table Print"
                    "\r\n--------------------------------------");

    router_cnt   = 0;
    terminal_cnt = 0;
    r_origin     = 0;

    /* ����·�ɱ���Ϣ */
    r_num = am_lrnet_center_routers_get(r_buf, sizeof(r_buf), &r_origin);
    uint8_t r_addr = r_buf[0];
    /*��ӡ��������ַ����������ַ�̶������·�ɱ����λ*/
    am_lora_kprintf("\r\n\r\nCenter addr: 0x%02X\r\n", (int)r_addr);

    /* �鼯�����µ��ն� */
    t_origin = 0;
    while (0 != (t_num = am_lrnet_center_route_sub_dev_get(
                            r_addr, t_buf, sizeof(t_buf), &t_origin))) {
        for(j = 0; j < t_num; j++) {
            uint16_t t_addr = t_buf[j];

            if (AM_LRNET_ADDR_IS_TERMINAL(t_addr)) {
                am_lora_kprintf("\r\nTerminal addr: 0x%04X\r\n", (int) t_addr);
                terminal_cnt += 1;
            }
        }
    }
    /*��ӡ�������¹��ص��ն�*/
    if(terminal_cnt > 0){
        am_lora_kprintf("\r\n\r\nCenter 1, Terminal %d\r\n--------------------", (int) terminal_cnt);
    }

    /*���������ڼ������µ�·��*/
    if (r_num > 1) {
        terminal_cnt = 0;
        for (i = 1; i < r_num; i++) {
            uint8_t r_addr = r_buf[i];
            /*��ӡ·�ɵ�ַ*/
            am_lora_kprintf("\r\n\r\nRouter addr: 0x%02X\r\n", (int) r_addr);

            /* ����������·���µ��ն�  */
            t_origin = 0;
            while (0 != (t_num = am_lrnet_center_route_sub_dev_get(
                                    r_addr, t_buf, sizeof(t_buf), &t_origin))) {
                for(j = 0; j < t_num; j++) {
                    uint16_t t_addr = t_buf[j];

                    if (AM_LRNET_ADDR_IS_TERMINAL(t_addr)) {
                        am_lora_kprintf("\r\nTerminal 0x%04X", (int) t_addr);
                        terminal_cnt += 1;
                    }
                }
            }
            router_cnt++;
        }
    }
    if(router_cnt > 0){
        am_lora_kprintf("\r\n\r\nCenter 1, Router %d, Terminal %d",
                         (int) router_cnt, (int) terminal_cnt);
    }

    return ;
}

static void __gfetch_rx_cb (void *p_arg, am_lrnet_data_t *p_rx_data)
{
    if (0 != p_rx_data->data_size) {
        am_lora_kprintf("\r\nGFetch 0x%04X Rx: %s",
                        p_rx_data->dev_addr, p_rx_data->p_data);
    } else {
        am_lora_kprintf("\r\nGFetch 0x%04X Rx: Invalid",
                         p_rx_data->dev_addr);
    }
}

static void __gfetch_finish_cb (void *p_arg)
{
    am_lora_kprintf("\r\nGFetch Completed.");
}

static int __fetch_request_cb (am_lrnet_fetch_request_t *p_request)
{
    am_lora_kprintf("\r\nFetch Request: %s", p_request->p_data);
    return 0;   /* do not reply */
}

static void __upstream_rx_cb (am_lrnet_data_t *p_data)
{
    am_lora_kprintf("\r\nUpstream Rx: '%s'", p_data->p_data);
}

static void __exception_rx_cb (am_lrnet_data_t *p_data)
{
    am_lora_kprintf("\r\nException Rx: '%s'", p_data->p_data);
}

/*
 * ִ��̽������
 * ����sub_router_num    :  Ԥ�ڵ�·������
 * ����sub_terminal_num  :  Ԥ�ڵ��ն�����
 */
static void __detect_test (int sub_router_num, int sub_terminal_num)
{
    uint32_t  sec1, sec2;

    am_lora_kprintf("\r\nDetect Startup.");

    sec1 = am_lora_utilities_rtc_sec_get();
    if (AM_LORA_RET_OK == am_lrnet_center_detect(sub_router_num, sub_terminal_num)) {
        sec2 = am_lora_utilities_rtc_sec_get();
        am_lora_kprintf("\r\nNetlink Completed! %ds", sec2-sec1);
        __print_route_table();
    }
}

/*
 * ����ɼ�����
 * ��·�ɱ��е������ն˽��з���ɼ�
 */
static void __gfetch_test (void)
{
    am_lrnet_center_gfetch_cfg_t cfg;
    int                          ret;

    /* ����ɼ����� */
    cfg.p_send_data     = (uint8_t *) CMD_GET_TEMPERATURE;
    cfg.send_data_size  = sizeof(CMD_GET_TEMPERATURE);
    cfg.fetch_data_size = _GFETCH_ACK_DATA_SIZE;

    /* ��������ɼ� */
    ret = am_lrnet_center_gfetch_asyn(&cfg,              /* ����ɼ����� */
                                      __gfetch_rx_cb,     /* �յ����ݻص� */
                                      __gfetch_finish_cb, /* �ɼ���ϻص� */
                                      (void*) 0);        /* ����p_arg */
    if (AM_LORA_RET_OK == ret) {
        am_lora_kprintf("\r\nGFetch Startup. Sent: %s",
                        (char*) cfg.p_send_data);
    } else {
        am_lora_kprintf("\r\nGFetch Startup Failed, error code=%d", (int) ret);
    }
}

/*
 * ����ɼ���ָ���� ����
 * ��ָ��·���µ�ָ��������̨�ն�ִ�з���ɼ�
 */
static void __gfetch_specific_test (void)
{
    const uint16_t addr_list[] = { 0x0100, 0x0101, 0x0102, 0x0103 };

    am_lrnet_center_gfetch_cfg_t cfg;
    int                          ret;

    /* ����ɼ����� */
    cfg.p_send_data     = (uint8_t *) CMD_GET_TEMPERATURE;
    cfg.send_data_size  = sizeof(CMD_GET_TEMPERATURE);
    cfg.fetch_data_size = _GFETCH_ACK_DATA_SIZE;

    ret= am_lrnet_center_gfetch_specific_asyn(
                 &cfg,
                 AM_LRNET_ADDR_CENTER,
                 addr_list,
                 AM_LORA_NELEMENTS(addr_list),
                 __gfetch_rx_cb,
                 __gfetch_finish_cb,
                 (void*)0);
    if (ret != AM_LORA_RET_OK) {
        am_lora_kprintf("\r\nGFetch_Specific Failed, error code=%d", ret);
    }
}

/*
 * ִ����ʱ�㲥
 */
static void __timebc_test (void)
{
    int ret = 0;

    am_lora_kprintf("\r\nTimeBc Test Start");

    ret = am_lrnet_center_time_broadcast();
    if (ret == AM_LORA_RET_OK) {
        am_lora_kprintf("\r\nTimeBc Test Success");
    } else {
        am_lora_kprintf("\r\nTimeBc Test Failed");
    }
}

/*
 * ��ʱ�ϱ��㲥����
 * �豸Ⱥ��ÿ��00:10:00ִ�з�ʱ�ϱ������ն�0x0100��ʼ���ݵ�ַ���������ϱ�
 * �����ն�0x0100-0x01FF��256
 * ��ע�ⶨ��ִ����ʱ�㲥�Ա���RTCʱ��ͬ��
 */
static void __upstreambc_test (void)
{
    am_lrnet_upstream_cfg_t cfg = {
        3,              /* �ϴ�ʱ���Ƭ����λ���룩 */
        { 0 }           /* ÿ���ϴ�ʱ������ */
    };

    am_lora_kprintf("\r\nUpstreamBc Test Start");

    AM_LRNET_UPSTREAM_TIME_BITMAP_SET(cfg.time_bitmap, 0, 10);
    (void) am_lrnet_center_upstream_broadcast(&cfg);

    am_lora_kprintf("\r\nUpstreamBc Test Finish");
}

/*
 * ·�ɶ������ԣ�
 * ����·�ɱ�center <- 0x01 <- 0x02 <- 0x03 <- 0x0100
 * ����0x0100ִ��fetch�������ɹ���ȡ�������ݣ�����Գɹ�
 */
static void __multihop_test (void)
{
    uint8_t         rx_buf[20];
    am_lrnet_data_t tx_data, rx_data;

    am_lrnet_center_route_add(0x01, 0x00);
    am_lrnet_center_route_add(0x02, 0x01);
    am_lrnet_center_route_add(0x03, 0x02);
    am_lrnet_center_route_add(0x0100, 0x03);

    tx_data.p_data    = (uint8_t *) CMD_GET_TEMPERATURE;
    tx_data.data_size = sizeof(CMD_GET_TEMPERATURE);
    tx_data.dev_addr  = 0x0100;
    if (AM_LORA_RET_OK == am_lrnet_center_fetch(&tx_data, rx_buf, sizeof(rx_buf), &rx_data)) {
        am_lora_kprintf("\r\n%s", (char *) rx_data.p_data);
    } else {
        am_lora_kprintf("\r\nFetch Failed");
    }
}

#ifdef AW_ARM926EJ_S
extern volatile uint32_t  g_max_tick_use;
extern volatile uint32_t  g_sm_max_tick_use;
static int  __get_max_use_tick(int                   argc,
                               char                 *argv[],
                               struct aw_shell_user *p_shell_user)
{
    aw_kprintf("sm max: %u, exe max: %u\n", g_sm_max_tick_use, g_max_tick_use);

    return  0;
}
#endif

static int __center_timebc_test_cmd (int                   argc,
                                     char                 *argv[],
                                     struct aw_shell_user *p_shell_user)
{
    __timebc_test();

    return  AW_OK;
}

static int __center_upstreambc_test_cmd (int                   argc,
                                         char                 *argv[],
                                         struct aw_shell_user *p_shell_user)
{
    __upstreambc_test();

    return  AW_OK;
}

static int __center_detect_cmd (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *p_shell_user)
{
    __detect_test(10, 100);

    return  AW_OK;
}

static int __center_gfetch_cmd (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *p_shell_user)
{
    __gfetch_test();

    return  AW_OK;
}

static int __center_ping_cmd (int                   argc,
                              char                 *argv[],
                              struct aw_shell_user *p_shell_user)
{
    int       used_ms = 0;
    uint16_t  addr    = 0x0101;

    am_lora_kprintf("\r\nPing Test Start\r\n");

    if (argc == 1) {
        addr = lrnet_simple_strtoul(argv[0], NULL, 16);
        am_lora_kprintf("addr: 0x%04x\n", addr);

        used_ms = am_lrnet_center_ping(addr, 20);
        if (used_ms >= 0) {
            am_lora_kprintf("\r\nPing %d(20byte) Test Success, %dms", addr, used_ms);
        } else {
            am_lora_kprintf("\r\nPing %d(20byte) Test Failed", addr);
        }

        return  AW_OK;
    }else{
        am_lora_kprintf("must designate addr\n");
        return AW_ERROR;
    }
}

static const aw_shell_cmd_item_t __g_center_cmds[] = {
    {__center_detect_cmd, "cdetect",  "center detect"},
    {__center_gfetch_cmd, "cgfetch",  "center gfetch"},
    {__center_ping_cmd, "cping",  "<addr> center ping"},
    {__center_upstreambc_test_cmd, "cupsbc",  "center broadcast the time-sharing report"},
    {__center_timebc_test_cmd, "ctimebc",  "center broadcast timing"},
#ifdef AW_ARM926EJ_S
    {__get_max_use_tick, "get_tick", "get loranet use max tick"},
#endif
};

static void __center_cmd_init(void)
{
    static aw_shell_cmd_list_t cmd_list;

    AW_SHELL_REGISTER_CMDS(&cmd_list, __g_center_cmds);
}

/**
 * \brief LoRaNet center demo
 *
 * \return ��
 */
void  demo_lrnet_center_entry (void)
{
    int  ret;

    const am_lrnet_center_cfg_t cfg = {
            &g_lrnet_network_cfg,                   /* �������� */
            {
                AM_LRNET_ADDR_CENTER,               /* �豸��ַ */
                2,                                  /* ����ط����� */
                AM_LRNET_MODE_NORMAL                /* ����ģʽ */
            },
            {
                {lrnet_common_rf_state_changed_cb }, /* RF״̬�ı�ص� */
                __fetch_request_cb,                  /* ��ȡ���� */
                __upstream_rx_cb,                    /* �յ���ʱ�ϱ����ݻص� */
                __exception_rx_cb                    /* �յ��쳣�ϱ��ص� */
            }
    };

    /* ������Ϣ��� */
    lrnet_debug_startup_info("Center", cfg.p_net_cfg, &cfg.role_cfg);

    /* ��ʼ��LoRaNet�������豸 */
    ret = am_lrnet_center_init(&cfg);
    if (ret != 0) {
        am_lora_kprintf("\r\nInitialize Failed. Error Code %d", ret);
        while(1);
    }

    /* ��ʼ�� shell ���� */
    __center_cmd_init();
}

/** [src_lrnet_center] */

/* end of file */
