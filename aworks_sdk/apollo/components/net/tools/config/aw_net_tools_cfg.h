/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief �������ù���
 *
 * - DNS
 *   0. "dns --help" �鿴ʹ��˵����ʾ��
 *   1. "dns" �鿴ϵͳ DNS ����������
 *   2. "dns set 0 addr 10.1.1.1" ������ DNS ��������ַΪ 10.1.1.1
 *   3. "dns set 1 addr 172.16.1.1" ���ñ��� DNS ��������ַΪ 172.16.1.1
 *
 * - ������
 *   0. "hostname --help" �鿴ʹ��˵����ʾ��
 *   1. "hostname" �鿴ϵͳ������
 *   2. "hostname set AWorksPlatform" ����������Ϊ AWorksPlatform
 *
 * - ��������(IP��ַ, etc)
 *   0. "ip --help" �鿴ʹ��˵����ʾ��
 *   1. "ip link" �鿴���нӿڵ���·״̬
 *   2. "ip addr" �鿴���нӿڵĵ�ַ
 *   3. "ip link show dev eth0" �鿴���нӿڵĵ�ַ eth0 ����·״̬
 *   4. "ip addr show dev eth0" �鿴���нӿڵĵ�ַ eth0 �ĵ�ַ
 *   5. "ip addr add 192.168.1.2/24 192.168.1.1 dev eth0" ���� eth0 ʹ�þ�̬��ַ
 *   6. "ip addr add auto dev eth0" ���� eth0 ʹ�� DHCP ��ȡ��̬��ַ
 *   7. "ip link set down dev eth0" ���� eth0(֮ǰ���õ� DHCP �ᱻͬʱ�ر�)
 *   8. "ip link set up dev eth0" ���� eth0(֮ǰ���õ� DHCP �����Զ���)
 *
 * - PING
 *   0. "ping --help" �鿴ʹ��˵����ʾ��
 *   1. "ping 10.1.1.1" ping IP��ַ
 *   2. "ping www.zlg.cn" ping ����(��Ҫ������Ч��DNS������)
 *
 * - Wi-Fi
 *   00. "help" �鿴ʹ��˵����ʾ��
 *   01. "scan" ���Բ鿴�����Ŀɼ��ȵ�
 *   02. "scan passive" ���Բ鿴�����������ȵ�
 *   03. "join ZYWIFI meiyoumima" ��������Ϊ ZYWIFI, ����Ϊ meiyoumima ���ȵ�
 *   04. "getapinfo" ��ȡ�Ѽ�����ȵ����Ϣ���ź�ǿ�ȵȣ�
 *   05. "leave" �뿪�Ѽ�����ȵ�
 *   06. "startap AWWIFI wpa2_aes meiyoumima 6" �� 6 �� Ƶ����������Ϊ AWWIFI,
 *        ����Ϊ meiyoumima ���ȵ�
 *   07. "maxassoc" ��ȡ���ɼ��뱾�ش������ȵ�Ŀͻ��˵�����
 *   08. "getstalist" ��ȡ�Ѽ��뱾�ش������ȵ�Ŀͻ�����Ϣ
 *   09. "stopap" �ر��Ѵ����ĵ��ȵ�
 *   10. "startap AWWIFI wpa2_aes meiyoumima 6 10.0.0.1/24 10.0.0.1" �����ȵ㲢
 *       ָ�� IP ��ַ
 *   11. "getlinkstatus" ��ȡ Wi-Fi ��·״̬
 *   12. ִ�� "pwr 0" �ر� Wi-Fi ��Դ
 *   13. ִ�� "pwr 1" ���´� Wi-Fi ��Դ
 *   14. ִ�� "starthiddenap AWWIFI wpa2_aes meiyoumima 6" �� 6 �� Ƶ������
 *       ����Ϊ AWWIFI, ����Ϊ meiyoumima �������ȵ�
 *   15. ִ�� "starthiddenap AWWIFI wpa2_aes meiyoumima 6 10.0.0.1/24 10.0.0.1"
 *       �� 6 �� Ƶ����������Ϊ AWWIFI, ����Ϊ meiyoumima �������ȵ㲢ָ�� IP ��ַ
 *
 * \par Դ����
 * \snippet aw_net_tools_cfg.h aw_net_tools_cfg
 *
 * \internal
 * \par Modification history
 * - 1.00 18-06-07  phd, first implementation.
 * \endinternal
 */

/**
 * \addtogroup aw_net_tools
 * \copydoc aw_net_tools_cfg.c
 */

/** [src_aw_net_tools_cfg] */

#include "aworks.h"
#include "aw_shell.h"
#include "aw_wifi.h"

#ifdef AXIO_AWORKS
#include "aw_net_tools_autoconf.h"
#endif

#define AW_CFG_NET_TOOLS_DNS
#define AW_CFG_NET_TOOLS_HOSTNAME
#define AW_CFG_NET_TOOLS_IP
#define AW_CFG_NET_TOOLS_IPERF
#define AW_CFG_NET_TOOLS_TCPPING

#ifdef AXIO_AWORKS
#ifdef  CONFIG_AW_NET_TOOL_PING
#define AW_CFG_NET_TOOLS_PING
#endif
#endif
#ifndef  AXIO_AWORKS
#define AW_CFG_NET_TOOLS_PING
#endif

#ifdef AXIO_AWORKS
#ifdef  CONFIG_AW_DRIVER_WIFI
#define AW_CFG_NET_TOOLS_WIFI
#endif
#endif
#ifndef  AXIO_AWORKS
#define AW_CFG_NET_TOOLS_WIFI
#endif

#ifndef BOARD_WIFI_STA_IF_NAME
#define BOARD_WIFI_STA_IF_NAME          "st"
#endif

#ifndef BOARD_WIFI_AP_IF_NAME
#define BOARD_WIFI_AP_IF_NAME           "ap"
#endif

#ifndef BOARD_WIFI_DEV_NAME
#define BOARD_WIFI_DEV_NAME             "wifi43362"
#endif

void aw_net_tools_register (void)
{
#ifdef AW_CFG_NET_TOOLS_DNS
    {
        aw_import void aw_net_tools_dns_register (void);
        aw_net_tools_dns_register();
    }
#endif

#ifdef AW_CFG_NET_TOOLS_HOSTNAME
    {
        aw_import void aw_net_tools_hostname_register (void);
        aw_net_tools_hostname_register();
    }
#endif

#ifdef AW_CFG_NET_TOOLS_IP
    {
        aw_import void aw_net_tools_ip_register (void);
        aw_net_tools_ip_register();
    }
#endif

#ifdef AW_CFG_NET_TOOLS_IPERF
    {
        aw_import void aw_net_tools_iperf_register (void);
        aw_net_tools_iperf_register();
    }
#endif

#ifdef AW_CFG_NET_TOOLS_PING
    {
        aw_import void aw_net_tools_ping_register (void);
        aw_net_tools_ping_register();
    }
#endif

#ifdef AW_CFG_NET_TOOLS_TCPPING
    {
        aw_import void aw_net_tools_tcpping_register (void);
        aw_net_tools_tcpping_register();
    }
#endif

#ifdef AW_CFG_NET_TOOLS_WIFI
    {
        aw_import void aw_net_tools_wifi_register (void);
        aw_net_tools_wifi_register();
    }
#endif
}

#ifdef AW_CFG_NET_TOOLS_WIFI
const char *aw_net_tools_wifi_get_dev_name (void)
{
    return BOARD_WIFI_DEV_NAME;
}

const char *aw_net_tools_wifi_get_staif_name (void)
{
    return BOARD_WIFI_STA_IF_NAME;
}

const char *aw_net_tools_wifi_get_apif_name (void)
{
    return BOARD_WIFI_AP_IF_NAME;
}
#endif

/** [src_aw_net_tools_cfg] */

/* end of file */
