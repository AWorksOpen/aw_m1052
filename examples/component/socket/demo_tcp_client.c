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
 * - ��������socket������һ��TCP �ͻ��ˡ�
 *
 * - ˵����
 *   1. ��Ҫ�� aw_prj_params.h ��ʹ������һ�����������磺
 *        1) ��̫��
 *        2) Wi-Fi
 *
 * - ��������(��̫��)��
 *   1. �����̿�����϶�Ӧ�ķ���˳���ʹ�ã�
 *      - ׼�����鿪���壬����һ�鿪�������� demo_tcp_server_block.c ����
 *        demo_tcp_server_nonblock.c ����������Ȼ�����̫���������ļ�
 *        awbl_hwconf_xxx_enet.h �޸�������IP��ַ(�����������IP��ַ������ͬһ����)��
 *        ���ظ����̵���һ�鿪���壻
 *      - �����鿪���������0ͨ���������ӡ�
 *   2. ������Ҳ�������TCP��λ�����ʹ��:
 *      - ׼��һ�鿪�������ظ����̣�
 *      - ������������0������������ӣ���֤���� IP ��ַ�뿪����� IP ����ͬһ�����£�
 *      - ��TCP��λ��������������������������ӣ��������ݡ�
 *
 * - ��������(Wi-Fi)��
 *   1. �����̿�����϶�Ӧ�ķ���˳���ʹ�ã�
 *      - �� aw_prj_params.h ��ʹ�� AW_NET_TOOLS��AW_COM_SHELL_SERIAL ��
 *      - ׼�����鿪���壬����һ������ demo_tcp_server_block.c ���� demo_tcp_server_nonblock.c
 *        ����������һ�����ر�������Ϊ�ͻ��ˡ�
 *      - ����������ͨ�������鿪����ֱ�ִ�� "wifi join wifiname wifipassword" shell��������������硣
 *   2. ������Ҳ�������TCP��λ�����ʹ��:
 *      - ׼��һ�鿪�������ظ����̣�
 *      - ����������ͨ����ִ�� "wifi join wifiname wifipassword" shell��������������硣
 *      - ��TCP��λ��������������������������ӣ��������ݡ�
 *
 * - ʵ������
 *   1. ���ڴ�ӡ���ͻ��˺ͷ������շ����ݵĵ�����Ϣ��
 *   2. ���������Զ�Ӧ����ͻ��ˣ�������ʵ�ֵ���һ��һ��ͨ��ģʽ���û�����ʵ��һ�Զ��ͨ��ģʽ�ɴ������߳����ж��壬��ϵͼ����:
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
 * \snippet demo_tcp_client.c src_demo_tcp_client
 *
 * \internal
 * \par Modification history
 * - 1.10 18-05-07  phd, �Ƴ���дIP��ַ���޹ز���.
 * - 1.00 17-09-11  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_tcp_client TCP�ͻ���
 * \copydoc demo_tcp_client.c
 */

/** [src_demo_tcp_client] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_task.h"

#include "aw_net.h"
#include "aw_sockets.h"

#include <string.h>


/* �ͻ��˷��ʵķ�����IP��ַ */
#define REMOTE_SERVER_ADDR          "192.168.12.237"      /* ��Ӧ�������� IP ��ַ,�û���Ҫ���ݾ��崴���ķ���˵�IP��ַ�޸� */
#define REMOTE_SERVER_PORT          4000                  /* �ͻ��˷��ʷ������˿� */

/**
 * \brief net ʾ���������
 * \return ��
 */
void demo_tcp_client_entry (void)
{
    struct sockaddr_in  server_addr;
    int                 rcv_len;
    int                 sock;
    char                net_buf[1500];

    memset(net_buf, '\0', 1500);

    /* ���ÿͻ��˷��ʵķ�����IP��ַ���˿ں� */
    inet_aton(REMOTE_SERVER_ADDR, &server_addr.sin_addr);
    server_addr.sin_family  = AF_INET;
    server_addr.sin_port    = htons(REMOTE_SERVER_PORT);
    server_addr.sin_len     = sizeof(server_addr);

    AW_INFOF(("TCP client: connecting..." ENDL));
    for (;;) {
        sock = socket(AF_INET, SOCK_STREAM, 0);     /* ����socket�׽��� */
        if (sock < 0) {
            AW_INFOF(("TCP server socket failed!" ENDL));
            return;
        }

        /* ���ӷ����� */
        if (0 == connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
            AW_INFOF(("TCP client: connected." ENDL));
            for (;;) {
                send(sock, "hello, i'm tcp client.", 21, 0);        /* ��������������� */
                rcv_len = recv(sock, net_buf, sizeof(net_buf), 0);  /* ���շ��������͵����� */
                if (rcv_len <= 0) {
                    AW_INFOF(("TCP client: disconnect. ret=%d, err=%d" ENDL, rcv_len, errno));
                    break;
                }
                aw_kprintf("recv:%s\r\n", net_buf);     /*��ӡ����*/
                memset(net_buf, '\0', 1500);            /*��ջ�����*/
            }
        }

        closesocket(sock); /* �رմ����� */
        aw_mdelay(1000);
    }
}

/** [src_demo_tcp_client] */

/* end of file */
