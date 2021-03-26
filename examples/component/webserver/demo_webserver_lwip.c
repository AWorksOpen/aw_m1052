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
 * \brief web����������(lwip�ӿ�ʵ��)
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ��Ӧƽ̨�������ꣻ
 *   2. �����߽����Ӻ͵�������������
 *   3. �û���򿪵��Ե����繲�����Ĳ鿴���ĵ���ʹ�õ��Ǿ�̬IP��ַ���Ƕ�̬IP��ַ��
 *      ������ö�̬��ʽ���� �ر� IP_ADDR_STATIC �궨�壬�������˲��裻
 *      ������þ�̬��ʽ����ͨ����������һ�ַ�ʽ����֤�����������ͬһ�����Σ�
 *      (1) �޸ĵ��Ե�ip��ַ�����ص�ַ����֤�Ϳ�����ͬһ���Σ�
 *      (2) �޸� LOCAL_IP_ADDR0 ��LOCAL_GATAWAY_ADDR0 �궨�屣֤������͵�����ͬһ������
 *   4. �ڵ��Զ˴����������������http://192.168.12.200,�������ʾһ���򵥵���ҳ��
 *   5. �����ʾʧ�ܣ����ڴ��ڴ�ӡ����ִ��shell���
 *      - "ip addr"�� �鿴����������0 ip��ַ�Ƿ����óɹ���
 *      - "ping ����ip��ַ"���鿴���ԺͿ�����������Ƿ����ӳɹ���
 *      �򿪵���cmd����ִ��"ping ������ip��ַ",�鿴���ԺͿ������໥֮�������Ƿ����ӳɹ���
 *      ������ϲ���ȫ��ִ�гɹ�����ɽ������ӣ�������鿴�޸�ʧ��ԭ��
 *
 * - ʵ������
 *   1. �ڵ��Զ˴����������������http://192.168.12.200,�������ʾһ���򵥵���ҳ��
 *
 * \par Դ����
 * \snippet demo_webserver_lwip.c src_webserver_lwip
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_webserver_lwip web������
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
 * �û���򿪵��Ե����繲�����Ĳ鿴���ĵ���ʹ�õ��Ǿ�̬IP��ַ���Ƕ�̬IP��ַ��
 * ������ĵ���ʹ�õ��Ǿ�̬IP��ַ�뿪���˺꣬�����彫ʹ�þ�̬��ʽ��������0 IP��ַ
 * ������ĵ���ʹ�õ��Ƕ�̬IP��ַ�����δ˺꣬�����彫ʹ�ö�̬��ʽ�������0 IP��ַ
 */
#define IP_ADDR_STATIC

#ifdef IP_ADDR_STATIC
#define LOCAL_IP_ADDR0                 "192.168.12.200"  /* ������ ����0 IP ��ַ���û��豣֤������͵��Ե�IP��ַ��ͬһ����   */
#define LOCAL_NET_MASK0                "255.255.255.0"   /* ������ ����0 ��������   */
#define LOCAL_GATAWAY_ADDR0            "192.168.12.254"  /* ������ ����0 ���ص�ַ ���û��豣֤������͵��Ե����ص�ַ��ͬһ���� */
#endif

#define LOCAL_NIC0_NAME                "eth0"            /* ����������0 ����(������������0�������ļ��ж���) */
#define LOCAL_PORT                     4000              /* ���Ӷ˿�         */

static aw_err_t __config_eth0_ip (void)
{
    struct in_addr  addr;
    aw_netif_t     *p_netif;
    aw_err_t        ret;

    /*������0�豸*/
    p_netif = aw_netif_dev_open(LOCAL_NIC0_NAME);
    if (p_netif == NULL) {
        aw_kprintf("aw_netif_dev_open fail\r\n");
        return AW_ERROR;
    }

#ifdef IP_ADDR_STATIC

    /* �ر� DHCPC �ͻ��ˣ�����0ʹ�þ�̬IP��ַ */
    ret =  aw_netif_dhcpc_stop(p_netif);
    if (ret != AW_OK) {
        aw_kprintf("awbl_nic_dhcpc_stop fail, error code:%d\r\n", ret);
    }

    /* ��������0 IP ��ַ  */
    inet_aton(LOCAL_IP_ADDR0, &addr);
    ret = aw_netif_ipv4_ip_set(p_netif, &addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_ip_set fail, error code:%d\r\n", ret);
        return ret;
    }

    /* ��������0 ��������  */
    inet_aton(LOCAL_NET_MASK0, &addr);
    ret = aw_netif_ipv4_netmask_set(p_netif, &addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_netmask_set fail, error code:%d\r\n", ret);
        return ret;
    }

    /* ��������0 ����  */
    inet_aton(LOCAL_GATAWAY_ADDR0, &addr);
    aw_netif_ipv4_gw_set(p_netif, &addr);
    if (ret != AW_OK) {
        aw_kprintf("aw_netif_ipv4_gw_set fail, error code:%d\r\n", ret);
        return ret;
    }

#else

    /* ����DHCPC �ͻ��� ����ʹ�ö�̬��ʽ���ÿ���������0��IP��ַ*/
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

    __pstConn = netconn_new(NETCONN_TCP);   /* ����TCP���� */
    netconn_bind(__pstConn, NULL, 80);      /* 80�˿� */
    netconn_listen(__pstConn);              /* ��ʼ���� */

    while (1) {

        aw_kprintf("web server waiting for connection\n");

        /* �����ⲿ�������� */
        ret = netconn_accept(__pstConn, &__pstNewConn);

        if (__pstNewConn == NULL) {
            continue;
        }

        if (ret == 0) {

            /* ������������ */
            ret = netconn_recv(__pstNewConn, &buf);
            if (buf != NULL) {

                /* get data */
                netbuf_data(buf, (void**)&data, &length);

                aw_kprintf("buf:%s\r\n", data);

                /* release buffer */
                netbuf_delete(buf);
            }

            /* ��ͻ��˷���һ����ҳ���� */
            netconn_write(__pstNewConn, html_data, sizeof(html_data), NETCONN_COPY);
            netconn_close(__pstNewConn);
        }
    }

    return 0;
}
/**
 * \brief tcp ����˵���ں���
 */
void demo_web_server_entry (void)
{
    /* ���ÿ����������0 IP��ַ */
    if (__config_eth0_ip() != AW_OK) {
        return ;
    }

    /* ��������ʵ�� webserver_task */
    AW_TASK_DECL_STATIC(webserver_task, 1024);

    /* ��ʼ������ */
    AW_TASK_INIT(webserver_task,            /* ����ʵ�� */
                 "webserver_task",          /* �������� */
                 5,                         /* �������ȼ� */
                 1024,                      /* �����ջ��С */
                 __webserver_task,    /* ������ں��� */
                 (void*)NULL);              /* ������ڲ��� */

    /* �������� */
    AW_TASK_STARTUP(webserver_task);

}

/** [src_webserver_lwip] */

/* end of file */
