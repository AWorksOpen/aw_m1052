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
 * \brief LoRaNet例程（终端）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_DEV_SX127X (具体模块使用的SPI、GPIO引脚，请查看awbus_hwconf_sx127x.h)
 *      - AW_COM_CONSOLE
 *      - AW_DEV_GPIO_LED
 *      - AW_DEV_EXTEND_PCF85063
 *      - 调试串口使能宏
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 准备两个设备，一个设备下载该例程，另一个下载demo_lrnet_center.c例程；
 *
 * - 实验现象：
 *   1. 该例程需要配合demo_lrnet_center.c例程使用，实验现象参见demo_lrnet_center.c例程。
 *
 * \par 源代码
 * \snippet demo_lrnet_terminal.c src_lrnet_terminal
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lrnet_terminal LoRaNet终端
 * \copydoc demo_lrnet_terminal.c
 */

/** [src_lrnet_terminal] */

#include <stdio.h>
#include <string.h>
#include "am_lrnet.h"
#include "am_lora_utilities.h"
#include "common/demo_lrnet_common.h"



static volatile float __g_temperature;

/** Fetch请求回调 */
static
int __terminal_fetch_request_handler (am_lrnet_fetch_request_t *p_request)
{
    if (NULL == p_request) {
        return 0;
    }

    am_lora_kprintf("\r\nFetch Request: '%s'", p_request->p_data);

    /* 收到命令 读取温度 */
    if (0 == strcmp((char*) p_request->p_data, CMD_GET_TEMPERATURE)) {
        int   temp_integer, temp_decimal, nchar;

        /* 输出两位小数 */
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

/** 收到异常上报ACK回调 */
static
void __terminal_exception_ack_rx_handler (void)
{
    am_lora_kprintf("\r\nException ACK Received.");
}

/**
 * \brief LoRaNet terminal demo
 *
 * \return 无
 */
void demo_lrnet_terminal_entery (void)
{
    int  ret = 0;

    static am_lrnet_terminal_cfg_t cfg = {
        &g_lrnet_network_cfg,                           /* 网络密钥 */
        {
            AM_LRNET_ADDR_TERMINAL_LOWEST+1,            /* 设备地址 */
            0,                                          /* 重发次数 */
            AM_LRNET_MODE_PERIOD_WAKE                   /* 供电模式 */
        },
        {
            {lrnet_common_rf_state_changed_cb },        /* RF状态改变回调 */
            __terminal_fetch_request_handler,           /* Fetch请求回调 */
            __terminal_upstream_request_handler,        /* 分时上报请求回调 */
            __terminal_exception_ack_rx_handler         /* 异常上报ACK接收回调 */
        }
    };

    /* 调试信息输出 */
    lrnet_debug_startup_info("Terminal", cfg.p_net_cfg, &cfg.role_cfg);

    /* 初始化LoRaNet终端设备 */
    ret = am_lrnet_terminal_init(&cfg);
    if (0 != ret) {
        am_lora_kprintf("\r\nInitialize Failed. Error Code %d.", ret);
        while(1);
    }
}

/** [src_lrnet_terminal] */

/* end of file */
