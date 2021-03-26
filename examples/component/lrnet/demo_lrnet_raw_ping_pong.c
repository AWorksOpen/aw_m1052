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
 * \brief LoRaNet���̣� raw ping pong��
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
 *   3. ׼�������豸�������и�demo���տ�ʼ�������Ǵӻ��豸����һ���豸ͨ��shell��������
 *      raw_start
 *      ������Ϊ�����豸��ping pong�������������豸���𣬴ӻ��豸���յ����ݺ�����+1���ַ�
 *      �ظ������豸������ѭ����ȥ��ͨѶ�쳣������ ping pong ����������
 *
 * - ʵ������
 *   1. ͨ�� shell �������� raw_start, ��ǰ�豸
 *
 * \par Դ����
 * \snippet demo_lrnet_raw_ping_pong.c src_lrnet_raw_ping_pong
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lrnet_raw_ping_pong LoRaNet(����/�Ի�)
 * \copydoc demo_lrnet_raw_ping_pong.c
 */

/** [src_lrnet_raw_ping_pong] */
#include <stdlib.h>
#include "aworks.h"
#include "aw_shell.h"
#include "aw_task.h"
#include "aw_mailbox.h"
#include "am_lrnet.h"
#include "am_lora_utilities.h"
#include "common/demo_lrnet_common.h"


#define _MASTER_ADDR        0x0800      /* ������ַ */
#define _SLAVE_ADDR         0x0801      /* �ӻ���ַ */

static volatile am_lora_bool_t __g_master_flg = AM_LORA_FALSE; /* ����flag ���ʾ���豸Ϊ���� */
static volatile am_lora_bool_t __g_reply_flg  = AM_LORA_FALSE; /* �ظ�flag ���ʾ��Ҫ�ظ� */
static volatile uint32_t  __g_value      = 0;

AW_MAILBOX_DECL_STATIC(__g_lrnet_raw_mailbox, 1);

static void _raw_rx (am_lrnet_raw_packet_t *p_packet);

/** ���ʹ�4�ֽ�uint32_t���ݵ����ݰ� */
static int _raw_send_syn (uint16_t tar_addr, uint32_t value)
{
    am_lrnet_raw_packet_t packet = { 0 };

    packet.receiver  = tar_addr;                /* ���ݰ������� */
    packet.data_size = sizeof(value);           /* ���ݰ����ݴ�С */
    packet.p_data    = (uint8_t*) &value;       /* ���ݰ����� */
    return am_lrnet_raw_tx(&packet);            /* ����ͬ���������ݰ�API */
}

/** �յ�Raw���ݰ�, �����յ���uint32_t���ݴ���_g_value, ��_g_reply_flg */
static void _raw_rx (am_lrnet_raw_packet_t *p_packet)
{
    if (sizeof(uint32_t) != p_packet->data_size) {
        am_lora_kprintf("\r\nRx Data Size Error");
        return;
    }

    __g_value =  p_packet->p_data[0]
             + (p_packet->p_data[1] << 8)
             + (p_packet->p_data[2] << 16)
             + (p_packet->p_data[2] << 24);     /* С���ֽ���ƴװ���� */
    __g_reply_flg = AM_LORA_TRUE;                /* ��reply_flg֪ͨ��ѭ���ظ� */

    am_lora_kprintf("\r\nReceived from 0x%04X[%ddBm]: %d",
                    p_packet->sender, p_packet->rssi, (int) __g_value);
}

static int __raw_pingpong_start_cmd (int                   argc,
                                     char                 *argv[],
                                     struct aw_shell_user *p_shell_user)
{
    int num = 0;

    if (argc == 1) {
        num = atoi(argv[0]);
    }

    AW_MAILBOX_SEND(__g_lrnet_raw_mailbox,
                    num,
                    0,
                    AW_MAILBOX_PRI_NORMAL);

    return  0;
}

static const aw_shell_cmd_item_t __g_raw_cmds[] = {
    {__raw_pingpong_start_cmd, "raw_start",  "<num> - lrnet raw ping pong to test num"},
};

static void __raw_cmd_init(void)
{
    static aw_shell_cmd_list_t cmd_list;

    AW_SHELL_REGISTER_CMDS(&cmd_list, __g_raw_cmds);
}

aw_local void __demo_libc_io_entry(void  *p_arg)
{
    int  ret = 0;

    for (;;) {

        /*
         * �ȴ� shell �Ƿ��л�Ϊ raw mater ����
         */
        ret = AW_MAILBOX_RECV(__g_lrnet_raw_mailbox, &__g_value, 1);
        if (ret == 0) {
            am_lrnet_dev_addr_set(_MASTER_ADDR);
            am_lora_kprintf("\r\nMaster Now. Addr 0x%04X",
                            (int) am_lrnet_dev_addr_get());
            __g_master_flg = AM_LORA_TRUE;
            __g_reply_flg = AM_LORA_TRUE;
        }

        if (__g_reply_flg) {             /* reply_flg��λ, ����(__g_value + 1) */
            uint16_t addr;

            __g_reply_flg = AM_LORA_FALSE;

            if (__g_master_flg) {        /* Master -> Slave */
                addr = _SLAVE_ADDR;
                am_lora_kprintf("\r\nMaster Send: %d...", (int) __g_value + 1);
            } else {                     /* Slave -> Master */
                addr = _MASTER_ADDR;
                am_lora_kprintf("\r\nSlave Send: %d...",  (int) __g_value + 1);
            }

            if (AM_LORA_RET_OK == _raw_send_syn(addr, __g_value + 1)) {
                am_lora_kprintf("OK");
            } else {
                am_lora_kprintf("Failed");
            }
        }
    }
}

/**
 * \brief LoRaNet raw ping pong demo
 *
 * \return ��
 */
void demo_lrnet_raw_ping_pong_entry (void)
{ 
    int   ret = 0;

    const am_lrnet_raw_cfg_t cfg = {
            &g_lrnet_network_cfg,                /* �������� */
            {
                _SLAVE_ADDR,                     /* �豸��ַ: ��λ��Ϊ�ӻ� */
                0,                               /* ����ط����� */
                AM_LRNET_MODE_PERIOD_WAKE        /* ����ģʽ: ���ڻ��� */
            },
            {
                {lrnet_common_rf_state_changed_cb},  /* RF״̬�ı�ص� */
                _raw_rx                              /* �յ�Raw���ݰ��ص� */
            }
    };

    /* ������Ϣ��� */
    lrnet_debug_startup_info("Raw", cfg.p_net_cfg, &cfg.role_cfg);

    /* ��ʼ��LoRaNetԭʼ�豸 */
    ret = am_lrnet_raw_init(&cfg);
    if (0 != ret) {
        am_lora_kprintf("\r\nInitialize failed. Error Code %d.", ret);
        while(1);
    }

    AW_MAILBOX_INIT(__g_lrnet_raw_mailbox, 1, AW_MAILBOX_Q_FIFO);
    __raw_cmd_init();

    {
        /* ��������ʵ�壬����ջ�ռ��СΪ2048  */
        AW_TASK_DECL_STATIC(lrnet_raw_task, 2048);

        AW_TASK_INIT(lrnet_raw_task,        /* ����ʵ��  */
                    "lrnet_raw_task",       /* �������� */
                     5,                     /* �������ȼ� */
                     2048,                  /* �����ջ��С */
                     __demo_libc_io_entry,  /* ������ں��� */
                     NULL);                 /* ������ڲ��� */

        AW_TASK_STARTUP(lrnet_raw_task);    /* �������� */
    }
}

/** [src_lrnet_raw_ping_pong] */

/* end of file */
