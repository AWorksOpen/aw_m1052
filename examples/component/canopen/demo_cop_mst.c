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
 * \brief CANopen ��վ��ʾ���̣����CANopen���İ�ʹ�ã�
 *
 * - �������裺
 *   1.��aw_prj_params.h�д����º궨�壺
 *     - AW_COM_CANOPEN
 *     - CAN�豸
 *     - GPT��ʱ��
 *   2.����xgate-cop10��վģ�飬��վ��վ�����ʾ�����Ϊ500kbps
 *   3.��xgate-demo��λ����������ڲ�����115200�����忴��վ���ã������ʼ����
 *   4.�򿪵��Դ��ڣ�������115200�����忴aworks���Դ������õĲ�����
 *
 * - ʵ������
 *   1.���Դ��ڻ᲻�ϵ������ӡ��Ϣ������յ��������Ļص���Ϣ�����յ�PDO���ݵ�
 *     �ص���Ϣ������PDO���ݵĴ�ӡ��Ϣ
 *   2.��xgate-demo��λ��������Կ���ouput��һֱ��ʾ�����Ƿ��͵�ʵʱ����, ��
 *     �����ʶ������ʾ��2018��5��1��,19ʱ25��20��
 *
 * \par Դ����
 * \snippet demo_cop_mst.c src_cop_mst
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-01  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup grp_demo_if_cop_mst
 * \copydoc demo_cop_mst.c
 */

/** [src_cop_mst] */
#include "aworks.h"
#include "aw_cop_mst.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "string.h"

#define __COP_MST_INST_NUM      0x00

#define __COP_MST_SLV_NODE_ID   0x01

static uint8_t  __g_cop_mst_slave_id_tab[] = {0x01, 0x02, 0x03};
static uint32_t __g_cop_mst_rpdo_id_tab[sizeof(__g_cop_mst_slave_id_tab)][4];
static uint32_t __g_cop_mst_tpdo_id_tab[sizeof(__g_cop_mst_slave_id_tab)][4];

static aw_bool_t __cop_mst_pdo_recv_callback (aw_cop_pdo_msg_t *p_msg);
static void      __cop_mst_emcc_event_callback (aw_cop_emcy_msg_t *p_emcy_msg);

#define __COP_MST_PROCESS_TASK_PRIO          3
#define __COP_MST_PROCESS_TASK_STACK_SIZE    4096
AW_TASK_DECL_STATIC(__cop_mst_process_task, __COP_MST_PROCESS_TASK_STACK_SIZE);
static void __cop_mst_process_task_entry (void *p_arg);

#define __COP_MST_PDO_TX_TASK_PRIO           4
#define __COP_MST_PDO_TX_TASK_STACK_SIZE     4096
AW_TASK_DECL_STATIC(__cop_mst_pdo_tx_task, __COP_MST_PDO_TX_TASK_STACK_SIZE);
static void __cop_mst_pdo_tx_task_entry (void *p_arg);

#define __COP_MST_PDO_RX_TASK_PRIO           5
#define __COP_MST_PDO_RX_TASK_STACK_SIZE     4096
AW_TASK_DECL_STATIC(__cop_mst_pdo_rx_task, __COP_MST_PDO_RX_TASK_STACK_SIZE);
static void __cop_mst_pdo_rx_task_entry (void *p_arg);

#define __COP_MST_NODE_STATUS_TASK_PRIO           6
#define __COP_MST_NODE_STATUS_TASK_STACK_SIZE     4096
AW_TASK_DECL_STATIC(
    __cop_mst_node_status_task, __COP_MST_PDO_RX_TASK_STACK_SIZE);
static void __cop_mst_node_status_task_entry (void *p_arg);

/******************************************************************************/
/* Э��ջ����ʹ����Ҫ����5������ */
/* step1: initialize CANopen stack */
/* step2: register node */
/* step3: register pdo */
/* step4: CANopen process task start */
/* step5: CANopen stack start */
void demo_cop_mst (void)
{
    aw_cop_mst_info_t mst_info;

    aw_cop_mst_node_info_t  node_info;

    uint32_t i, j;

    uint8_t  sdo_tempdata[]= {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint32_t sdo_len;
    
    uint8_t  slave_name[20];

    uint32_t rpdo_id_tab[] = {0x180, 0x280, 0x380, 0x480};
    uint32_t tpdo_id_tab[] = {0x200, 0x300, 0x400, 0x500};

    aw_cop_time_t cop_time;
    aw_cop_err_t  ret;

    /* step1: initialize CANopen stack */
    mst_info.baudrate = AW_COP_BAUD_500K;
    mst_info.node_id  = 127;

    mst_info.pfn_pdo_recv_callback   = __cop_mst_pdo_recv_callback;
    mst_info.pfn_emcc_event_callback = __cop_mst_emcc_event_callback;

    ret = aw_cop_mst_init(__COP_MST_INST_NUM, &mst_info);
    if (ret != AW_COP_ERR_NO_ERROR) {
        AW_INFOF(("aw_cop_mst_init() ret:%d\n", ret));
        aw_cop_mst_deinit(__COP_MST_INST_NUM);
        return;
    }
    
    for (i = 0; i < sizeof(__g_cop_mst_slave_id_tab); i++) {
	    if (__g_cop_mst_slave_id_tab[i] != AW_COP_NODE_ID_INVAILD) {
	        node_info.node_id      = __g_cop_mst_slave_id_tab[i];
	        node_info.check_mode   = AW_COP_MST_NODE_CHECK_WAY_HEARTBEAT;
	        node_info.check_period = 3000;
	        node_info.retry_factor = 3;

	        /* step2: register node */
	        ret = aw_cop_mst_add_node(__COP_MST_INST_NUM, &node_info);
	        if (ret != AW_COP_ERR_NO_ERROR) {
	            __g_cop_mst_slave_id_tab[i] = AW_COP_NODE_ID_INVAILD;
	        }
	    }
	}

    memset(__g_cop_mst_rpdo_id_tab, 0x00, sizeof(__g_cop_mst_rpdo_id_tab));
    memset(__g_cop_mst_tpdo_id_tab, 0x00, sizeof(__g_cop_mst_tpdo_id_tab));

    for (i = 0; i < AW_NELEMENTS(__g_cop_mst_slave_id_tab); i++) {

        if (__g_cop_mst_slave_id_tab[i] == AW_COP_NODE_ID_INVAILD) {
            continue;
        }

        for (j = 0; j < AW_NELEMENTS(__g_cop_mst_rpdo_id_tab[0]); j++) {

            __g_cop_mst_rpdo_id_tab[i][j] =
                rpdo_id_tab[j] + __g_cop_mst_slave_id_tab[i];

            /* step3: PDO register */
            ret = aw_cop_mst_input_pdo_install(
                __COP_MST_INST_NUM,
                __g_cop_mst_slave_id_tab[i],
                __g_cop_mst_rpdo_id_tab[i][j]);

            if (ret != AW_COP_ERR_NO_ERROR) {
                AW_INFOF(("aw_cop_mst_input_pdo_install()"\
                          " slave id:0x%x pdo id:0x%x ret:%d\n",
                          __g_cop_mst_slave_id_tab[i],
                          __g_cop_mst_rpdo_id_tab[i][j],
                          ret));
                __g_cop_mst_rpdo_id_tab[i][j] = 0;
            }
        }

        for (j = 0; j < AW_NELEMENTS(__g_cop_mst_tpdo_id_tab[0]); j++) {

            __g_cop_mst_tpdo_id_tab[i][j] =
                tpdo_id_tab[j] + __g_cop_mst_slave_id_tab[i];

            /* step3: PDO register */
            ret = aw_cop_mst_output_pdo_install(
                __COP_MST_INST_NUM,
                __g_cop_mst_slave_id_tab[i],
                __g_cop_mst_tpdo_id_tab[i][j]);

            if (ret != AW_COP_ERR_NO_ERROR) {
                AW_INFOF(("aw_cop_mst_output_pdo_install()"\
                          " slave id:0x%x pdo id:0x%x ret:%d\n",
                          __g_cop_mst_slave_id_tab[i],
                          __g_cop_mst_tpdo_id_tab[i][j],
                          ret));
                __g_cop_mst_tpdo_id_tab[i][j] = 0;
            }
        }
    }

    /* ��ʼ������COP���������� */
    AW_TASK_INIT(__cop_mst_process_task,            /* ����ʵ�� */
                 "__cop_mst_process_task",          /* �������� */
                 __COP_MST_PROCESS_TASK_PRIO,       /* �������ȼ� */
                 __COP_MST_PROCESS_TASK_STACK_SIZE, /* �����ջ��С */
                 __cop_mst_process_task_entry,      /* ������ں��� */
                 NULL);                             /* ������ڲ��� */

    /* step4: CANopen task process start */

    /* ��������__cop_mst_process_task */
    AW_TASK_STARTUP(__cop_mst_process_task);

    /* step5: CANopen stack start */
    ret = aw_cop_mst_start(__COP_MST_INST_NUM);
    if (ret != AW_COP_ERR_NO_ERROR) {
        AW_INFOF(("aw_cop_mst_start() ret:%d\n", ret));
        goto error;
    } else {
        aw_mdelay(100);
    }

    /* TEST: SDO �� */
    memset(slave_name, 0x00, sizeof(slave_name));

    ret = aw_cop_mst_sdo_upload(__COP_MST_INST_NUM,
                                __COP_MST_SLV_NODE_ID,
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

    /* TEST: SDO д */
    ret = aw_cop_mst_sdo_dwonload(__COP_MST_INST_NUM,
                                  __COP_MST_SLV_NODE_ID,
                                  0x2120,
                                  0x18,
                                  sdo_tempdata,
                                  4,
                                  1000);
    if (ret != AW_COP_ERR_NO_ERROR) {
        AW_INFOF(("aw_cop_mst_sdo_dwonload() ret:%d\n", ret));
    }

    /* TEST: ����ʱ��� */
    cop_time.year = 2018;
    cop_time.month = 5;
    cop_time.day = 1;
    cop_time.hour = 19;
    cop_time.minute = 25;
    cop_time.second = 20;
    cop_time.millisecond = 555;
    aw_cop_mst_timestamp_send(__COP_MST_INST_NUM, &cop_time);

    /* TEST: ����ͬ������ */
    aw_cop_mst_sync_prodcer_cfg(__COP_MST_INST_NUM, AW_COP_ID_SYNC, 5000);

    /* ��ʼ������__cop_mst_pdo_tx_task */
    AW_TASK_INIT(__cop_mst_pdo_tx_task,                    /* ����ʵ�� */
                 "__cop_mst_pdo_tx_task",                  /* �������� */
                 __COP_MST_PDO_TX_TASK_PRIO,               /* �������ȼ� */
                 __COP_MST_PDO_TX_TASK_STACK_SIZE,         /* �����ջ��С */
                 __cop_mst_pdo_tx_task_entry,              /* ������ں��� */
                 NULL);                                    /* ������ڲ��� */

    /* ��ʼ������__cop_mst_pdo_rx_task */
    AW_TASK_INIT(__cop_mst_pdo_rx_task,                    /* ����ʵ�� */
                 "__cop_mst_pdo_rx_task",                  /* �������� */
                 __COP_MST_PDO_RX_TASK_PRIO,               /* �������ȼ� */
                 __COP_MST_PDO_RX_TASK_STACK_SIZE,         /* �����ջ��С */
                 __cop_mst_pdo_rx_task_entry,              /* ������ں��� */
                 NULL);                                    /* ������ڲ��� */

    /* ��ʼ������__cop_mst_node_status_task */
    AW_TASK_INIT(__cop_mst_node_status_task,               /* ����ʵ�� */
                 "__cop_mst_node_status_task",             /* �������� */
                 __COP_MST_NODE_STATUS_TASK_PRIO,          /* �������ȼ� */
                 __COP_MST_NODE_STATUS_TASK_STACK_SIZE,    /* �����ջ��С */
                 __cop_mst_node_status_task_entry,         /* ������ں��� */
                 NULL);                                    /* ������ڲ��� */

    /* ��������__cop_mst_pdo_tx_task */
    AW_TASK_STARTUP(__cop_mst_pdo_tx_task);

    /* ��������__cop_mst_pdo_rx_task */
    AW_TASK_STARTUP(__cop_mst_pdo_rx_task);

    /* ��������__cop_mst_node_status_task */
    AW_TASK_STARTUP(__cop_mst_node_status_task);

    return;

error:
    ret = aw_cop_mst_stop(0);
    if (ret != AW_COP_ERR_NO_ERROR) {
        AW_INFOF(("aw_cop_mst_stop() ret:%d\n", ret));
    }

    aw_cop_mst_deinit(0);
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
        aw_cop_mst_process(__COP_MST_INST_NUM);
        aw_mdelay(1);
    }
}

/******************************************************************************/
static void __cop_mst_node_status_task_entry (void *p_arg)
{
    int i;

    aw_cop_err_t  ret;
    uint32_t      status;

    char slave_status[][25] = {
        "INIT",
        "DISCONNECT",
        "CONNECTING",
        "PREPARING",
        "STOP",
        "WORK",
        "PREWORK"
    };

    i = 0x00;

    while (1) {

        aw_mdelay(2000);

        if (__g_cop_mst_slave_id_tab[i] != AW_COP_NODE_ID_INVAILD) {

            /* TEST: ��ȡ��վ����Ĵ�վ�ڵ��״̬ */
            ret = aw_cop_mst_node_status_get(
                __COP_MST_INST_NUM, __g_cop_mst_slave_id_tab[i], &status);

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
                              __g_cop_mst_slave_id_tab[i]));
                    break;

                case AW_COP_MST_NODE_STATUS_PREWORK:
                    status = 6;
                    AW_INFOF(("slave status: %s, slave id:0x%x\n",
                              slave_status[status],
                              __g_cop_mst_slave_id_tab[i]));
                    break;

                default:
                    AW_INFOF(("slave status: %s\n", "UNKNOWN"));
                    break;
                }
            }
        }

        i++;
        if (i == AW_NELEMENTS(__g_cop_mst_slave_id_tab)) {
            i = 0;
        }
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

        /* TEST: ���������PDO���� */
        ret = aw_cop_mst_output_pdo_set(
            0,
            __COP_MST_SLV_NODE_ID,
            0x200 + __COP_MST_SLV_NODE_ID,
            send_data,
            8);
        if (ret != AW_COP_ERR_NO_ERROR) {
            AW_INFOF(("aw_cop_mst_output_pdo_set() ret:%d\n", ret));
        }

        aw_mdelay(1000);

        for (i = 0; i<sizeof(send_data); i++) {
            send_data[i] += 1;
        }

        ret = aw_cop_mst_output_pdo_set(
            0,
            __COP_MST_SLV_NODE_ID,
            0x300 + __COP_MST_SLV_NODE_ID,
            send_data,
            8);
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
    uint32_t   i, j, k;

    aw_cop_err_t ret;
    
    while (1) {

        for (i = 0; i < sizeof(__g_cop_mst_slave_id_tab); i++) {

            if (__g_cop_mst_slave_id_tab[i] == AW_COP_NODE_ID_INVAILD) {
                continue;
            }

            for (j = 0; j < AW_NELEMENTS(__g_cop_mst_rpdo_id_tab[0]); j++) {

                if (__g_cop_mst_rpdo_id_tab[i][j] == 0x00) {
                    continue;
                }

                recv_len = sizeof(recv_data);
                memset(recv_data, 0x00, recv_len);

                /* TEST: ��ȡ�����PDO���� */
                ret = aw_cop_mst_input_pdo_get(__COP_MST_INST_NUM,
                                               __g_cop_mst_slave_id_tab[i],
                                               __g_cop_mst_rpdo_id_tab[i][j],
                                               recv_data,
                                               &recv_len,
                                               1);

                if (ret == AW_COP_ERR_NO_ERROR) {
                    AW_INFOF(("receive pdo: node_id: 0x%x pdo id:0x%x data:",
                              __g_cop_mst_slave_id_tab[i],
                              __g_cop_mst_rpdo_id_tab[i][j]));
                    for (k = 0; k < recv_len; k++) {
                        AW_INFOF(("0x%x ", recv_data[k]));
                    }
                    AW_INFOF(("\n"));
                }
            }

            aw_mdelay(10);
        }
    }
} 

/** [src_cop_mst] */

/* end of file */
