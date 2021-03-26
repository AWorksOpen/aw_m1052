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
 * \brief ����CAN��������ʾ����
 * \note  ��������Ϊ�����ƶˣ����ƶ˿����� demo_can_master.c��
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      AW_DEV_GPIO_LED
 *      AW_COM_CONSOLE��
 *      ��Ӧƽ̨��CAN��UART�豸��
 *      ����Ҫ��������CAN�豸�����޸������е�CANͨ����ʹ��aw_prj_params.hͷ�ļ��ж�Ӧ��CAN�豸��
 *   2. ����CAN��������ŵ�CAN�շ��� RX=>RX TX=>TX��
 *      �շ����ٽӵ���ҪͨѶ���豸��ͨ��CAN���շ����� H=>H L=>L��
 *   3. �򿪴��ڵ����նˣ�������115200-8-N-1��
 *   4. ���������ط���LED��ʹ��
 *
 * - ʵ������
 *   1. �յ�LED���Ʊ��ģ�DATAÿbit����һ����״̬�����豸��ر����е�
 *      (��׼֡ ID=0X01 DATA=0X00 0X00 0X00 0X00 0X00 0X00 0X00 0X00)��
 *   2. �յ�LED���Ʊ��ģ�DATAÿbit����һ����״̬�����豸������е�
 *      (��׼֡ ID=0X01 DATA=0XFF 0XFF 0XFF 0XFF 0XFF 0XFF 0XFF 0XFF)��
 *   3. ��������Ʊ��ĺ���Ҫ�ظ�Ӧ����
 *      (��׼֡ ID=0X81 DATA=���ݿ��Ʊ��Ļ���)��
 *   4. ���ڴ�ӡ��ص�����Ϣ��
 *
 * \par Դ����
 * \snippet demo_can_slave.c src_can_slave
 *
 * \internal
 * \par Modification History
 * - 1.00 18-09-18  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_can_slave ����CAN������(�Ի�)
 * \copydoc demo_can_slave.c
 */

/** [src_can_slave] */

#include "aworks.h"
#include "string.h"
#include "aw_can.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_led.h"
#include "aw_vdebug.h"
#include "aw_rngbuf.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "aw_prj_params.h"
#include "aw_demo_config.h"

#define __CAN_CHN                  DE_CAN_CH    /* CANͨ�� */
#define __CAN_RECV_MSG_TIMEOUT     10           /* CAN���Ľ��ճ�ʱʱ�� */
#define __CAN_SEND_MSG_TIMEOUT     300          /* CAN���ķ��ͳ�ʱʱ�� */
#define __CAN_MSG_CNT              10           /* CAN��Ϣ���� */

#define __CAN_RECV_TASK_STACK_SIZE 2048         /* CAN���Ľ��������ջ��С */
#define __CAN_SEND_TASK_STACK_SIZE 2048         /* CAN���ķ��������ջ��С */
#define __CAN_RECV_TASK_PRIO       12           /* CAN���Ľ����������ȼ� */
#define __CAN_SEND_TASK_PRIO       13           /* CAN���ķ����������ȼ� */

#define __CAN_MSG_ID_LED           0X01         /* ��ϢID��LED���ƹ����� */
#define __CAN_MSG_ID_LED_ACK       0X81         /* ��ϢID��LED����Ӧ������ */

#undef AW_INFOF
#define AW_INFOF(x) aw_kprintf("CAN%d : ", __CAN_CHN); aw_kprintf x

/* CAN����״̬ */
static volatile aw_can_bus_status_t __g_can_bus_status = AW_CAN_BUS_STATUS_OK;

static aw_can_baud_param_t __g_can_btr = AW_CFG_CAN_BTR_500K;

AW_RNGBUF_SPACE_DEFINE(
    __g_can_msg_rngbuf_space, aw_can_std_msg_t, __CAN_MSG_CNT);

static struct aw_rngbuf __g_can_msg_rngbuf = {
    0,
    0,
    AW_RNGBUF_SPACE_SIZE(__g_can_msg_rngbuf_space),
    (char *)AW_RNGBUF_SPACE_GET(__g_can_msg_rngbuf_space)
};

AW_SEMB_DECL_STATIC(__g_can_msg_semb);

/******************************************************************************/
static uint32_t __can_timestamp_get_cb (uint32_t timestamp);
static aw_bool_t __can_err_sta_cb (
    int chn, aw_can_bus_err_t err, aw_can_bus_status_t status);

static aw_can_app_callbacks_t __g_can_app_callback_fun = {
    __can_timestamp_get_cb,
    NULL,
    NULL,
    __can_err_sta_cb,
    NULL
};

/**
 * \brief ʱ�����ȡ�Ļص�����
 *
 * \param[in] timestamp ��ǰ���ĵ�ʱ���
 *
 * \return �û��޸ĺ��ʱ���
 */
static uint32_t __can_timestamp_get_cb (uint32_t timestamp)
{
    return timestamp;
}

/**
 * \brief ����״̬�Ļص�����
 *
 * \param[in] chn ͨ����
 * \param[in] err CAN ���ߵĴ�������
 * \param[in] status CAN ���ߵ�״̬
 *
 * \retval AW_TRUE  ��������Ϣ������rngbuf
 * \retval AW_FALSE ������Ϣ������rngbuf
 */
static aw_bool_t __can_err_sta_cb (
    int chn, aw_can_bus_err_t err, aw_can_bus_status_t status)
{
    aw_can_err_cnt_t  can_err_reg;

    aw_can_err_cnt_get(chn, &can_err_reg);
    if ((can_err_reg.tx_err_cnt > 127) || (can_err_reg.rx_err_cnt > 127)) {
        aw_can_err_cnt_clr(chn);
    }

    __g_can_bus_status = status;

    return AW_TRUE; /* ��������ֵ����������Ϣ�Ƿ����rngbuf,AW_TRUE:������,AW_FALSE:���� */
}

/**
 * \brief CAN��ʼ��
 *
 * \param[in] ��
 *
 * \return AW_CAN_ERR_NONE��������������������ο��������
 */
static aw_can_err_t __can_startup (void)
{
    aw_can_err_t ret;

    /** \brief ��ʼ��CAN */
    ret = aw_can_init(__CAN_CHN,                   /** \brief CANͨ���� */
                      AW_CAN_WORK_MODE_NORMAL,     /** \brief ����������ģʽ */
                      &__g_can_btr,                /** \brief ��ǰ������ */
                      &__g_can_app_callback_fun);  /** \brief �û��ص����� */

    AW_INFOF(("controller init...\n"));

    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    /** \brief ����CAN */
    ret = aw_can_start(__CAN_CHN);
    AW_INFOF(("controller start!\n"));

exit:
    return ret;
}

/**
 * \brief CAN�����������
 *
 * \param[in] p_arg �������
 *
 * \return ��
 */
static void* __can_send_task (void *p_arg)
{
    aw_can_err_t        ret;
    aw_spinlock_isr_t   lock;
    aw_can_bus_status_t bus_status;         /* CAN bus����״̬ */
    uint32_t            send_cnt      = 0;  /* ���͸��� */
    uint32_t            send_done_cnt = 0;  /* ������ɸ��� */
    aw_can_std_msg_t    msg[__CAN_MSG_CNT]; /* CAN��Ϣ������ */

    aw_spinlock_isr_init(&lock, 0);

    AW_FOREVER {

        AW_SEMB_TAKE(__g_can_msg_semb, 10);

        send_cnt = aw_rngbuf_nbytes(p_arg) / sizeof(aw_can_std_msg_t);
        if (send_cnt > 0) {
            aw_rngbuf_get(p_arg,
                          (char *)msg,
                          send_cnt * sizeof(aw_can_std_msg_t));

            /* ���ͱ�׼CAN���� */
            ret = aw_can_std_msgs_send(__CAN_CHN,
                                       msg,
                                       send_cnt,
                                       &send_done_cnt,
                                       __CAN_SEND_MSG_TIMEOUT);

            if (ret == AW_CAN_ERR_NONE && send_done_cnt == send_cnt) {
                AW_INFOF(("send done! sent:%d\n", send_done_cnt));
            } else {
                AW_INFOF(("send error! sent:%d\n", send_done_cnt));
            }
        }

        aw_spinlock_isr_take(&lock);
        bus_status = __g_can_bus_status;
        __g_can_bus_status = AW_CAN_BUS_STATUS_OK;
        aw_spinlock_isr_give(&lock);

        if (bus_status == AW_CAN_BUS_STATUS_OFF) {

            /* ֹͣCAN */
            aw_can_stop(__CAN_CHN);
            AW_INFOF(("controller stop!\n"));

            /* ����CAN */
            aw_can_start(__CAN_CHN);
            AW_INFOF(("controller start!\n"));
        }
    }

    return 0;
}

/**
 * \brief CAN�����������
 *
 * \param[in] p_arg �������
 *
 * \return ��
 */
static void* __can_recv_task (void *p_arg)
{
    uint32_t         i, j;
    aw_can_err_t     ret;
    uint32_t         recv_done_cnt = 0;  /* ������ɸ��� */
    aw_can_std_msg_t msg[__CAN_MSG_CNT]; /* CAN��Ϣ������ */

    AW_FOREVER {

        /* ���ձ�׼CAN���� */
        ret = aw_can_std_msgs_rcv(__CAN_CHN,
                                  msg,
                                  __CAN_MSG_CNT,
                                  &recv_done_cnt,
                                  __CAN_RECV_MSG_TIMEOUT);

        while ((ret == AW_CAN_ERR_NONE) && (recv_done_cnt--)) {

            AW_INFOF(("recv cnt:%d\n", recv_done_cnt + 1));
            if (msg[recv_done_cnt].can_msg.id == __CAN_MSG_ID_LED) {

                for (i = 0; i < msg[recv_done_cnt].can_msg.length; i++) {
                    for (j = 0; j < 8; j++) {
                        if ((0x01 << j) & msg[recv_done_cnt].msgbuff[i]) {
                            aw_led_on((i * 8) + j);
                        } else {
                            aw_led_off((i * 8) + j);
                        }
                    }
                }

                msg[recv_done_cnt].can_msg.id = __CAN_MSG_ID_LED_ACK;

                /* ���Ʊ���Ӧ�� */
                aw_rngbuf_put(p_arg,
                              (char *)&msg[recv_done_cnt],
                              sizeof(aw_can_std_msg_t));

                AW_SEMB_GIVE(__g_can_msg_semb);
            }
        }
    }

    return 0;
}

/**
 * \brief ����CAN��������CAN����
 *
 * \param[in] ��
 *
 * \return ��
 */
void demo_can_slave_entry (void)
{
    aw_can_err_t ret;

    aw_task_id_t    send_task;
    aw_task_id_t    recv_task;

    /* ��ʼ��CAN */
    ret = __can_startup();

    if (ret != AW_CAN_ERR_NONE) {
        AW_INFOF(("controller startup failed:%d!\n", ret));
        return;
    }

    /* ��ʼ��һ���������ź��� */
    AW_SEMB_INIT(__g_can_msg_semb, 0, AW_SEM_Q_FIFO);

    send_task = aw_task_create("can_send_task",
                               __CAN_SEND_TASK_PRIO,
                               __CAN_SEND_TASK_STACK_SIZE,
                               __can_send_task,
                               (void *)&__g_can_msg_rngbuf);
    if (send_task == NULL) {
        aw_kprintf("can send task create failed\r\n");
        return;
    }

    recv_task = aw_task_create("can_recv_task",
                               __CAN_RECV_TASK_PRIO,
                               __CAN_RECV_TASK_STACK_SIZE,
                               __can_recv_task,
                               (void *)&__g_can_msg_rngbuf);
    if (recv_task == NULL) {
        aw_kprintf("can recv task create failed\r\n");
        return;
    }

    aw_task_startup(send_task);
    aw_task_startup(recv_task);
}

/** [src_can_slave] */

/* end of file */
