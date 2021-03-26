/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief GPRS���̣���������ʾGPRS�豸����socketͨ�ţ������������״̬�Ĺ��ܣ�
 *
 * - ��������:
 *   1. aw_prj_params.h ��ʹ�ܣ�
 *      - ��Ӧƽ̨��USBH��
 *      - ����Ӧƽ̨���ں�
 *      - AW_COM_CONSOLE
 *      - ��Ӧ��GPRS�豸��(���磺AW_DEV_GPRS_ME909S)
 *   2. ��GPRSģ����뿪�����USB HOST1�ӿڣ�
 *   3. �ڵ����ϴ� TCP&UDP ���������������������������
 *
 * - ʵ������:
 *   1. ���ڴ�ӡ�� GPRS�豸��������ĵ�����Ϣ��
 *   2. TCP&UDP ��������յ�GPRS�豸���͵����ݣ��Ҵ��ڴ�ӡTCP&UDP��λ�����͵����ݡ�
 *
 * - ��ע:
 *   1.ƽ̨֧�ֵ�GPRS�豸Ŀǰ��sim800��me909��u9300��ec20����Ӧ�豸��������鿴��Ӧ�ĳ�ʼ���ļ�awbl_hwconf_gprs_xxxxx.h��
 * \code
 *
 * \endcode
 *
 * \par Դ����
 * \snippet demo_gprs_socket_check.c src_gprs_socket_check
 *
 * \internal
 * \par modification history:
 * - 1.00 18-07-12 lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_gprs_socket_check GPRS����(״̬���)
 * \copydoc demo_gprs_socket_check.c
 */

/** [src_gprs_socket_check] */
#define AWBL_GPRS_USE_LWIP_2_X 1

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_gprs.h"
#include <string.h>
#include "aw_net.h"
#include "aw_sockets.h"
#include <string.h>
#include "aw_delay.h"
#include "aw_adc.h"
#include "aw_gprs.h"
#include "aw_demo_config.h"
#if AWBL_GPRS_USE_LWIP_2_X
#include <lwip/ext/net_sts.h>
#endif

/* ʹ��TCPЭ���������� */
#define TEST_GPRS_MODE                  "TCP"

/*���ʵķ�����IP���û�����ݾ�������޸�*/
#define TEST_SERVER_IP                  "121.33.243.38"

/*���ʵķ������˿ڣ��û�����ݾ�������޸�*/
#define TEST_SEEVER_PORT                22203

/* ϵͳ�Ĺ���״̬  */
enum GPRS_WORK_STATE {
    GPRS_WORK_POWER_DOWN,       /*  ģ��ϵ�  */
    GPRS_WORK_POWER_ON,         /*  ģ���ϵ� */
    GPRS_WORK_INIT,             /*  ģ���ʼ��  */
    GPRS_WORK_INIT_CHK,         /*  ģ��״̬��� */
    GPRS_WORK_PPP_LINK,         /*  ģ��PPP����  */
    GPRS_WORK_CLIENT_LINK,      /*  ģ��TCP�ͻ������� */
    GPRS_WORK_WORKING,          /*  ģ�鴫������  */
};

static uint8_t gprs_work_state;
static uint8_t tcp_link_enable;    /*  TCP����ʹ��  */
static aw_bool_t __g_gprs_link_up = 0;

aw_local void __4g_evt_handler (int dev_id, aw_gprs_evt_t *p_evt)
{
    aw_err_t                 ret;

    switch (p_evt->type) {

        case AW_GPRS_EVT_TYPE_PWR_ON: {
            __g_gprs_link_up = AW_FALSE;
        } break;

        case AW_GPRS_EVT_TYPE_PPP_LINK_UP:
        case AW_GPRS_EVT_TYPE_ETH_LINK_UP: {
            __g_gprs_link_up = AW_TRUE;
        } break;

        case AW_GPRS_EVT_TYPE_ETH_LINK_DOWN:
        case AW_GPRS_EVT_TYPE_PPP_LINK_DOWN: {
            __g_gprs_link_up = AW_FALSE;
        } break;

        default: break;
    }
}

static void __4g_log (void         *p_color,
                      const char   *func_name,
                      int           line,
                      const char   *fmt, ...)
{
}


aw_local void __gprs_state_task_entry (void * p_arg)
{
    aw_err_t             ret      = AW_OK;
    int                  gprs_id;
    aw_gprs_ioctl_args_t args;
    int                  i;

    gprs_work_state  = (uint8_t)(int)p_arg;

    gprs_id = aw_gprs_any_get(30);
    if (gprs_id < 0) {
        aw_kprintf("GPRS get dev id failed\r\n");
        return ;
    }


    while(1){
        switch(gprs_work_state){

        case GPRS_WORK_POWER_ON:
            aw_kprintf("GPRS power on\r\n\r\n");
            while(aw_gprs_power_on(gprs_id) != AW_OK) {
                aw_mdelay(100);
            }

            args.pfn_log_out = __4g_log;
            ret = aw_gprs_ioctl(gprs_id, AW_GPRS_LOG_SET, &args);
            aw_assert(ret == AW_OK);

            args.pfn_evt_handler = __4g_evt_handler;
            ret = aw_gprs_ioctl(gprs_id, AW_GPRS_EVT_CB_SET, &args);
            aw_assert(ret == AW_OK || ret == -AW_EEXIST);

            gprs_work_state = GPRS_WORK_INIT_CHK;
            break;

        case GPRS_WORK_INIT_CHK:

            ret = aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_STS, &args);

            if ( ret == AW_OK && args.sts == AW_GPRS_STS_OK){
                aw_kprintf("get status : AW_GPRS_STS_OK.\r\n");
                gprs_work_state = GPRS_WORK_INIT;
            }

            break;

        case GPRS_WORK_INIT:

            //��ȡ��Ϣǿ��
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_CSQ_GET, &args) == AW_OK){
                aw_kprintf("get csq : %d.\r\n", args.csq);
            }

            /* ʹ�� AT_TRANS ѡ���ѯ�ź�ǿ�� */
            {
                //����AT������
                char  rec_buf[256];

                memset(rec_buf, 0, 256);

                args.at_trans.p_at_cmd        = "AT+CSQ\r\n";
                args.at_trans.p_recv_expect   = "CSQ:";
                args.at_trans.p_recv_buf      = rec_buf;
                args.at_trans.bufsize         = sizeof(rec_buf);
                args.at_trans.recv_tm         = 1500;

                ret = aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_AT_TRANS, &args);
                if (ret == AW_OK) {
                    AW_INFOF(("recv : %s.\n", rec_buf));
                }
            }

            // ��ȡGPRSģ��CCID
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_CCID_GET, &args) == AW_OK) {
                aw_kprintf("get gprs device ccid : %s.\n", args.ccid);
            }

            //��ȡAPN
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_APN_GET, &args) == AW_OK) {
                aw_kprintf("get apn : %s.\n", args.p_apn);
            }

            // ��ȡGPRSģ��İ汾��
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_VERSION_GET, &args) == AW_OK) {
                aw_kprintf("get gprs device version : %s.\n", args.version);
            }

            // ��ȡGPRSģ���IMEI��
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_IMEI_GET, &args) == AW_OK) {
                aw_kprintf("get imei: %s.\n", args.imei);
            }

            // ��ȡGPRSģ���IMSI��
            char imsi[64];
            memset(imsi , 0, 64);
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_IMSI_GET, &args) == AW_OK) {
                aw_kprintf("get imsi: %s.\n", args.imsi);
            }

            // ��ȡGPRSģ��Ķ���Ϣ���ĺ���
            char sca[32];
            memset(sca , 0, 32);
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_SCA_GET, (void *)&sca) == AW_OK) {
                aw_kprintf("get sca: %s.\n", sca);
            }

            //��ȡ��վ��Ϣ
            uint32_t bs_info[2];
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_BS_GET, (void *)bs_info) == AW_OK) {
                aw_kprintf("bs region:%d,С��ID:%d",bs_info[0],bs_info[1]);
            }

            gprs_work_state = GPRS_WORK_PPP_LINK;
            break;

        case GPRS_WORK_POWER_DOWN:
            aw_kprintf("GPRS power off\r\n\r\n");
            aw_gprs_power_off(gprs_id);/* �ر�gprs�豸 */
            gprs_work_state = GPRS_WORK_POWER_ON;
            break;

        case GPRS_WORK_PPP_LINK:
            // ��������������
            ret = aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_NET_LINK_UP, NULL);
            aw_assert(ret == AW_OK);
            i = 0;
            while (!__g_gprs_link_up) {
                i++;
                aw_mdelay(1000);
                if (i >= 30) {
                    aw_kprintf("GPRS connection failed.\r\n");
                    gprs_work_state = GPRS_WORK_POWER_DOWN;
                    break;
                }
            }
            aw_kprintf("GPRS connection successful.\r\n");

            for (struct netif *p_if = netif_list; p_if != NULL; p_if = p_if->next) {
                if (p_if->name[0] == 'p') {
                    netif_set_default(p_if);
                }
            }

            ret = aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_IP_GET, &args);
            aw_assert(ret == AW_OK);
            aw_kprintf("IP: %s\r\n\r\n", inet_ntoa(args.ip_addr));
            gprs_work_state = GPRS_WORK_CLIENT_LINK;

            break;

        case GPRS_WORK_CLIENT_LINK:

            aw_kprintf(("GPRS TCP connect\r\n\r\n"));

            tcp_link_enable = AW_TRUE;            /* ����TCP��������ʼ����  */
            gprs_work_state = GPRS_WORK_WORKING;
            break;

        case GPRS_WORK_WORKING:

            /*�������״̬*/
            if (tcp_link_enable == AW_FALSE) {    /*  ���Ӵ���   */
                aw_kprintf("GPRS connection close\r\n\r\n");
                aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_NET_LINK_DOWN, (void *)NULL);
                while (__g_gprs_link_up) {
                    aw_mdelay(1000);
                }
                gprs_work_state = GPRS_WORK_POWER_DOWN;
            }

            aw_mdelay(100);

            break;
        }
    }

    return ;
}

/* TCP �������*/
aw_local void __socket_task_entry (void * p_arg){

    aw_err_t ret;
    struct sockaddr_in  server;
    int hd;
    char net_buf[1500];
    uint8_t link_err = 0;
    int rcv_len;
    int i = 0;

    while(1){

        if (tcp_link_enable == AW_FALSE) {
            link_err = 0;
            aw_mdelay(100);
            continue;
        }

        aw_kprintf("Connecting to the server IP: %s, PORT: %d\r\n\r\n", TEST_SERVER_IP, TEST_SEEVER_PORT);

        hd = socket(AF_INET, SOCK_STREAM, 0);
        if (hd == -1) {
            aw_kprintf("TCP server socket failed!\r\n");
            if (++link_err >= 10) {
                link_err = 0;
                tcp_link_enable = AW_FALSE;
            }
            continue;
        }

        /*��ʼ��server�ṹ��*/
        memset(&server, 0, sizeof(struct sockaddr_in));
        server.sin_family = AF_INET;
        server.sin_port   = htons(TEST_SEEVER_PORT);
        server.sin_len     = sizeof(server);
        ret = inet_aton(TEST_SERVER_IP, &(server.sin_addr));
        if (ret == 0) {
            aw_kprintf(("inet_aton fail\r\n\r\n"));
            closesocket(hd);
            if (++link_err >= 10) {
                link_err = 0;
                tcp_link_enable = AW_FALSE;
            }
            continue;
        }
#if AWBL_GPRS_USE_LWIP_2_X
        struct timeval rt;
        rt.tv_usec = 0;
        rt.tv_sec = 1;
        ret = setsockopt(hd, SOL_SOCKET, SO_RCVTIMEO, &rt, sizeof(rt));
#else
        /*���ý��ճ�ʱʱ��*/
        ret = setsockopt(hd, SOL_SOCKET, SO_RCVTIMEO, &rcv_timeout, sizeof(rcv_timeout));
#endif

        if (ret != 0) {
            aw_kprintf("setsockopt fail\r\n\r\n");
            closesocket(hd);
            if (++link_err >= 10) {
                link_err = 0;
                tcp_link_enable = AW_FALSE;
            }
            continue;
        }

        /* ���ӷ�����  */
        if (connect(hd, (struct sockaddr *) &server, sizeof(server)) == 0) {
            aw_kprintf("TCP connect success\r\n\r\n");
            link_err = 0;
            send(hd, "hello,i'm gprs tcp client.\r\n", 28, 0); /* ��������������� */

            for (;;) {
                memset(net_buf,'\0',1500);/*��ջ�����*/
                rcv_len = recv(hd, net_buf, sizeof(net_buf), 0);/* ���շ��������͵����� */
                if(rcv_len > 0){
                    aw_kprintf("recv:%s\r\n",net_buf);/*��ӡ����*/
                    for (i = 0; i < 3; i++) {
                        if (send(hd, net_buf, rcv_len, 0) == rcv_len) {
                           break;
                        }
                        aw_mdelay(1000);
                    }
                    if (i >= 3) {
                       aw_kprintf("send failed, disconnect\r\n\r\n");
                       break;
                    }
                }else if(rcv_len == 0){
                    aw_kprintf("connected close.\r\n");
                    break;
                }else if(rcv_len < 0){
                    if(errno == AW_EAGAIN){
//                        aw_kprintf("recv timeout.\r\n");
                        continue;
                    }else if(errno == 0){
                        //���ж����ڴ���keil��errnoΪ0������
                        continue;
                    }else{
                        aw_kprintf("recv error.\r\n");
                        break;
                    }
                }

            }
        }else {

            aw_kprintf("TCP connect failed\r\n\r\n");

            if (++link_err >= 10) {

                link_err = 0;
                tcp_link_enable = AW_FALSE;
            }
        }
        closesocket(hd);

    }


}
/**
 * \brief net ʾ���������
 * \return ��
 */
void demo_gprs_socket_check_entry (void)
{

    aw_task_id_t task_id1,task_id2;
    task_id1 = aw_task_create("gprs_state_task",
                    12,
                    1024 * 8,
                    __gprs_state_task_entry,
                    (void *)GPRS_WORK_POWER_ON);

    if(task_id1 == NULL){
        aw_kprintf("gprs_state_task creat fail.\r\n");
        return ;
    }

    aw_err_t ret = aw_task_startup(task_id1);
    if(ret != AW_OK){
        aw_kprintf("gprs_state_task startup fail.\r\n");
        return ;
    }

    task_id2 = aw_task_create("socket_task",
                    13,
                    1024 * 8,
                    __socket_task_entry,
                    (void *)0x00);

    if(task_id2 == NULL){
        aw_kprintf("socket_task creat fail.\r\n");
        return ;
    }

    ret = aw_task_startup(task_id2);
    if(ret != AW_OK){
        aw_kprintf("socket_task startup fail.\r\n");
    }

    return ;

}

/** [src_gprs_socket_check] */

/* end of file */
