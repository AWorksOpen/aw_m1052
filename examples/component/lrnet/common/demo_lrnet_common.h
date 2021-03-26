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

/* ����ID �������ֲ�ͬ��LoRaNet���� */
#define NETWORK_ID                0x11

/** group fetch����ɼ��������ݵĴ�С */
#define _GFETCH_ACK_DATA_SIZE               8

/* ��ȡ�¶�����, �յ�������ն����������¶��ַ��� */
#define CMD_GET_TEMPERATURE "Get Temperature"

/* ͻ���ϱ�����յ�������ն�����ִ��ͻ���ϱ� */
#define CMD_DO_BURST_REPORT "Do Burst Report"

/* ֪ͨ���� */
#define DATA_INFORMATION     "Information"

/* �쳣���� */
#define DATA_EXCEPTION      "Expection"

/* lrnet network config */
extern const am_lrnet_net_cfg_t g_lrnet_network_cfg;

/*
 * lrnet ״̬�л��ص�
 */
void lrnet_common_rf_state_changed_cb (enum am_lrnet_rf_state state);

/*
 * lrnet ��ʼ��������ӡ
 */
void lrnet_debug_startup_info (const char                       *p_dev_name,
                               const am_lrnet_net_cfg_t         *p_net_cfg,
                               const am_lrnet_role_common_cfg_t *p_role_cfg);

/*
 * lrnet �ַ���ת��
 * cp:    ��ת�ַ���
 * endp:  ���ؽ����ַ���NULL��
 * base��  16���ƣ�16��/10���ƣ�10��
 */
unsigned int lrnet_simple_strtoul(const char *cp,char **endp,unsigned int base);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __DEMO_LRNET_COMMON_H */
