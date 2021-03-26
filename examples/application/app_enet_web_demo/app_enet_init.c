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
 * \brief eth0初始化例程
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 对应平台的网卡宏；
 *   2. 用网线将板子和电脑连接起来；
 *   3. 用户需打开电脑的网络共享中心查看您的电脑使用的是静态IP地址还是动态IP地址：
 *      如果采用动态方式，请 打开 IP_OPEN_DHCPS 宏定义，并跳过此步骤；
 *      如果采用静态方式，请通过以下任意一种方式，保证电脑与板子在同一个网段：
 *      (1) 修改电脑的ip地址、网关地址，保证和开发板同一网段：
 *      (2) 修改 LOCAL_IP_ADDR0 、LOCAL_GATAWAY_ADDR0 宏定义保证开发板和电脑在同一个网段
 *   4. 在串口打印界面执行shell命令：
 *      - "ip addr"， 查看开发板网卡0 ip地址是否设置成功；
 *      - "ping 电脑ip地址"，查看电脑和开发板相互之间网络连接成功；
 *      打开电脑cmd工具执行"ping 开发板ip地址",查看电脑和开发板相互之间网络连接成功；
 *      如果以上步骤全都执行成功，则可建立连接；否则请查看修改失败原因。
 *
 * - 实验现象：
 *   1. 能查看ip地址，能ping通。
 *
 * \par 源代码
 * \snippet demo_enet_init.c src_enet_init
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  sdq, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_enet_init
 * \copydoc demo_enet_init.c
 */

/** [src_enet_init] */
#include "apollo.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "aw_vdebug.h"
#include <string.h>
#include "aw_netif.h"
#include "aw_delay.h"
#include "dhcp_server.h"

/** /note
 * 用户需打开电脑的网络共享中心查看您的电脑使用的是静态IP地址还是动态IP地址，
 * 如果您的电脑使用的是静态IP地址请开启此宏，开发板将使用静态方式分配网卡0 IP地址
 * 如果您的电脑使用的是动态IP地址请屏蔽此宏，开发板将使用动态方式获得网卡0 IP地址
 */
//#define IP_OPEN_DHCPS

#define LOCAL_IP_ADDR0                 "172.16.18.201"  /* 开发板 网卡0 IP 地址，用户需保证开发板和电脑的IP地址在同一网段   */
#define LOCAL_NET_MASK0                "255.255.255.0"   /* 开发板 网卡0 子网掩码   */
#define LOCAL_GATAWAY_ADDR0            "172.16.18.254"  /* 开发板 网卡0 网关地址 ，用户需保证开发板和电脑的网关地址在同一网段 */

#define LOCAL_NIC0_NAME                "eth0"            /* 开发板网卡0 名字(该名称在网卡0的配置文件中定义) */
#define LOCAL_PORT                     80                /* 板子端口         */


aw_local struct dhcp_server *__eth0_dhcp_server (void)
{
    aw_local struct dhcp_server dhcp_server;
    return &dhcp_server;
}


static aw_err_t __config_eth0_ip(void){
    struct in_addr addr;
    aw_netif_t * p_netif;
    aw_err_t ret;

    /*打开网卡0设备*/
    p_netif = aw_netif_dev_open (LOCAL_NIC0_NAME);
    if(p_netif == NULL){
        aw_kprintf("aw_netif_dev_open fail\r\n");
        return AW_ERROR;
    }

    /* 设置网卡0 IP 地址  */
    inet_aton(LOCAL_IP_ADDR0, &addr);
    ret = aw_netif_ipv4_ip_set(p_netif,&addr);
    if(ret != AW_OK){
        aw_kprintf("aw_netif_ipv4_ip_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* 设置网卡0 子网掩码  */
    inet_aton(LOCAL_NET_MASK0, &addr);
    ret = aw_netif_ipv4_netmask_set(p_netif,&addr);
    if(ret != AW_OK){
        aw_kprintf("aw_netif_ipv4_netmask_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* 设置网卡0 网关  */
    inet_aton(LOCAL_GATAWAY_ADDR0, &addr);
    aw_netif_ipv4_gw_set(p_netif,&addr);
    if(ret != AW_OK){
        aw_kprintf("aw_netif_ipv4_gw_set fail,error code:%d\r\n",ret);
        return ret;
    }

    dhcps_bind_netif(__eth0_dhcp_server(), LOCAL_NIC0_NAME);

#ifdef  IP_OPEN_DHCPS
    /* 开启 DHCPS 服务端，开发板将动态为客户端分配IP地址*/
    dhcps_start(__eth0_dhcp_server());
#else
    /* 关闭 DHCPS 服务端，客户端需配置静态IP地址 */
    dhcps_stop(__eth0_dhcp_server());
#endif

    return AW_OK;
}

/**
 * \brief eth0 配置初始化函数
 */
void app_enet_init (void)
{
    /* 配置开发板的网卡0 IP地址*/
    if (__config_eth0_ip() != AW_OK){
        return ;
    }
}
/** [src_enet_init] */

/* end of file */
