/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief CANopen 主站演示例程
 *
 * - 操作步骤：
 *
 *   1. 需要工程配置文件 aw_prj_params.h文件中打开以下宏:
 *      - AW_DEV_XXX_CAN1 (对应平台的CAN外设)
 *      - AW_COM_CANFESTIVAL
 *   2. 打开上位机软件xgate-demo，串口波特率115200，
 *      具体看从站波特率配置，点击开始测试
 *   3. ct1和cr1引脚需要连接 ctm8251t隔离收发器进行信号转换，
 *      转换后的信号接入xgate-cop10从站模块，即接入canh和canl，
 *      主站从站波特率均设置为500kbps，主站地址127，从站地址1，可由拨码开关设置
 *   4. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 调试串口会不断的输出打印信息，如接收到紧急报文回调信息，接收到PDO数据的
 *      回调信息，发送PDO数据的打印信息
 *   2. 在xgate-demo上位机软件可以看到ouput框一直显示着我们发送的实时数据, 在
 *      网络标识框上显示：2018年5月1日,19时25分20秒
 *
 * - 备注：
 *   1. 如果用rs232接入xgate-exal评估套件时uart1短接，
 *      若用串口板接入xgate-exal评估套件， 串口板tx和rx接xgate-exal评估板的uart1
 *   2. xgate-exal评估套件中的波特率和节点详细配置请参阅xgate-cop10手册，
 *      波特率和节点配置错误信息。
 *
 * \par 源代码
 * \snippet demo_cop_mst.c src_cop_mst
 *
 * \internal
 * \par Modification History
 * - 1.01 19-06-06  anu, modify pdo tx and rx.
 * - 1.00 18-06-01  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_cop_mst CANopen 主站演示例程
 * \copydoc demo_cop_mst.c
 */

/** [src_cop_mst] */
#include "aworks.h"
#include "aw_cop_mst.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "string.h"

/* 测试SDO的ID，必须存在于__g_cop_mst_node_id_tab */
#define __COP_MST_SDO_NODE_ID    0x01

/* 测试输出PDO数据的ID，必须存在于__g_cop_mst_ouput_pdo_id_tab */
#define __COP_MST_OUTPUT_PDO_ID  0X201

/* 测试输入PDO数据的ID，必须存在于__g_cop_mst_input_pdo_id_tab */
#define __COP_MST_INPUT_PDO_ID   0x181

#define __COP_MST_PDO_NODE_ID(pdo_id)  (pdo_id & 0x7F)

static uint8_t  __g_cop_mst_node_id_tab[] = {0x01, 0x02, 0x03, 0x04};
static uint32_t __g_cop_mst_input_pdo_id_tab[] = {0x181, 0x182, 0x183, 0x184};
static uint32_t __g_cop_mst_ouput_pdo_id_tab[] = {0x201, 0x202, 0x203, 0x204};

static aw_bool_t __cop_mst_pdo_recv_callback (aw_cop_pdo_msg_t *p_msg);
static void      __cop_mst_emcc_event_callback (aw_cop_emcy_msg_t *p_emcy_msg);

#define __COP_MST_PROCESS_TASK_PRIO          3
#define __COP_MST_PROCESS_TASK_STACK_SIZE    4096
AW_TASK_DECL_STATIC(__cop_mst_process_task, __COP_MST_PROCESS_TASK_STACK_SIZE);
static void __cop_mst_process_task_entry (void *p_arg);

#define __COP_MST_PDO_TX_TASK_PRIO          4
#define __COP_MST_PDO_TX_TASK_STACK_SIZE    4096
AW_TASK_DECL_STATIC(__cop_mst_pdo_tx_task, __COP_MST_PDO_TX_TASK_STACK_SIZE);
static void __cop_mst_pdo_tx_task_entry (void *p_arg);

#define __COP_MST_PDO_RX_TASK_PRIO         5
#define __COP_MST_PDO_RX_TASK_STACK_SIZE   4096
AW_TASK_DECL_STATIC(__cop_mst_pdo_rx_task, __COP_MST_PDO_RX_TASK_STACK_SIZE);
static void __cop_mst_pdo_rx_task_entry (void *p_arg);

/******************************************************************************/
/* 协议栈正常使用需要以下5个步骤 */
/* step1: initialize CANopen stack */
/* step2: register node */
/* step3: CANopen process task start */
/* step4: CANopen stack start */
/* step5: PDO register */
void demo_cop_mst (void)
{
    aw_cop_mst_info_t mst_info;

    aw_cop_mst_node_info_t  node_info;

    uint32_t i;

    uint8_t  sdo_tempdata[]= {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint32_t sdo_len, status;
    
    uint8_t  slave_name[20];
    char     slave_status[][25] = {
        "INIT",
        "DISCONNECT",
        "CONNECTING",
        "PREPARING",
        "STOP",
        "WORK",
        "PREWORK"
    };

    aw_cop_time_t cop_time;
    aw_cop_err_t  ret;

    /* step1: initialize CANopen stack */
    mst_info.baudrate = 500;
    mst_info.node_id  = 127;

    /*
     * 支持任务读rpdo的方式，也可以通过回调的方式发，
     * 本例程2个功能均打开，不需要注释即可
     */
    mst_info.pfn_pdo_recv_callback   = __cop_mst_pdo_recv_callback;
    mst_info.pfn_emcc_event_callback = __cop_mst_emcc_event_callback;

    ret = aw_cop_mst_init(0x00, &mst_info);
    if (ret != AW_COP_ERR_NO_ERROR) {
        AW_INFOF(("aw_cop_mst_init() ret:%d\n", ret));
        aw_cop_mst_deinit(0x00);
        return;
    }
    
    for (i = 0; i < sizeof(__g_cop_mst_node_id_tab); i++) {
        if (__g_cop_mst_node_id_tab[i] != AW_COP_NODE_ID_INVAILD) {
            node_info.node_id      = __g_cop_mst_node_id_tab[i];
            node_info.check_mode   = AW_COP_MST_NODE_CHECK_WAY_GUARDING;
            node_info.check_period = 3000;
            node_info.retry_factor = 3;

            /* step2: register node */
            ret = aw_cop_mst_add_node(0x00, &node_info);
            if (ret != AW_COP_ERR_NO_ERROR) {
                __g_cop_mst_node_id_tab[i] = AW_COP_NODE_ID_INVAILD;
            }
        }
    }

    /* 初始化任务COP主处理任务 */
    AW_TASK_INIT(__cop_mst_process_task,            /* 任务实体 */
                 "__cop_mst_process_task",          /* 任务名字 */
                 __COP_MST_PROCESS_TASK_PRIO,       /* 任务优先级 */
                 __COP_MST_PROCESS_TASK_STACK_SIZE, /* 任务堆栈大小 */
                 __cop_mst_process_task_entry,      /* 任务入口函数 */
                 NULL);                             /* 任务入口参数 */

    /* step3: CANopen task process start */

    /* 启动任务__cop_mst_process_task */
    AW_TASK_STARTUP(__cop_mst_process_task);

    /* step4: CANopen stack start */
    ret = aw_cop_mst_start(0x00);
    if (ret != AW_COP_ERR_NO_ERROR) {
        AW_INFOF(("aw_cop_mst_start() ret:%d\n", ret));
        goto error;
    } else {
        aw_mdelay(100);
    }

    for (i = 0; i < AW_NELEMENTS(__g_cop_mst_input_pdo_id_tab); i++) {

            /* step5: PDO register */
            ret = aw_cop_mst_input_pdo_install(
                0x00,
                __COP_MST_PDO_NODE_ID(__g_cop_mst_input_pdo_id_tab[i]),
                __g_cop_mst_input_pdo_id_tab[i]);

            if (ret != AW_COP_ERR_NO_ERROR) {
                AW_INFOF((
                    "aw_cop_mst_input_pdo_install()"\
                    " slave id:0x%x pdo id:0x%x ret:%d\n",
                    __COP_MST_PDO_NODE_ID(__g_cop_mst_input_pdo_id_tab[i]),
                    __g_cop_mst_input_pdo_id_tab[i],
                    ret));
                __g_cop_mst_input_pdo_id_tab[i] = 0;
            }
        }

        for (i = 0; i < AW_NELEMENTS(__g_cop_mst_ouput_pdo_id_tab); i++) {

            /* step5: PDO register */
            ret = aw_cop_mst_output_pdo_install(
                0x00,
                __COP_MST_PDO_NODE_ID(__g_cop_mst_ouput_pdo_id_tab[i]),
                __g_cop_mst_ouput_pdo_id_tab[i]);

            if (ret != AW_COP_ERR_NO_ERROR) {
                AW_INFOF((
                    "aw_cop_mst_output_pdo_install()"\
                    " slave id:0x%x pdo id:0x%x ret:%d\n",
                    __COP_MST_PDO_NODE_ID(__g_cop_mst_ouput_pdo_id_tab[i]),
                    __g_cop_mst_ouput_pdo_id_tab[i],
                    ret));
                __g_cop_mst_ouput_pdo_id_tab[i] = 0;
            }
        }

    /* TEST: SDO 读 */
    memset(slave_name, 0x00, sizeof(slave_name));

    ret = aw_cop_mst_sdo_upload(0x00,
                                __COP_MST_SDO_NODE_ID,
                                0x1008,
                                0x00,
                                slave_name,
                                &sdo_len,
                                1000);
    if (ret != AW_COP_ERR_NO_ERROR) {
        AW_INFOF(("aw_cop_mst_sdo_upload() ret:%d\n", ret));
    } else {
        AW_INFOF(("read slave name: %s\n", slave_name));
    }

    /* TEST: SDO 写 */
    ret = aw_cop_mst_sdo_dwonload(0x00,
                                  __COP_MST_SDO_NODE_ID,
                                  0x2120,
                                  0x18,
                                  sdo_tempdata,
                                  4,
                                  1000);
    if (ret != AW_COP_ERR_NO_ERROR) {
        AW_INFOF(("aw_cop_mst_sdo_dwonload() ret:%d\n", ret));
    }

    /* TEST: 发送时间戳 */
    cop_time.year = 2018;
    cop_time.month = 5;
    cop_time.day = 1;
    cop_time.hour = 19;
    cop_time.minute = 25;
    cop_time.second = 20;
    cop_time.millisecond = 555;
    aw_cop_mst_timestamp_send(0x00, &cop_time);

    /* TEST: 发送同步报文 */
    aw_cop_mst_sync_prodcer_cfg(0x00, AW_COP_ID_SYNC, 5000);

    /* 初始化任务__cop_mst_pdo_tx_task */
    AW_TASK_INIT(__cop_mst_pdo_tx_task,               /* 任务实体 */
                 "__cop_mst_pdo_tx_task",             /* 任务名字 */
                 __COP_MST_PDO_TX_TASK_PRIO,          /* 任务优先级 */
                 __COP_MST_PDO_TX_TASK_STACK_SIZE,    /* 任务堆栈大小 */
                 __cop_mst_pdo_tx_task_entry,         /* 任务入口函数 */
                 NULL);                               /* 任务入口参数 */

    /* 初始化任务__cop_mst_pdo_rx_task */
    AW_TASK_INIT(__cop_mst_pdo_rx_task,               /* 任务实体 */
                 "__cop_mst_pdo_rx_task",             /* 任务名字 */
                 __COP_MST_PDO_RX_TASK_PRIO,          /* 任务优先级 */
                 __COP_MST_PDO_RX_TASK_STACK_SIZE,    /* 任务堆栈大小 */
                 __cop_mst_pdo_rx_task_entry,         /* 任务入口函数 */
                 NULL);                               /* 任务入口参数 */

    /* 启动任务__cop_mst_pdo_tx_task */
    AW_TASK_STARTUP(__cop_mst_pdo_tx_task);

    /* 启动任务__cop_mst_pdo_rx_task */
    AW_TASK_STARTUP(__cop_mst_pdo_rx_task);

    i = 0x00;

    while (1) {

        aw_mdelay(2000);

        if (__g_cop_mst_node_id_tab[i] != AW_COP_NODE_ID_INVAILD) {

            /* TEST: 读取主站管理的从站节点的状态 */
            ret = aw_cop_mst_node_status_get(
                0x00, __g_cop_mst_node_id_tab[i], &status);

            if (ret == AW_COP_ERR_NO_ERROR) {

                switch (status) {
                case AW_COP_MST_NODE_STATUS_INIT:
                case AW_COP_MST_NODE_STATUS_DISCONNECT:
                case AW_COP_MST_NODE_STATUS_CONNECTING:
                case AW_COP_MST_NODE_STATUS_PREPARING:
                case AW_COP_MST_NODE_STATUS_STOP:
                case AW_COP_MST_NODE_STATUS_WORK:
                    AW_INFOF(("slave status: %s, slave id:0x%x\n",
                              slave_status[status],
                              __g_cop_mst_node_id_tab[i]));
                    break;

                case AW_COP_MST_NODE_STATUS_PREWORK:
                    status = 6;
                    AW_INFOF(("slave status: %s, slave id:0x%x\n",
                              slave_status[status],
                              __g_cop_mst_node_id_tab[i]));
                    break;

                default:
                    AW_INFOF(("slave status: %s, slave id:0x%x\n", "UNKNOWN",
                              __g_cop_mst_node_id_tab[i]));
                    break;
                }
            }
        }

        i++;
        if (i == AW_NELEMENTS(__g_cop_mst_node_id_tab)) {
            i = 0;
        }
    }

error:
    ret = aw_cop_mst_stop(0);
    if (ret != AW_COP_ERR_NO_ERROR) {
        AW_INFOF(("aw_cop_mst_stop() ret:%d\n", ret));
    }

    aw_cop_mst_deinit(0);
    while(1);
}

/******************************************************************************/
static aw_bool_t __cop_mst_pdo_recv_callback (aw_cop_pdo_msg_t *p_msg)
{
    AW_INFOF(("recv pdo callback\n"));
    return AW_TRUE;
}

/******************************************************************************/
static void __cop_mst_emcc_event_callback (aw_cop_emcy_msg_t *p_emcy_msg)
{
    AW_INFOF(("emcc event callback\n"));
}

/******************************************************************************/
static void __cop_mst_process_task_entry (void *p_arg)
{
    while (1) {
        aw_cop_mst_process(0);
        aw_mdelay(1);
    }
}

/******************************************************************************/
static void __cop_mst_pdo_tx_task_entry (void *p_arg)
{
    uint8_t  send_data[8];
    uint32_t i;

    aw_cop_err_t ret;
    
    memset(send_data, 0, sizeof(send_data));
    for (i = 0; i<sizeof(send_data); i++) {
        send_data[i] += i;
    }

    while (1) {

        for (i = 0; i<sizeof(send_data); i++) {
            send_data[i] += 1;
        }

        /* TEST: 设置输出的PDO报文 */
        ret = aw_cop_mst_output_pdo_set(
            0,
            __COP_MST_PDO_NODE_ID(__COP_MST_OUTPUT_PDO_ID),
            __COP_MST_OUTPUT_PDO_ID,
            send_data,
            sizeof(send_data));
        if (ret != AW_COP_ERR_NO_ERROR) {
            AW_INFOF(("aw_cop_mst_output_pdo_set() ret:%d\n", ret));
        }

        aw_mdelay(1000);
    }
}

/******************************************************************************/
static void __cop_mst_pdo_rx_task_entry (void *p_arg)
{
    uint8_t    recv_data[8];
    uint32_t   recv_len = 0;
    uint32_t   i;

    aw_cop_err_t ret;
    
    while (1) {

        recv_len = sizeof(recv_data);
        memset(recv_data, 0x00, recv_len);

        /* TEST: 获取输入的PDO报文 */
        ret = aw_cop_mst_input_pdo_get(
            0x00,
            __COP_MST_PDO_NODE_ID(__COP_MST_INPUT_PDO_ID),
            __COP_MST_INPUT_PDO_ID,
            recv_data,
            &recv_len,
            10);

        if (ret == AW_COP_ERR_NO_ERROR) {
            AW_INFOF(("receive pdo: node_id: 0x%x pdo id:0x%x data:",
                      __COP_MST_PDO_NODE_ID(__COP_MST_INPUT_PDO_ID),
                      __COP_MST_INPUT_PDO_ID));
            for (i = 0; i < recv_len; i++) {
                AW_INFOF(("0x%x ", recv_data[i]));
            }
            AW_INFOF(("\n"));
        }
    }
} 

/** [src_cop_mst] */

/* end of file */
