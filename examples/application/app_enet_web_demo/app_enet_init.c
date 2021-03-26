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
 * \brief eth0��ʼ������
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ��Ӧƽ̨�������ꣻ
 *   2. �����߽����Ӻ͵�������������
 *   3. �û���򿪵��Ե����繲�����Ĳ鿴���ĵ���ʹ�õ��Ǿ�̬IP��ַ���Ƕ�̬IP��ַ��
 *      ������ö�̬��ʽ���� �� IP_OPEN_DHCPS �궨�壬�������˲��裻
 *      ������þ�̬��ʽ����ͨ����������һ�ַ�ʽ����֤�����������ͬһ�����Σ�
 *      (1) �޸ĵ��Ե�ip��ַ�����ص�ַ����֤�Ϳ�����ͬһ���Σ�
 *      (2) �޸� LOCAL_IP_ADDR0 ��LOCAL_GATAWAY_ADDR0 �궨�屣֤������͵�����ͬһ������
 *   4. �ڴ��ڴ�ӡ����ִ��shell���
 *      - "ip addr"�� �鿴����������0 ip��ַ�Ƿ����óɹ���
 *      - "ping ����ip��ַ"���鿴���ԺͿ������໥֮���������ӳɹ���
 *      �򿪵���cmd����ִ��"ping ������ip��ַ",�鿴���ԺͿ������໥֮���������ӳɹ���
 *      ������ϲ���ȫ��ִ�гɹ�����ɽ������ӣ�������鿴�޸�ʧ��ԭ��
 *
 * - ʵ������
 *   1. �ܲ鿴ip��ַ����pingͨ��
 *
 * \par Դ����
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
 * �û���򿪵��Ե����繲�����Ĳ鿴���ĵ���ʹ�õ��Ǿ�̬IP��ַ���Ƕ�̬IP��ַ��
 * ������ĵ���ʹ�õ��Ǿ�̬IP��ַ�뿪���˺꣬�����彫ʹ�þ�̬��ʽ��������0 IP��ַ
 * ������ĵ���ʹ�õ��Ƕ�̬IP��ַ�����δ˺꣬�����彫ʹ�ö�̬��ʽ�������0 IP��ַ
 */
//#define IP_OPEN_DHCPS

#define LOCAL_IP_ADDR0                 "172.16.18.201"  /* ������ ����0 IP ��ַ���û��豣֤������͵��Ե�IP��ַ��ͬһ����   */
#define LOCAL_NET_MASK0                "255.255.255.0"   /* ������ ����0 ��������   */
#define LOCAL_GATAWAY_ADDR0            "172.16.18.254"  /* ������ ����0 ���ص�ַ ���û��豣֤������͵��Ե����ص�ַ��ͬһ���� */

#define LOCAL_NIC0_NAME                "eth0"            /* ����������0 ����(������������0�������ļ��ж���) */
#define LOCAL_PORT                     80                /* ���Ӷ˿�         */


aw_local struct dhcp_server *__eth0_dhcp_server (void)
{
    aw_local struct dhcp_server dhcp_server;
    return &dhcp_server;
}


static aw_err_t __config_eth0_ip(void){
    struct in_addr addr;
    aw_netif_t * p_netif;
    aw_err_t ret;

    /*������0�豸*/
    p_netif = aw_netif_dev_open (LOCAL_NIC0_NAME);
    if(p_netif == NULL){
        aw_kprintf("aw_netif_dev_open fail\r\n");
        return AW_ERROR;
    }

    /* ��������0 IP ��ַ  */
    inet_aton(LOCAL_IP_ADDR0, &addr);
    ret = aw_netif_ipv4_ip_set(p_netif,&addr);
    if(ret != AW_OK){
        aw_kprintf("aw_netif_ipv4_ip_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* ��������0 ��������  */
    inet_aton(LOCAL_NET_MASK0, &addr);
    ret = aw_netif_ipv4_netmask_set(p_netif,&addr);
    if(ret != AW_OK){
        aw_kprintf("aw_netif_ipv4_netmask_set fail,error code:%d\r\n",ret);
        return ret;
    }

    /* ��������0 ����  */
    inet_aton(LOCAL_GATAWAY_ADDR0, &addr);
    aw_netif_ipv4_gw_set(p_netif,&addr);
    if(ret != AW_OK){
        aw_kprintf("aw_netif_ipv4_gw_set fail,error code:%d\r\n",ret);
        return ret;
    }

    dhcps_bind_netif(__eth0_dhcp_server(), LOCAL_NIC0_NAME);

#ifdef  IP_OPEN_DHCPS
    /* ���� DHCPS ����ˣ������彫��̬Ϊ�ͻ��˷���IP��ַ*/
    dhcps_start(__eth0_dhcp_server());
#else
    /* �ر� DHCPS ����ˣ��ͻ��������þ�̬IP��ַ */
    dhcps_stop(__eth0_dhcp_server());
#endif

    return AW_OK;
}

/**
 * \brief eth0 ���ó�ʼ������
 */
void app_enet_init (void)
{
    /* ���ÿ����������0 IP��ַ*/
    if (__config_eth0_ip() != AW_OK){
        return ;
    }
}
/** [src_enet_init] */

/* end of file */
