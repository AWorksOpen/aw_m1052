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
 * \brief socket通信演示例程(一对一)。
 *
 * - 本例程用socket建立了一个TCP 服务器(阻塞模式)。
 *
 * - 说明：
 *   1. 需要在 aw_prj_params.h 里使能至少一个网卡，例如：
 *        1) 以太网
 *        2) Wi-Fi
 *
 * - 操作步骤(以太网)：
 *   1. 本例程可以配合客户端程序使用：
 *      - 准备两块开发板，首先一块开发板下载本例程；然后打开以太网的配置文件awbl_hwconf_xxx_enet.h，
 *        修改网卡0的IP地址(两个开发板的IP地址必须在同一网段)，下载demo_tcp_client.c例程到另一块开发板；
 *      - 将两块开发板的网口0通过网线连接。
 *   2. 本例程也可以配合TCP上位机软件使用:
 *      - 准备一块开发板下载该例程；
 *      - 将开发板网口0与电脑网口连接，保证电脑 IP 地址与设置的 IP 处于同一网段下；
 *      - 打开TCP上位机软件，创建客户端，建立连接，发送数据。
 *
 * - 操作步骤(Wi-Fi)：
 *   1. 本例程可以配合客户端程序使用：
 *      - 在 aw_prj_params.h 里使能 AW_NET_TOOLS、AW_COM_SHELL_SERIAL 宏
 *      - 准备两块开发板，首先一块下载本例程，另一块下载 demo_tcp_client.c 例程做为客户端。
 *      - 程序启动后通过，两块开发板分别执行 "wifi join wifiname wifipassword" shell命令，加入同一无线网络。
 *   2. 本例程也可以配合TCP上位机软件使用:
 *      - 准备一块开发板下载该例程；
 *      - 程序启动后通过，执行 "wifi join wifiname wifipassword" shell命令，加入同一无线网络。
 *      - 打开TCP上位机软件，创建客户端，建立连接，发送数据。
 *
 * - 实验现象：
 *   1. 串口打印出客户端和服务器收发数据的调试信息。
 *
 * - 备注：
 *   1. lwIP 配置文件 lwipopts.h 在 aworks_sdk\include\config 目录下 ，若配置不合适，可以将
 *      lwIP 源码放进工程中，并修改该配置文件；
 *   2. 以太网配置文件为 awbl_hwconf_xxx_enet.h（具体文件名和平台相关）；
 *   3. Wi-Fi 配置文件为 awbl_hwconf_cyw43362_wifi_spi.h 以及  awbl_hwconf_cyw43362.c（共用配置）。
 *   4. 服务器可以对应多个客户端，本例程实现的是一对一的通信模式，用户如需实现一对多的通信模式可创建多线程自行定义，关系图如下:
 *                      服务器<—— —— ——>客户端1
 *                        .
 *                       /|\
 *                        |
 *                       \|/
 *                        .
 *                       客户端2
 * \par 源代码
 * \snippet demo_server_block.c src_demo_server_block
 *
 * \internal
 * \par Modification history
 * - 1.10 18-05-07  phd, 移除读写IP地址等无关操作.
 * - 1.00 17-09-11  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_server_block TCP服务器(阻塞模式)
 * \copydoc demo_server_block.c
 */

/** [src_demo_server_block] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_task.h"

#include "aw_net.h"
#include "aw_sockets.h"

#include <string.h>

#define LOCAL_SERVER_PORT           4000    /* 本地服务器端口 */

/* 客户端访问的服务器IP地址 */

/**
 * \brief net 示例程序入口
 * \return 无
 */
void demo_tcp_server_block_entry (void)
{
    struct sockaddr_in  server_addr, client_addr;
    int                 server_sock, client_sock;
    int                 rcv_len;
    char                net_buf[1500];
    socklen_t           len;

    memset(net_buf, '\0', 1500);
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    /* 创建socket套接字，使用TCP连接 */
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > server_sock) {
        AW_INFOF(("TCP server socket failed!" ENDL));
        return;
    }

    /* 设置服务器的端口等属性 */
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(LOCAL_SERVER_PORT); /* 设置服务器的端口 */
    server_addr.sin_addr.s_addr = 0;                        /* 在所有接口上监听 */
    server_addr.sin_len         = sizeof(server_addr);
    memset(&(server_addr.sin_zero), 8, sizeof(server_addr.sin_zero));

    /* 绑定本地接口 */
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        AW_INFOF(("TCP server bind failed!" ENDL));
        closesocket(server_sock);
        return;
    }

    /* 进入监听模式 */
    listen(server_sock, 1);
    AW_INFOF(("TCP server: listen on %s:%d" ENDL,
              inet_ntoa(server_addr.sin_addr),
              htons(server_addr.sin_port)));

    len = sizeof(struct sockaddr);

    for (;;) {

        /* 阻塞等待客户端连接 */
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &len);
        if (client_sock > 0) {
            AW_INFOF(("TCP server: client %s:%d connected." ENDL,
                      inet_ntoa(client_addr.sin_addr),
                      htons(client_addr.sin_port)));

            for (;;) {
                send(client_sock, "hello,i'm tcp server.", 21, 0);          /* 发送数据 */
                rcv_len = recv(client_sock, &net_buf, sizeof(net_buf), 0);  /* 接收数据，阻塞，直到收到数据 */
                if (rcv_len <= 0) {
                    break;
                }
                aw_kprintf("recv:%s\r\n", net_buf);     /* 打印接收得到的数据 */
                memset(net_buf, '\0', 1500);            /* 清空缓存区 */
            }

            closesocket(client_sock);   /* 关闭此连接 */
            AW_INFOF(("TCP server: client %s:%d leave." ENDL,
                      inet_ntoa(client_addr.sin_addr),
                      htons(client_addr.sin_port)));

            aw_mdelay(5);
        }
    }
}

/** [src_demo_server_block] */

/* end of file */
