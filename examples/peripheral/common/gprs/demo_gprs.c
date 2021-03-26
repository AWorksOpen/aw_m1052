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
 * \brief GPRS例程（该例程演示GPRS设备发送短信和联网通信的功能）
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
 *   1. 手机客户端会收到短信。
 *   2. 串口打印： GPRS设备连接网络的调试信息；以及服务端发送的数据信息。
 *   3. TCP&UDP 调试软件收到GPRS设备发送的数据，
 *
 * - 备注:
 *   1.平台支持的GPRS设备目前有sim800、me909、u9300、ec20,对应设备的配置请查看相应的初始化文件awbl_hwconf_gprs_xxxxx.h。
 *   2.如果gprs模块出现“set PDP failed”的报错信息，请尝试将模块进行软复位或者重新上电。
 * \code
 *
 * \endcode
 *
 * \par 源代码
 * \snippet demo_gprs.c src_gprs
 *
 * \internal
 * \par modification history:
 * - 1.00 18-07-12 lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_gprs GPRS例程(标准接口)
 * \copydoc demo_gprs.c
 */

/** [src_gprs] */

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_gprs.h"
#include "awbl_gprs.h"
#include <string.h>
#include "aw_delay.h"
#include "aw_mem.h"
#include "aw_shell.h"
#if AWBL_GPRS_USE_LWIP_2_X
#include <lwip/ext/net_sts.h>
#endif
#include "aw_demo_config.h"

/* 使用TCP协议连接网络 */
#define TEST_GPRS_MODE                  "TCP"

/* 访问的服务器IP，用户需根据具体情况修改 */
#define TEST_SERVER_IP                  "121.33.243.38"

/* 访问的服务器端口，用户需根据具体情况修改 */
#define TEST_SEEVER_PORT                22203

/* 测试使用手机号，用户需修改*/
#define TEST_PHONE_NUM                  "+8618529453675"
/* 是否要进行复位操作*/
aw_bool_t is_reset = AW_FALSE;
/* 是否复位完成*/
aw_bool_t reset_ok = AW_FALSE;
#if AWBL_GPRS_USE_LWIP_2_X
/* 网络是否可用*/
aw_bool_t net_is_up = AW_FALSE;
#endif
/* GPRS设备ID*/
int gprs_id = -1;
/* 套接字句柄*/
int socket_hd = -1;

#if AWBL_GPRS_USE_LWIP_2_X
/** 网络状态事件处理函数*/
aw_local void __net_sts_evt_handler (aw_net_sts_evt_t *p_evt)
{
    uint32_t soft_reset = 0;

    if (p_evt->type ==  AW_NET_STS_EVT_TYPE_LINK_INET ||
            p_evt->type ==  AW_NET_STS_EVT_TYPE_LINK_SNET ||
            p_evt->type ==  AW_NET_STS_EVT_TYPE_LINK_DOWN) {
        if(p_evt->type ==  AW_NET_STS_EVT_TYPE_LINK_INET){
            net_is_up = AW_TRUE;
        }
        else if(p_evt->type ==  AW_NET_STS_EVT_TYPE_LINK_DOWN){
            net_is_up = AW_FALSE;
            if((gprs_id >= 0) && (socket_hd >= 0)){
                aw_kprintf("net link down, reset GPRS dev\r\n");
                reset_ok = AW_FALSE;
                /* 关闭gprs连接 */
                aw_gprs_close(gprs_id, socket_hd);
                /* 软复位*/
                aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_RESET, (aw_gprs_ioctl_args_t *)soft_reset);
                /* 关闭gprs设备 */
                aw_gprs_power_off(gprs_id);
                aw_kprintf("GPRS reset finish\r\n");
                reset_ok = AW_TRUE;
            }
        }

    }
}
#endif

/** AT命令测试*/
aw_local aw_err_t aw_gprs_at_test(int id, aw_gprs_ioctl_args_t *p_arg){
    aw_err_t ret;

    if(p_arg == NULL){
        return -AW_EINVAL;
    }

    /* 设置接收超时时间*/
    p_arg->at_trans.recv_tm = 5000;
    /* 设置AT命令*/
    p_arg->at_trans.p_at_cmd = (char *)aw_mem_alloc(32);
    if(p_arg->at_trans.p_at_cmd == NULL){
        ret = -AW_ENOMEM;
        goto __end;
    }
    memset(p_arg->at_trans.p_at_cmd, 0, 32);
    strcpy(p_arg->at_trans.p_at_cmd, "AT+CIMI\r");
    /* 设置期待接收到的数据*/
    p_arg->at_trans.p_recv_expect = (char *)aw_mem_alloc(32);
    if(p_arg->at_trans.p_recv_expect == NULL){
        ret = -AW_ENOMEM;
        goto __end;
    }
    memset(p_arg->at_trans.p_recv_expect, 0, 32);
    /* 不同的4G模块期待收到的信息有可能不一样*/
    strcpy(p_arg->at_trans.p_recv_expect, "\r\nOK\r\n");
    /* 设置接收缓存*/
    p_arg->at_trans.p_recv_buf = (char *)aw_mem_alloc(64);
    if(p_arg->at_trans.p_recv_buf == NULL){
        ret = -AW_ENOMEM;
        goto __end;
    }
    memset(p_arg->at_trans.p_recv_buf, 0, 64);
    /* 设置接收缓存大小*/
    p_arg->at_trans.bufsize = 64;

    ret = aw_gprs_ioctl(id, AW_GPRS_REQ_AT_TRANS, p_arg);
    if(ret == AW_OK){
        aw_kprintf("at test:\r\n");
        aw_kprintf("at cmd: %s\r\n", p_arg->at_trans.p_at_cmd);
        aw_kprintf("at recv: %s\r\n", p_arg->at_trans.p_recv_buf);
        ret = AW_OK;
        goto __end;
    }
__end:
    if(p_arg->at_trans.p_at_cmd){
        aw_mem_free(p_arg->at_trans.p_at_cmd);
    }
    if(p_arg->at_trans.p_recv_expect){
        aw_mem_free(p_arg->at_trans.p_recv_expect);
    }
    if(p_arg->at_trans.p_recv_buf){
        aw_mem_free(p_arg->at_trans.p_recv_buf);
    }
    return ret;
}

/** GPRS复位*/
aw_local int __gprs_reset (int argc, char* argv[], struct aw_shell_user *sh){
    uint32_t soft_reset;

    if(argc == 0){
        soft_reset = 0;
        aw_kprintf("start soft reset\r\n");
    }
    else if(argc == 1){
        if(strcmp(argv[0], "0") == 0){
            soft_reset = 0;
            aw_kprintf("start soft reset\r\n");
        }
        else if(strcmp(argv[0], "1") == 1){
            soft_reset = 1;
            aw_kprintf("start hardware reset\r\n");
        }
    }
    else{
        aw_kprintf("wrong param\r\n");
        return 0;
    }

    is_reset = AW_TRUE;
    reset_ok = AW_FALSE;
    /* 关闭gprs连接 */
    aw_gprs_close(gprs_id, socket_hd);
    /* 软复位*/
    aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_RESET, (aw_gprs_ioctl_args_t *)soft_reset);
    /* 关闭gprs设备 */
    aw_gprs_power_off(gprs_id);
    aw_kprintf("GPRS reset finish\r\n");
    reset_ok = AW_TRUE;
    return 0;
}

static void  __4g_log ( void         *p_color,
                        const char   *func_name,
                        int           line,
                        const char   *fmt, ...)
{

}

/** GPRS任务*/
void *__gprs_task(void *p_arg){
    aw_err_t    ret = AW_ERROR;
    int         i;
    uint8_t     p_buf[1000];
    char       *sms_ch_buf = "你好，我是AWorks！ ";
    char       *sms_en_buf = "hello, i am aworks!";
    aw_gprs_ioctl_args_t ioctl_arg;
     int  csq;
    struct aw_gprs_sms_buf  *p_sms = NULL;
#if AWBL_GPRS_USE_LWIP_2_X
    aw_net_sts_ctl_init_t    init;
    aw_net_sts_ctl_t        *p_ctl;
    aw_net_sts_dev_t        *p_dev;
    aw_net_sts_dev_init_t    dev_init;
#endif

    /* 申请短信结构体*/
    p_sms = aw_mem_alloc(sizeof(struct aw_gprs_sms_buf));
    if(p_sms == NULL){
        return 0;
    }
    memset(p_sms, 0, sizeof(struct aw_gprs_sms_buf));
    /* 申请短信号码缓存*/
    p_sms->p_num_buff = aw_mem_alloc(20);
    if(p_sms->p_num_buff  == NULL){
        return 0;
    }
    memset(p_sms->p_num_buff, 0, 20);
    p_sms->num_buff_len = 20;
    /* 申请短信内容缓存*/
    p_sms->p_sms_buff = aw_mem_alloc(60);
    if(p_sms->p_sms_buff  == NULL){
        return 0;
    }
    memset(p_sms->p_sms_buff, 0, 60);
    p_sms->sms_buff_len = 60;
__restart:
    /* 获取GPRS ID*/
    aw_kprintf("find GPRS dev...\r\n");
    gprs_id = aw_gprs_any_get(300);
    if (gprs_id < 0) {
        aw_kprintf("GPRS get dev id failed\r\n");
        return 0;
    }
    aw_kprintf("find GPRS dev, dev id %d\r\n", gprs_id);
    is_reset = AW_FALSE;

    /* 初始化GPRS*/
    ret = aw_gprs_power_on(gprs_id);
    if (ret != AW_OK) {
        aw_kprintf("aw_gprs_power_on error:%d\r\n", ret);
        return 0;
    }
    /* 获取4g模块版本号*/
    for(i = 0;i < 3;i++){
        ret = aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_VERSION_GET, &ioctl_arg);
        if(ret == AW_OK){
            aw_kprintf("GPRS version:%s\r\n",ioctl_arg.version);
            break;
        }
        aw_mdelay(100);
    }
    if(ret != AW_OK){
        aw_kprintf("get GPRS version failed\r\n");
    }

    /* 获取信号强度*/
    for(i = 0;i < 3;i++){
        ret = aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_CSQ_GET, (aw_gprs_ioctl_args_t *)&csq);
        if(ret == AW_OK){
            aw_kprintf("GPRS CSQ:%d\r\n",csq);
            break;
        }
        aw_mdelay(100);
    }
    if(ret != AW_OK){
        aw_kprintf("get GPRS CSQ failed\r\n");
    }

    /* 获取IMSI*/
    for(i = 0;i < 3;i++){
        ret = aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_IMSI_GET, &ioctl_arg);
        if(ret == AW_OK){
            aw_kprintf("GPRS IMSI:%s\r\n",ioctl_arg.imsi);
            break;
        }
        aw_mdelay(100);
    }
    if(ret != AW_OK){
        aw_kprintf("get GPRS IMSI failed\r\n");
    }
    /* 获取IMEI*/
    for(i = 0;i < 3;i++){
        ret = aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_IMEI_GET, &ioctl_arg);
        if(ret == AW_OK){
            aw_kprintf("GPRS IMEI:%s\r\n",ioctl_arg.imei);
            break;
        }
        aw_mdelay(100);
    }
    if(ret != AW_OK){
        aw_kprintf("get GPRS IMEI failed\r\n");
    }
    /* 获取短信中心号码*/
    for(i = 0;i < 3;i++){
        ret = aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_SCA_GET, &ioctl_arg);
        if(ret == AW_OK){
            aw_kprintf("GPRS SCA:%s\r\n",ioctl_arg.sca);
            break;
        }
        aw_mdelay(100);
    }
    if(ret != AW_OK){
        aw_kprintf("get GPRS SCA failed\r\n");
    }

    /* 获取SIM卡ICCID*/
    for(i = 0;i < 3;i++){
        ret = aw_gprs_ioctl(gprs_id, AW_GPRS_REQ_CCID_GET, &ioctl_arg);
        if(ret == AW_OK){
            aw_kprintf("GPRS SIM ICCID:%s\r\n",ioctl_arg.ccid);
            break;
        }
        aw_mdelay(100);
    }
    if(ret != AW_OK){
        aw_kprintf("get GPRS SIM ICCID failed\r\n");
    }

    /* AT命令发送测试*/
    aw_gprs_at_test(gprs_id, &ioctl_arg);

    /* 发送英文短信*/
    for(i = 0;i < 3;i++){
        ret = aw_gprs_sms_send(gprs_id, (uint8_t *)sms_en_buf, strlen(sms_en_buf), TEST_PHONE_NUM);
        if (ret != AW_OK) {
            aw_kprintf("aw_gprs_sms_send error:%d\r\n", ret);
            aw_mdelay(1000);
        }
        else{
            break;
        }
    }
    /* 发送中文短信*/
    for(i = 0;i < 3;i++){
        ret = aw_gprs_sms_send(gprs_id, (uint8_t *)sms_ch_buf, strlen(sms_ch_buf), TEST_PHONE_NUM);
        if (ret != AW_OK) {
            aw_kprintf("aw_gprs_sms_send error:%d\r\n", ret);
            aw_mdelay(1000);
        }
        else{
            break;
        }
    }
    aw_mdelay(1000);
    /* 读取短信*/
    for(i = 0;i < 3;i++){
        ret = aw_gprs_sms_recv(gprs_id, p_sms, 10000);
        if(ret != AW_OK){
            aw_kprintf("aw_gprs_sms_recv error:%d\r\n", ret);
            aw_mdelay(1000);
        }
        else{
            aw_kprintf("sms rev:%s from %s\r\n", p_sms->p_sms_buff, p_sms->p_num_buff);
            break;
        }
    }

#if AWBL_GPRS_USE_LWIP_2_X
    /* 初始化网络状态控制结构体*/
      memset(&init, 0, sizeof(init));
      init.pfn_log_out        = __4g_log;
      init.pfn_evt_handler    = __net_sts_evt_handler;
      p_ctl = aw_net_sts_ctl_create(&init);
      if(p_ctl == NULL){
          return 0;
      }
      /* 初始化网络状态设备结构体*/
      memset(&dev_init, 0, sizeof(dev_init));
      memcpy(dev_init.netif_name, "pp", 3);
      /* 检查时间*/
      dev_init.chk_ms          = 3 * 1000;
      dev_init.pfn_log_out     = NULL;
      /* 检查网络状态的IP地址*/
      dev_init.p_chk_inet_addr = TEST_SERVER_IP;
      dev_init.p_chk_snet_addr = TEST_SERVER_IP;
      /* 网络优先级(用于多网卡设备)*/
      dev_init.prioty          = 0;
      dev_init.type            = AW_NET_STS_DEV_TYPE_4G;
      dev_init.u._4g.pfn_event_notify = NULL;
      p_dev = aw_net_sts_dev_create(&dev_init);
      if(p_dev == NULL){
          return 0;
      }
      /* 添加网络状态监控*/
      ret = aw_net_sts_add(p_ctl, p_dev);
      if(ret != AW_OK){
          return 0;
      }
      /* 等待网络可用*/
      aw_kprintf("wait GPRS net available\r\n");
      while(net_is_up == AW_FALSE){
          aw_mdelay(500);
      }
#endif

    /* 建立GPRS连接 */
    aw_kprintf("connect to server...\r\n");
    socket_hd = aw_gprs_start(gprs_id, TEST_GPRS_MODE, TEST_SERVER_IP, TEST_SEEVER_PORT, 3000);
    if (socket_hd < 0) {
        aw_kprintf("aw_gprs_start error:%d\r\n", socket_hd);
        return 0;
    }
    aw_kprintf("connect to server success\r\n");
    while (is_reset == AW_FALSE) {

        /* 清空缓冲区 */
        memset(p_buf, '\0', sizeof(p_buf));

        /* 接收服务器端发送的数据，非阻塞 */
        ret = aw_gprs_recv(gprs_id, socket_hd, p_buf, sizeof(p_buf));
        if (ret > 0) {
            /* 向服务器端发送数据 */
            aw_gprs_send(gprs_id, socket_hd, p_buf, ret, 100);
        }
        /* 等待软复位*/
        aw_mdelay(1000);
    }
    while(reset_ok == AW_FALSE){
        aw_mdelay(1000);
    }
    goto __restart;

    return 0;
}


/*
 * \brief GPRS 网络通信例程入口
 * \return 无
 */
void demo_gprs_entry (void){
    aw_kprintf("GPRS demo test...\n");
    /* 注册GPRS软复位命令*/
    aw_local aw_const struct aw_shell_cmd g_shell_cmd_tbl[] = {
        {
            __gprs_reset,
            "gprs_reset",
            "reset gprs"
        },
    };
    aw_local struct aw_shell_cmd_list cl;

    (void) aw_shell_register_cmds(
               &cl,
               &g_shell_cmd_tbl[0],
               &g_shell_cmd_tbl[AW_NELEMENTS(g_shell_cmd_tbl)]);

    AW_TASK_DECL_STATIC(gprs_task, 1024 * 10);

    /* 初始化任务 */
    AW_TASK_INIT(gprs_task,             /* 任务实体 */
               "gprs_task",             /* 任务名字 */
               12,                      /* 任务优先级 */
               1024 * 10 ,              /* 任务堆栈大小 */
               __gprs_task,             /* 任务入口函数 */
               (void *)NULL);           /* 任务入口参数 */

    /* 启动任务 */
    AW_TASK_STARTUP(gprs_task);
}




