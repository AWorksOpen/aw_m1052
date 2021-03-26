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
 * - 本例程用socket建立了一个TCP 客户端。
 *
 * - 说明：
 *   1. 需要在 aw_prj_params.h 里使能至少一个网卡，例如：
 *        1) 以太网
 *        2) Wi-Fi
 *
 * - 操作步骤(以太网)：
 *   1. 本例程可以配合对应的服务端程序使用：
 *      - 准备两块开发板，首先一块开发板下载 demo_tcp_server_block.c 或者
 *        demo_tcp_server_nonblock.c 服务器程序；然后打开以太网的配置文件
 *        awbl_hwconf_xxx_enet.h 修改网卡的IP地址(两个开发板的IP地址必须在同一网段)，
 *        下载该例程到另一块开发板；
 *      - 将两块开发板的网口0通过网线连接。
 *   2. 本例程也可以配合TCP上位机软件使用:
 *      - 准备一块开发板下载该例程；
 *      - 将开发板网口0与电脑网口连接，保证电脑 IP 地址与开发板的 IP 处于同一网段下；
 *      - 打开TCP上位机软件，创建服务器，建立连接，发送数据。
 *
 * - 操作步骤(Wi-Fi)：
 *   1. 本例程可以配合对应的服务端程序使用：
 *      - 在 aw_prj_params.h 里使能 AW_NET_TOOLS、AW_COM_SHELL_SERIAL 宏
 *      - 准备两块开发板，首先一块下载 demo_tcp_server_block.c 或者 demo_tcp_server_nonblock.c
 *        服务器程序；一块下载本例程做为客户端。
 *      - 程序启动后通过，两块开发板分别执行 "wifi join wifiname wifipassword" shell命令加入无线网络。
 *   2. 本例程也可以配合TCP上位机软件使用:
 *      - 准备一块开发板下载该例程；
 *      - 程序启动后通过，执行 "wifi join wifiname wifipassword" shell命令加入无线网络。
 *      - 打开TCP上位机软件，创建服务器，建立连接，发送数据。
 *
 * - 实验现象：
 *   1. 串口打印出客户端和服务器收发数据的调试信息。
 *   2. 服务器可以对应多个客户端，本例程实现的是一对一的通信模式，用户如需实现一对多的通信模式可创建多线程自行定义，关系图如下:
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
 * \snippet demo_tcp_client.c src_demo_tcp_client
 *
 * \internal
 * \par Modification history
 * - 1.10 18-05-07  phd, 移除读写IP地址等无关操作.
 * - 1.00 17-09-11  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_tcp_client TCP客户端
 * \copydoc demo_tcp_client.c
 */

/** [src_demo_tcp_client] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_task.h"

#include "aw_net.h"
#include "aw_sockets.h"

#include <string.h>


/* 客户端访问的服务器IP地址 */
#define REMOTE_SERVER_ADDR          "192.168.12.237"      /* 对应服务器的 IP 地址,用户需要根据具体创建的服务端的IP地址修改 */
#define REMOTE_SERVER_PORT          4000                  /* 客户端访问服务器端口 */

/**
 * \brief net 示例程序入口
 * \return 无
 */
void demo_tcp_client_entry (void)
{
    struct sockaddr_in  server_addr;
    int                 rcv_len;
    int                 sock;
    char                net_buf[1500];

    memset(net_buf, '\0', 1500);

    /* 设置客户端访问的服务器IP地址、端口号 */
    inet_aton(REMOTE_SERVER_ADDR, &server_addr.sin_addr);
    server_addr.sin_family  = AF_INET;
    server_addr.sin_port    = htons(REMOTE_SERVER_PORT);
    server_addr.sin_len     = sizeof(server_addr);

    AW_INFOF(("TCP client: connecting..." ENDL));
    for (;;) {
        sock = socket(AF_INET, SOCK_STREAM, 0);     /* 创建socket套接字 */
        if (sock < 0) {
            AW_INFOF(("TCP server socket failed!" ENDL));
            return;
        }

        /* 连接服务器 */
        if (0 == connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
            AW_INFOF(("TCP client: connected." ENDL));
            for (;;) {
                send(sock, "hello, i'm tcp client.", 21, 0);        /* 向服务器发送数据 */
                rcv_len = recv(sock, net_buf, sizeof(net_buf), 0);  /* 接收服务器发送的数据 */
                if (rcv_len <= 0) {
                    AW_INFOF(("TCP client: disconnect. ret=%d, err=%d" ENDL, rcv_len, errno));
                    break;
                }
                aw_kprintf("recv:%s\r\n", net_buf);     /*打印数据*/
                memset(net_buf, '\0', 1500);            /*清空缓存区*/
            }
        }

        closesocket(sock); /* 关闭此连接 */
        aw_mdelay(1000);
    }
}

/** [src_demo_tcp_client] */

/* end of file */
