/*******************************************************************************
*                                 AWorks
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
 * \brief socket通信演示例程(通过多线程实现一个服务器与多个客户端通信)。
 *
 * - 本例程用socket建立了一个TCP 服务器。
 *
 * - 说明：
 *   1. 需要在 aw_prj_params.h 里使能至少一个网卡，例如：
 *        1) 以太网
 *        2) Wi-Fi
 *
 * - 操作步骤(以太网)：
 *   1. 本例程配合TCP上位机软件使用:
 *      - 准备一块开发板下载该例程；
 *      - 将开发板网口0与电脑网口连接，保证电脑 IP 地址与开发板的 IP 处于同一网段下；
 *      - 程序运行后，打开TCP上位机软件，创建客户端（最多5个）。
 *
 * - 操作步骤(Wi-Fi)：
 *   1. 本例程也可以配合TCP上位机软件使用:
 *      - 准备一块开发板下载该例程；
 *      - 程序启动后通过，执行 "wifi join wifiname wifipassword" shell命令加入无线网络。
 *      - 程序运行后，打开TCP上位机软件，创建客户端（最多5个）。
 *
 * - 实验现象：
 *   1. 串口打印出客户端和服务器收发数据的调试信息。
 *   2. 服务器最多可以对应5个客户端，关系图如下:
 *                      服务器<—— —— ——>客户端1
 *                        .
 *                       /|\
 *                        |
 *                       \|/
 *                        .
 *                       客户端2
 * - 备注：
 *   1. lwIP 配置文件 lwipopts.h 在 aworks_sdk\include\config 目录下 ，若配置不合适，可以将
 *      lwIP 源码放进工程中，并修改该配置文件；
 *   2. 以太网配置文件为 awbl_hwconf_xxx_enet.h（具体文件名和平台相关）；
 *   3. Wi-Fi 配置文件为 awbl_hwconf_cyw43362_wifi_spi.h 以及  awbl_hwconf_cyw43362.c（共用配置）。
 *
 * \par 源代码
 * \snippet demo_tcp_server_thread.c src_tcp_server_thread
 *
 * \internal
 * \par Modification history
 * - 1.10 18-05-07  phd, 移除读写IP地址等无关操作.
 * - 1.00 17-09-11  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_tcp_server_thread TCP服务器(一对多)
 * \copydoc demo_tcp_server_thread.c
 */

/** [src_tcp_server_thread] */

#include "aworks.h"
#include "aw_sockets.h"
#include "aw_vdebug.h"
#include "aw_defer.h"
#include "string.h"

aw_local struct aw_defer        __g_defer;
aw_local struct aw_defer_djob   __g_djob;

aw_local void __data_handle (void * p_arg)
{
    int     client_sockfd = *(int *)p_arg;
    char    ch_buf[64];
    int     rcv_len;

    while (1) {
        memset(ch_buf, 0, 64);
        rcv_len = recv(client_sockfd, &ch_buf[0], 64, 0);
        if (rcv_len <= 0) {
            break;
        }
        aw_kprintf("from fd %d data:%s\r\n", client_sockfd, ch_buf);
        send(client_sockfd, &ch_buf[0], 64, 0);
    }
    closesocket(client_sockfd); /* 关闭此连接 */
    aw_kprintf("fd %d client leave.\r\n", client_sockfd);
}

void demo_tcp_server_thread (void)
{
    int         server_sockfd, client_sockfd;
    socklen_t   server_len, client_len;

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int ret;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family       = AF_INET;
    server_address.sin_addr.s_addr  = htonl(INADDR_ANY);
    server_address.sin_port         = htons(1234);/*服务器端口号为1234*/
    server_len = sizeof(server_address);

    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    listen(server_sockfd, 5);

    /* 初始化一个延迟作业 */
    aw_defer_init(&__g_defer);

    /* 启动延迟作业 */
    AW_DEFER_START(&__g_defer, 5, 0, 4096, "defer");

    while (1) {
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
        if(client_sockfd > 0){

            /* 初始化一个工作节点 */
            aw_defer_job_init(&__g_djob, __data_handle, (void *)&client_sockfd);
            ret = aw_defer_commit(&__g_defer, &__g_djob);
            if (ret == AW_OK) {
                AW_INFOF(("client %s %d connected.\r\n",
                           inet_ntoa(client_address.sin_addr),
                           htons(client_address.sin_port)));
            } else {
                closesocket(client_sockfd);
            }
        }
    }

    return ;
}

/** [src_tcp_server_thread] */

/* end of file */
