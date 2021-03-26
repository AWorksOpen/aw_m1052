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
 * \brief web服务器例程(lwip接口实现)
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 对应平台的网卡宏；
 *   2. 用网线将板子和电脑连接起来；
 *   3. 用户需打开电脑的网络共享中心查看您的电脑使用的是静态IP地址还是动态IP地址：
 *      如果采用动态方式，请 关闭 IP_ADDR_STATIC 宏定义，并跳过此步骤；
 *      如果采用静态方式，请通过以下任意一种方式，保证电脑与板子在同一个网段：
 *      (1) 修改电脑的ip地址、网关地址，保证和开发板同一网段：
 *      (2) 修改 LOCAL_IP_ADDR0 、LOCAL_GATAWAY_ADDR0 宏定义保证开发板和电脑在同一个网段
 *   4. 在电脑端打开任意浏览器，输入http://192.168.12.200,浏览器显示一个简单的网页；
 *   5. 如果显示失败，请在串口打印界面执行shell命令：
 *      - "ip addr"， 查看开发板网卡0 ip地址是否设置成功；
 *      - "ping 电脑ip地址"，查看电脑和开发板的网络是否连接成功；
 *      打开电脑cmd工具执行"ping 开发板ip地址",查看电脑和开发板相互之间网络是否连接成功；
 *      如果以上步骤全都执行成功，则可建立连接；否则请查看修改失败原因。
 *
 * - 实验现象：
 *   1. 在电脑端打开任意浏览器，输入http://192.168.12.200,浏览器显示一个简单的网页。
 *
 * \par 源代码
 * \snippet demo_webserver_lwip.c src_webserver_lwip
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_webserver_lwip web服务器
 * \copydoc demo_webserver_lwip.c
 */

/** [src_enet_tcp_server] */
#include "apollo.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "aw_vdebug.h"
#include <string.h>
#include "aw_netif.h"
#include "aw_delay.h"
#include "lwip/api.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/ip_addr.h"
#include "aw_task.h"
#include "index_html.h"


/** /note
 * 用户需打开电脑的网络共享中心查看您的电脑使用的是静态IP地址还是动态IP地址，
 * 如果您的电脑使用的是静态IP地址请开启此宏，开发板将使用静态方式分配网卡0 IP地址
 * 如果您的电脑使用的是动态IP地址请屏蔽此宏，开发板将使用动态方式获得网卡0 IP地址
 */
#define IP_ADDR_STATIC

#ifdef IP_ADDR_STATIC
#define LOCAL_IP_ADDR0                 "192.168.12.200"  /* 开发板 网卡0 IP 地址，用户需保证开发板和电脑的IP地址在同一网段   */
#define LOCAL_NET_MASK0                "255.255.255.0"   /* 开发板 网卡0 子网掩码   */
#define LOCAL_GATAWAY_ADDR0            "192.168.12.254"  /* 开发板 网卡0 网关地址 ，用户需保证开发板和电脑的网关地址在同一网段 */
#endif

#define LOCAL_NIC0_NAME                "eth0"            /* 开发板网卡0 名字(该名称在网卡0的配置文件中定义) */
#define LOCAL_PORT                     4000              /* 板子端口         */

static aw_err_t __config_eth0_ip (void)
{
    struct in_addr  addr;
    aw_netif_t     *p_netif;
    aw_err_t        ret;

    /*打开网卡0设备*/
    p_netif = aw_netif_dev_open(LOCAL_NIC0_NAME);
    if (p_netif == NULL) {
        aw_kprintf("aw_netif_dev_open fail\r\n");
        return AW_ERROR;
    }

#ifdef IP_ADDR_STATIC

    /* 关闭 DHCPC 客户端，网卡0使用静态IP地址 */
    ret =  aw_netif_dhcpc_stop(p_netif);
    if (ret != AW_OK) {
        aw_kprintf("awbl_nic_dhcpc_stop fail, error code:%d\r\n", ret);
    }

    /* 设置网卡0 IP 地址  */
    inet_aton(LOCAL_IP_ADDR0, &addr);
    ret = aw_netif_ipv4_ip_set(p_netif, &addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_ip_set fail, error code:%d\r\n", ret);
        return ret;
    }

    /* 设置网卡0 子网掩码  */
    inet_aton(LOCAL_NET_MASK0, &addr);
    ret = aw_netif_ipv4_netmask_set(p_netif, &addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_netmask_set fail, error code:%d\r\n", ret);
        return ret;
    }

    /* 设置网卡0 网关  */
    inet_aton(LOCAL_GATAWAY_ADDR0, &addr);
    aw_netif_ipv4_gw_set(p_netif, &addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_gw_set fail, error code:%d\r\n", ret);
        return ret;
    }

#else

    /* 开启DHCPC 客户端 ，将使用动态方式配置开发板网卡0的IP地址*/
    ret = aw_netif_dhcpc_start(p_netif);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_dhcpc_start fail, error code:%d\r\n", ret);
        return ret;
    }

#endif

    return AW_OK;
}

aw_local void* __webserver_task(void *parg)
{
    struct netconn *__pstConn, *__pstNewConn;
    struct netbuf  *buf;
    aw_err_t        ret;
    uint8_t        *data;
    uint16_t        length;

    __pstConn = netconn_new(NETCONN_TCP);   /* 建立TCP连接 */
    netconn_bind(__pstConn, NULL, 80);      /* 80端口 */
    netconn_listen(__pstConn);              /* 开始监听 */

    while (1) {

        aw_kprintf("web server waiting for connection\n");

        /* 接受外部来的连接 */
        ret = netconn_accept(__pstConn, &__pstNewConn);

        if (__pstNewConn == NULL) {
            continue;
        }

        if (ret == 0) {

            /* 接收请求数据 */
            ret = netconn_recv(__pstNewConn, &buf);
            if (buf != NULL) {

                /* get data */
                netbuf_data(buf, (void**)&data, &length);

                aw_kprintf("buf:%s\r\n", data);

                /* release buffer */
                netbuf_delete(buf);
            }

            /* 向客户端发送一个网页数据 */
            netconn_write(__pstNewConn, html_data, sizeof(html_data), NETCONN_COPY);
            netconn_close(__pstNewConn);
        }
    }

    return 0;
}
/**
 * \brief tcp 服务端的入口函数
 */
void demo_web_server_entry (void)
{
    /* 配置开发板的网卡0 IP地址 */
    if (__config_eth0_ip() != AW_OK) {
        return ;
    }

    /* 定义任务实体 webserver_task */
    AW_TASK_DECL_STATIC(webserver_task, 1024);

    /* 初始化任务 */
    AW_TASK_INIT(webserver_task,            /* 任务实体 */
                 "webserver_task",          /* 任务名字 */
                 5,                         /* 任务优先级 */
                 1024,                      /* 任务堆栈大小 */
                 __webserver_task,    /* 任务入口函数 */
                 (void*)NULL);              /* 任务入口参数 */

    /* 启动任务 */
    AW_TASK_STARTUP(webserver_task);

}

/** [src_webserver_lwip] */

/* end of file */
