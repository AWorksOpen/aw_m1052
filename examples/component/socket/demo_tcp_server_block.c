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
 * \brief socketͨ����ʾ����(һ��һ)��
 *
 * - ��������socket������һ��TCP ������(����ģʽ)��
 *
 * - ˵����
 *   1. ��Ҫ�� aw_prj_params.h ��ʹ������һ�����������磺
 *        1) ��̫��
 *        2) Wi-Fi
 *
 * - ��������(��̫��)��
 *   1. �����̿�����Ͽͻ��˳���ʹ�ã�
 *      - ׼�����鿪���壬����һ�鿪�������ر����̣�Ȼ�����̫���������ļ�awbl_hwconf_xxx_enet.h��
 *        �޸�����0��IP��ַ(�����������IP��ַ������ͬһ����)������demo_tcp_client.c���̵���һ�鿪���壻
 *      - �����鿪���������0ͨ���������ӡ�
 *   2. ������Ҳ�������TCP��λ�����ʹ��:
 *      - ׼��һ�鿪�������ظ����̣�
 *      - ������������0������������ӣ���֤���� IP ��ַ�����õ� IP ����ͬһ�����£�
 *      - ��TCP��λ������������ͻ��ˣ��������ӣ��������ݡ�
 *
 * - ��������(Wi-Fi)��
 *   1. �����̿�����Ͽͻ��˳���ʹ�ã�
 *      - �� aw_prj_params.h ��ʹ�� AW_NET_TOOLS��AW_COM_SHELL_SERIAL ��
 *      - ׼�����鿪���壬����һ�����ر����̣���һ������ demo_tcp_client.c ������Ϊ�ͻ��ˡ�
 *      - ����������ͨ�������鿪����ֱ�ִ�� "wifi join wifiname wifipassword" shell�������ͬһ�������硣
 *   2. ������Ҳ�������TCP��λ�����ʹ��:
 *      - ׼��һ�鿪�������ظ����̣�
 *      - ����������ͨ����ִ�� "wifi join wifiname wifipassword" shell�������ͬһ�������硣
 *      - ��TCP��λ������������ͻ��ˣ��������ӣ��������ݡ�
 *
 * - ʵ������
 *   1. ���ڴ�ӡ���ͻ��˺ͷ������շ����ݵĵ�����Ϣ��
 *
 * - ��ע��
 *   1. lwIP �����ļ� lwipopts.h �� aworks_sdk\include\config Ŀ¼�� �������ò����ʣ����Խ�
 *      lwIP Դ��Ž������У����޸ĸ������ļ���
 *   2. ��̫�������ļ�Ϊ awbl_hwconf_xxx_enet.h�������ļ�����ƽ̨��أ���
 *   3. Wi-Fi �����ļ�Ϊ awbl_hwconf_cyw43362_wifi_spi.h �Լ�  awbl_hwconf_cyw43362.c���������ã���
 *   4. ���������Զ�Ӧ����ͻ��ˣ�������ʵ�ֵ���һ��һ��ͨ��ģʽ���û�����ʵ��һ�Զ��ͨ��ģʽ�ɴ������߳����ж��壬��ϵͼ����:
 *                      ������<���� ���� ����>�ͻ���1
 *                        .
 *                       /|\
 *                        |
 *                       \|/
 *                        .
 *                       �ͻ���2
 * \par Դ����
 * \snippet demo_server_block.c src_demo_server_block
 *
 * \internal
 * \par Modification history
 * - 1.10 18-05-07  phd, �Ƴ���дIP��ַ���޹ز���.
 * - 1.00 17-09-11  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_server_block TCP������(����ģʽ)
 * \copydoc demo_server_block.c
 */

/** [src_demo_server_block] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_task.h"

#include "aw_net.h"
#include "aw_sockets.h"

#include <string.h>

#define LOCAL_SERVER_PORT           4000    /* ���ط������˿� */

/* �ͻ��˷��ʵķ�����IP��ַ */

/**
 * \brief net ʾ���������
 * \return ��
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

    /* ����socket�׽��֣�ʹ��TCP���� */
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > server_sock) {
        AW_INFOF(("TCP server socket failed!" ENDL));
        return;
    }

    /* ���÷������Ķ˿ڵ����� */
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(LOCAL_SERVER_PORT); /* ���÷������Ķ˿� */
    server_addr.sin_addr.s_addr = 0;                        /* �����нӿ��ϼ��� */
    server_addr.sin_len         = sizeof(server_addr);
    memset(&(server_addr.sin_zero), 8, sizeof(server_addr.sin_zero));

    /* �󶨱��ؽӿ� */
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        AW_INFOF(("TCP server bind failed!" ENDL));
        closesocket(server_sock);
        return;
    }

    /* �������ģʽ */
    listen(server_sock, 1);
    AW_INFOF(("TCP server: listen on %s:%d" ENDL,
              inet_ntoa(server_addr.sin_addr),
              htons(server_addr.sin_port)));

    len = sizeof(struct sockaddr);

    for (;;) {

        /* �����ȴ��ͻ������� */
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &len);
        if (client_sock > 0) {
            AW_INFOF(("TCP server: client %s:%d connected." ENDL,
                      inet_ntoa(client_addr.sin_addr),
                      htons(client_addr.sin_port)));

            for (;;) {
                send(client_sock, "hello,i'm tcp server.", 21, 0);          /* �������� */
                rcv_len = recv(client_sock, &net_buf, sizeof(net_buf), 0);  /* �������ݣ�������ֱ���յ����� */
                if (rcv_len <= 0) {
                    break;
                }
                aw_kprintf("recv:%s\r\n", net_buf);     /* ��ӡ���յõ������� */
                memset(net_buf, '\0', 1500);            /* ��ջ����� */
            }

            closesocket(client_sock);   /* �رմ����� */
            AW_INFOF(("TCP server: client %s:%d leave." ENDL,
                      inet_ntoa(client_addr.sin_addr),
                      htons(client_addr.sin_port)));

            aw_mdelay(5);
        }
    }
}

/** [src_demo_server_block] */

/* end of file */
