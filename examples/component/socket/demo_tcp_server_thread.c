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
 * \brief socketͨ����ʾ����(ͨ�����߳�ʵ��һ�������������ͻ���ͨ��)��
 *
 * - ��������socket������һ��TCP ��������
 *
 * - ˵����
 *   1. ��Ҫ�� aw_prj_params.h ��ʹ������һ�����������磺
 *        1) ��̫��
 *        2) Wi-Fi
 *
 * - ��������(��̫��)��
 *   1. ���������TCP��λ�����ʹ��:
 *      - ׼��һ�鿪�������ظ����̣�
 *      - ������������0������������ӣ���֤���� IP ��ַ�뿪����� IP ����ͬһ�����£�
 *      - �������к󣬴�TCP��λ������������ͻ��ˣ����5������
 *
 * - ��������(Wi-Fi)��
 *   1. ������Ҳ�������TCP��λ�����ʹ��:
 *      - ׼��һ�鿪�������ظ����̣�
 *      - ����������ͨ����ִ�� "wifi join wifiname wifipassword" shell��������������硣
 *      - �������к󣬴�TCP��λ������������ͻ��ˣ����5������
 *
 * - ʵ������
 *   1. ���ڴ�ӡ���ͻ��˺ͷ������շ����ݵĵ�����Ϣ��
 *   2. �����������Զ�Ӧ5���ͻ��ˣ���ϵͼ����:
 *                      ������<���� ���� ����>�ͻ���1
 *                        .
 *                       /|\
 *                        |
 *                       \|/
 *                        .
 *                       �ͻ���2
 * - ��ע��
 *   1. lwIP �����ļ� lwipopts.h �� aworks_sdk\include\config Ŀ¼�� �������ò����ʣ����Խ�
 *      lwIP Դ��Ž������У����޸ĸ������ļ���
 *   2. ��̫�������ļ�Ϊ awbl_hwconf_xxx_enet.h�������ļ�����ƽ̨��أ���
 *   3. Wi-Fi �����ļ�Ϊ awbl_hwconf_cyw43362_wifi_spi.h �Լ�  awbl_hwconf_cyw43362.c���������ã���
 *
 * \par Դ����
 * \snippet demo_tcp_server_thread.c src_tcp_server_thread
 *
 * \internal
 * \par Modification history
 * - 1.10 18-05-07  phd, �Ƴ���дIP��ַ���޹ز���.
 * - 1.00 17-09-11  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_tcp_server_thread TCP������(һ�Զ�)
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
    closesocket(client_sockfd); /* �رմ����� */
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
    server_address.sin_port         = htons(1234);/*�������˿ں�Ϊ1234*/
    server_len = sizeof(server_address);

    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    listen(server_sockfd, 5);

    /* ��ʼ��һ���ӳ���ҵ */
    aw_defer_init(&__g_defer);

    /* �����ӳ���ҵ */
    AW_DEFER_START(&__g_defer, 5, 0, 4096, "defer");

    while (1) {
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
        if(client_sockfd > 0){

            /* ��ʼ��һ�������ڵ� */
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
