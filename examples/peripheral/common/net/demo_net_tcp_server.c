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
 * \brief TCP �������ʾ����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ��Ӧƽ̨��������
 *      - ����Э��ջ
 *   2. �����߽����Ӻ͵�������������
 *   3. �û���򿪵��Ե����繲�����ģ��鿴���ĵ���ʹ�õ��Ǿ�̬IP��ַ���Ƕ�̬IP��ַ��
 *      ������ö�̬��ʽ����� IP_OPEN_DHCPS �궨�壬�������˲��裻
 *      ������þ�̬��ʽ����ͨ����������һ�ַ�ʽ����֤�����������ͬһ�����Σ�
 *      (1) �޸ĵ��Ե�ip��ַ�����ص�ַ����֤�Ϳ�����ͬһ����
 *      (2) �޸� LOCAL_IP_ADDR0 ��LOCAL_GATAWAY_ADDR0 �궨�壬��֤������͵�����ͬһ������
 *   4. �򿪵��Զ�TCP&UDP���Թ��ߣ�����TCP���͵Ŀͻ��ˣ�Ŀ��IP��д����������0 ip��ַ��Ŀ��˿���4000��
 *   5. ������Զ˺Ϳ����彨������ʧ�ܣ����ڴ��ڴ�ӡ����ִ��shell���
 *      - "ip addr"�� �鿴����������0 ip��ַ�Ƿ����óɹ���
 *      - "ping ����ip��ַ"���鿴���ԺͿ������໥֮�������Ƿ����ӳɹ���
 *      �򿪵���cmd����ִ��"ping ������ip��ַ",�鿴���ԺͿ������໥֮���������ӳɹ���
 *      ������ϲ���ȫ��ִ�гɹ�����ɽ������ӣ��������������ԭ�򣬽����޸ġ�
 *
 * - ʵ������
 *   1. TCP&UDP���Թ������ӳɹ�������λ���˷������ݣ����ڴ�ӡ�����յ������ݣ�������λ��������յ�������ط�����Ϣ��
 *
 * \par Դ����
 * \snippet demo_net_tcp_server.c src_net_tcp_server
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  sdq, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_net_tcp_server TCP�����
 * \copydoc demo_net_tcp_server.c
 */

/** [src_net_tcp_server] */
#include "apollo.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "aw_vdebug.h"
#include <string.h>
#include "aw_netif.h"
#include "aw_delay.h"
#include "dhcp_server.h"
#include "aw_task.h"
#include "aw_serial.h"
#include "aw_demo_config.h"

/** /note
 * �û���򿪵��Ե����繲�����Ĳ鿴���ĵ���ʹ�õ��Ǿ�̬IP��ַ���Ƕ�̬IP��ַ��
 * ������ĵ���ʹ�õ��Ǿ�̬IP��ַ�����δ˺꣬�����彫ʹ�þ�̬��ʽ��������0 IP��ַ
 * ������ĵ���ʹ�õ��Ƕ�̬IP��ַ�뿪���˺꣬�����彫ʹ�ö�̬��ʽ�������0 IP��ַ
 */
//#define IP_OPEN_DHCPS

#define LOCAL_IP_ADDR0                 "192.168.1.10"   /* ������ ����0 IP ��ַ���û��豣֤������͵��Ե�IP��ַ��ͬһ���� */
#define LOCAL_NET_MASK0                "255.255.255.0"  /* ������ ����0 �������� */
#define LOCAL_GATAWAY_ADDR0            "192.168.1.254"  /* ������ ����0 ���ص�ַ���û��豣֤������͵��Ե����ص�ַ��ͬһ���� */

#define LOCAL_NIC0_NAME                DE_ENET_NAME     /* ����������0 ����(������������0�������ļ��ж���) */
#define LOCAL_PORT                     4000             /* ���Ӷ˿� */


aw_local struct dhcp_server *__eth0_dhcp_server (void)
{
    aw_local struct dhcp_server dhcp_server;
    return &dhcp_server;
}

/**
 * \brief ��������
 */
aw_local aw_err_t __config_eth0_ip (void)
{
    struct in_addr  addr;
    aw_netif_t     *p_netif;
    aw_err_t        ret;

    /* ������0�豸 */
    p_netif = aw_netif_dev_open(LOCAL_NIC0_NAME);
    if (p_netif == NULL) {
        aw_kprintf("aw_netif_dev_open fail\r\n");
        return AW_ERROR;
    }

    /* ��������0 IP ��ַ */
    inet_aton(LOCAL_IP_ADDR0, &addr);
    ret = aw_netif_ipv4_ip_set(p_netif, &addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_ip_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* ��������0 �������� */
    inet_aton(LOCAL_NET_MASK0, &addr);
    ret = aw_netif_ipv4_netmask_set(p_netif, &addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_netmask_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* ��������0 ���� */
    inet_aton(LOCAL_GATAWAY_ADDR0, &addr);
    aw_netif_ipv4_gw_set(p_netif, &addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_gw_set fail,error code:%d\r\n",ret);
        return ret;
    }

    dhcps_bind_netif(__eth0_dhcp_server(), LOCAL_NIC0_NAME);

#ifdef  IP_OPEN_DHCPS

    /* ���� DHCPS ����ˣ������彫��̬Ϊ�ͻ��˷���IP��ַ */
    dhcps_start(__eth0_dhcp_server());

#else

    /* �ر� DHCPS ����ˣ��ͻ��������þ�̬IP��ַ */
    dhcps_stop(__eth0_dhcp_server());

#endif

    return AW_OK;
}

/**
 * \brief tcp ����˵���ں���
 */
aw_local void* __task_handle (void *arg)
{
    int                 socfd, confd;
    int                 ret;
    char                msg[32] = { 0 };
    struct sockaddr_in  remote_addr;        /* Զ��(PC)��ַ */
    struct sockaddr_in  local_addr;         /* ������ַ */
    socklen_t           socklen = 0;
    int                 recv_size = 0;      /* ���յ����ַ����� */
    fd_set              rset;
    struct timeval tm = {
        .tv_sec  = 3,   /* select��ʱʱ������Ϊ3�� */
        .tv_usec = 0
    };

    /* ���ÿ����������0 IP��ַ */
    if (__config_eth0_ip() != AW_OK) {
        return 0;
    }

    /* ����socket�׽��� */
    socfd = socket(AF_INET, SOCK_STREAM, 0);    /* TCPΪ��ʽ�׽��� */
    if (socfd == -1) {
        AW_INFOF(("Socket creation failed\r\n"));
        return 0;
    }

    local_addr.sin_family       = AF_INET;
    local_addr.sin_port         = htons(LOCAL_PORT);
    local_addr.sin_addr.s_addr  = 0;

    /* ���׽��ֽӿںͶ˿ڰ� */
    if (-1 == bind(socfd, (struct sockaddr*)&local_addr, sizeof(local_addr))) {
        AW_INFOF(("Interface binding failed\r\n"));
    }

    /* �ȴ����� */
    if (-1 == listen(socfd, 1)) {
        AW_INFOF(("Listen failed\r\n"));
        goto err1;
    }

    while (1) {

        /* ��Ӧconnect���� */
        confd = accept(socfd, (struct sockaddr*)&remote_addr, &socklen);
        if (confd < 0) {
            AW_INFOF(("TCP Client connection failed\r\n"));
            goto err2;
        } else {
            AW_INFOF(("TCP Client is connected\r\n"));
        }

        while (1) {

            FD_ZERO(&rset);
            FD_SET(confd, &rset);
            ret = select(confd + 1, &rset, NULL, NULL, &tm);
            if (ret > 0) {
                if (FD_ISSET(confd, &rset)) {

                    /* �������Կͻ��˵����� */
                    recv_size = recv(confd, msg, sizeof(msg), 0);
                    if (recv_size > 0) {

                        /* �����յ�������ͨ�����Դ��ڴ�ӡ */
                        AW_INFOF(("msg: "));
                        aw_serial_write(AW_CFG_CONSOLE_COMID, msg, recv_size);
                        AW_INFOF(("\r\n"));

                        /* �������������ͻ�ȥ */
                        send(confd, msg, recv_size, 0);

                        /* ��ջ����� */
                        memset(msg, 0, sizeof(msg));
                    } else {
                        AW_INFOF(("TCP Client is closed\r\n"));
                        closesocket(confd);
                        break;
                    }
                }
            } else if (ret == -1) {
                closesocket(confd);
                break;
            }
        }
    }
err2:
    closesocket(confd);
err1:
    closesocket(socfd);

    return 0;
}

/******************************************************************************/
void demo_net_tcp_server_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Tcp server demo",
                         12,
                         4096,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Tcp server demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);
}

/** [src_net_tcp_server] */

/* end of file */
