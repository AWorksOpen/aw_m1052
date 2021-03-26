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

#ifndef __DEMO_LRNET_COMMON_H
#define __DEMO_LRNET_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet.h"

#define LED_STATE     0
#define LED_ACK       1

/* 网络ID 用于区分不同的LoRaNet网络 */
#define NETWORK_ID                0x11

/** group fetch分组采集返回数据的大小 */
#define _GFETCH_ACK_DATA_SIZE               8

/* 获取温度命令, 收到命令的终端立即返回温度字符串 */
#define CMD_GET_TEMPERATURE "Get Temperature"

/* 突发上报命令，收到命令的终端立即执行突发上报 */
#define CMD_DO_BURST_REPORT "Do Burst Report"

/* 通知数据 */
#define DATA_INFORMATION     "Information"

/* 异常数据 */
#define DATA_EXCEPTION      "Expection"

/* lrnet network config */
extern const am_lrnet_net_cfg_t g_lrnet_network_cfg;

/*
 * lrnet 状态切换回调
 */
void lrnet_common_rf_state_changed_cb (enum am_lrnet_rf_state state);

/*
 * lrnet 初始化参数打印
 */
void lrnet_debug_startup_info (const char                       *p_dev_name,
                               const am_lrnet_net_cfg_t         *p_net_cfg,
                               const am_lrnet_role_common_cfg_t *p_role_cfg);

/*
 * lrnet 字符串转数
 * cp:    待转字符串
 * endp:  返回结束字符（NULL）
 * base：  16进制（16）/10进制（10）
 */
unsigned int lrnet_simple_strtoul(const char *cp,char **endp,unsigned int base);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __DEMO_LRNET_COMMON_H */
