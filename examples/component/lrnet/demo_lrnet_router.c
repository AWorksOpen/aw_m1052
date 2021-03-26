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
 * \brief LoRaNet例程（路由）
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
 * \snippet demo_lrnet_router.c src_lrnet_router
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lrnet_router LoRaNet路由
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
 * \return 无
 */
void demo_lrnet_router_entry (void)
{
    int  ret = 0;

    const am_lrnet_router_cfg_t cfg = {
            &g_lrnet_network_cfg,                       /* 网络密钥 */
            {
                AM_LRNET_ADDR_ROUTER_LOWEST,            /* 设备地址 */
                3,                                      /* 重发次数 */
                AM_LRNET_MODE_PERIOD_WAKE               /* 供电模式 */
            },
            {
                {lrnet_common_rf_state_changed_cb },/* RF状态改变回调 */
                __fetch_request_cb                  /* 获取请求 */
            }
    };

    /* 调试信息输出 */
    lrnet_debug_startup_info("Router", cfg.p_net_cfg, &cfg.role_cfg);

    /* 初始化LoRaNet集中器设备 */
    ret = am_lrnet_router_init(&cfg);
    if (ret != AM_LORA_RET_OK) {
        am_lora_kprintf("\r\nInitialize Failed. Error Code %d", ret);
        while(1);
    }

}

/** [src_lrnet_router] */

/* end of file */
