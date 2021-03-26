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
 * \brief GPRS例程（该例程演示GPRS设备建立socket通信，持续检测网络状态的功能）
 *
 * - 操作步骤:
 *   1. aw_prj_params.h 中使能：
 *      - 对应平台的USBH宏
 *      - 及对应平台串口宏
 *      - AW_COM_CONSOLE
 *      - 对应的GPRS设备宏(例如：AW_DEV_GPRS_ME909S)
 *   2. 将GPRS模块插入开发板的USB HOST1接口；
 *   3. 在电脑上打开 TCP&UDP 调试软件，建立并启动服务器。
 *
 * - 实验现象:
 *   1. 串口打印： GPRS设备连接网络的调试信息；
 *   2. TCP&UDP 调试软件收到GPRS设备发送的数据，且串口打印TCP&UDP上位机发送的数据。
 *
 * - 备注:
 *   1.平台支持的GPRS设备目前有sim800、me909、u9300、ec20，对应设备的配置请查看相应的初始化文件awbl_hwconf_gprs_xxxxx.h。
 * \code
 *
 * \endcode
 *
 * \par 源代码
 * \snippet demo_gprs_socket_check.c src_gprs_socket_check
 *
 * \internal
 * \par modification history:
 * - 1.00 18-07-12 lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_gprs_socket_check GPRS例程(状态检测)
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

/* 使用TCP协议连接网络 */
#define TEST_GPRS_MODE                  "TCP"

/*访问的服务器IP，用户需根据具体情况修改*/
#define TEST_SERVER_IP                  "121.33.243.38"

/*访问的服务器端口，用户需根据具体情况修改*/
#define TEST_SEEVER_PORT                22203

/* 系统的工作状态  */
enum GPRS_WORK_STATE {
    GPRS_WORK_POWER_DOWN,       /*  模块断电  */
    GPRS_WORK_POWER_ON,         /*  模块上电 */
    GPRS_WORK_INIT,             /*  模块初始化  */
    GPRS_WORK_INIT_CHK,         /*  模块状态检测 */
    GPRS_WORK_PPP_LINK,         /*  模块PPP连接  */
    GPRS_WORK_CLIENT_LINK,      /*  模块TCP客户端连接 */
    GPRS_WORK_WORKING,          /*  模块传输数据  */
};

static uint8_t gprs_work_state;
static uint8_t tcp_link_enable;    /*  TCP连接使能  */
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

            //获取信息强度
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_CSQ_GET, &args) == AW_OK){
                aw_kprintf("get csq : %d.\r\n", args.csq);
            }

            /* 使用 AT_TRANS 选项查询信号强度 */
            {
                //接收AT命令结果
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

            // 获取GPRS模块CCID
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_CCID_GET, &args) == AW_OK) {
                aw_kprintf("get gprs device ccid : %s.\n", args.ccid);
            }

            //获取APN
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_APN_GET, &args) == AW_OK) {
                aw_kprintf("get apn : %s.\n", args.p_apn);
            }

            // 获取GPRS模块的版本号
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_VERSION_GET, &args) == AW_OK) {
                aw_kprintf("get gprs device version : %s.\n", args.version);
            }

            // 获取GPRS模块的IMEI号
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_IMEI_GET, &args) == AW_OK) {
                aw_kprintf("get imei: %s.\n", args.imei);
            }

            // 获取GPRS模块的IMSI号
            char imsi[64];
            memset(imsi , 0, 64);
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_IMSI_GET, &args) == AW_OK) {
                aw_kprintf("get imsi: %s.\n", args.imsi);
            }

            // 获取GPRS模块的短信息中心号码
            char sca[32];
            memset(sca , 0, 32);
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_SCA_GET, (void *)&sca) == AW_OK) {
                aw_kprintf("get sca: %s.\n", sca);
            }

            //获取基站信息
            uint32_t bs_info[2];
            if(aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_BS_GET, (void *)bs_info) == AW_OK) {
                aw_kprintf("bs region:%d,小区ID:%d",bs_info[0],bs_info[1]);
            }

            gprs_work_state = GPRS_WORK_PPP_LINK;
            break;

        case GPRS_WORK_POWER_DOWN:
            aw_kprintf("GPRS power off\r\n\r\n");
            aw_gprs_power_off(gprs_id);/* 关闭gprs设备 */
            gprs_work_state = GPRS_WORK_POWER_ON;
            break;

        case GPRS_WORK_PPP_LINK:
            // 请求建立网络连接
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

            tcp_link_enable = AW_TRUE;            /* 允许TCP连接任务开始连接  */
            gprs_work_state = GPRS_WORK_WORKING;
            break;

        case GPRS_WORK_WORKING:

            /*监控网络状态*/
            if (tcp_link_enable == AW_FALSE) {    /*  连接错误   */
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

/* TCP 任务入口*/
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

        /*初始化server结构体*/
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
        /*设置接收超时时间*/
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

        /* 连接服务器  */
        if (connect(hd, (struct sockaddr *) &server, sizeof(server)) == 0) {
            aw_kprintf("TCP connect success\r\n\r\n");
            link_err = 0;
            send(hd, "hello,i'm gprs tcp client.\r\n", 28, 0); /* 向服务器发送数据 */

            for (;;) {
                memset(net_buf,'\0',1500);/*清空缓存区*/
                rcv_len = recv(hd, net_buf, sizeof(net_buf), 0);/* 接收服务器发送的数据 */
                if(rcv_len > 0){
                    aw_kprintf("recv:%s\r\n",net_buf);/*打印数据*/
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
                        //此判断用于处理keil下errno为0的问题
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
 * \brief net 示例程序入口
 * \return 无
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
