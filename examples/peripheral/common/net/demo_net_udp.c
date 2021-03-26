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
 * \brief UDP��ʾ����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ��Ӧƽ̨��������
 *      - ����Э��ջ
 *   2. �����߽����Ӻ͵�������������
 *   3. �û���򿪵��Ե����繲�����ģ��鿴���ĵ���ʹ�õ��Ǿ�̬IP��ַ���Ƕ�̬IP��ַ��
 *      ������ö�̬��ʽ����ر� IP_ADDR_STATIC �궨�壬�������˲��裻
 *      ������þ�̬��ʽ����ͨ����������һ�ַ�ʽ����֤�����������ͬһ�����Σ�
 *      (1) �޸ĵ��Ե�ip��ַ�����ص�ַ����֤�Ϳ�����ͬһ����
 *      (2) �޸� LOCAL_IP_ADDR0 ��LOCAL_GATAWAY_ADDR0 �궨�壬��֤������͵�����ͬһ������
 *   4. �򿪵��Զ�TCP&UDP���Թ��ߣ�����UDP���͵����ӣ�Ŀ��IP��д����������0 ip��ַ��Ŀ��˿���4000��
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
 * \snippet demo_udp.c src_udp
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  sdq, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_udp UDP����
 * \copydoc demo_udp.c
 */

/** [src_udp] */
#include "apollo.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "aw_vdebug.h"
#include <string.h>
#include "aw_netif.h"
#include "aw_task.h"
#include "aw_serial.h"
#include "aw_demo_config.h"

#define __BOARD_PORT            4000                /* ���Ӷ˿� */

#define LOCAL_NIC0_NAME         DE_ENET_NAME        /* ����������0 ����(������������0�������ļ��ж���) */

/** /note
 * �û���򿪵��Ե����繲�����Ĳ鿴���ĵ���ʹ�õ��Ǿ�̬IP��ַ���Ƕ�̬IP��ַ��
 * ������ĵ���ʹ�õ��Ǿ�̬IP��ַ�뿪���˺꣬�����彫ʹ�þ�̬��ʽ��������0 IP��ַ
 * ������ĵ���ʹ�õ��Ƕ�̬IP��ַ�����δ˺꣬�����彫ʹ�ö�̬��ʽ�������0 IP��ַ
 */
#define IP_ADDR_STATIC

#ifdef IP_ADDR_STATIC
#define LOCAL_IP_ADDR0                 "192.168.1.10"   /* ������ ����0 IP ��ַ���û��豣֤������͵��Ե�IP��ַ��ͬһ���� */
#define LOCAL_NET_MASK0                "255.255.255.0"  /* ������ ����0 �������� */
#define LOCAL_GATAWAY_ADDR0            "192.168.1.254"  /* ������ ����0 ���ص�ַ���û��豣֤������͵��Ե����ص�ַ��ͬһ���� */
#endif


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

#ifdef IP_ADDR_STATIC

    /* �ر� DHCPC �ͻ��ˣ�����0ʹ�þ�̬IP��ַ */
    ret = aw_netif_dhcpc_stop(p_netif);
    if (ret != AW_OK) {
        aw_kprintf("awbl_nic_dhcpc_stop fail,error code:%d\r\n",ret);
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
 * \brief udp����ں���
 */
aw_local void* __task_handle (void *arg)
{
    int                 socfd;
    char                msg[32] = { 0 };
    struct sockaddr_in  remote_addr;                    /* Զ��(PC)��ַ */
    struct sockaddr_in  local_addr;                     /* ������ַ */
    int                 recv_size = 0;                  /* ���յ����ַ����� */
    socklen_t           socklen = sizeof(remote_addr);

    /* ���ÿ����������0 IP��ַ */
    if (__config_eth0_ip() != AW_OK) {
        return 0;
    }

    socfd = socket(AF_INET, SOCK_DGRAM, 0); /* UDPΪ���ݰ��׽��� */
    if (socfd == -1) {
        AW_INFOF(("fail to socket\r\n"));
        return 0;
    }

    local_addr.sin_family       = AF_INET;
    local_addr.sin_port         = htons(__BOARD_PORT);
    local_addr.sin_addr.s_addr  = 0;

    /* ���׽��ֽӿںͶ˿ڰ� */
    if (-1 == bind(socfd, (struct sockaddr*)&local_addr, sizeof(local_addr))) {
        AW_INFOF(("fail to bind\r\n"));
    }

    while (1) {
        recv_size = recvfrom(socfd,
                             msg,
                             sizeof(msg),
                             0,
                             (struct sockaddr *)&remote_addr,
                             &socklen);

        /* ��������Զ�˵����� */
        if (recv_size > 0) {

            /* �����յ�������ͨ�����Դ��ڴ�ӡ */
            AW_INFOF(("msg: "));
            aw_serial_write(AW_CFG_CONSOLE_COMID, msg, recv_size);
            AW_INFOF(("\r\n"));

            /* �����ݷ��ͻ�ȥ */
            sendto(socfd,
                   msg,
                   recv_size,
                   0,
                   (struct sockaddr *)&remote_addr,
                   sizeof(remote_addr));

            /* ��ջ����� */
            memset(msg, 0, sizeof(msg));
        }
    }

    closesocket(socfd);
    
    return 0;
}

/******************************************************************************/
void demo_net_udp_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Udp demo",
                         12,
                         4096,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Udp demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);
}

/** [src_udp] */

/* end of file */
