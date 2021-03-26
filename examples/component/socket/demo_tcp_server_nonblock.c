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
 * - ��������socket������һ��TCP ������(select������ģʽ)��
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
 *      - ����������ͨ�������鿪����ֱ�ִ�� "wifi join wifiname wifipassword" shell��������������硣
 *   2. ������Ҳ�������TCP��λ�����ʹ��:
 *      - ׼��һ�鿪�������ظ����̣�
 *      - ����������ͨ����ִ�� "wifi join wifiname wifipassword" shell�������ͬһwifi���硣
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
 * \snippet demo_tcp_server_nonblock.c src_demo_tcp_server_nonblock
 *
 * \internal
 * \par Modification history
 * - 1.10 18-05-07  phd, �Ƴ���дIP��ַ���޹ز���.
 * - 1.00 17-09-11  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_tcp_server_nonblock TCP������(������)
 * \copydoc demo_tcp_server_nonblock.c
 */

/** [src_tcp_server_nonblock] */
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
void demo_tcp_server_nonblock_entry (void)
{
    struct sockaddr_in  server_addr, client_addr;
    int                 server_sock, client_sock;
    int                 rcv_len, ret;
    char                net_buf[1500];
    socklen_t           len;
    struct timeval      select_timeout;
    fd_set              recv_fd, mon_fd;

    int keep_idle       = 10;   /* ����ʱ�� ���룩 */
    int keep_count      = 3;    /* ������ط������������һ��̽����յ�����Ӧ ������Ĳ��ٷ��� */
    int keep_interval   = keep_idle / keep_count + 1;   /* �������̽��ʱ�������룩 */
    int sockopt_on      = 1;    /* �����ظ�����˿� */

    memset(net_buf,'\0', 1500);
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));


    /* ���÷����������� */
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(LOCAL_SERVER_PORT); /* ���÷������Ķ˿� */
    server_addr.sin_addr.s_addr = 0;                        /* �����нӿ��ϼ��� */
    server_addr.sin_len         = sizeof(server_addr);
    memset(&(server_addr.sin_zero), 8, sizeof(server_addr.sin_zero));

    /* ����socket�׽��֣�ʹ��TCP���� */
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > server_sock) {
        AW_INFOF(("TCP server socket failed!" ENDL));
        return;
    }

    /* �����շ����ݳ�ʱʱ�� */
    select_timeout.tv_sec  = 1000;
    select_timeout.tv_usec = 1000;

    /* �����׽��� */
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&sockopt_on, sizeof(int));
    setsockopt(server_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&sockopt_on, sizeof(int));

    /* �󶨱��������ӿ� */
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

            /* ���ñ���ʱ�� */
            if (keep_idle > 0) {
                setsockopt(client_sock,
                           SOL_SOCKET,
                           SO_KEEPALIVE,
                           (void *)&sockopt_on,
                           sizeof(int));
                setsockopt(client_sock,
                           IPPROTO_TCP,
                           TCP_KEEPIDLE,
                           (void *)&keep_idle,
                           sizeof(keep_idle));
                setsockopt(client_sock,
                           IPPROTO_TCP,
                           TCP_KEEPINTVL,
                           (void *)&keep_interval,
                           sizeof(keep_interval));
                setsockopt(client_sock,
                           IPPROTO_TCP,
                           TCP_KEEPCNT,
                           (void *)&keep_count,
                           sizeof(keep_count));
            }

            FD_ZERO(&mon_fd);               /* ��������� */
            FD_SET(client_sock, &mon_fd);   /* ����׽��������� */
            for (;;) {
                memcpy(&recv_fd, &mon_fd, sizeof(fd_set));

                /* ������ģʽ */
                ret = select(client_sock + 1, &recv_fd, NULL, NULL, &select_timeout);
                if (ret > 0 && FD_ISSET(client_sock, &recv_fd)) {
                    send(client_sock, "hello,i'm tcp server.", 21, 0);          /* �������� */
                    rcv_len = recv(client_sock, &net_buf, sizeof(net_buf), 0);  /* �������ݣ����� */
                    if (rcv_len <= 0) {
                        break;
                    }
                    aw_kprintf("recv:%s\r\n", net_buf);
                    memset(net_buf, '\0', 1500);
                } else {
                    if (ret < 0) {
                        break;
                    }
                }
            }
            closesocket(client_sock);   /* �رմ����� */
            AW_INFOF(("TCP server: client %s:%d leave." ENDL,
                      inet_ntoa(client_addr.sin_addr),
                      htons(client_addr.sin_port)));
            aw_mdelay(5);
        }
    }
}

/** [src_tcp_server_nonblock] */

/* end of file */
