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
 * \brief TCP客户端演示例程
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 对应平台的网卡宏
 *      - 网络协议栈
 *   2. 用网线将板子和电脑连接起来；
 *   3. 用户需打开电脑的网络共享中心，查看您的电脑使用的是静态IP地址还是动态IP地址：
 *      如果采用动态方式，请打开 IP_OPEN_DHCPC 宏定义，并跳过此步骤；
 *      如果采用静态方式，请通过以下任意一种方式，保证电脑与板子在同一个网段：
 *      (1) 修改电脑的ip地址、网关地址，保证和开发板同一网段
 *      (2) 修改 LOCAL_IP_ADDR0 、LOCAL_GATAWAY_ADDR0 宏定义，保证开发板和电脑在同一个网段
 *   4. 打开电脑端TCP&UDP调试工具，创建TCP类型的服务器，指定IP地址为电脑的IP地址，端口为5000，然后启动服务器；
 *   5. 如果电脑端和开发板建立连接失败，请在串口打印界面执行shell命令：
 *      - "ip addr"， 查看开发板网卡0 ip地址是否设置成功；
 *      - "ping 电脑ip地址"，查看电脑和开发板相互之间网络是否连接成功；
 *      打开电脑cmd工具执行"ping 开发板ip地址"，查看电脑和开发板相互之间网络连接成功；
 *      如果以上步骤全都执行成功，则可建立连接；否则请查找其他原因，进行修改。
 *
 * - 实验现象：
 *   1. TCP&UDP调试工具连接成功后，在上位机上发送数据，串口打印出接收到的数据，并且上位机软件将收到开发板回发的信息。
 *
 * \par 源代码
 * \snippet demo_net_tcp_client.c src_net_tcp_client
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  sdq, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_net_tcp_client TCP客户端
 * \copydoc demo_net_tcp_client.c
 */

/** [src_tcp] */
#include "apollo.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "aw_vdebug.h"
#include <string.h>
#include "aw_netif.h"
#include "aw_delay.h"
#include "aw_demo_config.h"
#include "aw_serial.h"

/** /note
 * 用户需打开电脑的网络共享中心查看您的电脑使用的是静态IP地址还是动态IP地址，
 * 如果您的电脑使用的是静态IP地址请屏蔽此宏，开发板将使用静态方式分配网卡0 IP地址
 * 如果您的电脑使用的是动态IP地址请开启此宏，开发板将使用动态方式获得网卡0 IP地址
 */
//#define IP_OPEN_DHCPC

#ifndef IP_OPEN_DHCPC
#define LOCAL_IP_ADDR0                 "192.168.1.10"   /* 开发板 网卡0 IP 地址，用户需保证开发板和电脑的IP地址在同一网段 */
#define LOCAL_NET_MASK0                "255.255.255.0"  /* 开发板 网卡0 子网掩码 */
#define LOCAL_GATAWAY_ADDR0            "192.168.1.10"   /* 开发板 网卡0 网关地址 ，用户需保证开发板和电脑的网关地址在同一网段 */
#endif

#define REMOTE_IP_ADDR                 "192.168.1.34"   /* 远端服务器(电脑)IP地址，用户需参照操作步骤，根据具体情况修改 */
#define REMOT_PORT                     5000             /* 远端服务器端口 */

#define LOCAL_NIC0_NAME                DE_ENET_NAME     /* 开发板网卡0 名字(该名称在网卡0的配置文件中定义) */


/**
 * \brief 配置网卡
 */
aw_local aw_err_t __config_eth0_ip(void)
{
    struct in_addr addr;
    aw_netif_t * p_netif;
    aw_err_t ret;
    aw_bool_t is_on;

    /* 打开网卡0设备 */
    p_netif = aw_netif_dev_open (LOCAL_NIC0_NAME);
    if (p_netif == NULL) {
        aw_kprintf("aw_netif_dev_open fail\r\n");
        return AW_ERROR;
    }

#ifndef IP_OPEN_DHCPC

    /* 设置网卡0 IP 地址 */
    inet_aton(LOCAL_IP_ADDR0, &addr);
    ret = aw_netif_ipv4_ip_set(p_netif,&addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_ip_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* 设置网卡0 子网掩码 */
    inet_aton(LOCAL_NET_MASK0, &addr);
    ret = aw_netif_ipv4_netmask_set(p_netif,&addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_netmask_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* 设置网卡0 网关 */
    inet_aton(LOCAL_GATAWAY_ADDR0, &addr);
    aw_netif_ipv4_gw_set(p_netif,&addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_gw_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* 关闭 DHCPC 客户端，网卡0使用静态IP地址 */
    aw_netif_dhcpc_is_on(p_netif, &is_on);
    if (is_on == AW_TRUE) {
        ret =  aw_netif_dhcpc_stop(p_netif);
        if (ret != AW_OK) {
            aw_kprintf("awbl_nic_dhcpc_stop fail,error code:%d\r\n",ret);
        }
    }

#else

    /* 开启DHCPC 客户端 ，将使用动态方式配置开发板网卡0的IP地址 */
    ret = aw_netif_dhcpc_start(p_netif);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_dhcpc_start fail,error code:%d\r\n",ret);
        return ret;
    }

#endif

    return AW_OK;
}

/**
 * \brief tcp 客户端的入口函数
 */
aw_local void* __task_handle (void *arg)
{
    int                 ret = 0;
    int                 socfd;
    char                msg[32] = { 0 };
    struct sockaddr_in  remote_addr;        /* 远端(PC)地址 */
    int                 recv_size = 0;      /* 接收到的字符个数 */
    fd_set              rset;

    struct timeval tm = {
        .tv_sec  = 3,   /* select超时时间设置为3秒 */
        .tv_usec = 0
    };

    /* 配置开发板的网卡0 IP地址 */
    if (__config_eth0_ip() != AW_OK) {
        return 0;
    }

    remote_addr.sin_family      = AF_INET;
    remote_addr.sin_port        = htons(REMOT_PORT);
    remote_addr.sin_addr.s_addr = inet_addr(REMOTE_IP_ADDR);

    while (1) {
        socfd = socket(AF_INET, SOCK_STREAM, 0);  /* TCP为流式套接字 */
        if (socfd < 0) {
            AW_INFOF(("Interface binding failed\r\n"));
            return 0;
        }

        if (0 == connect(socfd,
                         (struct sockaddr *)&remote_addr,
                         sizeof(remote_addr))) {
            AW_INFOF(("TCP client: connected.\r\n"));

            while (1) {
                FD_ZERO(&rset);
                FD_SET(socfd, &rset);
                ret = select(socfd + 1, &rset, NULL, NULL, &tm);
                if (ret > 0) {

                    if (FD_ISSET(socfd, &rset)) {

                        /* 接收来自服务器的数据 */
                        recv_size = recv(socfd, msg, sizeof(msg), 0);

                        if (recv_size > 0) {

                            /* 将接收到的数据通过调试串口打印 */
                            AW_INFOF(("msg: "));
                            aw_serial_write(AW_CFG_CONSOLE_COMID, msg, recv_size);
                            AW_INFOF(("\r\n"));

                            /* 将数据立即发送回去 */
                            send(socfd, msg, recv_size, 0);

                            /* 清空缓存区 */
                            memset(msg, 0, sizeof(msg));
                        } else {

                            AW_INFOF(("TCP Server is closed\r\n"));
                            goto close_socket;
                        }
                    }
                } else if (ret == -1) {
                    goto close_socket;
                }
            }
        }

close_socket:
    closesocket(socfd);
    aw_mdelay(1000);
    }

    return 0;
}

/******************************************************************************/
void demo_net_tcp_client_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Tcp client demo",
                         12,
                         4096,
                         __task_handle,
                         NULL);
    if (tsk == NULL) {
        aw_kprintf("Tcp client demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);
}

/** [src_net_tcp_client] */

/* end of file */
