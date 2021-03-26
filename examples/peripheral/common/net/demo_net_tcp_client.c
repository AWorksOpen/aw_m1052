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
 * \brief TCP�ͻ�����ʾ����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ��Ӧƽ̨��������
 *      - ����Э��ջ
 *   2. �����߽����Ӻ͵�������������
 *   3. �û���򿪵��Ե����繲�����ģ��鿴���ĵ���ʹ�õ��Ǿ�̬IP��ַ���Ƕ�̬IP��ַ��
 *      ������ö�̬��ʽ����� IP_OPEN_DHCPC �궨�壬�������˲��裻
 *      ������þ�̬��ʽ����ͨ����������һ�ַ�ʽ����֤�����������ͬһ�����Σ�
 *      (1) �޸ĵ��Ե�ip��ַ�����ص�ַ����֤�Ϳ�����ͬһ����
 *      (2) �޸� LOCAL_IP_ADDR0 ��LOCAL_GATAWAY_ADDR0 �궨�壬��֤������͵�����ͬһ������
 *   4. �򿪵��Զ�TCP&UDP���Թ��ߣ�����TCP���͵ķ�������ָ��IP��ַΪ���Ե�IP��ַ���˿�Ϊ5000��Ȼ��������������
 *   5. ������Զ˺Ϳ����彨������ʧ�ܣ����ڴ��ڴ�ӡ����ִ��shell���
 *      - "ip addr"�� �鿴����������0 ip��ַ�Ƿ����óɹ���
 *      - "ping ����ip��ַ"���鿴���ԺͿ������໥֮�������Ƿ����ӳɹ���
 *      �򿪵���cmd����ִ��"ping ������ip��ַ"���鿴���ԺͿ������໥֮���������ӳɹ���
 *      ������ϲ���ȫ��ִ�гɹ�����ɽ������ӣ��������������ԭ�򣬽����޸ġ�
 *
 * - ʵ������
 *   1. TCP&UDP���Թ������ӳɹ�������λ���Ϸ������ݣ����ڴ�ӡ�����յ������ݣ�������λ��������յ�������ط�����Ϣ��
 *
 * \par Դ����
 * \snippet demo_net_tcp_client.c src_net_tcp_client
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  sdq, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_net_tcp_client TCP�ͻ���
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
 * �û���򿪵��Ե����繲�����Ĳ鿴���ĵ���ʹ�õ��Ǿ�̬IP��ַ���Ƕ�̬IP��ַ��
 * ������ĵ���ʹ�õ��Ǿ�̬IP��ַ�����δ˺꣬�����彫ʹ�þ�̬��ʽ��������0 IP��ַ
 * ������ĵ���ʹ�õ��Ƕ�̬IP��ַ�뿪���˺꣬�����彫ʹ�ö�̬��ʽ�������0 IP��ַ
 */
//#define IP_OPEN_DHCPC

#ifndef IP_OPEN_DHCPC
#define LOCAL_IP_ADDR0                 "192.168.1.10"   /* ������ ����0 IP ��ַ���û��豣֤������͵��Ե�IP��ַ��ͬһ���� */
#define LOCAL_NET_MASK0                "255.255.255.0"  /* ������ ����0 �������� */
#define LOCAL_GATAWAY_ADDR0            "192.168.1.10"   /* ������ ����0 ���ص�ַ ���û��豣֤������͵��Ե����ص�ַ��ͬһ���� */
#endif

#define REMOTE_IP_ADDR                 "192.168.1.34"   /* Զ�˷�����(����)IP��ַ���û�����ղ������裬���ݾ�������޸� */
#define REMOT_PORT                     5000             /* Զ�˷������˿� */

#define LOCAL_NIC0_NAME                DE_ENET_NAME     /* ����������0 ����(������������0�������ļ��ж���) */


/**
 * \brief ��������
 */
aw_local aw_err_t __config_eth0_ip(void)
{
    struct in_addr addr;
    aw_netif_t * p_netif;
    aw_err_t ret;
    aw_bool_t is_on;

    /* ������0�豸 */
    p_netif = aw_netif_dev_open (LOCAL_NIC0_NAME);
    if (p_netif == NULL) {
        aw_kprintf("aw_netif_dev_open fail\r\n");
        return AW_ERROR;
    }

#ifndef IP_OPEN_DHCPC

    /* ��������0 IP ��ַ */
    inet_aton(LOCAL_IP_ADDR0, &addr);
    ret = aw_netif_ipv4_ip_set(p_netif,&addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_ip_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* ��������0 �������� */
    inet_aton(LOCAL_NET_MASK0, &addr);
    ret = aw_netif_ipv4_netmask_set(p_netif,&addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_netmask_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* ��������0 ���� */
    inet_aton(LOCAL_GATAWAY_ADDR0, &addr);
    aw_netif_ipv4_gw_set(p_netif,&addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_gw_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* �ر� DHCPC �ͻ��ˣ�����0ʹ�þ�̬IP��ַ */
    aw_netif_dhcpc_is_on(p_netif, &is_on);
    if (is_on == AW_TRUE) {
        ret =  aw_netif_dhcpc_stop(p_netif);
        if (ret != AW_OK) {
            aw_kprintf("awbl_nic_dhcpc_stop fail,error code:%d\r\n",ret);
        }
    }

#else

    /* ����DHCPC �ͻ��� ����ʹ�ö�̬��ʽ���ÿ���������0��IP��ַ */
    ret = aw_netif_dhcpc_start(p_netif);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_dhcpc_start fail,error code:%d\r\n",ret);
        return ret;
    }

#endif

    return AW_OK;
}

/**
 * \brief tcp �ͻ��˵���ں���
 */
aw_local void* __task_handle (void *arg)
{
    int                 ret = 0;
    int                 socfd;
    char                msg[32] = { 0 };
    struct sockaddr_in  remote_addr;        /* Զ��(PC)��ַ */
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

    remote_addr.sin_family      = AF_INET;
    remote_addr.sin_port        = htons(REMOT_PORT);
    remote_addr.sin_addr.s_addr = inet_addr(REMOTE_IP_ADDR);

    while (1) {
        socfd = socket(AF_INET, SOCK_STREAM, 0);  /* TCPΪ��ʽ�׽��� */
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

                        /* �������Է����������� */
                        recv_size = recv(socfd, msg, sizeof(msg), 0);

                        if (recv_size > 0) {

                            /* �����յ�������ͨ�����Դ��ڴ�ӡ */
                            AW_INFOF(("msg: "));
                            aw_serial_write(AW_CFG_CONSOLE_COMID, msg, recv_size);
                            AW_INFOF(("\r\n"));

                            /* �������������ͻ�ȥ */
                            send(socfd, msg, recv_size, 0);

                            /* ��ջ����� */
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
