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
 * \brief CANopen ��վ��ʾ����
 *
 * - �������裺
 *
 *   1. ��Ҫ���������ļ� aw_prj_params.h�ļ��д����º�:
 *      - AW_DEV_XXX_CAN1 (��Ӧƽ̨��CAN����)
 *      - AW_COM_CANFESTIVAL
 *   2. ����λ�����xgate-demo�����ڲ�����115200��
 *      ���忴��վ���������ã������ʼ����
 *   3. ct1��cr1������Ҫ���� ctm8251t�����շ��������ź�ת����
 *      ת������źŽ���xgate-cop10��վģ�飬������canh��canl��
 *      ��վ��վ�����ʾ�����Ϊ500kbps����վ��ַ127����վ��ַ1�����ɲ��뿪������
 *   4. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���Դ��ڻ᲻�ϵ������ӡ��Ϣ������յ��������Ļص���Ϣ�����յ�PDO���ݵ�
 *      �ص���Ϣ������PDO���ݵĴ�ӡ��Ϣ
 *   2. ��xgate-demo��λ��������Կ���ouput��һֱ��ʾ�����Ƿ��͵�ʵʱ����, ��
 *      �����ʶ������ʾ��2018��5��1��,19ʱ25��20��
 *
 * - ��ע��
 *   1. �����rs232����xgate-exal�����׼�ʱuart1�̽ӣ�
 *      ���ô��ڰ����xgate-exal�����׼��� ���ڰ�tx��rx��xgate-exal�������uart1
 *   2. xgate-exal�����׼��еĲ����ʺͽڵ���ϸ���������xgate-cop10�ֲᣬ
 *      �����ʺͽڵ����ô�����Ϣ��
 *
 * \par Դ����
 * \snippet demo_cop_mst.c src_cop_mst
 *
 * \internal
 * \par Modification History
 * - 1.01 19-06-06  anu, modify pdo tx and rx.
 * - 1.00 18-06-01  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_cop_mst CANopen ��վ��ʾ����
 * \copydoc demo_cop_mst.c
 */

/** [src_cop_mst] */
#include "aworks.h"
#include "aw_cop_mst.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "string.h"

/* ����SDO��ID�����������__g_cop_mst_node_id_tab */
#define __COP_MST_SDO_NODE_ID    0x01

/* �������PDO���ݵ�ID�����������__g_cop_mst_ouput_pdo_id_tab */
#define __COP_MST_OUTPUT_PDO_ID  0X201

/* ��������PDO���ݵ�ID�����������__g_cop_mst_input_pdo_id_tab */
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
/* Э��ջ����ʹ����Ҫ����5������ */
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
     * ֧�������rpdo�ķ�ʽ��Ҳ����ͨ���ص��ķ�ʽ����
     * ������2�����ܾ��򿪣�����Ҫע�ͼ���
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

    /* ��ʼ������COP���������� */
    AW_TASK_INIT(__cop_mst_process_task,            /* ����ʵ�� */
                 "__cop_mst_process_task",          /* �������� */
                 __COP_MST_PROCESS_TASK_PRIO,       /* �������ȼ� */
                 __COP_MST_PROCESS_TASK_STACK_SIZE, /* �����ջ��С */
                 __cop_mst_process_task_entry,      /* ������ں��� */
                 NULL);                             /* ������ڲ��� */

    /* step3: CANopen task process start */

    /* ��������__cop_mst_process_task */
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

    /* TEST: SDO �� */
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

    /* TEST: SDO д */
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

    /* TEST: ����ʱ��� */
    cop_time.year = 2018;
    cop_time.month = 5;
    cop_time.day = 1;
    cop_time.hour = 19;
    cop_time.minute = 25;
    cop_time.second = 20;
    cop_time.millisecond = 555;
    aw_cop_mst_timestamp_send(0x00, &cop_time);

    /* TEST: ����ͬ������ */
    aw_cop_mst_sync_prodcer_cfg(0x00, AW_COP_ID_SYNC, 5000);

    /* ��ʼ������__cop_mst_pdo_tx_task */
    AW_TASK_INIT(__cop_mst_pdo_tx_task,               /* ����ʵ�� */
                 "__cop_mst_pdo_tx_task",             /* �������� */
                 __COP_MST_PDO_TX_TASK_PRIO,          /* �������ȼ� */
                 __COP_MST_PDO_TX_TASK_STACK_SIZE,    /* �����ջ��С */
                 __cop_mst_pdo_tx_task_entry,         /* ������ں��� */
                 NULL);                               /* ������ڲ��� */

    /* ��ʼ������__cop_mst_pdo_rx_task */
    AW_TASK_INIT(__cop_mst_pdo_rx_task,               /* ����ʵ�� */
                 "__cop_mst_pdo_rx_task",             /* �������� */
                 __COP_MST_PDO_RX_TASK_PRIO,          /* �������ȼ� */
                 __COP_MST_PDO_RX_TASK_STACK_SIZE,    /* �����ջ��С */
                 __cop_mst_pdo_rx_task_entry,         /* ������ں��� */
                 NULL);                               /* ������ڲ��� */

    /* ��������__cop_mst_pdo_tx_task */
    AW_TASK_STARTUP(__cop_mst_pdo_tx_task);

    /* ��������__cop_mst_pdo_rx_task */
    AW_TASK_STARTUP(__cop_mst_pdo_rx_task);

    i = 0x00;

    while (1) {

        aw_mdelay(2000);

        if (__g_cop_mst_node_id_tab[i] != AW_COP_NODE_ID_INVAILD) {

            /* TEST: ��ȡ��վ����Ĵ�վ�ڵ��״̬ */
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

        /* TEST: ���������PDO���� */
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

        /* TEST: ��ȡ�����PDO���� */
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
