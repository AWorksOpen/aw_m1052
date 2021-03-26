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
 * \brief CANfestival 主站演示例程
 *
 * - 操作步骤：
 *   1.接上xgate-cop10从站模块，主站从站波特率均设置为500kbps
 *   2.打开xgate-demo上位机软件，串口波特率115200，具体看从站配置，点击开始测试
 *   3.打开调试串口，波特率115200，具体看aworks调试串口设置的波特率
 *
 * - 实验现象：
 *   1.RPDF接收到数据，每隔2s钟调试串口会不断的输出打印信息
 *   2.在xgate-demo上位机软件可以看到ouput框一直显示着我们发送的实时数据
 *
 * \par 源代码
 * \snippet demo_cop_mst.c src_cop_mst
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-01  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_cop_mst
 * \copydoc demo_cop_mst.c
 */

/** [src_cop_mst] */
#include "aworks.h"                   /* 此头文件必须被首先包含 */
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_sem.h"
#include "canfestival.h"

#undef AW_INFOF
#define AW_INFOF(x)  aw_kprintf("COP0: "); aw_kprintf x

#define __COP_MST_MST_NODE_ID   127  /**< \brief 主机ID */
#define __COP_MST_SLV_NODE_ID   1    /**< \brief 从机ID */

#define __COP_MST_SYNC_ID       0X80 /**< \brief 同步报文ID */

#define __COP_MST_PDO_TX_TASK_PRIO          4
#define __COP_MST_PDO_TX_TASK_STACK_SIZE    4096
AW_TASK_DECL_STATIC(__cop_mst_pdo_tx_task, __COP_MST_PDO_TX_TASK_STACK_SIZE);
static void __cop_mst_pdo_tx_task_entry (void *p_arg);

#define __COP_MST_PDO_RX_TASK_PRIO          5
#define __COP_MST_PDO_RX_TASK_STACK_SIZE    4096
AW_TASK_DECL_STATIC(__cop_mst_pdo_rx_task, __COP_MST_PDO_RX_TASK_STACK_SIZE);
static void __cop_mst_pdo_rx_task_entry (void *p_arg);

#define __COP_MST_CHECK_STATUS_TASK_PRIO         5
#define __COP_MST_CHECK_STATUS_TASK_STACK_SIZE   4096
AW_TASK_DECL_STATIC(
    __cop_mst_check_status_task, __COP_MST_CHECK_STATUS_TASK_STACK_SIZE);
static void __cop_mst_check_status_task_entry (void *p_arg);

AW_SEMB_DECL_STATIC(__cop_mst_pdo_rx_semb);

extern CO_Data aworkstest_Data;

/******************************************************************************/
static UNS32 __cop_mst_pdo_recv_callback (
    CO_Data          *p_data,
    const indextable *p_unsused_indextable,
    UNS8              unsused_bSubindex)
{
#if 0 
    AW_INFOF("cop mst pdo recv callback\n");
#endif
    AW_SEMB_GIVE(__cop_mst_pdo_rx_semb);
    return 0;
}

static UNS8 __cop_mst_sdo_upload (UNS8  nodeId,
                                  UNS16 index,
                                  UNS8  subIndex,
                                  UNS8  dataType,
                                  void *p_data,
                                  UNS8 *p_size)
{
    UNS32 abortCode = 0;
    UNS8  res = SDO_UPLOAD_IN_PROGRESS;

    /* Read SDO */
    UNS8 err = readNetworkDict(&aworkstest_Data,
                               nodeId,
                               index,
                               subIndex,
                               dataType);
    if (err) {
        return 0xFF;
    }

    AW_FOREVER {
        res = getReadResultNetworkDict(&aworkstest_Data,
                                       nodeId,
                                       p_data,
                                       p_size,
                                       &abortCode);
        if (res != SDO_UPLOAD_IN_PROGRESS) {
            break;
        }

        aw_mdelay(1);
    }

    closeSDOtransfer(&aworkstest_Data, nodeId, SDO_CLIENT);
    if (res == SDO_FINISHED) {
        return 0;
    }

    return 0xFF;
}

static UNS8 __cop_mst_sdo_download (UNS8  nodeId,
                                    UNS16 index,
                                    UNS8  subIndex,
                                    UNS8  count,
                                    UNS8  dataType,
                                    void *p_data)
{
    UNS32 abortCode = 0;
    UNS8 res = SDO_DOWNLOAD_IN_PROGRESS;

    /* write SDO */
    UNS8 err = writeNetworkDict(&aworkstest_Data,
                                nodeId,
                                index,
                                subIndex,
                                count,
                                dataType,
                                p_data);
    if (err) {
        return 0xFF;
    }

    AW_FOREVER {
        res = getWriteResultNetworkDict(&aworkstest_Data, nodeId, &abortCode);
        if (res != SDO_DOWNLOAD_IN_PROGRESS) {
            break;
        }
        aw_mdelay(1);
    }

    closeSDOtransfer(&aworkstest_Data, nodeId, SDO_CLIENT);
    if (res == SDO_FINISHED) {
        return 0;
    }

    return 0xFF;
}

static UNS8 __cop_mst_tpdo_set (UNS32 cobId, UNS16 index)
{
    UNS8    res;

    res = buildPDO(&aworkstest_Data, index);
    if (res == OD_SUCCESSFUL) {
        res = PDOmGR(&aworkstest_Data, cobId);
    }

    return res;
}

void demo_cop_mst (void)
{
    UNS32   ret;
    UNS8    size;
    char    device_name[64] = "";
    UNS8    node_id = __COP_MST_SLV_NODE_ID;

    UNS32   RPDO1_COBID = 0x0185;
    UNS32   RPDO2_COBID = 0x0285;
    UNS8    rsize = sizeof(UNS32);

    /* modify Client SDO 1 Parameter */
    UNS32   COB_ID_Client_to_Server_Transmit_SDO = 0x600 + node_id;
    UNS32   COB_ID_Server_to_Client_Receive_SDO = 0x580 + node_id;
    UNS32   Node_ID_of_the_SDO_Server = node_id;
    UNS8    ExpectedSize = sizeof(UNS32);
    uint8_t sdo_data = 0xF8;

    CAN_PORT    p_fd;

    s_BOARD MasterBoard = { "0", "1000K" };

    /* 
     * 安装CANfestival 流程图canOpen 要比StartTimerLoop先初始化
     * 在canfestival的can驱动中提供了互斥锁给定时器使用 
     */
    p_fd = canOpen(&MasterBoard, &aworkstest_Data);
    if (p_fd == NULL) {
        AW_INFOF(("start canopen error\n"));
        return;
    } else {
        aw_mdelay(1);
    }

    AW_SEMB_INIT(__cop_mst_pdo_rx_semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    StartTimerLoop(NULL);

    /* Defining the node Id */
    setNodeId(&aworkstest_Data, __COP_MST_MST_NODE_ID);

    /******************************* 设置对象词典 ******************************/

    setODentry(&aworkstest_Data, /*CO_Data* d*/
               0x1400,           /*UNS16 index*/
               0x01,             /*UNS8 subind*/
               &RPDO1_COBID,     /*void * pSourceData,*/
               &rsize,           /* UNS8 * pExpectedSize*/
               RW);              /* UNS8 checkAccess */

    setODentry(&aworkstest_Data, /*CO_Data* d*/
               0x1401,           /*UNS16 index*/
               0x01,             /*UNS8 subind*/
               &RPDO2_COBID,     /*void * pSourceData,*/
               &rsize,           /* UNS8 * pExpectedSize*/
               RW);              /* UNS8 checkAccess */

    ret = setODentry(&aworkstest_Data,
                     0x1280,
                     1,
                     &COB_ID_Client_to_Server_Transmit_SDO,
                     &ExpectedSize,
                     RW);
    if (ret != OD_SUCCESSFUL) {
        AW_INFOF(("ERROR: Object dictionary access failed\n"));
        return;
    }

    ret = setODentry(&aworkstest_Data,
                     0x1280,
                     2,
                     &COB_ID_Server_to_Client_Receive_SDO,
                     &ExpectedSize,
                     RW);
    if (ret != OD_SUCCESSFUL) {
        AW_INFOF(("ERROR: Object dictionary access failed\n"));
        return;
    }

    ret = setODentry(&aworkstest_Data,
                     0x1280,
                     3,
                     &Node_ID_of_the_SDO_Server,
                     &ExpectedSize,
                     RW);
    if (ret != OD_SUCCESSFUL) {
        AW_INFOF(("ERROR: Object dictionary access failed\n"));
        return;
    }

    /******************************** 设置回调 *********************************/

    RegisterSetODentryCallBack(
        &aworkstest_Data, 0x2000, 0, &__cop_mst_pdo_recv_callback);

    /**************************************************************************/

    /* init */
    setState(&aworkstest_Data, Initialisation);

    /* Put the master in operational mode */
    setState(&aworkstest_Data, Operational);

    /* Ask slave node to go in operational mode */
    masterSendNMTstateChange(&aworkstest_Data, 0, NMT_Start_Node);
    AW_INFOF(("Starting node %d (%xh) ...\n", (int) node_id, (int) node_id));

    /* TEST: SDO 读 */
    size = sizeof(device_name);
    ret = __cop_mst_sdo_upload(
        node_id,
        0x1008,
        0,
        visible_string,
        device_name,
        &size);

    if (ret != 0) {
        AW_INFOF(("__cop_mst_sdo_upload() ret:%d\n", ret));
    } else {
        AW_INFOF(("read slave name: %s\n", device_name));
    }

    /* TEST: SDO 写 */
    ret = __cop_mst_sdo_download(node_id, 0x2000, 3, 1, uint8, &sdo_data);
    if (ret != 0) {
        AW_INFOF(("__cop_mst_sdo_download() ret:%d\n", ret));
    }

    /* TEST: 发送同步报文 */
    ret = sendSYNC(&aworkstest_Data, __COP_MST_SYNC_ID);
    if (ret != 0) {
        AW_INFOF(("sendSYNC() ret:%d\n", ret));
    }

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

    /* 初始化任务__cop_mst_check_status_task */
    AW_TASK_INIT(__cop_mst_check_status_task,               /* 任务实体 */
                 "__cop_mst_check_status_task",             /* 任务名字 */
                 __COP_MST_CHECK_STATUS_TASK_PRIO,          /* 任务优先级 */
                 __COP_MST_CHECK_STATUS_TASK_STACK_SIZE,    /* 任务堆栈大小 */
                 __cop_mst_check_status_task_entry,         /* 任务入口函数 */
                 NULL);                                     /* 任务入口参数 */

    /* 启动任务__cop_mst_pdo_tx_task */
    AW_TASK_STARTUP(__cop_mst_pdo_tx_task);

    /* 启动任务__cop_mst_pdo_rx_task */
    AW_TASK_STARTUP(__cop_mst_pdo_rx_task);

    /* 启动任务__cop_mst_check_status_task */
    AW_TASK_STARTUP(__cop_mst_check_status_task);

}

/******************************************************************************/
static void __cop_mst_pdo_tx_task_entry (void *p_arg)
{
    UNS32    ret;
    UNS8     size = 0x00;
    uint32_t data = 0x00;

    AW_FOREVER {

        aw_mdelay(200);

        data++;
        size = sizeof(uint32_t);
        setODentry(&aworkstest_Data, /*CO_Data* d*/
                   0x2004, /* UNS16 index*/
                   0x00,   /* UNS8 subind*/
                   &data,  /* void * pSourceData,*/
                   &size,  /* UNS8 * pExpectedSize*/
                   RW);    /* UNS8 checkAccess */

        data++;
        size = sizeof(uint32_t);
        setODentry(&aworkstest_Data, /*CO_Data* d*/
                   0x2005, /* UNS16 index*/
                   0x00,   /* UNS8 subind*/
                   &data,  /* void * pSourceData,*/
                   &size,  /* UNS8 * pExpectedSize*/
                   RW);    /* UNS8 checkAccess */

        data++;
        size = sizeof(uint8_t);
        setODentry(&aworkstest_Data, /*CO_Data* d*/
                   0x2006, /* UNS16 index*/
                   0x00,   /* UNS8 subind*/
                   &data,  /* void * pSourceData,*/
                   &size,  /* UNS8 * pExpectedSize*/
                   RW);    /* UNS8 checkAccess */

        data++;
        size = sizeof(uint8_t);
        setODentry(&aworkstest_Data, /*CO_Data* d*/
                   0x2007, /* UNS16 index*/
                   0x00,   /* UNS8 subind*/
                   &data,  /* void * pSourceData,*/
                   &size,  /* UNS8 * pExpectedSize*/
                   RW);    /* UNS8 checkAccess */

        /* TEST: 设置输出的PDO报文 */
        ret = __cop_mst_tpdo_set(0x217, 0x1800);
        if (ret != 0) {
            AW_INFOF(("__cop_mst_tpdo_set() ret:%d\n", ret));
        }

        aw_mdelay(200);

        ret = __cop_mst_tpdo_set(0x317, 0x1801);
        if (ret != 0) {
            AW_INFOF(("__cop_mst_tpdo_set() ret:%d\n", ret));
        }
    }
}

/******************************************************************************/
static void __cop_mst_pdo_rx_task_entry (void *p_arg)
{
    UNS8     size = 0x00;
    UNS8     len  = 0x00;
    uint32_t data = 0x00;

    AW_FOREVER {

        AW_SEMB_TAKE(__cop_mst_pdo_rx_semb, AW_SEM_WAIT_FOREVER);

        data = 0;
        size = sizeof(uint32_t);
        getODentry(&aworkstest_Data, /*CO_Data* d*/
                   0x2000, /* UNS16 index*/
                   0x00,   /* UNS8 subind*/
                   &data,  /* void * pSourceData */
                   &size,  /* UNS8 * pExpectedSize*/
                   &len,   /* UNS8 * pDataType */
                   RW);    /* UNS8 checkAccess */
        AW_INFOF(("od: 0x%x\n", data));

        data = 0;
        size = sizeof(uint32_t);
        getODentry(&aworkstest_Data, /*CO_Data* d*/
                   0x2001, /* UNS16 index*/
                   0x00,   /* UNS8 subind*/
                   &data,  /* void * pSourceData,*/
                   &size,  /* UNS8 * pExpectedSize*/
                   &len,   /* UNS8 * pDataType */
                   RW);    /* UNS8 checkAccess */
        AW_INFOF(("od: 0x%x\n", data));

        data = 0;
        size = sizeof(uint8_t);
        getODentry(&aworkstest_Data, /*CO_Data* d*/
                   0x2002, /* UNS16 index*/
                   0x00,   /* UNS8 subind*/
                   &data,  /* void * pSourceData,*/
                   &size,  /* UNS8 * pExpectedSize*/
                   &len,   /* UNS8 * pDataType */
                   RW);    /* UNS8 checkAccess */
        AW_INFOF(("od: 0x%x\n", data));

        data = 0;
        size = sizeof(uint8_t);
        getODentry(&aworkstest_Data, /*CO_Data* d*/
                   0x2003, /* UNS16 index*/
                   0x00,   /* UNS8 subind*/
                   &data,  /* void * pSourceData,*/
                   &size,  /* UNS8 * pExpectedSize*/
                   &len,   /* UNS8 * pDataType */
                   RW);    /* UNS8 checkAccess */
        AW_INFOF(("od: 0x%x\n", data));

        aw_mdelay(2000);
    }
}

/******************************************************************************/
static void __cop_mst_check_status_task_entry (void *p_arg)
{
    UNS8    node_id = __COP_MST_SLV_NODE_ID;

    char slave_status[][25] = {
        "Initialisation",
        "Disconnected",
        "Connecting",
        "Preparing",
        "Stopped",
        "Operational",
        "Pre_operational",
        "Unknown_state"
    };

    e_nodeState last_state;
    e_nodeState now_state;

    AW_FOREVER {

        /* TEST: 读取主站管理的从站节点的状态 */

        last_state = getNodeState(&aworkstest_Data, node_id);

        /* request node state */
        masterRequestNodeState(&aworkstest_Data, node_id);
        aw_mdelay(2000);

        now_state = getNodeState(&aworkstest_Data, node_id);
        aw_mdelay(100);

        if (last_state == now_state) {
            continue;
        }

        last_state = now_state;

        switch (now_state) {
        case Initialisation:
        case Disconnected:
        case Connecting:
/*        case Preparing: */
        case Stopped:
        case Operational:
            AW_INFOF(("slave status: %s, slave id:0x%x\n",
                      slave_status[now_state],
                      node_id));
            break;

        case Pre_operational:
            AW_INFOF(("slave status: %s, slave id:0x%x\n",
                      "Pre_operational",
                      node_id));
            masterSendNMTstateChange(&aworkstest_Data, node_id, NMT_Start_Node);
            break;

        default:
            AW_INFOF(("slave status: %s, slave id:0x%x\n",
                      "Unknown_state",
                      node_id));
            break;
        }
    }
}

/** [src_cop_mst] */

/* end of file */


