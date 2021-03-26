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
 * \brief 网络配置工具
 *
 * - DNS
 *   0. "dns --help" 查看使用说明和示例
 *   1. "dns" 查看系统 DNS 服务器配置
 *   2. "dns set 0 addr 10.1.1.1" 设置主 DNS 服务器地址为 10.1.1.1
 *   3. "dns set 1 addr 172.16.1.1" 设置备份 DNS 服务器地址为 172.16.1.1
 *
 * - 主机名
 *   0. "hostname --help" 查看使用说明和示例
 *   1. "hostname" 查看系统主机名
 *   2. "hostname set AWorksPlatform" 设置主机名为 AWorksPlatform
 *
 * - 网卡配置(IP地址, etc)
 *   0. "ip --help" 查看使用说明和示例
 *   1. "ip link" 查看所有接口的链路状态
 *   2. "ip addr" 查看所有接口的地址
 *   3. "ip link show dev eth0" 查看所有接口的地址 eth0 的链路状态
 *   4. "ip addr show dev eth0" 查看所有接口的地址 eth0 的地址
 *   5. "ip addr add 192.168.1.2/24 192.168.1.1 dev eth0" 设置 eth0 使用静态地址
 *   6. "ip addr add auto dev eth0" 配置 eth0 使用 DHCP 获取动态地址
 *   7. "ip link set down dev eth0" 禁用 eth0(之前启用的 DHCP 会被同时关闭)
 *   8. "ip link set up dev eth0" 启用 eth0(之前启用的 DHCP 不会自动打开)
 *
 * - PING
 *   0. "ping --help" 查看使用说明和示例
 *   1. "ping 10.1.1.1" ping IP地址
 *   2. "ping www.zlg.cn" ping 域名(需要配置有效的DNS服务器)
 *
 * - Wi-Fi
 *   00. "help" 查看使用说明和示例
 *   01. "scan" 可以查看附近的可见热点
 *   02. "scan passive" 可以查看附近的所有热点
 *   03. "join ZYWIFI meiyoumima" 加入名称为 ZYWIFI, 密码为 meiyoumima 的热点
 *   04. "getapinfo" 获取已加入的热点的信息（信号强度等）
 *   05. "leave" 离开已加入的热点
 *   06. "startap AWWIFI wpa2_aes meiyoumima 6" 在 6 号 频道创建名称为 AWWIFI,
 *        密码为 meiyoumima 的热点
 *   07. "maxassoc" 获取最多可加入本地创建的热点的客户端的数量
 *   08. "getstalist" 获取已加入本地创建的热点的客户端信息
 *   09. "stopap" 关闭已创建的的热点
 *   10. "startap AWWIFI wpa2_aes meiyoumima 6 10.0.0.1/24 10.0.0.1" 创建热点并
 *       指定 IP 地址
 *   11. "getlinkstatus" 获取 Wi-Fi 链路状态
 *   12. 执行 "pwr 0" 关闭 Wi-Fi 电源
 *   13. 执行 "pwr 1" 重新打开 Wi-Fi 电源
 *   14. 执行 "starthiddenap AWWIFI wpa2_aes meiyoumima 6" 在 6 号 频道创建
 *       名称为 AWWIFI, 密码为 meiyoumima 的隐藏热点
 *   15. 执行 "starthiddenap AWWIFI wpa2_aes meiyoumima 6 10.0.0.1/24 10.0.0.1"
 *       在 6 号 频道创建名称为 AWWIFI, 密码为 meiyoumima 的隐藏热点并指定 IP 地址
 *
 * \par 源代码
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
