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
 * \brief ����CAN��������������
 *
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      AW_COM_CONSOLE��
 *      ��Ӧƽ̨��CAN��UART�豸��
 *      ����Ҫ��������CAN�豸�����޸������е�CANͨ����ʹ��aw_prj_params.hͷ�ļ��ж�Ӧ��CAN�豸��
 *   2. ����CAN��������ŵ�CAN�շ��� RX=>RX TX=>TX��
 *      �շ����ٽӵ�USBCAN-II�豸�� H=>H L=>L, CAN������500K��
 *   3. �򿪴��ڵ����նˣ�������115200-8-N-1��
 *
 * - ʵ������
 *   1. ��cantest��λ�����ͱ��ģ� ���ڴ�ӡ����ؽ�����Ϣ��
 *
 * \par Դ����
 * \snippet demo_can_recv_simple.c src_can_recv_simple
 *
 * \internal
 * \par Modification History
 * - 1.00 18-09-18  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_can_recv_simple  ����CAN������(����)
 * \copydoc demo_can_recv_simple.c
 */

/** [src_can_recv_simple] */

#include "aworks.h"
#include "string.h"
#include "aw_can.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_spinlock.h"
#include "aw_prj_params.h"
#include "aw_demo_config.h"

#define __CAN_CHN                  DE_CAN_CH    /* CANͨ�� */
#define __CAN_RECV_MSG_TIMEOUT     10           /* CAN���Ľ��ճ�ʱʱ�� */
#define __CAN_MSG_CNT              128          /* CAN��Ϣ���� */

#define __CAN_TASK_STACK_SIZE      4096         /* CAN���Ľ��������ջ��С */
#define __CAN_TASK_PRIO            12           /* CAN���Ľ����������ȼ� */

/* �ú�Ϊ1ʱ ���յ�CAN֡�󽫴�ӡ���յ������� */
#define __CAN_TEST_MSG_PRINT       0

#undef AW_INFOF
#define AW_INFOF(x) aw_kprintf("CAN%d : ", __CAN_CHN); aw_kprintf x

/* CAN����״̬ */
static volatile aw_can_bus_status_t __g_can_bus_status = AW_CAN_BUS_STATUS_OK;

static aw_can_baud_param_t __g_can_btr = AW_CFG_CAN_BTR_500K;

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
static void* __can_simple_recv_task (void *p_arg)
{
    aw_can_err_t        ret;
    aw_can_bus_status_t bus_status;         /* CAN bus����״̬ */
    uint32_t            recv_fifo_size = 0; /* CAN FIFO��С */
    uint32_t            recv_cnt       = 0; /* һ�ν��յĸ��� */
    uint32_t            recv_done_cnt  = 0; /* ������ɸ��� */
#if (__CAN_TEST_MSG_PRINT == 1)
    uint32_t            i              = 0;
    uint32_t            j              = 0;
#endif

    aw_can_std_msg_t    msg[__CAN_MSG_CNT]; /* CAN��Ϣ������ */

    ret = __can_startup();
    if (ret != AW_CAN_ERR_NONE) {
        AW_INFOF(("controller startup failed:%d!\n", ret));
        goto exit;
    }

    /* ��ȡ����FIFO��С */
    recv_fifo_size = aw_can_rcv_fifo_size(__CAN_CHN);

    AW_INFOF(("receive fifo size %d messages.\n", recv_fifo_size));

    recv_cnt = min(recv_fifo_size, __CAN_MSG_CNT);

    AW_INFOF(("receive cnt %d messages.\n", recv_cnt));

    AW_FOREVER {

        /* ���ձ�׼CAN���� */
        ret = aw_can_std_msgs_rcv(__CAN_CHN,
                                  msg,
                                  recv_cnt,
                                  &recv_done_cnt,
                                  __CAN_RECV_MSG_TIMEOUT);

        if ((ret == AW_CAN_ERR_NONE) && (recv_done_cnt > 0)) {
            AW_INFOF(("recv cnt:%d\n", recv_done_cnt));

#if (__CAN_TEST_MSG_PRINT == 1)
            AW_INFOF(("recv data:\r\n"));
            for (i = 0; i < recv_done_cnt; i++) {
                for (j = 0; j < AW_CAN_HS_MAX_LENGTH; j++) {
                    aw_kprintf(" %02X", msg[i].msgbuff[j]);
                }
                aw_kprintf("\r\n");
            }
#endif
        }

        bus_status = __g_can_bus_status;
        __g_can_bus_status = AW_CAN_BUS_STATUS_OK;

        if (bus_status == AW_CAN_BUS_STATUS_OFF) {

            /* ֹͣCAN */
            aw_can_stop(__CAN_CHN);
            AW_INFOF(("controller stop!\n"));

            /* ����CAN */
            aw_can_start(__CAN_CHN);
            AW_INFOF(("controller start!\n"));
        }
    }

exit:
    AW_FOREVER {
        aw_mdelay(1000);
    }

    return 0;
}

/**
 * \brief ����CAN��������
 *
 * \param[in] ��
 *
 * \return ��
 */
void demo_can_simple_recv_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("can_simple_recv",
                         __CAN_TASK_PRIO,
                         __CAN_TASK_STACK_SIZE,
                         __can_simple_recv_task,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("can simple recv task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);
}

/** [src_can_recv_simple] */

/* end of file */
