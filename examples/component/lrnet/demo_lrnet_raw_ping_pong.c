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
 * \brief LoRaNet例程（ raw ping pong）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_DEV_SX127X (具体模块使用的SPI、GPIO引脚，请查看awbus_hwconf_sx127x.h)
 *      - AW_COM_CONSOLE
 *      - AW_COM_SHELL_SERIAL
 *      - AW_DEV_GPIO_LED
 *      - AW_DEV_EXTEND_PCF85063
 *      - 调试串口使能宏
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 准备两个设备，都运行该demo，刚开始两个都是从机设备，将一个设备通过shell输入命令
 *      raw_start
 *      它将成为主机设备，ping pong操作将由主机设备发起，从机设备接收到数据后将数据+1，又发
 *      回给主机设备，这样循环下去，通讯异常将导致 ping pong 操作结束。
 *
 * - 实验现象：
 *   1. 通过 shell 输入命令 raw_start, 当前设备
 *
 * \par 源代码
 * \snippet demo_lrnet_raw_ping_pong.c src_lrnet_raw_ping_pong
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lrnet_raw_ping_pong LoRaNet(主机/丛机)
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


#define _MASTER_ADDR        0x0800      /* 主机地址 */
#define _SLAVE_ADDR         0x0801      /* 从机地址 */

static volatile am_lora_bool_t __g_master_flg = AM_LORA_FALSE; /* 主机flag 真表示该设备为主机 */
static volatile am_lora_bool_t __g_reply_flg  = AM_LORA_FALSE; /* 回复flag 真表示需要回复 */
static volatile uint32_t  __g_value      = 0;

AW_MAILBOX_DECL_STATIC(__g_lrnet_raw_mailbox, 1);

static void _raw_rx (am_lrnet_raw_packet_t *p_packet);

/** 发送带4字节uint32_t数据的数据包 */
static int _raw_send_syn (uint16_t tar_addr, uint32_t value)
{
    am_lrnet_raw_packet_t packet = { 0 };

    packet.receiver  = tar_addr;                /* 数据包接收者 */
    packet.data_size = sizeof(value);           /* 数据包数据大小 */
    packet.p_data    = (uint8_t*) &value;       /* 数据包数据 */
    return am_lrnet_raw_tx(&packet);            /* 调用同步发送数据包API */
}

/** 收到Raw数据包, 保存收到的uint32_t数据存入_g_value, 置_g_reply_flg */
static void _raw_rx (am_lrnet_raw_packet_t *p_packet)
{
    if (sizeof(uint32_t) != p_packet->data_size) {
        am_lora_kprintf("\r\nRx Data Size Error");
        return;
    }

    __g_value =  p_packet->p_data[0]
             + (p_packet->p_data[1] << 8)
             + (p_packet->p_data[2] << 16)
             + (p_packet->p_data[2] << 24);     /* 小端字节序拼装数据 */
    __g_reply_flg = AM_LORA_TRUE;                /* 置reply_flg通知主循环回复 */

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
         * 等待 shell 是否切换为 raw mater 发送
         */
        ret = AW_MAILBOX_RECV(__g_lrnet_raw_mailbox, &__g_value, 1);
        if (ret == 0) {
            am_lrnet_dev_addr_set(_MASTER_ADDR);
            am_lora_kprintf("\r\nMaster Now. Addr 0x%04X",
                            (int) am_lrnet_dev_addr_get());
            __g_master_flg = AM_LORA_TRUE;
            __g_reply_flg = AM_LORA_TRUE;
        }

        if (__g_reply_flg) {             /* reply_flg置位, 则发送(__g_value + 1) */
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
 * \return 无
 */
void demo_lrnet_raw_ping_pong_entry (void)
{ 
    int   ret = 0;

    const am_lrnet_raw_cfg_t cfg = {
            &g_lrnet_network_cfg,                /* 网络配置 */
            {
                _SLAVE_ADDR,                     /* 设备地址: 复位后为从机 */
                0,                               /* 最大重发次数 */
                AM_LRNET_MODE_PERIOD_WAKE        /* 工作模式: 周期唤醒 */
            },
            {
                {lrnet_common_rf_state_changed_cb},  /* RF状态改变回调 */
                _raw_rx                              /* 收到Raw数据包回调 */
            }
    };

    /* 调试信息输出 */
    lrnet_debug_startup_info("Raw", cfg.p_net_cfg, &cfg.role_cfg);

    /* 初始化LoRaNet原始设备 */
    ret = am_lrnet_raw_init(&cfg);
    if (0 != ret) {
        am_lora_kprintf("\r\nInitialize failed. Error Code %d.", ret);
        while(1);
    }

    AW_MAILBOX_INIT(__g_lrnet_raw_mailbox, 1, AW_MAILBOX_Q_FIFO);
    __raw_cmd_init();

    {
        /* 定义任务实体，分配栈空间大小为2048  */
        AW_TASK_DECL_STATIC(lrnet_raw_task, 2048);

        AW_TASK_INIT(lrnet_raw_task,        /* 任务实体  */
                    "lrnet_raw_task",       /* 任务名字 */
                     5,                     /* 任务优先级 */
                     2048,                  /* 任务堆栈大小 */
                     __demo_libc_io_entry,  /* 任务入口函数 */
                     NULL);                 /* 任务入口参数 */

        AW_TASK_STARTUP(lrnet_raw_task);    /* 启动任务 */
    }
}

/** [src_lrnet_raw_ping_pong] */

/* end of file */
